
#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include <d3d11.h>
#include <dxgi1_2.h>

namespace kgx
{
    class RenderWindow;
    class SceneThread;
    class RenderThread;

    class KGXCore
    {
        public:
            static KGXCore* get();
            static void shutdown();

            KGXCore(const KGXCore&) noexcept            = delete;
            KGXCore& operator=(const KGXCore&) noexcept = delete;
            KGXCore(KGXCore&&) noexcept                 = delete;
            KGXCore& operator=(KGXCore&&) noexcept      = delete;

            /** Utility function that returns a direct pointer to the DirectX device */
            ID3D11Device* getDxDevicePtr() const;

            RenderWindow* createRenderWindow( HWND windowHandle );
            RenderWindow* getRenderWindow() const;

            bool isRunning() const;
            void startMainLoop( RenderWindow* renderWindow );
            void clearManagers();

            [[nodiscard]]
            SceneThread* getSceneThread() const;
            [[nodiscard]]
            RenderThread* getRenderThread() const;

        private:
            KGXCore();
            ~KGXCore();

            void clockTick();

            static KGXCore* m_inst;

            std::atomic<bool> m_isRunning;

            ID3D11Device *m_dxDev;
            IDXGIFactory2 *m_dxgiFactory;

            std::thread *m_mainClockThread;
            RenderWindow *m_renderWindow;
            SceneThread *m_sceneThread;
            RenderThread *m_renderThread;

            std::chrono::high_resolution_clock::time_point m_prevClockTime;
    };
}
