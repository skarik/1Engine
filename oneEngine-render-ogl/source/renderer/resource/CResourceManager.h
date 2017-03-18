//===============================================================================================//
//	class CResourceManager
// Class that performs main loading operations of renderer-used resources.
// The current active one may be accessed with "CRenderState::Active->mResourceManager"
//===============================================================================================//

#ifndef _C_RESOURCE_MANAGER_H_
#define _C_RESOURCE_MANAGER_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"

#include <vector>
#include <map>
#include <string>
#include <thread>
#include <atomic>

class CRenderState;
class CTexture;

//	struct resourceManagerSettings_t
// Settings structure for configuring behavior of the resource manager.
struct resourceManagerSettings_t
{
	// Are textures streamed?
	bool streamTextures;

	//	Constructor
	// Set default values
	resourceManagerSettings_t ()
		: streamTextures(true)
	{
		;
	}
};

//	CResourceManager
// Class that performs main loading operations of renderer-used resources.
// The current active one may be accessed with "CRenderState::Active->mResourceManager"
class CResourceManager
{
public:
	explicit CResourceManager ( void );
			~CResourceManager( void );

	//=========================================//
	// Frame Step
	//=========================================//

	// In non-release mode, checks files for changes
	void FileUpdate ( void );

	// Step per frame for updating the streaming system
	void RenderUpdate ( void );

	//=========================================//
	// Texture Streaming
	//=========================================//

	// Mark resources. Based on what?
	// How to interact with model manager, shader manager, and texture manager? Should we mess with them?
	// Idea: leave previous managers. Use this resource manager to set timings for the other manager update systems.
	void AddResource ( CTexture* );
	// removes from the source lsit
	void RemoveResource ( CTexture* );

	// Force load resources
	void ForceLoadResource ( CTexture* );
	// Finish load resource
	void FinishAddResource ( CTexture* );

public:
	//=========================================//
	// Public settings
	//=========================================//

	// Settings struct for setting and storing current mode of the resource manager.
	// Changing this during runtime will cause any race conditions or unexpected behavior.
	resourceManagerSettings_t settings;

private:
	// Allow the current rendering system to access this
	friend CRenderState;
	bool	m_renderStateOwned;
	
	//	struct resourceInfo_t
	// Structure storing current load state of a resource.
	struct resourceInfo_t
	{
	public:
		// Resource type enumeration
		enum eRESOURCE_TYPE {
			RESOURCE_MODEL,
			RESOURCE_TEXTURE,
			RESOURCE_RENDERTEXTURE,
			RESOURCE_SHADER
		};

	public:
		// Does item need to be reloaded into memory?
		bool	m_needReload;
		// Does item need streaming?
		bool	m_needStream;
		// Item's current stream state?
		int		m_streamState;
		
		eRESOURCE_TYPE	m_resourceType;
		void*	m_resource;
	};

	//	enum eSTREAM_STATE
	// Constants for current streaming state
	enum eSTREAM_STATE {
		S_NEED_STREAM = 0,
		S_STREAMING,
		S_NEED_GPU_UPLOAD,
		S_DONE
	};

	//=========================================//
	// Internal state
	//=========================================//

	std::vector<resourceInfo_t> mResourceList;
	std::thread mStreamThread;
	std::atomic<bool> mStreamFlag;

	//=========================================//
	// Internal streaming routines
	//=========================================//

	void StreamUpdate_Texture ( const char* n_targetFile );

};

#define RESOURCE_GET_TEXTURE(name, conditional_texture) conditional_texture

namespace Renderer
{
	static const char* TextureBlack = "black";
	static const char* TextureWhite = "white";
	static const char* TextureGrayA0 = "graya0";

	//	CResourceManager
	// Class that manages texture references in a fun and dirty way.
	class Resources
	{
	public:
		RENDER_API static CTexture*	GetTexture ( const std::string& identifier )
		{
			return GetTexture( identifier.c_str() );
		}
		RENDER_API static CTexture*	GetTexture ( const char* identifier );

		RENDER_API static void		AddTexture ( const std::string& identifier, CTexture* texture = (CTexture*)0xF33D )
		{
			return AddTexture( identifier.c_str(), texture );
		}
		RENDER_API static void		AddTexture ( const char* identifier, CTexture* texture = (CTexture*)0xF33D );

	protected:
		static std::map<arstring128, CTexture*> textureMap;
	};
}

#endif//_C_RESOURCE_MANAGER_H_