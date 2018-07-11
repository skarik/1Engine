
// Includes
#include "RrTextureMaster.h"
#include "core/debug/console.h"

// Instance Define
RrTextureMaster TextureMaster;

// Constructor
RrTextureMaster::RrTextureMaster ( void )
{

}

// Destructor
RrTextureMaster::~RrTextureMaster ( void )
{

}

// Add a reference to a mesh set
textureEntry_t* RrTextureMaster::AddReference ( RrTexture* ptex )
{
	// First look for it
	std::map<string,TexmapSetReference>::iterator it;

	it = textureMap.find( ptex->sFilename );
	// It's not here
	if ( it == textureMap.end() )
	{
#ifdef _ENGINE_DEBUG
		//std::cout << "New texture reference: " << ptex->sFilename << std::endl;
#endif
		// So create a new reference
		TexmapSetReference newRef;
		newRef.referenceCount = 1;
		// Copy the data
		newRef.texture.info = ptex->info;

		textureMap[ ptex->sFilename ] = newRef;

		//return &(newRef.texture);
		return &(textureMap[ ptex->sFilename ].texture);
	}
	else
	{
		// Increment the reference count
		it->second.referenceCount += 1;
		return &(it->second.texture);
	}

	// Now look in other list
	auto lit = textureList.begin();
	while ( lit != textureList.end() )
	{
		if ( lit->first == ptex ) {
			break;
		}
		++lit;
	}
	if ( lit == textureList.end() ) {
		textureList.push_back( std::pair<RrTexture*,uint>(ptex,ptex->info.index) );
	}

}
textureEntry_t* RrTextureMaster::AddReference ( string texMapSId, RrTexture* ptex )
{
	// First look for it
	std::map<string,TexmapSetReference>::iterator it;

	it = textureMap.find( texMapSId );
	// It's not here
	if ( it == textureMap.end() )
	{
#ifdef _ENGINE_DEBUG
		//std::cout << "New texture reference: " << texMapSId << std::endl;
#endif
		// So create a new reference
		TexmapSetReference newRef;
		newRef.referenceCount = 1;
		// Copy the data
		newRef.texture.info = ptex->info;

		textureMap[ texMapSId ] = newRef;

		//return &(newRef.texture);
		return &(textureMap[ texMapSId ].texture);
	}
	else
	{
		// Increment the reference count
		it->second.referenceCount += 1;
		return &(it->second.texture);
	}

	// Now look in other list
	auto lit = textureList.begin();
	while ( lit != textureList.end() )
	{
		if ( lit->first == ptex ) {
			break;
		}
		++lit;
	}
	if ( lit == textureList.end() ) {
		textureList.push_back( std::pair<RrTexture*,uint>(ptex,ptex->info.index) );
	}
}

// Decrement the target reference
void RrTextureMaster::RemoveReference ( RrTexture* ptex )
{
	// First look for it
	std::map<string,TexmapSetReference>::iterator it;

	it = textureMap.find( ptex->sFilename );
	if ( it == textureMap.end() )
	{
		if ( ptex->ClassType() != TextureClassRenderTarget && ptex->ClassType() != TextureClassRenderTarget_Cube && ptex->ClassType() != TextureClassRenderTarget_MRT ) {
			debug::Console->PrintWarning( "Attempting to free a non-existant texture reference: " + ptex->sFilename + "\n" );
		}
	}
	else
	{
		it->second.referenceCount -= 1;
	}

	// Now look in other list
	auto lit = textureList.begin();
	while ( lit != textureList.end() )
	{
		if ( lit->first == ptex ) {
			break;
		}
		++lit;
	}
	if ( lit != textureList.end() ) {
		textureList.erase( lit );
	}
}
// Decrement the target reference
void RrTextureMaster::RemoveReference ( string texMapSId, RrTexture* ptex )
{
	// First look for it
	std::map<string,TexmapSetReference>::iterator it;

	it = textureMap.find( texMapSId );
	if ( it == textureMap.end() )
	{
		debug::Console->PrintWarning( "Attempting to free a non-existant texture reference: " + texMapSId + "\n" );
	}
	else
	{
		it->second.referenceCount -= 1;
	}

	// Now look in other list
	auto lit = textureList.begin();
	while ( lit != textureList.end() )
	{
		if ( lit->first == ptex ) {
			break;
		}
		++lit;
	}
	if ( lit != textureList.end() ) {
		textureList.erase( lit );
	}
}

// Returns a pointer to the wanted model data, null if not found
const textureEntry_t* RrTextureMaster::GetReference ( RrTexture* ptex )
{
	// First look for it
	std::map<string,TexmapSetReference>::iterator it;

	it = textureMap.find( ptex->sFilename );
	// Find the reference
	if ( it == textureMap.end() )
	{
		return NULL;
	}
	else
	{
		// Return it booyah
		return &(it->second.texture);
	}
}
// Returns a pointer to the wanted model data, null if not found
const textureEntry_t* RrTextureMaster::GetReference ( string texMapSId, RrTexture* ptex )
{
	// First look for it
	std::map<string,TexmapSetReference>::iterator it;

	it = textureMap.find( texMapSId );
	// Find the reference
	if ( it == textureMap.end() )
	{
		return NULL;
	}
	else
	{
		// Return it booyah
		return &(it->second.texture);
	}
}


// Reload all textures that are loaded
void RrTextureMaster::ReloadAll ( void )
{
	std::map<string,TexmapSetReference>::iterator it;

	RrTexture* tempTexture;
	string tempName;
	for ( it = textureMap.begin(); it != textureMap.end(); ++it )
	{
		tempName = it->first;
		if ( tempName.find("__hx_") != string::npos ) {
			continue;	// Skip system textures
		}

		tempTexture = new RrTexture( tempName,
			it->second.texture.info.type, it->second.texture.info.internalFormat,
			1024,1024, it->second.texture.info.repeatX, it->second.texture.info.repeatY );
		tempTexture->Reload();
		delete tempTexture;
	}
}

// Change all matching texture indices
void RrTextureMaster::UpdateTextureIndex ( const uint n_old, const uint n_new )
{
	if ( n_old == n_new ) return;

	auto lit = textureList.begin();
	while ( lit != textureList.end() )
	{
		if ( lit->second == n_old ) {
			lit->second = n_new;
		}
		++lit;
	}
}