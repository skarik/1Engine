
// Includes
#include "CTransform.h"
//#include "CGameState.h"
#include <list>
#include <algorithm>

// Using
using std::find;
using std::cout;
using std::endl;
//using std::list;

// Static bool
//bool CTransform::updateRenderSide = false;
CTransform CTransform::root;

// Constructor
CTransform::CTransform ( void )
	: owner(NULL), ownerType(TYPE_INVALID), active(true), name("Transform"), _parent( NULL )
{
	position = Vector3d( 0,0,0 );
	scale = Vector3d( 1,1,1 );
	rotation = Rotator();
	pposition	= position;
	pscale		= scale;
	protation	= rotation;

	matx = Matrix4x4();

	localPosition = position;
	localScale = scale;
	localRotation = rotation;

	matxLocal = Matrix4x4();

	//_parent = NULL;
	SetParent( &root );

	_is_dirty = false;
	_is_ready = false;

	//noReference = false;
}

// Destructor
CTransform::~CTransform ( void )
{
	if ( this != &root ) {
		SetParent( NULL, true );
		UnlinkChildren();
	}
}


// == Public Accessors ==
void CTransform::Get ( CTransform const& targetTrans )
{
	position = targetTrans.position;
	rotation = targetTrans.rotation;
	scale = targetTrans.scale;
	pposition	= position;
	pscale		= scale;
	protation	= rotation;

	localPosition = targetTrans.localPosition;
	localRotation = targetTrans.localRotation;
	localScale = targetTrans.localScale;

	matx	= targetTrans.matx;
	matxRot = targetTrans.matxRot;
	matxLocal		= targetTrans.matxLocal;
	matxLocalRot	= targetTrans.matxLocalRot;
}
CTransform * CTransform::GetParent ( void )
{
	return _parent;
}
Matrix4x4 CTransform::GetTransformMatrix ( void )
{
	return matx;
}
Matrix4x4 CTransform::GetTransformMatrixRot ( void )
{
	return matxRot;
}

/*Matrix4x4 CTransform::GetTransformMatrixNoScale ( void )
{
	Matrix4x4 transMatrix;
	transMatrix.setTranslation( position );
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( rotation );
	return (transMatrix*rotMatrix);
}*/
Matrix4x4 CTransform::GetLocalTransformMatrix ( void )
{
	return matxLocal;
}
Matrix4x4 CTransform::GetLocalTransformMatrixRot ( void )
{
	return matxLocalRot;
}

/*Matrix4x4 CTransform::GetLocalTransformMatrixNoScale ( void )
{
	Matrix4x4 transMatrix;
	transMatrix.setTranslation( localPosition );
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( localRotation );
	return (transMatrix*rotMatrix);
}*/

// Return forward direction
Vector3d CTransform::Forward ( void )
{
	return rotation * Vector3d(1,0,0);
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( rotation );
	//return (Vector3d(1,0,0)*rotMatrix);
	return (rotMatrix*Vector3d(1,0,0));
}
// Return up direction
Vector3d CTransform::Up ( void )
{
	return rotation * Vector3d(0,0,1);
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( rotation );
	//return (Vector3d(0,0,1)*rotMatrix);
	return (rotMatrix*Vector3d(0,0,1));
}
// Return side direction
Vector3d CTransform::Side ( void )
{
	return rotation * Vector3d(0,1,0);
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( rotation );
	//return (Vector3d(0,1,0)*rotMatrix);
	return (rotMatrix*Vector3d(0,1,0));
}

// Unlinker
void CTransform::UnlinkChildren ( void )
{
	/*for ( vector<CTransform*>::iterator child = children.begin(); child != children.end(); ++child )
	{
		// Abandon our children
		(*child)->SetParent( NULL );
	}*/
	while ( !children.empty() )
	{
		//children[0]->SetParent( NULL ); // sometimes falls into infinite loop. don't feel like searching for the "root" of the issue (har har har)
		children.back()->SetParent(NULL); 
		children.pop_back();
	}
}

// Public Setters

// Set Parent
void CTransform::SetParent ( CTransform * pNewParent, bool onDeathlink )
{
	if ( _parent != NULL && !_is_ready )
		LateUpdate();

	// Add root
	if ( this == &root ) {
		return;
	}
	// Don't set new parent
	if ( pNewParent == NULL && !onDeathlink ) {
		pNewParent = &root;
	}

	if (( _parent != NULL )&&( _parent != pNewParent ))
	{
		//if ( CGameState::pActive->ObjectExists( _parent ) )
		if ( root.FindChildRecursive( _parent ) )
		{
			// Find index with this child
			std::vector<CTransform*>::iterator index;
			index = find( _parent->children.begin(), _parent->children.end(), this );
			if ( index != _parent->children.end() )
			{
				_parent->children.erase( index );
			}
			else
			{
				cout << __FILE__ << "(" << __LINE__  << "): The child of a transform could not be found in the parent. This is like parental abandonment." << endl;
			}
		}
	}

	if ( pNewParent == NULL ) {
		return;
	}

	{
		_parent = pNewParent;
		_parent->children.push_back( this );
		if ( !_parent->_is_ready )
			_parent->LateUpdate();

		/*
		Matrix4x4 mytransform, prtransform, localtransform;

		// now
		// mytransform = localtransform*prtransform;
		// mytransform * prtransform.inv = localtransform * prtransform * prtransform.inv
		// mytransform * prtransform.inv = localtransform

		// Get both current transform and parent transform
		mytransform = GetTransformMatrix();
		prtransform = _parent->GetTransformMatrix();
		// Calculate local transform
		localtransform = prtransform.inverse() * mytransform;
		// Apply the local tranform
		localPosition = localtransform.getTranslation();
		localScale = localtransform.getScaling();


		// Get both current transform and parent transform
		mytransform = GetTransformMatrixRot();
		prtransform = _parent->GetTransformMatrixRot();
		// Calculate local transform
		localtransform = prtransform.inverse() * mytransform;
		// Apply local rotation
		localRotation = localtransform.getRotator();
		//localRotation = localtransform.getEulerAngles();
		//localRotation.SwitchBasis();
		*/
	
		// Convert position,rotation,scaling into local coordinates
		{
			// Update local position by moving the translation into local space
			localPosition = _parent->GetTransformMatrix().inverse() * position;
			// Just take the new rotation in local space
			localRotation = (_parent->GetTransformMatrixRot().inverse().getRotator() * rotation);
			// Just take the new scale, I guess
			localScale = Vector3d( scale.x/_parent->scale.x, scale.y/_parent->scale.y, scale.z/_parent->scale.z );
		}

		pposition	= position;
		pscale		= scale;
		protation	= rotation;
	}
	/*for ( int i = 0; i < 16; ++i ) {
		cout << matx.pData[i] << ' ';
		if ( i%4==3) cout << endl;
	}
	cout << "endl" << endl;*/

	// Stabilize the new values
	LateUpdate();

	/*for ( int i = 0; i < 16; ++i ) {
		cout << matx.pData[i] << ' ';
		if ( i%4==3) cout << endl;
	}*/
}

// Late Update
//#include "CDebugDrawer.h"
#include "core/debug/CDebugConsole.h"
void CTransform::LateUpdate ( void )
{
	/*if ( pOwnerBehavior == NULL ) {
		if ( !updateRenderSide ) {
			return;
		}
	}*/

#ifdef _ENGINE_SAFE_CHECK_
	fnl_assert( VALID_FLOAT(position.x) );
	fnl_assert( VALID_FLOAT(position.y) );
	fnl_assert( VALID_FLOAT(position.z) );
	fnl_assert( VALID_FLOAT(scale.x) );
	fnl_assert( VALID_FLOAT(scale.y) );
	fnl_assert( VALID_FLOAT(scale.z) );
	static int i;
	for ( i = 0; i < 9; ++i ) {
		fnl_assert( VALID_FLOAT(rotation.pData[i]) );
	}
#endif

	/*Debug::Drawer->DrawLine( position, position+Forward() );
	Debug::Drawer->DrawLine( position, position+Up() );
	Debug::Drawer->DrawLine( position, position+Side() );*/

	_is_ready = true;
	//if ( _parent != NULL )
	if ( this != &root )
	{
		if ( _parent == NULL ) {
			Debug::Console->PrintError( "TRANSFORM WITH NO PARENT\n" );
			return;
		}

		/*Matrix4x4 localTransform, parentTransform, myTransform;
		// == Translation and Scaling ==
		{	// Generate a local transform matrix
			Matrix4x4 transMatrix;
			transMatrix.setTranslation( localPosition );
			Matrix4x4 rotMatrix;
			rotMatrix.setRotation( localRotation );
			Matrix4x4 scalMatrix;
			scalMatrix.setScale( localScale );
			matxLocal = transMatrix*((rotMatrix)*scalMatrix);
		}
		localTransform = GetLocalTransformMatrix();			// Get the local transform
		parentTransform = _parent->GetTransformMatrix();	// Get the parent transform
		
		myTransform = parentTransform*localTransform;		// Get the global transform (local*parent)

		// Apply the local tranform
		position = myTransform.getTranslation();
		scale = Vector3d( localScale.x * _parent->scale.x, localScale.y * _parent->scale.y, localScale.z * _parent->scale.z );

		// Set the matrices
		matx = myTransform;
		// ==============================

		// == Rotation ==
		{ // Generate a local rotation matrix
			Matrix4x4 rotMatrix;
			rotMatrix.setRotation( localRotation );
			matxLocalRot = rotMatrix;
		}
		localTransform = GetLocalTransformMatrixRot();		// Get the local transform
		parentTransform = _parent->GetTransformMatrixRot(); // Get the parent transform
		
		myTransform = parentTransform*localTransform;

		// Apply the local tranform
		rotation = myTransform.getRotator();

		// Set the matrices
		matxRot = myTransform;
		// ==============================
		*/

		// Edit local space if the global space doesn't equal the last frame's result: TODO SHOULD HAPPEN BEFORE TRANSFORM PROPOGATE
		if ( _is_dirty || pposition != position || protation != rotation || pscale != scale )
		{
			if ( pposition != position ) {
				// Update local position by moving the translation into local space
				//Vector3d offset = (_parent->GetTransformMatrix().inverse() * pposition) - (_parent->GetTransformMatrix().inverse() * position);
				// Now update it
				//localPosition += offset;
				localPosition = _parent->GetTransformMatrix().inverse() * position;
			}
			if ( protation != rotation ) {
				// Just take the new rotation in local space
				localRotation = (_parent->GetTransformMatrixRot().inverse().getRotator() * rotation);
			}
			if ( pscale != scale ) {
				// Just take the new scale, I guess
				localScale = Vector3d( scale.x/_parent->scale.x, scale.y/_parent->scale.y, scale.z/_parent->scale.z );
			}
		}

		// This.Global = Parent.Global * This.Local

		// == Translation and Scaling ==
		{	// Generate a local transform matrix
			Matrix4x4 transMatrix;
			transMatrix.setTranslation( localPosition );
			Matrix4x4 rotMatrix;
			rotMatrix.setRotation( localRotation );
			Matrix4x4 scalMatrix;
			scalMatrix.setScale( localScale );
			// Set the local transforms
			matxLocal = transMatrix*((rotMatrix)*scalMatrix);
			matxLocalRot = rotMatrix;
		}
		// Create the global transforms
		matx = _parent->GetTransformMatrix() * matxLocal;
		matxRot = _parent->GetTransformMatrixRot() * matxLocalRot;

		// Get back the values
		position	= matx.getTranslation();
		scale		= Vector3d( localScale.x * _parent->scale.x, localScale.y * _parent->scale.y, localScale.z * _parent->scale.z );
		rotation	= matxRot.getRotator();

		// Set the values
		pposition	= position;
		pscale		= scale;
		protation	= rotation;
	}
	else
	{
		// Root gets a very particular behavior.

		// Positions offset the world
		if ( position.sqrMagnitude() > FTYPE_PRECISION ) {
			for ( uint i = 0; i < children.size(); ++i ) {
				children[i]->position += position;
				children[i]->SetDirty();
			}
		}

		// Its local positions control it.
		position = localPosition;
		rotation = localRotation;
		scale	 = localScale;

		{	// Create the transformation matrix
			Matrix4x4 transMatrix;
			transMatrix.setTranslation( localPosition );
			Matrix4x4 rotMatrix;
			rotMatrix.setRotation( localRotation );
			Matrix4x4 scalMatrix;
			scalMatrix.setScale( localScale );
			// Set the local matrices
			matxLocal = scalMatrix * rotMatrix * transMatrix;
			matxLocalRot = rotMatrix;
			// Set the global matrices
			matx = matxLocal;
			matxRot = matxLocalRot;
		}
	}
}

/*
void CTransform::SetTransform ( Matrix4x4 & inMatrix )
{
	cout << "SetTransform( Matrix4x4& ) is DEPRECATED! Stop using it!" << endl;
}
*/
void CTransform::SetTransform ( Vector3d inPos, Rotator inRot, Vector3d inScal )
{
	if ( _parent == NULL ) 
	{
		position = inPos;
		rotation = inRot;
		scale	 = inScal;
	}
	else
	{
		position	= inPos;
		rotation	= inRot;
		scale		= inScal;
		// Convert position,rotation,scaling into local coordinates
		{
			// Update local position by moving the translation into local space
			localPosition = _parent->GetTransformMatrix().inverse() * position;
			// Just take the new rotation in local space
			localRotation = (_parent->GetTransformMatrixRot().inverse().getRotator() * rotation);
			// Just take the new scale, I guess
			localScale = Vector3d( scale.x/_parent->scale.x, scale.y/_parent->scale.y, scale.z/_parent->scale.z );
		}
	}
}
/*
void CTransform::SetLocalTransform ( Matrix4x4 & inMatrix )
{
	if ( _parent == NULL ) 
	{
		throw Core::NotYetImplementedException();
		//SetTransform( inMatrix );
	}
	else
	{
		localRotation = inMatrix.getRotator();
		localScale	  = inMatrix.getScaling();
		localPosition = inMatrix.getTranslation();

		Matrix4x4 prtransfrom = _parent->GetTransformMatrix();
		Matrix4x4 mytransform = prtransfrom * inMatrix;
		position = mytransform.getTranslation();
		rotation = mytransform.getRotator();
		scale	 = mytransform.getScaling();
	}
}
*/
void CTransform::SetLocalTransform ( Vector3d inPos, Rotator inRot, Vector3d inScal )
{
	if ( _parent == NULL ) 
	{
		SetTransform( inPos, inRot, inScal );
	}
	else
	{
		localRotation = inRot;
		localScale	  = inScal;
		localPosition = inPos;

		LateUpdate();
		/*Matrix4x4 prtransfrom = _parent->GetTransformMatrix();
		Matrix4x4 mytransform = prtransfrom * inMatrix;
		position = mytransform.getTranslation();
		//rotation = mytransform.getEulerAngles();
		rotation = mytransform.getRotator();
		//rotation.SwitchBasis();
		scale	 = mytransform.getScaling();*/
	}
}


void CTransform::SetTransform ( const XTransform& inTrans )
{
	SetTransform( inTrans.position, inTrans.rotation, inTrans.scale );
}

void CTransform::SetLocalTransform ( const XTransform& inTrans )
{
	/*if ( _parent == NULL ) 
	{
		SetTransform( inTrans );
	}
	else
	{
		localRotation = inTrans.rotation;
		localScale	  = inTrans.scale;
		localPosition = inTrans.position;

		Matrix4x4 prtransfrom = _parent->GetTransformMatrix();
		Matrix4x4 mytransform = prtransfrom * inMatrix;
		position = mytransform.getTranslation();
		//rotation = mytransform.getEulerAngles();
		rotation = mytransform.getRotator();
		//rotation.SwitchBasis();
		scale	 = mytransform.getScaling();
	}*/
	SetLocalTransform( inTrans.position, inTrans.rotation, inTrans.scale );
}


CTransform* CTransform::FindChild ( const string& sChildName )
{
	// Loop through all children and find the one with the given name
	for ( std::vector<CTransform*>::iterator child = children.begin(); child != children.end(); ++child )
	{
		//cout << "compare: " << (*child)->name << " == " << sChildName << endl;
		//if ( (*child)->name == sChildName )
		if ( (*child)->name.find( sChildName ) != string::npos )
		{
			return (*child);
		}
	}
	return NULL;
}

CTransform* CTransform::FindChildRecursive ( const string& sChildName )
{
	// Loop through all children and find the one with the given name
	CTransform* result = FindChild( sChildName );
	// If can't find the one with the given name, then repeat the process on the children
	if ( result == NULL )
	{
		for ( std::vector<CTransform*>::iterator child = children.begin(); child != children.end(); ++child )
		{
			result = (*child)->FindChildRecursive( sChildName );
			if ( result != NULL )
				return result;
		}
	}
	return result;
}


CTransform* CTransform::FindChildRecursive ( const CTransform* sChildInfo )
{
	if ( sChildInfo == NULL ) {
		return NULL;
	}
	else if ( sChildInfo == &root ) {
		return &root;
	}
	else if ( this == sChildInfo ) {
		return this;
	}
	std::list<CTransform*> transformList;
	transformList.push_front( this );
	while ( !transformList.empty() )
	{
		CTransform* t_transform = transformList.front();
		transformList.pop_front();
		// Loop through children and either return them or push them to the check list
		for ( auto t_tr = t_transform->children.begin(); t_tr != t_transform->children.end(); ++t_tr )
		{
			if ( *t_tr == sChildInfo ) {
				return *t_tr;
			}
			else {
				transformList.push_front( *t_tr );
			}
		}
		//
	}
	return NULL;
}

// Propogates transforms, starting at Root.
void CTransform::PropogateTransforms ( void )
{
	std::list<CTransform*> transformList;
	transformList.push_front( &CTransform::root );
	// Do all the transforms
	while ( !transformList.empty() )
	{
		CTransform* t_transform = transformList.front();
		transformList.pop_front();
		// If transform is active
		if ( t_transform->active ) {
			// Update the transform
			t_transform->LateUpdate();
			// Loop through children and push them to the front of the list
			//cout << t_transform->name << "'s children count: " << t_transform->children.size() << endl;
			for ( auto t_tr = t_transform->children.begin(); t_tr != t_transform->children.end(); ++t_tr )
			{
				transformList.push_front( *t_tr );
			}
		}
		//
	}
}
