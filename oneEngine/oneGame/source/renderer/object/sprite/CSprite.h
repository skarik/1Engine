// Sprites are nice and interesting in that they take care of all their material stuff on their own.
// Everything related to drawing sprites is defined in the sprite, making easy 2D drawing without
//  needing any material drawing.

#ifndef _C_SPRITE_H_
#define _C_SPRITE_H_

// Includes
//#include "glMainSystem.h"
//#include "RrMaterial.h"
//#include "renderer/texture/CTexture.h"
#include "core/types/float.h"
#include "core/math/Vector2d.h"
#include "core/math/Color.h"

// Include screen properties
#include "core/system/Screen.h"

#include "renderer/types/ObjectSettings.h"

#include <string>
using std::string;

class RrMaterial;
class CTexture;

// Class definition
class CSprite
{
public:
	// Constructor and Destructor
	RENDER_API CSprite ( void );
	RENDER_API ~CSprite( void );

	RENDER_API void LoadSprite ( string sSpriteFile );
	RENDER_API void LoadSprite ( string sImageFile,
		Vector2d targetSize,
		bool isAnimated = false,
		int	numFrames = 0
		);

	RENDER_API void Render ( void );
public:
	Vector2d position;
	Vector2d scale;
	Real	rotation;
	Color	blend;
	Real	depth;
	bool	visible;

	renderer::eDrawBlendMode blendmode;

public:
	// == Math Checker Functions ==
	RENDER_API bool	ContainsPoint ( Vector2d );

	RrMaterial*	GetMaterial ( void ) {
		return myMaterial;
	}
private:
	Vector2d size;

	RrMaterial*	myMaterial;
	CTexture*	myTexture;
};

#endif