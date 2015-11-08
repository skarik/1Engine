
// Include Header
#include "CDuskGUI.h"
// Include Elements
#include "controls/CDuskGUIElement.h"
#include "controls/CDuskGUIPanel.h"
#include "controls/CDuskGUIButton.h"
#include "controls/CDuskGUICheckbox.h"
#include "controls/CDuskGUIFloatfield.h"
#include "controls/CDuskGUILabel.h"
#include "controls/CDuskGUIColorpicker.h"
#include "controls/CDuskGUIParagraph.h"
#include "controls/CDuskGUIDialogue.h"
#include "controls/CDuskGUIDialogue_YN.h"
#include "controls/CDuskGUITextfield.h"
#include "controls/CDuskGUIDropdownList.h"
//#include "CDuskGUIListview.h"
//#include "CDuskGUIPropertyview.h"

#include "renderer/material/glMaterial.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

using namespace std;

// Static variables
glMaterial*	CDuskGUI::matDefDefault	= NULL;
glMaterial*	CDuskGUI::matDefHover	= NULL;
glMaterial*	CDuskGUI::matDefDown	= NULL;
glMaterial*	CDuskGUI::matDefFont	= NULL;
CBitmapFont*	CDuskGUI::fntDefDefault	= NULL;

// Constructor
CDuskGUI::CDuskGUI ( void )
	: CGameBehavior(), CRenderableObject()
{
	// Sys drawing
	renderType = Renderer::V2D;

	// Mat init
	InitializeDefaultMaterials();
	matDefault	= matDefDefault;
	//matHover	= matDefHover;
	//matDown		= matDefDown;
	matFont		= matDefFont;
	fntDefault	= fntDefDefault;
	SetMaterial( matDefault );

	// Set default colors and mat settings
	m_basecolor			= Color( 0.4f,0.4f,0.5f, 0.6f );
	m_basecolor_hover	= Color( 0.5f,0.5f,0.6f, 0.6f );
	m_basecolor_down	= Color( 0.2f,0.2f,0.4f, 0.7f );

	// Handle init
	hCurrentElement = -1;
	hCurrentDialogue = -1;
	hCurrentMouseover = -1;
	hCurrentFocus = -1;

	// Option init
	bInPixelMode = false;

	// Set active GUI
	Dusk::activeGUI = this;
}
// Destructor
CDuskGUI::~CDuskGUI ( void )
{
	// Clear the materials if they're not the default ones
	/*if (( matDefault != NULL )&&( matDefault != matDefDefault ))
		delete matDefault;
	matDefault = NULL;
	if (( matHover != NULL )&&( matHover != matDefHover ))
		delete matHover;
	matHover = NULL;
	if (( matDown != NULL )&&( matDown != matDefDown ))
		delete matDown;
	matDown = NULL;
	if (( matFont != NULL )&&( matFont != matDefFont ))
		delete matFont;
	matFont = NULL;
	if (( fntDefault != NULL )&&( fntDefault != fntDefDefault ))
		delete fntDefault;
	fntDefault = NULL;*/


	// Delete all the buttons and things we have
	Dusk::activeGUI = this;
	for ( unsigned int i = 0; i < vElements.size(); ++i )
	{
		delete (vElements[i]);
		vElements[i] = NULL;
	}

	// Remove this GUI from active state
	//if ( Dusk::activeGUI == this ) {
		Dusk::activeGUI = NULL;
	//}
}

// Initializing default materials
void CDuskGUI::InitializeDefaultMaterials ( void )
{
	if ( matDefDefault != NULL ) {
		return;
	}
	matDefDefault	= new glMaterial ();

	matDefFont		= new glMaterial ();
	//fntDefDefault	= new CBitmapFont ( "Calibri", 16, FW_NORMAL );
	fntDefDefault	= new CBitmapFont ( "HVD_Comic_Serif_Pro.otf", 16, FW_NORMAL );
	//fntDefDefault	= new CBitmapFont ( "monofonto.ttf", 16, FW_NORMAL );

	// Set default colors and mat settings
	m_basecolor			= Color( 0.4f,0.4f,0.5f, 0.6f );
	m_basecolor_hover	= Color( 0.5f,0.5f,0.6f, 0.6f );;
	m_basecolor_down	= Color( 0.2f,0.2f,0.4f, 0.7f );

	matDefDefault->m_diffuse = Color( 1,1,1,1 );
	//matDefDefault->setTexture( 0, new CTexture(".res/textures/hud/dusk_element.png") );
	matDefDefault->setTexture( 0, new CTexture("null") );
	matDefDefault->passinfo.push_back( glPass() );
	matDefDefault->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	matDefDefault->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matDefDefault->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	matDefDefault->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;

	matDefFont->m_diffuse = Color( 1,1,1,1 );
	matDefFont->setTexture( 0, fntDefDefault );
	matDefFont->passinfo.push_back( glPass() );
	matDefFont->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	matDefFont->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matDefFont->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	matDefFont->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
}

// Updating
void CDuskGUI::Update ( void )
{
	// Set pixel + screen parameters + current GUI
	Screen::_screen_info_t prevInfo = Screen::Info;
	if ( !bInPixelMode ) {
		CDuskGUIElement::cursor_pos = Vector2d( CInput::MouseX() / (ftype)Screen::Info.width, CInput::MouseY() / (ftype)Screen::Info.height );
	}
	else {
		CDuskGUIElement::cursor_pos = Vector2d( CInput::MouseX(), CInput::MouseY() );
		Screen::Info.width = 1;
		Screen::Info.height = 1;
		Screen::Info.scale = 1;
	}
	CDuskGUIElement::activeGUI = this;

	// Update focus
	UpdateCurrentMouseover();

	// Handle focus
	if ( CInput::MouseDown(CInput::MBLeft) )
	{
		hCurrentFocus = hCurrentMouseover;
		// Reset focuses
		for ( unsigned int i = 0; i < vElements.size(); ++i ) {
			if ( vElements[i] != NULL ) {
				vElements[i]->hasFocus = false;
			}
		}
		if ( hCurrentFocus >= 0 ) {
			vElements[int(hCurrentFocus)]->hasFocus = true;
		}
	}
	
	if ( hCurrentDialogue == -1 )
	{
		// Iterate through all the components
		for ( unsigned int i = 0; i < vElements.size(); ++i )
		{
			if ( vElements[i] != NULL ) {
				hCurrentElement = Handle(i);
				// Update them
				vElements[i]->Update();
			}
		}

		// Now, if there's focus, and tab is hit, cycle through the elements
		if ( CInput::Keydown( Keys.Tab ) )
		{
			int nextFocus = int(hCurrentFocus);
			if ( hCurrentFocus < 0 ) {
				nextFocus = -1;
			}
			// Cycle to next valid element
			bool keepCycling = true;
			while ( keepCycling )
			{
				nextFocus += 1;
				if ( (unsigned)nextFocus >= vElements.size() ) {
					nextFocus = 0;
				}
				if ( vElements[nextFocus] != NULL )
				{
					if ( vElements[nextFocus]->visible && vElements[nextFocus]->drawn && vElements[nextFocus]->canHaveFocus )
					{
						keepCycling = false;
					}
					if ( hCurrentFocus == nextFocus )
					{
						keepCycling = false;
					}
				}
			}
			// Set current focus
			hCurrentFocus = nextFocus;
			// Reset focuses
			for ( unsigned int i = 0; i < vElements.size(); ++i ) {
				if ( vElements[i] != NULL ) {
					vElements[i]->hasFocus = false;
				}
			}
			if ( hCurrentFocus >= 0 ) { // Set focus
				vElements[int(hCurrentFocus)]->hasFocus = true;
			}
		}
		// End tab cycling
	}
	else
	{
		hCurrentElement = hCurrentDialogue;
		hCurrentFocus = hCurrentDialogue;
		// Reset focuses
		for ( unsigned int i = 0; i < vElements.size(); ++i ) {
			if ( vElements[i] != NULL ) {
				vElements[i]->hasFocus = false;
				vElements[i]->mouseIn = false;
				if ( vElements[i]->m_type == 2 ) {
					((CDuskGUIButton*)(vElements[i]))->isPressed = false;
				}
			}
		}
		if ( hCurrentFocus >= 0 ) {
			vElements[int(hCurrentFocus)]->hasFocus = true;
		}
		// Update only the dialogue box
		vElements[int(hCurrentDialogue)]->Update();
	}
	
	// Restore screen info
	Screen::Info = prevInfo;
}

// Rendering
bool CDuskGUI::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS

	// Set pixel + screen parameters + current GUI
	Screen::_screen_info_t prevInfo = Screen::Info;
	if ( !bInPixelMode ) {
		CDuskGUIElement::cursor_pos = Vector2d( CInput::MouseX() / (ftype)Screen::Info.width, CInput::MouseY() / (ftype)Screen::Info.height );
	}
	else {
		CDuskGUIElement::cursor_pos = Vector2d( CInput::MouseX(), CInput::MouseY() );
		Screen::Info.width = 1;
		Screen::Info.height = 1;
		Screen::Info.scale = 1;
	}
	CDuskGUIElement::activeGUI = this;

	// Set the active font
	fntDefault->Set();

	// Create drawn last list
	bool* pbElementDrawn = new bool [vElements.size()];
	for ( unsigned int i = 0; i < vElements.size(); ++i )
		pbElementDrawn[i] = false;

	// Check for last element drawn
	Handle currentElement = 0;
	// Check for amounts of elements drawn
	int iDrawCount = 0;
	// List for the 'recursive' drawing
	std::list<Handle> drawList;
	drawList.push_back( currentElement );

	// Iterate through all the components
	for ( unsigned int i = 0; i < vElements.size(); ++i ) {
		if ( vElements[i] != NULL ) {
			vElements[i]->drawn = false;
		}
	}
	for ( unsigned int i = 0; i < vElements.size(); ++i )
	{
		if ( vElements[i] == NULL ) 
			continue;
		if ( pbElementDrawn[i] == false ) // If element not been drawn yet
			drawList.push_back( Handle(i) ); // Add it to the draw list
		// Draw all objects queued to draw list
		while ( !drawList.empty() )
		{
			// Get the current element
			currentElement = drawList.front();
			drawList.pop_front();

			// Skip null elements
			if ( vElements[currentElement] == NULL ) {
				continue;
			}

			// If the current element is visible
			if (( vElements[currentElement]->visible )||( hCurrentDialogue == currentElement ))
			{
				// If the parent is real
				if ( vElements[currentElement]->parent >= 0 )
				{
					// And if the parent has not been drawn yet
					if ( pbElementDrawn[vElements[currentElement]->parent] == false )
					{
						// Then postpone drawing, adding parent first
						drawList.push_back( vElements[currentElement]->parent );
						drawList.push_back( currentElement );
					}
					else // If it has been drawn
					{
						if ( pbElementDrawn[currentElement] == false ) // Make sure we were not made to skip
						{
							// Draw the element
							pbElementDrawn[currentElement] = true;
							setSubdrawDefault();
							vElements[currentElement]->Render();
							vElements[currentElement]->drawn = true;
						}
					}
				}
				else // If no parent, then just draw the element
				{
					if ( pbElementDrawn[currentElement] == false )
					{
						// Draw the element
						pbElementDrawn[currentElement] = true;
						setSubdrawDefault();
						vElements[currentElement]->Render();
						vElements[currentElement]->drawn = true;
					}
				}
			}
			// End if visible
			else
			{
				// Notify skip of all child elements
				list<Handle> skipList;
				// Check for last element skip
				Handle currentSkipElement = currentElement;
				skipList.push_back( currentSkipElement );

				while ( !skipList.empty() )
				{
					// Get the current element
					currentSkipElement = skipList.front();
					skipList.pop_front();
					
					// Add all elements with this as the parent to skip
					for ( unsigned int j = 0; j < vElements.size(); ++j )
					{
						if (( vElements[j] != NULL )&&( vElements[j]->parent == currentSkipElement )) {
							skipList.push_back( Handle(j) );
						}
					}

					// Notify skip
					pbElementDrawn[currentSkipElement] = true;
				}
				// End skip list
			}
			// End if not visible
		}
		// End draw queue (while loop)
	}
	// Draw the dialogue that's active
	if ( hCurrentDialogue >= 0 ) {
		if ( vElements[hCurrentDialogue] != NULL ) {
			setSubdrawDefault();
			vElements[hCurrentDialogue]->Render();
			vElements[hCurrentDialogue]->drawn = true;
		}
	}
	// Free the list
	delete [] pbElementDrawn;

	GL.prepareDraw();

	GL.beginOrtho( 0,0, 1,1, -45,45 );
	GLd.DrawSet2DScaleMode();

	matDefault->bindPass(0);
	matDefault->setShaderConstants( this );

	// Draw the solids
	if ( !modelSolidMeshList.empty() ) {
		GLd.BeginPrimitive( GL_TRIANGLES );
		for ( uint i = 0; i < modelSolidMeshList.size(); ++i )
		{
			GLd.P_PushColor( modelSolidMeshList[i].r, modelSolidMeshList[i].g, modelSolidMeshList[i].b, modelSolidMeshList[i].a );
			GLd.P_AddVertex( modelSolidMeshList[i].x, modelSolidMeshList[i].y );
		}
		GLd.EndPrimitive();
	}
	// Draw the lines
	if ( !modelLineMeshList.empty() ) {
		GLd.BeginPrimitive( GL_LINES );
		for ( uint i = 0; i < modelLineMeshList.size(); ++i )
		{
			GLd.P_PushColor( modelLineMeshList[i].r, modelLineMeshList[i].g, modelLineMeshList[i].b, modelLineMeshList[i].a );
			GLd.P_AddVertex( modelLineMeshList[i].x, modelLineMeshList[i].y );
		}
		GLd.EndPrimitive();
	}

	GL.endOrtho();
	GL.beginOrtho();

	// Draw the text
	matFont->setTexture( 0, fntDefault );
	matFont->bindPass(0);
	matFont->setShaderConstants( this );
	if ( !modelTextRequestList.empty() ) {
		for ( uint i = 0; i < modelTextRequestList.size(); ++i )
		{
			if ( modelTextRequestList[i].mode == 0 ) {
				GLd.DrawAutoText( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
			}
			else if ( modelTextRequestList[i].mode == 1 ) {
				GLd.DrawAutoTextCentered( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
			}
			else if ( modelTextRequestList[i].mode == 2 ) {
				GLd.DrawAutoTextWrapped( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].width, modelTextRequestList[i].text.c_str() );
			}
		}
	}

	GL.endOrtho();
	GL.cleanupDraw();

	// Clear draw lists (we're done here)
	modelLineMeshList.clear();
	modelSolidMeshList.clear();
	modelTextRequestList.clear();

	// Restore screen info
	Screen::Info = prevInfo;

	return true;
}

void CDuskGUI::GetChildren ( const CDuskGUI::Handle& parent, vector<CDuskGUI::Handle>& children )
{
	children.clear();

	bool* bpElementChecked = new bool [vElements.size()];
	for ( unsigned int i = 0; i < vElements.size(); ++i )
		bpElementChecked[i] = false;
	std::list<Handle> checklist;

	checklist.push_back( parent );
	while ( !checklist.empty() ) {
		Handle query = checklist.front();
		checklist.pop_front();

		// Check current query for children
		for ( uint i = 0; i < vElements.size(); ++i )
		{
			if ( vElements[i] != NULL ) {
				// If this element's parent is the same as query
				if ( vElements[i]->parent == query ) {
					// If not checked yet, add to children list
					if ( bpElementChecked[i] == false ) {
						children.push_back( i );
						bpElementChecked[i] = true;
					}
					// Add this to also check for children
					checklist.push_back( i );
				}
			}
		}
	}

	// Free temp array
	delete [] bpElementChecked;
}

// Grab handle based on pointer
CDuskGUI::Handle CDuskGUI::GetFromPointer ( const CDuskGUIElement* element )
{
	for ( uint i = 0; i < vElements.size(); ++i ) {
		if ( vElements[i] == element ) {
			return Handle(i);
		}
	}
	return Handle(-1);
}

// == Setters ==
// Set materials
void CDuskGUI::SetDefaultMaterial	( glMaterial* newmat )
{
	matDefault = newmat;
}
void CDuskGUI::SetHoverMaterial	( glMaterial* newmat )
{
	matHover = newmat;
}
void CDuskGUI::SetDownMaterial	( glMaterial* newmat )
{
	matDown = newmat;
}
void CDuskGUI::SetFontMaterial	( glMaterial* newmat )
{
	matFont = newmat;
}
void CDuskGUI::SetDefaultFont	( CBitmapFont* newfnt )
{
	fntDefault = newfnt;
}
// Set GUI properties
void CDuskGUI::SetPixelMode ( bool enabled )
{
	bInPixelMode = enabled;
}

// Set element properties
void CDuskGUI::SetElementVisible ( const Handle & handle, const bool vis )
{
	vElements[int(handle)]->visible = vis;
}
void CDuskGUI::SetElementText ( const Handle & handle, const string & str )
{
	vElements[int(handle)]->label = str;
}
void CDuskGUI::SetElementRect ( const Handle & handle, const Rect & rect )
{
	vElements[int(handle)]->rect = rect;
}
void CDuskGUI::SetElementParent ( const Handle & handle, const Handle & newparent )
{
	vElements[int(handle)]->parent = newparent;
}
void CDuskGUI::ToggleElementVisible ( const Handle & handle )
{
	vElements[int(handle)]->visible = !(vElements[int(handle)]->visible);
}

// == Getters/Observers ==
bool CDuskGUI::GetMouseInGUI ( void )
{
	return (hCurrentMouseover>=0);
}
// Grab states of elements
bool CDuskGUI::GetClicked ( const Handle & handle )
{
	return ( (CInput::MouseDown(CInput::MBLeft)) && (GetMouseOver( handle )) );
}
bool CDuskGUI::GetMouseOver ( const Handle & handle )
{
	return vElements[int(handle)]->mouseIn;
}
// Get button state
bool CDuskGUI::GetButtonClicked ( const Handle & handle )
{
	return ((CDuskGUIButton*)(vElements[int(handle)]))->isPressed;
}
// Get dialogue state
int CDuskGUI::GetDialogueResponse ( const Handle& handle )
{
	int value = ((CDuskGUIDialogue*)(vElements[int(handle)]))->dialogueReturn;
	((CDuskGUIDialogue*)(vElements[int(handle)]))->dialogueReturn = -1;
	return value;
}

// == Creating new elements ==
// Panel used for the backdrop of other elements
CDuskGUI::Handle CDuskGUI::CreatePanel	( const Handle& parent )
{
	vElements.push_back( new CDuskGUIPanel(6) );
	vElements.back()->parent = parent;
	return Handle(vElements.size()-1);
}
// Button used for clicking and stuff
CDuskGUI::Handle CDuskGUI::CreateButton	( const Handle& parent )
{
	vElements.push_back( new CDuskGUIButton() );
	vElements.back()->parent = parent;
	((CDuskGUIButton*)vElements.back())->isPressed = false;
	return Handle(vElements.size()-1);
}
// Checkbox for booleans
CDuskGUI::Handle CDuskGUI::CreateCheckbox ( const Handle& parent, const bool checked )
{
	vElements.push_back( new CDuskGUICheckbox() );
	vElements.back()->parent = parent;
	((CDuskGUICheckbox*)vElements.back())->isPressed = false;
	((CDuskGUICheckbox*)vElements.back())->boolValue = checked;
	((CDuskGUICheckbox*)vElements.back())->lastBoolValue = checked;
	return Handle(vElements.size()-1);
}
// A text-only control
CDuskGUI::Handle CDuskGUI::CreateText	( const Handle& parent, const string& str )
{
	vElements.push_back( new CDuskGUILabel() );
	vElements.back()->parent = parent;
	vElements.back()->label = str;
	return Handle(vElements.size()-1);
}
// Another text-only control
CDuskGUI::Handle CDuskGUI::CreateParagraph	( const Handle& parent, const string& str )
{
	vElements.push_back( new CDuskGUIParagraph() );
	vElements.back()->parent = parent;
	vElements.back()->label = str;
	return Handle(vElements.size()-1);
}

//Handle CDuskGUI::CreateTextfield	( const Handle& =0 )
// A textfield w/ io
CDuskGUI::Handle CDuskGUI::CreateTextfield ( const Handle & parent, const string & defVal )
{
	vElements.push_back( new CDuskGUITextfield() );
	vElements.back()->parent = parent;
	((CDuskGUITextfield*)vElements.back())->label = defVal;

	return Handle(vElements.size()-1);
}
// A color picker w/ io
CDuskGUI::Handle CDuskGUI::CreateColorPicker( const Handle & parent, const Color& color )
{
	vElements.push_back( new CDuskGUIColorpicker() );
	vElements.back()->parent = parent;
	((CDuskGUIColorpicker*)vElements.back())->colorValue = color;
	return Handle(vElements.size()-1);
}
// A dialogue with yes/no buttons
CDuskGUI::Handle CDuskGUI::CreateYesNoDialogue ( const Handle & parent )
{
	vElements.push_back( new CDuskGUIDialogue_YN() );
	vElements.back()->parent = parent;
	((CDuskGUIDialogue_YN*)vElements.back())->inDialogueMode = false;
	return Handle(vElements.size()-1);
}

// Updating elements
void CDuskGUI::UpdateCheckbox ( const Handle & handle, bool & inOutCheckboxVal )
{
	CDuskGUICheckbox* cb = (CDuskGUICheckbox*)vElements[int(handle)];
	if ( cb->lastBoolValue != inOutCheckboxVal )
	{
		cb->boolValue = inOutCheckboxVal;
	}
	else
	{
		inOutCheckboxVal = cb->boolValue;
	}
}
void CDuskGUI::UpdateTextfield ( const Handle & handle, string & inOutTextVal )
{
	CDuskGUITextfield* tf = (CDuskGUITextfield*)vElements[int(handle)];
	if ( tf->lastTextValue != inOutTextVal )
	{
		tf->textValue = inOutTextVal;
		tf->label = inOutTextVal;
	}
	else
	{
		inOutTextVal = tf->textValue;
	}
}
void CDuskGUI::UpdateColorPicker ( const Handle & handle, Color & inOutColorVal )
{
	CDuskGUIColorpicker* cp = (CDuskGUIColorpicker*)vElements[int(handle)];
	/*if (( cp->lastColorValue.red != inOutColorVal.red )
	  ||( cp->lastColorValue.blue != inOutColorVal.blue )
	  ||( cp->lastColorValue.alpha != inOutColorVal.alpha )
	  ||( cp->lastColorValue.green != inOutColorVal.green ))*/
	if (( fabs( cp->lastColorValue.red-inOutColorVal.red ) > 1.0e-5 )
		||( fabs( cp->lastColorValue.blue-inOutColorVal.blue ) > 1.0e-5 )
		||( fabs( cp->lastColorValue.green-inOutColorVal.green ) > 1.0e-5 )
		||( fabs( cp->lastColorValue.alpha-inOutColorVal.alpha ) > 1.0e-5 ))
	{
		//cp->colorValue = inOutColorVal;
		cp->SetColor( inOutColorVal );
		//cout << "external set" << endl;
	}
	else
	{
		inOutColorVal = cp->colorValue;
		//cout << "internal set" << endl;
	}
}
void CDuskGUI::SetColorPicker ( const Handle & handle, Color & inColorVal )
{
	CDuskGUIColorpicker* cp = (CDuskGUIColorpicker*)vElements[int(handle)];
	cp->SetColor( inColorVal );
}
bool CDuskGUI::ColorPickerInDialogue ( const Handle & handle )
{
	CDuskGUIColorpicker* cp = (CDuskGUIColorpicker*)vElements[int(handle)];
	return cp->inDialogueMode;
}


// Show Y/N dialogue
void CDuskGUI::ShowYesNoDialogue ( const Handle & handle )
{
	CDuskGUIDialogue_YN* dg = (CDuskGUIDialogue_YN*)vElements[int(handle)];
	if ( !dg->inDialogueMode )
	{
		dg->inDialogueMode = true;
		dg->dialogueReturn = -1;
	}
}
// Hide dialogue
void CDuskGUI::HideDialogue ( const Handle & handle )
{
	CDuskGUIDialogue* dg = (CDuskGUIDialogue*)vElements[int(handle)];
	if ( dg->inDialogueMode ) {
		dg->inDialogueMode = false;
		dg->dialogueReturn = -1;
	}
}

// Destroying elements
void CDuskGUI::DeleteElement ( const Handle & handleToDelete )
{
	if ( int(handleToDelete) >= int(vElements.size()) ) {
		return;
	}
	if ( vElements[int(handleToDelete)] != NULL )
	{
		delete vElements[int(handleToDelete)];
		vElements[int(handleToDelete)] = NULL;
	}
}

// Update hCurrentMouseOver variable
void CDuskGUI::UpdateCurrentMouseover ( void )
{
	// Reset mouseover
	hCurrentMouseover = -1;

	vector<Handle> mouseoverList;
	vector<Handle> mouseoverListFinal;

	for ( unsigned int i = 0; i < vElements.size(); ++i )
	{
		if ( vElements[i] == NULL )		// Skip deleted
			continue;
		if ( !vElements[i]->visible )	// Skip invisible
			continue;
		if ( !vElements[i]->drawn )		// Skip not drawn
			continue;

		// Add current object into list if mouse is in it
		if ( vElements[i]->mouseIn )
		{
			mouseoverList.push_back( Handle(i) );
			mouseoverListFinal.push_back( Handle(i) );
		}
	}

	// Add in the entire parent tree
	for ( unsigned int i = 0; i < mouseoverList.size(); ++i )
	{
		if ( vElements[(int)mouseoverList[i]]->parent >= 0 )
		{
			// First check that it's not in already
			if ( std::find( mouseoverList.begin(), mouseoverList.end(), vElements[(int)mouseoverList[i]]->parent ) == mouseoverList.end() )
			{
				mouseoverList.push_back( vElements[(int)mouseoverList[i]]->parent );
				mouseoverListFinal.push_back( vElements[(int)mouseoverList[i]]->parent );
			}
		}
	}

	// Loop through list and remove any parents (INCORRECT)
	for ( unsigned int i = 0; i < mouseoverList.size(); ++i )
	{
		for ( unsigned int j = 0; j < mouseoverListFinal.size(); ++j )
		{
			if ( vElements[(int)mouseoverList[i]]->parent == ((int)mouseoverListFinal[j]) )
			{
				mouseoverListFinal.erase( mouseoverListFinal.begin()+j );
				j -= 1;
			}
		}
	}

	if ( mouseoverList.size() > 0 ) {
		hCurrentMouseover = mouseoverListFinal[0];
	}
}