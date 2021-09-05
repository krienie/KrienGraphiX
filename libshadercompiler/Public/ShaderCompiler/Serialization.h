
#pragma once

#include "ShaderCompiler/ShaderProgramCompiler.h"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>


namespace boost::serialization
{
template<class Archive>
void serialize(Archive & ar, kgx::VertexInputElement & vie, [[maybe_unused]] const unsigned int version)
{
    ar & vie.semanticName;
    ar & vie.format;
}

template<class Archive>
void serialize(Archive & ar, kgx::ShaderProgramDescriptor & spd, [[maybe_unused]] const unsigned int version)
{
    ar & spd.name;
    ar & spd.inputLayout;
    ar & spd.vertexShaderPath;
    ar & spd.hullShaderPath;
    ar & spd.domainShaderPath;
    ar & spd.geometryShaderPath;
    ar & spd.pixelShaderPath;
}

template<class Archive>
void serialize(Archive & ar, kgx::BufferVariableDescriptor & bvd, [[maybe_unused]] const unsigned int version)
{
    ar & bvd.name;
    ar & bvd.offset;
    ar & bvd.size;
}

template<class Archive>
void serialize(Archive & ar, kgx::ConstantBufferDescriptor & cbd, [[maybe_unused]] const unsigned int version)
{
    ar & cbd.name;
    ar & cbd.bufferRegister;
    ar & cbd.size;
    ar & cbd.variables;
}

template<class Archive>
void serialize(Archive & ar, kgx::CompiledShader & cs, [[maybe_unused]] const unsigned int version)
{
    ar & cs.byteCode;
    ar & cs.pdb;
    ar & cs.boundConstantBuffers;
}

template<class Archive>
void serialize(Archive & ar, kgx::CompiledShaderProgram & csp, [[maybe_unused]] const unsigned int version)
{
    ar & csp.vertexShader;
    ar & csp.hullShader;
    ar & csp.domainShader;
    ar & csp.geometryShader;
    ar & csp.pixelShader;
    ar & csp.constantBuffers;
}
}