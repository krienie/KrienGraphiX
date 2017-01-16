
#pragma once

#include <d3d11.h>

#include "Defines.h"


namespace kgx
{
	class VertexInputLayout;
	class ShaderBase;
	class VertexShader;
	class PixelShader;

	//TODO: prob convert this class into a struct
	class ShaderProgram
	{
		public:
			enum class ShaderType
			{
				Vertex,
				/*Hull,
				Domain,
				Geometry,*/
				Pixel
			};

			typedef INT ShaderProgramID;

			ShaderProgram( ID3D11Device *dxDevice, ShaderProgramID id, const std::string &name = "" );
			virtual ~ShaderProgram();

			ShaderProgramID getID() const;

			VertexShader* createVertexShader( const std::string &filename, const VertexInputLayout &layout );
			//HullShader* createHullShader( const std::string &filename );
			//DomainShader* createDomainShader( const std::string &filename );
			//GeometryShader* createGeometryShader( const std::string &filename );
			PixelShader* createPixelShader( const std::string &filename );
			//TODO: add support for other shader types, except for compute-shaders => it will get its own thing, separate from ShaderProgram

			VertexShader* getVertexShader() const;
			//HullShader* getHullShader() const;
			//DomainShader* getDomainShader() const;
			//GeometryShader* getGeometryShader() const;
			PixelShader* getPixelShader() const;

			void activate();
			void activate( ID3D11DeviceContext *dxContext );

		private:
			// no copying allowed
			ShaderProgram( const ShaderProgram& );
			ShaderProgram& operator=( const ShaderProgram& );

			ShaderBase* getShader( ShaderType shader ) const;

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			ShaderProgramID m_progID;

			// shaders
			VertexShader *m_vertShader;
			//HullShader *m_hullShader;
			//DomainShader *m_domainShader;
			//GeometryShader *m_geomShader;
			PixelShader *m_pixShader;
	};
}
