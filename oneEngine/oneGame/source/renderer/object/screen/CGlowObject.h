
#ifndef _C_GLOW_OBJECT_H_
#define _C_GLOW_OBJECT_H_

#include "renderer/object/CRenderableObject.h"

class CModel;

class CGlowObject : public CRenderableObject
{
public:
	RENDER_API explicit CGlowObject ( const CModel* n_model );

	bool Render ( const char pass ) override;

public:
	Color	m_color;
	Real	m_width;

private:
	CModel*	model;
};

#endif//_C_GLOW_OBJECT_H_