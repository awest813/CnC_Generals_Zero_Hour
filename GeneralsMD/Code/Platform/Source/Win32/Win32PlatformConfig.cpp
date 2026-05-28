/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
*/

#include "Platform/Include/PlatformConfig.h"

#include <windows.h>

namespace Platform {

class Win32PlatformConfig final : public IPlatformConfig {
public:
    std::string Get(const std::string& key, const std::string& default_value) const override
    {
        char buffer[1024] = {0};
        const DWORD read = ::GetPrivateProfileStringA("CnCGenerals", key.c_str(), default_value.c_str(), buffer, static_cast<DWORD>(sizeof(buffer)), ".\\generals.ini");
        return read > 0 ? std::string(buffer) : default_value;
    }

    void Set(const std::string& key, const std::string& value) override
    {
        ::WritePrivateProfileStringA("CnCGenerals", key.c_str(), value.c_str(), ".\\generals.ini");
    }

    void Save() override {}
};

std::unique_ptr<IPlatformConfig> CreatePlatformConfig()
{
    return std::make_unique<Win32PlatformConfig>();
}

} // namespace Platform
