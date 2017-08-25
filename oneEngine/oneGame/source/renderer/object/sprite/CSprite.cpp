
#include "CSprite.h"
#include "CSpriteContainer.h"

#include "renderer/material/RrMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

// == Constructor and Destructor ==
CSprite::CSprite ( void )
{
	SpriteContainer::AddSprite( this );

	position	= Vector2d( 0,0 );
	scale		= Vector2d( 1,1 );
	rotation	= 0;
	size		= Vector2d( 64,64 );
	blend		= Color( 1,1,1, 1 );
	depth		= 0;
	visible		= true;
	blendmode	= renderer::BM_NORMAL;

	myMaterial	= new RrMaterial ();
	myMaterial->passinfo.push_back( RrPassForward() );
	myMaterial->passinfo[0].shader = new RrShader( ".res/shaders/v2d/default.glsl" );
	myTexture	= NULL;
}
CSprite::~CSprite ( void )
{
	SpriteContainer::RemoveSprite( this );

	//myMaterial->pTextures = NULL;
	myMaterial->removeReference();
	delete myMaterial;
	if ( myTexture != NULL ) {
		delete myTexture;
	}
}

// == Drawing Sprite ==
void CSprite::Render ( void )
{
	GL_ACCESS GLd_ACCESS
/*
	glTranslatef( position.x,position.y,depth );
	glRotatef( rotation, 0,0,1 );
	glScalef( scale.x,scale.y, 1 );
	*/
	//GLdefaultMaterial.bind();
	//glColor4f( blend.red,blend.green,blend.blue,blend.alpha );

	myMaterial->m_diffuse = blend;
	myMaterial->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	//myMaterial->useColors = true;
	myMaterial->passinfo[0].m_blend_mode = blendmode;
	if ( myTexture != NULL )
	{
		//myMaterial->useTexture = true;
		//myMaterial->pTextures[0] = myTexture;
		myMaterial->setTexture( TEX_MAIN, myTexture );
	}
	else
	{
		//myMaterial->useTexture = false;
		
	}
	myMaterial->bindPass(0);
	myMaterial->setShaderConstants(0);

	GLd.BeginPrimitive(GL_QUADS);
		GLd.P_PushTexcoord(0, 0); GLd.P_AddVertex(0, 0);
		GLd.P_PushTexcoord(0, 1); GLd.P_AddVertex(0, size.y);
		GLd.P_PushTexcoord(1, 1); GLd.P_AddVertex(size.x, size.y);
		GLd.P_PushTexcoord(1, 0); GLd.P_AddVertex(size.x, 0);
	GLd.EndPrimitive();

	//myMaterial->unbind();
}

// == Loading Sprite ==
void CSprite::LoadSprite ( string sImageFile,
		Vector2d targetSize,
		bool isAnimated,
		int	numFrames
		)
{
	myTexture = new CTexture( sImageFile );
	size = targetSize;
}