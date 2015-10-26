
#pragma once

#include <d3d11.h>
#include <vector>
#include <map>


namespace kgx
{
	class Camera;
	class RenderableObject;
	class VertexInputLayout;
	class ShaderBase;
	class VertexShader;
	class PixelShader;

	class ShaderProgram
	{
		public:
			enum ShaderAutoBindType
			{
				NoAutoBind,					// mainly used for parsing purposes
				CameraProjectionMatrix,
				CameraViewMatrix,
				CameraPosition,
				CameraTarget,
				CameraFieldOfView,
				CameraAspectRatio,
				CameraNearZ,
				CameraFarZ,
				ObjectModelMatrix,
				ObjectNormalMatrix
			};

			typedef UINT ShaderProgramID;

			ShaderProgram( _In_ ID3D11Device *dxDevice, ShaderProgramID id );
			virtual ~ShaderProgram();

			ShaderProgramID getID() const;

			VertexShader* createVertexShader( const std::string &filename, const VertexInputLayout &layout );
			VertexShader* createVertexShader( const std::wstring &filename, const VertexInputLayout &layout );
			//void setHullShader( _In_ HullShader *shader );
			//void setDomainShader( _In_ DomainShader *shader );
			//void setGeometryShader( _In_ GeometryShader *shader );
			PixelShader* createPixelShader( const std::string &filename );
			PixelShader* createPixelShader( const std::wstring &filename );
			//TODO: add support for other shader types, except for compute-shaders => it will get its own thing, separate from ShaderProgram

			VertexShader* getVertexShader() const;
			PixelShader* getPixelShader() const;

			void addAutoShaderVar( _In_ ShaderBase *shader, const std::string &varName, ShaderAutoBindType varType );

			void activate( _In_ Camera *renderCam, _In_ RenderableObject *renderObj );

		private:
			// no copying allowed
			ShaderProgram( const ShaderProgram& );
			ShaderProgram& operator=( const ShaderProgram& );

			struct AutoShaderVar
			{
				AutoShaderVar(std::string n, ShaderAutoBindType t) : name(n), type(t) {}

				std::string name;
				ShaderAutoBindType type;
			};

			void updateAutoShaderVar( _In_ Camera *renderCam, _In_ RenderableObject *renderObj,
									  _In_ ShaderBase *shader, AutoShaderVar shaderVar );

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			ShaderProgramID m_progID;

			// shaders
			VertexShader *m_vertShader;
			PixelShader *m_pixShader;

			std::map< ShaderBase*, std::vector<AutoShaderVar> > m_constVarLinks;
	};
}
