
#ifndef _C_RF_CHARACTER_MODEL_H_
#define _C_RF_CHARACTER_MODEL_H_

#include "after/entities/CCharacterModel.h"

#include "renderer/logic/model/CSkinnedModel.h"

#include "after/entities/character/CCharacter.h"

class CRfCharacterModel : public CCharacterModel
{

public:
	explicit		CRfCharacterModel ( CCharacter* owner );
					~CRfCharacterModel( void );

	// LoadModel ( model_filename )
	// returns:
	//		true when model loading is successful, false if error
	// description:
	//		Loads the base model with the given name.
	//		The basename is set to an empty string. If the model can not be found, then the game crashes. Usually. Or undefined behavior.
	bool			LoadModel ( const string& model_filename );


	void			Update ( void );

private:
	CSkinnedModel*	refModel;

};


#endif