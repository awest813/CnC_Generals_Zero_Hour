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

// FILE: Compat.h /////////////////////////////////////////////////////////////////
//
// Cross-platform compatibility shim.
// Include this header early (before BaseType.h / always.h) to paper over
// MSVC-specific types, calling conventions, CRT names, pragma handling,
// and inline-assembly gaps so the codebase can compile under Clang / GCC
// on macOS and Linux as well as under MSVC on Windows.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef __COMPAT_H__
#define __COMPAT_H__

// -------------------------------------------------------------------------
// 1. Compiler / platform detection
// -------------------------------------------------------------------------

#if defined(_MSC_VER)
#  define COMPILER_MSVC   1
#elif defined(__clang__)
#  define COMPILER_CLANG  1
#elif defined(__GNUC__)
#  define COMPILER_GCC    1
#endif

#if defined(_WIN32) || defined(_WIN64)
#  define PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
#  include <TargetConditionals.h>
#  define PLATFORM_APPLE   1
#  if TARGET_OS_MAC
#    define PLATFORM_MACOS 1
#  endif
#elif defined(__linux__)
#  define PLATFORM_LINUX   1
#endif

// Convenience: anything that is not Windows
#if !defined(PLATFORM_WINDOWS)
#  define PLATFORM_POSIX 1
#endif

// -------------------------------------------------------------------------
// 2. Integer types  (__int64 -> standard types)
// -------------------------------------------------------------------------

#if !defined(COMPILER_MSVC)
#  include <stdint.h>
#  include <stddef.h>

   // MSVC-ism used throughout the code
#  ifndef __int64
#    define __int64 long long
#  endif

   // Windows-style fixed-width types (used sparingly in WW code)
#  ifndef DWORD
     typedef uint32_t  DWORD;
#  endif
#  ifndef BYTE
     typedef uint8_t   BYTE;
#  endif
#  ifndef WORD
     typedef uint16_t  WORD;
#  endif
#  ifndef BOOL
     typedef int       BOOL;
#  endif
#  ifndef LONG
     typedef int32_t   LONG;
#  endif
#  ifndef ULONG
     typedef uint32_t  ULONG;
#  endif
#  ifndef UINT
     typedef unsigned int UINT;
#  endif
#  ifndef USHORT
     typedef unsigned short USHORT;
#  endif
#  ifndef UCHAR
     typedef unsigned char UCHAR;
#  endif
#  ifndef LPVOID
     typedef void*     LPVOID;
#  endif
#  ifndef LPCTSTR
     typedef const char* LPCTSTR;
#  endif
#  ifndef LPCSTR
     typedef const char* LPCSTR;
#  endif
#  ifndef LPSTR
     typedef char*     LPSTR;
#  endif
#  ifndef HRESULT
     typedef long      HRESULT;
#  endif

   // Win32 constants
#  ifndef TRUE
#    define TRUE  1
#  endif
#  ifndef FALSE
#    define FALSE 0
#  endif
#  ifndef MAX_PATH
#    define MAX_PATH 260
#  endif
#  ifndef _MAX_PATH
#    define _MAX_PATH MAX_PATH
#  endif
#  ifndef _MAX_FNAME
#    define _MAX_FNAME 256
#  endif
#  ifndef _MAX_EXT
#    define _MAX_EXT 256
#  endif
#  ifndef _MAX_DRIVE
#    define _MAX_DRIVE 3
#  endif
#  ifndef _MAX_DIR
#    define _MAX_DIR 256
#  endif

#endif // !COMPILER_MSVC

// -------------------------------------------------------------------------
// 3. Calling-convention macros  (no-ops outside MSVC / Windows)
// -------------------------------------------------------------------------

#if !defined(PLATFORM_WINDOWS)
#  ifndef __cdecl
#    define __cdecl
#  endif
#  ifndef __stdcall
#    define __stdcall
#  endif
#  ifndef __fastcall
#    define __fastcall
#  endif
#  ifndef WINAPI
#    define WINAPI
#  endif
#  ifndef CALLBACK
#    define CALLBACK
#  endif
#  ifndef __declspec
#    define __declspec(x)
#  endif
#endif

// -------------------------------------------------------------------------
// 4. Force-inline
// -------------------------------------------------------------------------

#if defined(COMPILER_MSVC)
#  define FORCE_INLINE __forceinline
#elif defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#  define FORCE_INLINE inline __attribute__((always_inline))
#else
#  define FORCE_INLINE inline
#endif

// Map the MSVC keyword so existing code compiles unchanged.
#if !defined(COMPILER_MSVC)
#  ifndef __forceinline
#    define __forceinline FORCE_INLINE
#  endif
#endif

// -------------------------------------------------------------------------
// 5. Pragma helpers  (MSVC #pragma warning → no-op on other compilers)
// -------------------------------------------------------------------------

// Clang and GCC do not understand MSVC warning numbers, so we silence
// the unknown-pragma diagnostic around blocks of #pragma warning(…).
// Individual files can continue to use #pragma warning as-is; the
// compiler will simply ignore unrecognised pragmas after this.
#if !defined(COMPILER_MSVC)
#  if defined(COMPILER_CLANG)
#    pragma clang diagnostic ignored "-Wunknown-pragmas"
#    pragma clang diagnostic ignored "-Wpragma-pack"
#  elif defined(COMPILER_GCC)
#    pragma GCC diagnostic ignored "-Wunknown-pragmas"
#  endif
#endif

// -------------------------------------------------------------------------
// 6. CRT name mapping  (MSVC _foo → POSIX foo)
// -------------------------------------------------------------------------

#if !defined(COMPILER_MSVC)
#  include <strings.h>
#  include <string.h>
#  include <stdio.h>
#  include <stdlib.h>

#  ifndef _stricmp
#    define _stricmp    strcasecmp
#  endif
#  ifndef _strnicmp
#    define _strnicmp   strncasecmp
#  endif
#  ifndef _wcsicmp
#    define _wcsicmp    wcscasecmp
#  endif
#  ifndef _wcsnicmp
#    define _wcsnicmp   wcsncasecmp
#  endif
#  ifndef stricmp
#    define stricmp     strcasecmp
#  endif
#  ifndef strnicmp
#    define strnicmp    strncasecmp
#  endif
#  ifndef _snprintf
#    define _snprintf   snprintf
#  endif
#  ifndef _vsnprintf
#    define _vsnprintf  vsnprintf
#  endif
#  ifndef _strdup
#    define _strdup     strdup
#  endif
#  ifndef _strlwr
     // POSIX has no direct equivalent; provide inline helper
     static inline char* _strlwr(char* s)
     {
         for (char* p = s; *p; ++p)
             *p = (char)tolower((unsigned char)*p);
         return s;
     }
#  endif
#  ifndef _strupr
     static inline char* _strupr(char* s)
     {
         for (char* p = s; *p; ++p)
             *p = (char)toupper((unsigned char)*p);
         return s;
     }
#  endif
#  ifndef _itoa
     static inline char* _itoa(int value, char* str, int base)
     {
         if (base == 10) { sprintf(str, "%d", value); }
         else if (base == 16) { sprintf(str, "%x", value); }
         else if (base == 8) { sprintf(str, "%o", value); }
         else { str[0] = '\0'; }
         return str;
     }
#  endif
#  ifndef _atoi64
#    define _atoi64 atoll
#  endif
#  ifndef _getcwd
#    define _getcwd getcwd
#  endif
#  ifndef _chdir
#    define _chdir chdir
#  endif
#  ifndef _mkdir
     // POSIX mkdir takes a mode; MSVC _mkdir does not
#    include <sys/stat.h>
#    define _mkdir(d) mkdir((d), 0755)
#  endif
#  ifndef _unlink
#    define _unlink unlink
#  endif
#  ifndef _access
#    include <unistd.h>
#    define _access access
#  endif
#  ifndef _fileno
#    define _fileno fileno
#  endif
#  ifndef _isnan
#    define _isnan isnan
#  endif
#  ifndef _finite
#    define _finite isfinite
#  endif
#  ifndef _copysign
#    define _copysign copysign
#  endif

#  include <ctype.h>    // for tolower / toupper used above
#endif // !COMPILER_MSVC

// -------------------------------------------------------------------------
// 7. Inline-assembly fallbacks
// -------------------------------------------------------------------------
// The codebase contains MSVC __asm blocks (x86 FPU, RDTSC, etc.).
// On non-MSVC compilers we rely on the C/C++ fallbacks that already exist
// in wwmath.h and related files (guarded by _MSC_VER && _M_IX86).
//
// For RDTSC specifically (used by PerfTimer), provide a portable helper:

#if !defined(COMPILER_MSVC)
#  if defined(__x86_64__) || defined(__i386__)
     static inline void __rdtsc_portable(long long* t)
     {
         unsigned int lo, hi;
         __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
         *t = ((long long)hi << 32) | lo;
     }
#  elif defined(__aarch64__)
     // ARM64: use the generic timer counter
     static inline void __rdtsc_portable(long long* t)
     {
         unsigned long long val;
         __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(val));
         *t = (long long)val;
     }
#  else
     // Fallback: use clock_gettime
#    include <time.h>
     static inline void __rdtsc_portable(long long* t)
     {
         struct timespec ts;
         clock_gettime(CLOCK_MONOTONIC, &ts);
         *t = (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
     }
#  endif
#endif

// -------------------------------------------------------------------------
// 8. Miscellaneous MSVC-isms
// -------------------------------------------------------------------------

// MSVC uses #pragma once universally; Clang/GCC support it too, so no
// action needed — but guarded compilation of _MSC_VER >= 1000 blocks
// should use the macro below instead:
#if !defined(_MSC_VER)
#  ifndef _MSC_VER
     // Ensure _MSC_VER is never accidentally tested as defined on non-MSVC.
     // (Nothing to do; just a documentation note.)
#  endif
#endif

// OutputDebugString – used in a few debug paths
#if !defined(PLATFORM_WINDOWS)
#  include <stdio.h>
#  ifndef OutputDebugString
#    define OutputDebugString(msg) fputs((msg), stderr)
#  endif
#  ifndef OutputDebugStringA
#    define OutputDebugStringA(msg) fputs((msg), stderr)
#  endif
#endif

// Sleep
#if !defined(PLATFORM_WINDOWS)
#  include <unistd.h>
#  ifndef Sleep
#    define Sleep(ms) usleep((ms) * 1000)
#  endif
#endif

// GetTickCount – millisecond timer
#if !defined(PLATFORM_WINDOWS)
#  include <time.h>
   static inline unsigned long GetTickCount(void)
   {
       struct timespec ts;
       clock_gettime(CLOCK_MONOTONIC, &ts);
       return (unsigned long)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
   }
#endif

#endif // __COMPAT_H__
