
#include "../Filesystem.h"
#include "../parsers/KGMaterialLibraryParser.h"
#include "MaterialLibrary.h"

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

			std::unordered_map<std::string, KgMatData>::const_iterator it;
			for ( it = materials.cbegin(); it != materials.cend(); ++it )
			{
				Material mat;
				mat.diffuse = it->second.diffuse;
				mat.specular = it->second.specular;

				if ( it->second.diffuseMap.size() > 0 )
					mat.diffuseMap = it->second.diffuseMap;

				if ( it->second.specularMap.size() > 0 )
					mat.specularMap = it->second.specularMap;

				if ( it->second.normalMap.size() > 0 )
					mat.normalMap = it->second.normalMap;

				m_materials.insert( std::pair<std::string, Material>( it->first, mat ) );
			}

			return true;
		}

		return false;
	}
}
