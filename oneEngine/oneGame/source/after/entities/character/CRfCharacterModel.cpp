
#include "CRfCharacterModel.h"
//#include "CRagdollTest.h"
#include "engine-common/physics/motion/CRagdollCollision.h"

CRfCharacterModel::CRfCharacterModel ( CCharacter* owner )
	: CCharacterModel( owner )
{
	refModel = NULL;
}

CRfCharacterModel::~CRfCharacterModel ( void )
{
	//cout << "Entering ~CRfCharacterModel" << endl;
	if ( refModel )
		delete refModel;
	refModel = NULL;
}

bool CRfCharacterModel::LoadModel ( const string& model_filename )
{
	CCharacterModel::LoadModel( model_filename );

	string file = "models/" + model_filename + "/ref.fbx";
	refModel = new CSkinnedModel ( file );
	refModel->SetReferencedBoneMode( charModel );

	//new CRagdollTest( charModel );
	
	return true;
}

void CRfCharacterModel::Update ( void )
{
	CCharacterModel::Update();
	if ( !charModel ) {
		return;
	}
	// Create ragdoll
	if ( !charRagdoll ) {
		charRagdoll = new CRagdollCollision( charModel );
		charRagdoll->SetActor( actor );
	}
	// Base model should not be visible
	//charModel->visible = false;
	charModel->SetVisibility( false );

	if ( refModel ) {
		refModel->transform.position = charModel->transform.position;
		refModel->transform.rotation = charModel->transform.rotation;
		//refModel->visible = bShowModel;
		refModel->SetVisibility( bShowModel );

		// Set uniforms for child models
		//uchar passNum = refModel->GetPassNumber();
		uchar passNum = refModel->GetMeshCount();
		for ( uchar pass = 0; pass < passNum; ++pass ) {
			//refModel->GetPassMaterial(pass)->setUniform( "gm_FadeValue", 1-fModelAlpha );
			refModel->SetShaderUniform( "gm_FadeValue", 1-fModelAlpha );
		}
	}
}