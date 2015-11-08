
#ifndef _C_SKINNED_MODEL_
#define _C_SKINNED_MODEL_

// Include model and skinned mesh types
#include "CModel.h"
//#include "glSkinnedMesh.h"

// Include vector
#include <vector>
#include <map>
//using std::vector;

class glSkinnedMesh;
class skeletonBone_t;

class CMorpher;

class CSkinnedModel : public CModel
{
public:
	// Constructor
	RENDER_API explicit CSkinnedModel ( const string& sFilename );
	// Destructor
	RENDER_API ~CSkinnedModel ( );

private:
	// Load the model
	void LoadSkinnedModel ( const string& sFilename );

	void DebugRenderSkeleton ( skeletonBone_t* );

	// Copy skeleton from the existing reference
	void GrabSkeletonCopy ( std::vector<skeletonBone_t*>* );
	// Uploads transform references to the animation
	void SendSkeletonReferences ( void );

	// Copy mesh streams from the existing reference
	void CopyMeshStreams ( std::vector<glMesh*> * );
	void FreeMeshStreams ( std::vector<glMesh*> * );
	std::vector<glMesh*> * pvOriginalMeshSet;
public:
	// == RENDERABLE OBJECT INTERFACE ==
	// Public Render Routine
	//bool Render ( const char pass );

	void PreStep ( void ) override;
	void PreStepSynchronus ( void ) override;
	void PostStepSynchronus ( void ) override;

	// == MODDERS ==
	// Updates the skeleton by updating parent transforms and then updating all the children transforms, recursively.
	RENDER_API void UpdateSkeleton ( bool lateUpdate=true );

	// == GETTERS ==
	// Returns a reference to the root heirarchy of the skeleton
	RENDER_API CTransform*	GetSkeletonRoot ( void );
	// Returns the reference to the skeleton list. Don't edit it unless you know what you're doing.
	RENDER_API std::vector<skeletonBone_t*>* GetSkeletonList ( void );
	// Get the morpher reference
	RENDER_API CMorpher*	GetMorpher ( void );

	// == SETTERS ==
	// This sets the bones to copy the target skinned model's bones completely.
	//  It does this referencing by creating a reference list that is generated by name matching.
	// If the referenced model is deleted, unexpected behavior will occur.
	RENDER_API void SetReferencedBoneMode ( CSkinnedModel* );

protected:
	// Render Options
	bool bUseBoneVertexBlending;
	bool bDrawSkeleton;

	// Skeleton Info
	skeletonBone_t*			rootBone;
	std::vector<skeletonBone_t*>	vSkeleton;

	bool						bReferenceSkeleton;
	bool						bPerfectReference; // Is the matching perfect? If not, we have to update the imperfects
	CSkinnedModel*				pReferencedSkeleton;
	std::map<skeletonBone_t*,skeletonBone_t*>	mBoneReferenceMap;
	std::vector<skeletonBone_t*>	mBoneMismatchList;

	// Morpher Info
	bool		bDoMorphing;
	int			iMorphTarget;
	CMorpher*	pMorpher;

	// Softness movement tracking
	Matrix4x4	matrixPreviousStep;
	Matrix4x4	matrixPreviousStepDiff;
	Matrix4x4	matrixSavePoints [6];

	// Recursive update function for updating the skeleton transforms
	void UpdateSkeleton ( Transform*, bool lateUpdate );
	void SetSkeletonBindPose ( skeletonBone_t* );
	// Sends bone matrices to the vertex shader
	//void PassBoneMatrices ( glMaterial* );
	// Recursive update function for writing skeleton transforms to the game transforms
	void SendOutSkeleton ( void );
	void SendOutSkeleton ( skeletonBone_t* );

	// Update jiggle bones
	void UpdateJigglebones ( void );

};

#endif