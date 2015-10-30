
#pragma once

#include <DirectXMath.h>

namespace kgx
{
	class Light
	{
		public:
			Light();
			//~Light();

		private:
			DirectX::XMFLOAT4 m_ambientColor;
			DirectX::XMFLOAT3 m_direction;
			float m_intensity;
	};
}