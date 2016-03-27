
#include <sstream>
#include <iostream>

#include <boost/spirit/include/qi.hpp>
#include <boost/filesystem.hpp>

#include "KGObjectGenerator.h"
#include "OBJParser.h"

namespace fs = boost::filesystem;
namespace qi = boost::spirit::qi;


bool convertFile( const std::string &filename, const std::string &fileDir, const std::string &outDir );
void printHelp();


int main( int argc, char* argv[] )
{
	std::cout << "-= KrienGraphiX Parser v0.4 - (c)2016 Krien Linnenbank =-" << std::endl << std::endl;

	std::string inDir  = "/";
	std::string outDir = "/";

	// parse input arguments
	if ( argc > 1 )
	{
		for ( int i = 1; i < argc; ++i )
		{
			if ( std::string( argv[i] ) == "--indir" && i < (argc - 1) )
			{
				++i;
				inDir = argv[i];
				continue;
			} else if ( std::string( argv[i] ) == "--outdir" && i < (argc - 1) )
			{
				++i;
				outDir = argv[i];
				continue;
			} else
			{
				printHelp();
				return 0;
			}
		}
	} else
	{
		printHelp();
		return 0;
	}

	// create input directory if needed
	if ( !fs::exists( inDir )
		 && !fs::create_directories( fs::path( inDir ) ) )
	{
		std::cout << "Error creating input directory: " << inDir << std::endl;
		std::cout << "Aborting conversion... Press enter to exit" << std::endl;
	}

	// create output directory if needed
	if ( !fs::exists( outDir )
		 && !fs::create_directories( fs::path(outDir) ) )
	{
		std::cout << "Error creating output directory: " << outDir << std::endl;
		std::cout << "Aborting conversion... Press enter to exit" << std::endl;
	}
	

	std::cout << "inDir: " << inDir << std::endl
			  << "outDir: " << outDir << std::endl;


	// start converting files in the input directory
	fs::directory_iterator dir( inDir );
	for ( dir; dir != fs::directory_iterator(); ++dir )
	{
		std::string file = dir->path().filename().string();
		std::string name;
		bool parseSuccess = qi::parse( file.cbegin(), file.cend(),
									   *~qi::char_( '.' ) >> ".obj" >> qi::eoi,
									   name );

		if ( parseSuccess )
		{
			// parse found obj file
			std::cout << "Processing " << file << "... ";
			clock_t tStart = clock();

			if ( !convertFile( name, inDir, outDir ) )
				std::cout << std::endl << "Error: converting " << file << " aborted" << std::endl;

			double time = double( clock() - tStart ) / (double)CLOCKS_PER_SEC;
			std::cout << "done in: " << time << " seconds" << std::endl;
		}
	}

	std::cin.get();

	return 0;
}


bool convertFile( const std::string &filename, const std::string &fileDir, const std::string &outDir )
{
	kgx::KgoData parserOutput;
	kgx::OBJParser objP;

	if ( !objP.parseFile( filename + ".obj", fileDir, parserOutput ) )
		return false;
	std::cout << "Successfully parsed file " << filename << ".obj" << std::endl;

	std::string kgoDataStr;
	kgx::KGObjectGenerator::generate( parserOutput, kgoDataStr );

	std::stringstream ss;
	ss << outDir << "/" << filename << ".kgo";
	std::string outputFile = ss.str();
	std::cout << "Writing data to " << outputFile << "... ";
	if ( fs::exists( outputFile ) )
		std::cout << "file already exists! Overwriting... ";

	//write sink to file
	std::ofstream fileOut( outputFile );
	fileOut << kgoDataStr;
	fileOut.close();
	std::cout << "done" << std::endl;

	return true;
}


void printHelp()
{
	std::stringstream ssHelp;
	ssHelp << "USAGE:\n    kgconvert [/? | /relative] [/indir path] [/outdir path]\n\n"
		<< "where\n    Options:\n        /?           Display this help message.\n"
		<< "        --indir       Specifies the input directory containing the files to be\n                     converted.\n"
		<< "        --outdir      Specifies the output directory where the converted files\n                     are to be stored.\n\n"
		<< "The default is to convert and store files in the current folder.\n\n"
		<< "Examples\n    > kgconvert                                   ... Convert files in the\n                                                      current folder\n"
		<< "    > kgconvert /relative /indir ..\\input         ... Convert files located in\n                                                      ..\\input\n"
		<< "    > kgconvert /indir c:\\input /outdir c:\\output ... Convert files located in\n"
		<< "                                                      c:\\input and store them in                                                      c:\\output";

	std::cout << ssHelp.str() << std::endl;
}

