#include "CPlayerInventoryGUI.h"
#include "core/input/CInput.h"
#include "core/system/Screen.h"
#include "after/entities/item/system/ItemTerraBlok.h"
#include "after/entities/item/wearable/CWearableItem.h"
//#include "CPlayerInventory.h"
#include "core-ext/input/CInputControl.h"
#include "engine-common/entities/CPlayer.h"
#include "CPlayerLogbook.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "after/entities/character/CAfterPlayer.h"

CPlayerInventoryGUI::CPlayerInventoryGUI (CAfterPlayer* p_player, CInventory** p_inventory, CRecipeLibrary* p_lib, CCrafting* p_craft, CPlayerLogbook* p_logbook, CInventoryGUI* p_chest )
	: CGameBehavior(), CRenderableObject() 
{
	//Set pointers to the player and the inventory of that player
	pPlayer		= p_player;
	pInventory	= (CPlayerInventory**)(p_inventory);
	pCraft		= p_craft;
	pLogbook	= p_logbook;
	pChest		= p_chest;

	//Initialize the font drawing stuff
	//fntDebug	= new CBitmapFont ( "Calibri", 18, FW_BOLD );
	fntDebug	= new CBitmapFont ( "HVD_Comic_Serif_Pro.otf", 12, FW_BOLD );
	matFntDebug = new glMaterial;
	matFntDebug->m_diffuse = Color( 0.0f,0,0 );
	matFntDebug->setTexture( 0, fntDebug );
	matFntDebug->passinfo.push_back( glPass() );
	matFntDebug->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matFntDebug->passinfo[0].m_transparency_mode= Renderer::ALPHAMODE_TRANSLUCENT;
	matFntDebug->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );
	matFntDebug->passinfo[0].b_depthmask = false;

	//Initialize the other drawing stuff
	matDrawDebug = new glMaterial;
	matDrawDebug->m_diffuse = Color( 0.0f,0,0 );
	matDrawDebug->setTexture( 0, new CTexture( "textures/white.jpg" ) );
	matDrawDebug->passinfo.push_back( glPass() );
	matDrawDebug->passinfo[0].m_lighting_mode		= Renderer::LI_NONE;
	matDrawDebug->passinfo[0].m_transparency_mode	= Renderer::ALPHAMODE_TRANSLUCENT;
	matDrawDebug->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );
	matDrawDebug->passinfo[0].b_depthmask = false;
	
	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	SetMaterial( matDrawDebug );

	sMouseControlStyle = 0;
	sBag			= -6;
	sSlotX			= 0;
	sSlotY			= 0;
	sFadeIn			= 0;
	sSolid			= 0;
	sCurrentBag		= 0;
	sCraftType		= 0;
	fHotbarX		= 0.0;
	fHotbarSpacing	= .013;
	fHotbarWidth	= 0.09;
	fHotbarPadding	= fHotbarWidth * 0.08;
	fHotbarYbottom	= 1-0.02;
	
	//Initialize variables
	//sBeltHeight = Belt->GetHeight(); For later
	wHotbar = (*pInventory)->GetHotbar();
	wPicked = NULL;
	wResult = NULL;

	//Initialize();
	visible = true;
	bDrawInventory = false;
	bDrawHotbar = true;
	bDrawItemName = false;
	bFade = false;
	bWearables = false;
	//bSwitchCraftWear = false;
	bCraftingMoreOptions = false;
	
	pGear = new CWearableItem* [9];
	for (short i = 0; i < 9; i++)
	{
		pGear[i] = NULL;
	}

	//pGearPositions = new CRect [9];
	pGearPositions[CPlayerInventory::GEAR_HEAD]		= Rect(.11, .05, .09, .09);
	pGearPositions[CPlayerInventory::GEAR_NECKLACE]	= Rect(.06, .15, .09, .09);
	pGearPositions[CPlayerInventory::GEAR_SHOULDER]	= Rect(.16, .15, .09, .09);
	pGearPositions[CPlayerInventory::GEAR_CHEST]	= Rect(.01, .25, .09, .09);
	pGearPositions[CPlayerInventory::GEAR_RIGHTHAND]= Rect(.11, .25, .09, .09);
	pGearPositions[CPlayerInventory::GEAR_LEFTHAND]	= Rect(.21, .25, .09, .09);
	pGearPositions[CPlayerInventory::GEAR_LEGS]		= Rect(.11, .35, .09, .09);
	pGearPositions[CPlayerInventory::GEAR_RIGHTFOOT]= Rect(.06, .45, .09, .09);
	pGearPositions[CPlayerInventory::GEAR_LEFTFOOT]	= Rect(.16, .45, .09, .09);

	pForgePositions[0] = Rect( 0.025, 0.07, .09, .09 );
	pForgePositions[1] = Rect( 0.025+0.19, 0.07, .09, .09 );
	pForgePositions[2] = Rect( 0.025+0.16, 0.07+0.08, .09, .09 );
	pForgePositions[3] = Rect( 0.025+0.08, 0.07+0.16, .09, .09 );
	pForgePositions[4] = Rect( 0.025, 0.07+0.19, .09, .09 );
	pForgePositions[5] = Rect( 0.025+0.29, 0.07+0.14, .09, .09 );
	pForgePositions[6] = Rect( 0.025+0.21, 0.07+0.22, .09, .09 );

	transform.position.x = 0.0;
	transform.position.y = 0.0;
	transform.position.z = -2;

	// Set window positions
	sItems.minimized = false;
	sItems.open = true;
	sItems.position = Vector2d (.15, .15);
	sItems.size = Vector2d( .41, .65 );

	sCrafting.minimized = false;
	sCrafting.open = true;
	sCrafting.position = Vector2d (1.16, 0.05);
	sCrafting.size = Vector2d( .41, .65 );

	sEquipment.minimized = false;
	sEquipment.open = true;
	sEquipment.position = Vector2d (.72, .15);
	sEquipment.size = Vector2d( .31, .55 );

	sHotbar.minimized = false;
	sHotbar.open = true;
	sHotbar.position = Vector2d ( .58, .15);
	sHotbar.size = Vector2d( .11, .65 );

	// Create input control system to push on control stack
	input = new CInputControl( this );

	// Set draw constants
	width_spacing = 0.4 / (pInventory[0])->GetWidth();
	cDrawItemFont = Color( 0.8f,0.1f,0.1f, 1.0f );
}

//CPlayerInventoryGUI destructor
CPlayerInventoryGUI::~CPlayerInventoryGUI ( void )
{
	// Free up anything we're using
	delete fntDebug;
	matFntDebug->removeReference();
	delete matFntDebug;
	//matDrawDebug->removeReference(); // Set as main material. So swag.
	//delete matDrawDebug;
	/*if (wHotbar)
		delete [] wHotbar;
	wHotbar = NULL;*/
	//if (pCraft)		// Player still has ownership of crafting.
	//	delete pCraft;
	//pCraft = NULL;
	if (wPicked)
		delete wPicked;
	wPicked = NULL;
	if (wResult)
		delete wResult;
	wResult = NULL;

	delete input;
}

//Set the visibility of the GUI. For other classes
void CPlayerInventoryGUI::SetVisibility ( bool visibility )
{
	if ( bDrawInventory != visibility ) {
		if ( visibility ) {
			input->Capture();
		}
		else {
			input->Release();
		}
		bDrawInventory = visibility;
	}
}

void CPlayerInventoryGUI::SetInventoryVisibility ( bool & visibility )
{
	/*if ( !sItems.dirty_state ) {
		sItems.minimized = !visibility;
		sHotbar.minimized = !visibility;
	}
	else {
		if ( !sItems.minimized != visibility ) {
			sMouseControlStyle = 0;	
		}
		visibility = !sItems.minimized;
		sItems.dirty_state = false;
	}*/
	if ( !sItems.dirty_state ) {
		sItems.open = visibility;
		sHotbar.open = visibility;
	}
	else {
		if ( sItems.open != visibility ) {
			sMouseControlStyle = 0;	
		}
		visibility = sItems.open;
		sItems.dirty_state = false;
	}
}
void CPlayerInventoryGUI::SetCraftingVisibility ( bool & visibility )
{
	if ( !sCrafting.dirty_state ) {
		sCrafting.open = visibility;
	}
	else {
		if ( sCrafting.open != visibility ) {
			sMouseControlStyle = 0;	
		}
		visibility = sCrafting.open;
		sCrafting.dirty_state = false;
	}
}
void CPlayerInventoryGUI::SetEquipmentVisibility ( bool & visibility )
{
	if ( !sEquipment.dirty_state ) {
		sEquipment.open = visibility;
	}
	else {
		if ( sEquipment.open != visibility ) {
			sMouseControlStyle = 0;	
		}
		visibility = sEquipment.open;
		sEquipment.dirty_state = false;
	}
}

//Stuff to do each frame
void CPlayerInventoryGUI::Update (void)
{
	input->Update( this, Time::deltaTime );
	//If the player presses U, switch whether the GUI is drawn or not
	/*if ( CInput::Keydown('U') )
	{
		bDrawInventory = !bDrawInventory;
		if (!bDrawInventory)
			EmptyCrafting();
	}*/
	
	ResolutionUpdate();

	if ( sMouseControlStyle == 0 )
	{
		//If the inventory GUI is being drawn
		if (bDrawInventory)
		{
			//Get the location of the mouse and update the crafting
			GetMouseInfo();
			UpdateCrafting();
			ReceiveArmorList();
			
			if (CInput::Keydown(CKeys::Escape))
			{
				SetVisibility (false);
			}

			//If the player has left clicked
			if (CInput::MouseDown(CInput::MBLeft))
			{
				if (!pChest->ValidClick() || !pChest->GetIsVisible())
				//Call the appropriate clicky stuff based on whether the player has something selected or not
					if (wPicked == NULL) {
						DoClickyEmpty();
					}
					else {
						DoClickyFull();
					}
				MinCloseWindows();
			}
			//If the player has right clicked, do stuff if the player has nothing selected. Otherwise, do nothing
			if (CInput::MouseDown(CInput::MBRight))
			{
				if (!pChest->ValidClick() || !pChest->GetIsVisible())
					if ( !Input::Key( Keys.Control ) ) {
						if (wPicked == NULL) {
							DoRightClicky();
						}	
					}
					else {
					// Do the right click menu
					// (if there's an item underneathe, then do the menu)
					}
			}
			//If the player is holding down the left click on a specific spot, move stuff around
			if (CInput::Mouse(CInput::MBLeft))
			{
				DragWindows();
			}
		}
		//While the inventory GUI is not visible
		else
		{
			// Update mouse info
			GetMouseInfo();
			//If the player has left clicked
			if (CInput::MouseDown(CInput::MBLeft)) {
				// Call do clicky full
				if ( wPicked ) {
					DoClickyFull();
				}
			}

			if ( input->HasCapture() || pPlayer->GetInputControl()->HasCapture() )
			{
				//Switch the equipped item based on what number the player has pressed
				if (CInput::Keydown('1')) (pInventory[0])->ChangeEquippedTo(0);
				if (CInput::Keydown('2')) (pInventory[0])->ChangeEquippedTo(1);
				if (CInput::Keydown('3')) (pInventory[0])->ChangeEquippedTo(2);
				if (CInput::Keydown('4')) (pInventory[0])->ChangeEquippedTo(3);
				if (CInput::Keydown('5')) (pInventory[0])->ChangeEquippedTo(4);
				if (CInput::Keydown('6')) (pInventory[0])->ChangeEquippedTo(5);
			}
		}
	}
	else if ( sMouseControlStyle == 1 ) {
		// Right click menu
	}
	else if ( sMouseControlStyle == 2 ) {
		// Examine button style
	}
}

//Stuff to draw each frame
bool CPlayerInventoryGUI::Render ( const char pass )
{
	GL_ACCESS;

	GL.beginOrtho();

	//Regardless of whether the inventory is drawn, draw the hotbar
	DrawHotbar();
		
	if (bDrawItemName && !bDrawInventory) {
		DrawNameOnSwitch (pInventory[0]->GetCurrentEquipped(pInventory[0]->GetLastHandEquipped()));
	}
		
	//If the inventory is visible, draw everything
	if (bDrawInventory)
	{
		//The order here is rather specific. Things drawn later draw over previous stuff
		//Highlights are drawn first so they appear as a border around the box
		if (sCrafting.open)
		//Crafting and Items drawn now so they are beneath the other stuff
			DrawCrafting();
		if (sEquipment.open)
			DrawWearables();
		DrawMenuHotbar();
		//It doesn't matter in which order the crafting and the inventory are drawn in because they should never overlap
		DrawItems();
		//Draw tooltips here so that they will appear over the items in the inventory, hotbar, or crafting
		DrawTooltips();
	}

	//Draw the picked item here so that it appears over everthing else and remains visible
	DrawPicked();

	GL.endOrtho();

	return true;
}

/*void CPlayerInventoryGUI::Initialize (void)
{
}*/

//Draw the items in the inventory
void CPlayerInventoryGUI::DrawItems (void)
{
	GLd_ACCESS;
	//Declare a lot of stuff, get information, and make some calculations
	CWeaponItem** temp;

	temp = (pInventory[0])->GetItems();
	GLd.DrawSet2DMode( GLd.D2D_FLAT );
	GLd.DrawSet2DScaleMode(GLd.SCALE_HEIGHT);
	short height = (pInventory[0])->GetHeight();
	short width = (pInventory[0])->GetWidth();
	//float height_spacing = .4 / height;
	//float width_spacing = .4 / width;

	if (sItems.open)
	{
		//Draw the background
		matDrawDebug->m_diffuse = Color (.85, .85, .85);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( sItems.position.x, sItems.position.y, sItems.size.x, sItems.size.y);

		//If the mouse is over the inventory, draw the highlight on the right slot
		if (sBag > 0)
		{
			//Draws the highlights for the inventory
			matDrawDebug->m_diffuse = Color (1.0, .2, .2);
			matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( (width_spacing * sSlotX) + sItems.position.x + .006, (width_spacing * sSlotY) + sItems.position.y + .047, width_spacing * .95, width_spacing * .95);
		}
	
		matDrawDebug->m_diffuse = Color (.15, .15, .15);
		matDrawDebug->bindPass(0);
		// Draw the item box stuff
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				if ((temp[i + j * width]) == NULL)
				{
					matDrawDebug->m_diffuse = Color (.10, .10, .10);
					matDrawDebug->bindPass(0);
				}
				else
				{
					matDrawDebug->m_diffuse = Color (.20, .20, .20);
					matDrawDebug->bindPass(0);
				}
				GLd.DrawRectangleA( (width_spacing * i) + sItems.position.x + .01, (width_spacing * j) + sItems.position.y + .05, width_spacing * .9, width_spacing * .9);
			}
		}

		// Draw the item per-slot icons
		matFntDebug->m_diffuse = Color (1.0, 1.0, 1.0);
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
					GLd.DrawRectangleA( (width_spacing * i) + sItems.position.x + .01, (width_spacing * j) + sItems.position.y + .05, width_spacing * .9, width_spacing * .9);
				}
			}
		}

		// Draw the item per-slot attributes
		matFntDebug->setTexture( 0, fntDebug );
		matFntDebug->m_diffuse = cDrawItemFont;
		matFntDebug->bindPass(0);
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				if ((temp[i + j * width]) != NULL)
				{
					CWeaponItem* item = (temp[i + j * width]);
					// Draw the item name
					/*string cheese = item->GetItemName();
					GLd.DrawAutoTextWrapped ( (width_spacing * i) + sItems.position.x + .01, (width_spacing * j) + sItems.position.y + .07, width_spacing, cheese.c_str() );*/

					// Draw the stack size
					if ( item->GetCanStack() )
					{
						short stack = item->GetStackSize();
						GLd.DrawAutoTextWrapped ( (width_spacing * i) + sItems.position.x + .01, (width_spacing * j) + .13 + sItems.position.y, width_spacing, "%d", stack );
					}
				}
			}
		}

		// Draw the item per-slot durabilities
		matFntDebug->m_diffuse = Color (1.0, 1.0, 1.0);
		matFntDebug->bindPass(0);
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				if ((temp[i + j * width]) != NULL)
				{
					CWeaponItem* item = (temp[i + j * width]);
			
					if (item->GetCanDegrade())
					{
						// Draw the item durability
						GLd.DrawRectangleA( (width_spacing * i) + sItems.position.x + .01, (width_spacing * j) + (width_spacing * .8) + sItems.position.y + .05, width_spacing * .9 * (item->GetDurability() / float (item->GetMaxDurability())), width_spacing * .05);
					}
				}
			}
		}
	}
	/*else
	{
		matDrawDebug->diffuse = Color (.85, .85, .85);
		matDrawDebug->bind();
			GLd.DrawRectangleA( sItems.position.x, sItems.position.y, sItems.size.x, .05);
		matDrawDebug->unbind();
	}*/
}

//Draw the hotbar
void CPlayerInventoryGUI::DrawHotbar (void)
{
	GLd_ACCESS;
	//Initialize a ton of stuff
	GLd.DrawSet2DMode( GLd.D2D_FLAT );
	GLd.DrawSet2DScaleMode(GLd.SCALE_HEIGHT);
	short width = 6;
	short current = (pInventory[0])->GetCurrent();
	float hotbar_width = fHotbarWidth*6 + fHotbarPadding*7;
	float box_width = .09;
	float highlight_offset = fHotbarPadding*0.7;
	float yBottom = fHotbarYbottom;
	//float padding = 0.05;
	
	//Draw the grey background for the hotbar
	matDrawDebug->m_diffuse = Color (.35, .35, .35, .65);
	matDrawDebug->bindPass(0);

	GLd.DrawRectangleA (fHotbarX, yBottom - (fHotbarWidth + fHotbarPadding*2), hotbar_width, fHotbarWidth + fHotbarPadding*2);


	//If the inventory is not being drawn
	if ( true ) // Always draw highlight for now
	{
		//Draw the highlight for the equipped hotbar item
		matDrawDebug->m_diffuse = Color (.2, .2, 1.0);
		matDrawDebug->bindPass(0);
		GLd.DrawRectangleA(
			fHotbarX+fHotbarPadding + ((fHotbarWidth+fHotbarPadding) * current) - highlight_offset,
			yBottom - (fHotbarWidth+fHotbarPadding) - highlight_offset,
			fHotbarWidth + highlight_offset*2,
			fHotbarWidth + highlight_offset*2 );
	}
	// Draw the equipped items
	for (short i = 0; i < pInventory[0]->GetHandCount(); ++i )
	{
		short offset = pInventory[0]->GetCurrentEquippedIndex(i);
		if ( offset != -1 ) {
			matDrawDebug->m_diffuse = Color (1.0f, 1.0f, 0.2f);
			matDrawDebug->bindPass(0);
			GLd.DrawRectangleA(
				fHotbarX+fHotbarPadding + ((fHotbarWidth+fHotbarPadding) * offset) - highlight_offset*0.7,
				yBottom - (fHotbarWidth+fHotbarPadding) - highlight_offset*0.7,
				fHotbarWidth + highlight_offset*0.6*2,
				fHotbarWidth + highlight_offset*0.6*2);
		}
	}
	//If the inventory is being drawn or a item is selected, and the mouse is over the hotbar
	if (sBag == HOTBAR && (bDrawInventory || wPicked) && sSlotY == -1)
	{
		//Draw the highlights for the hotbar
		matDrawDebug->m_diffuse = Color (1.0, .2, .2);
		matDrawDebug->bindPass(0);
		GLd.DrawRectangleA(
			fHotbarX+fHotbarPadding + ((fHotbarWidth+fHotbarPadding) * sSlotX) - highlight_offset,
			yBottom - (fHotbarWidth+fHotbarPadding) - highlight_offset,
			fHotbarWidth + highlight_offset*2,
			fHotbarWidth + highlight_offset*2);
	}
	
	matDrawDebug->m_diffuse = Color (.15, .15, .15);
	matDrawDebug->bindPass(0);
	// Draw the item box stuff
	for (int i = 0; i < width; i++)
	{
		if ((wHotbar[i]) == NULL)
		{
			matDrawDebug->m_diffuse = Color (.10, .10, .10);
			matDrawDebug->bindPass(0);
		}
		else
		{
			matDrawDebug->m_diffuse = Color (.20, .20, .20);
			matDrawDebug->bindPass(0);
		}
		GLd.DrawRectangleA(
			fHotbarX+fHotbarPadding + ((fHotbarWidth+fHotbarPadding) * i),
			yBottom - (fHotbarWidth+fHotbarPadding),
			fHotbarWidth,
			fHotbarWidth);
	}

	
	//Draw the icons for the items in the hotbar
	matFntDebug->m_diffuse = Color (1.0, 1.0, 1.0);
	for (int i = 0; i < 6; i++)
	{
		if ((wHotbar[i]) != NULL)
		{
			// Get item icon
			CTexture* icon = (wHotbar[i])->GetInventoryIcon();

			matFntDebug->setTexture( 0, icon );
			matFntDebug->bindPass( 0 );
			// Draw the item icon
			GLd.DrawRectangleA(
				fHotbarX+fHotbarPadding + ((fHotbarWidth+fHotbarPadding) * i),
				yBottom - (fHotbarWidth+fHotbarPadding),
				fHotbarWidth,
				fHotbarWidth);
		}
	}

	//Draw the info for the items in the hotbar
	matFntDebug->setTexture( 0, fntDebug );
	matFntDebug->m_diffuse = cDrawItemFont;
	matFntDebug->bindPass(0);
	for (int i = 0; i < 6; i++)
	{
		if ((wHotbar[i]) != NULL)
		{
			CWeaponItem* item = (wHotbar[i]);
			//Don't draw the item name anymore
			//string cheese = item->GetItemName();
			//GLd.DrawAutoTextWrapped ( (fHotbarWidth * i) + .45, fHotbarWidth + .82, fHotbarWidth * .9, cheese.c_str() );
			
			// Draw the stack size
			if ( item->GetCanStack() )
			{
				short stack = item->GetStackSize();
				//GLd.DrawAutoTextWrapped ( (fHotbarWidth * i) +  fHotbarX + fHotbarSpacing, fHotbarWidth + .88, box_width, "%d", stack );
				GLd.DrawAutoText (
					fHotbarX+fHotbarPadding + ((fHotbarWidth+fHotbarPadding) * i) + fHotbarWidth*0.03,
					yBottom - (fHotbarWidth+fHotbarPadding) + fHotbarWidth*0.18,
					"%d", stack );
			}
		}
	}
	
	// Draw the item per-slot durabilities
	matDrawDebug->m_diffuse = Color (1.0, 1.0, 1.0);
	matDrawDebug->bindPass(0);
	for (int i = 0; i < 6; i++)
	{
		if ((wHotbar[i]) != NULL)
		{
			CWeaponItem* item = wHotbar[i];
			
			if (item->GetCanDegrade())
			{
				// Draw the item durability
				//GLd.DrawRectangleA( (fHotbarWidth * i) + fHotbarX + fHotbarSpacing, fHotbarWidth + .803 + fHotbarWidth * .8, .1 * (item->GetDurability() / float (item->GetMaxDurability())), box_width * .05);
				GLd.DrawRectangleA(
					fHotbarX+fHotbarPadding + ((fHotbarWidth+fHotbarPadding) * i),
					yBottom - (fHotbarWidth+fHotbarPadding) + fHotbarWidth * 0.90,
					fHotbarWidth * (item->GetDurability() / float (item->GetMaxDurability())),
					fHotbarWidth * .05);
			}
		}
	}
}

void CPlayerInventoryGUI::DrawMenuHotbar(void)
{
	GLd_ACCESS;
	//if (sHotbar.minimized == false)
	if ( sHotbar.open )
	{
		matDrawDebug->m_diffuse = Color (.85, .85, .85);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( sHotbar.position.x, sHotbar.position.y, sHotbar.size.x, sHotbar.size.y);

		matDrawDebug->m_diffuse = Color (.15, .8, .15);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA ( sHotbar.position.x + .07, sHotbar.position.y + .01, .03, .03);

		if (sBag == HOTBAR && sSlotY > -1)
			{
				matDrawDebug->m_diffuse = Color (1.0, .2, .2);
				matDrawDebug->bindPass(0);
				GLd.DrawRectangleA( sHotbar.position.x + .006, (.1 * sSlotY) + sHotbar.position.y + .047, .095, .095);
			}

		matDrawDebug->m_diffuse = Color (.15, .15, .15);
		matDrawDebug->bindPass(0);
		for (int i = 0; i < 6; i++)
		{
			if ((wHotbar[i]) == NULL)
			{
				matDrawDebug->m_diffuse = Color (.10, .10, .10);
				matDrawDebug->bindPass(0);
			}
			else
			{
				matDrawDebug->m_diffuse = Color (.20, .20, .20);
				matDrawDebug->bindPass(0);
			}
			GLd.DrawRectangleA( sHotbar.position.x + .01, sHotbar.position.y + (i * .1) + .05, .09, .09);
		}

		//Draw the icons for the items in the hotbar
		matFntDebug->m_diffuse = Color (1.0, 1.0, 1.0);
		for (int i = 0; i < 6; i++)
		{
			if ((wHotbar[i]) != NULL)
			{
				// Get item icon
				CTexture* icon = (wHotbar[i])->GetInventoryIcon();

				matFntDebug->setTexture( 0, icon );
				matFntDebug->bindPass(0);
				// Draw the item icon
				GLd.DrawRectangleA( sHotbar.position.x + .01, sHotbar.position.y + (i * .1) + .05, .09, .09);
			}
		}

		//Draw the info for the items in the hotbar
		matFntDebug->setTexture( 0, fntDebug );
		matFntDebug->m_diffuse = cDrawItemFont;
		matFntDebug->bindPass(0);
		for (int i = 0; i < 6; i++)
		{
			if ((wHotbar[i]) != NULL)
			{
				CWeaponItem* item = (wHotbar[i]);
			
				//string cheese = item->GetItemName();
				//GLd.DrawAutoTextWrapped ( sHotbar.position.x + .015, sHotbar.position.y + (i * .1) + .065, .09, cheese.c_str() );
			
				// Draw the stack size
				if ( item->GetCanStack() )
				{
					short stack = item->GetStackSize();
					GLd.DrawAutoTextWrapped ( sHotbar.position.x + .015, sHotbar.position.y + (i * .1) + .12, .09, "%d", stack );
				}
			}
		}

		// Draw the item per-slot durabilities
		matDrawDebug->m_diffuse = Color (1.0, 1.0, 1.0);
		matDrawDebug->bindPass(0);
		for (int i = 0; i < 6; i++)
		{
			if ((wHotbar[i]) != NULL)
			{
				CWeaponItem* item = wHotbar[i];
			
				if (item->GetCanDegrade())
				{
					// Draw the item durability
					GLd.DrawRectangleA( sHotbar.position.x + .01, sHotbar.position.y + (i * .1) + .13,
						.095 * (item->GetDurability() / float (item->GetMaxDurability())), .005);
				}
			}
		}
	}
	else if ( sHotbar.minimized )
	{
		matDrawDebug->m_diffuse = Color (.85, .85, .85);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( sHotbar.position.x, sHotbar.position.y, sHotbar.size.x, .05);

		matDrawDebug->m_diffuse = Color (.15, .8, .15);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA ( sHotbar.position.x + .07, sHotbar.position.y + .01, .03, .03);
	}
}

void CPlayerInventoryGUI::DrawWearables (void)
{
	if (!sEquipment.minimized)
	{
		GLd_ACCESS;
		//Draw the background
		matDrawDebug->m_diffuse = Color (.85, .85, .85);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( sEquipment.position.x, sEquipment.position.y, .31, .55);

		matDrawDebug->m_diffuse = Color (.15, .8, .15);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA ( sEquipment.position.x + .23, sEquipment.position.y + .01, .03, .03);

		matDrawDebug->m_diffuse = Color (.15, .15, .8);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA ( sEquipment.position.x + .27, sEquipment.position.y + .01, .03, .03);

		//Draw the highlight
		if (sBag == WEAR)
			{
				matDrawDebug->m_diffuse = Color (1.0, .2, .2);
				matDrawDebug->bindPass(0);
				GLd.DrawRectangleA( pGearPositions[sSlotX].pos.x + sEquipment.position.x - .004, pGearPositions[sSlotX].pos.y + sEquipment.position.y - .004,
					pGearPositions[sSlotX].size.x + .01, pGearPositions[sSlotX].size.y + .008);
			}

		matDrawDebug->m_diffuse = Color (.15, .15, .15);
		matDrawDebug->bindPass(0);
		for (int i = 0; i < 9; i++)
		{
			if (pGear[i] == NULL)
			{
				matDrawDebug->m_diffuse = Color (.10, .10, .10);
				matDrawDebug->bindPass(0);
			}
			else
			{
				matDrawDebug->m_diffuse = Color (.20, .20, .20);
				matDrawDebug->bindPass(0);
			}
			GLd.DrawRectangleA( pGearPositions[i].pos.x + sEquipment.position.x, pGearPositions[i].pos.y + sEquipment.position.y, pGearPositions[i].size.x, pGearPositions[i].size.y);
		}

		//Draw the icons
		matFntDebug->m_diffuse = Color (1.0, 1.0, 1.0);
		for (int i = 0; i < 9; i++)
		{
			if (pGear[i] != NULL)
			{
				CTexture* icon = pGear[i]->GetInventoryIcon();
				matFntDebug->setTexture( 0, icon );
				matFntDebug->bindPass(0);
				GLd.DrawRectangleA( pGearPositions[i].pos.x + sEquipment.position.x, pGearPositions[i].pos.y + sEquipment.position.y, pGearPositions[i].size.x, pGearPositions[i].size.y);
			}
		}

		matDrawDebug->m_diffuse = Color (1.0, 1.0, 1.0);
		matDrawDebug->bindPass(0);
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				if (pGear[i] != NULL)
				{
					CWeaponItem* item = pGear[i];
			
					if (item->GetCanDegrade())
					{
						// Draw the item durability
						GLd.DrawRectangleA( pGearPositions[i].pos.x + sEquipment.position.x, pGearPositions[i].pos.y + sEquipment.position.y + .07,
							pGearPositions[i].size.x * (item->GetDurability() / float (item->GetMaxDurability())), .05 );
					}
				}
			}
		}
	}
	else
	{
		GLd_ACCESS;
		matDrawDebug->m_diffuse = Color (.85, .85, .85);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( sEquipment.position.x, sEquipment.position.y, .31, .05);

		matDrawDebug->m_diffuse = Color (.15, .8, .15);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA ( sEquipment.position.x + .23, sEquipment.position.y + .01, .03, .03);

		matDrawDebug->m_diffuse = Color (.15, .15, .8);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA ( sEquipment.position.x + .27, sEquipment.position.y + .01, .03, .03);
	}
}

//Draw the item that was picked on the mouse
void CPlayerInventoryGUI::DrawPicked (void)
{
	if ( wPicked != NULL )
	{
		//Get the location of the mouse
		float x = CInput::MouseX();
		float y = CInput::MouseY();
		//Make some calculations
		//float width_spacing = .4 / (pInventory[0])->GetWidth();
		short screen_height = Screen::Info.height;
		x /= screen_height;
		y /= screen_height;

		GLd_ACCESS;

		//Draw rectangle BG of held item
		matDrawDebug->m_diffuse = Color (.15, .15, .15);
		matDrawDebug->bindPass(0);
		GLd.DrawRectangleA(x, y, width_spacing * .9, width_spacing * .9);

		//Draw the icon to follow the mouse
		matFntDebug->m_diffuse = Color (1.0, 1.0, 1.0);
		{
			CTexture* icon = wPicked->GetInventoryIcon();
			matFntDebug->setTexture( 0, icon );
			matFntDebug->bindPass(0);
			GLd.DrawRectangleA(x, y, width_spacing * .9, width_spacing * .9);
		}

		//Draws the basic item info for the picked item
		matFntDebug->setTexture( 0, fntDebug );
		matFntDebug->m_diffuse = cDrawItemFont;
		matFntDebug->bindPass(0);
		//Draw the info for the picked item
		{
			string cheese = wPicked->GetItemName();
			GLd.DrawAutoText (x, y - .03, cheese.c_str());
		
			if (wPicked->GetCanStack())
			{
				short stack = wPicked->GetStackSize();
				GLd.DrawAutoText ( x, y + .07, "%d", stack );
			}
		}
	}
}
//Draw the tooltips while mousing over
void CPlayerInventoryGUI::DrawTooltips (void)
{
	//Get the heights and widths of the inventory and the crafting
	short iheight = (pInventory[0])->GetHeight();
	short iwidth = (pInventory[0])->GetWidth();
	short cheight = pCraft->GetHeight();
	short cwidth = pCraft->GetWidth();

	//float width_spacing = .6 / iwidth;
	
	//If the mouse is over the inventory
	if (sBag > 0)
	{
		//Update this when multiple bags on one character is implemented
		//Also, this draws the tooltip box, assuming an item is there
		if (pInventory[0]->GetItem (sSlotX + sSlotY * iwidth) != NULL)
		{    
			CWeaponItem::WTooltipStyle gangnam;
			gangnam.fontTexture = fntDebug;
			gangnam.matFont = matFntDebug;
			gangnam.matBg = matDrawDebug;
			matDrawDebug->m_diffuse = Color (0.1f, 0.1f, 0.1f,0.8f);

			CWeaponItem* temp = pInventory[0]->GetItem (sSlotX + sSlotY * iwidth);
			Vector2d position;
			position.x = sMouseX;
			position.y = sMouseY - .2;
			temp->DrawTooltip (position, gangnam);
		}
	}

	//If the mouse is over the crafting
	if (sBag == CRAFT)
	{
		//Draw the tooltip if an item is in the slot
		if ( sCrafting.substate == 0 )
		{
			if (sSlotX >= 0 && sSlotY >= 0 && pCraft->GetItem (sSlotX + sSlotY * cwidth) != NULL)
			{
				CWeaponItem::WTooltipStyle gangnam;
				gangnam.fontTexture = fntDebug;
				gangnam.matFont = matFntDebug;
				gangnam.matBg = matDrawDebug;
				matDrawDebug->m_diffuse = Color (0.1f, 0.1f, 0.1f,0.8f);
	
				CWeaponItem* temp = pCraft->GetItem (sSlotX + sSlotY * cwidth);
				Vector2d position;
				position.x = sMouseX;
				position.y = sMouseY - .2;
				temp->DrawTooltip (position, gangnam);
			}
		}
	}

	//If the mouse is over the hotbar
	if (sBag == HOTBAR)
	{
		//Draw the tooltip if there is an item there 
		if (wHotbar[sSlotX] != NULL)
		{
			CWeaponItem::WTooltipStyle gangnam;
			gangnam.fontTexture = fntDebug;
			gangnam.matFont = matFntDebug;
			gangnam.matBg = matDrawDebug;
			matDrawDebug->m_diffuse = Color (0.1f, 0.1f, 0.1f,0.8f);

			//CWeaponItem* temp = pCraft->GetItem (i + j * cwidth);
			Vector2d position;
			position.x = sMouseX;
			position.y = sMouseY - .2;
			wHotbar[sSlotX]->DrawTooltip (position, gangnam);
		}
	}
}

//This function finds where the mouse is currently and readies the data for use in other functions
void CPlayerInventoryGUI::GetMouseInfo (void)
{
	//Get the heights and widths of the inventory and crafting
	short iheight = (pInventory[0])->GetHeight();
	short iwidth = (pInventory[0])->GetWidth();
	//short cheight = pCraft->GetHeight();
	//short cwidth = pCraft->GetWidth();
	//Calculate the width spacing for nice square calculations
	//float height_spacing = .6 / height;
	//float width_spacing = .4 / iwidth;

	//Get where the mouse actually is from CInput
	float x = CInput::MouseX();
	float y = CInput::MouseY();
	//short screen_width = Screen::Info.width;
	short screen_height = Screen::Info.height;
	//Find where the mouse is relative to the size of the screen. Necessary so that different resolutions can be supported
	x /= screen_height;
	y /= screen_height;
	//Let the rest of the GUI class access the data
	sMouseX = x;
	sMouseY = y;
	
	//change = -1;

	// Don't change mouseover if dragging!
	if (CInput::Mouse(CInput::MBLeft)) {
		if ( sBag == DRAG ) {
			return;
		}
	}
	
	//Reset the variables that mark which slot the mouse is over
	sBag = -6;
	sSlotX = -1;
	sSlotY = -1;

	//Get clicky info for inventory
	if (sItems.open && bDrawInventory)
	{
		for (int i = 0; i < iwidth; i++)
		{
			for (int j = 0; j < iheight; j++)
			{
				if ((x >= (width_spacing * i) + sItems.position.x + .01 && x <= (width_spacing * i) + (width_spacing * .9) + sItems.position.x + .01) 
					&& (y >= (width_spacing * j) + sItems.position.y + .05 && y <= (width_spacing * j) + (width_spacing * .9) + sItems.position.y + .05))
				{
					sBag = 1;
					sSlotX = i;
					sSlotY = j;
				}
			}
		}
	}
	//Get clicky info for crafting
	/*if (sCrafting.open && !sCrafting.minimized)
	{
		for (int i = 0; i < cwidth; i++)
		{
			for (int j = 0; j < cheight; j++)
			{
				if ((x >= (width_spacing * i) + sCrafting.position.x && x <= (width_spacing * i) + (width_spacing * .9) + sCrafting.position.x) 
					&& (y >= (width_spacing * j) + sCrafting.position.y + .05 && y <= (width_spacing * j) + (width_spacing * .9) + sCrafting.position.y + .05))
				{
					sBag = CRAFT;
					sSlotX = i;
					sSlotY = j;
				}
			}
		}
	}*/
	if (sCrafting.open && !sCrafting.minimized && bDrawInventory) {
		GetCraftingMouseInfo();
	}

	//Get clicky info for gear
	if (sEquipment.open && !sEquipment.minimized && bDrawInventory)
	{
		for (int i = 0; i < 9; i++)
		{
			if ((x >= pGearPositions[i].pos.x + sEquipment.position.x && x <= pGearPositions[i].pos.x + pGearPositions[i].size.x + sEquipment.position.x) 
				&& (y >= pGearPositions[i].pos.y + sEquipment.position.y && y <= pGearPositions[i].pos.y + pGearPositions[i].size.y + sEquipment.position.y))
			{
				sBag = WEAR;
				sSlotX = i;
			}
		}
	}
	//Get clicky info for the hotbar. Both of them
	for (int i = 0; i < 6; i++)
	{
		/*if ((x >= (fHotbarWidth * i) + fHotbarX + fHotbarSpacing && x <= (fHotbarWidth * i) + fHotbarX + fHotbarSpacing + .09) && (y >= (width_spacing) + .8 && y <= (width_spacing) + .8 + (width_spacing * .9)))
		{
			sBag = HOTBAR;
			sSlotX = i;
			sSlotY = -1;
		}*/
		if ( (x >= (fHotbarX+fHotbarPadding + (fHotbarWidth+fHotbarPadding)*i))&&(x <= (fHotbarX+fHotbarPadding + (fHotbarWidth+fHotbarPadding)*i + fHotbarWidth))
			&&( y >= (fHotbarYbottom - (fHotbarPadding+fHotbarWidth)) )&&( y <= (fHotbarYbottom - (fHotbarPadding)) ) )
		{
			sBag = HOTBAR;
			sSlotX = i;
			sSlotY = -1;
		}
		if (!sHotbar.minimized && bDrawInventory)
		{
			if ((x >= sHotbar.position.x + .01 && x <= sHotbar.position.x + .1) 
					&& (y >= (width_spacing * i) + sItems.position.y + .05 && y <= (width_spacing * i) + (width_spacing * .9) + sItems.position.y + .05))
			{
				sBag = HOTBAR;
				sSlotX = i;
				sSlotY = i;
			}
		}
	}

	//Get clicky info for the crafting result
	/*if ((x >= sCrafting.position.x + .15 && x <= sCrafting.position.x + .15 + width_spacing * .9 ) 
		&& (y >= sCrafting.position.y + .46 && y <= sCrafting.position.y + .46 + width_spacing * .9) 
		&& sCrafting.open && !sCrafting.minimized)
	{
		sBag = RESULT;
		sSlotX = -1;
		sSlotY = -1;
	}
	*/
	//Get clicky info for the crafting tabs
	/*for (short i = 0; i < 4; i++)
	{//GLd.DrawRectangleA( sCrafting.position.x - .03, sCrafting.position.y + (.11 * i), .03, .1);
		if ((x >= sCrafting.position.x - .03 && x <= sCrafting.position.x)
			&& (y >= sCrafting.position.y + (.11 * i) && y <= sCrafting.position.y+ (.11 * i) + .1))
		{
			sBag = CRAFT;
			sSlotX = -2;
			sSlotY = i;
		}
	}
	*/
	// If not in a slot, and not in result slot, then check for dragging
	if ( sSlotX == -1 && sSlotY == -1 && sBag != RESULT && bDrawInventory )
	{
		//Get clicky info to drag the GUI around
		if ((x >= sItems.position.x && x <= sItems.position.x + sItems.size.x) 
			&& (y >= sItems.position.y && y <= sItems.position.y + .05))
		{
			sBag = DRAG;
			sSlotX = 1;
		}

		//Get clicky info for dragging the crafting around
		else if ((x >= sCrafting.position.x && x <= sCrafting.position.x + sCrafting.size.x) 
			&& (y >= sCrafting.position.y && y <= sCrafting.position.y + .05) && sCrafting.open)
		{
			sBag = DRAG;
			sSlotX = CRAFT;
			if ((x >= sCrafting.position.x + .33 && x <= sCrafting.position.x + .36) 
			&& (y >= sCrafting.position.y + .01 && y <= sCrafting.position.y + .04))
				sSlotY = 1;
			if ((x >= sCrafting.position.x + .37 && x <= sCrafting.position.x + .40) 
			&& (y >= sCrafting.position.y + .01 && y <= sCrafting.position.y + .04))
				sSlotY = 2;
		}

		//Get clicky info for dragging around the equipment
		else if ((x >= sEquipment.position.x && x <= sEquipment.position.x + sEquipment.size.x) 
			&& (y >= sEquipment.position.y && y <= sEquipment.position.y + .05) && sEquipment.open)
		{
			sBag = DRAG;
			sSlotX = WEAR;
			if ((x >= sEquipment.position.x + .23 && x <= sEquipment.position.x + .26) 
			&& (y >= sEquipment.position.y + .01 && y <= sEquipment.position.y + .04))
				sSlotY = 1;
			if ((x >= sEquipment.position.x + .27 && x <= sEquipment.position.x + .3) 
			&& (y >= sEquipment.position.y + .01 && y <= sEquipment.position.y + .04))
				sSlotY = 2;
		}
	
		//Get clicky info for dragging around the hotbar
		else if ((x >= sHotbar.position.x && x <= sHotbar.position.x + sHotbar.size.x) 
			&& (y >= sHotbar.position.y && y <= sHotbar.position.y + .05) && sHotbar.open)
		{
			sBag = DRAG;
			sSlotX = HOTBAR;
			if ((x >= sHotbar.position.x + .07 && x <= sHotbar.position.x + .1) 
			&& (y >= sHotbar.position.y +.01 && y <= sHotbar.position.y + .04))
				sSlotY = 1;
		}

		//See if the player has clicked outside of the GUI to drop an item
		/*if (((x < .2 + transform.position.x || x > 1.4 + transform.position.x) 
			|| (y < .15 + transform.position.y || y > .85 + transform.position.y)) && sBag == -6 )
			sBag = DROP;*/
		else
		{
			sBag = DROP;
		}
	}
	else if ( sSlotX == -1 && sSlotY == -1 && sBag != RESULT )
	{
		sBag = DROP;
	}
}

//Take the appropriate actions when the player clicks on the inventory GUI
void CPlayerInventoryGUI::DoClickyEmpty (void)
{
	//Get the heights and widths of the crafting and inventory
	short iheight = (pInventory[0])->GetHeight();
	short iwidth = (pInventory[0])->GetWidth();

	//Calculate width spacing to get nice squares
	//float width_spacing = .6 / iwidth;
	
	//Depending on where the player clicked, pick up the item
	//The mouse information comes from another function
	if (sBag > 0)
	{
		wPicked = (pInventory[0])->GetItem(sSlotX + sSlotY * iwidth);
		(pInventory[0])->ReceiveItem(NULL, sSlotX + sSlotY * iwidth);
	}

	if (sBag == CRAFT)
	{
		DoCraftingClicky();
	}
	if (sBag == RESULT)
	{
		if (wResult != NULL)
		{
			wPicked = DoCrafting();
			wResult = NULL;
		}
	}

	if (sBag == HOTBAR)
	{
		wPicked = wHotbar[sSlotX];
		wHotbar[sSlotX] = NULL;
		(pInventory[0])->RecheckEquipped(sSlotX);
	}
	
	if (sBag == WEAR)
	{
		wPicked = (pInventory[sCurrentBag])->UnequipArmorPiece(sSlotX);
	}

	if (pChest->GetIsVisible())
	{
		pChest->ReceiveSelected (wPicked);
	}
	//if (bSwitchCraftWear)
		//bWearables = !bWearables;
}

//Do the right stuff when the player clicks and has something selected
void CPlayerInventoryGUI::DoClickyFull (void)
{
	//Get the heights and widths of the inventory and crafting and calculate width spacing to get nice squares that scale with the size of the screen
	short iheight = (pInventory[0])->GetHeight();
	short iwidth = (pInventory[0])->GetWidth();
	short cheight = pCraft->GetHeight();
	short cwidth = pCraft->GetWidth();

	//float height_spacing = .6 / height;
	//float width_spacing = .6 / iwidth;

	//Do stuff based on where the player clicked
	//Once again, the data is gathered by another function
	//If the player has clicked on the inventory
	if (sBag > 0 && sItems.open && bDrawInventory)
	{
		//If the slot is empty, place the item there in the inventory
		if ((pInventory[0])->GetItem(sSlotX + sSlotY * iwidth) == NULL)
		{
			(pInventory[0])->ReceiveItem(wPicked, sSlotX + sSlotY * iwidth);
			wPicked = NULL;
		}
		//If the slot is full
		else
		{
			//Get a pointer to the slot
			CWeaponItem* temp;
			temp = (pInventory[0])->GetItem(sSlotX + sSlotY * iwidth);

			//If the items can stack, then stack them
			if ( (temp->GetID() == wPicked->GetID() && wPicked->GetCanStack())
			/*{
				if*/&& ( wPicked->GetID() != 2 || (wPicked->GetID() == 2 && ((ItemTerraBlok*) temp)->GetType() == ((ItemTerraBlok*) wPicked)->GetType()) ) ){
					wPicked = (pInventory[0])->AddStacks(wPicked, sSlotX + sSlotY * iwidth, false);
				//}
			}
			//if they can't stack, then switch which item you are holding
			else
			{
				(pInventory[0])->ReceiveItem(wPicked, sSlotX + sSlotY * iwidth);
				wPicked = temp;
				temp = NULL;
			}
		}
	}

	//If the player has clicked on the crafting
	if (sBag == CRAFT && sCrafting.open && bDrawInventory)
	{
		DoCraftingClickyFull();
	}
	//If the player has picked up the result from the crafting
	if (sBag == RESULT && sCrafting.open && bDrawInventory)
	{
		//If there is something actually there
		if (wResult != NULL)
		{
			//If the result is the same as the selected item, make more of it
			if (wResult->GetID() == wPicked->GetID())
			{
				// And we can actually pick up more of it
				if ( wResult->GetStackSize()+wPicked->GetStackSize() <= wResult->GetMaxStack() )
				{
					pCraft->MakeItem();
					wPicked->AddToStack(wResult->GetStackSize());
					delete wResult;
					wResult = NULL;
				}
			}
		}
	}


	//If the player has clicked on the hotbar
	if (sBag == HOTBAR)
	{
		//If the slot is empty, place the item there in the hotbar
		if (wHotbar[sSlotX] == NULL)
		{
			wHotbar[sSlotX] = wPicked;
			wPicked = NULL;
			pInventory[0]->RecheckEquipped(sSlotX);
		}
		//If the slot is not empty
		else
		{
			//Get a pointer to the item in the slot
			CWeaponItem* temp;
			temp = wHotbar[sSlotX];

			//If they can stack, add the stacks together
			if (temp->GetID() == wPicked->GetID() && wPicked->GetCanStack())
			{
				wPicked = (pInventory[0])->AddStacks(wPicked, sSlotX, true);
			}
			//If they can't stack, switch the items/skills
			else
			{
				wHotbar[sSlotX] = wPicked;
				wPicked = temp;
				temp = NULL;
			}
			pInventory[0]->RecheckEquipped(sSlotX);
		}
	}

	//If the player is trying to equip a piece of armor
	if (sBag == WEAR && sEquipment.open && bDrawInventory)
	{
		if ( wPicked->GetBaseClassName() == "WearableItem" ) 
		{
			wPicked = (pInventory[sCurrentBag])->EquipArmorPiece( dynamic_cast<CWearableItem*>(wPicked), sSlotX);
		}
	}

	//if (bSwitchCraftWear) {
	//	bWearables = !bWearables;
	//}

	if (sBag == DROP)
	{
		pInventory[0]->Drop(wPicked);
		wPicked = NULL;
	}

	if (pChest->GetIsVisible())
	{
		pChest->ReceiveSelected (wPicked);
	}
}

//Do stuff when the player right clicks on the inventory GUI
//It returns a pointer to a new, split stack of items if the player clicked on a stackable item
//Otherwise, it returns null
CWeaponItem* CPlayerInventoryGUI::DoRightClicky (void)
{
	//Get the heights and widths of the inventory and the crafting
	short iheight = (pInventory[0])->GetHeight();
	short iwidth = (pInventory[0])->GetWidth();
	short cheight = pCraft->GetHeight();
	short cwidth = pCraft->GetWidth();
	short stack = 0;
	CWeaponItem* source = NULL;
	CWeaponItem* temp = NULL;

	//Search for where the player has clicked and get the item they clicked on
	switch (sBag)
	{
	case CRAFT:
		if ( sCrafting.substate == 0 ) {
			if (pCraft->GetItem(sSlotX + sSlotY * cwidth) != NULL)
				source = pCraft->GetItem(sSlotX + sSlotY * cwidth);
		}
		else if ( sCrafting.substate == 1 ) {
			if ( sSlotX == 0 ) {
				if (pCraft->GetItem(sSlotY) != NULL)
					source = pCraft->GetItem(sSlotY);
			}
		}
		break;
	case HOTBAR:
		if (wHotbar[sSlotX] != NULL)
			source = wHotbar[sSlotX];
		break;
	default:
		if ((pInventory[0])->GetItem(sSlotX + sSlotY * iwidth) != NULL)
			source = (pInventory[0])->GetItem(sSlotX + sSlotY * iwidth);
		break;
	}
	//If there was nothing, do nothing and break out of the function
	if (source == NULL)
		return source;
	//If there's only one item on the stack, do nothing and break out of the function
	if (source->GetStackSize() == 1)
		return NULL;
	//If there is an item there that can stack, make a copy of it
	temp = source->Dupe();
	stack = source->GetStackSize();
	//If the stack size is odd, leave the one left over on the stack in the inventory and pick up the rest
	if (stack % 2 == 1)
	{
		source->SetStackSize(stack / 2 + 1);
		temp->SetStackSize(stack / 2);
	}
	//If it's even, then just split the stacks evenly
	else
	{
		source->SetStackSize(stack / 2);
		temp->SetStackSize(stack / 2);
	}
	//Make sure to set the owner so that the item won't mysteriously disppear when the items are checked
	temp->SetOwner (source->GetOwner());
	
	wPicked = temp;

	if (pChest->GetIsVisible())
	{
		pChest->ReceiveSelected (wPicked);
	}

	//Return the pointer to the newly created stack
	return temp;
}

void CPlayerInventoryGUI::DragWindows (void)
{
	Vector2d vDeltaPoint = Vector2d( Input::DeltaMouseX()/(ftype)Screen::Info.height, Input::DeltaMouseY()/(ftype)Screen::Info.height );
	if (sBag == DRAG)
	{
		if (sSlotY == -1)
		{
			if (sSlotX == 1)
			{
				sItems.position.x += vDeltaPoint.x; 
				sItems.position.y += vDeltaPoint.y;
				sHotbar.position.x += vDeltaPoint.x; 
				sHotbar.position.y += vDeltaPoint.y;
				UILimitPosition( sItems.position, sItems.size );
				UILimitPosition( sHotbar.position, sHotbar.size );
			}
			if (sSlotX == CRAFT && sCrafting.open)
			{
				sCrafting.position.x += vDeltaPoint.x; 
				sCrafting.position.y += vDeltaPoint.y;
				UILimitPosition( sCrafting.position, sCrafting.size );
			}
			if (sSlotX == WEAR && sEquipment.open)
			{
				sEquipment.position.x += vDeltaPoint.x; 
				sEquipment.position.y += vDeltaPoint.y;
				UILimitPosition( sEquipment.position, sEquipment.size );
			}
			if (sSlotX == HOTBAR && sHotbar.open)
			{
				sHotbar.position.x += vDeltaPoint.x; 
				sHotbar.position.y += vDeltaPoint.y;
				sItems.position.x += vDeltaPoint.x; 
				sItems.position.y += vDeltaPoint.y;
				UILimitPosition( sHotbar.position, sHotbar.size );
				UILimitPosition( sItems.position, sItems.size );
			}
		}
	}
}
void CPlayerInventoryGUI::UILimitPosition ( Vector2d& position, const Vector2d& size )
{
	Vector2d border;
	Vector2d limit;
	limit.x = Screen::Info.width / (ftype)Screen::Info.height;
	limit.y = 1;
	border.y = 0.05f;
	border.x = 0.05f;

	if ( position.x < border.x - size.x ) {
		position.x = border.x - size.x + 0.001f;
	}
	else if ( position.x > limit.x - border.x ) {
		position.x =  limit.x - border.x - 0.001f;
	}

	if ( position.y < border.y - size.y ) {
		position.y = border.y - size.y + 0.001f;
	}
	else if ( position.y > limit.y - border.y ) {
		position.y =  limit.y - border.y - 0.001f;
	}
}


void CPlayerInventoryGUI::MinCloseWindows (void)
{
	if (sBag == DRAG)
	{
		if (sSlotY == 1)
		{
			if (sSlotX == 1 || sSlotX == HOTBAR)
			{
				sItems.minimized = !sItems.minimized;
				sHotbar.minimized = sItems.minimized;
				sItems.dirty_state = true;
			}
			if (sSlotX == CRAFT) {
				sCrafting.minimized = !sCrafting.minimized;
				sCrafting.dirty_state = true;
			}
			if (sSlotX == WEAR) {
				sEquipment.minimized = !sEquipment.minimized;
				sEquipment.dirty_state = true;
			}
		}
		else if (sSlotY == 2)
		{
			if (sSlotX == CRAFT) {
				sCrafting.open = !sCrafting.open;
				sCrafting.dirty_state = true;
			}
			if (sSlotX == WEAR) {
				sEquipment.open = !sEquipment.open;
				sEquipment.dirty_state = true;
			}
		}
	}
}

void CPlayerInventoryGUI::ResolutionUpdate (void)
{
	float width_ratio = float (Screen::Info.width) / float (Screen::Info.height);
	float width_mid = width_ratio / 2;
	fHotbarX = width_mid - (fHotbarWidth*6+fHotbarPadding*7)*0.5f;
}

void CPlayerInventoryGUI::DrawNameOnSwitch (CWeaponItem* equipped)
{
	//cout << "WELCOME!!!" << bFade << endl;
	bool draw = false;

	if (sFadeIn < 1 && bFade == false)
	{
		//Draws the basic item info for the picked item
		matFntDebug->m_diffuse = Color (1.0f, 1.0, 1.0, sFadeIn);
		sFadeIn += Time::deltaTime * 3.0f;
		draw = true;
	}
	else if (sSolid < 1)
	{
		matFntDebug->m_diffuse = Color (1.0f, 1.0, 1.0);
		sSolid += Time::deltaTime;
		bFade = true;
		draw = true;
	}
	else if (sSolid >= 1 && bFade == true)
	{
		//Draws the basic item info for the picked item
		matFntDebug->m_diffuse = Color (1.0f, 1.0, 1.0, sFadeIn);
		sFadeIn -= Time::deltaTime;
		if (sFadeIn <= 0)
		{
			sFadeIn = 0.0;
			sSolid = 0.0;
			bFade = false;
			bDrawItemName = false;
		}
		draw = true;
	}
	if ( draw )
	{
		matFntDebug->setTexture( 0, fntDebug );
		matFntDebug->bindPass(0);
		//Draw the info for the picked item
		if (equipped != NULL)
		{
			GLd_ACCESS;
			string cheese = equipped->GetItemName();
			GLd.DrawAutoText ( 0.05 + fHotbarX, fHotbarYbottom - (fHotbarWidth+fHotbarPadding*3), cheese.c_str());
		
			if (equipped->GetCanStack())
			{
				short stack = equipped->GetStackSize();
				GLd.DrawAutoText ( 0.05 + fHotbarX - 0.03, fHotbarYbottom - (fHotbarWidth+fHotbarPadding*3), "%d", stack );
			}
		}
	}
}

void CPlayerInventoryGUI::ReceiveArmorList (void)
{
	pGear = pInventory[0]->GetArmorList();
}

void CPlayerInventoryGUI::SetDrawName (bool doit)
{
	bDrawItemName = doit;

	sSolid = 0;
	sFadeIn = 0;
	bFade = false;
}

void CPlayerInventoryGUI::ReceiveSkill (CSkill* skill)
{
	if (wPicked == NULL) { // Not carrying anything
		wPicked = skill;
	}
	else if (wPicked->GetID() > 0)	// If a normal item is already held
	{	// Add it to the inventory
		if (pInventory[0]->AddItem(wPicked))
			wPicked = skill; // And only grab the skill if the item fits in the inventory
	}
	else if (wPicked->GetID() < -1) // If a skill is being held
	{	// Delete the skill
		if ( wPicked != skill ) {
			delete wPicked;
			wPicked = skill;
		}
	}
	else {
		std::cout << "Error in Skill/Item ID: " << wPicked->GetID() << std::endl;
	}
}

CWeaponItem* CPlayerInventoryGUI::GetSelected (void)
{
	return wPicked;
}

void CPlayerInventoryGUI::DropSelected (void)
{
	if (wPicked != NULL)
	{
		pInventory[0]->Drop(wPicked);
		wPicked = NULL;
	}
}

bool CPlayerInventoryGUI::GetIsVisible (void)
{
	return bDrawInventory;
}

void CPlayerInventoryGUI::ReceiveSelected (CWeaponItem* selected)
{
	wPicked = selected;
}

bool CPlayerInventoryGUI::ValidClick (void)
{
	if (sSlotX > -1 && sSlotY > -1)
	{
		return true;
	}
	return false;
}