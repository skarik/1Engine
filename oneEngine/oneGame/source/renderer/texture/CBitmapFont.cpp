
#include "core/debug/console.h"
#include "core/utils/string.h"
#include "core/system/io/FileUtils.h"
#include "core-ext/system/io/Resources.h"
// Include class and structure definition
#include "CBitmapFont.h"
// Include Window definition to get the needed device for font generation
//#include "RrWindow.h"
// Include the material definition for the font conversion code
#include "renderer/material/RrMaterial.h"
// Include texture master class calls
#include "RrTextureMaster.h"

#include "renderer/system/glMainSystem.h"

//#include <stringstream>

using std::cout;
using std::endl;

// Include the Freetype2 Library
#include "fonts/ft2build.h"
#include FT_FREETYPE_H

FT_Library library = NULL;

// == Static Variables ==
CBitmapFont* CBitmapFont::pActiveFont = NULL;

// === Constructor ===
CBitmapFont::CBitmapFont ( string sInFontname,
		int iFontHeight,
		int iFontWeight,
		bool bItalic,
		bool bUnderline,
		unsigned long iCharacterSet,
		unsigned int iStartCharacter,
		unsigned int iCharacterNum )
	: RrTexture ( "_hx_SYSTEM_FONTLOAD" )
{
	if ( library == NULL ) {
		int error;
		error = FT_Init_FreeType( &library );
		if ( error ) {
			debug::Console->PrintError( "Could not initialize freetype library\n" ); 
		}
		else {
			debug::Console->PrintMessage( "Freetype library initialized! :)\n" );
		}
	}

	fontInfo.startCharacter = iStartCharacter;
	fontInfo.setLength		= iCharacterNum;

	fontInfo.width = 0;
	fontInfo.height = iFontHeight;
	fontInfo.weight = iFontWeight;

	fontInfo.italic = bItalic;
	fontInfo.underline = bUnderline;
	
	fontInfo.name = core::utils::string::GetLower(sInFontname);

	fontInfo.isTexture = false;

	fontInfo.fontSizes = NULL;
	fontInfo.fontOrigins = NULL;
	fontInfo.fontOffsets = NULL;
	fontInfo.charPositions = NULL;

	/*std::stringstream tempstream;
	tempstream << "__hx_font_" << fontInfo.name << fontInfo.height << "_" << fontInfo.weight;
	fontUniqueSId = string( tempstream.str() );*/
	fontUniqueSId = "__hx_font_" + fontInfo.name + std::to_string(fontInfo.height) + "_" + std::to_string(fontInfo.weight);

	// Check if the texture has a reference
	//const textureEntry_t* pTextureReference = TextureMaster.GetReference( fontUniqueSId, this );
	const textureEntry_t* pTextureReference = NULL;
	// Texture doesn't exist yet
	if ( pTextureReference == NULL ) 
	{
		LoadFontAsTexture();
		//LoadFont();
		//ConvertFontToBitmap ();

		// Add a reference to the data
		//TextureMaster.AddReference( fontUniqueSId, this )->info.userdata = iLetterList;
		textureEntry_t* pNewTextureReference = TextureMaster.AddReference( fontUniqueSId, this );

		// Save the font info to the user data
		pNewTextureReference->info.userdata = iLetterList;
		tBitmapFontInfo* newInfo = new tBitmapFontInfo;
		(*newInfo) = fontInfo;
		pNewTextureReference->info.userpdata = (void*)newInfo;

		//cout << ((tBitmapFontInfo*)(TextureMaster.GetReference( fontUniqueSId, this )->info.userpdata))->name;
		//cout << ((tBitmapFontInfo*)(pNewTextureReference->info.userpdata))->name;
	}
	else
	{
		// Get the index of the GL texture
		info.index = pTextureReference->info.index;
		// Get the type
		info.type = pTextureReference->info.type;

		// Set it to a texture type font and copy over the data
		fontInfo = *((tBitmapFontInfo*)(pTextureReference->info.userpdata));

		// Set the size, since that may differ
		info.width = pTextureReference->info.width;
		info.height = pTextureReference->info.height;

		// That's all we need, since everything else has been set already.

		// Add a reference to the data
		iLetterList = TextureMaster.AddReference( fontUniqueSId, this )->info.userdata;
	}
}

// === Destructor ===
CBitmapFont::~CBitmapFont ( void )
{
	//glDeleteLists( iLetterList, fontInfo.setLength );
	TextureMaster.RemoveReference( fontUniqueSId, this );
}

// === Bind and Unbind ===
void CBitmapFont::Bind ( void )
{
	if ( fontInfo.isTexture ) {
		pActiveFont = this;
		RrTexture::Bind();
	}
	else {
		cout << "Warning, fool: Cannot use " << (void*)this << " as a texture...is system generated font.\n";
	}
}

// === Set active font ===
void CBitmapFont::Set ( void )
{
	pActiveFont = this;
	/*if ( fontInfo.isTexture )
	{
		Bind();
	}*/
}

// == Load a font using Freetype library ==
void CBitmapFont::LoadFontAsTexture ( void )
{
	GL_ACCESS;

	FT_Face face;
	int error;

	const unsigned int	max_width = (unsigned int)pow(2, ceil(log( fontInfo.height * 16.0f )/log(2.0f))); //(unsigned int)(fontInfo.height * 1.5f);
	unsigned char* grayBitmap;
	Vector2d bitmap_pen = Vector2d( 1,1 );
	unsigned int max_font_height = 0;
			 pCharPositions	= new Vector2d [fontInfo.setLength];	// UV char positions
	Vector2d* pCharSizes	= new Vector2d [fontInfo.setLength];	// UV char sizes
	Vector2d* pCharOffsets	= new Vector2d [fontInfo.setLength];	// Distance from UV char position to character origin
	Vector2d* pCharAdvance	= new Vector2d [fontInfo.setLength];	// The distance to advance the text cursor

	// ============================================
	// == Load font to *grayBitmap with Freetype ==
	grayBitmap = new unsigned char [max_width*max_width];
	memset( grayBitmap,0, max_width*max_width );

	// Create the font face
	error = FT_New_Face( library, core::Resources::PathTo(string("fonts/") + fontInfo.name).c_str(), 0, &face );
	if ( error == FT_Err_Unknown_File_Format ) {
		debug::Console->PrintError( "the font file could be opened and read, but it appears that its font format is unsupported\n" );
		debug::Console->PrintWarning( "loading font from system\n" );
		LoadFontAsTexture_System();
		return;
	} 
	else if ( error ) {
		debug::Console->PrintError( "font file could not be opened or read, or is simply broken\n" );
		debug::Console->PrintWarning( "loading font from system\n" );
		LoadFontAsTexture_System();
		return;
	}
	
	// Set the font face size
	error = FT_Set_Pixel_Sizes( face, 0, fontInfo.height );
	if ( error ) { 
		debug::Console->PrintWarning( "Could not set font size (possibly a bitmap font)\n" );
	}

	// Query the glyphs
	const int padding = 3;
	for ( unsigned int char_index = fontInfo.startCharacter; char_index < fontInfo.startCharacter + fontInfo.setLength; ++char_index )
	{
		// Get glyph
		FT_UInt glyph_index = FT_Get_Char_Index( face, char_index );
		// Load the glyph
		error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT ); 
		if ( error ) {
			debug::Console->PrintWarning( "Skipping char\n" );
			continue;
		}
		// Get the glyph's bitmap
		error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
		if ( error ) {
			debug::Console->PrintWarning( "Skipping char\n" );
			continue;
		}

		// Store origin relative to top left corner of the glyph
		unsigned int i = char_index - fontInfo.startCharacter;
		pCharOffsets[i].x = (face->glyph->metrics.horiBearingX >> 6) * -1.0f;
		pCharOffsets[i].y = (face->glyph->metrics.horiBearingY >> 6) * 1.0f;

		pCharSizes[i].x = (face->glyph->metrics.width >> 6) * 1.0f;
		pCharSizes[i].y = (face->glyph->metrics.height >> 6) * 1.0f;

		pCharAdvance[i].x = (face->glyph->metrics.horiAdvance >> 6) * 1.0f;
		pCharAdvance[i].y = 0.0f;

		//if ( pCharSizes[i].y > max_font_height ) {
		if ( face->glyph->bitmap.rows > (signed)max_font_height ) {
			//max_font_height = (unsigned int)pCharSizes[i].y;
			max_font_height = (unsigned int)face->glyph->bitmap.rows;
		}

		//bitmap_pen.x += pCharSizes[i].x + padding;

		//if ( bitmap_pen.x + pCharSizes[i].x + 2 > max_width ) {
		if ( bitmap_pen.x + face->glyph->bitmap.width + 2 > max_width ) {
			bitmap_pen.x = 1;
			bitmap_pen.y += padding + max_font_height;
			max_font_height = 0;
		}

		pCharPositions[i] = bitmap_pen;

		// Copy bitmap over
		for ( unsigned int y = 0; y < face->glyph->bitmap.rows; ++y ) {
			for ( unsigned int x = 0; x < face->glyph->bitmap.width; ++x ) {
				unsigned int bmapIndex = (unsigned int)((bitmap_pen.x + x) + (bitmap_pen.y + y)*max_width);
				grayBitmap[bmapIndex] = face->glyph->bitmap.buffer[x + y*face->glyph->bitmap.width];
			}
		}

		// Move to next position using current width
		bitmap_pen.x += face->glyph->bitmap.width + padding;
	}
	// Free face now that we're done with it
	FT_Done_Face( face );

	// ===========================================
	// == Convert the font into a proper format ==
	// Set the size pointers
	pFontOrigins	= pCharOffsets;
	pFontOffsets	= pCharAdvance;
	pFontSizes		= pCharSizes;

	// Set the texture format info
	info.type			= Texture2D;
	info.internalFormat	= RGBA8;
	info.width			= max_width;
	info.height			= max_width;
	info.index			= 0;
	info.repeatX		= Clamp;
	info.repeatY		= Clamp;

	// Convert the color data to data we want
	pData = new tPixel [ info.width * info.height ];
	// Set initial data
	for ( unsigned int i = 0; i < info.width * info.height; ++i )
	{
		pData[i].r = 255;
		pData[i].g = 255;
		pData[i].b = 255;
		pData[i].a = 0;
	}

	// Copy over the layout of characters
	for ( unsigned int pixel_index = 0; pixel_index < max_width*max_width; ++pixel_index )
	{
		pData[pixel_index].r = 255;
		pData[pixel_index].g = 255;
		pData[pixel_index].b = 255;
		pData[pixel_index].a = grayBitmap[pixel_index];
	}
	delete [] grayBitmap;
	
	// Send the data to OpenGL
	//info.index = GL.GetNewTexture();
	glGenTextures( 1, &info.index );
	// Bind the texture object
	glBindTexture( GL_TEXTURE_2D, info.index );
	// Set the pack alignment
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	// Change the texture repeat
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL.Enum(info.repeatX) );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL.Enum(info.repeatY) );
	// Change the filtering
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	// Turn off automatic mipmap generation
	//glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
	// Copy the data to the texture object
	glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData );
	// Unbind the data
	glBindTexture( GL_TEXTURE_2D, 0 );

	// Delete the texture data
	delete [] pData;
	pData = NULL;

	fontInfo.isTexture = true;



	for ( unsigned char c = fontInfo.startCharacter; c < fontInfo.startCharacter+fontInfo.setLength; c += 1 )
	{
		unsigned char curChar = c-fontInfo.startCharacter;
		//Vector2d uvPos = (pCharPositions[curChar]) * (1.0f/max_width);
		//Vector2d uvSize = (pFontSizes[curChar]) * (1.0f/max_width);
		//pCharPositions[curChar] *= (1.0f/max_width);
		//pFontSizes[curChar] *= (1.0f/max_width);
		//glTranslatef( ,0);
		//glTranslatef( .x+pFontOrigins[curChar].x,pFontOffsets[curChar].y+pFontOrigins[curChar].y,0);
		Vector2d xyDrawOffset = pFontOrigins[curChar] * -1;
		Vector2d xyPenOffset = pFontOffsets[curChar];
	}

	fontInfo.fontSizes		= pFontSizes;
	fontInfo.fontOrigins	= pFontOrigins;
	fontInfo.fontOffsets	= pFontOffsets;
	fontInfo.charPositions	= pCharPositions;
}

#ifdef _WIN32
//#include "renderer/window/RrWindow.h"
#endif
// == Load a font using the primary way ==
void CBitmapFont::LoadFont ( void )
{
	throw core::InvalidCallException();
#ifdef _WIN32
	/*
	HFONT	font;
	HFONT	oldfont;
	HDC		hDC = RrWindow::pActive->getDevicePointer();
	iLetterList = glGenLists( fontInfo.setLength );

	SetMapMode( hDC, MM_TEXT );
	font = CreateFont(	-fontInfo.height,				// Height Of Font
						fontInfo.width,					// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						fontInfo.weight,				// Font Weight
						fontInfo.italic,				// Italic
						fontInfo.underline,				// Underline
						FALSE,							// Strikeout
						fontInfo.characterSet,			// Character Set Identifier
						OUT_OUTLINE_PRECIS,				// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						CLEARTYPE_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						fontInfo.name.c_str());			// Font Name

	oldfont = (HFONT)SelectObject( hDC, font );			// Selects The Font We Want
	if  ( !wglUseFontBitmaps( hDC,
			fontInfo.startCharacter,
			fontInfo.setLength, iLetterList ) )			// Builds 96 Characters Starting At Character 32
		cout << "Error creating font.\n";
	SelectObject( hDC, oldfont );						// Selects The Font We Want

	DeleteObject( font );								// Delete The Font
	*/
#elif __linux

	FontStruct *font;
    Display * dpy = RrWindow::pActive->getDevicePointer();
    iLetterList = glGenLists( fontInfo.setLength );

    /* load a font with a specific name in "Host Portable Character Encoding" */
    font = XLoadQueryFont( dpy,
        "-*-helvetica-bold-r-normal--24-*-*-*-p-*-iso8859-1");
    if (font == NULL)
    {
        /* this really *should* be available on every X Window System...*/
        font = XLoadQueryFont( dpy, "fixed" );
        if (font == NULL)
        {
            printf("Problems loading fonts :-(\n");
			exit(1); // TODO: REPLACE THIS LINE WITH PROPER ERROR
			//loadDefault();
			//GeneralDisplayListFromImage();
        }
    }
    /* build 96 display lists out of our font starting at char 32 */
    glXUseXFont(font->fid, fontInfo.startCharacter, fontInfo.setLength, iLetterList);
    /* free our XFontStruct since we have our display lists */
    XFreeFont(dpy, font);

#endif
}
//#include <strsafe.h>
// == Load a font by conversion to texture ==
void CBitmapFont::LoadFontAsTexture_System ( void )
{
	string filepath = core::Resources::PathTo( string("fonts/") + fontInfo.name );
	if ( IO::FileExists( filepath ) )
		throw core::InvalidCallException();
	else
		debug::Console->PrintError( "Font file does not exist" );
}

// Takes a loaded font file and draws them to a bitmap, then proceeds to 
void CBitmapFont::ConvertFontToBitmap ( void )
{
	string filepath = core::Resources::PathTo( string("fonts/") + fontInfo.name );
	if ( IO::FileExists( filepath ) )
		throw core::InvalidCallException();
	else
		debug::Console->PrintError( "Font file does not exist" );
}