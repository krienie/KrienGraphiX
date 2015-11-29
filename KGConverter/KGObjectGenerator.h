
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

	// the streaming operator for ShaderProgram::ShaderAutoBindType
	inline std::ostream& operator<< ( std::ostream& os, const ShaderProgram::ShaderAutoBindType &type )
	{
		switch ( type )
		{
			case ShaderProgram::ShaderAutoBindType::NoAutoBind:
				//os << "NoAutoBind";
				break;
			case ShaderProgram::ShaderAutoBindType::CameraProjectionMatrix:
				os << "CameraProjectionMatrix";
				break;
			case ShaderProgram::ShaderAutoBindType::CameraViewMatrix:
				os << "CameraViewMatrix";
				break;
			case ShaderProgram::ShaderAutoBindType::CameraPosition:
				os << "CameraPosition";
				break;
			case ShaderProgram::ShaderAutoBindType::CameraTarget:
				os << "CameraTarget";
				break;
			case ShaderProgram::ShaderAutoBindType::CameraFieldOfView:
				os << "CameraFieldOfView";
				break;
			case ShaderProgram::ShaderAutoBindType::CameraAspectRatio:
				os << "CameraAspectRatio";
				break;
			case ShaderProgram::ShaderAutoBindType::CameraNearZ:
				os << "CameraNearZ";
				break;
			case ShaderProgram::ShaderAutoBindType::CameraFarZ:
				os << "CameraFarZ";
				break;
			case ShaderProgram::ShaderAutoBindType::ObjectModelMatrix:
				os << "ObjectModelMatrix";
				break;
			case ShaderProgram::ShaderAutoBindType::ObjectNormalMatrix:
				os << "ObjectNormalMatrix";
				break;
			default:
				break;
		}

		return os;
	}


	class KGObjectGenerator
	{
		public:
			static void generate( KgoData &inputData, std::string &output );

		private:
			KGObjectGenerator();
			//~KGObjectGenerator();

			// no copying allowed
			KGObjectGenerator( const KGObjectGenerator& );
			KGObjectGenerator& operator=(const KGObjectGenerator&);

	};
}
