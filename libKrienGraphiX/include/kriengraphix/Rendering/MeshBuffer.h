#pragma once

#include <vector>

struct ID3D11Device;
struct ID3D11Buffer;

typedef unsigned int UINT;

namespace kgx
{
    class MeshBuffer final
    {
        public:
            MeshBuffer( const std::vector<float> &vertices, const std::vector<UINT> &indices,
                        UINT vertexBufferStride );
            ~MeshBuffer();

            bool isInit() const;
            ID3D11Buffer* getVertexBuffer() const;
            ID3D11Buffer* getIndexBuffer() const;
            UINT getVertexBufferStride() const;

        private:
            ID3D11Device *m_dxDev;

            bool m_isInit;

            UINT m_vertexBufferStride;
            ID3D11Buffer *m_vertBuff;
            ID3D11Buffer *m_indexBuff;
    };
}

