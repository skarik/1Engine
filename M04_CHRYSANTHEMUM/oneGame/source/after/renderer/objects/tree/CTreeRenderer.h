
#ifndef _C_TREE_RENDERER_H_
#define _C_TREE_RENDERER_H_

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"
#include "after/entities/foliage/CTreeBase.h"

class CTreeRenderer : public CRenderableObject, public CGameBehavior
{

public:
	CTreeRenderer ( void );
	~CTreeRenderer ( void );

	void AttemptMemoryAllocation ( void );

	void Update ( void ) {;}
	void LateUpdate ( void );
	void PostUpdate ( void );

	//void Interrupt ( void );

	bool Render ( const char pass );

	void UpdateTreeBuffer ( sTreeBufferData* treeBuffer, const CModelData* md );
	void UpdateLeafBuffer ( sTreeBufferData* treeBuffer, const CModelData* md );
	void FreeTreeBuffer ( sTreeBufferData* treeBuffer );
	void FreeLeafBuffer ( sTreeBufferData* treeBuffer );

	uint32_t	maxVertCount;
	uint32_t	maxTreeTriCount;
	uint32_t	maxLeafTriCount;
	uint32_t	maxTreeCount;

	CModelVertex*	vertexData;
	CModelTriangle*	treeTriData;
	CModelTriangle*	leafTriData;
	int32_t*		treeTriTable;
	int32_t*		leafTriTable;

	uint32_t		openVertex;
	uint32_t		lastRecordedVertex;

	uint32_t		treeTriCount;
	uint32_t		leafTriCount;

private:
	/*vector<CTreeBase*>	vTreeList;
	bool				bIsUpdating;
	bool				bVBOsInvalid;
	uint				iCurrentUpdateIndex;

	uint estimateVert;
	uint estimateTris;

	uint trisBuffer;
	uint vertBuffer;

	uint trisFrontBuffer;
	uint vertFrontBuffer;

	uint iTriangleCount;*/

	uint vertBuffer;
	uint treeTrisBuffer;
	uint leafTrisBuffer;


};


#endif