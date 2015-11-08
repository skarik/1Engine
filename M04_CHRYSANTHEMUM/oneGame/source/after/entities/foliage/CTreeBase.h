// Base class for procedural trees.

#ifndef _FOLIAGE_C_TREE_BASE_H_
#define _FOLIAGE_C_TREE_BASE_H_

// == Includes ==
#include "CFoliage.h"
//#include "CVoxelTerrain.h"
struct subblock16;
struct subblock8;
#include "after/types/terrain/BlockTracker.h" //Where/what is this?
#include "engine/physics/motion/CRigidbody.h"
#include "core/math/noise/Perlin.h"
#include "core/math/Ray.h"

#include <vector>
//using std::vector;

class CTreeRenderer;
//class CTreeLeafRenderer;
class CWeaponItem;

#include "engine-common/types/Damage.h"

struct sTreeBufferData {
	int32_t	tree_vert;
	int32_t	tree_vert_last;
	int32_t	leaf_vert;
	int32_t	leaf_vert_last;

	int32_t	tree_triTable;
	int32_t	tree_triCount; // Triangle count is very important to how it all works (as tris are contiguous)
	int32_t	leaf_triTable;
	int32_t	leaf_triCount;

	sTreeBufferData(void) : tree_vert(-1), leaf_vert(-1), tree_triTable(-1), leaf_triTable(-1)
	{
		;
	}
};

// == TreePart Struct Definition ==
// This struct is used to represent the parts of the tree.
struct TreePart
{
public:
	TreePart*	parent;		// Parent part
	Ray			shape;		// Traveling shape of the part
	float		size;		// Radius of the part
	float		strength;	// Strength of the part when weight is applied
	int			userData;
};

// == Class Definition ==
class CTreeBase : public CFoliage
{
	ClassName( "CTreeBase" );
	BaseClass( "CFoliage_TreeBase" );
public:
	CTreeBase ( void );
	~CTreeBase ( void );

	void Update ( void );

	// Public Generation method
	void Generate ( void );

	TreePart* GetPartClosestTo ( Vector3d );
	void BreakPart ( TreePart* pInPart, bool bDropItems = false );

	/*void SetToggle ( const uint64_t & );
	uint64_t GetToggle ( void );*/
	void SetToggle ( const char * ) override;
	void GetToggle ( char * ) override;

	sTreeBufferData buffer;

	virtual Damage GetPunchDamage ( void ) {
		Damage dmg;
		dmg.amount = 9.0f;
		dmg.type = DamageType::Crush|DamageType::Reflect;
		dmg.actor = this;
		return dmg;
	};
	virtual Damage GetClimbDamage ( void ) { return Damage(); };
protected:
	virtual CWeaponItem* CreatePieceItem ( const ftype fnSize, const Ray& rInShape, const Ray& rInPShape, const ftype fInSize, const ftype fInPSize );
	virtual CWeaponItem* CreateResinItem ( void );
protected:
	// Primary Attributes
	BlockTrackInfo			floor_block;
	std::vector<TreePart*>	part_list;
	std::vector<TreePart*>	part_list_full;
	std::vector<bool>		part_exists;
	// Leaf cloud properties
	std::vector<Vector3d>	leaf_cloud;
	std::vector<TreePart*>	leaf_branch;

	// Check if needs regen
	bool	bNeedsRegen;
	bool bNeedCollisionRegen;
	bool bRegenPriority;

	// Rendering info
	uchar	mLoDLevel;
	bool	bNeedsLoDRegen;
	bool	bInView;
	Vector3d	vLastRegenPosition;

	// World info
	uchar	mBiome;
	// Rendering attributes
	ftype	mTexCoordR;

	// Noise for generation
	static Perlin* noise;

	// ========================
	// RENDERING DATA
	CModelData	modelData;
	static CModelVertex*	pVertexBuffer;
	static CModelTriangle*	pTriangleBuffer;
	static uint	iVertexOffset;
	static uint iTriangleOffset;
	// Global push mesh to videocard again
	static bool	bNeedRegen; 
	static std::vector<CTreeBase*> vTreeList;
	static CTreeRenderer* mRenderManager;

	friend CTreeRenderer;
	//friend CTreeLeafRenderer;

	// Collision properties
	CStaticMeshCollider*	pTreeCollision;
	CRigidBody*				pTreeBody;

protected:
	// Updates state of LoD based on the current renderer
	void UpdateLoD ( void );

	// Frees used models
	void FreeModels ( void );

	// Tree Generation
	virtual void GenerateTreeData ( void );

	// Mesh Generation.
	virtual void GenerateTreeMesh ( void );
	virtual void GenerateLeafMesh ( std::vector<Vector3d>& );

public:
	// Materials specific to this type of tree
	static glMaterial*	pTreeBarkMaterial;
	static glMaterial*	pTreeLeafMaterial;

	static void			LoadTreeMaterials ( void );
};

#endif