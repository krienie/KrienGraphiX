
#pragma once

#include "ShaderCompiler/ShaderCompiler.h"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>


namespace boost::serialization
{
template<class Archive>
void serialize(Archive& ar, kgx::VertexInputElement& vie, [[maybe_unused]] const unsigned int version)
{
    ar & vie.semanticName;
    ar & vie.format;
}

template<class Archive>
void serialize(Archive& ar, kgx::BufferVariableDescriptor& bvd, [[maybe_unused]] const unsigned int version)
{
    ar & bvd.name;
    ar & bvd.offset;
    ar & bvd.size;
}

template<class Archive>
void serialize(Archive& ar, kgx::ConstantBufferDescriptor& cbd, [[maybe_unused]] const unsigned int version)
{
    ar & cbd.name;
    ar & cbd.bufferRegister;
    ar & cbd.size;
    ar & cbd.variables;
}

template<class Archive>
void serialize(Archive& ar, kgx::CompiledShader& cs, [[maybe_unused]] const unsigned int version)
{
    ar & cs.byteCode;
    ar & cs.pdb;
    ar & cs.constantBuffers;
}
}