# CnC Generals Zero Hour — Apple Silicon & Intel macOS Fork Plan

> **Scope:** A complete engineering roadmap for porting the GPL-licensed C&C Generals / Zero Hour
> source code to run natively on macOS 13 Ventura or later, targeting both
> **Apple Silicon (ARM64)** and **Intel (x86-64)** architectures.

---

## Table of Contents

1. [Executive Summary](#1-executive-summary)
2. [Repository Audit — What Must Change](#2-repository-audit--what-must-change)
   - 2.1 [Build System](#21-build-system)
   - 2.2 [Entry Point & OS Windowing](#22-entry-point--os-windowing)
   - 2.3 [Graphics — Direct3D 8](#23-graphics--direct3d-8)
   - 2.4 [Audio — Miles Sound System & DirectSound](#24-audio--miles-sound-system--directsound)
   - 2.5 [Video — Bink SDK](#25-video--bink-sdk)
   - 2.6 [Input — DirectInput 8](#26-input--directinput-8)
   - 2.7 [Networking — WinSock & GameSpy](#27-networking--winsock--gamespy)
   - 2.8 [File System & Path Handling](#28-file-system--path-handling)
   - 2.9 [Threading & Synchronisation Primitives](#29-threading--synchronisation-primitives)
   - 2.10 [Windows Registry](#210-windows-registry)
   - 2.11 [Copy Protection (SafeDisc)](#211-copy-protection-safedisc)
   - 2.12 [x86 Inline Assembly & SIMD](#212-x86-inline-assembly--simd)
   - 2.13 [MSVC-Specific Types & Compiler Extensions](#213-msvc-specific-types--compiler-extensions)
   - 2.14 [Windows COM & OLE](#214-windows-com--ole)
   - 2.15 [Tools (WorldBuilder, GUIEdit, etc.)](#215-tools-worldbuilder-guiedit-etc)
3. [Architecture Strategy](#3-architecture-strategy)
4. [Phased Implementation Plan](#4-phased-implementation-plan)
   - Phase 0: Toolchain & Repository Setup
   - Phase 1: Platform Abstraction Layer (PAL)
   - Phase 2: Build System Migration (CMake)
   - Phase 3: Core Engine Compilation
   - Phase 4: Graphics Back-End
   - Phase 5: Audio & Video
   - Phase 6: Input
   - Phase 7: Networking
   - Phase 8: Tools
   - Phase 9: Quality, Testing & Packaging
5. [Dependency Matrix](#5-dependency-matrix)
6. [Apple Silicon Specific Considerations](#6-apple-silicon-specific-considerations)
7. [Risk Register](#7-risk-register)
8. [Recommended Technology Choices](#8-recommended-technology-choices)
9. [Folder Layout for the Fork](#9-folder-layout-for-the-fork)

---

## 1. Executive Summary

The C&C Generals / Zero Hour source code was developed in 2001–2003 exclusively for
Win32 using Visual Studio 6. Every subsystem — graphics, audio, video, input, networking,
file I/O, threading, and build — is tightly coupled to Windows APIs and proprietary SDKs
that do not exist on macOS.

Porting to macOS is a **significant multi-phase effort** involving:

| Category | Impact | Approach |
|---|---|---|
| Build System | 40+ `.dsp`/`.dsw` files | Rewrite in CMake |
| Graphics (D3D8) | ~500+ API call sites across WW3D2 + W3DDevice | Metal via SDL_GPU or MoltenVK |
| Audio (Miles + DSound) | Core audio device | CoreAudio / SDL_mixer |
| Video (Bink) | Cutscene playback | FFmpeg or libvpx |
| Input (DirectInput) | Keyboard + mouse + gamepad | SDL3 Input |
| Networking (WinSock + GameSpy) | Online multiplayer | POSIX sockets + open GameSpy replacement |
| File I/O + Registry | 100+ path sites | Cross-platform abstraction |
| Threading | Win32 sync objects | `std::thread` / `std::mutex` (C++17) |
| MSVC extensions | `__int64`, `__asm`, `__forceinline` | Standard C++17 equivalents |
| x86 assembly (RDTSC, SSE asm blocks) | ~35 inline asm blocks | Platform-specific or compiler intrinsics |

Estimated source change surface: **>60,000 lines** across ~600 files.

---

## 2. Repository Audit — What Must Change

### 2.1 Build System

**Current state:**
- Visual Studio 6 workspace files (`*.dsw`) and project files (`*.dsp`) throughout.
- No CMake, Xcode, Meson, or any cross-platform build description exists.
- Two separate workspace roots: `Generals/Code/RTS.dsw` and `GeneralsMD/Code/RTS.dsw`.
- Compiler directives are MSVC-only (`#pragma comment(lib, …)`, `#pragma warning(disable: …)`).
- Post-build events fail on modern Windows due to UAC and will not work at all on macOS.

**Files affected:**
- `Generals/Code/RTS.dsw`, `RTS.dsp`
- `GeneralsMD/Code/RTS.dsw`, `RTS.dsp`
- All `GameEngineDevice.dsp`, `GameEngine.dsp`, `Libraries/**/*.dsp`
- All `Tools/**/*.dsp` and `Tools/**/*.dsw`

**Required action:** Migrate entirely to **CMake 3.25+** with target-based dependency graph.

---

### 2.2 Entry Point & OS Windowing

**Current state:**
- `Generals/Code/Main/WinMain.cpp` — Win32 `WinMain(HINSTANCE, HINSTANCE, LPSTR, int)` entry point.
- `HWND ApplicationHWnd`, `HINSTANCE ApplicationHInstance` — global Win32 handles.
- `WndProc()` — Windows message loop with `WM_*` message constants.
- Window creation via `CreateWindowEx()`, `RegisterClassEx()`.
- Load screen uses `HBITMAP gLoadScreenBitmap`.
- Mutex via `CreateMutex()` with GUID string for single-instance enforcement.
- On-screen rendering bootstrapped through `Reset_D3D_Device()`.
- `#include <windows.h>`, `<ole2.h>`, `<dbt.h>`, `<eh.h>`, `<crtdbg.h>` at top of file.

**Files affected:**
- `Generals/Code/Main/WinMain.cpp`
- `Generals/Code/Main/WinMain.h`
- `GeneralsMD/Code/Main/WinMain.cpp`
- `Generals/Code/GameEngineDevice/Source/Win32Device/Common/Win32GameEngine.cpp`
- `Generals/Code/GameEngineDevice/Include/Win32Device/Common/Win32GameEngine.h`
- `Generals/Code/GameEngineDevice/Source/Win32Device/Common/Win32OSDisplay.cpp`

**Required action:**
- Replace `WinMain` with `int main()` that uses **SDL3** for window creation.
- Create a `MacGameEngine` class (mirroring `Win32GameEngine`) that wraps `SDL_Window`.
- Implement macOS application lifecycle with `NSApplicationDelegate` or SDL event loop.

---

### 2.3 Graphics — Direct3D 8

**Current state — most critical subsystem:**

The entire rendering pipeline is built on Direct3D 8 via the in-house **WW3D2** library:

| File Group | Location | Notes |
|---|---|---|
| `dx8wrapper.h/.cpp` | `Libraries/Source/WWVegas/WW3D2/` | D3D8 device wrapper — core class |
| `dx8renderer.h/.cpp` | WW3D2 | Main render dispatch |
| `dx8polygonrenderer.h/.cpp` | WW3D2 | Polygon submit |
| `dx8vertexbuffer.h/.cpp` | WW3D2 | GPU vertex buffers |
| `dx8indexbuffer.h/.cpp` | WW3D2 | GPU index buffers |
| `dx8texman.h/.cpp` | WW3D2 | Texture manager |
| `dx8caps.h/.cpp` | WW3D2 | Device capability query |
| `dx8fvf.h/.cpp` | WW3D2 | Flexible Vertex Format |
| `dx8webbrowser.h/.cpp` | WW3D2 | Browser overlay (W3DWebBrowser) |
| `surfaceclass.h/.cpp` | WW3D2 | D3D surface wrapper |
| `ddsfile.h/.cpp` | WW3D2 | DDS texture file loader |
| All `W3DDevice/**` files | `GameEngineDevice/Source/W3DDevice/` | Game-specific rendering |

**D3D8 interfaces used throughout:**
- `IDirect3D8`, `IDirect3DDevice8`, `IDirect3DSurface8`
- `IDirect3DTexture8`, `IDirect3DVertexBuffer8`, `IDirect3DIndexBuffer8`
- `IDirect3DSwapChain8`
- HLSL-era pixel/vertex shaders via `IDirect3DPixelShader8`, `IDirect3DVertexShader8`

**DirectDraw (legacy 2D):**
- `Libraries/Source/WWVegas/WWLib/ddraw.cpp` — DirectDraw surface abstraction still present.
- `dsurface.h/.cpp` — DirectDraw surface wrapper.

**Key game-level files using D3D directly:**
- `W3DShaderManager.cpp` — `QueryPerformanceCounter` + many D3D state calls
- `W3DDisplay.cpp` — Device creation + registry reads for D3D settings
- `W3DShroud.cpp`, `W3DWater.cpp`, `W3DWaterTracks.cpp` — Effects using render targets
- `HeightMap.cpp`, `TerrainTex.cpp` — Terrain texture management

**Required action:**
- Implement a **Render Abstraction Layer (RAL)** that mirrors the D3D8 interface surface.
- Back the RAL with **Metal** (preferred, lowest latency on Apple hardware) via SDL_GPU or **MoltenVK** (Vulkan-on-Metal, less effort to port from D3D concepts).
- DDS texture loading can be replaced with a cross-platform DDS decoder.
- All `#include <d3d8.h>` and `#include <d3dx8.h>` must be gated behind platform ifdefs and the RAL headers substituted.

---

### 2.4 Audio — Miles Sound System & DirectSound

**Current state:**

Two audio layers:

**Layer A — Miles Sound System 6 (MSS):**
- SDK located at `Libraries/Source/WWVegas/Miles6/` (`.gitignore` — files intentionally omitted).
- Used via `MilesAudioManager.cpp/.h`.
- Provides hardware 3D audio, MP3/streaming decode, DSP effects chain.
- License is proprietary — source is NOT included in this repo.

**Layer B — WPAudio (DirectSound wrapper):**
- `Libraries/Source/WPAudio/AUD_DSoundDriver.cpp` — `IDirectSound8` calls.
- `AUD_Streamer.cpp` — streaming thread using Win32 `CreateThread`.
- `AUD_Lock.cpp` — Windows CRITICAL_SECTION.
- `Libraries/Source/WWVegas/WWAudio/` — Higher-level audio framework.

**Files affected:**
- `GameEngineDevice/Include/MilesAudioDevice/MilesAudioManager.h`
- `GameEngineDevice/Source/MilesAudioDevice/MilesAudioManager.cpp`
- `Libraries/Source/WPAudio/AUD_DSoundDriver.cpp`
- `Libraries/Source/WPAudio/AUD_Streamer.cpp`
- `Libraries/Source/WPAudio/AUD_Lock.cpp`
- `Libraries/Source/WWVegas/WWAudio/**`
- `GameEngine/Include/Common/GameAudio.h`

**Required action:**
- Replace Miles with **SDL_mixer 3** + **libsndfile** or **OpenAL Soft** for 3D positional audio.
- Replace `AUD_DSoundDriver` with a CoreAudio or SDL Audio backend.
- Implement `MacAudioManager` conforming to the existing `GameAudio` abstract interface.

---

### 2.5 Video — Bink SDK

**Current state:**
- RAD Game Tools Bink SDK — proprietary, source NOT included.
- `GameEngineDevice/Include/VideoDevice/Bink/BinkVideoPlayer.h`
- `GameEngineDevice/Source/VideoDevice/Bink/BinkVideoPlayer.cpp`
- API surface: `BinkOpen()`, `BinkDoFrame()`, `BinkNextFrame()`, `BinkClose()`
- Render target overlaid on D3D8 surface.

**Required action:**
- Replace Bink with **FFmpeg** (`libavcodec`/`libavformat`) to decode the `.bik` files.
- The Bink file format (`BIKf`) has an open specification and FFmpeg supports it natively.
- Implement `MacVideoPlayer` wrapping FFmpeg decode → Metal texture upload.

---

### 2.6 Input — DirectInput 8

**Current state:**

Mouse:
- `Win32DIMouse.h/.cpp` — `IDirectInputDevice8` for raw mouse delta.
- `Win32Mouse.h/.cpp` — Win32 message-based mouse fallback.

Keyboard:
- `Win32DIKeyboard.h/.cpp` — `IDirectInputDevice8` for raw keyboard scan codes.
- `GameEngine/Include/GameClient/KeyDefs.h` — Key code constants map to DirectInput DIK_ constants.

**Files affected:**
- `GameEngineDevice/Include/Win32Device/GameClient/Win32DIMouse.h`
- `GameEngineDevice/Include/Win32Device/GameClient/Win32DIKeyboard.h`
- `GameEngineDevice/Source/Win32Device/GameClient/Win32DIMouse.cpp`
- `GameEngineDevice/Source/Win32Device/GameClient/Win32DIKeyboard.cpp`
- `GameEngineDevice/Source/Win32Device/GameClient/Win32Mouse.cpp`
- `GameEngine/Include/GameClient/KeyDefs.h`

**Required action:**
- Implement `MacMouse` and `MacKeyboard` classes backed by **SDL3 input events**.
- Remap `DIK_*` key constants to `SDL_Scancode` equivalents in `KeyDefs.h`.
- SDL3 provides relative mouse mode for camera panning — maps directly.

---

### 2.7 Networking — WinSock & GameSpy

**Current state:**

**Sockets:**
- `GameEngine/Source/GameNetwork/Network.cpp` — raw WinSock2 UDP/TCP.
- `GameEngine/Include/GameNetwork/udp.h` — has `#ifdef _WINDOWS` / `#elif _UNIX` guards, so
  POSIX socket path already partially exists.
- `Tools/mangler/wnet/tcp.cpp`, `udp.cpp` — Similar dual-path layout.
- `Win32BIGFileSystem.cpp` — `WSAStartup()` / `WSACleanup()`.

**GameSpy (proprietary, NOT included):**
- `Libraries/Source/GameSpy/` — `.gitignore` blocks entire directory.
- Used by: `GameSpyOverlay.cpp`, `GameSpyGP.cpp`, `GameSpyPersistentStorage.cpp`,
  `FirewallHelper.cpp`, lobby, ladder, buddy, chat, persistent storage threads.

**NAT/UPnP:**
- `GameEngine/Source/GameNetwork/NAT.cpp`
- `GameEngine/Include/GameNetwork/FirewallHelper.h`

**Files affected (~25 files):**
- All `GameEngine/Source/GameNetwork/**`
- All `GameEngine/Include/GameNetwork/**`
- `Tools/mangler/**`, `Tools/matchbot/**`
- `Tools/PATCHGET/DownloadManager.cpp`

**Required action:**
- The POSIX socket path already exists — activate it on macOS by defining `_UNIX`.
- Replace `WSAStartup`/`WSACleanup` with no-ops on macOS.
- Replace GameSpy SDK with **gamed** (Apple Game Center) for modern online, **or** an
  open-source GameSpy replacement server stack (e.g., OpenSpy / NATNEG2).
- Alternatively implement a minimal LAN-only mode first, add online later.

---

### 2.8 File System & Path Handling

**Current state:**

**Win32-specific file APIs:**
- `Win32LocalFileSystem.cpp` — `FindFirstFile()`, `FindNextFile()`, `FindClose()`.
- `Win32LocalFile.cpp` — `CreateFile()`, `ReadFile()`, `WriteFile()`, `CloseHandle()`.
- `Win32BIGFile.cpp` / `Win32BIGFileSystem.cpp` — BIG archive access.
- `Win32CDManager.cpp` — CD-ROM detection via `GetDriveType()`.

**Hardcoded Windows-style path strings:**
```cpp
// WinMain.cpp
const Char *g_strFile = "data\\Generals.str";
const Char *g_csfFile = "data\\%s\\Generals.csf";

// W3DFileSystem.cpp
"Software\\Microsoft\\Direct3d"  // registry key disguised as path

// Win32LocalFileSystem.cpp
FindFirstFile("*.*", &findData)  // directory enumeration
```

**Backslash paths found in >50 files** across tools and game code.

**Required action:**
- Create `MacLocalFileSystem` and `MacLocalFile` classes using POSIX `opendir()` / `dirent` / `open()` / `read()` / `write()`.
- Implement a `Path::normalize()` utility that converts `\\` → `/` on non-Windows platforms.
- Preserve BIG archive reader — only the OS I/O layer changes, not the BIG format parser.
- User data directory: use `~/Library/Application Support/CnCGenerals/` (via `NSSearchPathForDirectoriesInDomains` or `$HOME`).

---

### 2.9 Threading & Synchronisation Primitives

**Current state:**

| Win32 API | Usage location |
|---|---|
| `CreateThread()` | Audio streaming, GameSpy threads, network |
| `CreateMutex()` / `WaitForSingleObject()` | Audio lock (`AUD_Lock.cpp`), general sync |
| `CreateEvent()` / `SetEvent()` | Audio device sync |
| `CRITICAL_SECTION` | `Win32DIKeyboard.cpp`, `MilesAudioManager.cpp` |
| `InitializeCriticalSection()` | Multiple audio and network files |
| `Sleep()` | Many files |
| `__declspec(thread)` | Thread-local storage |

**Files affected:**
- `Libraries/Source/WPAudio/AUD_Lock.cpp`
- `Libraries/Source/WWVegas/WWLib/thread.cpp`
- `GameEngineDevice/Source/MilesAudioDevice/MilesAudioManager.cpp`
- `GameEngine/Source/GameNetwork/GameSpy/Thread/**`

**Required action:**
- Replace all Win32 threading with **C++17 `<thread>`, `<mutex>`, `<condition_variable>`**.
- Map `Sleep(ms)` → `std::this_thread::sleep_for(std::chrono::milliseconds(ms))`.
- `CRITICAL_SECTION` → `std::mutex`.
- `CreateEvent` / `SetEvent` / `WaitForSingleObject` → `std::condition_variable`.
- Thread-local: `__declspec(thread)` → `thread_local`.

---

### 2.10 Windows Registry

**Current state:**

The Windows Registry is used for:
- Installation path discovery.
- D3D adapter preference settings (`Software\Microsoft\Direct3d`).
- Game language selection.
- Online service (WOL) configuration.
- Patch version tracking.

**Key usages:**

| File | Registry Purpose |
|---|---|
| `Win32BIGFileSystem.cpp` | `GetStringFromGeneralsRegistry()` — finds game data root |
| `W3DDisplay.cpp` | D3D device selection preference |
| `WinMain.cpp` | `GetRegistryLanguage()` |
| `Tools/wolSetup/wolInit.cpp` | WOL server configuration |
| `Tools/PATCHGET/registry.cpp` | Patch version check |
| `Tools/Autorun/Locale_API.cpp` | Locale from registry |

**Required action:**
- Replace all registry reads with a **platform-neutral config system**.
- Use a **plist** (`~/Library/Preferences/com.ea.cncgenerals.plist`) or **INI/JSON** config file.
- Implement `PlatformConfig::get(key, default)` / `PlatformConfig::set(key, value)` abstraction.
- On first run, scan standard data paths (`~/Library/Application Support`, beside the binary) instead of registry-based path lookup.

---

### 2.11 Copy Protection (SafeDisc)

**Current state:**
- `Common/SafeDisc/CdaPfn.h` included in `WinMain.cpp`.
- `Tools/Launcher/Protect.cpp`, `Tools/Launcher/DatGen/DatGen.cpp`.
- `Win32CDManager.cpp` — CD-ROM drive enumeration.

**Required action:**
- SafeDisc is completely obsolete (EA has stripped it from modern releases, and it is incompatible with Windows 10+ as well as macOS).
- **Remove all SafeDisc-related code unconditionally.**
- Remove `Win32CDManager` entirely (no CD-ROM requirement on macOS).
- The GPL license grants the right to modify and redistribute; removing DRM is legally required for open-source distribution.

---

### 2.12 x86 Inline Assembly & SIMD

**Current state:**

Inline x86 `__asm` blocks found in **~35 locations**:

| File | Assembly Purpose |
|---|---|
| `Libraries/Source/WWVegas/WWMath/vp.cpp` | SSE matrix/vector transforms (ICL-gated) |
| `Libraries/Source/WWVegas/WWMath/wwmath.h` | Fast float→int conversions (`FISTP`) |
| `Libraries/Source/WWVegas/WWMath/matrix3d.cpp` | SSE matrix multiply |
| `Libraries/Source/WWVegas/WWLib/lcw.cpp` | LCW compression codec |
| `Libraries/Source/WWVegas/WWMath/quat.cpp` | Quaternion normalisation |
| `GameEngine/Include/Common/PerfTimer.h` | `RDTSC` instruction for high-res timer |
| `Tools/timingTest/timingTest.cpp` | RDTSC timing test |
| `Tools/WW3D/pluglib/wwmath.h` | `FISTP` float→int |

**Key issue — `vp.cpp`:** The SSE path is gated `#if defined(__ICL)` (Intel compiler), so GCC/Clang
will fall through to a scalar path. This code is safe to leave as-is but the scalar path will be used.

**`RDTSC` in `PerfTimer.h`:** Used in performance profiling macros that are disabled in Release
builds (`NO_PERF_TIMERS` is defined). This is low risk.

**`FISTP` float-to-int:** Used in `WWMath::Float_To_Int_Trunc()` and similar. Apple Silicon uses
IEEE 754 `FCVTZS` instruction equivalently. These should be replaced with `static_cast<int>()` or
`std::lrint()` which both compilers handle optimally.

**Required action:**
- Replace all `__asm { FISTP … }` blocks with `static_cast<int>(f)` / `std::lrint(f)`.
- Replace `RDTSC` with `clock_gettime(CLOCK_MONOTONIC_RAW, …)` or `mach_absolute_time()`.
- The SSE `__asm` blocks in `vp.cpp` are already ICL-only guarded; replace the outer `#if defined(__ICL)` with `#if 0` or remove the fast path entirely (scalar fallback exists).
- On Apple Silicon, NEON intrinsics (`arm_neon.h`) can be used for any SIMD hot paths if profiling shows it necessary — but this is an optimisation step, not a blocker.

---

### 2.13 MSVC-Specific Types & Compiler Extensions

**Current state:**

| MSVC Extension | Count | Files |
|---|---|---|
| `__int64` | ~20 | `W3DAssetManager.cpp`, `W3DShaderManager.cpp`, `ScriptEngine.cpp`, etc. |
| `__forceinline` | Many | `PerfTimer.h`, `GameMemory.h`, `always.h`, `wwmath.h` |
| `__cdecl`, `__stdcall`, `__fastcall` | Many | `Tools/mangler`, `Tools/WW3D`, `Libraries` |
| `__declspec(dllexport/dllimport)` | Tools | `DebugWindow`, `ParticleEditor` |
| `__declspec(noinline)` | A few | |
| `#pragma comment(lib, …)` | ~10 | Link directives baked into source |
| `#pragma warning(disable: …)` | ~60 | Throughout |
| `_MAX_PATH` | Many | Path buffer declarations |
| `_stricmp`, `_strcmpi` | Many | String comparison |
| `_snprintf`, `_vsnprintf` | Many | Formatted output |

**Required action:**
- `__int64` → `int64_t` (from `<cstdint>`).
- `__forceinline` → `inline __attribute__((always_inline))` on Clang/GCC; add a macro in a central `Platform.h`.
- `__cdecl` / `__stdcall` / `__fastcall` → remove on non-Windows (all calling conventions are ABI-defined by the platform).
- `__declspec(dllexport/dllimport)` → `__attribute__((visibility("default")))` / empty on macOS static build.
- `#pragma comment(lib, …)` → CMake `target_link_libraries()`.
- `#pragma warning(…)` → `#pragma GCC diagnostic` / `#pragma clang diagnostic` equivalents.
- `_MAX_PATH` → `PATH_MAX` (from `<limits.h>`).
- `_stricmp` → `strcasecmp` (POSIX).
- `_snprintf` → `snprintf` (already standard in C99).

---

### 2.14 Windows COM & OLE

**Current state:**
- `CoInitialize()` / `CoUninitialize()` in `WinMain.cpp` and input code.
- `IUnknown`, `QueryInterface`, `AddRef`, `Release` — only as part of DirectX interface
  usage (D3D8, DirectInput, DirectSound).
- `#include <ole2.h>` in `WinMain.cpp`.

**Required action:**
- Remove `CoInitialize()` / `CoUninitialize()` entirely.
- COM only appears as the D3D8/DirectInput/DirectSound interface protocol; once those
  subsystems are replaced by SDL/Metal, all COM usage disappears automatically.

---

### 2.15 Tools (WorldBuilder, GUIEdit, etc.)

**Current state:**
- All tools (`WorldBuilder`, `GUIEdit`, `ImagePacker`, `Babylon`, `ParticleEditor`, etc.)
  are MFC-based Win32 applications.
- Heavy use of MFC classes: `CWnd`, `CDialog`, `CView`, `CDocument`, `CFrameWnd`.
- Separate MFC dialog resource files (`.rc`).

**Required action (phased):**
- **Phase 1 — deprioritise tools.** Focus the initial port on the game runtime only.
- **Phase 2 — headless/CLI variants.** `ImagePacker`, `MapCacheBuilder`, `assetcull`,
  `textureCompress` can be ported as CLI tools with no UI.
- **Phase 3 — WorldBuilder.** Rewrite the UI layer using a cross-platform framework
  such as **Qt 6** or **Dear ImGui** embedded in an SDL3 window, preserving all game-logic
  code underneath.

---

## 3. Architecture Strategy

```
┌───────────────────────────────────────────────────────────────────┐
│                        Game Logic Layer                           │
│   (GameEngine/**  — platform-agnostic, minimal changes needed)    │
└─────────────────────────────┬─────────────────────────────────────┘
                              │
┌─────────────────────────────▼─────────────────────────────────────┐
│              Platform Abstraction Layer (PAL)                      │
│  ILocalFileSystem  IDisplay  IAudioManager  IKeyboard  IMouse      │
│  IVideoPlayer  ICriticalSection  IThread  PlatformConfig          │
└──┬─────────────┬──────────────┬──────────────┬────────────────────┘
   │             │              │              │
┌──▼──┐      ┌──▼──┐       ┌──▼──┐       ┌──▼──┐
│Win32│      │macOS│       │macOS│       │macOS│
│Impl │      │File │       │SDL3 │       │Metal│
│(old)│      │Impl │       │Audio│       │Gfx  │
└─────┘      └─────┘       └─────┘       └─────┘
```

- All existing `Win32*` implementations remain untouched (compilable on Windows).
- New `Mac*` implementations added alongside.
- Selected at CMake configure time via `PLATFORM_MACOS` / `PLATFORM_WIN32` defines.

---

## 4. Phased Implementation Plan

### Phase 0 — Toolchain & Repository Setup

| Task | Details |
|---|---|
| Fork repository | Create `macos-port` branch from `main` |
| Install toolchain | Xcode 15+, CMake 3.25+, Homebrew, ccache |
| Add `.clang-format` | Consistent C++17 formatting across new files |
| Add CI skeleton | GitHub Actions matrix: `macos-13` (Intel), `macos-14` (Apple Silicon) |
| Document toolchain | `docs/BUILDING_MACOS.md` |

**Deliverable:** CI pipelines run but compilation is expected to fail.

---

### Phase 1 — Platform Abstraction Layer (PAL)

Create `Code/Platform/` containing:

```
Code/Platform/
  Include/
    Platform.h            ← Compiler/arch macros, __forceinline, int64_t, etc.
    PlatformConfig.h      ← Key-value config store (replaces Registry)
    ILocalFileSystem.h    ← Abstract file system (already partially abstracted in engine)
    ILocalFile.h
    ICriticalSection.h    ← Mutex abstraction
    IThread.h             ← Thread abstraction
    PathUtil.h            ← Path separator normalisation
  Source/
    Mac/
      MacPlatformConfig.cpp   ← plist / INI backend
      MacLocalFileSystem.cpp  ← POSIX opendir/dirent
      MacLocalFile.cpp        ← POSIX open/read/write
      MacCriticalSection.cpp  ← std::mutex
      MacThread.cpp           ← std::thread
    Win32/
      (existing Win32* impls moved here)
```

**Deliverable:** `Code/Platform/` compiles on both platforms. All Win32 sync/file code
isolated behind interfaces.

---

### Phase 2 — Build System Migration (CMake)

Convert every `.dsp` to a `CMakeLists.txt` module:

```
CMakeLists.txt                   ← Top-level, selects platform
Generals/Code/
  CMakeLists.txt
  GameEngine/CMakeLists.txt
  GameEngineDevice/CMakeLists.txt
  Libraries/
    WWVegas/WW3D2/CMakeLists.txt
    WWVegas/WWMath/CMakeLists.txt
    WWVegas/WWLib/CMakeLists.txt
    WWVegas/WWAudio/CMakeLists.txt
    WPAudio/CMakeLists.txt
    Compression/CMakeLists.txt
  Main/CMakeLists.txt
  Tools/CMakeLists.txt          ← Gate tools behind BUILD_TOOLS option
```

Key CMake settings:
```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")  # Universal binary
  set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0")
endif()
```

**Deliverable:** `cmake -B build -DPLATFORM_MACOS=ON` produces a build graph (link failures expected).

---

### Phase 3 — Core Engine Compilation

Make `GameEngine/**` and `Libraries/WWVegas/WWMath/**` compile cleanly on Clang:

| Task | Details |
|---|---|
| Fix MSVC type extensions | `__int64` → `int64_t`, `__forceinline` macro, `__cdecl` removal |
| Fix inline assembly | Replace 35 `__asm` blocks (see §2.12) |
| Fix string functions | `_stricmp` → `strcasecmp`, `_snprintf` → `snprintf` |
| Fix pragma directives | `#pragma comment(lib,…)` → CMake; warning pragmas → clang equivalents |
| Validate STLport removal | Replace STLport 4.5.3 with system `<stl>` headers (libc++ on macOS) |
| Fix packed struct alignment | Audit all `#pragma pack` for correctness under `__attribute__((packed))` |
| Fix `_MAX_PATH` | → `PATH_MAX` |

**Deliverable:** `GameEngine.a` static library compiles. `WWMath.a` compiles. No Windows headers included.

---

### Phase 4 — Graphics Back-End

This is the largest single phase. Two sub-options:

#### Option A (Recommended) — SDL_GPU + Metal

SDL3 ships `SDL_GPU`, a modern cross-platform GPU API backed by Metal on macOS.

Steps:
1. Integrate SDL3 as a CMake dependency (`FetchContent` or Homebrew).
2. Create `Code/MacDevice/GraphicsDevice/` with Metal-backed implementations:
   - `MetalWrapper.h/.mm` — replaces `dx8wrapper`
   - `MetalRenderer.h/.mm` — replaces `dx8renderer`
   - `MetalVertexBuffer.h/.mm` — replaces `dx8vertexbuffer`
   - `MetalIndexBuffer.h/.mm` — replaces `dx8indexbuffer`
   - `MetalTexture.h/.mm` — replaces `dx8texman`
3. Implement the existing abstract rendering interfaces (`RenderInfoClass`, `VertexBufferClass`, etc.) backed by SDL_GPU objects.
4. Port `W3DDisplay`, `W3DShaderManager`, terrain, water, shadow, shroud rendering to call the new API.

#### Option B — MoltenVK (Vulkan on Metal)

Use Vulkan as the intermediate API, translated to Metal by MoltenVK:
- Pros: Wider precedent in open-source game ports; Vulkan closer conceptually to D3D.
- Cons: Heavier SDK; MoltenVK has edge-case compatibility issues.

#### Both options share:
- DDS texture loading: use `ddsktx` or `gli` — no Windows dependency.
- Shader conversion: HLSL → MSL (Metal Shading Language) via `SPIRV-Cross` or `DirectXShaderCompiler`.
- Frame timing: replace `QueryPerformanceCounter` with `mach_absolute_time()`.
- `timeGetTime()` → `SDL_GetTicks()`.

**Deliverable:** Game window opens and renders terrain with basic geometry.

---

### Phase 5 — Audio & Video

#### Audio

1. Implement `MacAudioManager` conforming to `GameAudio` interface.
2. Use **OpenAL Soft** for 3D positional audio (identical API surface to what Miles provided conceptually).
3. Use **libvorbis** or **libopus** for compressed audio if sound assets are re-encoded, otherwise raw PCM.
4. Remove `AUD_DSoundDriver.cpp` — replace with CoreAudio buffer queue or SDL3 Audio.
5. Port all threading in `AUD_Streamer.cpp` to `std::thread`.

#### Video

1. Link **FFmpeg** (`libavcodec`, `libavformat`, `libswscale`, `libswresample`).
2. Implement `MacVideoPlayer` that decodes `.bik` files via FFmpeg's Bink decoder.
3. Upload decoded YUV frames to a Metal texture, composite over the game scene.
4. Audio track from cutscene: demux and feed to the audio manager.

**Deliverable:** Main menu music plays. Cutscenes play. In-game sound effects work.

---

### Phase 6 — Input

1. Implement `MacKeyboard` backed by `SDL_PollEvent` / `SDL_KEYDOWN`.
2. Implement `MacMouse` backed by `SDL_MOUSEMOTION`, `SDL_MOUSEBUTTONDOWN`, `SDL_SetRelativeMouseMode`.
3. Create a key mapping table from `DIK_*` constants → `SDL_Scancode`.
4. Gamepad support via `SDL_GameController` (optional enhancement).
5. IME for text entry: `SDL_StartTextInput()` / `SDL_TEXTINPUT` events cover `IMEManager`.

**Deliverable:** Camera panning, unit selection, keyboard shortcuts all function.

---

### Phase 7 — Networking

#### Step 1 — LAN multiplayer (low risk)

- Activate the existing POSIX socket path in `udp.h` / `tcp.cpp` by defining `_UNIX`.
- Test LAN discovery and game sessions between two macOS machines.

#### Step 2 — Online (choose one path)

**Path A — OpenSpy:**
- Point DNS / hard-code IPs to an [OpenSpy](https://openspy.net) server.
- OpenSpy implements the GameSpy protocol so existing GameSpy SDK calls (once SDK is
  obtainable) work unchanged.

**Path B — Stub GameSpy SDK:**
- Write a minimal open-source `libgamespy` stub that implements only the calls the game
  makes (Peer, GP, Stats, Persistent Storage).
- Back it with a custom server or GameSpy open-source replacement.

**Path C — Steam / Apple Game Center:**
- Replace the GameSpy UI flow with a Steam overlay (Steamworks SDK) or Apple Game Center.
- Requires significant game-flow surgery.

**Required for all paths:**
- Replace `WSAStartup()` / `WSACleanup()` with macOS no-ops.
- Fix `IPEnumeration.cpp` — uses `GetAdaptersInfo()` (Win32); replace with `getifaddrs()` (POSIX).

**Deliverable:** LAN multiplayer functional. Online multiplayer at minimum via server stub.

---

### Phase 8 — Tools

Priority order:

1. **`MapCacheBuilder`** — CLI tool, trivial to port.
2. **`assetcull`**, **`CRCDiff`**, **`Compress`** — CLI, port readily.
3. **`textureCompress`** — replace with `nvtt` (NVIDIA Texture Tools, open-source, macOS support).
4. **`ImagePacker`** — port UI to Qt 6 or Dear ImGui.
5. **`WorldBuilder`** — largest tool. Port UI to Qt 6; game logic code ports as part of engine.
6. **`GUIEdit`**, **`ParticleEditor`** — port UI to Qt 6 or Dear ImGui.
7. **`Autorun`**, **`Launcher`**, **`wolSetup`** — delete or replace with macOS `.app` bundle + launch scripts.

---

### Phase 9 — Quality, Testing & Packaging

| Task | Details |
|---|---|
| Universal Binary | `CMAKE_OSX_ARCHITECTURES "arm64;x86_64"` — produces single `.app` |
| Hardened Runtime | Enable `com.apple.security.cs.allow-unsigned-executable-memory` entitlement |
| Notarisation | Sign and notarise `.app` with Apple Developer ID |
| Code signing | `codesign --deep --force --verify` |
| Game data path | Bundle `Resources/Data/` or prompt user to locate existing install |
| Save data migration | Detect Windows save path and offer migration |
| Regression test | Scripted game replay determinism tests |
| Performance profile | Instruments.app — GPU frame capture, CPU usage |
| CI matrix | GitHub Actions: `macos-13` (Intel), `macos-14` (ARM64), `windows-latest` |

---

## 5. Dependency Matrix

| Dependency | Version | Purpose | License | Source |
|---|---|---|---|---|
| SDL3 | 3.2+ | Window, input, audio, GPU | Zlib | [libsdl.org](https://libsdl.org) |
| Metal (system) | macOS 13+ | GPU rendering | Proprietary-free | Apple system framework |
| MoltenVK (alt) | 1.2+ | Vulkan → Metal | Apache 2.0 | Khronos / KhronosGroup |
| FFmpeg | 6.x | Bink video, audio decode | LGPL 2.1 | [ffmpeg.org](https://ffmpeg.org) |
| OpenAL Soft | 1.23+ | 3D positional audio | LGPL 2 | [openal-soft.org](https://openal-soft.org) |
| zlib | 1.3+ | Compression (already in repo) | Zlib | in-repo |
| SPIRV-Cross | latest | HLSL → MSL shader cross-compile | Apache 2.0 | Khronos |
| Qt 6 (tools only) | 6.6+ | WorldBuilder UI replacement | LGPL 3 / Commercial | [qt.io](https://qt.io) |
| CMake | 3.25+ | Build system | BSD | [cmake.org](https://cmake.org) |
| Xcode | 15+ | Compiler (Clang 16), Metal tools | Proprietary-free | Apple |

---

## 6. Apple Silicon Specific Considerations

### Architecture-Specific Items

| Item | x86-64 | ARM64 (Apple Silicon) | Action |
|---|---|---|---|
| `RDTSC` instruction | ✅ Available | ❌ Not available | Replace with `mach_absolute_time()` |
| `__asm FISTP` float→int | ✅ Available | ❌ Not available | Replace with `static_cast<int>()` |
| SSE intrinsics | ✅ Native | ❌ Translated (Rosetta) | Use scalar or NEON |
| NEON intrinsics | ❌ N/A | ✅ Native | Optional optimisation |
| Memory model | x86 TSO (strong) | ARM (weak) | Audit lock-free code carefully |
| Pointer size | 64-bit | 64-bit | No change needed |
| Stack alignment | 16-byte | 16-byte | No change needed |

### Rosetta 2 (Temporary)

Apple Silicon Macs can run x86-64 binaries transparently via Rosetta 2. This can be used to:
- Validate the x86-64 macOS port before the ARM64 port is complete.
- Run profiling tools on ARM64 to identify bottlenecks.

However, the goal is a **native Universal Binary** — not a Rosetta-only release.

### Metal Performance on Apple Silicon

- Apple Silicon has a **unified memory architecture** — CPU and GPU share the same DRAM.
- Metal on Apple Silicon: zero-copy texture uploads from CPU-prepared buffers using shared memory regions (`MTLStorageModeShared`).
- The W3D2 renderer does many small texture uploads — this architecture eliminates the PCIe copy penalty seen on discrete GPUs.
- Target: 60+ FPS on M1/M2 at native resolution.

### Thermal & Power Management

- Use `SDL_SetRenderVSync(renderer, 1)` to enable VSync and prevent unnecessary GPU work.
- The game's fixed game-logic tick rate (5Hz to 30Hz) is already well-suited to Apple Silicon power gating.

---

## 7. Risk Register

| Risk | Probability | Impact | Mitigation |
|---|---|---|---|
| GameSpy SDK unavailable | High | High | Use OpenSpy or stub implementation |
| Miles Sound System unavailable | High | High | Replace with OpenAL Soft — same conceptual API |
| Bink SDK not available for macOS | Medium | Medium | FFmpeg decodes `.bik` natively |
| D3D8 shader byte code (fixed-function emulation) | High | High | Re-author shaders in MSL / GLSL |
| STLport 4.5.3 incompatibility with libc++ | Medium | Medium | Replace with system STL; audit template usage |
| Binary save-game format breakage | Low | Medium | Little-endian on both platforms; likely compatible |
| Determinism in multiplayer replay | Medium | High | Audit all `float` usage for platform differences; use `int` fixed-point where needed |
| MFC tools non-portable | High | Low | Deprioritise; CLI-first approach |
| Legal risk (GPL compliance) | Low | High | Keep all GPL code in repo; clearly attribute removed proprietary SDKs |
| Performance regression on Intel Mac | Low | Medium | Profile early; Intel GPU Metal driver is mature |

---

## 8. Recommended Technology Choices

| Subsystem | Recommended Choice | Rationale |
|---|---|---|
| Build | CMake 3.25+ | Industry standard; works on all platforms |
| Window | SDL3 | Single API for window + input + audio |
| Graphics | SDL_GPU (Metal backend) | Native Metal; no MoltenVK layer; Apple-first |
| Audio | OpenAL Soft + SDL3 Audio | Spatial audio + streaming; well-maintained |
| Video | FFmpeg 6 | Best `.bik` decoder; widely used in game ports |
| Input | SDL3 | Covers keyboard, mouse, gamepad |
| Networking | POSIX sockets + OpenSpy | Minimal change to existing network code |
| Config (Registry) | plist via `NSUserDefaults` or libconfig | Native macOS config |
| Tools UI | Qt 6 (LGPL) | Cross-platform, mature, C++ native |
| Shader cross-compile | SPIRV-Cross | D3D HLSL → MSL pipeline |
| Packaging | Xcode + notarisation | `.app` bundle + `.dmg` for distribution |

---

## 9. Folder Layout for the Fork

```
CnC_Generals_Zero_Hour/
├── CMakeLists.txt                  ← Top-level (new)
├── cmake/
│   ├── FindSDL3.cmake
│   ├── FindFFmpeg.cmake
│   └── PlatformSettings.cmake
├── docs/
│   ├── BUILDING_MACOS.md           ← New
│   └── MACOS_PORT_PLAN.md          ← This document
├── Generals/
│   └── Code/
│       ├── CMakeLists.txt          ← New
│       ├── Platform/               ← NEW: platform abstraction layer
│       │   ├── Include/
│       │   │   ├── Platform.h
│       │   │   ├── PlatformConfig.h
│       │   │   └── PathUtil.h
│       │   └── Source/
│       │       ├── Mac/
│       │       │   ├── MacLocalFileSystem.cpp
│       │       │   ├── MacLocalFile.cpp
│       │       │   ├── MacPlatformConfig.mm
│       │       │   └── MacThread.cpp
│       │       └── Win32/          ← Existing Win32* files moved here
│       ├── MacDevice/              ← NEW: macOS device implementations
│       │   ├── Common/
│       │   │   ├── MacGameEngine.cpp
│       │   │   └── MacOSDisplay.mm
│       │   ├── GraphicsDevice/
│       │   │   ├── MetalWrapper.h
│       │   │   ├── MetalWrapper.mm
│       │   │   ├── MetalRenderer.mm
│       │   │   ├── MetalTexture.mm
│       │   │   ├── MetalVertexBuffer.mm
│       │   │   └── MetalIndexBuffer.mm
│       │   ├── AudioDevice/
│       │   │   └── MacAudioManager.cpp
│       │   ├── VideoDevice/
│       │   │   └── MacVideoPlayer.cpp
│       │   └── GameClient/
│       │       ├── MacKeyboard.cpp
│       │       └── MacMouse.cpp
│       ├── Main/
│       │   ├── WinMain.cpp         ← Kept for Windows
│       │   └── MacMain.cpp         ← New macOS entry point
│       ├── GameEngine/             ← Largely unchanged
│       ├── GameEngineDevice/       ← Win32Device kept; W3DDevice refactored
│       └── Libraries/             ← WWVegas, WWMath, etc. — minimal changes
├── GeneralsMD/                     ← Zero Hour; same structure as Generals/
├── .github/
│   └── workflows/
│       ├── build-macos.yml         ← New CI
│       └── build-windows.yml       ← Existing CI (if any)
├── MACOS_PORT_PLAN.md              ← This document
└── README.md                       ← Update with macOS build instructions
```

---

## Appendix A — File Count Summary

| Category | Approximate Files to Touch |
|---|---|
| Build system (new CMakeLists) | ~40 |
| Platform abstraction (new) | ~20 |
| macOS device implementations (new) | ~20 |
| Core engine MSVC-type fixes | ~150 |
| Inline assembly removal | ~10 |
| Path/file I/O changes | ~30 |
| Registry removal | ~15 |
| Graphics port (WW3D2 + W3DDevice) | ~80 |
| Audio port | ~20 |
| Video port | ~5 |
| Input port | ~10 |
| Networking socket fix | ~25 |
| **Total** | **~425 files** |

---

## Appendix B — Immediate Next Steps (Starter Checklist)

- [ ] Fork repository; create `macos-port` branch
- [ ] Add top-level `CMakeLists.txt` with compiler standard and platform detection
- [ ] Convert `GameEngine/CMakeLists.txt` and get it to compile without D3D headers
- [ ] Add `Platform.h` with `__int64` → `int64_t`, `__forceinline`, `PATH_MAX` macros
- [ ] Replace all `__asm FISTP` blocks in `wwmath.h` / `pluglib/wwmath.h`
- [ ] Replace `RDTSC` in `PerfTimer.h` with `mach_absolute_time()`
- [ ] Add GitHub Actions workflow for `macos-14` runner
- [ ] Create `MacMain.cpp` with `int main()` and SDL3 window creation stub
- [ ] Remove SafeDisc includes from `WinMain.cpp` / `MacMain.cpp`
- [ ] Open issue tracker for each phase above

---

*Last updated: 2026-05-27*
*Based on audit of commit history and 5,512 source files across `Generals/` and `GeneralsMD/`.*
