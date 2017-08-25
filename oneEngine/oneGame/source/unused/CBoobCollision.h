
#ifndef _C_BOOB_COLLISION_H_
#define _C_BOOB_COLLISION_H_

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

class CBoobCollision
{
public:
	CBoobCollision ( void );
	~CBoobCollision ( void );

	void Update ( void );

	void CreateCollision ( void );

public:
	// Owner
	CBoob* pOwner;

	// Physics Info
	physShape* pShape;
	physRigidBody* pCollision;

	// Physics Collision
	static hkpStaticCompoundShape* m_staticCompoundShape;
	static vector<sBoobCollisionRef>	m_collisionReferences;
	unsigned int	iMyCollisionRef;
	hkpBvCompressedMeshShape* meshShape;

	// Update flag
	bool needUpdateOnCollider;
	bitset<8> update;
	physShape*		m_collisions [8];
	physRigidBody*	m_bodies [8];

	void CreateCollisionShape ( const uint );

	static void PreloadCollision ( void );
	static unsigned int GetTargetReferenceCount ( void );
	static unsigned int GetCurrentReferenceCount ( void );

	typedef Terrain::terra_t terra_t;
private:
	void InitiallizeCollisionReferences ( void );
};

// CBoobMesh Collision regenerator struct definition
/*struct sCBoobMeshCollisionRegen
{
	CBoobMesh* pTargetMesh;
	void operator() ( void );
};*/

#endif