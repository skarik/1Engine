// Mesh Interface.
// Pass it some vertices. It'll render them.
// THAT'S ALL IT DOES. TRANSFORMATIONS AND MATERIALS MUST BE APPLIED ELSEWHERE BEFORE RENDERING

#ifndef _GL_MESH_H_
#define _GL_MESH_H_

// Include vertex data
#include "core/types/ModelData.h"
#include "renderer/types/types.h"
// Include the OpenGL library
//#include "glMainSystem.h"

// Include string
#include <string>
using std::string;

// Prototype material
class glMaterial;

// Class Definition
class glMesh
{
public:
					// Constructor
					glMesh ( void );
					// Destructor
	virtual			~glMesh ( void );

	// Create a new VBO associated with this glMesh
	// Removes any old VBO data.
	// This object gains ownership of the model data. Well, it should, or you'll get memory errors and shit. And that's bad.
	virtual void	Initialize ( const string& nNewName, CModelData* const pNewModelData, unsigned int frames=1 );

	// Recalculates the normals per-triangle.
	void			RecalculateNormals ( void );
	// Smooths the normals
	void			SmoothNormals ( float val );

	// Render the mesh out
	virtual void	Render ( void );

	// Getters of R-O data
	const string&	GetName ( void ) const;

protected:
	// Frees up the data used by the object
	void			FreeVBOData ( void );
	// Recalculates the normals per-triangle.
	void			RecalculateNormals ( CModelData* modelData );
	// Sets things to the VBO's again
	void			Reinitialize ( void );

public:
	// Model data
	CModelData* pmData;
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
	glMaterial*	pmMat;

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