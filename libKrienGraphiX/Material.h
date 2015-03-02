
#pragma once

#include <d3d11.h>
#include <vector>

#include "VertexInputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"

namespace kgx
{
	class Camera;
	class RenderableObject;

	class Material
	{
		public:
			enum ShaderVarType
			{
				CameraProjectionMatrix,
				CameraViewMatrix,
				CameraPosition,
				CameraTarget,
				CameraFieldOfView,
				CameraAspectRatio,
				CameraNearZ,
				CameraFarZ,
				ObjectModelMatrix
			};

			Material( _In_ ID3D11Device *dxDevice );
			~Material();

			VertexShader* createVertexShader( const std::wstring &filename, const VertexInputLayout &layout );
			//void setHullShader( _In_ HullShader *shader );
			//void setDomainShader( _In_ DomainShader *shader );
			//void setGeometryShader( _In_ GeometryShader *shader );
			PixelShader* createPixelShader( const std::wstring &filename );
			//TODO: compute shader krijgt een eigen ding, los van een Material

			//TODO: add getters for the shaders
			VertexShader* getVertexShader() const;
			PixelShader* getPixelShader() const;

			void addTexture( _In_ ID3D11ShaderResourceView* texView, _In_ ID3D11Resource* texture );
			void setSampler( _In_ ID3D11SamplerState *samp );

			void addAutoShaderVar( _In_ Shader *shader, const std::string &varName, ShaderVarType varType );

			void activate( _In_ Camera *renderCam, _In_ RenderableObject *renderObj );

		private:
			struct AutoShaderVar
			{
				AutoShaderVar( std::string n, ShaderVarType t ) : name(n), type(t) { }

				std::string name;
				ShaderVarType type;
			};

			void updateAutoShaderVar( _In_ Camera *renderCam, _In_ RenderableObject *renderObj,
										_In_ Shader *shader, AutoShaderVar shaderVar );

			ID3D11Device *dxDev;
			ID3D11DeviceContext *dxDevCont;

			// shaders
			VertexShader *vertShader;
			PixelShader *pixShader;

			// texture resources
			std::vector<ID3D11ShaderResourceView*> texViews;
			std::vector<ID3D11Resource*> texData;
			ID3D11SamplerState *sampler;

			std::map< Shader*, std::vector<AutoShaderVar> > constVarLinks;
	};
}
