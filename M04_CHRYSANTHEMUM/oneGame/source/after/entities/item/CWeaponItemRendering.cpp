
#include "CWeaponItem.h"

#include "core/time/time.h"
#include "core/system/Screen.h"

#include "after/entities/character/CCharacter.h"

#include "renderer/material/glMaterial.h"
#include "renderer/light/CLight.h"
#include "renderer/logic/model/CModel.h"

#include "renderer/state/CRenderState.h"
#include "renderer/state/Settings.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "renderer/texture/CTexture.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/texture/CBitmapFont.h"

#include "engine/physics/collider/CCollider.h"
#include "engine/physics/motion/CRigidbody.h"

#include "engine-common/entities/CActor.h"

// ================================================
// Icon rendering code
// ================================================


// Destructor for freeing up owned pointers
CWeaponItem::~CWeaponItem ( void )
{
	// Unequip on destruction
	if ( pOwner && holdState == Item::Holding ) {
		OnUnequip( pOwner );
	}

	// Delete my child objects
	if ( pBody )
		delete pBody;
	pBody = NULL;
	if ( pCollider )
		delete pCollider;
	pCollider = NULL;
	if ( pModel )
		delete pModel;
	pModel = NULL;

	// Delete my icon
	if ( icon.pRenderTex ) {
		delete icon.pRenderTex;
		icon.pRenderTex = NULL;
		icon.bGenerated = false;
	}
}

// Sets the transform to set up a camera
void CWeaponItem::SetInventoryCameraTransform ( XTransform & inTransform )
{
	// Set camera
	/*inTransform.position = transform.position;
	inTransform.rotation = transform.rotation.getEulerAngles() + Vector3d( 45,45,45 );
	inTransform.position -= inTransform.rotation * Vector3d( 5,0,0 );*/
	/*inTransform.rotation = Vector3d( 45,-45,45 );
	//inTransform.position = inTransform.rotation * Vector3d( 5,0,0 );
	inTransform.position = Vector3d( -2.5,2.5,2.5 );*/

	/*Matrix4x4 rotMatx;
	rotMatx.setRotation( myCamera->transform.rotation );
	myCamera->transform.position = rotMatx*Vector3d( -cameraDistance,0,0 );*/

	//inTransform.rotation = Vector3d( 0,45,45 );
	//inTransform.rotation = Vector3d( 90,-45,-45 );
	inTransform.rotation = Rotator( 90,0,0 ) * Rotator( -45,-45,0 );
	// Start position at center of model bounding box
	if ( pModel ) {
		inTransform.position = pModel->GetBoundingBox().GetCenterPoint();
	}
	else {
		inTransform.position = Vector3d(0,0,0);
	}
	//inTransform.position -= Vector3d( 3,0,0 ).rvrMultMatx(!inTransform.rotation);
	//inTransform.position -= inTransform.rotation * Vector3d( 5,0,0 );
	//inTransform.position = Vector3d(0,0,0);
}
void CWeaponItem::SetInventoryCameraWidth ( ftype & inWidth )
{
	if ( pModel ) {
		Vector3d bbox_size = pModel->GetBoundingBox().GetSize();
		inWidth = std::max<ftype>( bbox_size.x, bbox_size.y );
		inWidth = std::max<ftype>( inWidth, bbox_size.z );
		//inWidth *= 0.866f;
		inWidth *= 0.577f;
	}
	else {
		inWidth = 1.5f;
	}
	//inWidth = 4.0f;
}
// Grabs the icon for this object
CTexture* CWeaponItem::GetInventoryIcon ( void )
{
	if ( icon.bGenerated )
	{
		return icon.pRenderTex;
	}
	else
	{
		// Icon needs to be rendered

		if ( icon.pRenderTex == NULL )
		{	// Create render texture
			icon.pRenderTex = new CRenderTexture( RGBA8, 64,64, Clamp,Clamp, Texture2D );
		}

		glMaterial* prevMat = glMaterial::current;
		uchar prevPass = glMaterial::current_pass;
		
		GL_ACCESS;
		
		{	// Set up the rendering thing
			GL.setupViewport( 0,0,64,64 ); // DirectX
			//GL.setupViewport( 0,64,64,-64 ); // OpenGL
		}

		Matrix4x4 projTransform, viewTransform;
		Vector3d offset;
		{	// Setup modelview matrix
			// Switch to model view

			// Get target camera transform
			XTransform camtrans; // todo: replace with XTransform
			SetInventoryCameraTransform( camtrans );

			// Rotate camera
			Matrix4x4 temp;
			temp.setRotation( camtrans.rotation );

			Matrix4x4 translation;
			translation.setTranslation( camtrans.position );
			offset = !temp * camtrans.position;

			viewTransform = temp;//temp * translation;
		}

		{	// Set up the projection matrix
			// Select The Projection Matrix
			ftype vsize;// = 1.5f;
			SetInventoryCameraWidth( vsize );
			GL.beginOrtho( -vsize + offset.x,vsize + offset.y, vsize*2,-vsize*2, -20.0f, 20.0f );
			projTransform = GL.getProjection();
		}
		GL.pushProjection( viewTransform * projTransform );
		GL.pushModelMatrix( Matrix4x4() );

		{	// Bind buffer
			icon.pRenderTex->BindBuffer();
		}

		// Draw stuff
		{
			GL.clearColor( Color(0.0f, 0.0f, 0.0f, 0.0f) );
			GL.EnableDepthWrite();
			GL.ClearBuffer( GL_COLOR_BUFFER_BIT );
			GL.ClearBuffer( GL_DEPTH_BUFFER_BIT );

			InventoryIconRenderModel();
		}

		{	// Unbind buffer
			icon.pRenderTex->UnbindBuffer();
		}
		GL.popProjection();
		GL.popModelMatrix();

		{	// Select The Projection Matrix
			GL.endOrtho();
		}

		{	// Viewport
			GL.setupViewport( 0,0,Screen::Info.width,Screen::Info.height );
		}

		if ( pModel ) // If the model exists
		{	// then the icon is ready
			icon.bGenerated = true;
		}

		if ( prevMat ) {
			prevMat->bindPass(prevPass);
		}

		return icon.pRenderTex;
	}
}

void CWeaponItem::InventoryIconRenderModel ( void )
{
	if ( pModel )
	{
		pModel->SetVisibility( true );
		pModel->SetForcedDraw();

		pModel->transform.position = Vector3d( 0,0,0 );
		pModel->transform.rotation = Vector3d( 0,0,0 );
		pModel->transform.scale = Vector3d( 1,1,1 );

		// Create light list
		static CLight* light = NULL;
		if ( light == NULL ) { 
			light = new CLight();
			light->transform.position = Vector3d( 3,3,3 );
			light->diffuseColor = Color( 0.8f, 0.8f, 0.8f );
			light->range = 20.0f;
			light->falloff = 3.0f;
		}
		light->SetActive( true );
		std::vector<CLight*> lightList;
		lightList.push_back( light );

		// Set render settings
		Renderer::renderSettings_t oldRenderSettings = Renderer::Settings;
		Renderer::Settings.ambientColor = Color( 0.6f,0.6f,0.6f );
		Renderer::Settings.fogStart = 10.0f;
		Renderer::Settings.fogEnd = 100.0f;
		Renderer::Settings.fogColor = Color( 1.0f,1.0f,1.0f );
		Renderer::Settings.clearColor = Color( 0.7f,0.7f,0.8f,0.0f );

		// Render item
		SceneRenderer->PreRenderSetLighting( lightList );
		pModel->SetForcedDraw();
		for ( uint i = 0; i < pModel->GetMeshCount(); ++i ) {
			SceneRenderer->RenderSingleObject( (CRenderableObject*) pModel->GetMesh(i) );
		}

		// Clear light
		light->SetActive( false );

		// Reset render setings
		GLd_ACCESS;
		Renderer::Settings = oldRenderSettings;
		GLd.BeginPrimitive( GL_POINTS );
		// Loop through all the particles and render
		for ( float x = 0; x < 3; x+=0.05f )
		{
			GLd.P_PushColor( 1.0f,1.0f, 0.0f, 1.0f );
			GLd.P_AddVertex( x,0,0 );
			GLd.P_AddVertex( 0,x,0 );
			GLd.P_AddVertex( 0,0,x );
		}
		GLd.EndPrimitive();
	}
}


// Destructor
CWeaponItem::WItemIcon::~WItemIcon ( void )
{
	if ( pRenderTex )
		delete pRenderTex;
}

// Draw the tooltip
void CWeaponItem::DrawTooltip ( const Vector2d & drawPos, const WTooltipStyle & style )
{
	TooltipDrawBackground( drawPos,style );
	TooltipDrawName( drawPos,style );
	TooltipDrawRarity( drawPos,style );
	TooltipDrawDescription( drawPos,style );
	TooltipDrawPicture( drawPos,style );
}
void	CWeaponItem::TooltipDrawBackground ( const Vector2d & drawPos, const WTooltipStyle & style )
{
	GLd_ACCESS;
	style.matBg->bindPass(0);
	GLd.DrawRectangleA( drawPos.x, drawPos.y, 0.4f,0.2f );
}
void	CWeaponItem::TooltipDrawName ( const Vector2d & drawPos, const WTooltipStyle & style )
{
	GLd_ACCESS;
	style.matFont->setTexture(0,style.fontTexture);
	style.matFont->bindPass(0);
	GLd.DrawAutoText( drawPos.x+0.01f, drawPos.y+0.02f, "%s", weaponItemState.sItemName.c_str() );
}
void	CWeaponItem::TooltipDrawRarity ( const Vector2d & drawPos, const WTooltipStyle & style )
{
	GLd_ACCESS;
	style.matFont->setTexture(0,style.fontTexture);
	style.matFont->bindPass(0);
	switch ( weaponItemData.eItemRarity ) {
		case WeaponItem::RarityNone:		GLd.DrawAutoText( drawPos.x+0.02f, drawPos.y+0.05f, "Unremarkable" ); break;
		case WeaponItem::RarityCommon:		GLd.DrawAutoText( drawPos.x+0.02f, drawPos.y+0.05f, "Common" ); break;
		case WeaponItem::RarityUncommon:	GLd.DrawAutoText( drawPos.x+0.02f, drawPos.y+0.05f, "Uncommon" ); break;
		case WeaponItem::RarityRare:		GLd.DrawAutoText( drawPos.x+0.02f, drawPos.y+0.05f, "Rare" ); break;
		case WeaponItem::RarityMythical:	GLd.DrawAutoText( drawPos.x+0.02f, drawPos.y+0.05f, "Mythical" ); break;
		case WeaponItem::RarityHeroic:		GLd.DrawAutoText( drawPos.x+0.02f, drawPos.y+0.05f, "Heroic" ); break;
		case WeaponItem::RarityVillainous:	GLd.DrawAutoText( drawPos.x+0.02f, drawPos.y+0.05f, "Villainous" ); break;
		case WeaponItem::RarityGeneric:		GLd.DrawAutoText( drawPos.x+0.02f, drawPos.y+0.05f, "Generic" ); break;
		case WeaponItem::RarityNewLegend:	GLd.DrawAutoText( drawPos.x+0.02f, drawPos.y+0.05f, "New Legend" ); break;
	}
}
void	CWeaponItem::TooltipDrawDescription ( const Vector2d & drawPos, const WTooltipStyle & style )
{
	GLd_ACCESS;
	style.matFont->setTexture(0,style.fontTexture);
	style.matFont->bindPass(0);
	GLd.DrawAutoTextWrapped( drawPos.x+0.01f, drawPos.y+0.08f, 0.38f, weaponItemData.sInfo.c_str() );
}
void	CWeaponItem::TooltipDrawPicture ( const Vector2d & drawPos, const WTooltipStyle & style )
{
}

// ================================================
// HUD Element Rendering
// ================================================
#include "core/settings/CGameSettings.h"
// Render HUD elements
void CWeaponItem::OnDrawHUD ( const WReticleStyle& n_reticleStyle )
{
	// Draw default reticle
	n_reticleStyle.matReticle->m_diffuse = Color( 1,1,1,0.5f );
	n_reticleStyle.matReticle->bindPass(0);

	Vector2d centerPosition ( Screen::Info.width/2.0f,Screen::Info.height/2.0f );
	Vector2d reticleHalfsize ( 16,16 );
	reticleHalfsize *= CGameSettings::Active()->f_cl_ReticleScale;
	GLd_ACCESS;
	GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
		GLd.P_PushColor( Color(1,1,1,1) );
		GLd.P_PushTexcoord( Vector2d(0,0)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(-reticleHalfsize.x,-reticleHalfsize.y) );
		GLd.P_PushTexcoord( Vector2d(0,1)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(-reticleHalfsize.x,reticleHalfsize.y) );
		GLd.P_PushTexcoord( Vector2d(1,0)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(reticleHalfsize.x,-reticleHalfsize.y) );
		GLd.P_PushTexcoord( Vector2d(1,1)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(reticleHalfsize.x,reticleHalfsize.y) );
	GLd.EndPrimitive();

	GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
		GLd.P_PushColor( Color(1,1,1,1) );
		GLd.P_PushTexcoord( Vector2d(1,0)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(-reticleHalfsize.x,-reticleHalfsize.y) );
		GLd.P_PushTexcoord( Vector2d(1,1)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(-reticleHalfsize.x,reticleHalfsize.y) );
		GLd.P_PushTexcoord( Vector2d(2,0)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(reticleHalfsize.x,-reticleHalfsize.y) );
		GLd.P_PushTexcoord( Vector2d(2,1)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(reticleHalfsize.x,reticleHalfsize.y) );
	GLd.EndPrimitive();
}