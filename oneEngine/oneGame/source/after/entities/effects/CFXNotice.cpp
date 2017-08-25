
#include "CFXNotice.h"
#include "renderer/object/shapes/CBillboard.h"
#include "renderer/texture/CTexture.h"

CFXNotice::CFXNotice ( const int ntype, const Vector3d& nposition )
{
	position = nposition;
	timer = 0;
	size = 0.75f;

	m_effect = new CBillboard();
	m_effect->SetSize( size );
	m_effect->transform.position = position;

	/*glMaterial* effect_mat = new glMaterial;
	effect_mat->isTransparent = true;
	if ( ntype == 0 ) effect_mat->loadTexture( "textures/icons/notice.png" );
	if ( ntype == 1 ) effect_mat->loadTexture( "textures/icons/anger.png" );
	m_effect->SetMaterial( effect_mat );
	effect_mat->removeReference();*/

	glMaterial* effect_mat = new glMaterial;
	if ( ntype == 0 ) effect_mat->setTexture( 0, new CTexture("textures/icons/notice.png") );
	if ( ntype == 1 ) effect_mat->setTexture( 0, new CTexture("textures/icons/anger.png") );
	effect_mat->m_diffuse = Color( 1,1,1,1 );
	effect_mat->passinfo.push_back( glPass() );
	effect_mat->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	effect_mat->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );

	m_effect->SetMaterial( effect_mat );
	effect_mat->removeReference();
}

CFXNotice::~CFXNotice ( void )
{
	delete_safe( m_effect );
}


void CFXNotice::Update ( void )
{
	timer += Time::deltaTime;
	if ( timer > 0.8f ) {
		size -= Time::deltaTime*4;
	}
	if ( timer < 1 ) {
		position.z += Time::deltaTime * (1-timer) * 0.6f;
	}

	m_effect->SetSize( size );
	m_effect->transform.position = position;

	if ( size <= 0 ) {
		DeleteObject( this );
	}
}