
#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include <map>

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
			RenderWindow* getRenderWindow( HWND windowHandle ) const;

			void clearManagers();
			void renderFrame();

		private:
			KGXCore();
			~KGXCore();

			// no copying allowed
			explicit KGXCore( const KGXCore& );
			KGXCore& operator=( const KGXCore& );

			static KGXCore* m_inst;

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;
			IDXGIFactory2 *m_dxgiFactory;

			std::map<HWND, RenderWindow*> m_renderWindows;
	};
}
