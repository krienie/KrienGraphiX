
#include <d3d11.h>
#include <iostream>
#include <assert.h>
#include <wincodec.h>
#include <memory>

#include "TextureLoader.h"


// Most of the code below taken from MSDN How to: Initialize a texture from file
// http://msdn.microsoft.com/en-us/library/windows/desktop/ff476904(v=vs.85).aspx
namespace kgx { namespace TextureLoader
{
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && !defined(DXGI_1_2_FORMATS)
	#define DXGI_1_2_FORMATS
#endif


	template<class T> class ScopedObject
	{
	public:
		explicit ScopedObject( T *p = 0 ) : _pointer(p) {}
		~ScopedObject()
		{
			if ( _pointer )
			{
				_pointer->Release();
				_pointer = nullptr;
			}
		}

		bool IsNull() const { return (!_pointer); }

		T& operator*() { return *_pointer; }
		T* operator->() { return _pointer; }
		T** operator&() { return &_pointer; }

		void Reset(T *p = 0) { if ( _pointer ) { _pointer->Release(); } _pointer = p; }

		T* get() const { return _pointer; }

	private:
		ScopedObject(const ScopedObject&);
		ScopedObject& operator=(const ScopedObject&);
        
		T* _pointer;
	};


	struct WICTranslate
	{
		GUID                wic;
		DXGI_FORMAT         format;
	};

	static WICTranslate wicFormatList[] = 
	{
		{ GUID_WICPixelFormat128bppRGBAFloat,       DXGI_FORMAT_R32G32B32A32_FLOAT },

		{ GUID_WICPixelFormat64bppRGBAHalf,         DXGI_FORMAT_R16G16B16A16_FLOAT },
		{ GUID_WICPixelFormat64bppRGBA,             DXGI_FORMAT_R16G16B16A16_UNORM },

		{ GUID_WICPixelFormat32bppRGBA,             DXGI_FORMAT_R8G8B8A8_UNORM },
		{ GUID_WICPixelFormat32bppBGRA,             DXGI_FORMAT_B8G8R8A8_UNORM },				// DXGI 1.1
		{ GUID_WICPixelFormat32bppBGR,              DXGI_FORMAT_B8G8R8X8_UNORM },				// DXGI 1.1

		{ GUID_WICPixelFormat32bppRGBA1010102XR,    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM },	// DXGI 1.1
		{ GUID_WICPixelFormat32bppRGBA1010102,      DXGI_FORMAT_R10G10B10A2_UNORM },
		{ GUID_WICPixelFormat32bppRGBE,             DXGI_FORMAT_R9G9B9E5_SHAREDEXP },

#ifdef DXGI_1_2_FORMATS

		{ GUID_WICPixelFormat16bppBGRA5551,         DXGI_FORMAT_B5G5R5A1_UNORM },
		{ GUID_WICPixelFormat16bppBGR565,           DXGI_FORMAT_B5G6R5_UNORM },

#endif // DXGI_1_2_FORMATS

		{ GUID_WICPixelFormat32bppGrayFloat,        DXGI_FORMAT_R32_FLOAT },
		{ GUID_WICPixelFormat16bppGrayHalf,         DXGI_FORMAT_R16_FLOAT },
		{ GUID_WICPixelFormat16bppGray,             DXGI_FORMAT_R16_UNORM },
		{ GUID_WICPixelFormat8bppGray,              DXGI_FORMAT_R8_UNORM },

		{ GUID_WICPixelFormat8bppAlpha,             DXGI_FORMAT_A8_UNORM },

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
		{ GUID_WICPixelFormat96bppRGBFloat,         DXGI_FORMAT_R32G32B32_FLOAT },
#endif
	};


	struct WICConvert
	{
		GUID source;
		GUID target;
	};

	static WICConvert wicConvertList[] = 
	{
		{ GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray },			// DXGI_FORMAT_R8_UNORM

		{ GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM 

		{ GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray },			// DXGI_FORMAT_R8_UNORM 
		{ GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray },			// DXGI_FORMAT_R8_UNORM 

		{ GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf },		// DXGI_FORMAT_R16_FLOAT 
		{ GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat },	// DXGI_FORMAT_R32_FLOAT 

#ifdef DXGI_1_2_FORMATS

		{ GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 },		// DXGI_FORMAT_B5G5R5A1_UNORM

#else

		{ GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat16bppBGRA5551,         GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat16bppBGR565,           GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM

#endif // DXGI_1_2_FORMATS

		{ GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 },	// DXGI_FORMAT_R10G10B10A2_UNORM

		{ GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM 

		{ GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM

		{ GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT 

		{ GUID_WICPixelFormat96bppRGBFixedPoint,    GUID_WICPixelFormat128bppRGBAFloat },	// DXGI_FORMAT_R32G32B32A32_FLOAT 
		{ GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat },	// DXGI_FORMAT_R32G32B32A32_FLOAT 
		{ GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat },	// DXGI_FORMAT_R32G32B32A32_FLOAT 
		{ GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat },	// DXGI_FORMAT_R32G32B32A32_FLOAT 
		{ GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat },	// DXGI_FORMAT_R32G32B32A32_FLOAT 

		{ GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
		{ GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT 
#endif

		// n-channel formats are not supported
	};
	
	
	static IWICImagingFactory* getWIC()
	{
		static IWICImagingFactory* wicFactory = nullptr;

		if ( wicFactory )
			return wicFactory;

		HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			__uuidof(IWICImagingFactory),
			(LPVOID*)&wicFactory
			);

		if ( FAILED(hr) )
		{
			wicFactory = nullptr;
			return nullptr;
		}

		return wicFactory;
	}


	static DXGI_FORMAT wicToDXGI( const GUID& guid )
	{
		for( size_t i=0; i < _countof(wicFormatList); ++i )
		{
			if ( memcmp( &wicFormatList[i].wic, &guid, sizeof(GUID) ) == 0 )
				return wicFormatList[i].format;
		}

		return DXGI_FORMAT_UNKNOWN;
	}


	static size_t wicBitsPerPixel( REFGUID targetGuid )
	{
		IWICImagingFactory* pWIC = getWIC();
		if ( !pWIC )
			return 0;
 
		IWICComponentInfo *cinfo;
		if ( FAILED( pWIC->CreateComponentInfo( targetGuid, &cinfo ) ) )
			return 0;

		WICComponentType type;
		if ( FAILED( cinfo->GetComponentType( &type ) ) )
			return 0;

		if ( type != WICPixelFormat )
			return 0;

		IWICPixelFormatInfo *pfinfo;
		if ( FAILED( cinfo->QueryInterface( __uuidof(IWICPixelFormatInfo), reinterpret_cast<void**>( &pfinfo )  ) ) )
			return 0;

		UINT bpp;
		if ( FAILED( pfinfo->GetBitsPerPixel( &bpp ) ) )
			return 0;

		return bpp;
	}


	HRESULT loadAsShaderResource( const std::wstring &file, ID3D11Resource **texture, ID3D11ShaderResourceView **shaderView,
									ID3D11Device *dxDev, ID3D11DeviceContext *dxDevCont )
	{
		if ( !dxDev || !dxDevCont )
			return E_INVALIDARG;

		if ( !comIsInit )
		{
			CoInitialize(NULL);
			comIsInit = true;
		}

		ScopedObject<IWICBitmapDecoder> decoder;
		HRESULT res = getWIC()->CreateDecoderFromFilename( file.c_str(), 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder );
		if ( FAILED(res) )
			return res;

		ScopedObject<IWICBitmapFrameDecode> frame;
		res = decoder->GetFrame(0, &frame);
		if ( FAILED(res) )
			return res;

		UINT width, height;
		res = frame->GetSize( &width, &height );
		if ( FAILED(res) )
			return res;

		assert( width > 0 && height > 0 );

		//calculate max width and height
		UINT maxSize = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
		UINT tWidth, tHeight;
		if ( width > maxSize || height > maxSize )
		{
			float ar = (float)height / width;
			if ( width > height )
			{
				tWidth  = maxSize;
				tHeight = (UINT)(float(maxSize) * ar);
			}
			else
			{
				tWidth  = (UINT)(float(maxSize) / ar);
				tHeight = maxSize;
			}
			assert( tWidth <= maxSize && tHeight <= maxSize );
		}
		else
		{
			tWidth  = width;
			tHeight = height;
		}


		// Determine format
		WICPixelFormatGUID pixelFormat;
		res = frame->GetPixelFormat( &pixelFormat );
		WICPixelFormatGUID convertGUID;
		memcpy( &convertGUID, &pixelFormat, sizeof(WICPixelFormatGUID) );

		size_t bpp = 0;

		DXGI_FORMAT format = wicToDXGI( pixelFormat );
		if ( format == DXGI_FORMAT_UNKNOWN )
		{
			for( size_t i = 0; i < _countof(wicConvertList); ++i )
			{
				if ( memcmp( &wicConvertList[i].source, &pixelFormat, sizeof(WICPixelFormatGUID) ) == 0 )
				{
					memcpy( &convertGUID, &wicConvertList[i].target, sizeof(WICPixelFormatGUID) );

					format = wicToDXGI( wicConvertList[i].target );
					assert( format != DXGI_FORMAT_UNKNOWN );
					bpp = wicBitsPerPixel( convertGUID );
					break;
				}
			}

			if ( format == DXGI_FORMAT_UNKNOWN )
				return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
		}
		else
		{
			bpp = wicBitsPerPixel( pixelFormat );
		}

		if ( !bpp )
			return E_FAIL;


		// Verify our target format is supported by the current device
		// (handles WDDM 1.0 or WDDM 1.1 device driver cases as well as DirectX 11.0 Runtime without 16bpp format support)
		UINT support = 0;
		res = dxDev->CheckFormatSupport( format, &support );
		if ( FAILED(res) || !(support & D3D11_FORMAT_SUPPORT_TEXTURE2D) )
		{
			// Fallback to RGBA 32-bit format which is supported by all devices
			memcpy( &convertGUID, &GUID_WICPixelFormat32bppRGBA, sizeof(WICPixelFormatGUID) );
			format = DXGI_FORMAT_R8G8B8A8_UNORM;
			bpp    = 32;
		}


		// Allocate temporary memory for image
		size_t rowPitch  = ( tWidth * bpp + 7 ) / 8;
		size_t imageSize = rowPitch * tHeight;

		std::unique_ptr<uint8_t[]> temp( new uint8_t[ imageSize ] );
		// Load image data
		if ( memcmp( &convertGUID, &pixelFormat, sizeof(GUID) ) == 0
				&& tWidth == width
				&& tHeight == height )
		{
			// No format conversion or resize needed
			res = frame->CopyPixels( 0, (UINT)rowPitch, (UINT)imageSize, temp.get() );  
			if ( FAILED(res) )
				return res;
		} else if ( tWidth != width || tHeight != height )
		{
			// Resize
			IWICImagingFactory* pWIC = getWIC();
			if ( !pWIC )
				return E_NOINTERFACE;

			ScopedObject<IWICBitmapScaler> scaler;
			res = pWIC->CreateBitmapScaler( &scaler );
			if ( FAILED(res) )
				return res;

			res = scaler->Initialize( frame.get(), tWidth, tHeight, WICBitmapInterpolationModeFant );
			if ( FAILED(res) )
				return res;

			WICPixelFormatGUID pfScaler;
			res = scaler->GetPixelFormat( &pfScaler );
			if ( FAILED(res) )
				return res;

			if ( memcmp( &convertGUID, &pfScaler, sizeof(GUID) ) == 0 )
			{
				// No format conversion needed
				res = scaler->CopyPixels( 0, (UINT)rowPitch, (UINT)imageSize, temp.get() );  
				if ( FAILED(res) )
					return res;
			} else
			{
				ScopedObject<IWICFormatConverter> FC;
				res = pWIC->CreateFormatConverter( &FC );
				if ( FAILED(res) )
					return res;

				res = FC->Initialize( scaler.get(), convertGUID, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom );
				if ( FAILED(res) )
					return res;

				res = FC->CopyPixels( 0, (UINT)rowPitch, (UINT)imageSize, temp.get() );  
				if ( FAILED(res) )
					return res;
			}
		} else
		{
			// Format conversion but no resize
			IWICImagingFactory* pWIC = getWIC();
			if ( !pWIC )
				return E_NOINTERFACE;

			ScopedObject<IWICFormatConverter> FC;
			res = pWIC->CreateFormatConverter( &FC );
			if ( FAILED(res) )
				return res;

			res = FC->Initialize( frame.get(), convertGUID, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom );
			if ( FAILED(res) )
				return res;

			res = FC->CopyPixels( 0, static_cast<UINT>( rowPitch ), static_cast<UINT>( imageSize ), temp.get() );  
			if ( FAILED(res) )
				return res;
		}


		// See if format is supported for auto-gen mipmaps
		bool autogen = false;
		if ( dxDevCont != 0 && shaderView != 0 ) // Must have context and shader-view to auto generate mipmaps
		{
			UINT fmtSupport = 0;
			res = dxDev->CheckFormatSupport( format, &fmtSupport );
			if ( SUCCEEDED(res) && ( fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN ) )
				autogen = true;
		}


		// Create texture
		D3D11_TEXTURE2D_DESC desc;
		desc.Width				= tWidth;
		desc.Height				= tHeight;
		desc.MipLevels			= autogen ? 0 : 1;
		desc.ArraySize			= 1;
		desc.Format				= format;
		desc.SampleDesc.Count   = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage				= D3D11_USAGE_DEFAULT;
		desc.BindFlags			= autogen ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
		desc.CPUAccessFlags		= 0;
		desc.MiscFlags			= autogen ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem		  = temp.get();
		initData.SysMemPitch	  = (UINT)rowPitch;
		initData.SysMemSlicePitch = (UINT)imageSize;

		ID3D11Texture2D *tex = nullptr;
		res = dxDev->CreateTexture2D( &desc, autogen ? nullptr : &initData, &tex );
		if ( SUCCEEDED(res) && tex != 0 )
		{
			if (shaderView != 0)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
				memset( &SRVDesc, 0, sizeof( SRVDesc ) );
				SRVDesc.Format				= format;
				SRVDesc.ViewDimension		= D3D11_SRV_DIMENSION_TEXTURE2D;
				SRVDesc.Texture2D.MipLevels = autogen ? -1 : 1;

				res = dxDev->CreateShaderResourceView( tex, &SRVDesc, shaderView );
				if ( FAILED(res) )
				{
					tex->Release();
					return res;
				}

				if ( autogen )
				{
					assert( dxDevCont != 0 );
					dxDevCont->UpdateSubresource( tex, 0, nullptr, temp.get(), (UINT)rowPitch, (UINT)imageSize );
					dxDevCont->GenerateMips( *shaderView );
				}
			}

			if (texture != 0)
				*texture = tex;
			else
				tex->Release();
		}

		return S_OK;
	}


	void closeThread()
	{
		if ( comIsInit )
			CoUninitialize();
	}
}}