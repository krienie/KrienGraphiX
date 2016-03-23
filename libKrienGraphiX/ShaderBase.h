
#pragma once

#include <d3d11.h>
#include <string>
#include <vector>

#include "Object.h"

namespace kgx
{
	class ConstantBuffer;
	class Texture;

	class ShaderBase : public Object
	{
		public:
			HRESULT loadFromFile( const std::wstring &filename );
			HRESULT loadFromSource( const std::string &source, const std::string &entryPoint = "main" );

			void updateConstantVariable( const std::string &name, const void *dataPtr );

			typedef std::vector<Texture*>::const_iterator const_textureiterator;
			const_textureiterator getTextureCBegin() const;
			const_textureiterator getTextureCEnd() const;
			void addTexture( Texture *tex );

			virtual void activate() = 0;

		protected:
			ShaderBase( ID3D11Device *dxDevice, const std::string &target );
			virtual ~ShaderBase();

			virtual HRESULT build( ID3DBlob *shaderSource ) = 0;

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			std::vector<ConstantBuffer*> m_constBuffers;
			std::vector<ID3D11Buffer*> m_dxConstBuffers;		// used for setting all the constant buffers in one API call

			//TODO: m_textures might not be necessary, but lets keep it for now..
			std::vector<Texture*> m_textures;
			std::vector<ID3D11Resource*> m_texBuffers;
			std::vector<ID3D11ShaderResourceView*> m_texViews;

			std::string m_target;

		private:
			// no copying allowed
			ShaderBase( const ShaderBase& );
			ShaderBase& operator=(const ShaderBase&);

			HRESULT processLoadedShaderBlob( ID3DBlob *shaderSource );
	};
}
