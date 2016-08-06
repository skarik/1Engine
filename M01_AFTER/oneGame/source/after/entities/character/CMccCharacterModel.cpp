#include "CMccCharacterModel.h"
#include "CCharacter.h"

#include "core/math/Math.h"
#include "core/math/Color.h"
#include "core/math/random/Random.h"

#include "core-ext/animation/CAnimIK.h"
#include "core-ext/animation/CAnimation.h"

#include "core/system/io/FileUtils.h"
#include "core-ext/system/io/FileUtils.h"
#include "core-ext/system/io/Resources.h"

#include "engine-common/entities/CParticleSystem.h"
#include "engine-common/physics/motion/CRagdollCollision.h"

#include "renderer/logic/model/CModel.h"
#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/texture/CTexture.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/logic/particle/CParticleEmitter.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "after/states/model/CTattooRenderer.h"
#include "after/states/model/CLipsyncSequence.h"

CMccCharacterModel::CMccCharacterModel( CCharacter* owner, CRacialStats* refstats )
	: CCharacterModel( owner ), mstats( refstats )
{
	// Add models to the manager
	AddAutoUpdate( &mdlBody ); //AddAutoUpdate initializes value to NULL
	AddAutoUpdate( &mdlEars );
	AddAutoUpdate( &mdlHair );
	AddAutoUpdate( &mdlHead );

	AddAutoUpdate( &mdlShirts );
	AddAutoUpdate( &mdlShorts );
	AddAutoUpdate( &mdlBoots );
	AddAutoUpdate( &mdlGloves );
	AddAutoUpdate( &mdlHelmet );
	AddAutoUpdate( &mdlGlasses );
	AddAutoUpdate( &mdlShoulder );

	// Initialize the culling code
	for ( uint i = 0; i < PART_MAX_COUNT; ++i ) {
		texParts[i]		= NULL;
		aPartHidden[i]	= false;
	}
	updatePartMask = true;
	texPartMask = new CRenderTexture( RGBA8, 256,256, Repeat, Repeat );

	// Initialize the tattoo code
	tattooRenderer	= NULL;
	updateTattoos	= false;

	// Initialize finger particles
	mFingerParticles = NULL;

	// Initialize the syncer
	mLipsyncer = new CLipsyncSequence;
	mLipsyncer->m_model = this;
	mPlayingLipsync = false;
}

void CMccCharacterModel::AddAutoUpdate ( CSkinnedModel** ppModel )
{
	*ppModel = NULL;
	modelList.push_back( ppModel );
}

void CMccCharacterModel::Update ( void )
{
	CCharacterModel::Update(); // Call base update
	if ( !charModel ) {
		return;
	}
	// Create ragdoll
	if ( charRagdoll == NULL ) {
		//static int i = 0;
		//if ( (++i)%7 == 6 ) {
			charRagdoll = new CRagdollCollision( charModel );
			charRagdoll->SetActor( (CActor*)actor );
		//}
	}
	// Base model should not be visible
	charModel->SetVisibility( false );

	//cout << "fma: " << fModelAlpha << endl;
	//fModelAlpha = 0.5f + 0.5f*sin(Time::currentTime);

	// Update part mask
	if ( updatePartMask ) {
		UpdatePartMask();
		updatePartMask = false;
	}

	// Update tattoos
	if ( updateTattoos ) {
		//if ( mstats && ( mstats->iTattooCount > 0 ) ) {
		if ( mstats ) {
			if ( tattooRenderer == NULL ) {
				tattooRenderer = new CTattooRenderer( this );
			}
			tattooRenderer->UpdateTattoos();
		}
		updateTattoos = false;
	}

	// Need to update all child models
	for ( uint i = 0; i < modelList.size(); ++i )
	{
		if ( *modelList[i] != NULL ) {
			//(*modelList[i])->transform.Get( charModel->transform );
			(*modelList[i])->transform.position = charModel->transform.position;
			(*modelList[i])->transform.rotation = charModel->transform.rotation;
			(*modelList[i])->SetVisibility( bShowModel );
			//cout << "ACTIVE" << i << endl;

			// Set uniforms for child models
			//uchar passNum = (*modelList[i])->GetPassNumber();
			uchar passNum = (*modelList[i])->GetMeshCount();
			for ( uchar pass = 0; pass < passNum; ++pass ) {
				//(*modelList[i])->GetPassMaterial(pass)->setUniform( "gm_FadeValue", (1-fModelAlpha) );
				(*modelList[i])->SetShaderUniform( "gm_FadeValue", 1-fModelAlpha );
			}
		}
	}

	// Set target transform model
	charTargetModel = mdlBody;

	// Do particle effects
	if ( mstats && (mstats->iRace == CRACE_FLUXXOR) )
	{
		if ( mFingerParticles == NULL )
		{
			//mFingerParticles = new CParticleSystem( "particlesystems/fluxtrail.pcf" ) [2];
			mFingerParticles = new CParticleSystem* [4];
			for ( uint i = 0; i < 4; ++i ) {
				mFingerParticles[i] = new CParticleSystem( "particlesystems/fluxtrail.pcf" );
				mFingerParticles[i]->GetEmitter()->vcColors.clear();
				mFingerParticles[i]->GetEmitter()->vcColors.push_back( Color(1,1,1,1) );
				mFingerParticles[i]->GetRenderable()->GetMaterial()->m_diffuse = mstats->cSkinColor;
			}
		}
		else
		{
			XTransform target;

			Transform* pFinger1 = charModel->GetSkeletonRoot()->FindChildRecursive( "R Finger02" );
			if ( pFinger1 ) {
				target.position = charModel->transform.rotation*pFinger1->position + charModel->transform.position;
				target.rotation = ( !charModel->transform.rotation * pFinger1->rotation ).getQuaternion();
	
				mFingerParticles[0]->transform.position = target.position;
			}
			
			Transform* pFinger2 = charModel->GetSkeletonRoot()->FindChildRecursive( "L Finger02" );
			if ( pFinger2 ) {
				target.position = charModel->transform.rotation*pFinger2->position + charModel->transform.position;
				target.rotation = ( !charModel->transform.rotation * pFinger2->rotation ).getQuaternion();

				mFingerParticles[1]->transform.position = target.position;
			}

			Transform* pFinger3 = charModel->GetSkeletonRoot()->FindChildRecursive( "R Finger22" );
			if ( pFinger3 ) {
				target.position = charModel->transform.rotation*pFinger3->position + charModel->transform.position;
				target.rotation = ( !charModel->transform.rotation * pFinger3->rotation ).getQuaternion();
	
				mFingerParticles[2]->transform.position = target.position;
			}
			
			Transform* pFinger4 = charModel->GetSkeletonRoot()->FindChildRecursive( "L Finger22" );
			if ( pFinger4 ) {
				target.position = charModel->transform.rotation*pFinger4->position + charModel->transform.position;
				target.rotation = ( !charModel->transform.rotation * pFinger4->rotation ).getQuaternion();

				mFingerParticles[3]->transform.position = target.position;
			}
		}
	}
	else
	{
		if ( mFingerParticles )
		{
			for ( uint i = 0; i < 4; ++i ) {
				delete_safe_decrement( mFingerParticles[i] );
			}
			delete [] mFingerParticles;
			mFingerParticles = NULL;
		}
	}

	// Do morph effects
	UpdateMorphEffects();

	// Update IK effects
	if ( charModel )
	{
		Real t_maxStrength = 1-GetRagdollStrength();

		if ( !pHead ) {
			pHead = charModel->GetSkeletonRoot()->FindChildRecursive( "Head" ); 
		}
		// Foot IK
		// Set the IK info
		CAnimation* anim = charModel->GetAnimation();
		{
			ikinfo_t& ik_lf = anim->GetIKInfo( "def_lfoot" );
			if ( mstats && (mstats->iRace == CRACE_KITTEN) ) {
				ik_lf.subinfo[0] = Math.Clamp( pHead->position.z / 5.0f, 0.2f, 1 );
			}
			else {
				ik_lf.subinfo[0] = 0;
			}
			ik_lf.enabled = (t_maxStrength > 0.5f);
		}
		{
			ikinfo_t& ik_rf = anim->GetIKInfo( "def_rfoot" );
			if ( mstats && (mstats->iRace == CRACE_KITTEN) ) {
				ik_rf.subinfo[0] = Math.Clamp( pHead->position.z / 5.0f, 0.2f, 1 );
			}
			else {
				ik_rf.subinfo[0] = 0;
			}
			ik_rf.enabled = (t_maxStrength > 0.5f);
		}
	}
}

CMccCharacterModel::~CMccCharacterModel ( void )
{
	// Free all used model parts
	for ( uint i = 0; i < modelList.size(); ++i )
	{
		if ( (*modelList[i]) != NULL ) {
			delete (*modelList[i]);
			(*modelList[i]) = NULL;
		}
	}

	// Free all the part textures
	for ( uint i = 0; i < PART_MAX_COUNT; ++i ) {
		if ( texParts[i] ) {
			delete texParts[i];
		}
		texParts[i]		= NULL;
	}
	delete texPartMask;
	texPartMask = NULL;

	// Free up the tattoo system
	if ( tattooRenderer != NULL ) {
		delete tattooRenderer;
	}
	tattooRenderer = NULL;

	// Delete finger particles
	if ( mFingerParticles )
	{
		for ( uint i = 0; i < 4; ++i ) {
			delete_safe_decrement( mFingerParticles[i] );
		}
		delete [] mFingerParticles;
		mFingerParticles = NULL;
	}

	// Delete the lipmatcher
	delete mLipsyncer;
	mLipsyncer = NULL;
}

void CMccCharacterModel::SetVisualsFromStats( CRacialStats* refstats )
{
	if ( refstats == NULL ) {
		return;
	}
	mstats = refstats;

	// First, control the reference base model
	/*if ( mstats->iRace == CRACE_FLUXXOR ) {
		if ( GetBasename() != "clara_fluxxor" ) {
			delete charModel;
			charModel = NULL;
			LoadBase( "clara_fluxxor" );
		}
		basename = "clara";
	}
	else {
		if ( GetBasename() != "clara" ) {
			delete charModel;
			charModel = NULL;
			LoadBase( "clara" );
		}
	}*/
	string file = "models/character/" + GetBasename();
	GetPartFilename( file );
	if ( charModel->GetFilename() != file )
	{
		/*if ( charModel ) {
			delete charModel;
			charModel = NULL;
		}
		charModel = new CSkinnedModel( file );*/
		CSkinnedModel* newModel = new CSkinnedModel( file );
		if ( charModel ) {
			newModel->transform.Get( charModel->transform );
			delete_safe( charModel );
			delete_safe( charRagdoll );
		}
		ConfigureBase( newModel );
	}
	for ( uint i = 0; i < modelList.size(); ++i )
	{
		if ( (*modelList[i]) != NULL ) {
			//(*modelList[i])->SetReferencedAnimationMode( charModel );
			(*modelList[i])->SetReferencedBoneMode( charModel );
		}
	}


	// Now, set the values based on the stats
	SetRace( mstats->iRace );
	//SetGender( mstats->iGender );
	SetHair( mstats->iHairstyle );

	SetSkinColor( mstats->cSkinColor );
	SetEyeColor( mstats->cEyeColor );
	SetHairColor( mstats->cHairColor );

	if ( refstats->sGearChest.length() > 0 ) {
		SetShirtModel( refstats->sGearChest );
	}
	else {
		if ( mstats->iGender != CGEND_MALE && mstats->iGender != CGEND_NDEF_MALE ) {
			SetShirtModel( "wraps" );
		}
		else {
			SetShirtModel( "" );
		}
	}
	if ( refstats->sGearLegs.length() > 0 ) {
		SetShortsModel( refstats->sGearLegs );
	}
	else {
		SetShortsModel( "shorts" );
	}
	if ( refstats->sGearRightfoot.length() > 0 ) {
		SetBootsModel( refstats->sGearRightfoot );
	}
	else {
		SetBootsModel( "" );
	}
}

// Loading
void CMccCharacterModel::Load ( void )
{
	string baseName = "models/character/" + GetBasename() + "/";

	// Load base textures
	string baseTexName = "textures/c/" + GetBasename() + "/";
	texParts[PART_HEAD]		= new CTexture( baseTexName + "lhead.png" );
	texParts[PART_HIPS]		= new CTexture( baseTexName + "lhips.png" );
	texParts[PART_LEGS]		= new CTexture( baseTexName + "llegs.png" );
	texParts[PART_ANKLES]	= new CTexture( baseTexName + "lankles.png" );
	texParts[PART_FEET]		= new CTexture( baseTexName + "lfeet.png" );
	texParts[PART_SHOULDERS]= new CTexture( baseTexName + "lshoulders.png" );
	texParts[PART_ARMS]		= new CTexture( baseTexName + "larms.png" );
	texParts[PART_HANDS]	= new CTexture( baseTexName + "lhands.png" );
	texParts[PART_BODY]		= new CTexture( baseTexName + "lbody.png" );
	texParts[PART_TORSO]	= new CTexture( baseTexName + "ltorso.png" );
	texParts[PART_LOWER_TORSO] = new CTexture( baseTexName + "llower_torso.png" );

	// Load base models
	mdlBody = new CSkinnedModel( baseName + "body.fbx" );
	mdlBody->DuplicateMaterials();
	//mdlBody->SetReferencedAnimationMode( charModel );
	mdlBody->SetReferencedBoneMode( charModel );
	charTargetModel = mdlBody;
	mdlHead = new CSkinnedModel( baseName + "head.fbx" );
	mdlHead->DuplicateMaterials();
	//mdlHead->SetReferencedAnimationMode( charModel );
	mdlHead->SetReferencedBoneMode( charModel );

	SetVisualsFromStats( mstats );

	// Update masks and render targets
	UpdatePartMask();
}


void CMccCharacterModel::SetHair ( const int hairType )
{
	char modelName [512];
	sprintf( modelName, "models/character/%s/hair%02d", GetBasename().c_str(), hairType );
	GetPartFilename( modelName );
	if ( !IO::ModelExists( modelName ) ) {
		sprintf( modelName, "models/character/%s/hair00.FBX", GetBasename().c_str() );
	}

	if ( mdlHair != NULL ) {
		if ( mdlHair->GetFilename() == modelName && hairType != -1 ) {
			return;
		}
		delete_safe( mdlHair );
	}
	if ( hairType != -1 ) {
		mdlHair = new CSkinnedModel( modelName );
		mdlHair->DuplicateMaterials();
		//mdlHair->SetReferencedAnimationMode( charModel );
		mdlHair->SetReferencedBoneMode( charModel );
	}
}


void CMccCharacterModel::SetRace ( const eCharacterRace race )
{
	bool canLoad = true;
	string modelName;

	// Set the body model
	modelName = "models/character/" + GetBasename() + "/body";
	GetPartFilename( modelName );
	if ( mdlBody != NULL ) {
		if ( mdlBody->GetFilename() == modelName ) {
			//
		}
		else {
			delete_safe( mdlBody );
		}
	}
	if ( mdlBody == NULL ) {
		mdlBody = new CSkinnedModel( modelName );
		mdlBody->DuplicateMaterials();
		//mdlBody->SetReferencedAnimationMode( charModel );
		mdlBody->SetReferencedBoneMode( charModel );

		UpdateTattoos();
	}
	charTargetModel = mdlBody;

	// Set the head model
	modelName = "models/character/" + GetBasename() + "/head";
	GetPartFilename( modelName );
	if ( mdlHead != NULL ) {
		if ( mdlHead->GetFilename() == modelName ) {
			//
		}
		else {
			delete_safe( mdlHead );
		}
	}
	if ( mdlHead == NULL ) {
		mdlHead = new CSkinnedModel( modelName );
		mdlHead->DuplicateMaterials();
		//mdlBody->SetReferencedAnimationMode( charModel );
		mdlHead->SetReferencedBoneMode( charModel );

		UpdateTattoos();
	}

	// Set the ears
	modelName = "models/character/" + GetBasename() + "/";
	if ( race == CRACE_HUMAN ) {
		modelName += "ears_human.FBX";
	}
	else if ( race == CRACE_ELF ) {
		modelName += "ears_elven.FBX";
	}
	else if ( race == CRACE_KITTEN ) {
		modelName += "ears_kitten.FBX";
	}
	else if ( race == CRACE_FLUXXOR ) {
		modelName += "ears_fluxxor.FBX";
	}
	else {
		delete_safe( mdlEars );
		canLoad = false;
	}

	if ( mdlEars != NULL ) {
		if ( mdlEars->GetFilename() == modelName ) {
			canLoad = false;	
		}
		else {
			delete_safe( mdlEars );
		}
	}
	if ( canLoad ) {
		mdlEars = new CSkinnedModel( modelName );
		mdlEars->DuplicateMaterials();
		//mdlEars->SetReferencedAnimationMode( charModel );
		mdlEars->SetReferencedBoneMode( charModel );
	}

	// Set the eyeball materials
	glMaterial* eyeMatA,* eyeMatB;
	if ( mdlHead ) {
		eyeMatA = mdlHead->FindMaterial( "eye",0 );
		eyeMatB = mdlHead->FindMaterial( "eye",1 );
		if ( eyeMatA ) {
			if ( race == CRACE_ELF )
				eyeMatA->loadFromFile( "c/eyes_elf" );
			else if ( race == CRACE_FLUXXOR )
				eyeMatA->loadFromFile( "c/eyes_fluxxor" );
			else if ( race == CRACE_KITTEN )
				eyeMatA->loadFromFile( "c/eyes_kitty" );
			else
				eyeMatA->loadFromFile( "c/eyes" );
		}
		if ( eyeMatB ) {
			if ( race == CRACE_ELF )
				eyeMatB->loadFromFile( "c/eyes_elf" );
			else if ( race == CRACE_FLUXXOR )
				eyeMatB->loadFromFile( "c/eyes_fluxxor" );
			else if ( race == CRACE_KITTEN )
				eyeMatB->loadFromFile( "c/eyes_kitty" );
			else
				eyeMatB->loadFromFile( "c/eyes" );
		}
	} 
}

void CMccCharacterModel::GetPartFilename ( string & modelName )
{
	string previousName = modelName;
	if ( mstats->iGender == CGEND_MALE || mstats->iGender == CGEND_NDEF_MALE )
	{
		modelName += "_m";
	}
	if ( mstats->iRace )
	{
		if (( mstats->iRace == CRACE_FLUXXOR )&& IO::ModelExists( modelName + "_fluxxor" )) {
			modelName = modelName + "_fluxxor.FBX";
			return;
		}
		else if (( mstats->iRace == CRACE_DWARF )&& IO::ModelExists( modelName + "_dwarf" )) {
			modelName = modelName + "_dwarf.FBX";
			return;
		}
		else if (( mstats->iRace == CRACE_KITTEN )&& IO::ModelExists( modelName + "_kitten" )) {
			modelName = modelName + "_kitten.FBX";
			return;
		}
		else if ( IO::ModelExists( modelName ) ) {
			modelName = modelName + ".FBX";
			return;
		}
	}
	// If here, then was a failure on first pass
	modelName = previousName; //reset to base set
	// perform identical check again
	if ( mstats->iRace )
	{
		if (( mstats->iRace == CRACE_FLUXXOR )&& IO::ModelExists( modelName + "_fluxxor" )) {
			modelName = modelName + "_fluxxor.FBX";
			return;
		}
		else if (( mstats->iRace == CRACE_DWARF )&& IO::ModelExists( modelName + "_dwarf" )) {
			modelName = modelName + "_dwarf.FBX";
			return;
		}
		else if (( mstats->iRace == CRACE_KITTEN )&& IO::ModelExists( modelName + "_kitten" )) {
			modelName = modelName + "_kitten.FBX";
			return;
		}
		else if ( IO::ModelExists( modelName ) ) {
			modelName = modelName + ".FBX";
			return;
		}
	}
	// if here, then couldn't find anything
	Debug::Console->PrintError( "Could not find char part: "+modelName+"!\n" );
	// But apply default model anyways
	modelName = modelName + ".FBX";
}

void CMccCharacterModel::GetPartFilename ( char* modelName )
{
	string temp ( modelName );
	GetPartFilename( temp );
	strcpy( modelName, temp.c_str() );
}
/*void CMccCharacterModel::GetPartFilename ( char * modelName )
{
	bool success = false;

	char tempString [512];
	strcpy( tempString, modelName );

	if ( mstats->iRace )
	{
		if ( mstats->iRace == CRACE_FLUXXOR ) {
			sprintf( modelName, "%s_fluxxor.FBX", tempString );
			if ( IO::FileExists( modelName ) ) {
				success = true;
			}
			else
			{
				sprintf( modelName, "%s_fluxxor.PAD", tempString );
				if ( IO::FileExists( modelName ) ) {
					success = true;
				}
			}
		}
		else if ( mstats->iRace == CRACE_DWARF ) {
			sprintf( modelName, "%s_dwarf.FBX", tempString );
			if ( IO::FileExists( modelName ) ) {
				success = true;
			}
			else
			{
				sprintf( modelName, "%s_dwarf.PAD", tempString );
				if ( IO::FileExists( modelName ) ) {
					success = true;
				}
			}
		}
	}
	if ( !success ) {
		sprintf( modelName, "%s.FBX", tempString );
		if ( !IO::FileExists( modelName ) ) {
			sprintf( modelName, "models/character/%s/hair%02d", GetBasename().c_str(), 0 );
		}
	}
}*/

void CMccCharacterModel::SetPartModel ( const string & partName, CSkinnedModel** pMdlTarget )
{
	if ( partName.length() <= 1 ) {
		delete *pMdlTarget;
		*pMdlTarget = NULL;
		return;
	}
	string modelName = "models/character/" + GetBasename() + "/" + partName;
	GetPartFilename( modelName );
	if ( *pMdlTarget != NULL ) {
		if ( (*pMdlTarget)->GetFilename() == modelName ) {
			return;
		}
		delete *pMdlTarget;
	}
	*pMdlTarget = new CSkinnedModel( modelName );
	//(*pMdlTarget)->SetReferencedAnimationMode( charModel );
	(*pMdlTarget)->SetReferencedBoneMode( charModel );
}
void CMccCharacterModel::SetShirtModel ( const string & shirtname )
{
	SetPartModel( shirtname, &mdlShirts );
}
void CMccCharacterModel::SetShortsModel ( const string & shortsname )
{
	SetPartModel( shortsname, &mdlShorts );
}
void CMccCharacterModel::SetBootsModel ( const string & bootsname )
{
	SetPartModel( bootsname, &mdlBoots );
}
void CMccCharacterModel::SetGlovesModel ( const string & glovesname )
{
	SetPartModel( glovesname, &mdlGloves );
}
void CMccCharacterModel::SetHelmetModel ( const string & helmetname )
{
	SetPartModel( helmetname, &mdlHelmet );
}
void CMccCharacterModel::SetGlassesModel ( const string & glassesname )
{
	SetPartModel( glassesname, &mdlGlasses );
}
void CMccCharacterModel::SetShoulderModel ( const string & shouldername )
{
	SetPartModel( shouldername, &mdlShoulder );
}

void CMccCharacterModel::SetHairColor ( const Color & c )
{
	glMaterial* hairMat;
	if ( mdlHair ) {
		hairMat = mdlHair->FindMaterial( "hair",0 );
		if ( hairMat ) {
			hairMat->m_diffuse = c;
		}
		hairMat = mdlHair->FindMaterial( "hair",1 );
		if ( hairMat ) {
			hairMat->m_diffuse = c;
		}
	}
	if ( mdlHead ) {
		hairMat = mdlHead->FindMaterial( "hair" );
		if ( hairMat ) {
			hairMat->m_diffuse = c;
		}
	}
	if ( mdlEars ) {
		hairMat = mdlEars->FindMaterial( "hair" );
		if ( hairMat ) {
			hairMat->m_diffuse = c;
		}
	}
}
void CMccCharacterModel::SetEyeColor ( const Color & c )
{
	Color tEmissive = Color(0,0,0,0);
	if ( mstats->iRace == CRACE_ELF ) {
		tEmissive = c*0.2f;
	}
	else if ( mstats->iRace == CRACE_FLUXXOR ) {
		tEmissive = c*0.5f;
	}

	glMaterial* eyeMatA,* eyeMatB;
	if ( mdlHead ) {
		eyeMatA = mdlHead->FindMaterial( "eye",0 );
		eyeMatB = mdlHead->FindMaterial( "eye",1 );
		if ( eyeMatA ) {
			eyeMatA->m_diffuse = c;
			eyeMatA->m_emissive = tEmissive;
		}
		if ( eyeMatB ) {
			eyeMatB->m_diffuse = c;
			eyeMatB->m_emissive = tEmissive;
		}
	}
}
void CMccCharacterModel::SetSkinColor ( const Color & c )
{
	Color tEmissive = Color(0,0,0,0);
	if ( mstats->iRace == CRACE_FLUXXOR ) {
		tEmissive = c*0.38f;
	}

	glMaterial* skinMat;
	if ( mdlBody ) {
		skinMat = mdlBody->FindMaterial( "skin" );
		if ( skinMat ) {
			skinMat->m_diffuse = c;
			skinMat->m_emissive = tEmissive;
		}
	}
	if ( mdlHead ) {
		skinMat = mdlHead->FindMaterial( "skin" );
		if ( skinMat ) {
			skinMat->m_diffuse = c;
			skinMat->m_emissive = tEmissive;
		}
	}
	if ( mdlEars ) {
		skinMat = mdlEars->FindMaterial( "skin" );
		if ( skinMat ) {
			skinMat->m_diffuse = c;
			skinMat->m_emissive = tEmissive;
		}
	}
}

// Toggle part hiding via a mask
void CMccCharacterModel::SetPartHidden ( const eHideParts& partToHide, const bool hidePart )
{
	if ( aPartHidden[partToHide] != hidePart )
	{
		updatePartMask = true;
		aPartHidden[partToHide] = hidePart;
	}
}
#include "renderer/debug/CDebugRTInspector.h"
// Update part mask
void CMccCharacterModel::UpdatePartMask ( void )
{
	// Prepare to render to texture (directly!)
	GL_ACCESS;
	GLd_ACCESS;
	{	// Bind buffer
		texPartMask->BindBuffer();
	}

	{	// Set up the rendering thing
		//GL.setupViewport( 0,0,256,256 ); // DirectX
		GL.setupViewport( 0,0,256,256 ); // OpenGL
	}
	GL.beginOrtho( 0,0, 256,256, -45,45, false );

	GL.clearColor( Color( 0.0f,0.0f,0.0f,1.0f ) );
	GL.ClearBuffer( GL_COLOR_BUFFER_BIT );

	glMaterial tempMat;
	tempMat.passinfo.push_back( glPass() );
	tempMat.passinfo[0].m_blend_mode = Renderer::BM_ADD;
	tempMat.passinfo[0].m_face_mode = Renderer::FM_BACK;
	tempMat.passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );
	tempMat.removeReference();
	GLd.DrawSet2DMode( GLd.D2D_FLAT );
	for ( uint i = 0; i < PART_MAX_COUNT; ++i )
	{
		if ( aPartHidden[i] ) {
			tempMat.setTexture( 0, texParts[i] );
			tempMat.bindPass(0);
			GLd.DrawRectangle( 0,0, 256,256 );
		}
	}

	GL.endOrtho();


	{	// Unbind buffer
		texPartMask->UnbindBuffer();
	}

	///Debug::RTInspector->AddWatch( texPartMask );

	// Update skin mat
	glMaterial* skinMat;
	if ( mdlBody ) {
		skinMat = mdlBody->FindMaterial( "skin" );
		if ( skinMat ) {
			skinMat->setTexture( 1, texPartMask );
		}
	}
	/*if ( mdlHead ) {
		skinMat = mdlHead->FindMaterial( "skin" );
		if ( skinMat ) {
			skinMat->pTextures[1] = texPartMask;
		}
	}
	if ( mdlEars ) {
		skinMat = mdlEars->FindMaterial( "skin" );
		if ( skinMat ) {
			skinMat->pTextures[1] = texPartMask;
		}
	}*/
}

void	CMccCharacterModel::UpdateTattoos ( void )
{
	updateTattoos = true;
}

CMorpher* CMccCharacterModel::GetFaceMorpher ( void )
{
	CMorpher* morpher = NULL;
	if ( mdlHead ) {
		morpher = mdlHead->GetMorpher();
	}
	return morpher;
}
CLipsyncSequence* CMccCharacterModel::GetLipSyncer ( void )
{
	return mLipsyncer;
}
void CMccCharacterModel::SetLipsyncPlay ( void )
{
	mPlayingLipsync = true;
}

void	CMccCharacterModel::UpdateMorphEffects ( void )
{
	CMorpher* morpher = NULL;
	if ( mdlHead ) {
		morpher = mdlHead->GetMorpher();
	}
	
	if ( !morpher ) {
		return;
	}

	// Perform blink effects
	//if ( !character || ( character && character->IsAlive() ) )
	{
		if ( !actor || ( actor && actor->IsAlive() ) )
		{
			// Blink between 100% and 10-15%
			if ( eyeBlinkState.bBlinkShut ) {
				eyeBlinkState.fBlinkLevel += Time::deltaTime*10.0f;
				if ( eyeBlinkState.fBlinkLevel > 1.0f ) {
					eyeBlinkState.fBlinkLevel = 1.0f;
					eyeBlinkState.bBlinkShut = false;
				}
			}
			else {
				eyeBlinkState.fBlinkLevel -= Time::deltaTime*9.0f;
				if ( eyeBlinkState.fBlinkLevel <= 0.12f ) {
					eyeBlinkState.fBlinkLevel = 0.12f;
				}
			}
		}
		else // Character is dead, close their eyes (mostly)
		{
			eyeBlinkState.fBlinkLevel += (0.9f-eyeBlinkState.fBlinkLevel)*0.01f;
		}
		CMorphAction* blinkAction = morpher->FindAction("blink");
		if ( blinkAction ) {
			blinkAction->weight = eyeBlinkState.fBlinkLevel;
		}
		// Blink at a specified time interval
		eyeBlinkState.fTimer += Time::deltaTime;
		if ( eyeBlinkState.fTimer > eyeBlinkState.fTimeUntilNextBlink ) {
			eyeBlinkState.bBlinkShut = true;
			eyeBlinkState.fTimer = 0;
			eyeBlinkState.fTimeUntilNextBlink  = Random.Range( 0.4f, 14.0f );
		}
	}

	// Perform idle mouth fidget effects
	{
		CMorphAction* mouthBreath = morpher->FindAction("mouthBreathe");
		CMorphAction* smile = morpher->FindAction("smile");
		if ( mouthBreath && smile )
		{
			mouthBreath->weight = Math.Clamp( (sinf( Time::currentTime*1.12f )-0.2f)*1.5f, 0, 0.9f );
			smile->weight = Math.Clamp( (sinf( Time::currentTime*0.53f )-0.8f)*6.0f, 0, 0.7f );
		}
	}

	// Perform lipsyncer now
	if ( mPlayingLipsync )
	{
		mLipsyncer->m_position += Time::deltaTime;
		if ( mLipsyncer->m_position >= mLipsyncer->m_sequence_length ) {
			mLipsyncer->m_position = mLipsyncer->m_sequence_length;
			mPlayingLipsync = false;
		}
		mLipsyncer->UpdateMorphs();
	}
}


void CMccCharacterModel::AddDecal ( const Vector3d& n_sourcePosition, const uint32_t n_damagetype )
{
	CRacialStats::tattoo_t cutdecal;
	cutdecal.color = Color( 0.45f,0.45f,0.45f ).Lerp( mstats->cSkinColor * 0.45f, 0.1f );
	cutdecal.mirror = false;

	if ( n_damagetype == DamageType::Slash )
	{
		cutdecal.pattern = "slice1";
	}

	cutdecal.projection_pos = n_sourcePosition - transform.position;
	cutdecal.projection_pos -= GetSkeletonRoot()->position;
	cutdecal.projection_pos = GetModelRotation().transpose() * cutdecal.projection_pos;
	/*cutdecal.projection_pos.x *= 2;
	cutdecal.projection_pos.y *= 2;*/
	std::cout << cutdecal.projection_pos << std::endl;
	cutdecal.projection_angle = Random.Range(-90.0f,90.0f);
	cutdecal.projection_scale = Vector3d(1.4f,1.4f,1.4f);
	cutdecal.projection_dir = -cutdecal.projection_pos;
	//cutdecal.projection_dir.z *= 0.3;
	cutdecal.projection_dir.z *= 0.02f;
	cutdecal.projection_dir.normalize();

	tats_cuts.push_back( cutdecal );
	UpdateTattoos();
}