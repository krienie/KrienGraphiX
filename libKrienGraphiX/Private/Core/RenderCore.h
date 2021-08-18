
#pragma once

#include "Private/Core/RenderThread.h"
#include "Private/RHI/RenderHardwareInterface.h"

#include <memory>

namespace kgx::core
{

class RenderCore final
    {
        public:
            static RenderCore* get();
            static void startup();
            static void shutdown();

            RenderCore(const RenderCore&) noexcept            = delete;
            RenderCore& operator=(const RenderCore&) noexcept = delete;
            RenderCore(RenderCore&&) noexcept                 = delete;
            RenderCore& operator=(RenderCore&&) noexcept      = delete;

            [[nodiscard]] RHI::RHIGraphicsDevice * getGraphicsDevicePtr() const;
            [[nodiscard]] RHI::RHICommandQueue * getGraphicsCommandQueuePtr() const;
            [[nodiscard]] RHI::RenderHardwareInterface* getRHI() const;

            //RenderWindow* createRenderWindow( HWND windowHandle );
            //RenderWindow* getRenderWindow() const;

            void render();

        private:
            RenderCore();
            ~RenderCore() = default;

            static RenderCore* mInst;
            static int mRefCount;

            RenderThread mRenderThread;

            std::unique_ptr<RHI::RenderHardwareInterface> mRHI;
            std::unique_ptr<RHI::RHIGraphicsDevice> mGraphicsDevice;
            std::unique_ptr<RHI::RHICommandQueue> mCommandQueue;
    };
}
