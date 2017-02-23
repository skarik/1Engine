
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

public:
	std::string		m_text;

protected:
	CBitmapFont*	m_font_texture;
};

#endif//_RENDER2D_C_TEXTMESH_H_