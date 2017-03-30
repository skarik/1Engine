// Mesh Interface.
// Pass it some vertices and move its bones. It will render them.
// THAT'S ALL IT DOES. TRANSFORMATIONS AND MATERIALS MUST BE APPLIED ELSEWHERE BEFORE RENDERING

#ifndef _GL_SKINNED_MESH_H_
#define _GL_SKINNED_MESH_H_

// Include the OpenGL library
//#include "glMainSystem.h"
#include "rrMesh.h"
#include "physical/skeleton/skeletonBone.h"

// Include vertex data
//#include "CModelData.h"

// Include string
#include <string>
using std::string;
// Include vector
#include <vector>
//using std::vector;

// Boost Threads
//#include "boost\thread.hpp"
//using boost::thread;

// Class Definition
class rrSkinnedMesh : public rrMesh
{
public:
	// Constructor
	rrSkinnedMesh ( void );
	// Destructor
	virtual ~rrSkinnedMesh ( void );

	//	Initialize ( name, data ) : sets up new mesh data
	// Creates a new VBO associated with this rrMesh
	// Removes any old VBO data.
	void Initialize ( const string& nNewName, CModelData* const pNewModelData, unsigned int frames=1, bool isStreamed=false ) override;

	//	Copy ( ) : creates a new rrSkinnedMesh with duplicated streams
	rrSkinnedMesh* Copy ( void );

	////	CopyStream ( ) : prepare mesh for CPU-side skinning
	//// Creates a new VBO and CPU-side stream for this mesh.
	//void CopyStream ( rrSkinnedMesh* const psrcMesh );
	////	FreeStream ( ) : frees used VBOs
	//// Frees a stream that was created with CopyStream.
	//void FreeStream ( void );

	// Render the mesh out
	//void Render ( void ) override;

	// Render mode
	enum eRenderMode_t
	{
		RD_GPU = 0,
		RD_CPU,
		RD_GPU_FALLBACK //for modes that do not support GPU pallettes (splits up mesh, ala doom3)
	};
	eRenderMode_t		iRenderMode;

	// Quality Type
	enum eQualityType_t
	{
		QD_HIGH = 0,
		QD_MED,
		QD_LOW
	};
	eQualityType_t		iQualityType;

	// Set skeleton pointer
	/*void SetSkeleton ( std::vector<skeletonBone_t*>* pInSkeleton )
	{
		pvSkeleton = pInSkeleton;
		//rootBone = (*pInSkeleton)[0];
	};
	std::vector<skeletonBone_t*>* GetSkeleton ( void )
	{
		return pvSkeleton;
	};*/
	//void SetMatrixBuffer ( std::vector<
	void SetSkinningData ( const skinningData_t& n_skinning_data );

	// Get pose matrices
	//bool	newStreamReady;
	//bool	isStreaming;
	//bool	threadIsValid;
	//void PrepareStream ( void );
	////void GetPoseMatrices ( void );
	//void DoCPUSkinning ( void );
	//void StartSkinning ( void );

	void UpdateVBO ( void );

	//struct sSkinningUpdater
	//{
	//	rrSkinnedMesh*	mesh;
	//	void operator() ( void );
	//};

	/*void SwitchSkeleton ( vector<skeletonBone_t*>* newSkelly )
	{
		pvSkeleton = newSkelly;
	}*/
	/*CModelData* getCurrentStream ( void )
	{
		return pmStreamData;
	}*/

	//glHandle getTextureBuffer ( void ) {
	//	return m_textureBuffer;
	//}
	///*GLuint getSoftTextureBuffer ( void ) {
	//	return m_textureSoftBuffer;
	//}*/
	//glHandle getBuffer ( void ) {
	//	return m_textureBufferData;
	//}
	///*GLuint getSoftBuffer ( void ) {
	//	return m_textureSoftBufferData;
	//}*/
//protected:
//	bool	isDuplicatedStream;
//
protected:
	//skeletonBone_t*	rootBone;
	//std::vector<skeletonBone_t*>*	pvSkeleton;
	//Matrix4x4*	pvPoseMatrices;
	//Matrix4x4*	pvSoftMatrices;

	//glBone*		parentBone;
	bool		useSkinning;

	//CModelData* pmStreamData;
	//friend sSkinningUpdater;
	//thread	tSkinning;

	//unsigned int	partNum;
	//uint*	iVBOfaces_gpu;
	//unsigned int*	partTriCount;

public:
	skinningData_t	skinning_data;
	/*glHandle	m_textureBuffer;
	glHandle	m_textureBufferData;
	glHandle	m_textureBufferData_Swap;*/
	/*GLuint	m_textureSoftBuffer;
	GLuint	m_textureSoftBufferData;
	GLuint	m_textureSoftBufferData_Swap;*/

private:
	// Now for epic system stuff
	//void CreateSplitMesh ( void );
	//void RenderGPUFallback ( void );
	//void SendBoneMatrices ( int partOffset );
	//void FreeSplitMesh ( void );

	// GPU based
	//void UpdateVBO ( void );
	//void RenderCPU ( void );
	//void FreeCPUMesh ( void );

	//void FreeGPUBuffer ( void );
};

#endif