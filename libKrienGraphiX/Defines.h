
#pragma once

#include <d3d11.h>

#include "VertexInputLayout.h"

namespace kgx
{
	/* vertex- and index buffers */
	struct MeshBuffer
	{
		MeshBuffer()
			: vertBuff(0), indexBuff(0), inputDescriptor() {}

		//TODO: define copy-constructor and assignment-operator overload to make a deep-copy

		ID3D11Buffer *vertBuff, *indexBuff;
		VertexInputLayout inputDescriptor;
	};
}
