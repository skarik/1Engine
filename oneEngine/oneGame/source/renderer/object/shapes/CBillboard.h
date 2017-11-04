#ifndef _C_BILLBOARD_H_
#define _C_BILLBOARD_H_

// Includes
#include "../CRenderableObject.h"
#include "renderer/material/RrMaterial.h"
#include <vector>

// Class Definition
class CBillboard : public CRenderableObject
{
public:
	RENDER_API CBillboard ( void );
	RENDER_API ~CBillboard ( void );

	RENDER_API bool PreRender ( void ) override;
	RENDER_API bool Render ( const char pass ) override;

	//void SetMaterial ( RrMaterial* );

	void SetSize ( Real newSize )
	{
		m_size = newSize;
	};
public:
	// If billboard should point at camera (true) or match the camera orientation (false).
	// Defaults to match camera orientation (false).
	bool			m_curvetowards;
protected:
	Real			m_size;
	glHandle		m_vbo;

};

#endif