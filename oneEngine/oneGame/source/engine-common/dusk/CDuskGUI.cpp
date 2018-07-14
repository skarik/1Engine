#include "core/input/CInput.h"

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

// Renderer bits
#include "renderer/material/RrMaterial.h"
#include "renderer/texture/RrFontTexture.h"
#include "renderer/texture/RrRenderTexture.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

// Main header
#include "CDuskGUI.h"

using namespace std;

// Constructor
CDuskGUI::CDuskGUI ( RrFontTexture* font )
	: CGameBehavior(), CRenderableObject()
{
	// Sys drawing
	renderType = renderer::kRLV2D;

	// Mat init
	fntDefault = font;
	InitializeDefaultMaterials();

	// Set default colors and mat settings
	/*m_basecolor			= Color( 0.25f,0.25f,0.35f, 0.7f );
	m_basecolor_hover	= Color( 0.50f,0.50f,0.60f, 0.7f );
	m_basecolor_down	= Color( 0.05f,0.05f,0.15f, 0.8f );*/
	m_basecolor			= Color( 0.30F,0.30F,0.75F, 0.70F );
	m_basecolor_hover	= Color( 0.60F,0.60F,1.00F, 0.80F );
	m_basecolor_down	= Color( 0.05F,0.05F,0.15F, 0.90F );

	// Handle init
	hCurrentElement = -1;
	hCurrentDialogue = -1;
	hCurrentMouseover = -1;
	hCurrentFocus = -1;

	// Option init
	bInPixelMode = true; // This should always work

	// Set active GUI
	Dusk::activeGUI = this;

	// Null out renderbuffer for now
	renderBuffer = NULL;
	
	// Create the copy material
	{
		matScreenCopy = new RrMaterial ();
		matScreenCopy->m_diffuse = Color( 1,1,1,1 );
		matScreenCopy->passinfo.push_back( RrPassForward() );
		matScreenCopy->passinfo[0].shader = new RrShader( "shaders/sys/copy_buffer.glsl" );
		matScreenCopy->passinfo[0].set2DCommon();
		matScreenCopy->passinfo[0].m_lighting_mode = renderer::LI_NONE;
		matScreenCopy->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
		matScreenCopy->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	}
	SetMaterial( matScreenCopy );
	matScreenCopy->removeReference();
}
// Destructor
CDuskGUI::~CDuskGUI ( void )
{
	matDefault->removeReference();

	// Delete all the buttons and things we have
	Dusk::activeGUI = this;
	for ( unsigned int i = 0; i < vElements.size(); ++i )
	{
		delete (vElements[i]);
		vElements[i] = NULL;
	}

	// Remove this GUI from active state
	Dusk::activeGUI = NULL;
}

// Initializing default materials
void CDuskGUI::InitializeDefaultMaterials ( void )
{
	if (fntDefault == NULL)
	{
		fntDefault	= new RrFontTexture ( "ComicNeue-Bold.ttf", 16, FW_NORMAL );
	}
	if (matDefault == NULL)
	{
		matDefault = new RrMaterial;
		matDefault->m_diffuse = Color( 1,1,1,1 );
		matDefault->setTexture( TEX_MAIN, fntDefault );
		matDefault->passinfo.push_back( RrPassForward() );
		matDefault->passinfo[0].shader = new RrShader( "shaders/v2d/duskui_default.glsl" );
		matDefault->passinfo[0].set2DCommon();
		matDefault->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	}
}

// Updating
void CDuskGUI::Update ( void )
{
	// Skip if there's nothing to do
	if ( vElements.empty() ) return;

	// Delete objects that need to be deleted
	{
		std::vector<int> delete_list;
		for ( unsigned int i = 0; i < vElements.size(); ++i )
		{
			if ( vElements[i] != NULL && vElements[i]->delete_me )
			{
				delete_list.push_back(i);
			}
		}
		for ( unsigned int i = 0; i < delete_list.size(); ++i )
		{
			DeleteElement(delete_list[i]);
		}
	}

	// Set pixel + screen parameters + current GUI
	Screen::_screen_info_t prevInfo = Screen::Info;
	if ( !bInPixelMode ) {
		CDuskGUIElement::cursor_pos = Vector2d( CInput::MouseX() / (Real)Screen::Info.width, CInput::MouseY() / (Real)Screen::Info.height );
	}
	else {
		CDuskGUIElement::cursor_pos = Vector2d( CInput::MouseX(), CInput::MouseY() );
		/*Screen::Info.width = 1;
		Screen::Info.height = 1;
		Screen::Info.scale = 1;*/
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
		// Reset offset
		parenting_offset = Vector2d(0,0);
		offsetList.resize( vElements.size(), Vector2d(0,0) );

		// Iterate through all the components
		for ( unsigned int i = 0; i < vElements.size(); ++i )
		{
			if ( vElements[i] != NULL ) {
				// Save offset
				parenting_offset = offsetList[i] - vElements[i]->rect.pos;
				// Update them
				hCurrentElement = Handle(i);
				vElements[i]->Update();
			}
		}

		// Now, if there's focus, and tab is hit, cycle through the elements
		if ( CInput::Keydown( Keys.Tab ) )
		{
			int repeatCount = 0;
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
					// Limit infinite loops
					if ( repeatCount++ > 2 ) {
						nextFocus = hCurrentFocus;
						break;
					}
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
		// Reset offset
		parenting_offset = Vector2d(0,0);
		// Fix the focus
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
		parenting_offset = offsetList[hCurrentDialogue] - vElements[hCurrentDialogue]->rect.pos;
		vElements[int(hCurrentDialogue)]->Update();
	}
	
	// Restore screen info
	Screen::Info = prevInfo;

	// Now render the UI!
	RenderUI();
}

//void CDuskGUI::Render_SetupMaterial ( RrMaterial* mat )
//{
//	if ( mat != NULL )
//	{
//		for ( RrPassForward& pass : mat->passinfo )
//		{
//			pass.b_depthmask = false;
//			pass.m_lighting_mode = renderer::LI_NONE;
//		}
//	}
//}

// Re-render UI to buffer
void CDuskGUI::RenderUI ( void )
{
	GL_ACCESS GLd_ACCESS;

	// Update the buffer to render to
	if ( renderBuffer )
	{
		if ( renderBuffer->GetSize() != Vector2i( Screen::Info.width, Screen::Info.height ) )
		{
			delete renderBuffer;
			renderBuffer = NULL;
		}
	}
	if ( renderBuffer == NULL )
	{
		renderBuffer = new RrRenderTexture( Screen::Info.width, Screen::Info.height, Clamp, Clamp, RGBA8, DepthNone, StencilNone );
	}
	// Update material options
	/*Render_SetupMaterial(matDefault);
	Render_SetupMaterial(matHover);
	Render_SetupMaterial(matDown);
	Render_SetupMaterial(matFont);*/
	
	// Create temp renderer
	rrTextBuilder2D builder (fntDefault, 100);
	m_builder = &builder;

	// Set pixel + screen parameters + current GUI
	Screen::_screen_info_t prevInfo = Screen::Info;
	if ( !bInPixelMode )
	{
		throw core::DeprecatedCallException();
		CDuskGUIElement::cursor_pos = Vector2d( CInput::MouseX() / (Real)Screen::Info.width, CInput::MouseY() / (Real)Screen::Info.height );
	}
	else
	{
		CDuskGUIElement::cursor_pos = Vector2d( CInput::MouseX(), CInput::MouseY() );
	}
	CDuskGUIElement::activeGUI = this;

	// Set the active font
	fntDefault->Set();

	// Create drawn last list
	bool* t_element_handled = new bool [vElements.size()];
	Vector2d* t_element_reference_position = new Vector2d [vElements.size()];

	// Check for last element drawn
	Handle currentElement = 0;
	// Check for amounts of elements drawn
	int iDrawCount = 0;
	// List for the 'recursive' drawing
	std::list<Handle> drawList;
	std::vector<Handle> t_finalDrawList;

	// Reset offset
	parenting_offset = Vector2d(0,0);
	offsetList.resize( vElements.size(), Vector2d(0,0) );
	offsetList.assign( vElements.size(), Vector2d(0,0) );

	// Create base update rect
	Rect update_rect;
	if ( forceUpdateRects.empty() ) 
	{
		update_rect = Rect( Input::MouseX(), Input::MouseY(), 0, 0 );
	}
	else
	{
		update_rect = forceUpdateRects[0];
		for ( Rect rect : forceUpdateRects)
		{
			update_rect.Expand(rect);
		}
		forceUpdateRects.clear();
	}

	// Iterate through all the components to figure out which to update
	drawList.clear();
	drawList.push_back( currentElement );
	memset( t_element_handled, 0, sizeof(bool) * vElements.size() );
	for ( CDuskGUIElement* element : vElements ) {
		if ( element != NULL ) element->drawn = false;
	}
	for ( unsigned int i = 0; i < vElements.size(); ++i )
	{
		if ( vElements[i] == NULL ) 
			continue;
		if ( t_element_handled[i] == false ) // If element not been drawn yet
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
			if ( vElements[currentElement]->visible || hCurrentDialogue == currentElement )
			{
				// Skip already handled objects
				if (t_element_handled[currentElement]) {
					continue;
				}

				Handle parent = vElements[currentElement]->parent;

				// If we have a parent and the parent has rendered
				if ( (parent >= 0 && t_element_handled[parent]) || (parent < 0) )
				{
					// Create new offset if parent exists
					if ( parent >= 0 )
						offsetList[currentElement] += vElements[currentElement]->parent->rect.pos;
					// Save offsets
					parenting_offset = offsetList[currentElement];

					// "Draw" the element
					t_element_handled[currentElement] = true;
					t_element_reference_position[currentElement] = parenting_offset;
					vElements[currentElement]->last_visible_rect = Rect(
						t_element_reference_position[currentElement] + vElements[currentElement]->rect.pos,
						vElements[currentElement]->rect.size);
					if ( vElements[currentElement]->mouseIn || vElements[currentElement]->hasFocus )
					{
						update_rect.Expand( vElements[currentElement]->last_visible_rect );
					}

					// Push element to the list for drawing
					t_finalDrawList.push_back(currentElement);

					vElements[currentElement]->drawn = true;
					// Save offsets
					offsetList[currentElement] += vElements[currentElement]->rect.pos;
				}
				// If parent exists, but has not been drawn
				else if ( parent >= 0 && t_element_handled[parent] == false )
				{
					// Then postpone drawing, adding parent first
					drawList.push_back( vElements[currentElement]->parent );
					drawList.push_back( currentElement );
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
					t_element_handled[currentSkipElement] = true;
				}
				// End skip list
			}
			// End if not visible
		}
		// End draw queue (while loop)
	}

	// Draw the dialogue that's active
	if ( hCurrentDialogue >= 0 )
	{
		currentElement = hCurrentDialogue;
		if ( vElements[currentElement] != NULL )
		{
			parenting_offset = offsetList[currentElement] - vElements[currentElement]->rect.pos;

			// "Draw" the element
			t_element_handled[currentElement] = true;
			t_element_reference_position[currentElement] = parenting_offset;
			vElements[currentElement]->last_visible_rect = Rect(
				t_element_reference_position[currentElement] + vElements[currentElement]->rect.pos,
				vElements[currentElement]->rect.size);

			if ( vElements[currentElement]->mouseIn || vElements[currentElement]->hasFocus )
			{
				update_rect.Expand( vElements[currentElement]->last_visible_rect );
			}

			// Push element to the list for drawing
			t_finalDrawList.push_back(currentElement);

			vElements[hCurrentDialogue]->drawn = true;
		}
	}

	// Check update rect. Don't need to re-render if there's nothing to update
	if ( update_rect.size.x < 1 && update_rect.size.y < 1 )
	{
		delete [] t_element_handled;
		delete [] t_element_reference_position;
		return;
	}

	// Bind to the render buffer
	renderBuffer->BindBuffer();

	// Scissor viewport to what needs to be updated
	GL.scissorViewport(
		(int)update_rect.pos.x,
		(int)(Screen::Info.height - update_rect.pos.y - update_rect.size.y),
		(int)update_rect.size.x,
		(int)update_rect.size.y );

	// Clear the color of where we're rendering
	GL.clearColor( Color(0.0F, 0.0F, 0.0F, 0.0F) );
	GL.ClearBuffer( GL_COLOR_BUFFER_BIT );

	// Iterate through all the components
	for ( unsigned int i = 0; i < t_finalDrawList.size(); ++i )
	{
		currentElement = t_finalDrawList[i];

		// Skip null elements
		if ( vElements[currentElement] == NULL ) {
			continue;
		}

		parenting_offset = t_element_reference_position[currentElement];
		// Draw the element
		setSubdrawDefault();
		vElements[currentElement]->Render();
	}

	// Free the list
	delete [] t_element_handled;
	delete [] t_element_reference_position;

	// Clear draw lists (we're done here)
	/*modelLineMeshList.clear();
	modelSolidMeshList.clear();
	modelTextRequestList.clear();*/

	// Render
	RrScopedMeshRenderer renderer;
	renderer.render(this, matDefault, 0, builder);

	// Restore screen info
	Screen::Info = prevInfo;

	// Restore scissor
	GL.scissorViewport( 0,0, Screen::Info.width, Screen::Info.height );

	// Unbind the buffer
	renderBuffer->UnbindBuffer();

	// Clear builder
	m_builder = NULL;
}

// Rendering
bool CDuskGUI::PreRender ( void )
{
	matScreenCopy->prepareShaderConstants();
	matDefault->prepareShaderConstants();
	//matDown->prepareShaderConstants(this);
	//matFont->prepareShaderConstants();
	//matHover->prepareShaderConstants(this);
	return true;
}
bool CDuskGUI::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS;

	// Bind the material for alpha-blending
	matScreenCopy->setTexture( TEX_MAIN, renderBuffer );
	matScreenCopy->bindPass( pass );
	GLd.DrawScreenQuad(matScreenCopy);

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
void CDuskGUI::SetDefaultMaterial	( RrMaterial* newmat )
{
	matDefault = newmat;
	matDefault->setTexture(TEX_SLOT0, fntDefault);
}
//void CDuskGUI::SetHoverMaterial	( RrMaterial* newmat )
//{
//	matHover = newmat;
//}
//void CDuskGUI::SetDownMaterial	( RrMaterial* newmat )
//{
//	matDown = newmat;
//}
//void CDuskGUI::SetFontMaterial	( RrMaterial* newmat )
//{
//	matFont = newmat;
//}
void CDuskGUI::SetDefaultFont	( RrFontTexture* newfnt )
{
	if (fntDefault != NULL && fntDefault != newfnt)
	{
		delete fntDefault;
	}
	fntDefault = newfnt;
	matDefault->setTexture(TEX_SLOT0, fntDefault);
}
// Set GUI properties
void CDuskGUI::SetPixelMode ( bool enabled )
{
	//bInPixelMode = enabled;
	if (enabled == false) {
		throw core::DeprecatedFeatureException();
	}
}

// Set element properties
void CDuskGUI::SetElementVisible ( const Handle & handle, const bool vis )
{
	if (vElements[int(handle)]->visible != vis)
	{
		vElements[int(handle)]->visible = vis;
		forceUpdateRects.push_back( vElements[int(handle)]->last_visible_rect );
	}
}
void CDuskGUI::SetElementText ( const Handle & handle, const string & str )
{
	if (vElements[int(handle)]->label != str)
	{
		vElements[int(handle)]->label = str;
		forceUpdateRects.push_back( vElements[int(handle)]->last_visible_rect );
	}
}
void CDuskGUI::SetElementRect ( const Handle & handle, const Rect & rect )
{
	vElements[int(handle)]->rect = rect;
	forceUpdateRects.push_back( Rect(0,0, (Real)Screen::Info.width, (Real)Screen::Info.height) );
}
void CDuskGUI::SetElementParent ( const Handle & handle, const Handle & newparent )
{
	vElements[int(handle)]->parent = newparent;
	forceUpdateRects.push_back( Rect(0,0, (Real)Screen::Info.width, (Real)Screen::Info.height) );
}
void CDuskGUI::ToggleElementVisible ( const Handle & handle )
{
	SetElementVisible( !vElements[int(handle)]->visible );
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
// Is a dialogue active
bool CDuskGUI::HasOpenDialogue ( void )
{
	if ( hCurrentDialogue != -1 )
		return true;
	else
		return false;
}
CDuskGUI::Handle CDuskGUI::GetOpenDialogue ( void )
{
	return hCurrentDialogue;
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
void CDuskGUI::GetTextfieldData ( const Handle& handle, string & outTextVal )
{
	CDuskGUITextfield* tf = (CDuskGUITextfield*)vElements[int(handle)];
	outTextVal = tf->textValue;
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
	forceUpdateRects.push_back( vElements[int(handle)]->last_visible_rect );
}
void CDuskGUI::GetColorPicker ( const Handle & handle, Color & outColorVal )
{
	CDuskGUIColorpicker* cp = (CDuskGUIColorpicker*)vElements[int(handle)];
	outColorVal = cp->colorValue;
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
	forceUpdateRects.push_back( Rect(0,0, (Real)Screen::Info.width, (Real)Screen::Info.height) );
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

// Return the rect of the screen that is being used for drawing
Rect CDuskGUI::GetScreenRect ( void )
{
	if ( !bInPixelMode ) {
		return Rect( 0.0F, 0.0F, 1.0F, 1.0F );
	}
	else {
		return Rect( 0.0F, 0.0F, (Real)Screen::Info.width, (Real)Screen::Info.height );
	}
}