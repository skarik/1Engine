#include "CInventoryGUI.h"
#include "CPlayerLogbook.h"

#include "core/input/CInput.h"
#include "core/system/Screen.h"

#include "core-ext/input/CInputControl.h"

#include "engine-common/entities/CPlayer.h"

#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "after/entities/item/system/ItemTerraBlok.h"
#include "after/entities/item/wearable/CWearableItem.h"
#include "after/entities/character/CAfterPlayer.h"


CInventoryGUI::CInventoryGUI (CAfterPlayer* p_player, CInventory** p_inventory)
	: CGameBehavior(), CRenderableObject() 
{
	//Set pointers to the player and the inventory of that player
	pPlayer		= p_player;
	pInventory	= p_inventory;
	pTryingThisOut = NULL;

	//Initialize the font drawing stuff
	fntDebug	= new CBitmapFont ( "HVD_Comic_Serif_Pro.otf", 12, FW_BOLD );
	matFntDebug = new glMaterial;
	matFntDebug->m_diffuse = Color( 0.0f,0,0 );
	matFntDebug->setTexture( 0, fntDebug );
	matFntDebug->passinfo.push_back( glPass() );
	matFntDebug->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matFntDebug->passinfo[0].m_transparency_mode= Renderer::ALPHAMODE_TRANSLUCENT;
	matFntDebug->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	matFntDebug->passinfo[0].b_depthmask = false;

	//Initialize the other drawing stuff
	matDrawDebug = new glMaterial;
	matDrawDebug->m_diffuse = Color( 0.0f,0,0 );
	matDrawDebug->setTexture( 0, new CTexture("null") );
	matDrawDebug->passinfo.push_back( glPass() );
	matDrawDebug->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matDrawDebug->passinfo[0].m_transparency_mode= Renderer::ALPHAMODE_TRANSLUCENT;
	matDrawDebug->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	matDrawDebug->passinfo[0].b_depthmask = false;

	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	SetMaterial( matDrawDebug );

	sMouseControlStyle = 0;
	sBag			= -6;
	sSlotX			= 0;
	sSlotY			= 0;

	wPicked = NULL;

	//Initialize();
	visible = true;
	bDrawInventory = false;

	transform.position.x = 0.0;
	transform.position.y = 0.0;
	transform.position.z = -2;

	// Set window positions
	sItems.minimized = false;
	sItems.open = true;
	sItems.position = Vector2d (.77, .15);
	sItems.size = Vector2d( .41, .65 );

	// Create input control system to push on control stack
	input = new CInputControl( this );

	// Set draw constants
	width_spacing = 0.4 / (pInventory[0])->GetWidth();
	cDrawItemFont = Color( 0.8f,0.1f,0.1f, 1.0f );
}

//CInventoryGUI destructor
CInventoryGUI::~CInventoryGUI ( void )
{
	// Free up anything we're using
	delete fntDebug;
	matFntDebug->removeReference();
	delete matFntDebug;
	//matDrawDebug->removeReference();
	//delete matDrawDebug;

	if (wPicked)
		delete wPicked;
	wPicked = NULL;

	delete input;
}

//Set the visibility of the GUI. For other classes
void CInventoryGUI::SetVisibility ( bool visibility )
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

//Stuff to do each frame
void CInventoryGUI::Update (void)
{
	input->Update( this, Time::deltaTime );
	
	ResolutionUpdate();

	if ( sMouseControlStyle == 0 )
	{
		//If the inventory GUI is being drawn
		if (bDrawInventory)
		{
			//Get the location of the mouse and update the crafting
			GetMouseInfo();

			//If the player has left clicked
			if (CInput::MouseDown(CInput::MBLeft))
			{
				//Call the appropriate clicky stuff based on whether the player has something selected or not
				if (ValidClick())
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
				if (ValidClick())
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
		}
	}
	else if ( sMouseControlStyle == 1 ) {
		// Right click menu
	}
	else if ( sMouseControlStyle == 2 ) {
		// Examine button style
	}

	if (bDrawInventory)
	{
		if ( CInput::Keydown('E') || CInput::Keydown(Keys.Escape) || CInput::Keydown(Keys.Tab) )
		{
			SetVisibility (false);
			((CAfterPlayer*)(pPlayer))->TurnOffInventoryGUI();
		}
	}
}

//Stuff to draw each frame
bool CInventoryGUI::Render ( const char pass )
{
	GL_ACCESS;
	GL.beginOrtho();
		
	//If the inventory is visible, draw everything
	if (bDrawInventory)
	{
		//It doesn't matter in which order the crafting and the inventory are drawn in because they should never overlap
		DrawItems();
		//Draw tooltips here so that they will appear over the items in the inventory, hotbar, or crafting
		DrawTooltips();
	}

	GL.endOrtho();

	return true;
}

//Draw the items in the inventory
void CInventoryGUI::DrawItems (void)
{
	//Declare a lot of stuff, get information, and make some calculations
	CWeaponItem** temp;
	GLd_ACCESS;

	temp = (pInventory[0])->GetItems();
	GLd.DrawSet2DMode( GLd.D2D_FLAT );
	GLd.DrawSet2DScaleMode(GLd.SCALE_HEIGHT);
	short height = (pInventory[0])->GetHeight();
	short width = (pInventory[0])->GetWidth();

	if (sItems.minimized == false)
	{
		//Draw the background
		matDrawDebug->m_diffuse = Color (.85, .85, .85);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( sItems.position.x, sItems.position.y, sItems.size.x, sItems.size.y);

		//If the mouse is over the inventory, draw the highlight on the right slot
			//Draws the highlights for the inventory
			matDrawDebug->m_diffuse = Color (1.0, .2, .2);
			matDrawDebug->bindPass(0);
			if (sSlotX > -1 && sSlotY > -1) {
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
		matFntDebug->setTexture( 0, fntDebug );

		// Draw the item per-slot attributes
		matFntDebug->m_diffuse = cDrawItemFont;
		matFntDebug->bindPass(0);
		fntDebug->Set();
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				if ((temp[i + j * width]) != NULL)
				{
					CWeaponItem* item = (temp[i + j * width]);

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
	else
	{
		matDrawDebug->m_diffuse = Color (.85, .85, .85);
		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( sItems.position.x, sItems.position.y, sItems.size.x, .05);
	}
}

//Draw the tooltips while mousing over
void CInventoryGUI::DrawTooltips (void)
{
	//Get the heights and widths of the inventory and the crafting
	short iheight = (pInventory[0])->GetHeight();
	short iwidth = (pInventory[0])->GetWidth();

	//float width_spacing = .6 / iwidth;
	
	//If the mouse is over the inventory
	//if (sBag > 0)
	//{
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
		if (temp != NULL)
			temp->DrawTooltip (position, gangnam);
	}
}

//This function finds where the mouse is currently and readies the data for use in other functions
void CInventoryGUI::GetMouseInfo (void)
{
	//Get the heights and widths of the inventory and crafting
	short iheight = (pInventory[0])->GetHeight();
	short iwidth = (pInventory[0])->GetWidth();

	//Get where the mouse actually is from CInput
	float x = CInput::MouseX();
	float y = CInput::MouseY();

	short screen_height = Screen::Info.height;
	//Find where the mouse is relative to the size of the screen. Necessary so that different resolutions can be supported
	x /= screen_height;
	y /= screen_height;
	//Let the rest of the GUI class access the data
	sMouseX = x;
	sMouseY = y;

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
	if (!sItems.minimized)
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
	//Get clicky info to drag the GUI around
	else if ((x >= sItems.position.x && x <= sItems.position.x + sItems.size.x) 
		&& (y >= sItems.position.y && y <= sItems.position.y + .05))
	{
		sBag = DRAG;
		sSlotX = 1;
	}
	else {
		sBag = DROP;
	}
	//cout << sBag << ": " << sSlotX << ", " << sSlotY << endl;
}

//Take the appropriate actions when the player clicks on the inventory GUI
void CInventoryGUI::DoClickyEmpty (void)
{
	//Get the heights and widths of the crafting and inventory
	short iheight = (pInventory[0])->GetHeight();
	short iwidth = (pInventory[0])->GetWidth();
	
	//Depending on where the player clicked, pick up the item
	//The mouse information comes from another function
	wPicked = (pInventory[0])->GetItem(sSlotX + sSlotY * iwidth);
	(pInventory[0])->ReceiveItem(NULL, sSlotX + sSlotY * iwidth);
	
	((CAfterPlayer*)(pPlayer))->PassSelected(wPicked);

}

//Do the right stuff when the player clicks and has something selected
void CInventoryGUI::DoClickyFull (void)
{
	//Get the heights and widths of the inventory and crafting and calculate width spacing to get nice squares that scale with the size of the screen
	short iheight = (pInventory[0])->GetHeight();
	short iwidth = (pInventory[0])->GetWidth();
	
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
				wPicked = (pInventory[0])->AddStacks(wPicked, sSlotX + sSlotY * iwidth);//, false);
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
	((CAfterPlayer*)(pPlayer))->PassSelected(wPicked);
}

//Do stuff when the player right clicks on the inventory GUI
//It returns a pointer to a new, split stack of items if the player clicked on a stackable item
//Otherwise, it returns null
CWeaponItem* CInventoryGUI::DoRightClicky (void)
{
	//Get the heights and widths of the inventory and the crafting
	short iheight = (pInventory[0])->GetHeight();
	short iwidth = (pInventory[0])->GetWidth();
	//short cheight = pCraft->GetHeight();
	//short cwidth = pCraft->GetWidth();
	short stack = 0;
	CWeaponItem* source = NULL;
	CWeaponItem* temp = NULL;

	//Search for where the player has clicked and get the item they clicked on
	if ((pInventory[0])->GetItem(sSlotX + sSlotY * iwidth) != NULL)
		source = (pInventory[0])->GetItem(sSlotX + sSlotY * iwidth);
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
	((CAfterPlayer*)(pPlayer))->PassSelected(wPicked);
	//Return the pointer to the newly created stack
	return temp;
}

void CInventoryGUI::DragWindows (void)
{
	Vector2d vDeltaPoint = Vector2d( Input::DeltaMouseX()/(ftype)Screen::Info.height, Input::DeltaMouseY()/(ftype)Screen::Info.height );
	if (sBag == DRAG && sSlotX == 1)
	{
		sItems.position.x += vDeltaPoint.x; 
		sItems.position.y += vDeltaPoint.y;
	}

	UILimitPosition( sItems.position, sItems.size );
}

void CInventoryGUI::UILimitPosition ( Vector2d& position, const Vector2d& size )
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


void CInventoryGUI::MinCloseWindows (void)
{
	if (sBag == DRAG && sSlotY == 10)
	{
		sItems.minimized = !sItems.minimized;
		sItems.dirty_state = true;
	}
}

void CInventoryGUI::ResolutionUpdate (void)
{
	float width_ratio = float (Screen::Info.width) / float (Screen::Info.height);
	float width_mid = width_ratio / 2;
}

CWeaponItem* CInventoryGUI::GetSelected (void)
{
	return wPicked;
}

void CInventoryGUI::DropSelected (void)
{
	if (wPicked != NULL)
	{
		pInventory[0]->Drop(wPicked);
		wPicked = NULL;
	}
}

void CInventoryGUI::OpenInventory (CInventory* new_inventory)
{
	pTryingThisOut = new_inventory;
	pInventory = &pTryingThisOut;
}

bool CInventoryGUI::GetIsVisible (void)
{
	return bDrawInventory;
}

void CInventoryGUI::ReceiveSelected (CWeaponItem* selected)
{
	wPicked = selected;
	//cout << wPicked << endl;
}

bool CInventoryGUI::ValidClick (void)
{
	if (sSlotX > -1 && sSlotY > -1)
	{
		return true;
	}
	return false;
}