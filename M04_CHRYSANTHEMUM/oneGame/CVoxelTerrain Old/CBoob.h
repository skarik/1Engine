
#ifndef _C_BOOB_TERRAIN_
#define _C_BOOB_TERRAIN_

// ==Includes==
// Windows for some brute-force errors
#ifdef _WIN32
#include "windows.h"
#endif
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

// Enumeration declaration
enum EFaceDir
{
	TOP = 1,
	BOTTOM,
	FRONT,
	BACK,
	LEFT,
	RIGHT
};
// block, biome, terra, instance, and cave type definitions
#include "TerrainTypes.h"


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
	uint32_t	data [512];
};

struct subblock16
{
	subblock8 data [8];
	bitset<8> visible;
};

// Include the data accessors
#include "CBoobBits.hpp"

// Include the boob position type
#include "LongIntPosition.h"

// Exception declaration
class C_xcp_vertex_overflow: public std::exception
{
public:
	virtual const char* what() const throw()
	{
		return "Vertex indexes have had an overflow. Report such exceptions immediately.";
	}
};
extern C_xcp_vertex_overflow xcp_vertex_overflow;

// Boob prototype class def
class CBoob;

// CBoobMesh Collision regenerator struct prototype
struct sCBoobMeshCollisionRegen;

// CBoobMesh collision reference struct
struct sBoobCollisionRef
{
	hkpStaticCompoundShape*	m_shape;
	bool					b_inUse;
};

// Lesbian pair
struct lesbianpair_t
{
	CBoob* konoka_l;
	CBoob* konoka_r;
	CBoob* setsuna_l;
	CBoob* setsuna_r;
};

// == TERRAIN MESH ==
// The Boob mesh definition.
// This class controls the visual representation of its parent terrain boob, as well as its collision mesh.
class CBoobMesh
{
public:
	// Owner
	CBoob* pOwner;

	// Maximum values
	static const unsigned int maxVertexCount = (32768*2)-1;

	// OpenGL Rendering info: Terrain Mesh
	GLuint iVBOverts;
	GLuint iVBOfaces;
	unsigned int vertexCount;
	unsigned int faceCount;
	// Mesh Update States
	bool needUpdateOnVBO;
	int iVBOUpdateState; // Current update state

	// OpenGL Rendering info: Water Mesh
	GLuint iWaterVBOverts;
	GLuint iWaterVBOfaces;
	unsigned int vertexCountWater;
	unsigned int faceCountWater;
	// Water mesh Update States
	bool needUpdateOnWaterVBO;

	// Model Info
	CTerrainVertex* vertices;
	CModelQuad* quads;
	Vector3d vCameraPos;

	Vector3d* physvertices;
	//CModelTriangle* phystris;

	// Physics Info
	physShape* pShape;
	physRigidBody* pCollision;

	// Current Thread
	thread currentRegenThread;

	// Physics Collision
	static hkpStaticCompoundShape* m_staticCompoundShape;
	static vector<sBoobCollisionRef>	m_collisionReferences;
	unsigned int	iMyCollisionRef;
	hkpBvCompressedMeshShape* meshShape;
	//hkGeometry geometry;
	//hkpDefaultBvCompressedMeshShapeCinfo cInfo;

public:
	// Constructor
	CBoobMesh ( void );
	// Destructor
	~CBoobMesh ( void );

	// Terrain Mesh Regeneration
	bool PrepareRegen ( void );
	void CleanupRegen ( void );
	void UpdateRegen ( void );

	// Terrain mesh Regeneration
	void UpdateVBOMesh ( void );
	void BeginUpdateCollisionMesh ( void );

	// Version 1 Regeneration
	void GenerateCube ( CBoob *, char const, char const, int const, Vector3d const&, char const );
	bool SideVisible ( CBoob *, char const, char const, char *, int const, int const, EFaceDir const, unsigned char, Vector3d const& );

	// Version 2 Regeneration
	void GenerateInCube ( CBoob *, char const, char const, int const, Vector3d const&, char const );
	bool InSideVisible ( CBoob *, char const, char const, char *, int const, int const, EFaceDir const, unsigned char, Vector3d const&, char& );

	// Version 3 Regeneration
	void GenerateInVoxel ( CBoob *, char const, char const, int const, Vector3d const&, char const );
	bool GetBlockValue ( CBoob *, char, char, char *, short, int const, char const grabIndex, unsigned char, char&, unsigned char& ); 

	// Water mesh regeneration
	bool PrepareWaterRegen ( void );
	void CleanupWaterRegen ( void );

	void UpdateWaterVBOMesh ( void );
	void GenerateInCubeWater ( CBoob *, char const, char const, int const, Vector3d const&, char const );
	bool InSideVisibleWater ( CBoob *, char const, char const, char *, int const, int const, EFaceDir const, unsigned char, Vector3d const&, char& );

private:
	// == Private Inline Routines ==
	// These routines set specific details of a new generated terrain face.
	inline void SetFaceUVs( CTerrainVertex*, EFaceDir const, unsigned char );
	inline void SetFaceColors( CTerrainVertex*, EFaceDir const, unsigned char );

	inline void SetInFaceUVs( CTerrainVertex*, EFaceDir const, unsigned char );

	inline void SetWaterFaceColors( CTerrainVertex*, EFaceDir const, unsigned char );
	inline void SetInFaceWaterUVs( CTerrainVertex*, EFaceDir const, unsigned char );

	inline void SetSmoothFaceUVs ( CTerrainVertex*, const Vector3d &, unsigned char, const Vector3d & );
	inline bool IsSolid ( char );
	inline Vector3d VertexInterp( ftype, const Vector3d &, const Vector3d &, ftype , ftype );
};

// CBoobMesh Collision regenerator struct definition
struct sCBoobMeshCollisionRegen
{
	CBoobMesh* pTargetMesh;
	void operator() ( void );
};

// Includes specific to terrain boobs
//#include "CFoliageTree.h"
#include "CFoliage.h"
#include "CTerraComponent.h"
#include "CTerraGrass.h"

// == TERRAIN BOOBS ==
// Because hills and tits.
// This is the foundation of the voxel terrain.
class CBoob
{
public:
	// Public Modifiable Data
	//subblock16 data [8];	// This is the actual terrain data
	subblock16* data;

	bitset<8> visible;		// This is legacy from earlier versions, when the terrain was more octree-like.
							//  This was used to check if the subblock16's were visible and therefore available
							//  to draw. In the older terrain versions, this was absolutely essential! It is kept
							//  around in case we get some sort of genius idea here.

	// [0] is visibility, [1] is drawn flag, [2] is checked visibility flag, [3] is generation flag,
	// [4] is checked regen flag, [5] is vbo generation flag, [6] is the player movement check flag, [7] is deletion flag
	// However, during terrain generation, [0] becomes the secondary generation pass flag.
	// Also, [7] is usually used for flagging other things, just because it has a dedicated 'reset' routine
	bitset<8> bitinfo; 

	// This indicates whether or not the boob already has data and should be loaded. This was introduced to fix the
	// "dual-loading" issue on starting the game, where boobs would be generated twice on game start. With this boolean
	// added, if the terrain generation has filled the area with data, the file-loader cannot fill it. Nor vice-versa. 
	bool hasData;

	// This indicates whether or not the boob already has block data and should have that loaded. This was introduced to
	// fix the "dual-loading" issue on loading LOD terrain parts. With this boolean added, if this value is set, then the
	// file loader cannot fill it.
	// Also, this fixes the "I'm not going to finish loading, haha bitch" bug in the LOD mode.
	bool hasBlockData;

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
	LongIntPosition li_position; // This is set to the terrain position of the boob. This is primarily used for ordering rendering and loading.

	// The pointer to the boob's rendering and collision object. The Mesh takes care of all visual aspects.
	CBoobMesh* pMesh;

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
	typedef CTerraComponent*					ComponentRefType;
	typedef vector<ComponentRefType>::iterator	ComponentIterator;
	vector<ComponentRefType>	v_component;	// List containing all general components for the Boob.
												// The difference between components and foliage is that 

	// These deal with the grass rendering
	vector<sTerraGrass>	v_grass;	// List of grass points
	CTerraGrass*	pGrass;			// Pointer to the grass renderer. Grass is only rendered if its boob is visible.


public:
	// Constructor
	CBoob ( )
	{
		/*pMesh = new CBoobMesh;
		pMesh->pOwner = this;
		pGrass = new CTerraGrass( &v_grass, this );
		cout << "New CBoob" << endl;*/
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
	}
};

// TODO: Limit those physics boundaries to something true managable!

// == TERRAIN METABOOBS ==
// This is a solely storage version of the boob.
// While it has all the same pointers, it doesn't create any meshes.
class CMetaboob : public CBoob
{
public:
	// Additional storage for straight-to-hdd generation

	// Foliage IO, mirroring what's stored in the terrain files.
	struct sFoliageIO
	{
		unsigned short foliage_index;
		Vector3d	position;
		//uint64_t	bitmask;
		string		userdata;
	};
	// List of IO foliage
	vector<sFoliageIO> v_foliageIO;

	// Component IO, mirrors what's stored in the terrain files.
	struct sComponentIO
	{
		unsigned short component_index;
		Vector3d	position;
		Vector3d	rotation;
		Vector3d	scaling;
		uint64_t	data;
		char		b16index;
		char		b8index;
		short		bindex;
	};
	// List of IO components
	vector<sComponentIO> v_componentIO;

public:
	CMetaboob ( )
	{
		// Set pointers to null
		pMesh = NULL;
		pGrass = NULL;
		//cout << "New CMetaboob" << endl;
	}
	~CMetaboob ( )
	{
		// Nothing
	}
};



#endif