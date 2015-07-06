
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

	class Material
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
				ObjectModelMatrix
			};

			explicit Material( _In_ ID3D11Device *dxDevice );
			~Material();

			VertexShader* createVertexShader( const std::string &filename, const VertexInputLayout &layout );
			VertexShader* createVertexShader( const std::wstring &filename, const VertexInputLayout &layout );
			//void setHullShader( _In_ HullShader *shader );
			//void setDomainShader( _In_ DomainShader *shader );
			//void setGeometryShader( _In_ GeometryShader *shader );
			PixelShader* createPixelShader( const std::string &filename );
			PixelShader* createPixelShader( const std::wstring &filename );
			//TODO: compute shader krijgt een eigen ding, los van een Material

			//TODO: add getters for the shaders
			VertexShader* getVertexShader() const;
			PixelShader* getPixelShader() const;

			void addTexture( _In_ ID3D11ShaderResourceView* texView, _In_ ID3D11Resource* texture );
			void setSampler( _In_ ID3D11SamplerState *samp );

			void addAutoShaderVar( _In_ ShaderBase *shader, const std::string &varName, ShaderAutoBindType varType );

			void activate( _In_ Camera *renderCam, _In_ RenderableObject *renderObj );

		private:
			// no copying allowed
			Material( const Material& );
			Material& operator=( const Material& );

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

			// shaders
			VertexShader *m_vertShader;
			PixelShader *m_pixShader;

			// texture resources
			std::vector<ID3D11ShaderResourceView*> m_texViews;
			std::vector<ID3D11Resource*> m_texData;
			ID3D11SamplerState *m_sampler;

			std::map< ShaderBase*, std::vector<AutoShaderVar> > m_constVarLinks;
	};
}
