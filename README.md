# Command & Conquer Generals / Zero Hour

This repository contains the source code for **Command & Conquer Generals** and its expansion pack **Command & Conquer Generals - Zero Hour**, released by Electronic Arts under the GPL v3 license. This preservation project supports modern platform builds for Windows, macOS, and Linux.

## About This Project

This source code release enables:
- **Modding & Customization**: Build your own mods for both Generals and Zero Hour
- **Steam Workshop Support**: Prepare mods for [C&C Generals Workshop](https://steamcommunity.com/workshop/browse/?appid=2229870) and [Zero Hour Workshop](https://steamcommunity.com/workshop/browse/?appid=2732960)
- **Cross-Platform Development**: Modern CMake build system with support for Windows, macOS (M1/M2 and Intel), and Linux
- **Preservation**: The original 2003 game code in a modernized development environment

> **Note**: To use any compiled binaries or mods, you must own the game. Purchase the [C&C Ultimate Collection](https://www.ea.com/en-gb/games/command-and-conquer/command-and-conquer-the-ultimate-collection) on [EA App](https://www.ea.com/en-gb/games/command-and-conquer/command-and-conquer-the-ultimate-collection/buy/pc) or [Steam](https://store.steampowered.com/bundle/39394/Command__Conquer_The_Ultimate_Collection/).

## Quick Start

### Prerequisites
- **CMake** 3.25 or later
- **C++ compiler** with C++17 support (MSVC 2015+, GCC 7+, Clang 5+)
- **SDL3** (macOS only, install via `brew install sdl3`)

### Build
```bash
# Configure the build (builds both Generals and Zero Hour by default)
cmake -B build

# Compile
cmake --build build

# For macOS with different architectures
cmake -B build -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
```

**Build Options:**
- `-DBUILD_ZEROHOUR=ON|OFF` — Include/exclude Zero Hour build (default: ON)
- `-DBUILD_TOOLS=ON|OFF` — Build editor tools: WorldBuilder, GUIEdit, etc. (default: OFF)

Output binaries are placed in `<build>/bin/` and organized by configuration and platform.

## Platform-Specific Guide

### Windows
The modern CMake build works with Visual Studio 2015 and later. To generate a Visual Studio solution:
```bash
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

**Legacy Support (Visual Studio 6.0):**
The original `rts.dsw` workspace still exists for building with VC6.0 (SP6 recommended for binary matching Generals patch 1.08 and Zero Hour patch 1.04). However, the modern CMake build is recommended.

**Note on UAC:** Executables with "version", "update", or "install" in their filenames require UAC elevation on Windows. The post-build events for `versionUpdate` and `buildVersionUpdate` projects may prompt for elevation.

### macOS
Requires SDL3 for the modern bootstrap targets:
```bash
brew install cmake sdl3
cmake -B build
cmake --build build
```

Universal binaries (Apple Silicon + Intel) are built by default. See [BUILDING_MACOS.md](docs/BUILDING_MACOS.md) for detailed macOS-specific instructions.

### Linux
CMake build with standard GNU toolchain:
```bash
cmake -B build
cmake --build build
```

## Project Structure

The repository contains **two separate but nearly identical code trees**:

```
├── Generals/           # Command & Conquer Generals (2003)
├── GeneralsMD/         # Command & Conquer Generals - Zero Hour (2004)
├── docs/               # Documentation
├── CMakeLists.txt      # Modern CMake build system
└── rts.dsw             # Legacy Visual Studio 6.0 workspace
```

**Important:** Cross-platform changes must be applied to **both** `Generals/` and `GeneralsMD/` directories independently, as they are separate code trees.

### Key Directories
- `Code/GameEngine/` — Core game engine
- `Code/Libraries/` — Third-party libraries and dependencies
- `Code/Tools/` — Utilities (WorldBuilder, GUIEdit, map editor, etc.)

## Dependencies

### For Modern CMake Builds
- **Windows**: Standard Win32 API, DirectX 9 SDK (legacy), MSVC runtime
- **macOS**: SDL3, Cocoa framework
- **Linux**: Standard POSIX libraries, SDL3

### For Editor Tools (`BUILD_TOOLS=ON`)
Tool-specific dependencies include:
- **3DSMax 4 SDK** — WorldBuilder (3D model viewing)
- **Bink SDK** — Video playback support
- Additional multimedia and graphics libraries (see build logs)

### Legacy/Optional Dependencies
If you wish to maintain compatibility with the original build or rebuild editor tools, you may need to source or replace:

| Library | Purpose | Expected Path | Notes |
|---------|---------|---|---|
| DirectX SDK 9.0+ | Graphics, input, audio | `\Code\Libraries\DirectX\` | Included in Windows SDK |
| STLport 4.5.3 | C++ Standard Library | `\Code\Libraries\STLport-4.5.3` | Requires [stlport.diff](stlport.diff) patch |
| 3DSMax 4 SDK | Model editor | `\Code\Libraries\Max4SDK\` | For WorldBuilder tool |
| RAD Miles 6 | Sound system | `\Code\Libraries\Source\WWVegas\Miles6\` | Legacy audio |
| RAD Bink | Video codec | `\Code\GameEngineDevice\Include\VideoDevice\Bink` | Video playback |
| GameSpy SDK | Online services | `\Code\Libraries\Source\GameSpy\` | Legacy multiplayer |
| ZLib 1.1.4 | Compression | `\Code\Libraries\Source\Compression\ZLib\` | Optional |
| LZH-Light 1.0 | LZH compression | `\Code\Libraries\Source\Compression\LZHCompress\` | Optional |

**STLport Note:** If building with VC6.0 or targeting exact binary compatibility, apply the provided [stlport.diff](stlport.diff) patch to STLport 4.5.3.

## Building with Legacy Tools

### Visual Studio 6.0 (VC6.0)
1. Open `rts.dsw` in Visual Studio C++ 6.0 (SP6 recommended)
2. Select **Build → Batch Build**
3. Click **"Rebuild All"**
4. Compiled binaries appear in `<game>/Run/` after building

### Modern Visual Studio (2015+)
If you must upgrade the legacy project:
1. Open `rts.dsw` in Visual Studio .NET 2003
2. Allow automatic conversion to a modern solution
3. Open the converted solution in MSVC 2015 or newer
4. **Warning**: Modern MSVC versions enforce stricter C++ standards. Significant code changes will be needed, especially for Win64 builds.

## Known Issues

1. **Windows UAC Elevation**: Projects named "versionUpdate" and "buildVersionUpdate" trigger UAC prompts. Rename output binaries to exclude "version", "update", or "install".

2. **macOS SDL3 Detection**: If SDL3 is not found, the build warns and uses a fallback stub. Install SDL3 with `brew install sdl3` or `brew install sdl3 --HEAD` for the latest version.

3. **Legacy Code & Modern Compilers**: The codebase uses C++ patterns from 2003. Modern compilers may warn about deprecated features, signed/unsigned comparisons, or uninitialized fields. Most are non-fatal.

4. **STLport Compatibility**: VC6.0 builds require the [stlport.diff](stlport.diff) patch to resolve compilation errors. Ensure you're using STLport 4.5.3.

## Building Editor Tools

To build map editors and tools (WorldBuilder, GUIEdit, etc.), add the build flag:
```bash
cmake -B build -DBUILD_TOOLS=ON
cmake --build build
```

Tool-specific requirements (3DSMax SDK, etc.) are noted in the build output if missing.

## Documentation

- [BUILDING_MACOS.md](docs/BUILDING_MACOS.md) — Detailed macOS build guide
- [LICENSE.md](LICENSE.md) — GPL v3 license with EA's additional terms
- [stlport.diff](stlport.diff) — STLport patch for legacy builds

## Contributing & Modding

### This Repository
This repository is **archived for preservation** and does **not accept**:
- Pull requests
- Issues or bug reports
- Feature requests

### Create Your Own Mods
To create mods or modifications:
1. **Fork this repository** to your GitHub account
2. Make your changes and customizations
3. Build your modified version
4. Share with the community or submit to [Steam Workshop](https://steamcommunity.com/workshop/browse/?appid=2229870)

For collaboration and community mods, create a fork or standalone repository. The modding community is vibrant — many successful mods and variants exist!

## Support

This repository is maintained for **preservation purposes only**. No active support is provided. However:
- The modding community is active on forums and Discord
- Check existing mods for solutions and examples
- Review the source code and documentation for technical details

## License

This repository and all source code is licensed under the **GNU General Public License v3 (GPL v3)**, with **additional terms applied by Electronic Arts**. 

See [LICENSE.md](LICENSE.md) for the complete license text and terms.

**TL;DR**: You may modify and distribute this code under GPL v3 terms, provided you include the license and preserve the original authorship. Commercial use is permitted under GPL v3 terms (you must make source available).

---

**Command & Conquer** is a trademark of Electronic Arts Inc. This project is an unofficial source code release for preservation and modding purposes.
