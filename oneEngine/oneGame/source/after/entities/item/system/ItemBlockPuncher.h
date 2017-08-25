
#ifndef _ITEM_BLOCK_PUNCHER_H_
#define _ITEM_BLOCK_PUNCHER_H_

// Includes
#include "after/entities/item/CWeaponItem.h"
//#include "CVoxelTerrain.h"
//#include "CTreeBase.h"
//#include "Raytracer.h"
//#include "after/entities/props/CTerrainProp.h"
//#include "CRenderablePrimitive.h"
//#include "CBoxCollider.h"
#include "after/types/terrain/BlockTracker.h"

class CTreeBase;
struct TreePart;


#include <vector>
//using std::vector;
#include <list>
//using std::list;
#include <queue>
//using std::queue;

// Class Definition
class ItemBlockPuncher : public CWeaponItem
{
	ClassName( "ItemBlockPuncher" );
public:
	// == Constructor ==
	ItemBlockPuncher ( void );
	// == Destructor ==
	~ItemBlockPuncher ( void );

	// Initialize the physics object
	void CreatePhysics ( void );

	// Update function
	void Update ( void );
	// Lateupdate function
	//void LateUpdate ( void );

	// Use function
	bool Use( int x );
	// Attack function
	void Attack ( XTransform& ) override;
public:
	struct HitPartInfo
	{
		CTreeBase*	pTree;
		TreePart*	treePart;
		BlockTrackInfo	blockInfo;
		short		hitCount;
		bool		hasItemDrop;
	};
private:
	//queue<HitPartInfo,list<HitPartInfo>>	hitList;
	std::list<HitPartInfo>	hitList;

protected:
	// Hardness of the block
	char	cBlockHardness [1024];
	// Hit multiplier. If it's set to 2, then something normally taking 2 hits to destroy now takes 4 hits.
	// This allows to pace low-cooldown weapons easier.
	char	iHitMutliplier;
	// Cooldowns when using
	float	fCooldownOnHit;
	float	fCooldownOnMiss;
	// Range of tool
	float	fMaxToolRange;

protected:
	short GetHitCount ( char hardness );
	

};

#endif