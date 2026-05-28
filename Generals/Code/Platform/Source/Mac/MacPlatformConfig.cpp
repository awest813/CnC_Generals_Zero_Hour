/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
*/

#include "Platform/Include/PlatformConfig.h"
#include "Platform/Include/PathUtil.h"

#include <fstream>
#include <unordered_map>

namespace Platform {

class IniPlatformConfig final : public IPlatformConfig {
public:
    explicit IniPlatformConfig(std::string file_path) : m_file_path(Path::Normalize(file_path))
    {
        Load();
    }

    std::string Get(const std::string& key, const std::string& default_value) const override
    {
        auto it = m_values.find(key);
        return it != m_values.end() ? it->second : default_value;
    }

    void Set(const std::string& key, const std::string& value) override
    {
        m_values[key] = value;
    }

    void Save() override
    {
        std::ofstream out(m_file_path, std::ios::trunc);
        if (!out.is_open()) {
            return;
        }

        for (const auto& pair : m_values) {
            out << pair.first << '=' << pair.second << '
';
        }
    }

private:
    void Load()
    {
        std::ifstream in(m_file_path);
        if (!in.is_open()) {
            return;
        }

        std::string line;
        while (std::getline(in, line)) {
            const std::size_t separator = line.find('=');
            if (separator == std::string::npos) {
                continue;
            }
            m_values[line.substr(0, separator)] = line.substr(separator + 1);
        }
    }

    std::string m_file_path;
    std::unordered_map<std::string, std::string> m_values;
};

std::unique_ptr<IPlatformConfig> CreatePlatformConfig()
{
    return std::make_unique<IniPlatformConfig>(Path::UserDataDirectory() + "config.ini");
}

} // namespace Platform
