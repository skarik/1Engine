#ifndef RENDERER_DEBUG_RT_INSPECTOR_H_
#define RENDERER_DEBUG_RT_INSPECTOR_H_

// Includes
#include <vector>

#include "renderer/object/CRenderableObject.h"
class RrRenderTexture;

namespace debug
{
	class RrDebugRTInspector : public CRenderableObject
	{
	public:
		// Constructor and destructor
		RENDER_API explicit	RrDebugRTInspector ( void );
		RENDER_API			~RrDebugRTInspector ( void );
	public:
		bool		BeginRender ( void ) override;

		bool		PreRender ( void ) override;
		bool		Render ( const char pass ) override;

	public:
		RENDER_API void AddWatch ( RrRenderTexture* rtToWatch );
		RENDER_API void RemoveWatch ( RrRenderTexture* rtToUnwatch );

	private:
		bool bDrawRTs;
		std::vector<RrRenderTexture*>  rtList;
	};

	// Publicly available RTInspector~
	RENDER_API extern RrDebugRTInspector* RTInspector;
};

#endif