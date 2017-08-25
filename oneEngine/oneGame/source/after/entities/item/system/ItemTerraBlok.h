
#ifndef _ITEM_TERRA_BLOK_H_
#define _ITEM_TERRA_BLOK_H_

// Includes
#include "engine/physics/collider/types/CBoxCollider.h"

#include "after/entities/item/CWeaponItem.h"
#include "after/types/terrain/DirectionFlags.h"

// Class Definition
class ItemTerraBlok : public CWeaponItem
{
	ClassName( "ItemTerraBlok" );

	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 2;
		wpdata.bCanStack = true;
		wpdata.iMaxStack = 128;
		wpdata.sInfo = "Terrain Block";
		return wpdata;
	}
public:
	// == Constructor ==
	explicit ItemTerraBlok ( void );
	explicit ItemTerraBlok ( glMaterial*, ushort );
	// == Destructor ==
	~ItemTerraBlok ( void );
	// == Copier ==
	CWeaponItem& operator= ( const CWeaponItem * original ) override;
	// == Comparison ==
	bool IsEqual ( CWeaponItem* compare ) override;
	// Serialization
	virtual void serialize ( Serializer &, const uint );

	// Initialize the physics object
	void CreatePhysics ( void );

	// Update function
	void Update ( void );
	// Lateupdate function
	void LateUpdate ( void );

	// Use function
	bool Use( int x );

	//Returns the blok type
	ushort GetType (void);
	void SetType (ushort);

	// Lookat calls
	void	OnInteractLookAt	( CActor* interactingActor ) override;
	void	OnInteractLookAway	( CActor* interactingActor ) override;

	// Sets the transform to set up a camera
	void SetInventoryCameraTransform ( XTransform & ) override;
	void SetInventoryCameraWidth ( ftype & ) override;
	// Grabs the icon for this object
	CTexture* GetInventoryIcon ( void ) override;
private:
	void			GenerateModel ( void );
	void			GenerateMaterial ( void );

	glMaterial*		blokMaterial;
	ushort			blokType;
	//CRenderablePrimitive* blokRender;
	CModel*			blokRender;
	float			fHoverTimer;

	// Set the model
	CModelVertex* SetVertices ( void );
	// Set the uvs
	void SetUVs ( CModelVertex*, Terrain::EFaceDir );
	void SetColors ( CModelVertex* vertices );
};

#endif