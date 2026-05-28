/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
*/

#pragma once

#ifndef __ICRITICALSECTION_H__
#define __ICRITICALSECTION_H__

#include <memory>

namespace Platform {

class ICriticalSection {
public:
    virtual ~ICriticalSection() = default;
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
};

std::unique_ptr<ICriticalSection> CreateCriticalSection();

class ScopedCriticalSection {
public:
    explicit ScopedCriticalSection(ICriticalSection& section) : m_section(section) { m_section.Lock(); }
    ~ScopedCriticalSection() { m_section.Unlock(); }

    ScopedCriticalSection(const ScopedCriticalSection&) = delete;
    ScopedCriticalSection& operator=(const ScopedCriticalSection&) = delete;

private:
    ICriticalSection& m_section;
};

} // namespace Platform

#endif // __ICRITICALSECTION_H__
