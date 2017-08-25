
#ifndef _C_R_MAIN_MENU_TITLE_H_BITCH_
#define _C_R_MAIN_MENU_TITLE_H_BITCH_

#include "core/containers/arstring.h"
#include "renderer/object/CRenderableObject.h"

class CBitmapFont;

class C_RMainMenuTitle : public CRenderableObject
{

public:
				C_RMainMenuTitle ( void );
				~C_RMainMenuTitle ( void );


	bool		Render ( const char pass );

private:
	arstring<128>	sTagline;
	void		GenerateTagline ( void );


	CBitmapFont*	fntTitle;
	CBitmapFont*	fntTagline;

	glMaterial*		matText;

};


#endif//_C_R_MAIN_MENU_TITLE_H_