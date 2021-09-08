#ifndef M04_ENTITY_TEST_INSTANCED_GRASS_RENDER_OBJECT_H_
#define M04_ENTITY_TEST_INSTANCED_GRASS_RENDER_OBJECT_H_

#include "renderer/object/RrRenderObject.h"

#include "core/math/Vector3.h"
#include "renderer/object/RrRenderObject.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"

class RrAnimatedMeshGroup;
class rrMeshBuffer;

struct grInstancedGrassInfo
{
	Matrix4x4			transform; 
	Vector3f			color;
	int					variation_index;
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

	//		LoadGrassMeshes(meshs, count)
	// Load the meshes that the grass variations pull from
	GAME_API void			LoadGrassMeshes ( const char* const* mesh_resources, const int mesh_count );

	//		PrepRender(cameraPass)
	// Push instance properties for this camera
	GAME_API bool			PrepRender ( rrCameraPass* cameraPass ) override;

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
	struct grassMeshType
	{
		RrAnimatedMeshGroup*
						m_meshGroup = nullptr;
		rrMeshBuffer*	m_meshBuffer = nullptr;
	};
	std::vector<grassMeshType>
						mesh_types;

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

	struct grassSubDrawInfo
	{
		int32			instance_count = 0;
		int32			base_offset = 0;
	};
	std::vector<grassSubDrawInfo>
						instancing_subdraw_info;

	// Mesh information
	//uint				m_indexCount;
	// GPU information
	//rrMeshBuffer		m_meshBuffer;
};

#endif//M04_ENTITY_TEST_INSTANCED_GRASS_RENDER_OBJECT_H_