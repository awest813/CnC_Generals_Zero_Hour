/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

////////////////////////////////////////////////////////////////////////////////
//
//  Platform.h — Cross-platform compatibility macros for the macOS port.
//
//  This header provides portable replacements for MSVC-specific types,
//  intrinsics, and compiler extensions so that the codebase compiles
//  under Clang/GCC as well as MSVC.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

// ── Compiler Detection ───────────────────────────────────────────────────────

#if defined(_MSC_VER)
    #define PLATFORM_COMPILER_MSVC 1
#elif defined(__clang__)
    #define PLATFORM_COMPILER_CLANG 1
#elif defined(__GNUC__)
    #define PLATFORM_COMPILER_GCC 1
#endif

// ── Architecture Detection ───────────────────────────────────────────────────

#if defined(_M_IX86) || defined(__i386__)
    #define PLATFORM_ARCH_X86 1
#elif defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__)
    #define PLATFORM_ARCH_X64 1
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define PLATFORM_ARCH_ARM64 1
#endif

// ── Integer Types ────────────────────────────────────────────────────────────
//
// MSVC uses __int64 / unsigned __int64.  Standard C++ uses <cstdint>.
//

#if !defined(PLATFORM_COMPILER_MSVC)
    #include <cstdint>

    // Only define these if not already typedef'd elsewhere (e.g. BaseType.h).
    // Guard with a macro so BaseType.h can skip its own __int64 typedefs.
    #ifndef PLATFORM_INT64_DEFINED
    #define PLATFORM_INT64_DEFINED
        typedef int64_t  Int64;
        typedef uint64_t UnsignedInt64;
    #endif

    // Provide __int64 as a macro for legacy code that uses the MSVC spelling.
    #ifndef __int64
    #define __int64 long long
    #endif
#endif

// ── Inline Forcing ───────────────────────────────────────────────────────────
//
// MSVC: __forceinline
// Clang/GCC: inline with always_inline attribute
//

#if !defined(PLATFORM_COMPILER_MSVC)
    #ifdef __forceinline
        #undef __forceinline
    #endif
    #define __forceinline inline __attribute__((always_inline))
#endif

// ── Calling Conventions ──────────────────────────────────────────────────────
//
// __cdecl, __stdcall, __fastcall are Windows-only.  On other platforms the ABI
// is defined by the OS/CPU, so these expand to nothing.
//

#if !defined(_WIN32)
    #ifndef __cdecl
    #define __cdecl
    #endif
    #ifndef __stdcall
    #define __stdcall
    #endif
    #ifndef __fastcall
    #define __fastcall
    #endif
#endif

// ── DLL Import/Export ────────────────────────────────────────────────────────
//
// On non-Windows platforms shared library symbol visibility is controlled
// with __attribute__((visibility("default"))).  For the initial port we
// build everything statically, so these are empty.
//

#if !defined(_WIN32)
    #ifndef __declspec
    #define __declspec(x)
    #endif
#endif

// ── Path Limits ──────────────────────────────────────────────────────────────
//
// Windows: _MAX_PATH (260)
// POSIX:   PATH_MAX (from <limits.h>)
//

#if !defined(_WIN32)
    #include <limits.h>
    #ifndef _MAX_PATH
        #ifdef PATH_MAX
            #define _MAX_PATH PATH_MAX
        #else
            #define _MAX_PATH 4096
        #endif
    #endif
    #ifndef MAX_PATH
        #define MAX_PATH _MAX_PATH
    #endif
#endif

// ── String Functions ─────────────────────────────────────────────────────────
//
// MSVC prefixes many POSIX string functions with an underscore.
//

#if !defined(_WIN32)
    #include <strings.h>  // strcasecmp
    #include <stdio.h>    // snprintf

    #ifndef _stricmp
    #define _stricmp  strcasecmp
    #endif
    #ifndef _strnicmp
    #define _strnicmp strncasecmp
    #endif
    #ifndef _strcmpi
    #define _strcmpi  strcasecmp
    #endif
    #ifndef _snprintf
    #define _snprintf snprintf
    #endif
    #ifndef _vsnprintf
    #define _vsnprintf vsnprintf
    #endif
    #ifndef stricmp
    #define stricmp   strcasecmp
    #endif
    #ifndef strnicmp
    #define strnicmp  strncasecmp
    #endif
#endif

// ── High-Resolution Timer ────────────────────────────────────────────────────
//
// Replaces RDTSC / QueryPerformanceCounter on non-Windows platforms.
//

#if !defined(_WIN32)
    #if defined(__APPLE__)
        #include <mach/mach_time.h>
        static inline uint64_t Platform_GetTicks()
        {
            return mach_absolute_time();
        }
    #else
        #include <time.h>
        static inline uint64_t Platform_GetTicks()
        {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
            return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
        }
    #endif
#endif

// ── MSVC Pragma Compatibility ────────────────────────────────────────────────
//
// #pragma warning(...) is MSVC-only.  Clang supports a limited subset via
// #pragma clang diagnostic, but for simplicity we suppress them at the
// compiler command-line level (CMake) and ignore the #pragma warning
// directives via this guard:
//
//   #if defined(_MSC_VER)
//       #pragma warning(...)
//   #endif
//
// This header does NOT globally redefine #pragma because that is not
// possible in standard C++.  Instead, individual source files will be
// updated to gate their #pragma warning lines.

// ── Naked Functions ──────────────────────────────────────────────────────────
//
// MSVC __declspec(naked) is not supported on ARM or on Clang for x86-64.
// Naked asm functions must be rewritten as standard C++ on non-MSVC.
//

// (No macro needed — the affected code paths are gated with
//  #if defined(_MSC_VER) && defined(_M_IX86) already.)

#endif /* __PLATFORM_H__ */
