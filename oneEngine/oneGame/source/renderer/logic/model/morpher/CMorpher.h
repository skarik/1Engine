#ifndef RENDERER_MORPHER_MANAGER_H_
#define RENDERER_MORPHER_MANAGER_H_

#include "core/types/ModelData.h"
#include "core/containers/arstring.h"
#include <map>
#include "CMorpherSet.h"
#include "CMorphAction.h"

// Prototypes
class RrCModel;
class CSkinnedModel;
class rrMeshBuffer;
namespace gpu
{
	class GraphicsContext;
}

// Morpher class
// Used for badass morphing
class CMorpher
{
protected:
	friend CMorphAction;
	friend RrCModel;

public:
	// Dead action returned when an invalid index is used.
	static CMorphAction	m_DeadAction;

public:
	RENDER_API explicit		CMorpher ( const string &, const CMorpher* );
	RENDER_API explicit		CMorpher ( const string &, const CMorpherSet* );

	RENDER_API CMorpher&	operator= ( CMorpher const& );
	RENDER_API CMorpher&	operator= ( const CMorpher* );

	RENDER_API virtual		~CMorpher ( void );

	RENDER_API CMorphAction&
							operator [] ( const char* );
	RENDER_API CMorphAction&
							operator [] ( const int & );
	RENDER_API CMorphAction*
							FindAction ( const char* );

	RENDER_API void			AddAction ( CMorphAction );

	const CMorpherSet*		GetMorpherSet ( void ) const
		{ return pMorphSet; }
	bool					IsValid ( void )
		{ return bIsValid; }

	void					SetOwner ( RrCModel* model )
		{ pOwner = model; }

	//	FrameUpdate()
	// Updates the morph cbuffers and parameters needed for rendering.
	// This must be called every frame by the renderer.
	void					FrameUpdate ( void );
	
	//	SetVertexBuffers( gfx context, start slot )
	// Throws in used vertex buffers into given slot.
	void					SetVertexBuffers( gpu::GraphicsContext* gfx, int startSlot );
	//	SetBlendCBuffers ( gfx context, cbuffer slot )
	// Throws in previously generated blend cbuffer into given slot.
	void					SetBlendCBuffers ( gpu::GraphicsContext* gfx, int cbufferSlot );

protected:

	std::map<arstring<128>,CMorphAction>
						mActions;
	const CMorpherSet*	pMorphSet;
	string				sFilename;

	bool				bIsValid;

	RrCModel*			pOwner;
};

#endif//RENDERER_MORPHER_MANAGER_H_