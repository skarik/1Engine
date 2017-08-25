
#include "EnvHeavyWeather.h"
#include "renderer/logic/model/CModel.h"
#include "renderer/material/glMaterial.h"
#include "renderer/camera/CCamera.h"
#include "after/physics/wind/WindMotion.h"

#include "after/types/terrain/WeatherTypes.h"

EnvHeavyWeather::EnvHeavyWeather ( void )
{
	model = new CModel( "models/effects/heavy_weather.fbx" );

	mTargetWeather	= Terrain::WTH_NONE;
	mCurrentWeather	= Terrain::WTH_NONE;
	mInside	= false;
	mWasInside = false;

	weatherStrength = 0;

	targetMaterial = NULL;

}

EnvHeavyWeather::~EnvHeavyWeather ( void )
{
	delete model;
}


void EnvHeavyWeather::Update ( void )
{
	if ( !CCamera::activeCamera ) { // Skip if no active camera
		return;
	}

	// Do weather blending effects
	if ( weatherStrength <= 0.0f )
	{
		// Swap weather
		if ( mCurrentWeather != mTargetWeather ) {
			// Switch the shader
			switch ( mTargetWeather )
			{
			case Terrain::WTH_HEAVY_RAIN:
				model->GetMaterial()->loadFromFile( "sky/env_heavy_rain" );
				break;
			case Terrain::WTH_HEAVY_RAIN_THUNDER:
				model->GetMaterial()->loadFromFile( "sky/env_heavy_rain" );
				break;
			case Terrain::WTH_HEAVY_SNOW:
				model->GetMaterial()->loadFromFile( "sky/env_heavy_snow" );
				break;
			case Terrain::WTH_STORM_SNOW:
				model->GetMaterial()->loadFromFile( "sky/env_heavy_snow" );
				break;
			}
			// Set material properly
			targetMaterial = model->GetMaterial();
		}
		mCurrentWeather = mTargetWeather;
	}
	if ( (mCurrentWeather != mTargetWeather) || (mTargetWeather == Terrain::WTH_NONE) ) {
		weatherStrength -= Time::deltaTime * 0.14f;
		if ( weatherStrength < 0.0f && !mInside ) {
			mWasInside = false;
		}
	}
	else if ( mInside ) {
		weatherStrength -= Time::deltaTime;
		mWasInside = true;
	}
	else {
		if ( mWasInside ) {
			weatherStrength += Time::deltaTime;
			if ( weatherStrength >= 1.0f ) {
				mWasInside = false;
			}
		}
		else {
			weatherStrength += Time::deltaTime * 0.14f;
		}
	}
	weatherStrength = std::max<ftype>( -0.02f, std::min<ftype>( weatherStrength, 1.02f ) );
	if ( weatherStrength < 0.01f ) {
		model->SetVisibility( false );
	}
	else {
		model->SetVisibility( true );
		// Set shader uniforms
		if ( targetMaterial ) {
			//targetMaterial->setUniform( "gm_FadeValue", weatherStrength );
			model->SetShaderUniform( "gm_FadeValue", weatherStrength );
		}
	}
	
	// move to camera
	transform.position = CCamera::activeCamera->transform.position;
	transform.position.x = (ftype)((int)(transform.position.x+0.5f));	// Round position for proper results
	transform.position.y = (ftype)((int)(transform.position.y+0.5f));
	transform.position.z = (ftype)((int)(transform.position.z+0.5f));

	// angle based on wind motion
	Vector3d directionVector = Vector3d( 0,0,-1.7f );
	if ( mCurrentWeather == Terrain::WTH_HEAVY_SNOW || mCurrentWeather == Terrain::WTH_STORM_SNOW ) {
		directionVector *= 0.44f;
	}
	directionVector += WindMotion.GetFlowFieldFast( transform.position );

	Quaternion rot = Quaternion::CreateRotationTo( Vector3d(0,0,-1), directionVector.normal() );
	Rotator finalRot ( rot );
	transform.rotation.Lerp( finalRot, 0.1f );

	model->transform.Get( transform );
}

void EnvHeavyWeather::SetWeather ( const uchar nWeatherType )
{
	if ( mTargetWeather != nWeatherType )
	{
		mTargetWeather = nWeatherType;
		/*switch ( nWeatherType )
		{
		case Terrain::WTH_HEAVY_RAIN:
			model->GetPassMaterial(1)->loadFromFile( "sky/env_heavy_rain" );
			break;
		case Terrain::WTH_HEAVY_RAIN_THUNDER:
			model->GetPassMaterial(1)->loadFromFile( "sky/env_heavy_rain" );
			break;
		case Terrain::WTH_HEAVY_SNOW:
			model->GetPassMaterial(1)->loadFromFile( "sky/env_heavy_snow" );
			break;
		case Terrain::WTH_STORM_SNOW:
			model->GetPassMaterial(1)->loadFromFile( "sky/env_heavy_snow" );
			break;
		}*/
	}
}

void EnvHeavyWeather::SetInside ( const bool nIsInside )
{
	mInside = nIsInside;
}
