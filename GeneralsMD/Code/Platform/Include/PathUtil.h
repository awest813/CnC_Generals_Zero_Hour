/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
*/

#pragma once

#ifndef __PATH_UTIL_H__
#define __PATH_UTIL_H__

#include <algorithm>
#include <cstdlib>
#include <string>

namespace Platform {
namespace Path {

inline std::string Normalize(const std::string& path)
{
#if defined(_WIN32)
    return path;
#else
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\', '/');
    return normalized;
#endif
}

inline int MaxPathLength()
{
#ifdef _MAX_PATH
    return _MAX_PATH;
#else
    return 4096;
#endif
}

inline std::string UserDataDirectory()
{
    const char* home = std::getenv("HOME");
    std::string base = home ? home : "";

#if defined(__APPLE__)
    return Normalize(base + "/Library/Application Support/CnCGenerals/");
#elif defined(_WIN32)
    return ".\";
#else
    return Normalize(base + "/.local/share/CnCGenerals/");
#endif
}

} // namespace Path
} // namespace Platform

#endif // __PATH_UTIL_H__
