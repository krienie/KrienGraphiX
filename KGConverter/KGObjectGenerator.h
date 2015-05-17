
#pragma once

#include <parser_defines.h>


namespace kgx
{
	// the streaming operator for VertexInputLayout::Type
	inline std::ostream& operator<< ( std::ostream& os, const VertexInputLayout::Type &type )
	{
		switch ( type )
		{
			case VertexInputLayout::Position:
				os << "Position";
				break;
			case VertexInputLayout::TextureCoordinate:
				os << "TextureCoordinate";
				break;
			case VertexInputLayout::Normal:
				os << "Normal";
				break;
			case VertexInputLayout::Tangent:
				os << "Tangent";
				break;
			default:
				break;
		}

		return os;
	}

	// the streaming operator for Material::ShaderAutoBindType
	inline std::ostream& operator<< ( std::ostream& os, const Material::ShaderAutoBindType &type )
	{
		switch ( type )
		{
			case Material::ShaderAutoBindType::NoAutoBind:
				//os << "NoAutoBind";
				break;
			case Material::ShaderAutoBindType::CameraProjectionMatrix:
				os << "CameraProjectionMatrix";
				break;
			case Material::ShaderAutoBindType::CameraViewMatrix:
				os << "CameraViewMatrix";
				break;
			case Material::ShaderAutoBindType::CameraPosition:
				os << "CameraPosition";
				break;
			case Material::ShaderAutoBindType::CameraTarget:
				os << "CameraTarget";
				break;
			case Material::ShaderAutoBindType::CameraFieldOfView:
				os << "CameraFieldOfView";
				break;
			case Material::ShaderAutoBindType::CameraAspectRatio:
				os << "CameraAspectRatio";
				break;
			case Material::ShaderAutoBindType::CameraNearZ:
				os << "CameraNearZ";
				break;
			case Material::ShaderAutoBindType::CameraFarZ:
				os << "CameraFarZ";
				break;
			case Material::ShaderAutoBindType::ObjectModelMatrix:
				os << "ObjectModelMatrix";
				break;
			default:
				break;
		}

		return os;
	}

	//TODO: dit hoeft helemaal geen klasse te zijn...
	class KGObjectGenerator
	{
		public:
			KGObjectGenerator();
			//~KGObjectGenerator();

			void generate( _In_ KgoData &inputData, _Inout_ std::string &output );

		private:

	};
}
