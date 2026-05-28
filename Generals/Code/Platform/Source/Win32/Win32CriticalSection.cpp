/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
*/

#include "Platform/Include/ICriticalSection.h"

#include <windows.h>

namespace Platform {

class Win32CriticalSection final : public ICriticalSection {
public:
    Win32CriticalSection() { ::InitializeCriticalSection(&m_section); }
    ~Win32CriticalSection() override { ::DeleteCriticalSection(&m_section); }

    void Lock() override { ::EnterCriticalSection(&m_section); }
    void Unlock() override { ::LeaveCriticalSection(&m_section); }

private:
    CRITICAL_SECTION m_section;
};

std::unique_ptr<ICriticalSection> CreateCriticalSection()
{
    return std::make_unique<Win32CriticalSection>();
}

} // namespace Platform
