
#ifndef _C_BASE_RANDOM_BOW_H_
#define _C_BASE_RANDOM_BOW_H_

#include "CBaseRandomItem.h"

class AmmoArrow;

class CBaseRandomBow : public CBaseRandomItem
{
	ClassName( "RandomBaseBow" );
	BaseClass( "RandomBaseBow" );
public:
	struct tBowWeaponProperties;

	explicit CBaseRandomBow ( tBowWeaponProperties&, const WItemData & wdat );
	~CBaseRandomBow ( void );
	CWeaponItem& operator= ( const CWeaponItem * original ) override;

	// Serialization
	virtual void serialize ( Serializer &, const uint );

	// Properties struct
	struct tBowWeaponProperties
	{
		ftype range;
		ftype damage;
		int hands;
		ftype speed;
		ftype pspeed;
	};
	tBowWeaponProperties weapon_stats;

	// Use function
	bool Use ( int x );

	void StartUse ( int x );
	// Attack function
	//void Attack ( void ) override;

	void Update ( void );
	void LateUpdate ( void );

	// This is called when the HUD for the current weapon needs to be drawn.
	// For this class, draws bow aiming reticle the indicates pullback strength
	void OnDrawHUD ( const WReticleStyle& n_reticleStyle );
	// Randomized weapons get a slightly different tooltip that shows their stats.
	void DrawTooltip ( const Vector2d & drawPos, const WTooltipStyle & style ) override;

private:
	int		pauseTime;
	ftype	tension;
	bool	hasAmmo;
	AmmoArrow*	currentAmmo;
};

#endif