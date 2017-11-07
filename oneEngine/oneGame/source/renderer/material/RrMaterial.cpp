
#include "RrMaterial.h"

#include "core/time/time.h"
#include "core/settings/CGameSettings.h"
#include "core/types/ModelData.h"
#include "core-ext/transform/TransformUtility.h"

#include "renderer/types/ObjectSettings.h"
#include "renderer/object/CRenderableObject.h"
#include "renderer/light/CLight.h"

#include "renderer/state/Settings.h"

#include "renderer/camera/CCamera.h"
#include "renderer/camera/CRTCamera.h"
#include "renderer/camera/CRTCameraCascade.h"

#include "renderer/texture/CRenderTexture.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/texture/CRenderTextureCube.h"

#include "renderer/system/glMainSystem.h"

#include <map>

//RrMaterial* RrMaterial::current	= NULL;
RrMaterial*	RrMaterial::Default = NULL;
RrMaterial*	RrMaterial::Copy	= NULL;
RrMaterial*	RrMaterial::Fallback= NULL;
uint		RrMaterial::current_sampler_slot = 0;

// Current material special mode.
// The value of this will drastically change the rest of the program. These are hard-coded special effects.
uchar		RrMaterial::special_mode = renderer::SP_MODE_NORMAL;

// Current material world sampler state
CTexture*	RrMaterial::m_sampler_reflection = NULL;

// =====================================

RrMaterial::RrMaterial ( void )
	: m_isScreenShader(false), m_isSkinnedShader(false), m_isInstancedShader(false),
	m_diffuse(1,1,1,1), m_specular(0,0,0,1), /*m_specularPower(32),*/ m_emissive(0,0,0,0),
	m_texcoordScaling(1,1,1,1), m_texcoordOffset(0,0,0,0),
	gm_WindDirection(0,0,0,1), gm_FadeValue(0), gm_HalfScale(0),
	m_bufferMatricesSkinning(0), /*m_bufferMatricesSoftbody(0),*/ m_bufferSkeletonSize(0),
	referenceCount(1), staticResource(false)
{
	memset( m_highlevel_storage, 0, sizeof(CTexture*) * 12 );
	memset( m_samplers,			 0, sizeof(glHandle) * 12 );
	memset( m_sampler_targets,	 0, sizeof(glHandle) * 12 );
}

RrMaterial::~RrMaterial ( void ) throw(...)
{
	if ( hasReference() ) {
		throw std::exception();
	}

	// Clear off existing textures
	for ( int n_index = 0; n_index < 12; ++n_index )
	{
		if ( m_highlevel_storage[n_index] != NULL )
		{
			m_highlevel_storage[n_index]->RemoveReference();
			if ( !m_highlevel_storage[n_index]->HasReference() ) {
				delete_safe(m_highlevel_storage[n_index]);
			}
		}
	}
}

// UBO locations
//const glHandle fogLocation = 0;
//const glHandle lightLocation = 1;
const glHandle skinningMajorLocation = 6;
const glHandle skinningMinorLocation = 7;
//const glHandle reflectLocation = 4;
//const glHandle pureShadowMatrices = 5;

// Duplicate material (starting with 1 reference)
RrMaterial*	RrMaterial::copy ( void )
{
	RrMaterial* newMaterial = new RrMaterial;
	newMaterial->m_diffuse	= m_diffuse;
	newMaterial->m_specular	= m_specular;
	//newMaterial->m_specularPower	= m_specularPower;
	newMaterial->m_emissive	= m_emissive;
	newMaterial->gm_WindDirection	= gm_WindDirection;
	newMaterial->gm_FadeValue		= gm_FadeValue;
	newMaterial->gm_HalfScale		= gm_HalfScale;
	newMaterial->m_bufferMatricesSkinning	= m_bufferMatricesSkinning;
	//newMaterial->m_bufferMatricesSoftbody	= m_bufferMatricesSoftbody;

	memcpy( newMaterial->m_highlevel_storage, m_highlevel_storage, sizeof(CTexture*) * 12 );
	memcpy( newMaterial->m_samplers, m_samplers, sizeof(glHandle) * 12 );
	memcpy( newMaterial->m_sampler_targets, m_sampler_targets, sizeof(glHandle) * 12 );

	newMaterial->passinfo = passinfo;
	newMaterial->deferredinfo = deferredinfo;

	newMaterial->m_name = m_name;
	newMaterial->m_filename = m_filename;

	newMaterial->m_isScreenShader = m_isScreenShader;
	newMaterial->m_isSkinnedShader = m_isSkinnedShader;

	return newMaterial;
}

uchar RrMaterial::getPassCount ( void )
{
	if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_FORWARD )
	{
		return (uchar)passinfo.size();
	}
	else
	{
		return (uchar)deferredinfo.size();
	}
}
uchar RrMaterial::getPassCountForward ( void )
{
	return (uchar)passinfo.size();
}
uchar RrMaterial::getPassCountDeferred ( void )
{
	return (uchar)deferredinfo.size();
}

//glHandle	RrMaterial::m_ubo_foginfo = 0;
glHandle	RrMaterial::m_ubo_lightinginfo = 0;
glHandle	RrMaterial::m_ubo_reflectinfo = 0;
//GLuint	m_ubo_worldinfo = 0;
//struct _fogInfo_t
//{
//	Color	color;
//	Color	atmo;
//	float	end;
//	float	scale;
//}; 
#define HARD_MAX 8
struct _lightingInfo_t
{
	Vector4d LightColor[HARD_MAX];
	Vector4d LightPosition[HARD_MAX];
	Vector4d LightProperties[HARD_MAX];
	Vector4d LightShadowInfo[HARD_MAX];
	Matrix4x4 LightMatrix[HARD_MAX];
};
struct _reflectInfo_t
{
	Vector4d ReflectMinBox;
	Vector4d ReflectMaxBox;
	Vector4d ReflectSource;
};
void RrMaterial::updateStaticUBO ( void )
{
	//{
	//	_fogInfo_t t_fogInfo;
	//	// Create fog struct data
	//	t_fogInfo.color	= renderer::Settings.fogColor;
	//	t_fogInfo.color.alpha = 1.0F;
	//	t_fogInfo.atmo	= Color::Lerp( renderer::Settings.fogColor, renderer::Settings.ambientColor, 0.5F ) * Color(0.9F,0.9F,1.1F);
	//	t_fogInfo.atmo.alpha = 1.0F;
	//	t_fogInfo.end	= renderer::Settings.fogEnd;
	//	t_fogInfo.scale	= renderer::Settings.fogScale;
	//	// Send it to video card
	//	if ( m_ubo_foginfo == 0 ) {
	//		glGenBuffers( 1, &m_ubo_foginfo );
	//		glBindBuffer( GL_UNIFORM_BUFFER, m_ubo_foginfo );
	//		glBufferData( GL_UNIFORM_BUFFER, sizeof(_fogInfo_t), NULL, GL_STREAM_DRAW );
	//	}
	//	glBindBuffer( GL_UNIFORM_BUFFER, m_ubo_foginfo );
	//	glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof(_fogInfo_t), &t_fogInfo );
	//}

	if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_FORWARD )
	{
		if ( renderer::Settings.maxLights <= 0 ) throw core::NullReferenceException(); // Null data passed to shader

		_lightingInfo_t t_lightingInfo;
		// Get the light list
		std::vector<CLight*>* lightList = CLight::GetActiveLightList();
		// Set the maximum value to iterate through
		uint t_maxLights = std::min<uint>( (uint)(renderer::Settings.maxLights-1), HARD_MAX );
		uint maxVal = lightList->size();
		maxVal = std::min<uint>( maxVal, t_maxLights );
		// Loop through all the lights in the list
		for ( unsigned int i = 0; i < maxVal; i++ )
		{
			// Set the light properties
			t_lightingInfo.LightProperties[i].x = (1.0f/(*lightList)[i]->range);
			t_lightingInfo.LightProperties[i].y = (*lightList)[i]->falloff;
			t_lightingInfo.LightProperties[i].z = (*lightList)[i]->pass;
			t_lightingInfo.LightProperties[i].w = 1.0f;
			// Set the light position
			t_lightingInfo.LightPosition[i].x = (*lightList)[i]->GetSendVector().red;
			t_lightingInfo.LightPosition[i].y = (*lightList)[i]->GetSendVector().green;
			t_lightingInfo.LightPosition[i].z = (*lightList)[i]->GetSendVector().blue;
			t_lightingInfo.LightPosition[i].w = (*lightList)[i]->GetSendVector().alpha;
			// Set the light color
			t_lightingInfo.LightColor[i].x = (*lightList)[i]->diffuseColor.red;
			t_lightingInfo.LightColor[i].y = (*lightList)[i]->diffuseColor.green;
			t_lightingInfo.LightColor[i].z = (*lightList)[i]->diffuseColor.blue;
			t_lightingInfo.LightColor[i].w = (*lightList)[i]->diffuseColor.alpha;
			// Set shadow casting info
			if ( CGameSettings::Active()->b_ro_EnableShadows )
			{
				// No shadows, send dead shadow info
				if ( !(*lightList)[i]->generateShadows )
				{
					t_lightingInfo.LightShadowInfo[i].x = 0;
					t_lightingInfo.LightShadowInfo[i].y = 0;
					t_lightingInfo.LightShadowInfo[i].z = 0;
					t_lightingInfo.LightShadowInfo[i].w = 0;
				}
				// Has shadows, send shadow info
				else if ( (*lightList)[i]->isDirectional )
				{
					// Set shadow parameters
					t_lightingInfo.LightShadowInfo[i].x = 1;
					t_lightingInfo.LightShadowInfo[i].y = (*lightList)[i]->shadowStrength;
					t_lightingInfo.LightShadowInfo[i].z = 0;
					t_lightingInfo.LightShadowInfo[i].w = 0;

					// Set shadow matrix
					if ( lightList->at(i)->GetShadowCamera() && lightList->at(i)->GetShadowCamera()->GetType() == CAMERA_TYPE_RT_CASCADE ) { 
						//t_lightingInfo.LightMatrix[i] = lightList->at(i)->GetShadowCamera()->textureMatrix.transpose();
						CRTCameraCascade* t_cascadeCamera = (CRTCameraCascade*)lightList->at(i)->GetShadowCamera();
						t_lightingInfo.LightMatrix[0] = t_cascadeCamera->m_renderMatrices[0].transpose();
						t_lightingInfo.LightMatrix[1] = t_cascadeCamera->m_renderMatrices[1].transpose();
						t_lightingInfo.LightMatrix[2] = t_cascadeCamera->m_renderMatrices[2].transpose();
						t_lightingInfo.LightMatrix[3] = t_cascadeCamera->m_renderMatrices[3].transpose();
					}
				}
			}
			else
			{
				// Shadows are disabled
				t_lightingInfo.LightShadowInfo[i].x = 0;
				t_lightingInfo.LightShadowInfo[i].y = 0;
				t_lightingInfo.LightShadowInfo[i].z = 0;
				t_lightingInfo.LightShadowInfo[i].w = 0;
			}
		}
		// Loop through all the lights NOT in the list, and set their range to zero
		for ( uint i = maxVal; i < t_maxLights; i++ )
		{
			// Set the light properties
			t_lightingInfo.LightProperties[i].x = 0.001f;
			t_lightingInfo.LightProperties[i].y = 1.0f;
			t_lightingInfo.LightProperties[i].z = 0.0f;
			t_lightingInfo.LightProperties[i].w = 1.0f;
			// Set the light color
			t_lightingInfo.LightColor[i].x = 0;
			t_lightingInfo.LightColor[i].y = 0;
			t_lightingInfo.LightColor[i].z = 0;
			t_lightingInfo.LightColor[i].w = 0;
			// Shadows are disabled
			t_lightingInfo.LightShadowInfo[i].x = 0;
			t_lightingInfo.LightShadowInfo[i].y = 0;
			t_lightingInfo.LightShadowInfo[i].z = 0;
			t_lightingInfo.LightShadowInfo[i].w = 0;
		}

		// Send it to video card
		if ( m_ubo_lightinginfo == 0 )
		{
			glGenBuffers( 1, &m_ubo_lightinginfo );
			glBindBuffer( GL_UNIFORM_BUFFER, m_ubo_lightinginfo );
			glBufferData( GL_UNIFORM_BUFFER, sizeof(_lightingInfo_t), NULL, GL_STREAM_DRAW );
		}
		glBindBuffer( GL_UNIFORM_BUFFER, m_ubo_lightinginfo );
		glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof(_lightingInfo_t), &t_lightingInfo );
	}

	if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_FORWARD && m_sampler_reflection )
	{
		_reflectInfo_t t_reflectInfo;
		// Create reflect information
		if ( m_sampler_reflection->ClassType() == TextureClassRenderTarget_Cube ) 
		{
			CRenderTextureCube* t_cubeTexture = (CRenderTextureCube*)m_sampler_reflection;
			t_reflectInfo.ReflectSource = t_cubeTexture->m_renderPosition;
			t_reflectInfo.ReflectMaxBox = t_reflectInfo.ReflectSource + Vector4d( 1000,1000,1000 );
			t_reflectInfo.ReflectMinBox = t_reflectInfo.ReflectSource - Vector4d( 1000,1000,1000 );
		}
		else
		{
			t_reflectInfo.ReflectSource = CCamera::activeCamera->transform.position;
			t_reflectInfo.ReflectMaxBox = t_reflectInfo.ReflectSource + Vector4d( 1000,1000,1000 );
			t_reflectInfo.ReflectMinBox = t_reflectInfo.ReflectSource - Vector4d( 1000,1000,1000 );
		}
		// Send it to video card
		if ( m_ubo_reflectinfo == 0 )
		{
			glGenBuffers( 1, &m_ubo_reflectinfo );
			glBindBuffer( GL_UNIFORM_BUFFER, m_ubo_reflectinfo );
			glBufferData( GL_UNIFORM_BUFFER, sizeof(_reflectInfo_t), NULL, GL_STREAM_DRAW );
		}
		glBindBuffer( GL_UNIFORM_BUFFER, m_ubo_reflectinfo );
		glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof(_reflectInfo_t), &t_reflectInfo );
	}
}
#undef HARD_MAX

// Light VBO specific to deferred renderer
//#include "engine/audio/CAudioInterface.h"
#include "audio/CAudioSource.h"
GLuint	RrMaterial::m_ubo_deflightinginfo = 0;
GLuint	RrMaterial::m_tbo_lightinfo = 0;
GLuint	RrMaterial::m_tex_lightinfo = 0;
GLuint	RrMaterial::m_tex_shadowinfo= 0;
GLuint  RrMaterial::m_lightCount = 0;
struct _lightInfo_t
{
	float red;
	float green;
	float blue;
	float specular;

	float range;
	float falloff;
	float passthrough;
	float hasshadow;

	float x;
	float y;
	float z;
	float directional;

	float dx;
	float dy;
	float dz;
	float dw;
};
struct _shadowMatrix_t
{
	Matrix4x4 shadowCascade [4];
};
void RrMaterial::updateLightTBO ( void )
{
	if ( m_tbo_lightinfo == 0 ) {
		glGenBuffers( 1, &m_tbo_lightinfo );
		glBindBuffer( GL_TEXTURE_BUFFER, m_tbo_lightinfo );
		glBufferData( GL_TEXTURE_BUFFER, sizeof(_lightInfo_t), NULL, GL_STREAM_DRAW );
	}
	if ( m_tex_lightinfo == 0 ) {
		glGenTextures( 1, &m_tex_lightinfo );
		glBindTexture( GL_TEXTURE_BUFFER, m_tex_lightinfo );
		glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA32F, m_tbo_lightinfo ); 
		glBindTexture( GL_TEXTURE_BUFFER, 0 );
	}
	/*if ( m_tex_shadowinfo == 0 ) {
		glGenTextures( 1, &m_tex_shadowinfo );
		glBindTexture( GL_TEXTURE_BUFFER, m_tex_shadowinfo );
		glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA32F, m_tex_shadowinfo ); 
		glBindTexture( GL_TEXTURE_BUFFER, 0 );
	}*/
	if ( special_mode == renderer::SP_MODE_NORMAL || special_mode == renderer::SP_MODE_SHAFT || special_mode == renderer::SP_MODE_2DPALETTE )
	{
		// Go through all the lights and add them to this list
		_lightInfo_t lightProperties [32];
		m_lightCount = 0;

		// Get the light list
		std::vector<CLight*>* lightList = CLight::GetActiveLightList(); 
		for ( uint li = 0; li < lightList->size(); ++li )
		{
			CLight* light = lightList->at(li);
			lightProperties[li].red		= light->diffuseColor.red;
			lightProperties[li].green	= light->diffuseColor.green;
			lightProperties[li].blue	= light->diffuseColor.blue;
			lightProperties[li].specular= light->diffuseColor.alpha;

			lightProperties[li].range	= 1/light->range;
			lightProperties[li].falloff	= light->falloff;
			lightProperties[li].passthrough	= light->pass;
			lightProperties[li].hasshadow	= light->generateShadows ? light->shadowStrength : -1.0F;

			lightProperties[li].x	= light->GetSendVector().red;
			lightProperties[li].y	= light->GetSendVector().green;
			lightProperties[li].z	= light->GetSendVector().blue;
			lightProperties[li].directional	= light->GetSendVector().alpha;

			m_lightCount += 1;
			if ( m_lightCount == 32 ) {
				break;
			}
		}
		// Generate shadow matrices
		if ( m_ubo_deflightinginfo == 0 ) {
			glGenBuffers( 1, &m_ubo_deflightinginfo );
			glBindBuffer( GL_UNIFORM_BUFFER, m_ubo_deflightinginfo );
			glBufferData( GL_UNIFORM_BUFFER, sizeof(_shadowMatrix_t), NULL, GL_STREAM_DRAW );
		}
		if ( !lightList->empty() && lightList->at(0)->isDirectional )
		{
			_shadowMatrix_t	shadowMatrices [1];
			CRTCameraCascade* t_cascadeCamera = (CRTCameraCascade*)lightList->at(0)->GetShadowCamera();
			if ( t_cascadeCamera )
			{
				shadowMatrices[0].shadowCascade[0] = t_cascadeCamera->m_renderMatrices[0].transpose();
				shadowMatrices[0].shadowCascade[1] = t_cascadeCamera->m_renderMatrices[1].transpose();
				shadowMatrices[0].shadowCascade[2] = t_cascadeCamera->m_renderMatrices[2].transpose();
				shadowMatrices[0].shadowCascade[3] = t_cascadeCamera->m_renderMatrices[3].transpose();
			}
			{
				// Send it to video card
				glBindBuffer( GL_UNIFORM_BUFFER, m_ubo_deflightinginfo );
				//glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof(_shadowMatrix_t), &shadowMatrices );
				void* p = glMapBufferRange( GL_UNIFORM_BUFFER, 0, sizeof(_shadowMatrix_t), GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
				if ( p ) {
					memcpy( p, &shadowMatrices, sizeof(_shadowMatrix_t) );
					glUnmapBuffer( GL_UNIFORM_BUFFER );
				}
			}
		}

		if ( m_lightCount > 0 )
		{
			glBindBuffer( GL_TEXTURE_BUFFER, m_tbo_lightinfo );
			glBufferData( GL_TEXTURE_BUFFER, sizeof(_lightInfo_t)*m_lightCount, NULL, GL_STREAM_DRAW ); //orphan old data
			//glBufferSubData( GL_TEXTURE_BUFFER, 0, sizeof(_lightInfo_t)*m_lightCount, &lightProperties );
			//GLvoid* p = glMapBuffer( GL_TEXTURE_BUFFER, GL_WRITE_ONLY );
			GLvoid* p = glMapBufferRange( GL_TEXTURE_BUFFER, 0, sizeof(_lightInfo_t)*m_lightCount, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
			if ( p ) {
				memcpy( p, &lightProperties, sizeof(_lightInfo_t)*m_lightCount );
				glUnmapBuffer( GL_TEXTURE_BUFFER );
			}

			//glBindBuffer( GL_UNIFORM_BUFFER, m_textureBufferData );
			/*glBindTexture( GL_TEXTURE_BUFFER, m_tex_lightinfo );
			glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA32F, m_tbo_lightinfo );
			glBindTexture( GL_TEXTURE_BUFFER, 0 );*/
		}
	}
	else if ( special_mode == renderer::SP_MODE_ECHO )
	{
		static std::map<unsigned int,_lightInfo_t> echomap;
		std::vector<CAudioSource*> currentSounds = *CAudioMaster::GetCurrent()->GetSources();//Audio.GetCurrentSources();
		for ( uint i = 0; i < currentSounds.size(); ++i )
		{
			auto echomap_find = echomap.find( currentSounds[i]->GetID() );
			// If light not in there, make it
			if ( echomap_find == echomap.end() )
			{
				_lightInfo_t newecho;
				newecho.hasshadow		= 1;
				newecho.directional		= 1;
				newecho.falloff			= 1;
				newecho.passthrough		= 0.6f;

				newecho.red			= 0;
				newecho.green		= 0;
				newecho.blue		= 0;
				newecho.specular	= 0;
				echomap[currentSounds[i]->GetID()] = newecho;

				echomap_find = echomap.find( currentSounds[i]->GetID() );
			}
			else // Skip first frame (light is black) because sounds delay a frame
			{	
				// Set light options
				echomap_find->second.x = currentSounds[i]->position.x;
				echomap_find->second.y = currentSounds[i]->position.y;
				echomap_find->second.z = currentSounds[i]->position.z;

				// Set ranger
				echomap_find->second.hasshadow = 1; 
				echomap_find->second.dw = (float)currentSounds[i]->GetCurrentMagnitude();

				// Set color
				echomap_find->second.red	= 1;
				echomap_find->second.green	= 1;
				echomap_find->second.blue	= 1;
			}
		}

		// Go through all the echos and add them to this list
		_lightInfo_t lightProperties [32];
		m_lightCount = 0;

		{	// Create a little sound near camera
			lightProperties[0].x = CCamera::activeCamera->transform.position.x;
			lightProperties[0].y = CCamera::activeCamera->transform.position.y;
			lightProperties[0].z = CCamera::activeCamera->transform.position.z + 0.1f;

			// Set color
			lightProperties[0].red		= 0.5f;
			lightProperties[0].green	= 0.4f;
			lightProperties[0].blue		= 0.4f;

			// Set blend
			lightProperties[0].specular = abs(sinf(Time::currentTime * 0.33f)) * 0.3f;

			// Set ranger
			lightProperties[0].directional		= 1;
			lightProperties[0].falloff			= 1;
			lightProperties[0].passthrough		= 0.4f;
			lightProperties[0].range		= 1/(4.0f);//1/(std::max<Real>( 3.0f, std::min<Real>( 12.0f, CCamera::activeCamera->focalDistance )));

			m_lightCount += 1;
		}

		for ( auto echo = echomap.begin(); echo != echomap.end(); )
		{
			// Calculate the range
			Real apparentRange = 12 * (1+echo->second.dw);
			echo->second.range = 1/(apparentRange*echo->second.hasshadow);

			if ( m_lightCount < 32 ) {
				// Add echo to the light list
				lightProperties[m_lightCount] = echo->second;
				m_lightCount += 1;	
			}

			// Apply per-step modifiers
			echo->second.hasshadow -= Time::deltaTime;
			echo->second.specular += Time::deltaTime;
			
			// Remove echo if it's old
			if ( echo->second.hasshadow <= 0 ) {
				echo = echomap.erase( echo );
			}
			else {
				++echo;
			}
		}

		if ( m_lightCount > 0 )
		{
			glBindBuffer( GL_TEXTURE_BUFFER, m_tbo_lightinfo );
			glBufferData( GL_TEXTURE_BUFFER, sizeof(_lightInfo_t)*m_lightCount, NULL, GL_STREAM_DRAW ); //orphan old data
			glBufferSubData( GL_TEXTURE_BUFFER, 0, sizeof(_lightInfo_t)*m_lightCount, &lightProperties );

			/*glBindTexture( GL_TEXTURE_BUFFER, m_tex_lightinfo );
			glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA32F, m_tbo_lightinfo ); 
			glBindTexture( GL_TEXTURE_BUFFER, 0 );*/
		}
	}
}

#include "core-ext/profiler/CTimeProfiler.h"

void RrMaterial::bindPass ( uchar pass )
{
	GL_ACCESS;
	/*if ( !this->m_isScreenShader && this != RrMaterial::Copy && this != RrMaterial::Default ) {
		RrMaterial::Default->bindPass(pass);
		return;
	}*/

	// Save this material and pass as current
	//current			= this;
	//current_pass	= pass;
	TimeProfiler.BeginTimeProfile( "rs_mat_bindpass" );

	// Forward binding
	if ( /*GL.inOrtho() ||*/ CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_FORWARD || deferredinfo.empty() )
	{
		bindPassForward( pass );
	}
	else if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_DEFERRED || passinfo.empty() )
	{
		bindPassDeferred( pass );
	}
	else
	{
		throw std::exception();
	}

	TimeProfiler.EndAddTimeProfile( "rs_mat_bindpass" );
}

static struct _pass_forward_state {
	RrShader* shader;
	RrPassForward*	  pass;
	_pass_forward_state ( void )
		: shader(NULL), pass(NULL)
	{
		;
	}
} state;
static struct _pass_deferred_state {
	RrShader*			shader;
	RrPassDeferred*	pass;
	_pass_deferred_state ( void )
		: shader(NULL), pass(NULL)
	{
		;
	}
} df_state;


void RrMaterial::bindPassForward ( uchar pass )
{
	// Reset deferred state
	df_state.shader = NULL;

	// Save this material and pass as current
	m_currentShader	= passinfo[pass].shader;
	m_currentPass = pass;
	m_currentPassForward = true;

	// Set face mode
	switch ( passinfo[pass].m_face_mode )
	{
	case renderer::FM_FRONT:
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
		break;
	case renderer::FM_BACK:
		glEnable( GL_CULL_FACE );
		glCullFace( GL_FRONT );
		break;
	case renderer::FM_FRONTANDBACK:
		glDisable( GL_CULL_FACE );
		break;
	}
	// Set blend mode
	glEnable( GL_BLEND );
	switch ( passinfo[pass].m_blend_mode )
	{
	case renderer::BM_NORMAL:
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		break;
	case renderer::BM_ADD:
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		break;
	case renderer::BM_SOFT_ADD:
		glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
		break;
	case renderer::BM_INV_MULTIPLY:
		glBlendFunc( GL_ZERO, GL_ONE_MINUS_SRC_COLOR );
		break;
	case renderer::BM_MULTIPLY:
		glBlendFunc( GL_DST_COLOR, GL_ZERO );
		break;
	case renderer::BM_MULTIPLY_X2:
		glBlendFunc( GL_DST_COLOR, GL_SRC_COLOR );
		break;
	case renderer::BM_NONE:
		glDisable( GL_BLEND );
		glBlendFunc( GL_ONE, GL_ZERO );
		break;
	}

	// Set transparency mode
	if ( passinfo[pass].b_depthtest )
	{
		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LEQUAL );
	}
	else
	{
		glDisable( GL_DEPTH_TEST );
	}
	if ( !passinfo[pass].b_depthmask )
	{
		glDepthMask( GL_FALSE );
	}
	else
	{
		if (passinfo[pass].m_transparency_mode == renderer::ALPHAMODE_NONE ||
			passinfo[pass].m_transparency_mode == renderer::ALPHAMODE_ALPHATEST)
		{
			glDepthMask( GL_TRUE );
		}
		else
		{
			glDepthMask( GL_FALSE );
		}
	}

	// Bind shader
	TimeProfiler.BeginTimeProfile( "rs_mat_bindshader" );
	if ( passinfo[pass].shader )
	{
		if ( !state.shader || passinfo[pass].shader->get_program() != state.shader->get_program() )
		{
			try
			{
				passinfo[pass].shader->begin();
				state.shader = passinfo[pass].shader;
				state.pass = &(passinfo[pass]);
			}
			catch ( const std::exception& )
			{
				RrMaterial::Fallback->passinfo[0].shader->begin();
				state.shader = RrMaterial::Fallback->passinfo[0].shader;
				state.pass = &(RrMaterial::Fallback->passinfo[0]);
			}
		}
	}
	else {
		throw std::exception();
	}
	TimeProfiler.EndAddTimeProfile( "rs_mat_bindshader" );
	// Send all constants
	TimeProfiler.BeginTimeProfile( "rs_mat_binduniforms" );

	TimeProfiler.BeginTimeProfile( "rs_mat_uni_sampler" );
	current_sampler_slot = 0;
	shader_bind_samplers(state.shader);
	TimeProfiler.EndAddTimeProfile( "rs_mat_uni_sampler" );

	// Tell the shader to use the constants
	TimeProfiler.BeginTimeProfile( "rs_mat_cbuf" );
	shader_set_constantbuffers(passinfo[pass].shader, pass, true);
	TimeProfiler.EndTimeProfile( "rs_mat_cbuf" );

	TimeProfiler.EndAddTimeProfile( "rs_mat_binduniforms" );
}

void RrMaterial::bindPassDeferred ( uchar pass )
{
	// Reset forward state
	state.shader = NULL;

	// Save this material and pass as current
	m_currentShader	= deferredinfo[pass].shader;
	m_currentPass = pass;
	m_currentPassForward = false;

	// Set face mode
	if (0) // TODO: Fix this.
	{
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
	}
	else
	{
		glDisable( GL_CULL_FACE );
	}

	// Set transparency mode
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	switch ( deferredinfo[pass].m_transparency_mode )
	{
	case renderer::ALPHAMODE_NONE:
		glDisable( GL_BLEND );
		glBlendFunc( GL_ONE, GL_ZERO );
		glDepthMask( GL_TRUE );
		break;
	case renderer::ALPHAMODE_ALPHATEST:
		glDisable( GL_BLEND );
		glBlendFunc( GL_ONE, GL_ZERO );
		glDepthMask( GL_TRUE );
		break;
	case renderer::ALPHAMODE_TRANSLUCENT:
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glDepthMask( GL_FALSE );
		break;
	}

	// Do first-time initialization checks
	if ( !deferredinfo[pass].m_ready )
	{
		deferredinfo[pass].m_ready = true;
		deferred_shader_build( pass );
	}

	// Bind shader
	TimeProfiler.BeginTimeProfile( "rs_mat_bindshader" );
	if ( deferredinfo[pass].shader )
	{
		if ( !df_state.shader || df_state.shader->get_program() != deferredinfo[pass].shader->get_program() )
		{
			//try
			//{
				deferredinfo[pass].shader->begin();
				df_state.shader = deferredinfo[pass].shader;
				//shader_bind_world(deferredinfo[pass].shader);
				df_state.pass = &(deferredinfo[pass]);
			/*}
			catch ( const std::exception& e )
			{
				RrMaterial::Fallback->passinfo[0].shader->begin();
				state.shader = RrMaterial::Fallback->passinfo[0].shader;
				shader_bind_world(RrMaterial::Fallback->passinfo[0].shader);
				state.pass = &(RrMaterial::Fallback->passinfo[0]);
			}*/
		}
	}
	else {
		throw std::exception();
	}
	TimeProfiler.EndAddTimeProfile( "rs_mat_bindshader" );

	TimeProfiler.BeginTimeProfile( "rs_mat_binduniforms" );

	TimeProfiler.BeginTimeProfile( "rs_mat_uni_sampler" );
	current_sampler_slot = 0;
	shader_bind_samplers(deferredinfo[pass].shader);
	TimeProfiler.EndAddTimeProfile( "rs_mat_uni_sampler" );

	//TimeProfiler.BeginTimeProfile( "rs_mat_uni_consts" );
	//shader_bind_constants(deferredinfo[pass].shader);
	//TimeProfiler.EndAddTimeProfile( "rs_mat_uni_consts" );

	//TimeProfiler.BeginTimeProfile( "rs_mat_uni_lights" );
	//shader_bind_deferred(deferredinfo[pass].shader);
	//TimeProfiler.EndAddTimeProfile( "rs_mat_uni_lights" );

	//TimeProfiler.BeginTimeProfile( "rs_mat_uni_obj" );
	//setShaderConstants(NULL); // TODO: move this (default take the current stack options)
	//TimeProfiler.EndAddTimeProfile( "rs_mat_uni_obj" );

	// Tell the shader to use the constants
	TimeProfiler.BeginTimeProfile( "rs_mat_cbuf" );
	shader_set_constantbuffers(deferredinfo[pass].shader, pass, false);
	TimeProfiler.EndTimeProfile( "rs_mat_cbuf" );

	TimeProfiler.EndAddTimeProfile( "rs_mat_binduniforms" );
}

void RrMaterial::shader_bind_samplers ( RrShader* shader )
{
	//int uniformLocation;
	//CTexture*	m_samplers [8];
	for ( uint i = 0; i < 12; ++i )
	{
		if ( m_samplers[i] )
		{
			// Get the uniform location
			//char uniformname_sh1 [] = "textureSamplerX"; // TODO: Cache the texture sampler and other uniform locations in shader class
			//uniformname_sh1[14] = '0' + i;
			//uniformLocation = shader->get_uniform_location( uniformname_sh1 );
			// If the spot exists, then use it
			//if ( uniformLocation >= 0 )
			//{
				glUniform1i( renderer::UNI_SAMPLER_0 + i, current_sampler_slot );
				glActiveTexture( GL_TEXTURE0+current_sampler_slot );
				CTexture::Unbind(0);
				//m_samplers[i]->Bind();
				glBindTexture( m_sampler_targets[i], m_samplers[i] );

				// Increment used texture count
				current_sampler_slot += 1;

				if ( m_highlevel_storage[i] && m_highlevel_storage[i]->GetIsFont() ) {
					((CBitmapFont*)(m_highlevel_storage[i]))->Set();
				}
			//}
			/*else {
				CTexture::Unbind(0);
			}*/
		}
	}

	if ( m_bufferMatricesSkinning && m_bufferSkeletonSize )
	{
		//int uniformLocation = shader->get_uniform_block_location( "sys_SkinningDataMajor" );
		int uniformLocation = shader->getUniformBlockLocation( "sys_SkinningDataMajor" ); // TODO: Fix this.
		if ( uniformLocation >= 0 )
		{
			glUniformBlockBinding( shader->get_program(), uniformLocation, skinningMajorLocation );
			//glBindBufferRange( GL_UNIFORM_BUFFER, skinningMajorLocation, m_bufferMatricesSkinning, NIL, sizeof(Matrix4x4)*m_bufferSkeletonSize );
			glBindBufferRange( GL_UNIFORM_BUFFER, skinningMajorLocation, m_bufferMatricesSkinning, NIL, sizeof(Matrix4x4)*256 );
		}
	}
	/*if ( m_bufferMatricesSoftbody && m_bufferSkeletonSize )
	{
		uniformLocation = shader->get_uniform_block_location( "sys_SkinningDataMinor" );
		if ( uniformLocation >= 0 )
		{
			glUniformBlockBinding( shader->get_program(), uniformLocation, skinningMinorLocation );
			glBindBufferRange( GL_UNIFORM_BUFFER, skinningMinorLocation, m_bufferMatricesSoftbody, NULL, sizeof(Matrix4x4)*m_bufferSkeletonSize );
			//GLenum error = glGetError();
		}
	}*/

	glUniform1i( renderer::UNI_SAMPLER_LIGHT_BUFFER_0, current_sampler_slot );
	glActiveTexture( GL_TEXTURE0+current_sampler_slot );
	CTexture::Unbind(0);
	glBindTexture( GL_TEXTURE_BUFFER, m_tex_lightinfo );
	current_sampler_slot += 1;

	glUniform1i( renderer::UNI_SAMPLER_INSTANCE_BUFFER_0, current_sampler_slot );
	glActiveTexture( GL_TEXTURE0+current_sampler_slot );
	CTexture::Unbind(0);
	glBindTexture( GL_TEXTURE_BUFFER, m_tex_instancedinfo );
	current_sampler_slot += 1;
}

void RrMaterial::prepareShaderConstants ( void )
{
	Matrix4x4 modelTRS, modelRS;
	// Update matrix constants
	pushConstantsPerObject(modelTRS, modelRS);
}
void RrMaterial::prepareShaderConstants ( const core::Transform& n_transform )
{
	Matrix4x4 modelTRS, modelRS;
	modelTRS = n_transform.WorldMatrix();
	modelRS = n_transform.WorldRotation();

	// Update matrix constants
	pushConstantsPerObject(!modelTRS, modelRS);
}
void RrMaterial::prepareShaderConstants ( const XrTransform& n_transform )
{	
	Matrix4x4 modelTRS, modelRS;
	core::TransformUtility::TRSToMatrix4x4(n_transform, modelTRS, modelRS);

	// Update matrix constants
	pushConstantsPerObject(!modelTRS, modelRS);
}

void	RrMaterial::bindPassAtrribs ( void )
{
	bindAttribute( renderer::ATTRIB_VERTEX,		3, GL_FLOAT, false, sizeof(arModelVertex), ((char*)0) + (sizeof(float)*0) );
	bindAttribute( renderer::ATTRIB_TEXCOORD0,	3, GL_FLOAT, false, sizeof(arModelVertex), ((char*)0) + (sizeof(float)*3) );
	bindAttribute( renderer::ATTRIB_COLOR,		4, GL_FLOAT, false, sizeof(arModelVertex), ((char*)0) + (sizeof(float)*15) );
	bindAttribute( renderer::ATTRIB_NORMAL,		3, GL_FLOAT, false, sizeof(arModelVertex), ((char*)0) + (sizeof(float)*6) );
	bindAttribute( renderer::ATTRIB_TANGENTS,	3, GL_FLOAT, false, sizeof(arModelVertex), ((char*)0) + (sizeof(float)*9) );
	bindAttribute( renderer::ATTRIB_BINORMALS,	3, GL_FLOAT, false, sizeof(arModelVertex), ((char*)0) + (sizeof(float)*12) );
	bindAttribute( renderer::ATTRIB_BONEWEIGHTS,4,GL_FLOAT, false, sizeof(arModelVertex), ((char*)0) + (sizeof(float)*20) );
	bindAttributeI( renderer::ATTRIB_BONEINDICES,4,GL_UNSIGNED_BYTE, sizeof(arModelVertex), ((char*)0) + (sizeof(float)*19) );
	bindAttribute( renderer::ATTRIB_TEXCOORD2,	3, GL_FLOAT, false, sizeof(arModelVertex), ((char*)0) + (sizeof(float)*24) );
	bindAttribute( renderer::ATTRIB_TEXCOORD3,	3, GL_FLOAT, false, sizeof(arModelVertex), ((char*)0) + (sizeof(float)*27) );
	bindAttribute( renderer::ATTRIB_TEXCOORD4,	2, GL_FLOAT, false, sizeof(arModelVertex), ((char*)0) + (sizeof(float)*30) );
}
void RrMaterial::bindAttribute ( int attributeIndex, const uint vec_size, const uint vec_type, const bool normalize, const int struct_size, const void* struct_offset )
{
	RrPassForward::enabled_attributes[attributeIndex] = true;
	glEnableVertexAttribArray( attributeIndex );
	glVertexAttribPointer( attributeIndex,
		vec_size, vec_type, normalize,
		struct_size, struct_offset );
}
void RrMaterial::bindAttributeI ( int attributeIndex, const uint vec_size, const uint vec_type, const int struct_size, const void* struct_offset )
{
	RrPassForward::enabled_attributes[attributeIndex] = true;
	glEnableVertexAttribArray( attributeIndex );
	glVertexAttribIPointer( attributeIndex,
		vec_size, vec_type,
		struct_size, struct_offset );
}