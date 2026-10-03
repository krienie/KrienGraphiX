
#pragma once

#include "KrienGraphiX/Math/MathDefines.h"

#include <functional>

namespace kgx
{
using SceneUpdateDelegate = std::function<void(float deltaTime)>;

struct Vertex
{
	math::Vector3 Position;
	math::Vector3 Normal;
	math::Vector2 UVCoordinate;
	//TODO(KL): Make vertex color optional
	math::Vector4 Color;
};

struct RawMeshData
{
	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;
};
}
