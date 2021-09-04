#include "core/types.h"
#include "core/os.h"

#include "BpdThumbnailProvider.h"

#include "core/system/io/FileWin32Stream.h"
#include "core-ext/system/io/assets/TextureIO.h"
#include "core/math/Math.h"

#define FPF_IMPLEMENTATION
#include "five-pixel-font/five-pixel-font.h"

#include <shlwapi.h>
#include <thumbcache.h> // For IThumbnailProvider.
#include <wincodec.h>   // Windows Imaging Codecs
#include <msxml6.h>
#include <new>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "msxml6.lib")

// this thumbnail provider implements IInitializeWithStream to enable being hosted
// in an isolated process for robustness

class CBpdThumbnailProvider : public IInitializeWithStream,
							  public IThumbnailProvider
{
public:
	CBpdThumbnailProvider()
		: m_refCount(1), m_stream(NULL)
	{
	}

	virtual ~CBpdThumbnailProvider()
	{
		if (m_stream)
		{
			m_stream->Release();
		}
	}

	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE
							QueryInterface( REFIID riid, void **ppv ) override
	{
		static const QITAB qit[] =
		{
			QITABENT(CBpdThumbnailProvider, IInitializeWithStream),
			QITABENT(CBpdThumbnailProvider, IThumbnailProvider),
			{ 0 },
		};
		return QISearch(this, qit, riid, ppv);
	}

	virtual ULONG STDMETHODCALLTYPE
							AddRef( void ) override
	{
		return InterlockedIncrement(&m_refCount);
	}

	virtual ULONG STDMETHODCALLTYPE
							Release( void ) override
	{
		ULONG refCount = InterlockedDecrement(&m_refCount);
		if (refCount <= 0)
		{
			delete this;
		}
		return refCount;
	}

	// IInitializeWithStream
	IFACEMETHODIMP Initialize(IStream *pStream, DWORD grfMode);

	// IThumbnailProvider
	IFACEMETHODIMP GetThumbnail(UINT cx, HBITMAP *phbmp, WTS_ALPHATYPE *pdwAlpha);

private:
	
	long				m_refCount;
	IStream*			m_stream;     // provided during initialization.
};

HRESULT CBpdThumbnailProvider_CreateInstance ( REFIID riid, void **ppv )
{
	CBpdThumbnailProvider *pNew = new (std::nothrow) CBpdThumbnailProvider();
	HRESULT hr = pNew ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = pNew->QueryInterface(riid, ppv);
		pNew->Release();
	}
	return hr;
}

// IInitializeWithStream
IFACEMETHODIMP CBpdThumbnailProvider::Initialize ( IStream *pStream, DWORD )
{
	HRESULT hr = E_UNEXPECTED;  // can only be inited once
	if (m_stream == NULL)
	{
		// take a reference to the stream if we have not been inited yet
		hr = pStream->QueryInterface(&m_stream);
	}
	return hr;
}

unsigned char fpf_texture [FPF_TEXTURE_WIDTH * FPF_TEXTURE_HEIGHT];
static void InitFontRendering ( void )
{
	fpf_create_alpha_texture(fpf_texture, FPF_TEXTURE_WIDTH * FPF_TEXTURE_HEIGHT, FPF_TEXTURE_WIDTH, FPF_RASTER_Y_AXIS);
}

struct ByteType
{
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
};

static void PutCharScale ( ByteType* dest, uint x, uint y, uint w, uint h, float scale, const char character )
{
	uint fpf_x = 0;
	uint fpf_y = 0;
	fpf_get_glyph_position( character, &fpf_x, &fpf_y );

	uint scale_size = math::round(5 * scale);
	for ( uint cx = 0; cx < scale_size; ++cx )
	{
		for ( uint cy = 0; cy < scale_size; ++cy )
		{
			if ( x + cx < 0 || x + cx >= w
				|| y + cy < 0 || y + cy >= h )
			{	// Skip out-of-bounds
				continue;
			}

			// Get the 5x5 lookup value
			uint cx_char = (uint)(5 * (cx / (float)scale_size));
			uint cy_char = (uint)(5 * (cy / (float)scale_size));

			uchar source_alpha = fpf_texture[ (fpf_x + cx_char) + (fpf_y + cy_char) * FPF_TEXTURE_WIDTH ];
			ByteType& dest_pixel = dest[ (x + cx) + (y + cy) * w ];
			
			dest_pixel.r = (uint8)math::lerp<float>( source_alpha / 255.0F, (float)dest_pixel.r, 255.0F );
			dest_pixel.g = (uint8)math::lerp<float>( source_alpha / 255.0F, (float)dest_pixel.g, 255.0F );
			dest_pixel.b = (uint8)math::lerp<float>( source_alpha / 255.0F, (float)dest_pixel.b, 255.0F );
			dest_pixel.a = (uint8)math::lerp<float>( source_alpha / 255.0F, (float)dest_pixel.a, 255.0F );
		}
	}
}

static void PutString ( ByteType* dest, uint x, uint y, uint w, uint h, float scale, const char* str )
{
	size_t len = strlen(str);
	for (int i = 0; i < len; ++i )
	{
		PutCharScale( dest, math::round(x + i * 6 * scale), y, w, h, scale, str[i] );
	}
}


// IThumbnailProvider
IFACEMETHODIMP CBpdThumbnailProvider::GetThumbnail(UINT cx, HBITMAP *phbmp, WTS_ALPHATYPE *pdwAlpha)
{
	HRESULT hr;

	const uint maxSize = (uint)cx;

	// Get the data from the Stream
	core::FileWin32StreamRead l_streamWrapper (m_stream);
	
	core::BpdLoader l_loader;
	l_loader.m_loadImageInfo = true;
	l_loader.LoadBpd(&l_streamWrapper);
	l_loader.m_loadImageInfo = false;

	// Now that we have the image info, we allocate a buffer for Mip 0.
	// BPDs default to 8-bit RGBA for now.
	const core::gfx::tex::arColorFormat internalFormat = core::gfx::tex::kColorFormatRGBA8;
	const size_t pixelByteSize = core::gfx::tex::getColorFormatByteSize(internalFormat);
	const size_t imageByteSize = pixelByteSize * l_loader.info.width * l_loader.info.height * l_loader.info.depth;
	char* imageData = new char [imageByteSize];

	l_loader.m_buffer_Mipmaps[0] = imageData;
	l_loader.m_loadMipmapMask = 0x01 << 0;
	bool load_success = l_loader.LoadBpd();
	l_loader.m_buffer_Mipmaps[0] = NULL;

	// Given the stream earlier, create the raw BPD data from Mip 0.
	uint16 output_width, output_height;
	BYTE* output_image;
	{
		float downscaleFactor = std::min<float>( cx / (float)l_loader.info.width, cx / (float)l_loader.info.height );

		// Downscale to the given size
		const uint16 full_width  = (uint16)math::round(l_loader.info.width * downscaleFactor);
		const uint16 full_height = (uint16)math::round(l_loader.info.height * downscaleFactor);

		const uint16 block_width  = std::max<uint16>(1, l_loader.info.width / full_width);
		const uint16 block_height = std::max<uint16>(1, l_loader.info.height / full_height);

		// Allocate the data and clear to zero
		output_image = new BYTE [full_width * full_height * 4];
		memset( output_image, 0, full_width * full_height * 4 );

		// Declare the pixel layout
		struct trSrcPixelLayout
		{
			uint8 r;
			uint8 g;
			uint8 b;
			uint8 a;
		};
		struct trDestPixelLayout
		{
			BYTE b;
			BYTE g;
			BYTE r;
			BYTE a;
		};

		if (downscaleFactor <= 1.0)
		{
			// Downscale the image
			for ( uint x = 0; x < full_width; ++x )
			{
				for ( uint y = 0; y < full_height; ++y )
				{
					uint32 agg_r = 0;
					uint32 agg_g = 0;
					uint32 agg_b = 0;
					uint32 agg_a = 0;

					for ( uint block_x = 0; block_x < block_width; ++block_x )
					{
						for ( uint block_y = 0; block_y < block_height; ++block_y )
						{
							//trSrcPixelLayout* src_pixel = (trSrcPixelLayout*)&imageData[((x * block_width + block_x) + (y * block_height + block_y) * l_loader.info.height) * pixelByteSize];

							// Find closest pixel in the block
							uint closest_src_x = (uint)(l_loader.info.width * (x / (float)full_width) + (block_x / (float)l_loader.info.width));
							uint closest_src_y = (uint)(l_loader.info.height * (y / (float)full_height) + (block_y / (float)l_loader.info.height));
							trSrcPixelLayout* src_pixel = (trSrcPixelLayout*)&imageData[(closest_src_x + closest_src_y * l_loader.info.width) * pixelByteSize];
							agg_r += src_pixel->r;
							agg_g += src_pixel->g;
							agg_b += src_pixel->b;
							agg_a += src_pixel->a;
						}
					}

					const uint32_t block_sz = block_width * block_height;
					double avg_r = agg_r / (double)(block_sz);
					double avg_g = agg_g / (double)(block_sz);
					double avg_b = agg_b / (double)(block_sz);
					double avg_a = agg_a / (double)(block_sz);

					trDestPixelLayout& dest_pixel = *(trDestPixelLayout*)&output_image[(x + y * full_width) * sizeof(trDestPixelLayout)];

					dest_pixel.r = (uint8_t)std::min<uint64_t>( math::round(avg_r), 255 );
					dest_pixel.g = (uint8_t)std::min<uint64_t>( math::round(avg_g), 255 );
					dest_pixel.b = (uint8_t)std::min<uint64_t>( math::round(avg_b), 255 );
					dest_pixel.a = (uint8_t)std::min<uint64_t>( math::round(avg_a), 255 );
				}
			} // End downscaling.
		}
		else
		{
			// Upscale the image
			for ( uint x = 0; x < full_width; ++x )
			{
				for ( uint y = 0; y < full_height; ++y )
				{
					// Find closest pixel
					uint closest_src_x = (uint)(l_loader.info.width * (x / (float)full_width));
					uint closest_src_y = (uint)(l_loader.info.height * (y / (float)full_height));

					trSrcPixelLayout* src_pixel = (trSrcPixelLayout*)&imageData[(closest_src_x + closest_src_y * l_loader.info.height) * pixelByteSize];

					trDestPixelLayout& dest_pixel = *(trDestPixelLayout*)&output_image[(x + y * full_width) * sizeof(trDestPixelLayout)];
					dest_pixel.r = src_pixel->r;
					dest_pixel.g = src_pixel->g;
					dest_pixel.b = src_pixel->b;
					dest_pixel.a = src_pixel->a;
				}
			} // End upscaling.
		}

		// Set output params
		output_width = full_width;
		output_height = full_height;
	}

	// Done with image data
	delete[] imageData;

	// Draw text on the bitmap
	{
		InitFontRendering();

		float textScale = std::max(1.0F, cx / 24.0F);

		const char* typeString = "???";
		switch ( l_loader.info.type )
		{
		case core::gfx::tex::kTextureType2D:
			typeString = "2D";
			break;
		case core::gfx::tex::kTextureType3D:
			typeString = "3D";
			break;
		case core::gfx::tex::kTextureType2DArray:
			typeString = "2D*A";
			break;
		case core::gfx::tex::kTextureTypeCube:
			typeString = "CUBE";
			break;
		}
		
		PutString( 
			(ByteType*)output_image,
			1, math::round(output_height - 6 * textScale),
			output_width, output_height,
			textScale,
			typeString );
	}

	// Create a bitmap
	{
		BITMAPINFO bmi = {};
		bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biWidth = output_width;
		bmi.bmiHeader.biHeight = output_height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		BYTE *pBits;
		HBITMAP hbmp = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, reinterpret_cast<void **>(&pBits), NULL, 0);
		hr = hbmp ? S_OK : E_OUTOFMEMORY;
		if (SUCCEEDED(hr))
		{
			// Copy in the rows upside down
			for ( uint row = 0; row < output_height; ++row )
			{
				memcpy( pBits + output_width * 4 * row, output_image + output_width * 4 * (output_height - row - 1), output_width * 4 );
			}

			*phbmp = hbmp;
		}
	}

	// Done with downscaled data
	delete[] output_image;

	// Set to ARGB
	*pdwAlpha = WTSAT_ARGB;
	
	return S_OK;
}
