
CVoxelTerrain*	CVoxelTerrain::GetActive ( void )
{
	if ( !terrainList.empty() )
		return terrainList[0];
	else
		return NULL;
}

// Foliage Def
struct CVoxelTerrain::TerraFoiliage
{
	unsigned short	foliage_index;
	Vector3d		position;
	//uint64_t	bitmask;
	//string			userdata;
	char			userdata [48];
	CBoob*			myBoob;
};
// Component Def
struct CVoxelTerrain::TerraProp
{
	unsigned short	component_index;
	Vector3d		position;
	Vector3d		rotation;
	Vector3d		scaling;
	uint64_t		userdata;
	BlockInfo		m_block;
};

// Queued I/O Structs
struct CVoxelTerrain::sVTQueuedLoad
{
	CBoob*			pBoob;
	RangeVector		position;
	bool			regenSurround;
	bool			regenMesh;
	bool			regenWater;
};
struct CVoxelTerrain::sVTQueuedSave
{
	CBoob*			pBoob;
	RangeVector		position;
};

struct CVoxelTerrain::sCallableThread
{
	CVoxelTerrain* pMyTerrain;
	void	operator() ( void );
};
struct CVoxelTerrain::sGenerationThread
{
	CVoxelTerrain* pMyTerrain;
	void	operator() ( void );
};
struct CVoxelTerrain::sGenerationRequest
{
	RangeVector	position;
	bool		checkfile;
};