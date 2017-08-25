
#include "CSkill.h"

#include "engine/physics/collider/CCollider.h"
#include "engine/physics/motion/CRigidbody.h"

#include "after/entities/character/CCharacter.h"
#include "after/entities/character/CAfterPlayer.h"

CSkill::CSkill ( const SkillData & skData ) : CWeaponItem( skData ), tex_icon( NULL )
{
	mana_cost	= 20;
	stamina_cost= 0;
	health_cost	= 0;
	cooldown	= 3.0f;
	skill_lvl	= 0;
	passive		= false;

	icon_filename = weaponItemData.sIconOverride;

	holdType = Item::SpellHanded;

	isCasting = false;
	isChannelling = false;
}

CSkill::~CSkill ( void )
{
	if ( tex_icon ) {
		delete tex_icon;
	}
}
// Serialization system
void CSkill::serialize ( Serializer &ser, const uint ver )
{
	CWeaponItem::serialize( ser,ver );
	ser & skill_lvl;
}

// Skill class copy operator
//	Needs to copy more data than a regular weapon
CWeaponItem& CSkill::operator= ( const CWeaponItem * original )
{
	if ( original->GetIsSkill() )
	{
		CSkill* skill = (CSkill*)original;
		skill_lvl = skill->skill_lvl;
	}

	return CWeaponItem::operator=(original);
}

// Reset the physics object
void CSkill::CreatePhysics ( void )
{
	// Remove any physics objects
	if ( pBody != NULL )
	{
		delete pBody;
		pBody = NULL;
	}
	if ( pCollider != NULL )
	{
		delete pCollider;
		pCollider = NULL;
	}
}

// Update override
void CSkill::Update ( void )
{
	CWeaponItem::Update(); // Perform parent class's update.

	if ( isCasting )
	{
		
		// Check if still is casting
		if ( CanUse(0) && !isChannelling )
		{
			isCasting = false;
		}

		std::vector<CSkill*> t_skillist;
		if ( pOwner && pOwner->IsCharacter() ) {
			t_skillist = ((CCharacter*)pOwner)->GetCastingSkills();
		}
		// Loop through the skills
		for ( uint i = 0; i < t_skillist.size(); ++i )
		{
			CSkill* targetSkill = t_skillist[i];
			if ( targetSkill != this && targetSkill->IsCasting() )
			{
				// This skill is casting too!
				if ( targetSkill->IsChannelling() && !isChannelling )
				{
					if ( !targetSkill->ChannelCombine( this ) ) {
						isCasting = false; // ChannelCombine returned false, stop casting.
					}
				}
				else if ( !targetSkill->IsChannelling() && !isChannelling )
				{
					if ( targetSkill->CastCombine( this ) ) {
						// CastCombine return true. Stop this and targetSkill from casting.
						this->StopCasting();
						targetSkill->StopCasting();
					}
				}
			}
			// End casting check
		}
	}

	// Perform passive code if the skill is passive
	/*if ( passive ) {
		DoPassive();
	}*/
}


void CSkill::SetLevel ( const int new_level )
{
	skill_lvl = new_level;
}

int CSkill::GetLevel ( void )
{
	return skill_lvl;
}

bool	CSkill::CanCast ( void )
{
	//if ( holdState == Holding )
	if ( pOwner != NULL )
	{
		if ( pOwner->GetBaseClassName() == "CActor_Character" ) {
			CCharacter* pOwnerChar = (CCharacter*)pOwner;

			return (
				( pOwnerChar->GetCharStats()->fMana		>= ((ftype)mana_cost) )&&
				( pOwnerChar->GetCharStats()->fStamina	>= ((ftype)stamina_cost) )&&
				( pOwnerChar->GetCharStats()->fHealth	>= ((ftype)health_cost) ) );
		}
	}
	/*else if (( holdState == OnBelt )||( holdState == Hidden ))
	{
		return false;
	}*/
	return true;
}

void	CSkill::CastMana ( void )
{
	if ( holdState == Item::Holding )
	{
		if ( pOwner->GetBaseClassName() == "CActor_Character" ) {
			CCharacter* pOwnerChar = (CCharacter*)pOwner;
			pOwnerChar->GetCharStats()->fMana	-= ((ftype)mana_cost);
			pOwnerChar->GetCharStats()->fStamina-= ((ftype)stamina_cost);
			pOwnerChar->GetCharStats()->fHealth	-= ((ftype)health_cost);
		}
		isCasting = true;
	}
}

// Gets the focus color from the player stats and returns it
Color	CSkill::GetFocusColor ( void )
{
	if ( pOwner != NULL )
	{
		/*if ( pOwner->GetTypeName() == "CPlayer" )
		{
			CMccPlayer* pl = (CMccPlayer*)pOwner;
			CPlayerStats* pl_stats = pl->GetPlayerStats();*/
		CRacialStats* race_stats = pOwner->GetRacialStats();
		if ( race_stats != NULL )
		{
			Color baseColor = race_stats->cFocusColor;	// STRENGTHEN THE COLOR
			ftype centerPoint = (baseColor.red*0.299f + baseColor.green*0.587f + baseColor.blue*0.114f) * 0.5f + 0.25f;
			baseColor.red += (baseColor.red - centerPoint)*3.0f;
			baseColor.green += (baseColor.green - centerPoint)*3.0f;
			baseColor.blue += (baseColor.blue - centerPoint)*3.0f;
			baseColor.alpha = (baseColor.alpha + 1.0f)*0.5f;
			return baseColor;
		}
	}
	return Color( 0,0,0,0.0f );
}

#include "renderer/texture/CTexture.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/material/glMaterial.h"
#include "core/system/Screen.h"

// ================================================
// HUD Element Rendering
// ================================================
#include "core/settings/CGameSettings.h"
// Render HUD elements
void CSkill::OnDrawHUD ( const WReticleStyle& n_reticleStyle )
{
	// Draw default reticle
	n_reticleStyle.matReticle->m_diffuse = Color( CanCast()?1.0f:0.4f,CanCast()?1.0f:0.4f,1,CanUse(Item::UPrimary)?1.0f:0.4f );
	n_reticleStyle.matReticle->bindPass(0);

	Vector2d centerPosition ( Screen::Info.width/2.0f,Screen::Info.height/2.0f );
	Vector2d reticleHalfsize ( 11,11 );
	reticleHalfsize *= CGameSettings::Active()->f_cl_ReticleScale;
	if ( n_reticleStyle.hand_id%2 == 0 ) {
		centerPosition.x -= 5 * CGameSettings::Active()->f_cl_ReticleScale;
		centerPosition.y -= 3 * CGameSettings::Active()->f_cl_ReticleScale;
	}
	else {
		centerPosition.x += 5 * CGameSettings::Active()->f_cl_ReticleScale;
		centerPosition.y += 3 * CGameSettings::Active()->f_cl_ReticleScale;
	}

	GLd_ACCESS;
	// Draw default reticle
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

// ================================================
// Icon rendering code
// ================================================

// Grabs the icon for this object
CTexture* CSkill::GetInventoryIcon ( void )
{
	const int iconW = 64;
	const int iconH = 64;
	static CTexture* nullTex = NULL;
	if ( !icon.bGenerated )
	{
		
		{	// Create render texture
			icon.pRenderTex = new CRenderTexture( RGBA8, iconW,iconH, Clamp,Clamp, Texture2D );
		}
		{	// Load the icon
			if ( icon_filename.length() > 0 ) {
				tex_icon = new CTexture( icon_filename );
			}
		}
		{
			if ( nullTex == NULL ) {
				nullTex = new CTexture( "null" );
			}
		}
		icon.bGenerated = true;
	}

	// Actually render down here
	{
		glMaterial*	previousMaterial	= glMaterial::current;
		uchar		previousPass		= glMaterial::current_pass;
		GL_ACCESS;
		{	// Bind buffer
			icon.pRenderTex->BindBuffer();
		}
		
		{	// Set up the rendering thing
			GL.setupViewport( 0,0,64,64 ); // DirectX
		}

		ftype maxCooldown = GetCooldown(0);
		for ( uint i = 1; i < 9; ++i ) {
			if ( GetCooldown(i) > maxCooldown ) {
				maxCooldown = GetCooldown(i);
			}
		}
		/*{	// Change the projection to orthographic
			glMatrixMode( GL_PROJECTION );
			glPushMatrix();
			glLoadIdentity();
			glOrtho( 0, iconW,iconH, 0, -45.0, 45.0 ); 
			// Reset the model view matrix
			glMatrixMode( GL_MODELVIEW );
			glPushMatrix();
			glLoadIdentity();
		}*/
		GL.beginOrtho( 0,0, (ftype)iconW, (ftype)iconH, -45.0f, 45.0f, false );

		GL.pushModelMatrix( Matrix4x4() );


		glMaterial drawMat;
		drawMat.passinfo.push_back( glPass() );
		drawMat.passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );
		drawMat.passinfo[0].m_transparency_mode	= Renderer::ALPHAMODE_TRANSLUCENT;
		drawMat.passinfo[0].m_lighting_mode		= Renderer::LI_NONE;
		drawMat.passinfo[0].m_face_mode			= Renderer::FM_FRONTANDBACK;
		drawMat.removeReference();

		// Render and shit
		{
			/*glClearColor( 0.0f, 0.0f, 0.2f, 1.0f );
			glDepthMask( GL_TRUE );
			glClear( GL_COLOR_BUFFER_BIT );
			glClear( GL_DEPTH_BUFFER_BIT );*/
			//glCullFace( GL_BACK );
			GLd_ACCESS;
			if ( tex_icon )
			{
				//glColor4f( 1.0f,1.0f,1.0f,1.0f );
				drawMat.setTexture( 0, tex_icon );
				drawMat.m_diffuse = Color( 1,1,1,1 );
				//tex_icon->Bind();
				drawMat.bindPass(0);
				GLd.DrawRectangle( 0,0,iconW,iconH );
				//tex_icon->Unbind();
			}
			drawMat.setTexture( 0, nullTex );
			if ( !CanCast() )
			{
				drawMat.m_diffuse = Color( 0.20f,0.15f,0.50f, 1.0 );
				drawMat.bindPass(0);
				//glBlendFunc( GL_ZERO, GL_SRC_COLOR );
				GL.BlendMode( BM_ZERO, BM_SRC_COLOR );
				GLd.DrawRectangle( 0,0,iconW,iconH );
				//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			}

			//glColor4f( 0.5f,0.5f,0.5f,1.0f );
			drawMat.m_diffuse = Color( 0.5,0.5,0.5, 1.0 );
			drawMat.bindPass(0);
			GLd.DrawRectangle( 0,0,iconW,(int)(iconH*(maxCooldown/cooldown)) );

			//glColor4f( 1.0f,1.0f,1.0f,1.0f );

			//glCullFace( GL_BACK );
		}
		
		GL.popModelMatrix();
		/*{	// Get the old projection matrix
			glMatrixMode( GL_PROJECTION );
			glPopMatrix();
			// Go back to modelview matrix
			glMatrixMode( GL_MODELVIEW );
			glPopMatrix();
		}*/
		GL.endOrtho();

		{	// Unbind buffer
			icon.pRenderTex->UnbindBuffer();
		}

		{	// Viewport
			GL.setupViewport( 0,0,Screen::Info.width,Screen::Info.height );
		}

		// Restore previous material
		previousMaterial->bindPass( previousPass );
	}

	// Return icon index
	return icon.pRenderTex;
}

#include "renderer/texture/CBitmapFont.h"
void CSkill::DrawTooltip ( const Vector2d & drawPos, const WTooltipStyle & style )
{
	//CWeaponItem::DrawTooltip( drawPos, style );
	TooltipDrawBackground( drawPos,style );
	TooltipDrawName( drawPos,style );
	GLd_ACCESS;
	style.matFont->setTexture( 0, style.fontTexture );
	style.matFont->bindPass(0);
	GLd.DrawAutoTextWrapped( drawPos.x+0.01f, drawPos.y+0.10f, 0.2f, "Level: %d\nMana cost: %.0f\nCooldown: %.2fs", skill_lvl, mana_cost, cooldown );
}