
#include "glMaterial.h"

#include "core/math/Vector3d.h"
#include "core/settings/CGameSettings.h"

/*#include "CRenderableObject.h"
#include "CLight.h"
#include "CCamera.h"
#include "CRTCamera.h"*/
#include "renderer/texture/CRenderTexture.h"
#include "renderer/system/glMainSystem.h"

using namespace std;


//Texture set
void		glMaterial::setTexture ( const uint n_index, CTexture* n_texture )
{
	GL_ACCESS
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
void		glMaterial::setSampler ( const uint n_index, const uint n_sampler, const uint n_sampler_target )
{
	GL_ACCESS
#ifdef _ENGINE_DEBUG
	if ( n_sampler_target == 0 )
	{
		throw Core::InvalidArgumentException();
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
CTexture*	glMaterial::getTexture ( const uint n_index )
{
	return m_highlevel_storage[n_index];
}

// Shader
glShader*	glMaterial::getUsingShader ( void )
{
	glShader*	shader = NULL;
	if ( /*CGameSettings::Active()->i_ro_RendererMode*/m_currentShaderState == RENDER_MODE_FORWARD ) {
		shader = passinfo[current_pass].shader;
	}
	else if ( /*CGameSettings::Active()->i_ro_RendererMode*/m_currentShaderState == RENDER_MODE_DEFERRED ) {
		shader = deferredinfo[current_pass].shader;
	}
	else {
		throw std::exception();
	}
	return shader;
}

// ==Shader Constants==
void glMaterial::setUniform ( const char* sUniformName, float const fInput )
{
	glShader* shader = getUsingShader();
	if (( shader == NULL )||( !CGameSettings::Active()->b_ro_EnableShaders )) {
		return;
	}
	if ( glMaterial::current != this ) {
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}
	int uniformLocation = shader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 ) {
		glUniform1f( uniformLocation, fInput );
	}
}
void glMaterial::setUniform ( const char* sUniformName, Vector2d const& vInput )
{
	glShader* shader = getUsingShader();
	if (( shader == NULL )||( !CGameSettings::Active()->b_ro_EnableShaders )) {
		return;
	}
	if ( glMaterial::current != this ) {
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}
	int uniformLocation = shader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 ) {
		glUniform2f( uniformLocation, vInput.x, vInput.y );
	}
}
void glMaterial::setUniform ( const char* sUniformName, Vector3d const& vInput )
{
	glShader* shader = getUsingShader();
	if (( shader == NULL )||( !CGameSettings::Active()->b_ro_EnableShaders )) {
		return;
	}
	if ( glMaterial::current != this ) {
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}
	int uniformLocation = shader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 ) {
		glUniform3f( uniformLocation, vInput.x, vInput.y, vInput.z );
	}
}
void glMaterial::setUniform ( const char* sUniformName, Color const& cInput )
{
	glShader* shader = getUsingShader();
	if (( shader == NULL )||( !CGameSettings::Active()->b_ro_EnableShaders )) {
		return;
	}
	if ( glMaterial::current != this ) {
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}
	int uniformLocation = shader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 ) {
		glUniform4f( uniformLocation, cInput.red, cInput.green, cInput.blue, cInput.alpha );
	}
}
void glMaterial::setUniform ( const char* sUniformName, Matrix4x4 const& matxInput )
{
	glShader* shader = getUsingShader();
	if (( shader == NULL )||( !CGameSettings::Active()->b_ro_EnableShaders )) {
		return;
	}
	if ( glMaterial::current != this ) {
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}
	int uniformLocation = shader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 ) {
		glUniformMatrix4fv( uniformLocation, 1,false, matxInput[0]  );
	}
}
/*
void glMaterial::setUniformV( const char* sUniformName, unsigned int count, const Matrix4x4 * pMatxInput )
{
	glShader* shader = getUsingShader();
	if (( shader == NULL )||( !CGameSettings::Active()->b_ro_EnableShaders )) {
		return;
	}
	if ( glMaterial::current != this ) {
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}
	int uniformLocation = shader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 ) {
		glUniformMatrix4fv( uniformLocation, count,false, pMatxInput[0][0]  );
	}
}
void glMaterial::setUniformV( const char* sUniformName, unsigned int count, const Matrix3x3 * pMatxInput )
{
	glShader* shader = getUsingShader();
	if (( shader == NULL )||( !CGameSettings::Active()->b_ro_EnableShaders )) {
		return;
	}
	if ( glMaterial::current != this ) {
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}
	int uniformLocation = shader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 ) {
		glUniformMatrix3fv( uniformLocation, count,false, pMatxInput[0][0]  );
	}
}

void glMaterial::setUniformV ( const char* sUniformName, unsigned int count, const Vector3d* pVInput )
{
	glShader* shader = getUsingShader();
	if (( shader == NULL )||( !CGameSettings::Active()->b_ro_EnableShaders )) {
		return;
	}
	if ( glMaterial::current != this ) {
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}
	int uniformLocation = shader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 ) {
		glUniform3fv( uniformLocation, count, &(pVInput[0].x)  );
	}
}
*/