
#pragma once
#include <string>

namespace kgx::rendering
{
struct KGXSubMesh
{
    unsigned int IndexCount = 0;
	unsigned int StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

class KGXRenderGeometry
{
private:
	KGXRenderGeometry();
	~KGXRenderGeometry();

    //// System memory copies.  Use Blobs because the vertex/index format can be generic.
	//// It is up to the client to cast appropriately.  
	//Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	//Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU  = nullptr;
	//
	//Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	//Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;
	//
	//Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	//Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;
	//
    //// Data about the buffers.
	//UINT VertexByteStride = 0;
	//UINT VertexBufferByteSize = 0;
	//DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	//UINT IndexBufferByteSize = 0;
	//
	//// A MeshGeometry may store multiple geometries in one vertex/index buffer.
	//// Use this container to define the Submesh geometries so we can draw
	//// the Submeshes individually.
	//std::unordered_map<std::string, SubmeshGeometry> DrawArgs;
};
}
