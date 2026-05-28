# Building C&C Generals / Zero Hour on macOS

> **Status:** Early port — the build system is in place but compilation of the full
> engine is not yet possible. This document tracks requirements and progress.

## Requirements

| Tool | Minimum Version | Install |
|------|----------------|---------|
| macOS | 13.0 (Ventura) | — |
| Xcode Command Line Tools | 15+ | `xcode-select --install` |
| CMake | 3.25+ | `brew install cmake` |
| SDL3 | 3.2+ | `brew install sdl3` |

## Quick Start

```bash
# Configure (from repository root)
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build --parallel $(sysctl -n hw.ncpu)
```

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_ZEROHOUR` | `ON` | Build Zero Hour (GeneralsMD) alongside Generals |
| `BUILD_TOOLS` | `OFF` | Build editor tools (WorldBuilder, etc.) |
| `CMAKE_OSX_ARCHITECTURES` | `arm64;x86_64` | Target architectures (Universal Binary) |
| `CMAKE_OSX_DEPLOYMENT_TARGET` | `13.0` | Minimum macOS version |

## Port Progress

See [MACOS_PORT_PLAN.md](../MACOS_PORT_PLAN.md) for the full engineering roadmap.

### Completed

- [x] Top-level CMake build system with platform detection
- [x] `Platform.h` cross-platform compatibility header
- [x] `always.h` updated for Clang/GCC compatibility
- [x] `BaseType.h` — `__int64` → `int64_t`, asm blocks gated behind `_MSC_VER`
- [x] `PerfTimer.h` — RDTSC replaced with `mach_absolute_time()` on macOS
- [x] `wwmath.h` — asm blocks already had cross-platform fallbacks (verified)
- [x] SafeDisc includes gated behind `_WIN32`
- [x] Windows-only system headers gated behind `_WIN32`
- [x] SDL3-backed `MacMain.cpp` bootstrap targets for both game trees
- [x] GitHub Actions CI for macOS

### In Progress

- [ ] CMake sub-projects for GameEngine, Libraries
- [ ] Full engine compilation on Clang
- [ ] Platform Abstraction Layer (file I/O, threading, config)

## Architecture

The port uses a Platform Abstraction Layer (PAL) approach:

```
Code/Platform/Include/Platform.h    ← Cross-platform macros
Code/Platform/Include/...           ← PAL interfaces (future)
Code/Platform/Source/Mac/...        ← macOS implementations (future)
```

Existing Win32 implementations remain untouched. New macOS implementations
are added alongside, selected at CMake configure time.
