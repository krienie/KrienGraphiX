
#pragma once

#include <d3d11.h>
#include <vector>
#include <map>

#include "Defines.h"
#include "Object.h"


namespace kgx
{
	class Camera;
	class RenderableObject;
	class VertexInputLayout;
	class ShaderBase;
	class VertexShader;
	class PixelShader;

	class ShaderProgram : public Object
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
				ObjectModelMatrix,
				ObjectNormalMatrix
			};

			enum class ShaderType
			{
				Vertex,
				/*Hull,
				Domain,
				Geometry,*/
				Pixel
			};

			typedef UINT ShaderProgramID;

			ShaderProgram( ID3D11Device *dxDevice, ShaderProgramID id, const std::string &name = "" );
			virtual ~ShaderProgram();

			ShaderProgramID getID() const;

			VertexShader* createVertexShader( const std::string &filename, const VertexInputLayout &layout );
			//HullShader* createHullShader( const std::string &filename );
			//DomainShader* createDomainShader( const std::string &filename );
			//GeometryShader* createGeometryShader( const std::string &filename );
			PixelShader* createPixelShader( const std::string &filename );
			//TODO: add support for other shader types, except for compute-shaders => it will get its own thing, separate from ShaderProgram

			ShaderBase* getShader( ShaderType shader ) const;
			VertexShader* getVertexShader() const;
			//HullShader* getHullShader() const;
			//DomainShader* getDomainShader() const;
			//GeometryShader* getGeometryShader() const;
			PixelShader* getPixelShader() const;

			void addAutoShaderVar( ShaderType shaderType, const std::string &varName, ShaderAutoBindType varType );
			void addAutoShaderVar( ShaderBase *shader, const std::string &varName, ShaderAutoBindType varType );
			void updateShaderVar( ShaderType shaderType, const std::string &name,  const void *dataPtr );

			void activate( Camera *renderCam, RenderableObject *renderObj );

		private:
			// no copying allowed
			ShaderProgram( const ShaderProgram& );
			ShaderProgram& operator=( const ShaderProgram& );

			struct AutoShaderVar
			{
				AutoShaderVar(std::string n, ShaderAutoBindType t) : name(n), type(t) {}

				std::string name;
				ShaderAutoBindType type;
			};

			void updateAutoShaderVar( Camera *renderCam,  RenderableObject *renderObj,
									   ShaderBase *shader, AutoShaderVar shaderVar );

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			ShaderProgramID m_progID;

			// shaders
			VertexShader *m_vertShader;
			//HullShader *m_hullShader;
			//DomainShader *m_domainShader;
			//GeometryShader *m_geomShader;
			PixelShader *m_pixShader;

			std::map< ShaderBase*, std::vector<AutoShaderVar> > m_constVarLinks;
	};
}
