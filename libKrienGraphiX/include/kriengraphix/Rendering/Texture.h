
#pragma once

struct ID3D11Resource;
struct ID3D11ShaderResourceView;

namespace kgx
{
class Texture
{
    public:
        typedef int TextureID;

        Texture(TextureID id, ID3D11Resource *buffer, ID3D11ShaderResourceView *shaderResourceView);
        virtual ~Texture();

        TextureID getID() const;
        ID3D11Resource* getBuffer() const;
        ID3D11ShaderResourceView* getResourceView() const;

        Texture( const Texture &other ) = delete;
        Texture& operator=( const Texture &rhs ) = delete;

    protected:
        ID3D11Resource *m_buffer;
        ID3D11ShaderResourceView *m_shaderView;

    private:
        TextureID m_id;

};
}
