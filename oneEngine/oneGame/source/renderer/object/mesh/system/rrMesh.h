#ifndef RR_MESH_H_
#define RR_MESH_H_

#include "core/types/ModelData.h"
#include "core/containers/arstring.h"
#include "renderer/types/types.h"
#include "renderer/types/ModelStructures.h"
#include "renderer/material/RrShader.cbuffers.h"
#include "renderer/gpuw/Buffers.h"

// will translate an input arModelData structure into a collection of vertex buffers
// stores which vertex buffer attributes are valid and whatnot
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

#endif//RR_MESH_H_