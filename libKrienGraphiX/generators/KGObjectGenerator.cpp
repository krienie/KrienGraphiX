
#pragma warning( disable : 4996 )		// warning C4996: Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. These are correct.


//TODO: maybe scratch this whole thing, because it is not needed...


#define BOOST_DATE_TIME_NO_LIB
#define BOOST_SPIRIT_USE_PHOENIX_V3

#include <comdef.h>
#include <sstream>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#include "../KGXCore.h"
#include "../ShaderProgram.h"
#include "../RenderableObject.h"
#include "../VertexShader.h"
#include "../PixelShader.h"
#include "../Texture.h"
#include "KGObjectGenerator.h"


BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgModelData,
	(std::string, modelName)
	(int, startIndex)
	(int, indexCount)
	(std::string, matName)
);

/*BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgMatData::ShaderVar,
	(kgx::ShaderProgram::ShaderAutoBindType, autoBindType)
	(std::string, type)
	(std::string, name)
	(std::string, defaultValue)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgMatData::ShaderDef,
	(std::string, filename)
	(std::vector<kgx::KgMatData::ShaderVar>, variables)
	(std::vector<std::string>, textures)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgMatData,
	(std::string, name)
	(kgx::KgMatData::ShaderDef, vertexShader)
	(kgx::KgMatData::ShaderDef, pixelShader)
);*/


namespace phx   = boost::phoenix;
namespace karma = boost::spirit::karma;
typedef std::back_insert_iterator<std::string> BackInsertIt;


namespace kgx
{
	KGObjectGenerator::KGObjectGenerator()
	{
	}

	/*KGObjectGenerator::~KGObjectGenerator()
	{
	}*/


	void KGObjectGenerator::generate( const RenderableObject *renObj, std::string &output )
	{
		if ( !renObj )
			return;

		// get generator-input data
		KgoData inputData;
		renderableObjToKgoData( renObj, inputData );

		// generate Kgo string
		generate( inputData, output );
	}

	void KGObjectGenerator::generate( KgoData &inputData, std::string &outputString )
	{
		/*
		// construct header string
		std::stringstream headerStr;
		headerStr << "// KrienGraphiX Object v0.2 - (c)2016 Krien Linnenbank" << std::endl;
		// get local date and time
		boost::posix_time::time_facet *facet = new boost::posix_time::time_facet( "%d-%b-%Y %H:%M:%S" );
		headerStr.imbue( std::locale( headerStr.getloc(), facet ) );
		headerStr << "// File created: " << boost::posix_time::second_clock::local_time() << std::endl;

		struct KgoGram : karma::grammar<BackInsertIt, karma::space_type>
		{
			KgoGram( const std::string &header, const std::vector<VertexInputLayout::Type> &inLayout,
					 const std::vector<float> &inVerts, const std::vector<UINT> &inIndices,
					 const std::vector<KgModelData> &inModels, const std::vector<KgMatData> &inMatData )
					 : KgoGram::base_type( output )
			{
				using namespace karma;
				inputLayoutType = karma::stream;
				inputLayout = (inputLayoutType % ", ")[karma::_1 = phx::ref( inLayout )];

				vertex = double_;
				vertices = "Vertices(" << inputLayout << ")" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< lit( '\t' ) << no_delimit[(vertex % karma::space)[karma::_1 = phx::ref( inVerts )]] << no_delimit[eol]
					<< "}" << no_delimit[eol];

				indices = "Indices()" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< lit('\t') << no_delimit[(uint_ % karma::space)[karma::_1 = phx::ref( inIndices )]] << no_delimit[eol]
					<< "}" << no_delimit[eol];

				model = "Model(" << karma::string << ")" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< "\tIndices(" << no_delimit[int_] << "," << int_ << ")" << no_delimit[eol]
					<< "\tMaterial(" << karma::string << ")" << no_delimit[eol]
					<< "}" << no_delimit[eol];

				shaderAutoBindType = karma::stream;
				shaderVariable     = no_delimit[eol] << "\t\t" << shaderAutoBindType
										<< karma::string << no_delimit[karma::string] << "(" << karma::string << ");"
										<< no_delimit[eol << "\t"];

				vertexShader = "\tVertexShader(" << karma::string << no_delimit[")"] << no_delimit[eol]
								<< "\t{"
								<< *shaderVariable << no_delimit[eol]
								<< *("\t\tTexture(" << karma::string << ");" << no_delimit[eol]) << no_delimit[eol]
								<< "\t}" << no_delimit[eol];

				pixelShader  = "\tPixelShader(" << karma::string << no_delimit[")"] << no_delimit[eol]
								<< "\t{"
								<< *shaderVariable << no_delimit[eol]
								<< *("\t\tTexture(" << karma::string << ");" << no_delimit[eol])
								<< "\t}" << no_delimit[eol];

				material = "Material(" << karma::string << ")" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< vertexShader << no_delimit[eol] << pixelShader
					<< "}" << no_delimit[eol];

				output = header << no_delimit[eol] << vertices << no_delimit[eol] << indices << no_delimit[eol]
					<< (*model)[karma::_1 = phx::ref( inModels )] << no_delimit[eol] << (*material)[karma::_1 = phx::ref( inMatData )];
			}

			karma::rule<BackInsertIt, karma::space_type> output;
			karma::rule<BackInsertIt> inputLayout;
			karma::rule<BackInsertIt, VertexInputLayout::Type()> inputLayoutType;
			karma::rule<BackInsertIt, float()> vertex;
			karma::rule<BackInsertIt, karma::space_type> vertices;
			karma::rule<BackInsertIt, karma::space_type> indices;
			karma::rule<BackInsertIt, KgModelData(), karma::space_type> model;
			karma::rule<BackInsertIt, ShaderProgram::ShaderAutoBindType()> shaderAutoBindType;
			karma::rule<BackInsertIt, KgMatData::ShaderVar(), karma::space_type> shaderVariable;
			karma::rule<BackInsertIt, KgMatData::ShaderDef(), karma::space_type> vertexShader;
			karma::rule<BackInsertIt, KgMatData::ShaderDef(), karma::space_type> pixelShader;
			karma::rule<BackInsertIt, KgMatData(), karma::space_type> material;
		} kgoGram( headerStr.str(), inputData.inputLayout, inputData.vertices, inputData.indices, inputData.models, inputData.mats );


		BackInsertIt sink( outputString );
		bool result = karma::generate_delimited( sink, kgoGram, karma::space );

		*/

		//TODO: fix code above
	}


	void KGObjectGenerator::renderableObjToKgoData( const RenderableObject *renObj, KgoData &outputData )
	{
		// get vertex input-layout
		outputData.inputLayout = renObj->m_meshBuff.inputDescriptor.getInputTypes();

		// get DirectX immediate context
		ID3D11Device* dxDev = KGXCore::getInst()->getDxDevicePtr();
		ID3D11DeviceContext *dxDevCont = nullptr;
		dxDev->GetImmediateContext( &dxDevCont );

		// get vertex-data
		if ( renObj->m_meshBuff.vertBuff )
			copyFromBufferToVector( renObj->m_meshBuff.vertBuff, outputData.vertices, dxDev, dxDevCont );
		// get index-data
		if ( renObj->m_meshBuff.indexBuff )
			copyFromBufferToVector( renObj->m_meshBuff.indexBuff, outputData.indices, dxDev, dxDevCont );

		// get model and shader program data
		std::vector<RenderableObject::Mesh>::const_iterator meshIt;
		for ( meshIt = renObj->m_meshContainers.cbegin(); meshIt != renObj->m_meshContainers.cend(); ++meshIt )
		{
			// get model data
			/*std::vector<RenderableObject::Mesh>::const_iterator meshIt;
			for ( meshIt = objContIt->meshes.cbegin(); meshIt != objContIt->meshes.cend(); ++meshIt )
				outputData.models.push_back( KgModelData( meshIt->name, objContIt->shaderProg->getName(), meshIt->startIndex, meshIt->indexCount ) );

			// get shader program data
			KgMatData matData;
			matData.name = objContIt->shaderProg->getName();

			// get Vertex shader data
			KgMatData::ShaderDef vertShaderData;
			VertexShader *vertShader = objContIt->shaderProg->getVertexShader();
			if ( vertShader )
			{
				vertShaderData.filename = vertShader->getName();

				//TODO: collect shader variable data

				ShaderBase::const_textureiterator texIt;
				for ( texIt = vertShader->getTextureCBegin(); texIt != vertShader->getTextureCEnd(); ++texIt )
					vertShaderData.textures.push_back( (*texIt)->getName() );
			}

			// get Pixel shader data
			KgMatData::ShaderDef pixShaderData;
			PixelShader *pixShader = objContIt->shaderProg->getPixelShader();
			if ( pixShader )
			{
				pixShaderData.filename = pixShader->getName();

				//TODO: collect shader variable data

				ShaderBase::const_textureiterator texIt;
				for ( texIt = pixShader->getTextureCBegin(); texIt != pixShader->getTextureCEnd(); ++texIt )
					pixShaderData.textures.push_back( (*texIt)->getName() );
			}*/

			//TODO: fix code above

			
			//TODO: create generic file manager that keeps track of files and decides if files need to be saved to disk or not
		}

		dxDevCont->Release();
	}

	template<typename T> void KGObjectGenerator::copyFromBufferToVector( ID3D11Buffer *inputBuff, std::vector<T> &outputVec,
																		 ID3D11Device* dxDev, ID3D11DeviceContext *dxDevCont )
	{
		if ( !inputBuff || !dxDevCont )
			return;

		// create a buffer we can read from on CPU to copy the bufferdata to
		D3D11_BUFFER_DESC buffDesc;
		ZeroMemory( &buffDesc, sizeof( buffDesc ) );
		inputBuff->GetDesc( &buffDesc );
		buffDesc.Usage          = D3D11_USAGE_STAGING;
		buffDesc.BindFlags      = 0;
		buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

		ID3D11Buffer *tempBuff;
		HRESULT result = dxDev->CreateBuffer( &buffDesc, nullptr, &tempBuff );
		if ( FAILED( result ) )
		{
			_com_error error( result );
			std::cout << "Error (KGObjectGenerator::copyFromBufferToVector): Error creating temporary buffer. " << error.ErrorMessage() << std::endl;
			return;
		}

		// copy input bufferdata to temporary buffer
		dxDevCont->CopyResource( tempBuff, inputBuff );

		// copy contents of temporary buffer to outputData
		D3D11_MAPPED_SUBRESOURCE subRes;
		dxDevCont->Map( tempBuff, 0u, D3D11_MAP_READ, 0u, &subRes );

		UINT buffSize = buffDesc.ByteWidth / sizeof(T);
		outputVec.resize( buffSize );

		T *tempBuffData = static_cast<T*>(subRes.pData);
		memcpy( outputVec.data(), subRes.pData, buffSize );

		dxDevCont->Unmap( tempBuff, 0u );
	}
}

#pragma warning( default : 4996 )	// restore default warning level
