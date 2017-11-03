
#include "MaterialLibrary.h"
#include "../Filesystem.h"
#include "../parsers/KGMaterialLibraryParser.h"

namespace kgx
{
	MaterialLibrary::MaterialLibrary()
		: m_materials()
	{
	}

	Material MaterialLibrary::getMaterial( const std::string &matName )
	{
		if ( loadMaterials() )
		{
			std::unordered_map<std::string, Material>::const_iterator it;
			it = m_materials.find( matName );

			if ( it != m_materials.cend() )
				return it->second;
		}

		return Material();
	}

	bool MaterialLibrary::loadMaterials()
	{
		if ( !m_materials.empty() )
			return true;

		std::string matLibraryString;
		if ( filesystem::openFile( "MaterialLibrary.kgmat", matLibraryString ) )
		{
			std::unordered_map<std::string, KgMatData> materials;
			KGMaterialLibraryParser::parse( matLibraryString, materials );

			for ( auto &material : materials )
			{
				Material newMat;
				newMat.diffuse = material.second.diffuse;
				newMat.specular = material.second.specular;

				if ( material.second.diffuseMap.size() > 0 )
					newMat.diffuseMap = material.second.diffuseMap;

				if ( material.second.specularMap.size() > 0 )
					newMat.specularMap = material.second.specularMap;

				if ( material.second.normalMap.size() > 0 )
					newMat.normalMap = material.second.normalMap;

				m_materials.insert( std::pair<std::string, Material>( material.first, newMat ) );
			}

			return true;
		}

		return false;
	}
}
