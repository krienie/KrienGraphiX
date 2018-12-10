
#pragma once

#include <string>

#include <d3d11.h>

namespace kgx
{
	class VertexInputLayout;
	class ShaderBase;
	class VertexShader;
	class PixelShader;

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

			ShaderProgram( ID3D11Device *dxDevice, ShaderProgramID id, const std::string &name );
			virtual ~ShaderProgram();

			std::string getName() const;
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
			ShaderProgram( const ShaderProgram& )            = delete;
			ShaderProgram& operator=( const ShaderProgram& ) = delete;

			ShaderBase* getShader( ShaderType shader ) const;

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			std::string m_name;
			ShaderProgramID m_progID;

			// shaders
			VertexShader *m_vertShader;
			//HullShader *m_hullShader;
			//DomainShader *m_domainShader;
			//GeometryShader *m_geomShader;
			PixelShader *m_pixShader;
	};
}
