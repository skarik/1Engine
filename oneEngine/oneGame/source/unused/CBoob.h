
#ifndef _C_BOOB_TERRAIN_
#define _C_BOOB_TERRAIN_

// ==Includes==
// Windows for some brute-force errors
/*#ifdef _WIN32
#include "windows.h"
#endif*/
// Needed for full memory usage w/ bools
#include <bitset>
// Used with errors on regeneration
#include <exception>
// Render info
#include "CRenderableObject.h"
#include "CModelData.h"
// Physics common
#include "CPhysics.h"
// Boost Threads for regeneration
#include "boost\thread.hpp"

// Namespace
using std::bitset;
using std::bad_alloc;
using boost::thread;

// block, biome, terra, instance, and cave type definitions
#include "TerrainTypes.h"
// Include the data accessors and bit type definitions
#include "world/BlockTypes.h"
// Include the world
#include "world/DirectionFlags.h"

// Struct declaration
struct subblock8
{
	/*unsigned char data [512];
	unsigned char light [512];
	unsigned char water [512];*/
	/*unsigned char* data;
	unsigned char* light;
	unsigned char* water;
	unsigned char* temp;*/
	Terrain::terra_t		data [512];
};

struct subblock16
{
	subblock8 data [8];
	//bitset<8> visible;
};

// Include the boob position type
#include "RangeVector.h"

#include "CBoobMesh.h"
#include "CBoobCollision.h"

// Includes specific to terrain boobs
//#include "CFoliageTree.h"
#include "CFoliage.h"
#include "CTerrainProp.h"
#include "CTerraGrass.h"

#define TERRA_SYSTEM_VERSION 10
// == TERRAIN BOOBS ==
// Because hills and tits.
// This is the foundation of the voxel terrain.
class CBoob
{
public:
	// Public Modifiable Data
	//subblock16 data [8];	// This is the actual terrain data
	subblock16* data;		// Width of this block is 32768*4 = 131072 bytes

	//bitset<8> visible;		// This is legacy from earlier versions, when the terrain was more octree-like.
							//  This was used to check if the subblock16's were visible and therefore available
							//  to draw. In the older terrain versions, this was absolutely essential! It is kept
							//  around in case we get some sort of genius idea here.

	bitset<8> solid;		// Tells whether or not the corresponding subblock16 is completely full of solid blocks
							//  This is used to control culling, skip simulation (or mesh generation), and be
							//  generally cool.

	// [0] is visibility, [1] is drawn flag, [2] is checked visibility flag, [3] is generation flag,
	// [4] is checked regen flag, [5] is vbo generation flag, [6] is the player movement check flag, [7] is deletion flag
	// However, during terrain generation, [0] becomes the secondary generation pass flag.
	// Also, [7] is usually used for flagging other things, just because it has a dedicated 'reset' routine
	bitset<8> bitinfo; 

	// Boolean indicating if this boob is locked for order-important tasks, such as loading or deletion
	bool locked;

	// This indicates whether or not the boob already has data and should be loaded. This was introduced to fix the
	// "dual-loading" issue on starting the game, where boobs would be generated twice on game start. With this boolean
	// added, if the terrain generation has filled the area with data, the file-loader cannot fill it. Nor vice-versa. 
	//bool hasData;

	// This indicates whether or not the boob already has prop data and should have the prop data loaded. This flag's
	// name is a much more explicit definition of "loaded" means in terms of the terrain, especially when compared to the
	// previous version's "hasData" flag. The file loader controls the value of this flag, and other systems act on it.
	bool hasPropData;

	// This idicates whether or not the boob may load prop data. This tends to be false on lower detail objects, but
	// assists in making the code more readable.
	bool loadPropData;

	// This indicates whether or not the boob already has block data and should have that loaded. This was introduced to
	// fix the "dual-loading" issue on loading LOD terrain parts. With this boolean added, if this value is set, then the
	// file loader cannot fill it.
	// Also, this fixes the "I'm not going to finish loading, haha bitch" bug in the LOD mode.
	bool hasBlockData;

	// This indicated whether or not the boob already has had NPC information loaded.
	bool npcsLoaded;

	// The terrain is implemented as a sort of a linked list, though in this case, would be more of a linked-lattice
	// The way the terrain is managed greatly resembles an octree, due to how the terrain used to be handled in the
	//  pre-vertex buffer builds. However, the current actual implementation usage isn't too similar to an octree,
	//  as each boob does not quite get subdivided farther.
	CBoob *top,*bottom, *front,*back, *left,*right;

	// This is the current LOD of the Boob. LOD still needs to be implemented properly, but the basic ProofOfConcept code exists in CVoxelTerrain.
	unsigned char current_resolution;
	// These variables control the generation algorithm.
	unsigned char terrain;	// Terrain type of the boob. This could be flatlands, desert, high plains, mountains. Essentially, the physical representation.
	unsigned char biome;	// Biome type of the boob. This controls the type of blocks generated. This could be grassland, desert, or perhaps arctic
	unsigned char version;	// This is the version of the generation used. This is here to allow for blending between previous terrain versions and current
							//  terrain versions. This actually was decided when it was found that Minecraft makes no attempt to keep consistent generation
							//  across versions on the same world, causing much pain and suffering.

	// Position of the boob.
	Vector3d position;	// This is the set to the center of the boob. This makes it easy for using with bounding-box checks and such.
	RangeVector li_position; // This is set to the terrain position of the boob. This is primarily used for ordering rendering and loading.
	ftype elevation; /// This is set to the elevation at the boob's XY position, in boob-space.

	// The pointer to the boob's rendering object. The Mesh takes care of all visual aspects.
	CBoobMesh* pMesh;
	// The pointer to the boob's collision object. The Collider takes care of the physics aspect.
	CBoobCollision* pCollision;

	// List of foliage.
	//typedef CFoliageTree*						FoliageRefType;
	typedef CFoliage*							FoliageRefType;
	typedef vector<FoliageRefType>::iterator	FoliageIterator;
	vector<FoliageRefType> v_foliage;	// List containing all general foliage for the Boob.
										// 
										// Old Documentation (issue noted has been fixed):
										// This should eventually go to contain just general foilage.
										// The boob maintains the list of relevant objects within it. In this list, it's the
										//  foliage objects specifically that are maintained. The Boob retains ownership of
										//  the foliage objects, as generally, the world seems to have issues with freeing
										//  foliage. It's a pretty atrocious bug. It'll get fixed. I believe in code-kun.

	// List of components
	typedef CTerrainProp*					ComponentRefType;
	typedef vector<ComponentRefType>::iterator	ComponentIterator;
	vector<ComponentRefType>	v_component;	// List containing all general components for the Boob.
												// The difference between components and foliage is that 

	// These deal with the grass rendering
	vector<sTerraGrass>	v_grass;	// List of grass points
	CTerraGrass*	pGrass;			// Pointer to the grass renderer. Grass is only rendered if its boob is visible.

	// List of NPCS
	vector<uint64_t>	v_npc;		// List containing all NPCs.
public:
	// Constructor
	CBoob ( )
	{
		pMesh = NULL;
		pGrass = NULL;
		pCollision = NULL;

		npcsLoaded = false;
		locked = false;
	}
	// Destructor
	~CBoob ( )
	{
		if ( pMesh )
			delete pMesh;
		pMesh = NULL;
		if ( pGrass )
			delete pGrass;
		pGrass = NULL;
		if ( pCollision )
			delete pCollision;
		pCollision = NULL;
	}
};

// TODO: Limit those physics boundaries to something true managable!


// Lesbian pair
struct lesbianpair_t
{
	CBoob* konoka_l;
	CBoob* konoka_r;
	CBoob* setsuna_l;
	CBoob* setsuna_r;
};


#endif