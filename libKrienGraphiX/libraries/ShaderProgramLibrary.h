
#pragma once

#include <unordered_map>

#include "../ShaderProgram.h"

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace kgx
{
	class ShaderProgramLibrary
	{
		public:
			explicit ShaderProgramLibrary( bool loadDefaultPrograms );
			~ShaderProgramLibrary();

			ShaderProgram* getShaderProgram( const std::string &name ) const;
			ShaderProgram* getShaderProgram( ShaderProgram::ShaderProgramID id ) const;
			ShaderProgram* createShaderProgram( const std::string &name );

			void releaseShaderProgram( const std::string &name );
			void releaseShaderProgram( ShaderProgram::ShaderProgramID id );

		private:
			bool loadDefaultShaderPrograms();

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			ShaderProgram::ShaderProgramID m_nextShaderProgramID;
			std::unordered_map<ShaderProgram::ShaderProgramID, ShaderProgram*> m_shaderPrograms;
			std::unordered_map<std::string, ShaderProgram::ShaderProgramID> m_shaderProgramNames;
	};
}
