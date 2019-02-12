#ifndef RR_MESH_H_
#define RR_MESH_H_

#include "core/types/ModelData.h"
#include "core/containers/arstring.h"
#include "renderer/types/types.h"
#include "renderer/types/ModelStructures.h"
#include "renderer/types/shaders/vattribs.h"
#include "renderer/gpuw/Buffers.h"

/*enum rrMeshBufferType
{
	kMeshBuffer_Normal,
	kMeshBuffer_Skinned,
};*/

// Skinning information
/*struct rrMeshSkinningData
{
	uint32_t	bonecount;
	//glHandle	textureBuffer;
	gpuHandle	textureBufferData;
	gpuHandle	textureBufferData_Swap;
	//glHandle	textureSoftBuffer;
	//glHandle	textureSoftBufferData;
	//glHandle	textureSoftBufferData_Swap;
};*/

class rrMeshBuffer
{
public:
	RENDER_API				rrMeshBuffer ( void );
	RENDER_API				~rrMeshBuffer ( void );

	//
	// Setters:

	//	InitIdentifier( name )
	//RENDER_API void			InitParameters ( const char* identifierName );

	//	InitMeshBuffers( mesh ) : Creates mesh buffers based on the model data.
	// Will call FreeMeshBuffers() if there is already a mesh loaded up.
	// Will hold onto the input model data.
	RENDER_API void			InitMeshBuffers ( arModelData* const modelData );
	
	//	FreeMeshBuffers() : Frees mesh buffers
	RENDER_API void			FreeMeshBuffers ( void );
	
	//	StreamMeshBuffers( mesh ) : Updates contents of mesh buffers based the model data.
	// Performs similar actions to InitMeshBuffers, but is optimized for per-frame streaming..
	// Will hold onto the input model data, as it may not be finished when the function exits.
	RENDER_API void			StreamMeshBuffers ( arModelData* const modelData );

	//
	// Getters:

	//	GetName() : Returns the identifier
	//RENDER_API const char*	GetName ( void ) const
	//	{ return m_name.c_str(); }

public:
	gpu::Buffer			m_buffer [renderer::shader::kVBufferSlotMaxCount];
	bool				m_bufferEnabled [renderer::shader::kVBufferSlotMaxCount];
	gpu::Buffer			m_indexBuffer;

	//uint16_t			m_boneCount;
	//gpu::Buffer			m_boneBuffer;

	arModelData*		m_modeldata;

	bool				m_mesh_uploaded;

protected:
	
	//bool				m_mesh_uploaded;
	//arstring64			m_name;

};

#if 0
// will translate an input arModelData structure into a collection of vertex buffers
// stores which vertex buffer attributes are valid and whatnot
// rrMesh's are shared between CMesh's
class rrMesh
{
public:
							// Constructor
							rrMesh ( void );
							// Destructor
	virtual					~rrMesh ( void );

	// Create a new VBO associated with this rrMesh
	// Removes any old VBO data.
	// This object gains ownership of the model data. Well, it should, or you'll get memory errors and shit. And that's bad.
	virtual void			Initialize ( const char* nNewName, arModelData* const pNewModelData, unsigned int frames=1, bool willStream=false );
	// Sends new VBO data to the mesh
	virtual void			Restream ( void );

	// Recalculates the normals per-triangle.
	RENDER_API void			RecalculateNormals ( void );
	// Recalculates the tangents and binormals
	RENDER_API void			RecalculateTangents ( void );

	// Getters of R-O data
	RENDER_API const char*	GetName ( void ) const;

protected:
	// Frees up the GPU data used by the object
	/*void			FreeVBOData ( void );
	// Frees up the RAM data used by the object
	void			FreeRAMData ( void );
	// Sets things to the VBO's again
	void			Reinitialize ( void );*/

public:
	/*enum eUserDataType
	{
		kUserdataTypeNone = 0,
		kUserdataTypeBonePointer = 1,
		kUserdataTypeString = 2
	};*/

public:

	// Model data
	arModelData*		modeldata;
	// Userdata
	/*void*			userdata;
	eUserDataType	userdata_type;

	glHandle	GetVBOverts ( void ) {
		return iVBOverts;
	}
	glHandle	GetVBOfaces ( void ) {
		return iVBOfaces;
	}*/

	gpu::VertexBuffer	m_buffer [renderer::kAttributeMaxCount];
	bool				m_bufferEnabled [renderer::kAttributeMaxCount];

protected:
	//bool	bReady;
	//bool	bShaderSetup;

	//glHandle	iVBOverts;
	//glHandle	iVBOfaces;

	arstring64	name;
};
#endif

#endif//RR_MESH_H_