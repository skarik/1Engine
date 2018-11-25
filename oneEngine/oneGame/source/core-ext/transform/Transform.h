//===============================================================================================//
//
//		class Transform
//
// Structure for holding commonly used matrix transformations.
// Can be constructed with XTransforms.
//
// Currently, Transform is an alias for TransformLite, so they do not incur update costs.
//
//===============================================================================================//
#ifndef CORE_TRANSFORM_H_
#define CORE_TRANSFORM_H_

// Includes
#include <string>
#include <vector>

#include "core/common.h"
#include "core/types/float.h"
#include "core/math/Vector2.h"
#include "core/math/Vector3.h"
#include "core/math/Vector4.h"
#include "core/math/vect2d_template.h"
#include "core/math/vect3d_template.h"
#include "core/math/Quaternion.h"
#include "core/math/matrix/CMatrix.h"
#include "core/math/Rotator.h"

#include "core/math/XTransform.h"

#include "core-ext/transform/TransformUtility.h"

namespace core
{
	typedef TransformLite Transform;

	CORE_API	extern Vector3f_d	world_origin;
}

#endif//CORE_TRANSFORM_H_

#if 0
#ifndef _C_TRANSFORM_H_
#define _C_TRANSFORM_H_

// Includes
#include <string>
#include <vector>

#include "core/common.h"
#include "core/types/float.h"
#include "core/math/Vector2.h"
#include "core/math/Vector3.h"
#include "core/math/Vector4.h"
#include "core/math/Quaternion.h"
#include "core/math/matrix/CMatrix.h"
#include "core/math/Rotator.h"

#include "core/math/XTransform.h"

// Prototypes
class CGameBehavior;		// Engine
class CRenderableObject;	// Renderer
class RrLogicObject;			// Renderer

// Using
using std::string;

// Class
class ALIGNAS(64) CTransform
{
private:
	CTransform ( CTransform const& );
public:
	CORE_API CTransform ( void );
	CORE_API ~CTransform ( void );

	CORE_API void SetPosition( Vector3f );
	CORE_API void SetPosition( Real, Real, Real );
	CORE_API void SetScaling( Vector3f );
	CORE_API void SetScaling( Real, Real, Real );
	CORE_API void SetRotation( Vector3f );
	CORE_API void SetRotation( Real, Real, Real );
	//DEPRECATED( void SetTransform ( Matrix4x4 & ) );
	CORE_API void SetTransform ( Vector3f, Rotator, Vector3f );
	//DEPRECATED( void SetLocalTransform ( Matrix4x4 & ) );
	//DEPRECATED( void SetLocalTransform ( Vector3f, Vector3f, Vector3f ) );
	CORE_API void SetLocalTransform ( Vector3f, Rotator, Vector3f );

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

	//Matrix4x4 GetTransformMatrixNoScale ( void );
	//Matrix4x4 GetLocalTransformMatrixNoScale ( void );

	Matrix4x4 CTransform::WorldMatrix ( void )
	{
		return matx;
	}
	Matrix4x4 CTransform::WorldRotation ( void )
	{
		return matxRot;
	}
	Matrix4x4 CTransform::LocalMatrix ( void )
	{
		return matxLocal;
	}
	Matrix4x4 CTransform::LocalRotation ( void )
	{
		return matxLocalRot;
	}

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

private:
	// Transformation
	ALIGNAS(64) Matrix4x4	matx;
	ALIGNAS(64) Matrix4x4	matxLocal;
	// Rotation
	ALIGNAS(64) Matrix4x4	matxRot;
	ALIGNAS(64) Matrix4x4	matxLocalRot;

	// TODO: move to matrix, add quaternion representations

public:
	string name;
	bool active;
	enum eOwnerType {
		kOwnerTypeInvalid,
		kOwnerTypeBehavior,
		kOwnerTypeRendererObject,
		kOwnerTypeRendererLogic,
		kOwnerTypeBone_Deprecated,
		kOwnerTypeRendererCamera,
	} ownerType;
	void*	owner;

	Vector3f position;
	Vector3f scale;
	Rotator  rotation;

private:
	XrTransform previous;

public:
	Vector3f localPosition;
	Vector3f localScale;
	Rotator localRotation;

public:
	CORE_API Vector3f Forward ( void );
	CORE_API Vector3f Up ( void ); 
	CORE_API Vector3f Side ( void ); 

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
#endif