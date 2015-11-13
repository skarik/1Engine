
#ifndef _C_DEBUG_RT_INSPECTOR_H_
#define _C_DEBUG_RT_INSPECTOR_H_

// Includes
#include <vector>

#include "renderer/object/CRenderableObject.h"
class CRenderTexture;

namespace Debug
{
	// Using
	//using std::vector;

	// Class Definition
	class CDebugRTInspector : public CRenderableObject
	{
	public:
		// Constructor and destructor
		RENDER_API explicit	CDebugRTInspector ( void );
		RENDER_API			~CDebugRTInspector ( void );
	public:
		bool		BeginRender ( void ) override;
		bool		Render ( const char pass ) override;

	public:
		RENDER_API void AddWatch ( CRenderTexture* rtToWatch );
		RENDER_API void RemoveWatch ( CRenderTexture* rtToUnwatch );

	private:
		bool bDrawRTs;
		std::vector<CRenderTexture*>  rtList;
		glMaterial* defaultMat;
	};

	// Typedef for ease of use
	RENDER_API extern CDebugRTInspector* RTInspector;
};

#endif
