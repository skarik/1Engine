
#ifndef _RENDER2D_C_TEXTMESH_H_
#define _RENDER2D_C_TEXTMESH_H_

#include "CRenderable2D.h"

class CBitmapFont;

class CTextMesh : public CRenderable2D
{
public:
	RENDER2D_API explicit	CTextMesh ( void );
	RENDER2D_API virtual	~CTextMesh ();

	//		SetFont ( ) 
	// Loads a font as a texture for use with this text mesh
	RENDER2D_API void		SetFont ( const char* fontFile, int fontSize, int fontWeight );

	//		UpdateText ( ) 
	// Updates the mesh with the text in m_text
	RENDER2D_API void		UpdateText ( void );

	//		GetLineHeight ( )
	// Returns approximate line height based on letter M, plus 3 pixels
	RENDER2D_API Real		GetLineHeight ( void );

public:
	std::string		m_text;
	float			m_max_width;

protected:
	CBitmapFont*	m_font_texture;
	size_t			m_text_triangle_count;
};

#endif//_RENDER2D_C_TEXTMESH_H_