
#define Version 0.3

#include <sstream>
#include <iostream>

//#define FUSION_MAX_VECTOR_SIZE 20
//#define BOOST_SPIRIT_USE_PHOENIX_V3
//#include "boost/fusion/include/adapt_struct.hpp"
//#include "boost/spirit/include/qi.hpp"
//#include "boost/spirit/include/karma.hpp"
//#include "boost/spirit/include/phoenix.hpp"
#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;
//namespace qi = boost::spirit::qi;


void printHelp();


int main(int argc, char* argv[])
{
	std::cout << "-= KrienGraphiX Parser v" << Version << " - (c)2015 Krien Linnenbank =-" << std::endl << std::endl;

	bool relative = false;
	std::string inDir;
	std::string outDir;

	if ( argc > 1 )
	{
		for ( int i = 1; i < argc; ++i )
		{
			if ( std::string( argv[i] ) == "/relative" )
			{
				relative = true;
				continue;
			} else if ( std::string( argv[i] ) == "/indir" && i < (argc - 1) )
			{
				i++;
				inDir = argv[i];
				continue;
			} else if ( std::string( argv[i] ) == "/outdir" && i < (argc - 1) )
			{
				i++;
				outDir = argv[i];
				continue;
			} else
			{
				printHelp();

				//if ( std::string( argv[i] ) != "/?" )
					return 0;
			}
		}
	}

	if ( relative )
	{
		std::string curDir = fs::current_path().generic_string() + "/";
		inDir  = curDir + inDir;
		outDir = curDir + outDir;
	}
	

	std::cout << "relative: " << relative << std::endl
		<< "inDir: " << inDir << std::endl
		<< "outDir: " << outDir << std::endl;


	


	//std::cout << ssHelp.str() << std::endl;



	/*std::string file = "blabla.obj";
	std::string name;
	bool parseSuccess = qi::parse( file.cbegin(), file.cend(),
		*~qi::char_('.') >> ".obj" >> qi::eoi, // qi::skip[  >> *qi::print ],
		name );

	if ( !parseSuccess )
	{
		std::cout << "Error: Unknown file format. Unable to parse." << std::endl
			<< "Aborting conversion... Press enter to exit." << std::endl;
		return 0;
	}





	std::cout << "parseSuccess: " << parseSuccess << " | name: " << name << std::endl;*/

	//std::cin.get();

	return 0;
}


void printHelp()
{
	std::stringstream ssHelp;
	ssHelp << "USAGE:\n    kgconvert [/? | /relative] [/indir path] [/outdir path]\n\n"
		<< "where\n    Options:\n        /?           Display this help message.\n"
		<< "        /relative    All paths are considered relative. Absolute paths are\n                     considered otherwise.\n"
		<< "        /indir       Specifies the input directory containing the files to be\n                     converted.\n"
		<< "        /outdir      Specifies the output directory where the converted files\n                     are to be stored.\n\n"
		<< "The default is to convert and store files in the current folder.\n\n"
		<< "Examples\n    > kgconvert                                   ... Convert files in the\n                                                      current folder\n"
		<< "    > kgconvert /relative /indir ..\\input         ... Convert files located in\n                                                      ..\\input\n"
		<< "    > kgconvert /indir c:\\input /outdir c:\\output ... Convert files located in\n"
		<< "                                                      c:\\input and store them in                                                      c:\\output";

	std::cout << ssHelp.str() << std::endl;
}

