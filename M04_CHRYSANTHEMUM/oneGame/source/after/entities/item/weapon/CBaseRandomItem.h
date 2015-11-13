//
//
// CBaseRandomItem
// 
// The base class for all randomized weapons and items in the game.
//
//


#ifndef _C_BASE_RANDOM_ITEM_H_
#define _C_BASE_RANDOM_ITEM_H_

#include "after/entities/item/CWeaponItem.h"
#include "ItemAdditives.h"

class CBaseRandomItem : public CWeaponItem
{
	ClassName( "AbstractRandomItem" );
	BaseClass( "AbstractRandomItem" );
public:
	CBaseRandomItem ( const WItemData & wdat );
	~CBaseRandomItem ( void );
	CWeaponItem& operator= ( const CWeaponItem * original ) override;

	// Serialization
	virtual void serialize ( Serializer &, const uint );

	struct tForgeProperties
	{
		// Current additives and their strength
		/*struct _t_additives {
			ItemAdditives::eItemAdditive type;
			ftype strength;
		} additives [4];*/
		// Current enchantments and their strength
		struct _t_enchantments {
			// Todo: enchantment type
			ftype strength;
		} enchantments [6];
		
		tForgeProperties ( void )
		{
			/*for ( uint i = 0; i < 4; ++i ) {
				additives[i].type = ItemAdditives::None;
				additives[i].strength = 0;
			}*/
			for ( uint i = 0; i < 6; ++i ) {
				enchantments[i].strength = 0;
			}
		}
	};
	tForgeProperties forge_stats;
	struct tPartProperties {
		arstring<128> brand;
		arstring<128> componentMain;
		arstring<128> componentSecondary;
		arstring<128> component3;
		arstring<128> component4;

		ItemAdditives::eItemAdditive	materialMain;
		ItemAdditives::eItemAdditive	materialSecondary;
		ItemAdditives::eItemAdditive	material3;
		ItemAdditives::eItemAdditive	material4;

		tPartProperties ( void ) :
			brand("BASIC"),
			componentMain(""), componentSecondary(""), component3(""), component4(""),
			materialMain(ItemAdditives::Invalid), materialSecondary(ItemAdditives::Invalid), material3(ItemAdditives::Invalid), material4(ItemAdditives::Invalid)
		{
			;
		}
	};
	tPartProperties part_info;

	// Virtual function to put your weapon model loading into
	virtual void Generate ( void );
	
	// Custom physics generation based on vModels
	void	CreatePhysics ( void );

	// Has specific update and postupdate code
	void	Update ( void );
	void	PostUpdate ( void );

	// Lookat calls (highlighting when mouse is over and whatnot)
	void	OnInteractLookAt	( CActor* interactingActor );
	void	OnInteractLookAway	( CActor* interactingActor );

	// Sets the transform to set up a camera for rendering an icon
	void	SetInventoryCameraTransform ( XTransform & ) override;
	void	SetInventoryCameraWidth ( ftype & ) override;
	// Render the model
	void	InventoryIconRenderModel ( void ) override;
protected:
	bool bGenerated;

	std::vector<CModel*>	vModels;

};

#endif