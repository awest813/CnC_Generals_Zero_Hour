/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
*/

#pragma once

#ifndef __PLATFORM_CONFIG_H__
#define __PLATFORM_CONFIG_H__

#include <memory>
#include <string>

namespace Platform {

class IPlatformConfig {
public:
    virtual ~IPlatformConfig() = default;
    virtual std::string Get(const std::string& key, const std::string& default_value) const = 0;
    virtual void Set(const std::string& key, const std::string& value) = 0;
    virtual void Save() = 0;
};

std::unique_ptr<IPlatformConfig> CreatePlatformConfig();

} // namespace Platform

#endif // __PLATFORM_CONFIG_H__
