
#include "Rendering/ShaderProgramLibrary.h"

#include <d3d11.h>
#include <iostream>
#include <string>

#include "Core/KGXCore.h"
#include "IO/Filesystem.h"
#include "IO/KGShaderProgramsParser.h"

namespace kgx
{
    ShaderProgramLibrary::ShaderProgramLibrary( bool loadDefaultPrograms )
        : m_dxDev( nullptr ), m_dxDevCont( nullptr ), m_nextShaderProgramID( 0u ),
        m_shaderPrograms(), m_shaderProgramNames()
    {
        m_dxDev = KGXCore::get()->getDxDevicePtr();
        m_dxDev->GetImmediateContext( &m_dxDevCont );

        if ( loadDefaultPrograms )
            loadDefaultShaderPrograms();
    }

    ShaderProgramLibrary::~ShaderProgramLibrary()
    {
        if ( m_dxDevCont )
            m_dxDevCont->Release();

        // release all ShaderPrograms
        for ( auto &shaderProgPair : m_shaderPrograms )
            delete shaderProgPair.second;
    }

    ShaderProgram* ShaderProgramLibrary::getShaderProgram( const std::string &name ) const
    {
        if ( const auto it = m_shaderProgramNames.find(name); it != m_shaderProgramNames.cend() )
            return getShaderProgram( it->second );

        return nullptr;
    }
    ShaderProgram* ShaderProgramLibrary::getShaderProgram( ShaderProgram::ShaderProgramID id ) const
    {
        std::unordered_map<ShaderProgram::ShaderProgramID, ShaderProgram*>::const_iterator it;
        it = m_shaderPrograms.find( id );

        if ( it != m_shaderPrograms.cend() )
            return it->second;

        return nullptr;
    }

    ShaderProgram* ShaderProgramLibrary::createShaderProgram( const std::string &name )
    {
        ShaderProgram *shaderProg = getShaderProgram( name );
        if ( shaderProg )
        {
            std::cout << "Warning (ShaderProgramLibrary::createShaderProgram): ShaderProgram with name " << name << " already exists." << std::endl;
            return shaderProg;
        }

        shaderProg = new ShaderProgram( m_dxDev, m_nextShaderProgramID, name );

        m_shaderPrograms.insert( std::pair<ShaderProgram::ShaderProgramID, ShaderProgram*>( m_nextShaderProgramID, shaderProg ) );
        m_shaderProgramNames.insert( std::pair<std::string, ShaderProgram::ShaderProgramID>( name, m_nextShaderProgramID ) );
        ++m_nextShaderProgramID;

        return shaderProg;
    }

    void ShaderProgramLibrary::releaseShaderProgram( const std::string &name )
    {
        ShaderProgram *sp = getShaderProgram( name );
        if ( sp ) releaseShaderProgram( sp->getID() );
    }
    void ShaderProgramLibrary::releaseShaderProgram( ShaderProgram::ShaderProgramID id )
    {
        std::unordered_map<ShaderProgram::ShaderProgramID, ShaderProgram*>::iterator it;
        it = m_shaderPrograms.find( id );

        if ( it != m_shaderPrograms.end() )
        {
            // remove from name-id map
            m_shaderProgramNames.erase( it->second->getName() );

            // remove from id-pointer map
            delete it->second;
            m_shaderPrograms.erase( it );
        }
    }

    bool ShaderProgramLibrary::loadDefaultShaderPrograms()
    {
        // Parse and load the shader programs
        std::string shaderProgString;
        if ( filesystem::openFile( "ShaderPrograms.kgsp", shaderProgString ) )
        {
            std::unordered_map<std::string, KgShaderProgData> shaderProgData;
            KGShaderProgramsParser::parse( shaderProgString, shaderProgData );

            std::unordered_map<std::string, KgShaderProgData>::const_iterator it;
            for ( it = shaderProgData.cbegin(); it != shaderProgData.cend(); ++it )
            {
                ShaderProgram *shaderProg = createShaderProgram( it->first );
                if ( !it->second.vertexShader.empty() && !it->second.vertLayoutTypes.empty() )
                    shaderProg->createVertexShader( it->second.vertexShader, VertexInputLayout( it->second.vertLayoutTypes ) );

                if ( !it->second.pixelShader.empty() )
                    shaderProg->createPixelShader( it->second.pixelShader );
            }

            return true;
        }

        return false;
    }
}
