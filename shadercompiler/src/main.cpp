
#include <iostream>
#include <ShaderCompiler/Serialization.h>
#include <ShaderCompiler/ShaderProgramCompiler.h>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <string>

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    po::variables_map vm;

    // Declare the supported options.
    po::options_description poDesc("KrienGraphiX Shader Compiler");
    poDesc.add_options()
        ("help,h", "Print this help message")
        ("vertexshader,vs", po::value<std::string>(), "Path to vertex shader source file")
        ("hullshader,hs", po::value<std::string>(), "Path to hull shader source file")
        ("domainshader,ds", po::value<std::string>(), "Path to domain shader source file")
        ("geometryshader,gs", po::value<std::string>(), "Path to geometry shader source file")
        ("pixelshader,ps", po::value<std::string>(), "Path to pixel shader source file");

    try
    {
        po::store(po::parse_command_line(argc, argv, poDesc), vm);
        po::notify(vm);
    } catch(...)
    {
        std::cout << "Error parsing input arguments" << std::endl;
        std::cout << poDesc << std::endl;
        return -1;
    }

    if (vm.count("help"))
    {
        std::cout << poDesc << std::endl;
        return 1;
    }

    kgx::ShaderProgramDescriptor shDesc;

    bool inputDefined = false;
    if (vm.count("vertexshader"))
    {
        shDesc.vertexShaderPath = vm["vertexshader"].as<std::string>();
        inputDefined = true;
    }

    if (vm.count("hullshader"))
    {
        shDesc.hullShaderPath = vm["hullshader"].as<std::string>();
        inputDefined = true;
    }

    if (vm.count("domainshader"))
    {
        shDesc.domainShaderPath = vm["domainshader"].as<std::string>();
        inputDefined = true;
    }

    if (vm.count("geometryshader"))
    {
        shDesc.geometryShaderPath = vm["geometryshader"].as<std::string>();
        inputDefined = true;
    }

    if (vm.count("pixelshader"))
    {
        shDesc.pixelShaderPath = vm["pixelshader"].as<std::string>();
        inputDefined = true;
    }

    if (!inputDefined)
    {
        std::cout << poDesc << std::endl;
        return 1;
    }
    
    try
    {
        kgx::CompiledShaderProgram program = kgx::ShaderProgramCompiler::compileProgram(shDesc);

        std::ofstream ofs("compiledShader.kgx");
        boost::archive::binary_oarchive binArchive(ofs);

        binArchive << program;
    } catch (...)
    {
        std::cout << "Something went wrong!" << std::endl;
        return -1;
    }

    //TODO(KL): serialize compiled shader program

    return 0;
}