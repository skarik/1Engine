
#ifndef _C_BITMAP_FONT_H_
#define _C_BITMAP_FONT_H_

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

// ===STRUCTS===
// Font info struct
struct tBitmapFontInfo
{	
	unsigned int startCharacter;
	unsigned int setLength;

	long int characterSet;
	bool underline;
	bool italic;
	int width;
	int height;
	int weight;

	string name;

	bool isTexture;

	//Vector2d*	letterSizes;
	Vector2d*	fontSizes;		// Sizes of characters for converting fonts to bitmaps and for governing UVs
	Vector2d*	fontOrigins;
	Vector2d*	fontOffsets;
	Vector2d*	charPositions; // Positions of characters UV wise
};

// ===CLASS===
// Class definition
class CBitmapFont : public RrTexture
{
	TextureType( TextureClassBitmapFont );
public:
	// Load a texture file
	RENDER_API explicit CBitmapFont (
		string sInFilename );
	// Load a system font
	RENDER_API explicit CBitmapFont (
		string sInFontname,
		int iFontHeight,
		int iFontWeight				= 400,//FW_NORMAL,
		bool bItalic				= false,
		bool bUnderline				= false,
		unsigned long iCharacterSet = 0,//ANSI_CHARSET,
		unsigned int iStartCharacter= 32,
		unsigned int iCharacterNum	= 96 );
	// Destructor
	RENDER_API ~CBitmapFont ( void );

	// Use and unused (exist to output error messages)
	RENDER_API void Bind ( void );

	// Use and unuse (draw list style)
	RENDER_API void Set ( void );
	// Get the active list
	RENDER_API glHandle GetLetterList ( void ) {
		return iLetterList;
	}
	RENDER_API const tBitmapFontInfo& GetFontInfo ( void ) {
		return fontInfo;
	}

	RENDER_API bool GetIsFont ( void ) override
	{
		return true;
	}

	RENDER_API void Reload ( void ) override {
		;
	}
public:
	RENDER_API static CBitmapFont* pActiveFont;

protected:
	void LoadFont ( void );
	void LoadFontAsTexture ( void );
	void LoadFontAsTexture_System ( void );
	void ConvertFontToBitmap ( void );

	void LoadFontFromImage ( void );
	void GeneralDisplayListFromImage ( void );

protected:
	glHandle	iLetterList;
	unsigned int iStartCharacter;
	
	tBitmapFontInfo fontInfo;

private:
	string		fontUniqueSId;	// Unique font ID for storage in the texture master

	Vector2d*	pFontSizes;		// Sizes of characters for converting fonts to bitmaps and for governing UVs
	Vector2d*	pFontOrigins;
	Vector2d*	pFontOffsets;
	Vector2d*	pCharPositions; // Positions of characters UV wise
};

#endif