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

#ifndef __ILOCAL_FILE_SYSTEM_H__
#define __ILOCAL_FILE_SYSTEM_H__

#include <memory>
#include <string>
#include <vector>

namespace Platform {

class ILocalFileSystem {
public:
    virtual ~ILocalFileSystem() = default;
    virtual bool Exists(const std::string& path) const = 0;
    virtual bool IsDirectory(const std::string& path) const = 0;
    virtual bool IsRegularFile(const std::string& path) const = 0;
    virtual std::vector<std::string> ListDirectory(const std::string& path) const = 0;
};

std::unique_ptr<ILocalFileSystem> CreateLocalFileSystem();

} // namespace Platform

#endif // __ILOCAL_FILE_SYSTEM_H__
