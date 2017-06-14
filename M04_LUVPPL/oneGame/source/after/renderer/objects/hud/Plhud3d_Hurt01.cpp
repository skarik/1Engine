
#include "core/math/Math.h"

#include "renderer/logic/model/CModel.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/glMaterial.h"

#include "Plhud3d_Hurt01.h"

Plhud3d_Hurt01::Plhud3d_Hurt01 ( const Real n_hurtValue, const Rotator n_rotation )
	: CGameBehavior(), m_hurtValue( n_hurtValue )
{
	m_model = new CModel ( "models/hud/hurt_ring.FBX" );
	m_model->transform.rotation = n_rotation;
	m_model->SetRenderType( Renderer::Foreground );
	m_model->transform.scale.z = 0.8f;
}

Plhud3d_Hurt01::~Plhud3d_Hurt01 ( void )
{
	if ( m_model ) {
		delete m_model;
		m_model = NULL;
	}
}


void Plhud3d_Hurt01::Update ( void )
{
	if ( m_hurtValue > 15.0f ) {
		m_hurtValue -= Time::deltaTime * 7.0f;
	}
	if ( m_hurtValue > 5.0f ) {
		m_hurtValue -= Time::deltaTime * 4.0f;
	}
	else {
		m_hurtValue -= Time::deltaTime * 2.0f;
	}
	if ( m_hurtValue <= 0.0f ) {
		DeleteObject( this );
	}
}

void Plhud3d_Hurt01::LateUpdate ( void )
{
	if ( CCamera::activeCamera && m_model )
	{
		m_model->transform.position = CCamera::activeCamera->transform.position + CCamera::activeCamera->transform.rotation * Vector3d( 1.2f,0,-0.4f );
		// Fade against damage value
		if ( m_hurtValue > 0 )
		{
			Real fadeValue = Math.Clamp( sqrt(m_hurtValue)/4.0f, 0.0f, 1.0f );
			m_model->GetMaterial()->m_diffuse = Color(2,2,2,1) * fadeValue;
		}
	}
}