
#ifndef _C_TREE_LEAF_RENDERER_H_
#define _C_TREE_LEAF_RENDERER_H_

#include <vector>

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

class CTreeBase;

class CTreeLeafRenderer : public CRenderableObject, public CGameBehavior
{

public:
	CTreeLeafRenderer ( void );
	~CTreeLeafRenderer ( void );

	void Update ( void );
	void PostUpdate ( void );

	void Interrupt ( void );

	bool Render ( const char pass );

private:
	std::vector<CTreeBase*>	vTreeList;
	bool				bIsUpdating;
	bool				bNeedRegen;
	bool				bVBOsInvalid;
	uint				iCurrentUpdateIndex;

	uint estimateVert;
	uint estimateTris;

	uint trisBuffer;
	uint vertBuffer;

	uint trisFrontBuffer;
	uint vertFrontBuffer;

	uint iTriangleCount;

	uint iVertexOffset;
	uint iTriangleOffset;
};


#endif