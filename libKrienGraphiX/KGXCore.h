
#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>

namespace kgx
{
	class RenderWindow;

	class KGXCore
	{
		public:
			static KGXCore* getInst();
			static void destroy();

			/** Utility function that returns a direct pointer to the DirectX device */
			ID3D11Device* getDxDevicePtr() const;
			
			RenderWindow* createRenderWindow( HWND windowHandle );
			RenderWindow* getRenderWindow() const;

			void clearManagers();
			void renderFrame() const;

		private:
			KGXCore();
			~KGXCore();

			// no copying allowed
			KGXCore( const KGXCore& )            = delete;
			KGXCore& operator=( const KGXCore& ) = delete;

			static KGXCore* m_inst;

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;
			IDXGIFactory2 *m_dxgiFactory;

			RenderWindow* m_renderWindow;
	};
}
