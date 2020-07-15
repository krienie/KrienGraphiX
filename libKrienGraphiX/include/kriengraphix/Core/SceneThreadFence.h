#pragma once

#include <atomic>

namespace kgx
{
    class SceneThreadFence final
    {
    public:
        SceneThreadFence();

    private:
        std::atomic<bool> m_done;
    };
}
