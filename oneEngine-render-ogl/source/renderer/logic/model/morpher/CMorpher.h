
#ifndef _C_MORPHER_H_
#define _C_MORPHER_H_

// == Includes ==
#include "core/types/ModelData.h"
#include "CMorpherSet.h"

// Using string and map to reference animation
#include "core/containers/arstring.h"
#include <map>

// Include morph "action"
#include "CMorphAction.h"

// Prototypes
class CModel;
class CSkinnedModel;
class glSkinnedMesh;

// Morpher class
// Used for badass morphing
class CMorpher
{

public:
	RENDER_API explicit CMorpher ( const string &, CMorpher* );
	RENDER_API explicit CMorpher ( const string &, CMorpherSet* );

	RENDER_API CMorpher&	operator= ( CMorpher const& );
	RENDER_API CMorpher&	operator= ( CMorpher* );

	RENDER_API virtual ~CMorpher ( void );

	RENDER_API CMorphAction& operator [] ( const char* );
	RENDER_API CMorphAction& operator [] ( const int & );
	RENDER_API CMorphAction* FindAction ( const char* );

	RENDER_API void AddAction ( CMorphAction );

	CMorpherSet*	GetMorpherSet ( void ) {
		return pMorphSet;
	}
	bool	IsValid ( void ) {
		return bIsValid;
	}

	void SetOwner ( CModel* model ) {
		pOwner = model;
	}

	static CMorphAction	deadAction;
protected:
	// Function for model system
	void	PerformMorph ( glSkinnedMesh* );

protected:
	friend CMorphAction;
	friend CModel;
	friend CSkinnedModel;

	std::map<arstring<128>,CMorphAction>	mActions;
	CMorpherSet*			pMorphSet;
	string					sFilename;

	bool					bIsValid;

	CModel*					pOwner;
};

#endif//_C_MORPHER_H_