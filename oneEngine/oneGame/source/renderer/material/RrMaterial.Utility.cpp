#include "RrMaterial.h"

#include "core/math/Vector3d.h"
#include "core/settings/CGameSettings.h"

#include "renderer/texture/RrRenderTexture.h"
#include "renderer/system/glMainSystem.h"

using namespace std;

//Texture set
void		RrMaterial::setTexture ( const textureslot_t n_index, RrTexture* n_texture )
{
	GL_ACCESS;
	// Check doubly set texture
	if ( m_highlevel_storage[n_index] == n_texture ) {
		return;
	}
	// Clear off existing texture
	if ( m_highlevel_storage[n_index] != NULL )
	{
		m_highlevel_storage[n_index]->RemoveReference();
		if ( !m_highlevel_storage[n_index]->HasReference() ) {
			delete m_highlevel_storage[n_index];
		}
	}
	// Set new texture
	if ( n_texture )
	{
		n_texture->AddReference();
		m_highlevel_storage[n_index]= n_texture;
		m_samplers[n_index]			= n_texture->GetColorSampler();
		m_sampler_targets[n_index]	= GL.Enum( n_texture->GetSamplerTarget() );
	}
	else
	{
		m_highlevel_storage[n_index]= NULL;
		m_samplers[n_index]			= 0;
		m_sampler_targets[n_index]	= 0;
	}
}
void		RrMaterial::setSampler ( const textureslot_t n_index, const uint n_sampler, const uint n_sampler_target )
{
#ifdef _ENGINE_DEBUG
	if ( n_sampler_target == 0 )
	{
		throw core::InvalidArgumentException();
	}
#endif
	// Clear off existing texture
	if ( m_highlevel_storage[n_index] != NULL )
	{
		m_highlevel_storage[n_index]->RemoveReference();
		if ( !m_highlevel_storage[n_index]->HasReference() ) {
			delete m_highlevel_storage[n_index];
		}
	}
	// Set new texture
	m_highlevel_storage[n_index]= NULL;
	m_samplers[n_index]			= n_sampler;
	m_sampler_targets[n_index]	= n_sampler_target;
}
RrTexture*	RrMaterial::getTexture ( const textureslot_t n_index )
{
	return m_highlevel_storage[n_index];
}

// Shader
RrShader*	RrMaterial::getUsingShader ( void )
{
	return m_currentShader;
}
