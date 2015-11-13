
// Includes
#include "CTextureMaster.h"
#include "core/debug/console.h"

// Instance Define
CTextureMaster TextureMaster;

// Constructor
CTextureMaster::CTextureMaster ( void )
{

}

// Destructor
CTextureMaster::~CTextureMaster ( void )
{

}

// Add a reference to a mesh set
glTexture* CTextureMaster::AddReference ( CTexture* ptex )
{
	// First look for it
	std::map<string,TexmapSetReference>::iterator it;

	it = textureMap.find( ptex->sFilename );
	// It's not here
	if ( it == textureMap.end() )
	{
#ifdef _ENGINE_DEBUG
		std::cout << "New texture reference: " << ptex->sFilename << std::endl; //todo: comment out
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
		textureList.push_back( std::pair<CTexture*,uint>(ptex,ptex->info.index) );
	}

}
glTexture* CTextureMaster::AddReference ( string texMapSId, CTexture* ptex )
{
	// First look for it
	std::map<string,TexmapSetReference>::iterator it;

	it = textureMap.find( texMapSId );
	// It's not here
	if ( it == textureMap.end() )
	{
#ifdef _ENGINE_DEBUG
		std::cout << "New texture reference: " << texMapSId << std::endl; //todo: comment out
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
		textureList.push_back( std::pair<CTexture*,uint>(ptex,ptex->info.index) );
	}
}

// Decrement the target reference
void CTextureMaster::RemoveReference ( CTexture* ptex )
{
	// First look for it
	std::map<string,TexmapSetReference>::iterator it;

	it = textureMap.find( ptex->sFilename );
	if ( it == textureMap.end() )
	{
		if ( ptex->ClassType() != TextureClassRenderTarget && ptex->ClassType() != TextureClassRenderTarget_Cube && ptex->ClassType() != TextureClassRenderTarget_MRT ) {
			Debug::Console->PrintWarning( "Attempting to free a non-existant texture reference: " + ptex->sFilename + "\n" );
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
void CTextureMaster::RemoveReference ( string texMapSId, CTexture* ptex )
{
	// First look for it
	std::map<string,TexmapSetReference>::iterator it;

	it = textureMap.find( texMapSId );
	if ( it == textureMap.end() )
	{
		Debug::Console->PrintWarning( "Attempting to free a non-existant texture reference: " + texMapSId + "\n" );
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
const glTexture* CTextureMaster::GetReference ( CTexture* ptex )
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
const glTexture* CTextureMaster::GetReference ( string texMapSId, CTexture* ptex )
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
void CTextureMaster::ReloadAll ( void )
{
	std::map<string,TexmapSetReference>::iterator it;

	CTexture* tempTexture;
	string tempName;
	for ( it = textureMap.begin(); it != textureMap.end(); ++it )
	{
		tempName = it->first;
		if ( tempName.find("__hx_") != string::npos ) {
			continue;	// Skip system textures
		}

		tempTexture = new CTexture( tempName,
			it->second.texture.info.type, it->second.texture.info.internalFormat,
			1024,1024, it->second.texture.info.repeatX, it->second.texture.info.repeatY );
		tempTexture->Reload();
		delete tempTexture;
	}
}

// Change all matching texture indices
void CTextureMaster::UpdateTextureIndex ( const uint n_old, const uint n_new )
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