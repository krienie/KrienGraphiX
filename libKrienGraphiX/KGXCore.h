
#pragma once

#include <map>

#include "RenderWindow.h"

namespace kgx
{
	class KGXCore
	{
		public:
			static KGXCore* getInst();
			static void destroy();

			/** Utility function that returns a direct pointer to the DirectX device */
			ID3D11Device* getDxDevice() const;

			RenderWindow* createRenderWindow( HWND windowHandle );

			//void addFrameListener( Listener *l ); //TODO: add listener functionality

			void renderFrame();
			//void startRendering();

		private:
			KGXCore();
			~KGXCore();

			// no copying allowed
			KGXCore( const KGXCore& );
			KGXCore& operator=( const KGXCore& );

			static KGXCore* inst;

			ID3D11Device *dxDev;
			ID3D11DeviceContext *dxDevCont;
			IDXGIFactory2 *dxgiFactory;

			std::map<HWND, RenderWindow*> renderWindows;
	};
}
