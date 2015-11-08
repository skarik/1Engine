
#ifndef _C_TRANSFORM_H_
#define _C_TRANSFORM_H_

// Includes
#include <string>
#include <vector>

#include "core/common.h"
#include "core/types/float.h"
#include "core/math/Vector2d.h"
#include "core/math/Vector3d.h"
#include "core/math/Vector4d.h"
#include "core/math/Quaternion.h"
#include "core/math/matrix/CMatrix.h"
#include "core/math/Rotator.h"

#include "core/math/XTransform.h"

// Prototypes
class CGameBehavior;		// Engine
class CRenderableObject;	// Renderer
class CLogicObject;			// Renderer

// Using
using std::string;

// Class
class CTransform
{
private:
	CTransform ( CTransform const& );
public:
	CORE_API CTransform ( void );
	CORE_API ~CTransform ( void );

	CORE_API void SetPosition( Vector3d );
	CORE_API void SetPosition( ftype, ftype, ftype );
	CORE_API void SetScaling( Vector3d );
	CORE_API void SetScaling( ftype, ftype, ftype );
	CORE_API void SetRotation( Vector3d );
	CORE_API void SetRotation( ftype, ftype, ftype );
	//DEPRECATED( void SetTransform ( Matrix4x4 & ) );
	CORE_API void SetTransform ( Vector3d, Rotator, Vector3d );
	//DEPRECATED( void SetLocalTransform ( Matrix4x4 & ) );
	//DEPRECATED( void SetLocalTransform ( Vector3d, Vector3d, Vector3d ) );
	CORE_API void SetLocalTransform ( Vector3d, Rotator, Vector3d );

	CORE_API void SetTransform ( const XTransform& );
	CORE_API void SetLocalTransform ( const XTransform& );

	CORE_API void SetParent ( CTransform *, bool onDeathlink=false );
	CORE_API void UnlinkChildren ( void );

	// Set update
	inline void SetDirty ( void )
	{
		_is_dirty = true;
	};
	inline bool IsDirty ( void )
	{
		bool val = _is_dirty;
		_is_dirty = false;
		return val;
	};

	CORE_API void Get ( CTransform const& );

	CORE_API CTransform * GetParent ( void );

	CORE_API Matrix4x4 GetTransformMatrix ( void );
	//Matrix4x4 GetTransformMatrixNoScale ( void );
	CORE_API Matrix4x4 GetTransformMatrixRot ( void );

	CORE_API Matrix4x4 GetLocalTransformMatrix ( void );
	//Matrix4x4 GetLocalTransformMatrixNoScale ( void );
	CORE_API Matrix4x4 GetLocalTransformMatrixRot ( void );

public:
	//void Update ( void ) {};
	CORE_API void LateUpdate ( void );

	//void FixedUpdate ( void ) {};

	// FindChild()
	// Returns the first occurrence of the transform with the given name.
	// This will only search this transform's children, not any other layers.
	CORE_API CTransform* FindChild ( const string& );
	// FindChildRecursive()
	// Returns the first occurrence of the transform with the given name.
	// This will search for children of children.
	CORE_API CTransform* FindChildRecursive ( const string& );
	// Finds the matching transform. Is a good replacement for "transform exists"
	CORE_API CTransform* FindChildRecursive ( const CTransform* );

	CORE_API CTransform* FindChildBone ( const string& );
	CORE_API CTransform* FindChildBoneRecursive ( const string& );

public:
	string name;
	bool active;
	//CGameBehavior* pOwnerBehavior;
	//CRenderableObject* pOwnerRenderer;
	enum eOwnerType_t {
		TYPE_INVALID,
		TYPE_BEHAVIOR,
		TYPE_RENDERER_OBJECT,
		TYPE_RENDERER_LOGIC,
		TYPE_RENDERER_GLBONE,
		TYPE_RENDERER_CAMERA,
	} ownerType;
	void*	owner;

	/*dirtval<Vector3d> position;
	dirtval<Vector3d> scale;
	dirtval<Rotator> rotation;*/
	Vector3d position;
	Vector3d scale;
	Rotator  rotation;

	Vector3d pposition;
	Vector3d pscale;
	Rotator  protation;
//private:
	//Vector3d rotation;
//public:

	Vector3d localPosition;
	Vector3d localScale;
	Rotator localRotation;
//private:
	//Vector3d localRotation;

	//static bool updateRenderSide;
private:
	Matrix4x4	matx;
	Matrix4x4	matxLocal;

	//Matrix4x4	matxNS;
	//Matrix4x4	matxLocalNS;

	Matrix4x4	matxRot;
	Matrix4x4	matxLocalRot;

	// Todo: move to matrix, add quaternion representations

public:
	CORE_API Vector3d Forward ( void );
	CORE_API Vector3d Up ( void ); 
	CORE_API Vector3d Side ( void ); 

public:
	std::vector<CTransform*> children;
private:
	CTransform * _parent;
	bool _is_dirty;
	bool _is_ready;

public:
	CORE_API static CTransform root;

	// Propogates transforms, starting at Root.
	CORE_API static void PropogateTransforms ( void );
};

// Typedef for laziness
typedef CTransform Transform;


#endif