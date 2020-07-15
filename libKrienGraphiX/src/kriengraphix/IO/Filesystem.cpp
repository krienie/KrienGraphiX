
#include "IO/Filesystem.h"

#include <filesystem>
#include <set>
#include <fstream>
#include <iostream>

#include <Windows.h>

namespace
{
    std::set<std::string> searchPaths;
}

namespace kgx::filesystem
{
    std::string getCurrentProgramPath()
    {
        char result[MAX_PATH];
        std::string progPath = std::string( result, GetModuleFileName( nullptr, result, MAX_PATH ) );
        return progPath.substr( 0, progPath.find_last_of( "/\\" ) );
    }

    std::string getAbsolutePath( const std::string &file )
    {
        //TODO: handle files with the same name, but in different search paths
        addSearchPath( getCurrentProgramPath() );

        std::set<std::string>::const_iterator it;
        for ( it = searchPaths.cbegin(); it != searchPaths.cend(); ++it )
        {
            std::filesystem::recursive_directory_iterator dirIt( *it ), eod;
            while ( dirIt != eod )
            {
                std::filesystem::path curFile = dirIt->path();
                if ( std::filesystem::is_regular_file( curFile )
                        && curFile.filename().string() == file )
                {
                    return curFile.string();
                }

                ++dirIt;
            }
        }

        return std::string();
    }


    void addSearchPath( const std::string &absolutePath )
    {
        std::filesystem::path p( absolutePath );
        if ( std::filesystem::is_directory( p ) )
            searchPaths.insert( absolutePath );
    }

    bool removeSearchPath( const std::string &pathToRemove )
    {
        return searchPaths.erase( pathToRemove ) == 1;
    }

    void clearSearchPaths()
    {
        searchPaths.clear();
    }

    std::string getFile( const std::string &filename )
    {
        std::string absFile = filename;
        if ( !std::filesystem::path( filename ).is_absolute() )
            absFile = getAbsolutePath( filename );
        if (absFile.empty() || !std::filesystem::exists( absFile ) )
        {
            std::cout << "Error (Filesystem::openFile): Source file not found." << std::endl;
            return std::string();
        }

        return absFile;
    }

    bool openFile( const std::string &file, std::string &contents )
    {
        std::string absFile = getFile( file );
        if ( absFile.empty() )
            return false;

        std::ifstream in( absFile, std::ios::in | std::ios::binary );
        if ( in )
        {
            in.seekg( 0, std::ios::end );
            contents.resize( in.tellg() );
            in.seekg( 0, std::ios::beg );
            in.read( &contents[0], contents.size() );
            in.close();
            return true;
        }

        return false;
    }

    bool saveFile( const std::string &fileDir, const std::string &fileName, const std::string &contents )
    {
        if ( !std::filesystem::is_directory( fileDir ) )
        {
            std::cout << "Error (filesytem::saveFile): " << fileDir << " is not a valid directory. Aborting" << std::endl;
            return false;
        }

        std::filesystem::path dir( fileDir );
        if ( dir.is_relative() || !std::filesystem::exists( dir ) && !std::filesystem::create_directory( dir ) )
        {
            std::cout << "Error (filesytem::saveFile): Error creating directory: " << fileDir << " Aborting" << std::endl;
            return false;
        }

        std::filesystem::path file( fileName );
        std::filesystem::path fullPath = dir / file;

        if ( std::filesystem::exists( fullPath ) )
            std::cout << "Warning (filesytem::saveFile): File already exists. Overwriting..." << std::endl;

        std::ofstream fileOut( fullPath.string() );
        fileOut << contents;
        fileOut.close();
        return true;
    }
}
