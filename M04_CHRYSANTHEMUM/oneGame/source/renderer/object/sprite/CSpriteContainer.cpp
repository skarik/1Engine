
#include "CSprite.h"
#include "CSpriteContainer.h"
#include "renderer/system/glMainSystem.h"

// Static Variables
CSpriteContainer* CSpriteContainer::Active = NULL;

// Constructor
CSpriteContainer::CSpriteContainer ( void )
	: CRenderableObject ()
{
	renderType = Renderer::V2D;
	Active = this;
}
// Destructor
CSpriteContainer::~CSpriteContainer ( void )
{
	if ( Active == this )
	{
		Active = NULL;
	}
}

// Add a sprite to be drawn
void CSpriteContainer::AddSprite ( CSprite* pNewSprite )
{
	Active->lSpriteList.insert( Active->lSpriteList.begin(), pNewSprite );
}
// Remove sprite specified
void CSpriteContainer::RemoveSprite ( CSprite* pOldSprite )
{
	//pActive->lSpriteList.insert( pActive->lSpriteList.begin(), pNewSprite );
	/*list<CSprite*>::iterator itrP;
	for ( itrP = pActive->lSpriteList.begin(); itrP != pActive->lSpriteList.end(); itrP++ )
	{
		if ( (*itrP) == pOldSprite )
		{
			pActive->lSpriteList.remove( itrP );
			break;
		}
	}*/

	/*for ( int i = 0; i < pActive->lSpriteList.size(); i )
	{
		if ( pActive->lSpriteList(i) == pOldSprite )
		{
			pActive->lSpriteList.remove( i );
			break;
		}
	}*/
	Active->lSpriteList.remove( pOldSprite );
	
}
// == Render Sprites ==
/*bool CSpriteContainer::Render ( void )
{
	return true;
}*/

// == Render Sprites ==
bool CSpriteContainer::Render ( const char pass )
{
	GL_ACCESS
	//GL.prepareDraw();

	//glClear( GL_DEPTH_BUFFER_BIT );

	GL.beginOrtho();
	std::list<CSprite*>::iterator itrP;
	for ( itrP = Active->lSpriteList.begin(); itrP != Active->lSpriteList.end(); itrP++ )
	{
		if ( (*itrP)->visible )
		{
			GL.prepareDraw();
			(*itrP)->Render();
			GL.cleanupDraw();
		}
	}
	GL.endOrtho();

	//GL.cleanupDraw();
	return true;
}