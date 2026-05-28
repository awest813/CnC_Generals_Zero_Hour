/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
*/

#include "Platform/Include/ICriticalSection.h"

#include <mutex>

namespace Platform {

class MacCriticalSection final : public ICriticalSection {
public:
    void Lock() override { m_mutex.lock(); }
    void Unlock() override { m_mutex.unlock(); }

private:
    std::mutex m_mutex;
};

std::unique_ptr<ICriticalSection> CreateCriticalSection()
{
    return std::make_unique<MacCriticalSection>();
}

} // namespace Platform
