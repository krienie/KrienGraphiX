
#include "Rendering/TextureLibrary.h"

#include <comdef.h>
#include <iostream>
#include <tuple>

#include <d3d11.h>

#include "Core/KGXCore.h"
#include "IO/Filesystem.h"
#include "Rendering/Texture.h"
#include "Rendering/TextureLoader.h"
#include "Rendering/RenderTexture.h"

namespace kgx
{
    TextureLibrary::TextureLibrary()
        : m_dxDev(nullptr), m_dxDevCont(nullptr), m_nextTextureID(1),
        m_textures(), m_textureNames(), m_renderTextures(), m_defaultSampler(nullptr)
    {
        m_dxDev = KGXCore::get()->getDxDevicePtr();
        m_dxDev->GetImmediateContext(&m_dxDevCont);

        m_defaultSampler = createSampler( D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, 4u );
    }

    TextureLibrary::~TextureLibrary()
    {
        clear();

        if ( m_dxDevCont )
            m_dxDevCont->Release();
    }

    void TextureLibrary::clear()
    {
        if ( m_defaultSampler )
        {
            m_defaultSampler->Release();
            m_defaultSampler = nullptr;
        }

        for ( auto &textureIDPair : m_textures )
            delete textureIDPair.second;

        m_textures.clear();
        m_textureNames.clear();
        m_renderTextures.clear();
    }

    ID3D11SamplerState* TextureLibrary::getDefaultSampler() const
    {
        return m_defaultSampler;
    }

    Texture* TextureLibrary::getTexture( const std::string &textureName ) const
    {
        std::unordered_map<std::string, Texture::TextureID>::const_iterator it;
        it = m_textureNames.find( textureName );

        if ( it != m_textureNames.cend() )
            return getTexture( it->second );

        return nullptr;
    }
    Texture* TextureLibrary::getTexture( Texture::TextureID id ) const
    {
        if ( const auto it = m_textures.find(id); it != m_textures.cend() )
            return it->second;

        return nullptr;
    }

    Texture* TextureLibrary::loadFromDisk( const std::string &file )
    {
        Texture* texture = getTexture( file );
        if ( texture )
            return texture;

        std::string absFile = filesystem::getFile( file );
        if ( absFile.empty() )
        {
            std::cout << "(TextureLibrary::loadFromDisk) Error: File " << file << " was not found." << std::endl;
            return false;
        }

        ID3D11Resource *buffer = nullptr;
        ID3D11ShaderResourceView *shaderResourceView = nullptr;
        HRESULT res = TextureLoader::loadAsShaderResource( std::wstring( absFile.begin(), absFile.end() ), &buffer, &shaderResourceView, m_dxDev, m_dxDevCont );
        if ( FAILED( res ) )
            return nullptr;

        Texture *newTexture = new Texture( m_nextTextureID, buffer, shaderResourceView );
        m_textures.insert( std::make_pair(m_nextTextureID, newTexture) );
        m_textureNames.insert( std::make_pair(file, m_nextTextureID) );

        ++m_nextTextureID;

        return newTexture;
    }

    std::shared_ptr<RenderTexture> TextureLibrary::getRenderTexture(unsigned int width, unsigned int height)
    {
        // Search for an already created render texture that is currently not used
        auto &renderTexVec = m_renderTextures[std::make_pair(width, height)];
        for (const auto &renTex : renderTexVec)
        {
            if (renTex.use_count() == 1)
            {
                return renTex;
            }
        }

        // No available render texture found. Create a new one and return that.
        auto renderTexture = std::make_shared<RenderTexture>(m_nextTextureID++, width, height);
        renderTexVec.push_back(renderTexture);
        return renderTexture;
    }

    ID3D11SamplerState * TextureLibrary::createSampler( const D3D11_FILTER &filter, const D3D11_TEXTURE_ADDRESS_MODE &mode, UINT anisotropy ) const
    {
        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory( &sampDesc, sizeof( sampDesc ) );
        sampDesc.Filter = filter;
        sampDesc.AddressU = mode;
        sampDesc.AddressV = mode;
        sampDesc.AddressW = mode;
        sampDesc.MipLODBias = 0.0f;
        sampDesc.MaxAnisotropy = anisotropy;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        // borderColor stays at 0 (black with no alpha)
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

        ID3D11SamplerState* sampler = nullptr;
        HRESULT res = m_dxDev->CreateSamplerState( &sampDesc, &sampler );
        if ( FAILED( res ) )
        {
            _com_error error( res );
            std::cout << "Error (TextureLibrary::createSampler): Error creating texture sampler:" << error.ErrorMessage() << std::endl;
            return nullptr;
        }

        return sampler;
    }

    void TextureLibrary::getShaderResourceViews( std::vector<ID3D11ShaderResourceView*>& srvList ) const
    {
        //srvList.clear();
        //srvList.reserve( m_textures.size() );
        //
        //for ( auto &texture : m_textures )
        //    srvList.push_back( texture.second->getResourceView() );
        
        srvList.clear();
        srvList.reserve( m_renderTextures.size() );
        
        for (const auto & textureSizeList : m_renderTextures )
        {
            for (const auto & texture : textureSizeList.second)
            {
                srvList.push_back( texture->getResourceView() );
            }
        }
    }
}
