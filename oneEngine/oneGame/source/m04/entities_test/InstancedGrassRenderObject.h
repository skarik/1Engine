#ifndef M04_ENTITY_TEST_INSTANCED_GRASS_RENDER_OBJECT_H_
#define M04_ENTITY_TEST_INSTANCED_GRASS_RENDER_OBJECT_H_

#include "renderer/object/RrRenderObject.h"

#include "core/math/Vector3.h"
#include "renderer/object/RrRenderObject.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"

struct grInstancedGrassInfo
{
	Matrix4x4			transform; 
	Vector3f			color;
};

struct grInstancedDataGrassTransform
{
	Matrix4x4			transform; 
};
struct grInstancedDataGrassVariation
{
	Vector3f			color;
	int32				index;
};

class InstancedGrassRenderObject : public RrRenderObject
{
public:
	GAME_API				InstancedGrassRenderObject ( void );
	GAME_API				~InstancedGrassRenderObject ( void );

	//		PreRender()
	// Push the uniform properties
	GAME_API bool			PreRender ( rrCameraPass* cameraPass ) override;

	//		Render()
	// Render the model using the 2D engine's style
	GAME_API bool			Render ( const rrRenderParams* params ) override;

	//	EndRender() : Called after the render-loop executes, outside of the world loop.
	// Called once per frame.
	GAME_API bool			EndRender ( void ) override;

public:
	std::vector<grInstancedGrassInfo>
						m_grassInfo;

protected:
	struct bufferFrameInfo
	{
		gpu::Buffer		buffer_transforms;
		gpu::Buffer		buffer_variations;

		std::vector<grInstancedDataGrassTransform>
						transforms;
		std::vector<grInstancedDataGrassVariation>
						variations;

		int32			instance_count = 0;
	};

	// TODO: Allow for multiple outputs. For now, for simplicity, we assume one output.

	bufferFrameInfo		instancing_info;

	// Mesh information
	uint				m_indexCount;
	// GPU information
	rrMeshBuffer		m_meshBuffer;
};

#endif//M04_ENTITY_TEST_INSTANCED_GRASS_RENDER_OBJECT_H_