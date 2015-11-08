#include "CPlayerInventoryGUI.h"

#include "core/math/Rect.h"

#include "renderer/system/glDrawing.h"
#include "renderer/material/glMaterial.h"

#include "after/entities/character/CAfterPlayer.h"

#include "after/entities/item/system/ItemTerraBlok.h"
#include "after/entities/hud/player/CPlayerLogbook.h"

#include "after/entities/item/weapon/CBaseRandomItem.h"
#include "after/entities/item/weapon/CBaseRandomMelee.h"
#include "after/entities/item/weapon/CBaseRandomGun.h"
#include "after/entities/item/weapon/CBaseRandomTossable.h"

#include "after/entities/item/weapon/melee/CRWSword.h"
#include "after/entities/item/ItemCrafting.h"
#include "after/entities/item/ItemForging.h"

#include "after/states/inventory/CPlayerInventory.h"

//Draw the crafting stuff and the result
void CPlayerInventoryGUI::DrawCrafting (void)
{
	//Initialize a lot of stuff
	CWeaponItem** temp;

	GLd_ACCESS;

	temp = pCraft->GetItems();
	GLd.DrawSet2DMode( GLd.D2D_FLAT );
	GLd.DrawSet2DScaleMode(GLd.SCALE_HEIGHT);
	short height = pCraft->GetHeight();
	short width = pCraft->GetWidth();
	//short boxes = height * width;
	//float height_spacing = .6 / height;
	//float width_spacing = .4 / width;
	tItemRecipe::item_entry product = pCraft->GetResult();
	
	if ( !sCrafting.minimized )
	{
		//Draw the background
		matDrawDebug->m_diffuse = Color (.85F, .85F, .85F);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( sCrafting.position.x, sCrafting.position.y, sCrafting.size.x, sCrafting.size.y);

		//Draw the minimize button
		matDrawDebug->m_diffuse = Color (.15F, .8F, .15F);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA ( sCrafting.position.x + .33F, sCrafting.position.y + .01F, .03F, .03F);

		//Draw the close button
		matDrawDebug->m_diffuse = Color (.15F, .15F, .8F);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA ( sCrafting.position.x + .37F, sCrafting.position.y + .01F, .03F, .03F);

		// Draw the highlights and text for the tabs
		if ( sBag == CRAFT && sSlotX == -2 )
		{
			// Draw the highlight
			matDrawDebug->m_diffuse = Color ( 0.9F, .2F, .2F );
			matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( sCrafting.position.x - 0.045F, sCrafting.position.y + (0.11F*sSlotY) - 0.005F + 0.01F, 0.045F, 0.11F );

			// Draw the text
			const char* tabTextInfo[] = { "Craft", "Forge", "Cook", "Brew" };
			matFntDebug->setTexture( 0, fntDebug );
			matFntDebug->m_diffuse = Color (1.0F, 1.0F, 1.0F);
			matFntDebug->bindPass(0);
			GLd.DrawAutoText( sCrafting.position.x - 0.11F, sCrafting.position.y + (0.11F*sSlotY) + 0.05F, tabTextInfo[sSlotY] );
		}

		//Draw the tabs
		matDrawDebug->m_diffuse = Color (.55F, .55F, .75F);
		matDrawDebug->bindPass(0);
		for (short i = 0; i < 4; i++)
			GLd.DrawRectangleA( sCrafting.position.x - .04F, sCrafting.position.y + (.11F * i) + 0.01F, .04F, .1F);
		 // draw selected tab
		matDrawDebug->m_diffuse = Color (.85F, .85F, .85F);
		matDrawDebug->bindPass(0);
		GLd.DrawRectangleA( sCrafting.position.x - .04F, sCrafting.position.y + (.11F * sCrafting.substate) + 0.01F, .04F, .1F);

		// Draw the substates
		if ( sCrafting.substate == 0 ) {
			DrawCraftingDefault();
		}
		else if ( sCrafting.substate == 1 ) {
			DrawCraftingForge();
		}

		// Draw the mouse text
		if ( mCraftingState.sMouseMessageAlpha > 0 ) {
			matFntDebug->setTexture( 0, fntDebug );
			matFntDebug->m_diffuse = Color ( 0.9F, 0.2F, 0.1F, mCraftingState.sMouseMessageAlpha );
			matFntDebug->bindPass(0);
			GLd.DrawAutoTextCentered( sMouseX, sMouseY - 0.02F, mCraftingState.sMouseMessage.c_str() );
		}
	}
	else
	{
		matDrawDebug->m_diffuse = Color (.85F, .85F, .85F);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( sCrafting.position.x, sCrafting.position.y, sCrafting.size.x, .05F);

		matDrawDebug->m_diffuse = Color (.15F, .8F, .15F);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA ( sCrafting.position.x + .33F, sCrafting.position.y + .01F, .03F, .03F);

		matDrawDebug->m_diffuse = Color (.15F, .15F, .8F);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA ( sCrafting.position.x + .37F, sCrafting.position.y + .01F, .03F, .03F);
	}
}

void CPlayerInventoryGUI::DrawCraftingDefault ( void )
{
	CWeaponItem** temp;
	temp = pCraft->GetItems();

	short height = pCraft->GetHeight();
	short width = pCraft->GetWidth();

	const Vector2d slot_offset ( .01, .05+.02 );
	const Vector2d result_offset ( .16, .46+.04 );
	const float highlight_size = 0.0025f;

	//vector<tItemRecipe*> possibleRecipies;
	//pCraft->GetAllMatches( possibleRecipies );
	const std::vector<tItemRecipe*>& possibleRecipies = pCraft->GetAllMatches();
	
	GLd_ACCESS;

	if ( bCraftingMoreOptions || !possibleRecipies.empty() ) 
	{
		matDrawDebug->m_diffuse = Color (0.45f, 0.45f, 0.45f);
		matDrawDebug->bindPass(0);
		GLd.DrawRectangleA( sCrafting.position.x, sCrafting.position.y + sCrafting.size.y, sCrafting.size.x, 0.15f);
	}

	if ( sBag == CRAFT )
	{
		matDrawDebug->m_diffuse = Color (1.0f, .2, .2);
		matDrawDebug->bindPass(0);
		if ( sSlotX >= 0 ) {
			//Draw the highlights for the crafting
			GLd.DrawRectangleA( (width_spacing * sSlotX) + sCrafting.position.x + slot_offset.x - highlight_size, (width_spacing * sSlotY) + sCrafting.position.y + slot_offset.y - highlight_size, width_spacing * .9 + highlight_size*2, width_spacing * .9 + highlight_size*2);
		}
		else if ( sSlotX == -4 ) {
			if ( sSlotY == 0 ) {
				GLd.DrawRectangleA ( sCrafting.position.x + slot_offset.x + 0.02 - highlight_size, sCrafting.position.y + result_offset.y - highlight_size, width_spacing * .9 + highlight_size*2, width_spacing * .9 + highlight_size*2);
			}
			else if ( sSlotY == 1 ) {
				GLd.DrawRectangleA ( sCrafting.position.x + slot_offset.x + result_offset.x*1.77f - highlight_size, sCrafting.position.y + result_offset.y - highlight_size, width_spacing * .9 + highlight_size*2, width_spacing * .9 + highlight_size*2 );
			}
			else if ( sSlotY == 2 ) {
				GLd.DrawRectangleA ( sCrafting.position.x + result_offset.x - highlight_size, sCrafting.position.y + result_offset.y + width_spacing - highlight_size, width_spacing * .9 + highlight_size*2, 0.027f + highlight_size*2);
			}
		}
		else if ( sSlotX == -5 ) {
			if ( sSlotY >= 0 && sSlotY < possibleRecipies.size() ) {
				GLd.DrawRectangleA( sCrafting.position.x + 0.02f + width_spacing*sSlotY- highlight_size, sCrafting.position.y + sCrafting.size.y + 0.04f- highlight_size, width_spacing * .9+ highlight_size*2, width_spacing * .9+ highlight_size*2 );
			}
		}
	}
	else if ( sBag == RESULT )
	{
		// Draw the highlights for the result
		matDrawDebug->m_diffuse = Color (1.0f, .2, .2);
		matDrawDebug->bindPass(0);
		GLd.DrawRectangleA( sCrafting.position.x + result_offset.x - 0.0025, sCrafting.position.y + result_offset.y - 0.0025, width_spacing * .95, width_spacing * .95);
	}

	//Draw the crafting item boxes
	matDrawDebug->m_diffuse = Color (.15f, .15, .15);
	matDrawDebug->bindPass(0);
	// Draw ingredient boxes
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			GLd.DrawRectangleA( (width_spacing * i) + sCrafting.position.x + slot_offset.x, (width_spacing * j) + sCrafting.position.y + slot_offset.y, width_spacing * .9, width_spacing * .9);
		}
	}
	//Draws the results box
	GLd.DrawRectangleA ( sCrafting.position.x + result_offset.x, sCrafting.position.y + result_offset.y, width_spacing * .9, width_spacing * .9);
	//matDrawDebug->unbind();
	if ( bCraftingMoreOptions || !possibleRecipies.empty() ) 
	{
		// Draw the more results box
		for ( int i = 0; i < possibleRecipies.size(); ++i ) {
			GLd.DrawRectangleA( sCrafting.position.x + 0.02f + width_spacing*i, sCrafting.position.y + sCrafting.size.y + 0.04f, width_spacing * .9, width_spacing * .9 );
		}
	}
	// Draw the invent box
	//matDrawDebug->diffuse = Color (.35f, .35, .35);
	//matDrawDebug->bind();
	GLd.DrawRectangleA ( sCrafting.position.x + slot_offset.x + 0.02, sCrafting.position.y + result_offset.y, width_spacing * .9, width_spacing * .9);
	//matDrawDebug->unbind();
	// Draw the logbook box
	//matDrawDebug->diffuse = Color (.35f, .35, .35);
	//matDrawDebug->bind();
	GLd.DrawRectangleA ( sCrafting.position.x + slot_offset.x + result_offset.x*1.77f, sCrafting.position.y + result_offset.y, width_spacing * .9, width_spacing * .9);
	//matDrawDebug->unbind();
	// Draw the more box
	GLd.DrawRectangleA ( sCrafting.position.x + result_offset.x, sCrafting.position.y + result_offset.y + width_spacing, width_spacing * .9, 0.027f);

	//Draw the icons
	matFntDebug->m_diffuse = Color (1.0f, 1.0, 1.0);
	//matFntDebug->bind();
	//Draw them for the ingredients
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if ((temp[i + j * width]) != NULL)
			{
				// Get item icon
				CTexture* icon = (temp[i + j * width])->GetInventoryIcon();

				matFntDebug->setTexture( 0, icon );
				matFntDebug->bindPass(0);
				// Draw the item icon
				GLd.DrawRectangleA( (width_spacing * i) + sCrafting.position.x + slot_offset.x, (width_spacing * j) + sCrafting.position.y + slot_offset.y, width_spacing * .9, width_spacing * .9);
			}
		}
	}
	//Draw the result icon
	if (wResult != NULL)
	{
		CTexture* icon = wResult->GetInventoryIcon();
		matFntDebug->setTexture( 0, icon );
		matFntDebug->bindPass(0);
		// Draw item
		GLd.DrawRectangleA(sCrafting.position.x + result_offset.x, sCrafting.position.y + result_offset.y, width_spacing * .9, width_spacing * .9);
	}

	//Draws the basic item info for the crafting
	matFntDebug->setTexture( 0, fntDebug );
	matFntDebug->m_diffuse = cDrawItemFont;
	matFntDebug->bindPass(0);
	//Draw the info for the ingredients
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if ((temp[i + j * width]) != NULL)
			{
				//string cheese = (temp[i + j * width])->GetItemName();
				//GLd.DrawAutoTextWrapped ( (width_spacing * i) + sCrafting.position.x + .01, (width_spacing * j) + sCrafting.position.y + .07, width_spacing, cheese.c_str() );	

				if ((temp[i + j * width])->GetCanStack())
				{
					short stack = (temp[i + j * width])->GetStackSize();
					GLd.DrawAutoTextWrapped ( (width_spacing * i) + sCrafting.position.x + slot_offset.x, (width_spacing * j) + sCrafting.position.y + slot_offset.y + .04, width_spacing, "%d", stack );
				}
			}
		}
	}
	//Draw the info for the result
	if (wResult != NULL)
	{
		//string cheese = wResult->GetItemName();
		//GLd.DrawAutoTextWrapped (sCrafting.position.x + .185, sCrafting.position.y + .55, width_spacing * .9 + transform.position.y, cheese.c_str());
		if (wResult->GetCanStack())
		{
			short stack = wResult->GetStackSize();
			GLd.DrawAutoTextWrapped ( sCrafting.position.x + result_offset.x + .035, sCrafting.position.y + result_offset.y + 0.11 , width_spacing, "%d", stack );
		}
	}
	// Draw info for the "more results"
	if ( bCraftingMoreOptions || !possibleRecipies.empty() ) 
	{
		// Draw the more results box
		for ( int i = 0; i < possibleRecipies.size(); ++i ) {
			//GLd.DrawRectangleA( sCrafting.position.x + 0.02f + width_spacing*i, sCrafting.position.y + sCrafting.size.y + 0.04f, width_spacing * .9, width_spacing * .9 );
			GLd.DrawAutoText( sCrafting.position.x + 0.02f + width_spacing*i, sCrafting.position.y + sCrafting.size.y + 0.04f, possibleRecipies[i]->name.c_str() );
		}
	}

	// Draw the invent box
	/*matDrawDebug->diffuse = Color (.35f, .35, .35);
	matDrawDebug->bind();
	GLd.DrawRectangleA ( sCrafting.position.x + slot_offset.x + 0.02, sCrafting.position.y + result_offset.y, width_spacing * .9, width_spacing * .9);
	matDrawDebug->unbind();*/

	// Draw the headers
	matFntDebug->setTexture( 0, fntDebug );
	matFntDebug->m_diffuse = Color( 0,0,0,1 );
	matFntDebug->bindPass(0);
	GLd.DrawAutoText( sCrafting.position.x + slot_offset.x + 0.03, sCrafting.position.y + 0.06, "Materials" );
	GLd.DrawAutoText( sCrafting.position.x + result_offset.x, sCrafting.position.y + result_offset.y - 0.01, "Result" );
	GLd.DrawAutoText( sCrafting.position.x + 0.03, sCrafting.position.y + result_offset.y + width_spacing + 0.01, "Invent" );
	GLd.DrawAutoText( sCrafting.position.x + 0.02 + result_offset.x*1.77f, sCrafting.position.y + result_offset.y + width_spacing + 0.01, "Recipies" );
	GLd.DrawAutoText( sCrafting.position.x + 0.02, sCrafting.position.y + 0.02, "Crafting" );

	// Draw hovering text
	/*if ( sSlotX == -4 ) {
		matFntDebug->diffuse = Color( 1.0f,0.2f,0.2f,1.0f );
		matFntDebug->bind();
		fntDebug->Set();
		if ( sSlotY == 0 ) {
			GLd.DrawAutoText( sCrafting.position.x + 0.03, sCrafting.position.y + result_offset.y + width_spacing + 0.01, "Attemp" );
		}
		else if ( sSlotY == 1 ) {

		}
		else if ( sSlotY == 2 ) {

		}
		fntDebug->Unbind();
		matFntDebug->unbind();
	}*/

}

void CPlayerInventoryGUI::DrawCraftingForge ( void )
{
	// Draw the forge positions

	CWeaponItem** temp;
	temp = pCraft->GetItems();

	GLd_ACCESS;

	// Draw bg extension
	if ( mCraftingState.mForgeMoreOptions ) 
	{
		matDrawDebug->m_diffuse = Color (0.45f, 0.45f, 0.45f);
		matDrawDebug->bindPass(0);
		GLd.DrawRectangleA( sCrafting.position.x, sCrafting.position.y + sCrafting.size.y, sCrafting.size.x, 0.15f);
	}

	// Draw the item ingredient highlights
	if ( sBag == CRAFT && sSlotX == 0 )
	{
		matDrawDebug->m_diffuse = Color (1.0f, .2, .2);
		matDrawDebug->bindPass(0);
		GLd.DrawRectangleA ( sCrafting.position.x + pForgePositions[sSlotY].pos.x - 0.0025, sCrafting.position.y + pForgePositions[sSlotY].pos.y - 0.0025, width_spacing*0.7 + 0.005, width_spacing*0.7 + 0.005 );
	}
	//Draw the crafting item ingredient boxes
	matDrawDebug->m_diffuse = Color (.15f, .15, .15);
	matDrawDebug->bindPass(0);
	for ( int i = 0; i < 7; ++i ) {
		GLd.DrawRectangleA ( sCrafting.position.x + pForgePositions[i].pos.x, sCrafting.position.y + pForgePositions[i].pos.y, width_spacing*0.7, width_spacing*0.7 );
	}

	// Draw the item type selections
	matDrawDebug->m_diffuse = Color (.15, .15, .15);
	matDrawDebug->bindPass(0);
	for ( int i = 0; i < 9; ++i ) {
		GLd.DrawRectangleA ( sCrafting.position.x + sCrafting.size.x*0.08 + sCrafting.size.x*0.093*i, sCrafting.position.y+sCrafting.size.y*0.57,sCrafting.size.x*0.084, sCrafting.size.x*0.084 );
	}

	// Draw the selection backgrounds for the weapon subclasses
	if ( mCraftingState.mForgeMoreOptions ) 
	{
		for ( int i = 0; i < mCraftingState.mForgeCategories.size(); ++i ) {
			matDrawDebug->m_diffuse = Color (.75, .75, .75);
			if ( mCraftingState.mForgeCategories[i].enabled ) {
				//glColor3f(.75,.75,.75);
				matDrawDebug->m_diffuse = matDrawDebug->m_diffuse* Color(.75,.75,.75);
			}
			else {
				//glColor3f(.6,.6,.6);
				matDrawDebug->m_diffuse = matDrawDebug->m_diffuse* Color(.6,.6,.6);
			}
			matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( sCrafting.position.x + sCrafting.size.x*0.05, sCrafting.position.y+sCrafting.size.y + 0.01 +0.025*i, sCrafting.size.x*0.9, 0.022 );
			if ( sSlotX == -4 && sSlotY == i ) {
				//glColor3f(1,1,1);
				matDrawDebug->m_diffuse = Color (.75, .75, .75);
				matDrawDebug->bindPass(0);
				GLd.DrawSet2DMode( GLd.D2D_WIRE );
				GLd.DrawRectangleA( sCrafting.position.x + sCrafting.size.x*0.05, sCrafting.position.y+sCrafting.size.y + 0.01 +0.025*i, sCrafting.size.x*0.9, 0.022 );
				GLd.DrawSet2DMode( GLd.D2D_FLAT );
			}
		}
	}

	// Draw the forge button
	matDrawDebug->m_diffuse = Color (.35f, .35, .35);
	matDrawDebug->bindPass(0);
	GLd.DrawRectangleA ( sCrafting.position.x + sCrafting.size.x*0.5 - width_spacing*0.7, sCrafting.position.y + sCrafting.size.y - width_spacing*0.57, width_spacing*1.4, width_spacing * .5);
	if ( sSlotX == -5 ) {
		//glColor3f( 1,1,1 );
		matDrawDebug->m_diffuse = Color (1,1,1);
		matDrawDebug->bindPass(0);
		GLd.DrawSet2DMode( GLd.D2D_WIRE );
		GLd.DrawRectangleA ( sCrafting.position.x + sCrafting.size.x*0.5 - width_spacing*0.7, sCrafting.position.y + sCrafting.size.y - width_spacing*0.57, width_spacing*1.4, width_spacing * .5);
		GLd.DrawSet2DMode( GLd.D2D_FLAT );
	}

	// Draw the component selection area
	matDrawDebug->m_diffuse = Color(.5,.3,.3 );
	matDrawDebug->bindPass(0);
	for ( uint i = 0; i < 4; ++i ) {
		GLd.DrawRectangleA( sCrafting.position.x + sCrafting.size.x*0.07 + sCrafting.size.x*.22*i, sCrafting.position.y + sCrafting.size.y*0.64, sCrafting.size.x*.2, sCrafting.size.x*.4 );
	}
	//glColor3f( 1.0,.2,0.2 );
	matDrawDebug->m_diffuse = Color(1.0,.2,0.2 );
	matDrawDebug->bindPass(0);
	for ( uint i = 0; i < 4; ++i ) {
		if ( sSlotX == -6 && sSlotY == i && !mCraftingState.mForgeParts[i].empty() ) {
			GLd.DrawSet2DMode( GLd.D2D_WIRE );
			GLd.DrawRectangleA ( sCrafting.position.x + sCrafting.size.x*0.07 + sCrafting.size.x*.22*i, sCrafting.position.y + sCrafting.size.y*0.64, sCrafting.size.x*.2, sCrafting.size.x*.4 );
			GLd.DrawSet2DMode( GLd.D2D_FLAT );
		}
	}

	//Draw the icons
	matFntDebug->m_diffuse = Color (1.0f, 1.0, 1.0);
	//Draw them for the ingredients
	for ( int i = 0; i < 7; ++i )
	{
		if ( temp[i] != NULL )
		{
			CTexture* icon = temp[i]->GetInventoryIcon();
			matFntDebug->setTexture(0,icon);
			matFntDebug->bindPass(0);
			GLd.DrawRectangleA( sCrafting.position.x + pForgePositions[i].pos.x, sCrafting.position.y + pForgePositions[i].pos.y, width_spacing*0.7, width_spacing*0.7 );
		}
	}
	//Draw the result icon
	/*if (wResult != NULL)
	{
		CTexture* icon = wResult->GetInventoryIcon();
		icon->Bind();
		// Draw item
		GLd.DrawRectangleA(sCrafting.position.x + result_offset.x, sCrafting.position.y + result_offset.y, width_spacing * .9, width_spacing * .9);
		icon->Unbind();
	}*/

	matFntDebug->setTexture(0,fntDebug);
	matFntDebug->m_diffuse = Color( 0,0,0,1 );
	matFntDebug->bindPass(0);
	//fntDebug->Set();
	// Draw the headers
	GLd.DrawAutoText( sCrafting.position.x + pForgePositions[0].pos.x, sCrafting.position.y + pForgePositions[0].pos.y-0.01, "Base Material" );
	GLd.DrawAutoText( sCrafting.position.x + pForgePositions[1].pos.x, sCrafting.position.y + pForgePositions[1].pos.y-0.01, "Additives" );
	GLd.DrawAutoText( sCrafting.position.x + pForgePositions[5].pos.x - 0.02, sCrafting.position.y + pForgePositions[5].pos.y-0.01, "Enchants" );
	GLd.DrawAutoText( sCrafting.position.x + 0.02, sCrafting.position.y + 0.02, "Forging" );
	// Draw the names for the weapon subclasses
	if ( mCraftingState.mForgeMoreOptions ) {
		for ( int i = 0; i < mCraftingState.mForgeCategories.size(); ++i ) {
			if ( mCraftingState.mForgeCategories[i].enabled ) {
				//glColor3f(0,0,0);
				matFntDebug->m_diffuse = Color( 0,0,0,1 );
			}
			else {
				//glColor3f(0.3f,0.3f,0.3f);
				matFntDebug->m_diffuse = Color( 0.3f,0.3f,0.3f,1 );
			}
			matFntDebug->bindPass(0);
			GLd.DrawAutoText( sCrafting.position.x + sCrafting.size.x*0.05 + 0.02, sCrafting.position.y+sCrafting.size.y + 0.026 + 0.025*i, mCraftingState.mForgeCategories[i].name.c_str() );
		}
	}
	// Draw the names for the parts
	for ( int i = 0; i < 4; ++i ) {
		//GLd.DrawRectangleA( sCrafting.position.x + sCrafting.size.x*0.07 + sCrafting.size.x*.22*i, sCrafting.position.y + sCrafting.size.y*0.64, sCrafting.size.x*.2, sCrafting.size.x*.4 );
		if ( !mCraftingState.mForgeParts[i].empty() ) {
			//glColor3f(0,0,0);
			matFntDebug->m_diffuse = Color( 0,0,0,1 );
			matFntDebug->bindPass(0);
			GLd.DrawAutoText(
				sCrafting.position.x + sCrafting.size.x*(0.07+0.01) + sCrafting.size.x*.22*i,
				sCrafting.position.y + sCrafting.size.y*0.64 + (i*.07+0.12)*sCrafting.size.x, 
				mCraftingState.mForgeParts[i][mCraftingState.mForgePartSelect[i]].componentIdentifier );
		}
		//glColor3f(0.3f,0.2f,0.2f);
		matFntDebug->m_diffuse = Color( 0.3f,0.2f,0.2f,1 );
		matFntDebug->bindPass(0);
		GLd.DrawAutoText(
			sCrafting.position.x + sCrafting.size.x*(0.07+0.01) + sCrafting.size.x*.22*i,
			sCrafting.position.y + sCrafting.size.y*(0.64+0.024),
			"%d/%d", mCraftingState.mForgePartSelect[i]+(mCraftingState.mForgeParts[i].empty()?0:1), mCraftingState.mForgeParts[i].size() );
	}
	// Draw the forge button text
	matFntDebug->m_diffuse = Color( 1,1,1,1 );
	matFntDebug->bindPass(0);
	//glColor3f(1,1,1);
	GLd.DrawAutoTextCentered( sCrafting.position.x + sCrafting.size.x*0.5, sCrafting.position.y + sCrafting.size.y - width_spacing * 0.3, "FORGE" );
	//fntDebug->Unbind();
	//matFntDebug->unbind();
}

//This function makes the calls to the crafting to make an item
CWeaponItem* CPlayerInventoryGUI::DoCrafting (void)
{
	if (pCraft->HasLevelAndForge())
	{
		//Make the item
		pCraft->MakeItem();
		//Set its ownership so that it won't disappear
		wResult->SetOwner(pInventory[0]->GetOwner());
		//Set its holdstate so it doesn't suddenly appear
		wResult->SetHoldState(Item::Hidden);
		return wResult;
	}
	else return NULL;
}

// Updates the crafting
// Specifically, will instantiate the item in the crafting for icon renderering
// Does visual message fade out
void CPlayerInventoryGUI::UpdateCrafting (void)
{
	// Crafting
	if ( sCrafting.substate == 0 )
	{
		//Find if there is a match for between a recipe and the ingredients
		tItemRecipe::item_entry result = pCraft->GetResult();
		static short previous = 0;

		//If there is no result or a new result has been found AND there is a match for a recipe with the current ingredients
		if ((wResult == NULL || result.id != previous) && result.id > 0)
		{
			//Get rid of the previous result if there was one
			if ( wResult != NULL ) delete wResult;
			//if (result > 0 && result != previous) {
			//Create an instance of the result and mark that it was made. That makes sure the item isn't remade every frame
			if (result.id == 2)
			{
				wResult = new ItemTerraBlok ( NULL, result.subid );
				wResult->SetStackSize (result.stack);
				previous = result.id;
			}
			else
			{
				wResult = CWeaponItem::Instantiate (result.id);
				wResult->SetStackSize (result.stack);
				previous = result.id;
			}
			//}
		}
		//If there was no match between the ingredients and the recipes
		else if (result.id == 0)
		{
			//If there was a result, get rid of it
			delete_safe(wResult);
		}
	}
	// Message fade outs
	if ( mCraftingState.sMouseMessageTimer > 0 ) {
		mCraftingState.sMouseMessageTimer -= Time::deltaTime;
	}
	else {
		if ( mCraftingState.sMouseMessageAlpha > 0 ) {
			mCraftingState.sMouseMessageAlpha -= Time::deltaTime * 3.0f;
		}
		else {
			mCraftingState.sMouseMessageAlpha = 0;
		}
	}
}

void CPlayerInventoryGUI::EmptyCrafting (void)
{
	CWeaponItem** temp;
	temp = pCraft->GetItems();
	short cWidth = pCraft->GetWidth();
	short cHeight = pCraft->GetHeight();

	for (int i = 0; i < cHeight; i++)
	{
		for (int j = 0; j < cWidth; j++)
		{
			if (temp[j + i * cWidth] != NULL)
			{
				if ( !pInventory[0]->AddItem(temp[j + i * cWidth]) ) {
					temp[j + i*cWidth]->transform.position = pPlayer->transform.position + Vector3d( 0,0,2 );
					temp[j + i*cWidth]->transform.SetDirty();
					temp[j + i*cWidth]->SetOwner( NULL );
					temp[j + i*cWidth]->SetHoldState( Item::None );
				}
				temp[j + i * cWidth] = NULL;
			}
		}
	}
}


// Mouse control for crafting (put here since it varies state to state)
void CPlayerInventoryGUI::GetCraftingMouseInfo ( void )
{
	//Get the heights and widths of the inventory and crafting
	short iwidth = (pInventory[0])->GetWidth();
	short cheight = pCraft->GetHeight();
	short cwidth = pCraft->GetWidth();

	//Calculate the width spacing for nice square calculations
	//float width_spacing = .4 / iwidth;
	// Get mouse positions
	float x = sMouseX;
	float y = sMouseY;

	// Do clicking info
	if ( sCrafting.substate == 0 )
	{
		const Vector2d slot_offset ( .01, .05+.02 );
		const Vector2d result_offset ( .16, .46+.04 );

		// Get clicky info for the crafting table
		for (int i = 0; i < cwidth; i++)
		{
			for (int j = 0; j < cheight; j++)
			{
				if ((x >= (width_spacing * i) + sCrafting.position.x + slot_offset.x && x <= (width_spacing * i) + (width_spacing * .9) + sCrafting.position.x + slot_offset.x) 
					&& (y >= (width_spacing * j) + sCrafting.position.y + slot_offset.y && y <= (width_spacing * j) + (width_spacing * .9) + sCrafting.position.y + slot_offset.y))
				{
					sBag = CRAFT;
					sSlotX = i;
					sSlotY = j;
				}
			}
		}
		//Get clicky info for the crafting result
		if ((x >= sCrafting.position.x + result_offset.x && x <= sCrafting.position.x + result_offset.x + width_spacing * .9 ) 
			&& (y >= sCrafting.position.y + result_offset.y && y <= sCrafting.position.y + result_offset.y + width_spacing * .9) )
		{
			sBag = RESULT;
			sSlotX = -1;
			sSlotY = -1;
		}
		// Get click info for the invent and recipe button
		Rect rect;
		rect.pos = Vector2d( sCrafting.position.x + slot_offset.x + 0.02, sCrafting.position.y + result_offset.y );
		rect.size = Vector2d( width_spacing * .9, width_spacing * .9 );
		if ( rect.Contains(Vector2d(x,y)) ) {
			sBag = CRAFT;
			sSlotX = -4;
			sSlotY = 0;
		}
		rect.pos = Vector2d( sCrafting.position.x + slot_offset.x + result_offset.x*1.77f, sCrafting.position.y + result_offset.y );
		rect.size = Vector2d( width_spacing * .9, width_spacing * .9 );
		if ( rect.Contains(Vector2d(x,y)) ) {
			sBag = CRAFT;
			sSlotX = -4;
			sSlotY = 1;
		}
		rect.pos = Vector2d( sCrafting.position.x + result_offset.x, sCrafting.position.y + result_offset.y + width_spacing );
		rect.size = Vector2d( width_spacing * .9, 0.027f );
		if ( rect.Contains(Vector2d(x,y)) ) {
			sBag = CRAFT;
			sSlotX = -4;
			sSlotY = 2;
		}
		// Get click info for the more results area
		const std::vector<tItemRecipe*>& possibleRecipies = pCraft->GetAllMatches();
		if ( bCraftingMoreOptions || !possibleRecipies.empty() ) 
		{
			for ( int i = 0; i < possibleRecipies.size(); ++i ) {
				rect.pos = Vector2d(sCrafting.position.x + 0.02f + width_spacing*i, sCrafting.position.y + sCrafting.size.y + 0.04f);
				rect.size = Vector2d(width_spacing * .9, width_spacing * .9);
				if ( rect.Contains(Vector2d(x,y)) ) {
					sBag = CRAFT;
					sSlotX = -5;
					sSlotY = i;
				}
			}
		}
		
	}
	else if ( sCrafting.substate == 1 )
	{
		// Get mouse info for the forge UI
		for ( int i = 0; i < 7; ++i )
		{
			//if ( x >= sCrafting.position.x + pForgePositions[i].pos.x && x <= sCrafting.position.x + pForgePositions[i].pos.x 
			Rect collisionBox;
			collisionBox.pos = sCrafting.position + pForgePositions[i].pos;
			collisionBox.size = Vector2d(width_spacing,width_spacing)*0.7;
			if ( collisionBox.Contains(Vector2d(x,y)) )
			{
				sBag = CRAFT;
				sSlotX = 0;
				sSlotY = i;
			}
		}
		// Get mouse info for the category selectors
		if ( wPicked == NULL ) {
			for ( int i = 0; i < 9; ++i )
			{
				Rect collisionBox (
					sCrafting.position.x + sCrafting.size.x*0.08 + sCrafting.size.x*0.093*i,
					sCrafting.position.y+sCrafting.size.y*0.57,sCrafting
					.size.x*0.084, sCrafting.size.x*0.084 );
				if ( collisionBox.Contains(Vector2d(x,y)) )
				{
					sBag = CRAFT;
					sSlotX = -3;
					sSlotY = i;

					// Print the category types
					mCraftingState.sMouseMessageTimer = 0.5f;
					mCraftingState.sMouseMessageAlpha = 1.0f;
					switch ( i ) {
					case 0: mCraftingState.sMouseMessage = "Blades"; break;
					case 1: mCraftingState.sMouseMessage = "Axes"; break;
					case 2: mCraftingState.sMouseMessage = "Diggers"; break;
					case 3: mCraftingState.sMouseMessage = "Polearms"; break;
					case 4: mCraftingState.sMouseMessage = "Bashers"; break;
					case 5: mCraftingState.sMouseMessage = "Bow"; break;
					case 6: mCraftingState.sMouseMessage = "Crossbow"; break;
					case 7: mCraftingState.sMouseMessage = "???"; break; // flintlocks
					case 8: mCraftingState.sMouseMessage = "????"; break; //magitech
					}
				}
			}
		} // End category selectors
		// Get mouse info for the subcategory selectors
		if ( wPicked == NULL ) {
			for ( int i = 0; i < mCraftingState.mForgeCategories.size(); ++i )
			{
				Rect collisionBox (
					sCrafting.position.x + sCrafting.size.x*0.05,
					sCrafting.position.y+sCrafting.size.y + 0.01 +0.025*i,
					sCrafting.size.x*0.9, 0.022 );
				if ( collisionBox.Contains(Vector2d(x,y)) )
				{
					sBag = CRAFT;
					sSlotX = -4;
					sSlotY = i;
				}
			}
		} // End subcategory selectors
		// Get mouse info for forge button
		if ( wPicked == NULL ) {
			Rect collisionBox (
				sCrafting.position.x + sCrafting.size.x*0.5 - width_spacing*0.7,
				sCrafting.position.y + sCrafting.size.y - width_spacing*0.57,
				width_spacing*1.4, width_spacing * .5);
			if ( collisionBox.Contains(Vector2d(x,y)) )
			{
				sBag = CRAFT;
				sSlotX = -5;
				sSlotY = 0;
			}
		}
		// Get mouse info for component selectors
		if ( wPicked == NULL ) {
			for ( int i = 0; i < 4; ++i )
			{
				Rect collisionBox (
					sCrafting.position.x + sCrafting.size.x*0.07 + sCrafting.size.x*.22*i,
					sCrafting.position.y + sCrafting.size.y*0.64,
					sCrafting.size.x*.2, sCrafting.size.x*.4 );
				if ( collisionBox.Contains(Vector2d(x,y)) )
				{
					sBag = CRAFT;
					sSlotX = -6;
					sSlotY = i;
				}
			}
		} // End component selectors
		//
	}

	//Get clicky info for the crafting tabs (same for all states)
	for (short i = 0; i < 4; i++)
	{
		if ((x >= sCrafting.position.x - .03 && x <= sCrafting.position.x)
			&& (y >= sCrafting.position.y + (.11 * i) && y <= sCrafting.position.y+ (.11 * i) + .1))
		{
			sBag = CRAFT;
			sSlotX = -2;
			sSlotY = i;
		}
	}
}

void CPlayerInventoryGUI::DoCraftingClicky (void)
{
	if ( sSlotX == -2 )
	{
		sCraftType = sSlotY;
		if ( sCraftType != sCrafting.substate ) {
			EmptyCrafting(); // Clear off crafting board when switching between places
		}
		sCrafting.substate = sCraftType;
	}
	else
	{
		// Crafting mouse controls
		if ( sCrafting.substate == 0 )
		{
			short cheight = pCraft->GetHeight();
			short cwidth = pCraft->GetWidth();

			// Crafting mouse
			if ( sSlotX >= 0 )
			{
				wPicked = pCraft->GetItem(sSlotX + sSlotY * cwidth);
				pCraft->ReceiveItem(NULL, sSlotX + sSlotY * cwidth);
			}
			else if ( sSlotX == -4 )
			{
				if ( sSlotY == 0 ) {
					// invent!
				}
				else if ( sSlotY == 1 ) {
					// Show crafting window
					bool value = true;
					pLogbook->SetVisibility(value,true);
					pLogbook->SetTab(1);
				}
				else if ( sSlotY == 2 ) {
					// Toggle the more matches window
					bCraftingMoreOptions = !bCraftingMoreOptions;
				}
			}
			else if ( sSlotX == -5 )
			{
				pCraft->SetRecipeChoice( sSlotY );
			}
		}
		// End craft table mouse controls
		// Forging mouse controls
		if ( sCrafting.substate == 1 )
		{
			if ( sSlotX == 0 ) {
				wPicked = pCraft->GetItem( sSlotY );
				pCraft->ReceiveItem( NULL,sSlotY );
				
				// Item change, recreate partlists
				CreateForgeSubclassList();
				ForgeRefreshPartlist();
				ForgeRefreshLocks();
			}
			else if ( sSlotX == -3 ) {
				switch ( sSlotY ) {
					case 0: mCraftingState.mForgeSearchParameter = WeaponItem::ItemBlade; break;
					case 1: mCraftingState.mForgeSearchParameter = WeaponItem::ItemAxe; break;
					case 2: mCraftingState.mForgeSearchParameter = WeaponItem::ItemDigger; break;
					case 3: mCraftingState.mForgeSearchParameter = WeaponItem::ItemPolearm; break;
					case 4: mCraftingState.mForgeSearchParameter = WeaponItem::ItemBasher; break;
					case 5: mCraftingState.mForgeSearchParameter = WeaponItem::ItemBow; break;
					case 6: mCraftingState.mForgeSearchParameter = WeaponItem::ItemCrossbow; break;
					case 7: mCraftingState.mForgeSearchParameter = WeaponItem::ItemFlintlock; break;
					case 8: mCraftingState.mForgeSearchParameter = WeaponItem::ItemMagitechGun; break;
				}
				mCraftingState.mForgeMoreOptions = true;
				CreateForgeSubclassList();
				ForgeRefreshLocks();
			}
			else if ( sSlotX == -4 ) {
				// Refresh part list only when choose a new item subtype
				if ( mCraftingState.mForgeCategories[sSlotY].enabled ) {
					if ( mCraftingState.mForgeItemType != mCraftingState.mForgeCategories[sSlotY].type ) {
						mCraftingState.mForgeItemType = mCraftingState.mForgeCategories[sSlotY].type;
						ForgeRefreshPartlist();
						ForgeRefreshLocks();
					}
				}
			}
			else if ( sSlotX == -5 ) {
				// Forge button
				CBaseRandomMelee* pWeapon = NULL;
				switch ( mCraftingState.mForgeItemType ) {
					case ItemType::TypeWeaponSword: pWeapon = new CRWSword();	break;
				}

				pWeapon->part_info.componentMain = mCraftingState.mForgeParts[0][mCraftingState.mForgePartSelect[0]].componentIdentifier;
				if ( !mCraftingState.mForgeParts[1].empty() ) {
					pWeapon->part_info.componentSecondary = mCraftingState.mForgeParts[1][mCraftingState.mForgePartSelect[1]].componentIdentifier;
				}
				if ( !mCraftingState.mForgeParts[2].empty() ) {
					pWeapon->part_info.component3 = mCraftingState.mForgeParts[2][mCraftingState.mForgePartSelect[2]].componentIdentifier;
				}
				if ( !mCraftingState.mForgeParts[3].empty() ) {
					pWeapon->part_info.component4 = mCraftingState.mForgeParts[3][mCraftingState.mForgePartSelect[3]].componentIdentifier;
				}

				pCraft->ForgeApplyMaterials( pWeapon );

				pWeapon->Generate(); // Force cache and load the weapon immediately

				pWeapon->SetOwner( this->pPlayer );
				pWeapon->SetHoldState( Item::Hidden );

				pInventory[0]->AddItem( pWeapon );

				pCraft->ForgeDecrementMaterials();
			}
			else if ( sSlotX == -6 ) {
				// Cycle thru parts
				mCraftingState.mForgePartSelect[sSlotY] += 1;
				if ( mCraftingState.mForgePartSelect[sSlotY] >= mCraftingState.mForgeParts[sSlotY].size() ) {
					mCraftingState.mForgePartSelect[sSlotY] = 0;
				}
			}
			//
		}
		// End forge mouse controls
	}
}
	
void CPlayerInventoryGUI::DoCraftingClickyFull ( void )
{
	// Crafting mouse controls
	if ( sCrafting.substate == 0 )
	{
		short cheight = pCraft->GetHeight();
		short cwidth = pCraft->GetWidth();
		if ( sSlotX >= 0 && sSlotY >= 0 )
		{
			//If the slot is empty, place the item there in the crafting
			if (pCraft->GetItem(sSlotX + sSlotY * cwidth) == NULL)
			{
				pCraft->ReceiveItem(wPicked, sSlotX + sSlotY * cwidth);
				wPicked = NULL;
			}
			//If the slot is full
			else
			{
				//Get a pointer to the slot
				CWeaponItem* temp;
				temp = pCraft->GetItem(sSlotX + sSlotY * cwidth);
			
				//If the items can stack, then stack them
				if (temp->GetID() == wPicked->GetID() && wPicked->GetCanStack())
				{
					wPicked = pCraft->AddStacks(wPicked, sSlotX + sSlotY * cwidth);
				}
				//if they can't stack, then switch which item you are holding
				else
				{
					pCraft->ReceiveItem(wPicked, sSlotX + sSlotY * cwidth);
					wPicked = temp;
					temp = NULL;
				}
			}
		}
	}
	// Forge mouse controls
	else if ( sCrafting.substate == 1 )
	{
		short cheight = pCraft->GetHeight();
		short cwidth = pCraft->GetWidth();
		// Check placement of crafting materials
		if ( sSlotX == 0 && sSlotY >= 0 )
		{
			// Check if we can place the item
			if ( wPicked && (ItemAdditives::ItemToAdditive( wPicked ) != ItemAdditives::None) )
			{
				// If the slot is empty, place the item there in the crafting
				if (pCraft->GetItem(sSlotY) == NULL)
				{
					pCraft->ReceiveItem(wPicked, sSlotY);
					wPicked = NULL;
				}
				// If the slot is full
				else
				{
					//Get a pointer to the slot
					CWeaponItem* temp;
					temp = pCraft->GetItem(sSlotY);
			
					//If the items can stack, then stack them
					if (temp->GetID() == wPicked->GetID() && wPicked->GetCanStack())
					{
						wPicked = pCraft->AddStacks(wPicked, sSlotY);
					}
					//if they can't stack, then switch which item you are holding
					else
					{
						pCraft->ReceiveItem(wPicked, sSlotY);
						wPicked = temp;
						temp = NULL;
					}
				}
				// Refresh forge locks
				ForgeRefreshLocks();
				// Refresh part lists
				ForgeRefreshPartlist();
			}
			else
			{
				// Print that it's not a valid material
				mCraftingState.sMouseMessageTimer = 1.5f;
				mCraftingState.sMouseMessageAlpha = 1.0f;
				mCraftingState.sMouseMessage = "bad material";
			}
		}
	}
}

void CPlayerInventoryGUI::CreateForgeSubclassList ( void )
{
	mCraftingState.mForgeCategories.clear();
	switch ( mCraftingState.mForgeSearchParameter )
	{
	case WeaponItem::ItemBlade:
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Knives",WeaponItem::TypeWeaponKnife) );
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Swords",WeaponItem::TypeWeaponSword) );
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Big Blades",WeaponItem::TypeWeaponLongSword) );
		break;
	case WeaponItem::ItemDigger:
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Shovels",WeaponItem::TypeWeaponShovel) );
		break;
	case WeaponItem::ItemAxe:
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Picks",WeaponItem::TypeWeaponPick) );
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Pickaxes",WeaponItem::TypeWeaponPickaxe) );
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Axes",WeaponItem::TypeWeaponAxe) );
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Big Axes",WeaponItem::TypeWeaponGreatAxe) );
		break;
	case WeaponItem::ItemPolearm:
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Glaives (pole swords)",WeaponItem::TypeWeaponGlaive) );
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Pole axes",WeaponItem::TypeWeaponPoleAxe) );
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Pikes",WeaponItem::TypeWeaponPike) );
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Scythes",WeaponItem::TypeWeaponScythe) );
		break;
	case WeaponItem::ItemBasher:
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Hammers",WeaponItem::TypeWeaponHammer) );
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Warhammers",WeaponItem::TypeWeaponWarHammer) );
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Clubs",WeaponItem::TypeWeaponClub) );
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Chained",WeaponItem::TypeWeaponFlail) );
		break;
	case WeaponItem::ItemBow:

		break;
	case WeaponItem::ItemCrossbow:

		break;
	case WeaponItem::ItemFlintlock:

		break;
	case WeaponItem::ItemMagitechGun:
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Talisman Launchers",WeaponItem::TypeWearSocks) );
		mCraftingState.mForgeCategories.push_back( t_forgegroup_info("Electromagical Launchers",WeaponItem::TypeWearSocks) );
		break;
	}
}

void CPlayerInventoryGUI::ForgeRefreshLocks ( void )
{
	ItemType sourcetype = WeaponItem::TypeDefault;
	if ( pCraft->GetItem(0) ) {
		sourcetype = pCraft->GetItem(0)->GetItemData()->eItemType;
	}

	if ( sourcetype == WeaponItem::TypeDefault ) {
		// Lock everything
		for ( int i = 0; i < mCraftingState.mForgeCategories.size(); ++i ) {
			mCraftingState.mForgeCategories[i].enabled = false;
		}
	}
	else
	{
		// Unlock based on moving reforging
		for ( int i = 0; i < mCraftingState.mForgeCategories.size(); ++i ) {
			bool &enabled = mCraftingState.mForgeCategories[i].enabled;
			ItemType targettype = mCraftingState.mForgeCategories[i].type;
			switch ( sourcetype ) {
				// Sword reforge family
				case WeaponItem::TypeWeaponKnife: enabled = (targettype==WeaponItem::TypeWeaponSword); break;
				case WeaponItem::TypeWeaponSword: enabled = (targettype==WeaponItem::TypeWeaponKnife)||(targettype==WeaponItem::TypeWeaponLongSword)||(targettype==WeaponItem::TypeWeaponGlaive); break;
				case WeaponItem::TypeWeaponLongSword: enabled = (targettype==WeaponItem::TypeWeaponSword); break;
				// Axe reforge family
				case WeaponItem::TypeWeaponPick: enabled = (targettype==WeaponItem::TypeWeaponPickaxe); break;
				case WeaponItem::TypeWeaponPickaxe: enabled = (targettype==WeaponItem::TypeWeaponPick)||(targettype==WeaponItem::TypeWeaponAxe); break;
				case WeaponItem::TypeWeaponAxe: enabled = (targettype==WeaponItem::TypeWeaponPickaxe)||(targettype==WeaponItem::TypeWeaponGreatAxe)||(targettype==WeaponItem::TypeWeaponPoleAxe); break;
				case WeaponItem::TypeWeaponGreatAxe: enabled = (targettype==WeaponItem::TypeWeaponAxe); break;
				// Polearm reforge family
				case WeaponItem::TypeWeaponGlaive: enabled = (targettype==WeaponItem::TypeWeaponSword); break;
				case WeaponItem::TypeWeaponPoleAxe: enabled = (targettype==WeaponItem::TypeWeaponAxe); break;
				//case WeaponItem::TypeWeaponPike: enabled = (targettype==WeaponItem::TypeWeaponKnife); break;
				case WeaponItem::TypeWeaponScythe: enabled = (targettype==WeaponItem::TypeWeaponShovel); break;
				// Basher reforge familily
				case WeaponItem::TypeWeaponHammer: enabled = (targettype==WeaponItem::TypeWeaponWarHammer); break;
				case WeaponItem::TypeWeaponWarHammer: enabled = (targettype==WeaponItem::TypeWeaponHammer); break;
				//case WeaponItem::TypeWeaponClub: enabled = (targettype==WeaponItem::TypeWeaponClub); break;
				//case WeaponItem::TypeWeaponFlail: enabled = (targettype==WeaponItem::TypeWeaponFlail); break;
				// Diggers
				//case WeaponItem::TypeWeaponShovel: enabled = (targettype==WeaponItem::TypeWeaponScythe); break;
			}
			// And can always reforge one's self
			enabled = enabled || (sourcetype==targettype);
		}
	}
	// Now loop through the list of stuff and unlock based on recipies
	std::vector<ItemType> matchingItems = pCraft->ForgeGetTypeMatches();
	for ( int i = 0; i < mCraftingState.mForgeCategories.size(); ++i )
	{
		if ( find( matchingItems.begin(), matchingItems.end(), mCraftingState.mForgeCategories[i].type ) != matchingItems.end() )
		{
			mCraftingState.mForgeCategories[i].enabled = true;
		}
	}
}

// Refresh (recreate) the part list. Will apply either forge or reforge rules based on input.
void CPlayerInventoryGUI::ForgeRefreshPartlist ( void )
{
	// Check the item in the main slot
	/*ItemType sourcetype = WeaponItem::TypeDefault;
	ItemAdditives::eItemAdditive sourceadditive = ItemAdditives::None;
	if ( pCraft->GetItem(0) ) {
		sourcetype = pCraft->GetItem(0)->GetItemData()->eItemType;
		sourceadditive = ItemAdditives::ItemToAdditive( pCraft->GetItem(0) );
	}
	else {
		// Can't build without a base
		// Clear part list
		return;
	}*/

	tForgeInput currentTable;
	currentTable.targetType = mCraftingState.mForgeItemType;
	currentTable.base = pCraft->GetItem(0);
	currentTable.additives[0] = pCraft->GetItem(1);
	currentTable.additives[1] = pCraft->GetItem(2);
	currentTable.additives[2] = pCraft->GetItem(3);
	currentTable.additives[3] = pCraft->GetItem(4);
	currentTable.enchantments[0] = pCraft->GetItem(5);
	currentTable.enchantments[1] = pCraft->GetItem(6);

	// Give library the table of current values
	const std::vector<tForgePart>& forgeResults = pCraft->ForgeFindMatches( currentTable );
	// Clear tables
	/*mCraftingState.mForgeParts0.clear();
	mCraftingState.mForgeParts1.clear();
	mCraftingState.mForgeParts2.clear();
	mCraftingState.mForgeParts3.clear();*/
	for ( int i = 0; i < 4; ++i ) {
		mCraftingState.mForgeParts[i].clear();
	}
	// Populate tables
	for ( auto part = forgeResults.begin(); part != forgeResults.end(); ++part )
	{
		/*switch ( part->partType ) {
			case 0: mCraftingState.mForgeParts0.push_back( *part ); break;
			case 1: mCraftingState.mForgeParts1.push_back( *part ); break;
			case 2: mCraftingState.mForgeParts2.push_back( *part ); break;
			case 3: mCraftingState.mForgeParts3.push_back( *part ); break;
		}*/
		mCraftingState.mForgeParts[part->partType].push_back( *part );
	}
	for ( int i = 0; i < 4; ++i ) {
		if ( mCraftingState.mForgePartSelect[i] >= mCraftingState.mForgeParts[i].size() ) {
			mCraftingState.mForgePartSelect[i] = mCraftingState.mForgeParts[i].size();
		}
	}

	// Need to categorize parts into their types

	// give crafting the forge input. It will return a list of parts.
	
	// Toggle the part adding modes
	/*if ( sourceadditive != ItemAdditives::Composite )
	{
		// FORGING MODE

		// Give the library a forge recipe of the current values to get a list of matching recipies

		// Loop through matching recipies and add parts to list
		//ForgeFindMatches( currentTable );
	}
	else
	{
		// REFORGING MODE
		//ReforgeFindMatches( currentTable );
	}*/

}