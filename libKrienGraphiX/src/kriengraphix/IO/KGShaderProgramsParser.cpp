
#define BOOST_DATE_TIME_NO_LIB
#define BOOST_SPIRIT_USE_PHOENIX_V3

#include "IO/KGShaderProgramsParser.h"

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

#include "Rendering/VertexInputLayout.h"

namespace phx     = boost::phoenix;
namespace qi      = boost::spirit::qi;
namespace iso8859 = boost::spirit::iso8859_1;
typedef qi::rule<std::string::const_iterator> Skipper;


namespace kgx
{
    KGShaderProgramsParser::KGShaderProgramsParser()
    {
    }

    /*KGShaderProgramsParser::~KGShaderProgramsParser()
    {
    }*/

    bool KGShaderProgramsParser::parse( const std::string &inputData, std::unordered_map<std::string, KgShaderProgData> &outputData )
    {
        outputData.clear();

        std::string parseString = inputData;
        do
        {
            KgShaderProgData progData;
            if ( parseProgram( parseString, progData ) )
                outputData.insert( std::make_pair( progData.programName, progData ) );
        } while ( !parseString.empty() );

        return !outputData.empty();
    }

    bool KGShaderProgramsParser::parseProgram( std::string &parseString, KgShaderProgData &progData )
    {
        struct ShaderProgGrammar : qi::grammar<std::string::const_iterator, Skipper>
        {
            ShaderProgGrammar( KgShaderProgData &progData )
                : base_type( start )
            {
                comment = "//" >> qi::skip( qi::blank )[*qi::print];

                vertexInputLayoutTypes = qi::string( "Position" )[qi::_val = VertexInputLayout::Type::Position]
                    | qi::string( "TextureCoordinate" )[qi::_val = VertexInputLayout::Type::TextureCoordinate]
                    | qi::string( "Normal" )[qi::_val = VertexInputLayout::Type::Normal]
                    | qi::string( "Tangent" )[qi::_val = VertexInputLayout::Type::Tangent];

                inputLayout = qi::lit( "inputlayout" ) >> qi::lit( '(' )
                    >> vertexInputLayoutTypes[phx::push_back( phx::ref( progData.vertLayoutTypes ), qi::_1 )] % qi::char_( ',' ) >> ')';

                shaderProgName = qi::lit( "ShaderProgram" ) >> qi::lit( '(' ) >> *~qi::char_( ')' ) >> qi::lit( ')' );
                source = qi::lit( "source" ) >> qi::lit( '(' ) >> *~qi::char_( ')' ) >> qi::lit( ')' );

                vertexShader = qi::lit( "VertexShader" )
                    >> qi::lit( '{' )
                    >> qi::repeat( 2 )[inputLayout | source[phx::ref( progData.vertexShader ) = qi::_1]]
                    >> qi::lit( '}' );
                pixelShader = qi::lit( "PixelShader" )
                    >> qi::lit( '{' )
                    >> source[phx::ref( progData.pixelShader ) = qi::_1]
                    >> qi::lit( '}' );

                start = shaderProgName[phx::ref( progData.programName ) = qi::_1]
                    >> qi::lit( '{' )
                    >> qi::repeat( 2 )[vertexShader | pixelShader]
                    >> qi::lit( '}' );
            }

            qi::rule<std::string::const_iterator> comment;

            qi::rule<std::string::const_iterator, Skipper> inputLayout;
            qi::rule<std::string::const_iterator, VertexInputLayout::Type(), Skipper> vertexInputLayoutTypes;
            qi::rule<std::string::const_iterator, std::string(), Skipper> shaderProgName, source;
            qi::rule<std::string::const_iterator, Skipper> vertexShader, pixelShader;

            qi::rule<std::string::const_iterator, Skipper> start;
        } shaderProgGrammar( progData );

        Skipper skipper = iso8859::space | shaderProgGrammar.comment;

        std::string::const_iterator f = parseString.cbegin();
        bool res = qi::phrase_parse( f, parseString.cend(), shaderProgGrammar, skipper );

        // store everything that hasn't been processed by the parser
        parseString = std::string( f, parseString.cend() );

        return res;
    }
}
