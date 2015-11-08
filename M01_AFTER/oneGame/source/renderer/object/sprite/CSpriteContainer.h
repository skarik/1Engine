
#ifndef	_C_SPRITE_CONTAINER_H_
#define	_C_SPRITE_CONTAINER_H_

// Includes
//#include "CSprite.h"
#include "renderer/object/CRenderableObject.h"
//#include "glMainSystem.h"
#include <list>

// Using: Bring list into current scope
//using std::list;
class CRenderState;
class CSprite;

// Class Definition
class CSpriteContainer : public CRenderableObject
{
public:
	CSpriteContainer ( void );
	~CSpriteContainer( void );

public:
	static void AddSprite ( CSprite* );
	static void RemoveSprite ( CSprite* );

	std::list<CSprite*> lSpriteList;

public:
	// Drawing
	bool Render ( const char pass );
	//bool PostRender ( void ) ;

private:
	friend CRenderState;
	static CSpriteContainer* Active;
};

typedef CSpriteContainer SpriteContainer;

#endif