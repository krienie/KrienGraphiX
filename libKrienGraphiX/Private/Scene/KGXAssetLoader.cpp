
#include "KrienGraphiX/Scene/KGXAssetLoader.h"

#include <ufbx.h>
#include <string>
#include <filesystem>

#include "KrienGraphiX/Core/Logging.h"
#include "KrienGraphiX/Scene/KGXMeshComponent.h"

namespace
{
kgx::math::Vector2 toVector2(const ufbx_vec2& ufbxVec2)
{
	return { ufbxVec2.x, ufbxVec2.y };
}

kgx::math::Vector3 toVector3(const ufbx_vec3& ufbxVec3)
{
	return { ufbxVec3.x, ufbxVec3.y, ufbxVec3.z };
}

kgx::math::Vector4 toVector4(const ufbx_vec4& ufbxVec4)
{
	return { ufbxVec4.x, ufbxVec4.y, ufbxVec4.z, ufbxVec4.w };
}

kgx::RawMeshData convertMeshPart(ufbx_mesh &mesh, ufbx_mesh_part &part, uint32_t indexOffset)
{
	kgx::RawMeshData partMeshData;

	partMeshData.vertices.reserve(part.num_triangles * 3);

	std::vector<uint32_t> triangleIndices;
	triangleIndices.resize(mesh.max_face_triangles * 3);

	// Iterate over each face using the specific material.
	for (uint32_t faceIndex : part.face_indices)
	{
		ufbx_face face = mesh.faces[faceIndex];

		// Triangulate the face into `tri_indices[]`.
		uint32_t numTris = ufbx_triangulate_face(
			triangleIndices.data(), triangleIndices.size(), &mesh, face);

		// Iterate over each triangle corner contiguously.
		for (size_t i = 0; i < numTris * 3; i++)
		{
			uint32_t index = triangleIndices[i];

			kgx::Vertex v
			{
				.Position = toVector3(mesh.vertex_position[index]),
				.Normal = toVector3(mesh.vertex_normal[index]),
				.UVCoordinate = toVector2(mesh.vertex_uv[index]),
				.Color = mesh.vertex_color.exists ? toVector4(mesh.vertex_color[index]) : kgx::math::Vector4(1),
			};
			partMeshData.vertices.push_back(v);
		}
	}

	// Should have written all the vertices.
	assert(partMeshData.vertices.size() == part.num_triangles * 3);

	// Generate the index buffer.
	ufbx_vertex_stream streams[1] =
	{
		{ partMeshData.vertices.data(), partMeshData.vertices.size(), sizeof(kgx::Vertex) },
	};
	partMeshData.indices.resize(part.num_triangles * 3);

	// This call will deduplicate vertices, modifying the arrays passed in `streams[]`,
	// indices are written in `indices[]` and the number of unique vertices is returned.
	size_t numVertices = ufbx_generate_indices(
		streams, 1, partMeshData.indices.data(), partMeshData.indices.size(), nullptr, nullptr);

	// Trim to only unique vertices.
	partMeshData.vertices.resize(numVertices);

	if (indexOffset > 0)
	{
		std::ranges::transform(partMeshData.indices.begin(), partMeshData.indices.end(), partMeshData.indices.begin(),
			[indexOffset](uint32_t index)
			{
				return index + indexOffset;
			});
	}

	return partMeshData;
}
}

namespace kgx::core::KGXAssetLoader
{
std::shared_ptr<KGXSceneObject> loadFromFile(const std::string& filePathString)
{
	if (!std::filesystem::exists(filePathString))
	{
		KGXLOG_ERROR("Error: file not found {}", filePathString);
		return nullptr;
	}

	const auto filePath = std::filesystem::path(filePathString);
	const std::string extension = filePath.extension().string();
	if (extension != ".fbx")
	{
		KGXLOG_ERROR("Unknown extension {}. Supported extension is FBX.", extension);
		return nullptr;
	}

	ufbx_load_opts opts = { 0 };
	opts.target_axes = ufbx_axes_right_handed_z_up;
	//opts.target_unit_meters = 1.0f;
	//opts.space_conversion = UFBX_SPACE_CONVERSION_MODIFY_GEOMETRY;
	//opts.geometry_transform_handling = UFBX_GEOMETRY_TRANSFORM_HANDLING_MODIFY_GEOMETRY;
	ufbx_error error;
	ufbx_scene *scene = ufbx_load_file(filePathString.c_str(), &opts, &error);

	if (!scene)
	{
		KGXLOG_ERROR("Failed to load: {}", error.description.data);
		return nullptr;
	}

	RawMeshData loadedMeshData;
	for (ufbx_node *node : scene->nodes)
	{
		if (!node->mesh || node->mesh->num_vertices == 0)
		{
			continue;
		}

		printf("%s\n", node->name.data);

		for (auto& materialPart : node->mesh->material_parts)
		{
			auto partMeshData = convertMeshPart(*node->mesh, materialPart, static_cast<uint32_t>(loadedMeshData.vertices.size()));
			loadedMeshData.vertices.insert(loadedMeshData.vertices.end(),
				partMeshData.vertices.begin(), partMeshData.vertices.end());

			loadedMeshData.indices.insert(loadedMeshData.indices.end(),
				partMeshData.indices.begin(), partMeshData.indices.end());
		}
	}

	ufbx_free_scene(scene);

	if (loadedMeshData.vertices.empty() || loadedMeshData.indices.empty())
	{
		return nullptr;
	}

	auto newSceneObject = std::make_shared<KGXSceneObject>(filePath.filename());

	newSceneObject->addNewComponent<KGXCustomMeshComponent>(loadedMeshData);

	return newSceneObject;
}
}

