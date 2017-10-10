// Include the main system
#include "glMainSystem.h"
#include "renderer/state/Settings.h"


void	glMainSystem::setupFog ( void )
{
	// Set the fog
	/*glFogf( GL_FOG_START, renderer::Settings.fogStart );
	glFogf( GL_FOG_END, renderer::Settings.fogEnd );
	glFogfv( GL_FOG_COLOR, renderer::Settings.fogColor.start_point() );
	glEnable( GL_FOG );*/

	throw core::DeprecatedCallException();

	bFogEnabled = true;
}
bool	glMainSystem::FogEnabled ( void ) {
	//bFogEnabled = glIsEnabled( GL_FOG );
	return bFogEnabled;
}
void	glMainSystem::DisableFog ( void ) {
	if ( bFogEnabled )
	{
		//glDisable( GL_FOG );
		bFogEnabled = false;
	}
}
void	glMainSystem::EnableFog ( void ) {
	if ( !bFogEnabled )
	{
		//glEnable( GL_FOG );
		bFogEnabled = true;
	}
}


void	glMainSystem::clearColor ( void )
{
	glClearColor( renderer::Settings.clearColor.red, renderer::Settings.clearColor.green, renderer::Settings.clearColor.blue, renderer::Settings.clearColor.alpha );
}
void	glMainSystem::clearColor ( const Color& clearColor )
{
	glClearColor( clearColor.red, clearColor.green, clearColor.blue, clearColor.alpha );
}


bool	glMainSystem::DepthWriteEnabled ( void )
{
	return bDepthWriteEnabled;
}
void	glMainSystem::EnableDepthWrite ( void )
{
	if ( !bDepthWriteEnabled ) {
		glDepthMask( true );
		bDepthWriteEnabled = 1;
	}
}
void	glMainSystem::DisableDepthWrite ( void )
{
	if ( bDepthWriteEnabled ) {
		glDepthMask( false );
		bDepthWriteEnabled = 0;
	}
}


uint get_blend_mode_mask ( const eBlendModes bm )
{
	uint result = 0;
	switch ( bm )
	{
	case BM_ZERO:
		result = GL_ZERO;
		break;
	case BM_ONE:
		result = GL_ONE;
		break;
	case BM_SRC_COLOR:
		result = GL_SRC_COLOR;
		break;
	case BM_SRC_ALPHA:
		result = GL_SRC_ALPHA;
		break;
	case BM_INV_SRC_COLOR:
		result = GL_INVERT | GL_SRC_COLOR;
		break;
	case BM_INV_SRC_ALPHA:
		result = GL_INVERT | GL_SRC_ALPHA;
		break;
	case BM_DEST_COLOR:
		result = GL_SRC_COLOR;
		break;
	case BM_DEST_ALPHA:
		result = GL_SRC_ALPHA;
		break;
	case BM_INV_DEST_COLOR:
		result = GL_INVERT | GL_SRC_COLOR;
		break;
	case BM_INV_DEST_ALPHA:
		result = GL_INVERT | GL_SRC_ALPHA;
		break;
	}
	return result;
}
void	glMainSystem::BlendMode( const eBlendModes src, const eBlendModes dest )
{
	glEnum src_mask = get_blend_mode_mask( src );
	glEnum dest_mask = get_blend_mode_mask( dest );
	glBlendFunc( src_mask, dest_mask );
}