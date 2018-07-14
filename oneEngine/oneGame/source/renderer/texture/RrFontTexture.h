#ifndef RENDERER_FONT_TEXTURE_H_
#define RENDERER_FONT_TEXTURE_H_

// ==Includes==
#include "core/math/Vector2d.h"
// RrTexture class
#include "RrTexture.h"
// Stringstream for unique string id generation
//#include <sstream>
//using std::stringstream;

// ===ENUMERATIONS===
// Enumerations for the font weight
#ifndef _WIN32
enum eFontWeight
{
	FW_NORMAL = 400
	FW_BOLD = 700,
	FW_BLACK = 900
};
enum eFontCharset
{
	ANSI_CHARSET,
    BALTIC_CHARSET,
    CHINESEBIG5_CHARSET,
    DEFAULT_CHARSET,
    EASTEUROPE_CHARSET,
    GB2312_CHARSET,
    GREEK_CHARSET,
    HANGUL_CHARSET,
    MAC_CHARSET,
    OEM_CHARSET,
    RUSSIAN_CHARSET,
    SHIFTJIS_CHARSET,
    SYMBOL_CHARSET,
    TURKISH_CHARSET,
    VIETNAMESE_CHARSET
};
#endif

enum rrFontWeight : int
{
	kFW_DontCare	= 0,
	kFW_Thin		= 100,
	kFW_ExtraLight	= 200,
	kFW_Light		= 300,
	kFW_Normal		= 400,
	kFW_Medium		= 500,
	kFW_SemiBold	= 600,
	kFW_Bold		= 700,
	kFW_ExtraBold	= 800,
	kFW_Heavy		= 900,
};

// ===STRUCTS===
// Font info struct
struct rrFontTextureInfo
{	
	//unsigned int startCharacter;
	//unsigned int setLength;

	long int	characterSet;
	bool		underline;
	bool		italic;
	//int			width;
	int			height;
	int			weight;

	//string		name;
	//bool isTexture;

	//Vector2d*	letterSizes;
	Vector2i*	glyphSize;		// Sizes of characters for converting fonts to bitmaps and for governing UVs
	Vector2i*	glyphOrigin;
	Vector2i*	glyphAdvance;
	Vector2i*	glyphTexelPosition;	// Positions of characters UV wise
};

struct rrFontTextureLoadParams
{
	int height;
	int weight;
	bool italic;
	bool underline;

	unsigned long characterSets;
};

// ===CLASS===
// Class definition
class RrFontTexture : public RrTexture
{
	TextureType( core::gfx::tex::kTextureClassFont );
protected:

	RENDER_API explicit		RrFontTexture (
		const char* s_resourceId,
		const char* s_resourcePath
	);
	RENDER_API				~RrFontTexture ( void );

public:  // Creation Interface
	// Load a texture file
	/*RENDER_API explicit RrFontTexture (
		string sInFilename );
	// Load a system font
	RENDER_API explicit RrFontTexture (
		string sInFontname,
		int iFontHeight,
		int iFontWeight				= 400,//FW_NORMAL,
		bool bItalic				= false,
		bool bUnderline				= false,
		unsigned long iCharacterSet = 0,//ANSI_CHARSET,
		unsigned int iStartCharacter= 32,
		unsigned int iCharacterNum	= 96 );
	// Destructor
	RENDER_API ~RrFontTexture ( void );*/

	//	Load ( filename, params ) : Loads a font from the disk and generates a bitmap.
	// May return a previously loaded instance of the texture.
	RENDER_API static RrFontTexture*
							Load ( const char* resource_name, int height, int weight );

	//	Load ( filename, params ) : Loads a font from the disk and generates a bitmap.
	// May return a previously loaded instance of the texture.
	RENDER_API static RrFontTexture*
							Load ( const char* resource_name, const rrFontTextureLoadParams& params );

public:
	
	//// Use and unused (exist to output error messages)
	//RENDER_API void Bind ( void );

	//// Use and unuse (draw list style)
	//RENDER_API void Set ( void );
	//// Get the active list
	//RENDER_API glHandle GetLetterList ( void ) {
	//	return iLetterList;
	//}
	//RENDER_API const rrFontTextureInfo& GetFontInfo ( void ) {
	//	return fontInfo;
	//}

	RENDER_API bool			GetIsFont ( void ) override { return true; }

	RENDER_API void			Reload ( void ) override { ; }
//public:
//	RENDER_API static RrFontTexture* pActiveFont;

protected:
	/*void LoadFont ( void );
	void LoadFontAsTexture ( void );
	void LoadFontAsTexture_System ( void );
	void ConvertFontToBitmap ( void );

	void LoadFontFromImage ( void );
	void GeneralDisplayListFromImage ( void );*/

	core::gfx::arPixel*		LoadFreetype ( const rrFontTextureLoadParams& params, uint16_t* o_width, uint16_t* o_height );

protected:
	//glHandle	iLetterList;
	//unsigned int iStartCharacter;
	
	rrFontTextureInfo fontInfo;

private:
	//string		fontUniqueSId;	// Unique font ID for storage in the texture master

	uint32_t*		m_glyphLookup;
	uint32_t		m_glyphStart;
	uint32_t		m_glyphCount;

	/*Vector2d*	pFontSizes;		// Sizes of characters for converting fonts to bitmaps and for governing UVs
	Vector2d*	pFontOrigins;
	Vector2d*	pFontOffsets;
	Vector2d*	pCharPositions; // Positions of characters UV wise
	*/
};

#endif//RENDERER_FONT_TEXTURE_H_