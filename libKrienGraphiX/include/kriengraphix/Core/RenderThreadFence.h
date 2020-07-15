#pragma once

#include <atomic>

namespace kgx
{
    class RenderThreadFence final
    {
    public:
        RenderThreadFence();

    private:
        std::atomic<bool> m_done;
    };
}
