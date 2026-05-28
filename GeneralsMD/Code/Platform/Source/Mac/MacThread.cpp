/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
*/

#include "Platform/Include/IThread.h"

#include <thread>
#include <utility>

namespace Platform {

class MacThread final : public IThread {
public:
    explicit MacThread(ThreadEntryPoint entry_point) : m_thread(std::move(entry_point)) {}

    void Join() override
    {
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    bool Joinable() const override { return m_thread.joinable(); }

private:
    std::thread m_thread;
};

std::unique_ptr<IThread> CreateThread(ThreadEntryPoint entry_point)
{
    return std::make_unique<MacThread>(std::move(entry_point));
}

} // namespace Platform
