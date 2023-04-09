
#include <ShaderCompiler/Serialization.h>
#include <ShaderCompiler/ShaderCompiler.h>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/program_options.hpp>

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>

namespace po = boost::program_options;

namespace
{
const std::string CompiledShaderExtension = ".kgs";
}

int main(int argc, char* argv[])
{
    po::variables_map vm;

    // Declare the supported options.
    po::options_description poDesc("KrienGraphiX Shader Compiler");
    poDesc.add_options()
        ("help,h", "Print this help message")
        ("filepath,f", po::value<std::string>()->required(), "Path to shader source file")
        ("target,t", po::value<std::string>()->required(), "Target of the shader (e.g. vs_6_5, ps_6_5)")
        ("output,o", po::value<std::string>(), "Output path of the compiled shader");

    po::positional_options_description pos;
    pos.add("filepath,f", 1);
    pos.add("target,t", 1);

    try
    {
        po::store(po::command_line_parser(argc, argv).options(poDesc).positional(pos).run(), vm);
        po::notify(vm);
    } catch(...)
    {
        if (vm.contains("help"))
        {
            std::cout << poDesc << std::endl;
            return 0;
        }

        if (!vm.contains("filepath"))
        {
            std::cout << "Error: missing argument filepath" << std::endl;
        }
        else if (!vm.contains("target"))
        {
            std::cout << "Error: missing argument target" << std::endl;
        }
        else
        {
            std::cout << "Error parsing arguments" << std::endl;
        }

        std::cout << poDesc << std::endl;
        return 1;
    }

    kgx::CompiledShader compiledShader;

    std::string shaderFilePath = vm["filepath"].as<std::string>();
    std::string shaderTarget = vm["target"].as<std::string>();
    
    try
    {
        bool success = kgx::ShaderCompiler::compileShader(shaderFilePath, shaderTarget, true, compiledShader);
        if (!success)
        {
            std::cout << "Failed to compile shader " << shaderFilePath << std::endl;
            return -1;
        }

        std::string outputFile;
        if (vm.contains("output"))
        {
            outputFile = vm["output"].as<std::string>();

            std::filesystem::path outputPath = std::filesystem::path(outputFile);
            if (outputPath.extension() != CompiledShaderExtension)
            {
                outputFile = std::format("{0}{1}", outputFile, CompiledShaderExtension);
            }
        }
        else
        {
            std::filesystem::path shaderPath = std::filesystem::path(shaderFilePath);
            std::filesystem::path outputPath = shaderPath.parent_path() / shaderPath.stem();
            outputFile = std::format("{0}{1}", outputPath.string(), CompiledShaderExtension);
        }

        std::ofstream ofs(outputFile);
        boost::archive::binary_oarchive binArchive(ofs);

        binArchive << compiledShader;

        std::cout << "Compiled shader written to " << outputFile << std::endl;
    } catch (...)
    {
        std::cout << "Something went wrong!" << std::endl;
        return -1;
    }
    
    return 0;
}