
#ifndef _C_BOOB_MESH_H_
#define _C_BOOB_MESH_H_

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
	static CTerrainVertex* vertices;
	static CModelQuad* quads;
	Vector3d vCameraPos;

	//Vector3d* physvertices;
	//CModelTriangle* phystris;

	// Current Thread
	//thread currentRegenThread;

	// Physics Info
	/*physShape* pShape;
	physRigidBody* pCollision;

	// Physics Collision
	static hkpStaticCompoundShape* m_staticCompoundShape;
	static vector<sBoobCollisionRef>	m_collisionReferences;
	unsigned int	iMyCollisionRef;
	hkpBvCompressedMeshShape* meshShape;*/
	//hkGeometry geometry;
	//hkpDefaultBvCompressedMeshShapeCinfo cInfo;
	typedef Terrain::terra_t terra_t;

public:
	// Constructor
	CBoobMesh ( void );
	// Destructor
	virtual ~CBoobMesh ( void );

	// =Terrain Mesh Regeneration=
	void UpdateRegen ( void );

	// =Terrain mesh Regeneration=
	bool PrepareRegen ( void );
	void CleanupRegen ( void );
	// Sends the built mesh to memory
	void UpdateVBOMesh ( void );
	// Version 6 Regeneration
	void AddBlockMesh ( CBoob *, char const, char const, short const, Vector3d const&, char const );
	//bool InSideVisible ( CBoob *, char const, char const, char *, int const, int const, EFaceDir const, unsigned char, Vector3d const&, char& );
	static terra_t GetBlockValue ( CBoob *, char const, char const, terra_t *, int const, int const, EFaceDir const, unsigned char ); 

	// =Water mesh regeneration=
	bool PrepareWaterRegen ( void );
	void CleanupWaterRegen ( void );
	// Sends the built water mesh to memory
	void UpdateWaterVBOMesh ( void );
	// Version 1 Water Regeneration
	void GenerateInCubeWater ( CBoob *, char const, char const, int const, Vector3d const&, char const );
	//bool InSideVisibleWater ( CBoob *, char const, char const, char *, int const, int const, EFaceDir const, unsigned char, Vector3d const&, char& );


	// Version 1 Regeneration
	//void GenerateCube ( CBoob *, char const, char const, int const, Vector3d const&, char const );
	/*bool SideVisible ( CBoob *, char const, char const, char *, int const, int const, EFaceDir const, unsigned char, Vector3d const& );
	// Version 2 Regeneration
	void GenerateInCube ( CBoob *, char const, char const, int const, Vector3d const&, char const );
	bool InSideVisible ( CBoob *, char const, char const, char *, int const, int const, EFaceDir const, unsigned char, Vector3d const&, char& );
	// Version 3 Regeneration
	void GenerateInVoxel ( CBoob *, char const, char const, int const, Vector3d const&, char const );
	bool GetBlockValue ( CBoob *, char, char, char *, short, int const, char const grabIndex, unsigned char, char&, unsigned char& ); */
protected:
	// == Private Inline Routines ==
	// These routines set specific details of a new generated terrain face.
	//inline void SetFaceUVs( CTerrainVertex*, EFaceDir const, unsigned short );
	inline void SetFaceColors( CTerrainVertex*, EFaceDir const, unsigned short, unsigned char );
	inline void SetInFaceUVs( CTerrainVertex*, EFaceDir const, unsigned short );

	inline void SetWaterFaceColors( CTerrainVertex*, EFaceDir const, unsigned char );
	inline void SetInFaceWaterUVs( CTerrainVertex*, EFaceDir const, unsigned short );
public:
	//inline void SetSmoothFaceUVs ( CTerrainVertex*, const Vector3d &, unsigned char, const Vector3d & );
	//inline bool IsSolid ( char );
	//inline Vector3d VertexInterp( ftype, const Vector3d &, const Vector3d &, ftype , ftype );
	static inline bool IsOpaque ( unsigned short );
	static inline bool IsTransparent ( unsigned short );
	static inline bool IsFluid ( unsigned short );
	static inline bool IsCollidable ( unsigned short );
};

inline bool CBoobMesh::IsOpaque ( unsigned short blocktype )
{
	return (( blocktype != EB_NONE )&&( blocktype != EB_WATER ));
}
inline bool CBoobMesh::IsCollidable ( unsigned short blockType )
{
	return (( blockType != EB_NONE )&&( blockType != EB_WATER )&&( blockType != EB_TOP_SNOW ));
}
inline bool CBoobMesh::IsFluid ( unsigned short block )
{
	return ( block == EB_WATER );
}


#endif