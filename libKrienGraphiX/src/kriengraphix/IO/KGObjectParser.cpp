
#ifndef BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_USE_PHOENIX_V3
#endif

#pragma warning( disable : 4244 )		// warning C4244: conversion from 'unsigned int' to 'float', possible loss of data. This is intentional.

#include "IO/KGObjectParser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include "Core/KGXCore.h"
#include "Core/SceneThread.h"
#include "IO/KGParserDefines.h"
#include "IO/Filesystem.h"
#include "Rendering/MeshBuffer.h"
#include "Rendering/VertexInputLayout.h"
#include "Rendering/MaterialLibrary.h"
#include "Simulation/SceneObject.h"
#include "Simulation/MeshComponent.h"

BOOST_FUSION_ADAPT_STRUCT(
    DirectX::XMFLOAT4,
    (float, x)
    (float, y)
    (float, z)
    (float, w)
);

BOOST_FUSION_ADAPT_STRUCT(
    kgx::KgModelData,
    (std::string, modelName)
    (int, startIndex)
    (int, indexCount)
    (std::string, materialName)
);

namespace phx     = boost::phoenix;
namespace qi      = boost::spirit::qi;
namespace iso8859 = boost::spirit::iso8859_1;
typedef qi::rule<std::string::const_iterator> Skipper;

namespace kgx
{
    void KGObjectParser::loadKGO(const std::string &kgoFile, const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &scale)
    {
        std::string kgObjectData;
        if ( !filesystem::openFile( kgoFile, kgObjectData ) )
        {
            std::cout << "(KGSceneParser::loadKGO): Error loading kgobject file." << std::endl;
            return;
        }

        std::vector<float> vertices;
        std::vector<UINT> indices;
        std::vector<VertexInputLayout::Type> vertLayoutTypes;
        std::vector<KgModelData> models;
        struct KgoGrammar : qi::grammar<std::string::const_iterator, Skipper>
        {
            KgoGrammar( std::vector<float> &v, std::vector<UINT> &i, std::vector<VertexInputLayout::Type> &l,
                        std::vector<KgModelData> &m )
                : base_type( start )
            {
                comment = "//" >> qi::skip( qi::blank )[*qi::print];

                vertexInputLayout = qi::string( "Position" )[qi::_val = VertexInputLayout::Type::Position]
                    | qi::string( "TextureCoordinate" )[qi::_val = VertexInputLayout::Type::TextureCoordinate]
                    | qi::string( "Normal" )[qi::_val = VertexInputLayout::Type::Normal]
                    | qi::string( "Tangent" )[qi::_val = VertexInputLayout::Type::Tangent];
                vertices = "Vertices" >> qi::lit( "(" ) >> vertexInputLayout[phx::push_back( phx::ref( l ), qi::_1 )] % qi::char_( ',' ) >> ")"
                    >> qi::lit( "{" ) >> *qi::float_[phx::push_back( phx::ref( v ), qi::_1 )] >> "}";

                indices = "Indices" >> qi::lit( "(" ) >> qi::lit( ")" ) >> qi::lit( "{" ) >> *qi::uint_[phx::push_back( phx::ref( i ), qi::_1 )] >> "}";

                //TODO: add support for different order of model contents
                models = "Model" >> qi::lit( "(" ) >> *~qi::char_( ')' ) >> qi::lit( ")" ) >> qi::lit( "{" )
                    >> "Indices" >> qi::lit( "(" ) >> qi::int_ >> qi::lit( "," ) >> qi::int_ >> qi::lit( ")" )
                    >> "Material" >> qi::lit( "(" ) >> *~qi::char_( ')' ) >> qi::lit( ")" )
                    >> qi::lit( "}" );

                start = *(vertices | indices | models[phx::push_back( phx::ref( m ), qi::_1 )]);
            }

            qi::rule<std::string::const_iterator, Skipper> start;
            qi::rule<std::string::const_iterator, Skipper> vertices, indices;
            qi::rule<std::string::const_iterator, VertexInputLayout::Type(), Skipper> vertexInputLayout;
            qi::rule<std::string::const_iterator, KgModelData(), Skipper> models;

            qi::rule<std::string::const_iterator> comment;
        } kgmGrammar( vertices, indices, vertLayoutTypes, models );

        Skipper skipper = iso8859::space | kgmGrammar.comment;

        std::string::const_iterator f = kgObjectData.cbegin();
        qi::phrase_parse( f, kgObjectData.cend(), kgmGrammar, skipper );

        // print everything that hasn't been processed by the parser
        if ( f != kgObjectData.cend() )
        {
            std::string::const_iterator end = std::distance( f, kgObjectData.cend() ) > 100 ? f + 100 : kgObjectData.cend();
            std::cout << std::endl << "KGO parsing trail: " << std::string( f, end ) << std::endl;
            return;
        }

        addParsedDataToScene( vertices, indices, vertLayoutTypes, models, position, scale );
    }

    bool KGObjectParser::addParsedDataToScene(std::vector<float> vertices, std::vector<UINT> &indices,
                                              std::vector<VertexInputLayout::Type> &vertLayoutTypes,
                                              std::vector<KgModelData> &models,
                                              const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &scale)
    {
        VertexInputLayout vertLayout( vertLayoutTypes );
        MeshBuffer *meshBuffer = new MeshBuffer( vertices, indices, vertLayout.getBufferStride() );
        if ( !meshBuffer->isInit() )
        {
            delete meshBuffer;
            return false;
        }

        SceneThread *sceneThread = KGXCore::get()->getSceneThread();

        MaterialLibrary matLibrary;
        for ( KgModelData &kgModelData : models )
        {
            // TODO(KL): Create a texture streaming system to load textures while running over multiple frames
            Material mat = matLibrary.getMaterial(kgModelData.materialName);
            if (!mat.diffuseMap.empty())
            {
                sceneThread->enqueueSingleCommand([mat](Scene *scene)
                {
                    scene->getSceneTextures()->loadFromDisk(mat.diffuseMap);
                });
            }
            if (!mat.normalMap.empty())
            {
                sceneThread->enqueueSingleCommand([mat](Scene *scene)
                {
                    scene->getSceneTextures()->loadFromDisk(mat.normalMap);
                });
            }
            if (!mat.specularMap.empty())
            {
                sceneThread->enqueueSingleCommand([mat](Scene *scene)
                {
                    scene->getSceneTextures()->loadFromDisk(mat.specularMap);
                });
            }

            auto * newSceneObject = new SceneObject(kgModelData.modelName);
            auto * meshComponent = newSceneObject->addNewComponent<MeshComponent>(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, meshBuffer, kgModelData.indexCount, kgModelData.startIndex, 0u);
            meshComponent->setMaterial(mat);
            newSceneObject->setPosition(position.x, position.y, position.z);
            newSceneObject->setScale(scale.x, scale.y, scale.z);

            sceneThread->addSceneObject(newSceneObject);
        }

        return true;
    }
}

#pragma warning( default : 4244 )	// restore default warning level
