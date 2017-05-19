// Mesh Interface.
// Pass it some vertices. It'll render them.
// THAT'S ALL IT DOES. TRANSFORMATIONS AND MATERIALS MUST BE APPLIED ELSEWHERE BEFORE RENDERING

#ifndef _GL_MESH_H_
#define _GL_MESH_H_

// Include vertex data
#include "core/types/ModelData.h"
#include "renderer/types/types.h"
#include "renderer/types/ModelStructures.h"
// Include the OpenGL library
//#include "glMainSystem.h"

// Include string
#include <string>
using std::string;

// Prototype material
class RrMaterial;

// Class Definition
class rrMesh
{
public:
					// Constructor
					rrMesh ( void );
					// Destructor
	virtual			~rrMesh ( void );

	// Create a new VBO associated with this rrMesh
	// Removes any old VBO data.
	// This object gains ownership of the model data. Well, it should, or you'll get memory errors and shit. And that's bad.
	virtual void	Initialize ( const string& nNewName, arModelData* const pNewModelData, unsigned int frames=1, bool willStream=false );
	// Sends new VBO data to the mesh
	virtual void	Restream ( void );

	// Recalculates the normals per-triangle.
	RENDER_API void	RecalculateNormals ( void );
	// Smooths the normals
	void			SmoothNormals ( float val );

	// Render the mesh out
	//virtual void	Render ( void );

	// Getters of R-O data
	const string&	GetName ( void ) const;

protected:
	// Frees up the GPU data used by the object
	void			FreeVBOData ( void );
	// Frees up the RAM data used by the object
	void			FreeRAMData ( void );
	// Recalculates the normals per-triangle.
	void			RecalculateNormals ( arModelData* modelData );
	// Sets things to the VBO's again
	void			Reinitialize ( void );

public:
	// Model data
	arModelData* pmData;
	// Userdata
	void*		pbData;
	int			ibDataType;
	// Userdata enum
	enum eUserData
	{
		USERDATA_NONE = 0,
		USERDATA_BONE = 1,
		USERDATA_CSTRING = 2
	};
	// Material data
	RrMaterial*	pmMat;

	glHandle	GetVBOverts ( void ) {
		return iVBOverts;
	}
	glHandle	GetVBOfaces ( void ) {
		return iVBOfaces;
	}

protected:
	bool	bReady;
	bool	bShaderSetup;
	unsigned short meshNum;

	glHandle	iVBOverts;
	glHandle	iVBOfaces;

	string	mName;
	
};

#endif