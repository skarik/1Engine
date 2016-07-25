
#include "CloudSphere.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/glMaterial.h"
#include "renderer/logic/model/CModel.h"
#include "renderer/texture/CTexture.h"

CloudSphere::CloudSphere ( void ) : CGameBehavior()
{
	// Cloudsphere
	pCloudMat = new glMaterial();
	pCloudMat->setTexture( 0, new CTexture( ".res/textures/cloudmap_hf.png" ) );
	pCloudMat->passinfo.push_back( glPass() );
	pCloudMat->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	pCloudMat->passinfo[0].b_depthmask = false;
	pCloudMat->passinfo[0].shader = new glShader ( ".res/shaders/sky/cloud_stepped.glsl" );
	pCloudMat->removeReference();

	cloudModel = new CModel ( "models/geosphere.FBX" );
	cloudModel->SetMaterial( pCloudMat );
	cloudModel->SetRenderType( Renderer::Secondary );
	cloudModel->transform.scale = Vector3d( 300.0,300.0f,-300.0f );
	cloudModel->SetFrustumCulling( false );

	//cloudModel->SetVisibility( false ); // Disable clouds for now

	cloudDensity = 0;
}

CloudSphere::~CloudSphere ( void )
{
	delete cloudModel;
}

void CloudSphere::PostUpdate ( void )
{
	if ( CCamera::activeCamera ) {
		cloudModel->transform.position = CCamera::activeCamera->transform.position;
	}
	/*pCloudMat->setUniform( "gm_Datettime", timeofDay/3600 );
	pCloudMat->setUniform( "gm_Stormvalue", cloudDensity );*/
	cloudModel->SetShaderUniform( "gm_Datettime", timeofDay/3600 );
	cloudModel->SetShaderUniform( "gm_Stormvalue", cloudDensity );
}

void CloudSphere::SetTimeOfDay ( const ftype tm )
{
	timeofDay = tm;
}
void CloudSphere::SetCloudDensity ( const ftype dn )
{
	cloudDensity = dn;
}