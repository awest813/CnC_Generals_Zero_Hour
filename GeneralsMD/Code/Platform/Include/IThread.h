/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
*/

#pragma once

#ifndef __ITHREAD_H__
#define __ITHREAD_H__

#include <functional>
#include <memory>

namespace Platform {

using ThreadEntryPoint = std::function<void()>;

class IThread {
public:
    virtual ~IThread() = default;
    virtual void Join() = 0;
    virtual bool Joinable() const = 0;
};

std::unique_ptr<IThread> CreateThread(ThreadEntryPoint entry_point);

} // namespace Platform

#endif // __ITHREAD_H__
