
#ifndef _C_MCC_CHARACTER_MODEL_H_
#define _C_MCC_CHARACTER_MODEL_H_

#include "after/entities/CCharacterModel.h"

#include "after/states/CharacterStats.h"
#include "after/states/CRacialStats.h"

#include "renderer/logic/model/morpher/CMorpher.h"

//class CRacialStats;
//struct CRacialStats::tattoo_t;

class Color;
class CTexture;
class CRenderTexture;

class CTattooTester;
class CTattooRenderer;

class CParticleSystem;

class CLipsyncSequence;

class CMccCharacterModel : public CCharacterModel
{
	friend CTattooTester;
	friend CTattooRenderer;
	BaseClass( "CharacterModel" );
public:
	explicit		CMccCharacterModel ( CCharacter* owner, CRacialStats* refstats = NULL );
					~CMccCharacterModel( void );

	// Update Override
	void			Update ( void );

	// Get morph override
	CMorpher*		GetFaceMorpher ( void ) override;
	CLipsyncSequence*	GetLipSyncer ( void );
	void			SetLipsyncPlay ( void );

	// Set transforms here
	// Set custom character skin here
	// Set clothing models here
	// Set tail, ears, hair, and misc here
public:
	enum eHideParts
	{
		PART_HEAD,
		PART_HIPS,
		PART_LEGS,
		PART_ANKLES,
		PART_FEET,
		PART_SHOULDERS,
		PART_ARMS,
		PART_HANDS,
		PART_BODY,
		PART_TORSO,
		PART_LOWER_TORSO,

		PART_MAX_COUNT
	};

public:
	// = Model Set Operations =
	// SetVisualsFromStats ( PlayerStats* )
	// desc:
	//		This sets the default visuals from player stats.
	//		Note that this function is actually slow as balls and should not be called often.
	//		Instead, only call this when a change has DEFINITELY been found
	void			SetVisualsFromStats( CRacialStats* );

	void			SetRace ( const eCharacterRace );
	//void			SetGender ( const eCharacterGender );

	// = Detail Set Operations =
	void			SetHair ( const int );

	void			SetSkinColor ( const Color & );
	void			SetEyeColor ( const Color & );
	void			SetHairColor ( const Color & );

	void			SetShirtModel ( const string & );
	void			SetShortsModel ( const string & );
	void			SetBootsModel ( const string & );
	void			SetGlovesModel ( const string & );
	void			SetHelmetModel ( const string & );
	void			SetGlassesModel ( const string & );
	void			SetShoulderModel ( const string & );

	void			SetPartHidden ( const eHideParts&, const bool );

	// = Queue Update Operations =
	void			UpdateTattoos ( void );

	void			AddDecal ( const Vector3d& n_sourcePosition, const uint32_t n_damagetype );
protected:
	virtual void	Load ( void ) override;

private:
	CRacialStats*	mstats;	// Stats to watch for changes (maybe)

	// Model management
	std::vector<CSkinnedModel**> modelList;
	void			AddAutoUpdate ( CSkinnedModel ** );
	void			SetPartModel ( const string &, CSkinnedModel** );
	void			GetPartFilename ( string & );
	//void			GetPartFilename ( char * );
	void			GetPartFilename ( char * );
	// Essential models
	CSkinnedModel*	mdlBody;
	CSkinnedModel*	mdlHead;
	CSkinnedModel*	mdlEars;
	CSkinnedModel*	mdlHair;
	// "Attached" models
	CSkinnedModel*	mdlShirts;
	CSkinnedModel*	mdlShorts;
	CSkinnedModel*	mdlBoots;
	CSkinnedModel*	mdlGloves;
	CSkinnedModel*	mdlHelmet;
	CSkinnedModel*	mdlGlasses;
	CSkinnedModel*	mdlShoulder;

	// Part visibility
	bool	aPartHidden [PART_MAX_COUNT];
	bool	updatePartMask;
	void			UpdatePartMask ( void );
	// Textures
	CTexture*		texParts [PART_MAX_COUNT];
	CRenderTexture*	texPartMask;
	// Tattoo system
	CTattooRenderer*	tattooRenderer;
	bool				updateTattoos;
	std::vector<CRacialStats::tattoo_t>	tats_cuts;

	// Eye effect
	struct eyeBlinkState_t {
		eyeBlinkState_t ( void ) : fTimeUntilNextBlink(1.0f),fTimer(0.0f),fBlinkLevel(0.0f),bBlinkShut(false) {;};
		ftype	fTimeUntilNextBlink;
		ftype	fTimer;
		ftype	fBlinkLevel;
		bool	bBlinkShut;
	} eyeBlinkState;
	void			UpdateMorphEffects ( void );

	CLipsyncSequence*	mLipsyncer;
	bool				mPlayingLipsync;

	// Race Particle Effects
	CParticleSystem**	mFingerParticles;
};

#endif//_C_MCC_CHARACTER_MODEL_H_