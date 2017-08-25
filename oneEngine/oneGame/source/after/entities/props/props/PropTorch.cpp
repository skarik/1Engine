
#include "PropTorch.h"
#include "after/entities/item/props/ItemTorch.h"

#include "renderer/light/CLight.h"
#include "engine-common/entities/CParticleSystem.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"

// Constructor (on terrain)
PropTorch::PropTorch ( BlockTrackInfo const& inInfo )
	: CTerrainProp( inInfo )
{
	pLight = new CLight();

	m_model = new CModel( string("models/items/torch_1.FBX") );
	m_model->transform.scale = Vector3d( 1.3f,1.3f,1.3f );

	pFireSystem = new CParticleSystem ( ".res/particlesystems/flame01.pcf" );
	glMaterial* mat = new glMaterial();
	mat->passinfo.push_back( glPass() );
	mat->passinfo[0].m_blend_mode = Renderer::BM_ADD;
	mat->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	mat->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	mat->passinfo[0].shader = new glShader( ".res/shaders/particles/colorBlended.glsl" );
	mat->setTexture( 0, new CTexture(".res/textures/particles/fluxflame1.png") );
	mat->removeReference();
	pFireSystem->GetRenderable()->SetMaterial( mat );
}
// Destructor
PropTorch::~PropTorch ( void )
{
	if ( pLight != NULL )
		delete pLight;
	pLight = NULL;
	if ( pFireSystem != NULL )
		delete pFireSystem;
	pFireSystem = NULL;
}

// Update
void PropTorch::Update ( void )
{
	pLight->diffuseColor = Color( 0.71f, 0.55f, 0.24f ) * random_range( 1.2f,1.3f );
	pLight->range = 40.0f * random_range( 1.2f,1.3f );
	pLight->falloff = 3.0f;
	pLight->pass = 0.2f;

	pLight->drawHalo = true;

	pLight->transform.position = transform.position + transform.rotation*Vector3d( 0,0,0.5f );

	pFireSystem->transform.position = transform.position + transform.rotation*Vector3d( 0,0,0.5f );

	if ( this->mBlockInfo.pBlock->block == 0 ) {
		ItemTorch* pNewItem = new ItemTorch();
		pNewItem->transform.Get( transform );

		DeleteObject( this );
	}
}

// Lookats create some outlines
void	PropTorch::OnInteractLookAt	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		m_modelGlow->SetVisible( true );
		m_modelGlow->m_color = Color( 1.0f,1,1 );
		m_modelGlow->m_width = 0.08f;
	}
}
void	PropTorch::OnInteractLookAway	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		m_modelGlow->SetVisible( false );
	}
}

// On Punched. Is executed when an ItemBlockPuncher or derived hits this object.
void PropTorch::OnPunched ( RaycastHit const& hitInfo )
{
	ItemTorch* pNewItem = new ItemTorch();
	pNewItem->transform.Get( transform );

	DeleteObject( this );
}