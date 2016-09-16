// The foliage class is, in most simple terms, an empty class
// It's purpose is to provide a common interface that classes can
//  inherited from while still being able to be stored and saved/loaded by the CVoxelTerrain class.


#ifndef _C_FOLIAGE_BASE_H_
#define _C_FOLIAGE_BASE_H_

// ==Includes==
// Include for basic components
#include "engine-common/entities/CActor.h"
#include "renderer/logic/model/CModel.h"
#include "engine/physics/collider/types/CStaticMeshCollider.h"

// Include for input/output interface
#include <iostream>
using std::ostream;
using std::istream;

// ==Class Definition==
class CFoliage : public CActor
{
	ClassName( "CFoliage" );
	BaseClass( "CActor_Foliage" );
public:
	CFoliage ( void );
	~CFoliage ( void );

	void RemoveFromTerrain ( void );
	//void Update ( void );

	virtual void SetToggle ( const char * ) {};
	virtual void GetToggle ( char * ) {};

	//virtual void SaveData ( ostream& );
	//virtual void LoadData ( istream& );
	//CBoob*	pBoob;

private:
	//CModel * pTreeModel;

};

#endif