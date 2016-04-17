
#pragma warning( disable : 4996 )		// warning C4996: Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. These are correct.

#define BOOST_DATE_TIME_NO_LIB
#define BOOST_SPIRIT_USE_PHOENIX_V3

#include <comdef.h>
#include <sstream>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/karma_real.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#include "../KGXCore.h"
#include "../RenderableObject.h"
#include "KGObjectGenerator.h"

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
	(std::string, matName)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgMatData,
	(std::string, name)
	(DirectX::XMFLOAT4, diffuse)
	(DirectX::XMFLOAT4, specular)
	(std::string, diffuseMap)
	(std::string, specularMap)
	(std::string, normalMap)
);


namespace phx   = boost::phoenix;
namespace karma = boost::spirit::karma;
typedef std::back_insert_iterator<std::string> BackInsertIt;

template <typename Num>
struct scientific_policy : karma::real_policies<Num>
{
	// we want the numbers always to be in scientific format
	static int floatfield(Num n) { return 1; }
};
typedef karma::real_generator<double, scientific_policy<double> > science_type;


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
		// construct header string
		std::stringstream headerStr;
		headerStr << "// KrienGraphiX Object v0.3 - (c)2016 Krien Linnenbank" << std::endl;
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
				inputLayoutType = karma::stream;
				inputLayout = (inputLayoutType % ", ")[karma::_1 = phx::ref( inLayout )];


				science_type scienceDouble;

				vertex = scienceDouble;
				vertices = "Vertices(" << inputLayout << ")" << karma::no_delimit[karma::eol]
					<< "{" << karma::no_delimit[karma::eol]
					<< karma::lit('\t') << karma::no_delimit[(vertex % karma::space)[karma::_1 = phx::ref(inVerts)]] << karma::no_delimit[karma::eol]
					<< "}" << karma::no_delimit[karma::eol];

				indices = "Indices()" << karma::no_delimit[karma::eol]
					<< "{" << karma::no_delimit[karma::eol]
					<< karma::lit('\t') << karma::no_delimit[(karma::uint_ % karma::space)[karma::_1 = phx::ref(inIndices)]] << karma::no_delimit[karma::eol]
					<< "}" << karma::no_delimit[karma::eol];

				model = "Model(" << karma::string << ")" << karma::no_delimit[karma::eol]
					<< "{" << karma::no_delimit[karma::eol]
					<< "\tIndices(" << karma::no_delimit[karma::int_] << "," << karma::int_ << ")" << karma::no_delimit[karma::eol]
					<< "\tMaterial(" << karma::string << ")" << karma::no_delimit[karma::eol]
					<< "}" << karma::no_delimit[karma::eol];

				xmFloat4 = "(" << karma::no_delimit[karma::double_] << ","
							   << karma::no_delimit[karma::double_] << ","
							   << karma::no_delimit[karma::double_] << ","
							   << karma::double_ << ")";

				material = "Material(" << karma::string << ")" << karma::no_delimit[karma::eol]
					<< "{" << karma::no_delimit[karma::eol]
					<< karma::no_delimit["\tdiffuse"] << xmFloat4 << karma::no_delimit[karma::eol]
					<< karma::no_delimit["\tspecular"] << xmFloat4 << karma::no_delimit[karma::eol]
					<< karma::no_delimit["\tdiffuseMap( "] << karma::string << ")" << karma::no_delimit[karma::eol]
					<< karma::no_delimit["\tspecularMap( "] << karma::string << ")" << karma::no_delimit[karma::eol]
					<< karma::no_delimit["\tnormalMap( "] << karma::string << ")" << karma::no_delimit[karma::eol]
					<< "}" << karma::no_delimit[karma::eol];

				output = header << karma::no_delimit[karma::eol] << vertices << karma::no_delimit[karma::eol] << indices << karma::no_delimit[karma::eol]
					<< (*model)[karma::_1 = phx::ref(inModels)] << karma::no_delimit[karma::eol] << (*material)[karma::_1 = phx::ref(inMatData)];
			}

			karma::rule<BackInsertIt, karma::space_type> output;
			karma::rule<BackInsertIt> inputLayout;
			karma::rule<BackInsertIt, VertexInputLayout::Type()> inputLayoutType;
			karma::rule<BackInsertIt, float()> vertex;
			karma::rule<BackInsertIt, karma::space_type> vertices;
			karma::rule<BackInsertIt, karma::space_type> indices;
			karma::rule<BackInsertIt, KgModelData(), karma::space_type> model;
			karma::rule<BackInsertIt, DirectX::XMFLOAT4(), karma::space_type> xmFloat4;
			karma::rule<BackInsertIt, KgMatData(), karma::space_type> material;
		} kgoGram( headerStr.str(), inputData.inputLayout, inputData.vertices, inputData.indices, inputData.models, inputData.mats );


		BackInsertIt sink( outputString );
		bool result = karma::generate_delimited( sink, kgoGram, karma::space );
		std::cout << std::endl;
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
		std::vector<RenderableObject::MaterialMeshContainer>::const_iterator matMeshIt;
		for ( matMeshIt = renObj->m_matMeshContainers.cbegin(); matMeshIt != renObj->m_matMeshContainers.cend(); ++matMeshIt )
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
