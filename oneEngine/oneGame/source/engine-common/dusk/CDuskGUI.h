#ifndef C_DUSK_GUI_H_
#define C_DUSK_GUI_H_

#include "core/system/Screen.h"
#include "core/system/System.h"
#include "core/types/ModelData.h"
#include "core/math/Rect.h"

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"
#include "renderer/object/immediate/immediate.h"

// Include Draw stuff
class RrMaterial;
class RrFontTexture;
class RrRenderTexture;

#include <string>
#include <vector>
#include <list>

using std::string;

// Include handle type
#include "CDuskGUIHandle.h"

// Forward declaration of all Dusk UI element classes
class CDuskGUIElement;
class CDuskGUIPanel;
class CDuskGUIButton;
class CDuskGUICheckbox;
class CDuskGUIFloatfield;
class CDuskGUILabel;
class CDuskGUIColorpicker;
class CDuskGUIParagraph;
class CDuskGUIDialogue;
class CDuskGUIDialogue_YN;
class CDuskGUITextfield;
class CDuskGUIDropdownList;
class CDuskGUIListview;
class CDuskGUIPropertyview;
class CDuskGUITabview;
class CDuskGUIDraggablePanel;
class CDuskGUIVector3fPicker;
class CDuskGUIDropdownList_GO;
class CDuskGUIMaterialField;
class CDuskGUIMaterialPicker;
class CDuskGUISlider;
class CDuskGUIEdgePanel;
namespace Dusk
{
	class DialogueFileSelector;
	class DialogueColorpicker;
}

// Class Definition
class CDuskGUI : public CGameBehavior, public CRenderableObject
{
	ClassName( "DuskGUI" );
public:
	// Handle type
	friend Dusk::Handle;
	typedef Dusk::Handle Handle;

private:
	// Hide the copy constructor
	CDuskGUI ( const CDuskGUI& );
	// Friend elements
	friend CDuskGUIElement;
	friend CDuskGUIPanel;
	friend CDuskGUIButton;
	friend CDuskGUICheckbox;
	friend CDuskGUIFloatfield;
	friend CDuskGUILabel;
	friend CDuskGUIColorpicker;
	friend CDuskGUIParagraph;
	friend CDuskGUIDialogue;
	friend CDuskGUIDialogue_YN;
	friend CDuskGUITextfield;
	friend CDuskGUIDropdownList;
	friend CDuskGUIListview;
	friend CDuskGUIPropertyview;
	friend CDuskGUITabview;
	friend CDuskGUIDraggablePanel;
	friend CDuskGUIVector3fPicker;
	friend CDuskGUIDropdownList_GO;
	friend CDuskGUIMaterialField;
	friend CDuskGUIMaterialPicker;
	friend CDuskGUISlider;
	friend CDuskGUIEdgePanel;
	friend Dusk::DialogueFileSelector;
	friend Dusk::DialogueColorpicker;
public:
	// Constructor+Destructor
	ENGCOM_API explicit		CDuskGUI ( RrFontTexture* font );
	ENGCOM_API				~CDuskGUI ( void );

	// == Stepping Functions ==
	// Update
	void	Update ( void ) override;
	// Render
	bool	PreRender ( void ) override;
	bool	Render ( const char pass ) override;

private:

	//void	Render_SetupMaterial ( RrMaterial* );
	void	RenderUI ( void );

public:

	// == Setters ==
	// Set materials
	ENGCOM_API void SetDefaultMaterial	( RrMaterial* );
	//ENGCOM_API void SetHoverMaterial	( RrMaterial* );
	//ENGCOM_API void SetDownMaterial	( RrMaterial* );
	//ENGCOM_API void SetFontMaterial	( RrMaterial* );
	ENGCOM_API void SetDefaultFont		( RrFontTexture* );
	// Set pixel mode
	ENGCOM_API void SetPixelMode ( bool enabled=false );
	// Set element properties
	ENGCOM_API void SetElementVisible ( const Handle&, const bool=true );
	ENGCOM_API void SetElementText ( const Handle&, const string& );
	ENGCOM_API void SetElementRect ( const Handle&, const Rect& );
	ENGCOM_API void SetElementParent ( const Handle&, const Handle& );
	ENGCOM_API void ToggleElementVisible ( const Handle& );

	// == Getters ==
	// Grab GUI parenting info
	ENGCOM_API void GetChildren ( const Handle&, std::vector<Handle>& );
	// Grab handle based on pointer
	ENGCOM_API Handle GetFromPointer ( const CDuskGUIElement* );
	// Grab states of GUI
	ENGCOM_API bool GetMouseInGUI ( void );
	// Grab states of elements
	ENGCOM_API bool GetClicked ( const Handle& );
	ENGCOM_API bool GetMouseOver ( const Handle& );
	// Element specific
	ENGCOM_API bool GetButtonClicked ( const Handle& );
	ENGCOM_API int GetDialogueResponse ( const Handle& );


	// == Creating new elements ==
	// Panel used for the backdrop of other elements
	ENGCOM_API Handle CreatePanel	( const Handle& =-1 );
	// Draggable panel for element backdrop
	ENGCOM_API Handle CreateDraggablePanel ( const Handle& =-1 );
	// Edge panel for bar menu backdrop
	ENGCOM_API Handle CreateEdgePanel ( const Handle& =-1 );
	// Button used for clicking and stuff
	ENGCOM_API Handle CreateButton	( const Handle& =-1 );
	// A text-only control
	ENGCOM_API Handle CreateText	( const Handle& =-1, const string& str="" );
	// Another text-only control
	ENGCOM_API Handle CreateParagraph	( const Handle& =-1, const string& str="" );
	// A slider control w/ io
	ENGCOM_API Handle CreateSlider	( const Handle& =-1 );
		// One-way get
		ENGCOM_API Real GetSliderValue ( const Handle& );
		// Slider updating
		ENGCOM_API void UpdateSlider ( const Handle&, Real & inOutSliderVal );
		// Slider options
		ENGCOM_API void SetSliderMinMax ( const Handle&, const Real, const Real );
		ENGCOM_API void SetSliderSnapping ( const Handle&, const Real =-1 );
	// A checkbox control w/ io
	ENGCOM_API Handle CreateCheckbox	( const Handle& =-1, const bool=false );
	// A color picker w/ io
	ENGCOM_API Handle CreateColorPicker( const Handle& =-1, const Color& color=Color(1.0f,1.0f,1.0f,1.0f) );
	// A vector3d picker
	ENGCOM_API Handle CreateVector3fPicker ( const Handle& =-1, const Vector3f& vect=Vector3f( 0,0,0 ) );
		// Update Vector3f picker
		ENGCOM_API void UpdateVector3fPicker ( const Handle&, Vector3f & inOutVectorVal );
		ENGCOM_API void SetVector3fPicker ( const Handle &, Vector3f & inVectorVal );
	// A floatfield w/ io
	ENGCOM_API Handle CreateFloatfield ( const Handle& =-1, const float =0 );
	// A textfield w/ io
	ENGCOM_API Handle CreateTextfield ( const Handle& =-1, const string& =string("") );
		// Textfield updating
		ENGCOM_API void UpdateTextfield ( const Handle&, string & inOutTextVal );
		ENGCOM_API void GetTextfieldData ( const Handle&, string & outTextVal );
	// A material textfield
	ENGCOM_API Handle CreateMaterialfield ( const Handle& =-1, const string& =string(""), RrMaterial* =NULL );
		ENGCOM_API void SetMaterialfieldTarget ( const Handle&, RrMaterial* );
	// A dropdown list
	ENGCOM_API Handle CreateDropdownList ( const Handle& =-1 );
		ENGCOM_API void AddDropdownOption ( const Handle&, const string&, const int );
		ENGCOM_API void SetDropdownValue ( const Handle&, const int );
		// Get/set dropdown value
		ENGCOM_API string GetCurrentDropdownString ( const Handle& );
		ENGCOM_API int GetDropdownOption ( const Handle& );
		ENGCOM_API void SetDropdownOption ( const Handle&, const int );
		// Clear values
		ENGCOM_API void ClearDropdownList ( const Handle& );
	// A dropdown list for gameobject lists
	ENGCOM_API Handle CreateDropdownGameobjectList ( const Handle& =-1 );
		ENGCOM_API void RefreshGameobjectDropdownMenu ( const Handle& );
		ENGCOM_API void SetGameobjectDropdownFilter ( const Handle&, const string& );
	// A listview
	ENGCOM_API Handle CreateListview ( const Handle& =-1 );
		ENGCOM_API void AddListviewOption ( const Handle&, const string&, const int );
		ENGCOM_API void ClearListview ( const Handle& );
		ENGCOM_API void SetListviewFieldHeight ( const Handle&, const Real );
		ENGCOM_API int  GetListviewCount ( const Handle& );
	// A property view
	ENGCOM_API Handle CreatePropertyview ( const Handle& =-1 );
		template <typename type>
		void AddPropertyOption ( const Handle&, const string&, type* );
		ENGCOM_API void AddPropertyOptionDropdown ( const Handle&, const string&, int*, const Handle& );
		ENGCOM_API void AddPropertyDivider ( const Handle&, const string& );
		ENGCOM_API void ClearPropertyview ( const Handle& );
		ENGCOM_API void SetPropertyviewHeight ( const Handle&, const Real );
	// A tab view
	ENGCOM_API Handle CreateTabview ( const Handle& =-1 );
		ENGCOM_API Handle AddTabOption ( const Handle&, const string& ); // Adds a tab with the string to designated tabview, returns handle to panel

	// A dialogue with yes/no buttons
	ENGCOM_API Handle CreateYesNoDialogue ( const Handle& =-1 );

	// == Updating elements ==
	// Checkbox updating
	ENGCOM_API void UpdateCheckbox ( const Handle&, bool & inOutCheckboxVal );
	// Color picker updating
	ENGCOM_API void UpdateColorPicker ( const Handle&, Color & inOutColorVal );
	ENGCOM_API void SetColorPicker ( const Handle &, Color & inColorVal );
	ENGCOM_API void GetColorPicker ( const Handle &, Color & outColorVal );
	ENGCOM_API bool ColorPickerInDialogue ( const Handle & );
	// Floatfield updating
	ENGCOM_API void UpdateFloatfield ( const Handle&, float & inOutFloatVal );

	// Get/set listview value
	ENGCOM_API int GetListviewSelection ( const Handle& );
	ENGCOM_API int GetListviewIndex ( const Handle& );
	ENGCOM_API void SetListviewSelection ( const Handle&, const int );

	// Show Y/N dialogue
	ENGCOM_API void ShowYesNoDialogue ( const Handle& );
	// Hide dialogue
	ENGCOM_API void HideDialogue ( const Handle& );
	// Is a dialogue active
	ENGCOM_API bool HasOpenDialogue ( void );
	ENGCOM_API Handle GetOpenDialogue ( void );

	// IO Dialogues
	ENGCOM_API Handle DialogueOpenFilename ( System::sFileDialogueEntry* nFiletypes, int nFiletypeCount, const char* nInitialDir="", const char* nDialogueTitle="Open File" );
	ENGCOM_API Handle DialogueSaveFilename ( System::sFileDialogueEntry* nFiletypes, int nFiletypeCount, const char* nInitialDir="", const char* nDialogueTitle="Save File" );
	ENGCOM_API bool OpenDialogueHasSelection ( const Handle& handleOverride=-1 );
	ENGCOM_API bool SaveDialogueHasSelection ( const Handle& handleOverride=-1 );
	ENGCOM_API bool GetOpenFilename ( char* nOutFilename, const Handle& handleOverride=-1 );
	ENGCOM_API bool GetSaveFilename ( char* nOutFilename, const Handle& handleOverride=-1 );

	// Item Dialogues
	ENGCOM_API Handle DialogueOpenColorpicker ( const Color& n_initialColor, const char* nDialogueTitle );
	ENGCOM_API bool ColorpickerDialogueHasSelection ( const Handle& handleOverride=-1 );
	ENGCOM_API bool GetColorpickerValue ( Color* n_outColor, const Handle& handleOverride=-1 );

	// Destroying elements
	ENGCOM_API void DeleteElement ( const Handle & );

private:
	// Current options
	bool		bInPixelMode;

private:
	// Materials used in drawing
	RrMaterial*	matScreenCopy;
	RrMaterial*	matDefault;
	/*RrMaterial*	matHover;
	RrMaterial*	matDown;
	RrMaterial* matFont;*/
	RrFontTexture*	fntDefault;

	// Static default values
	/*static RrMaterial*	matDefDefault;
	static RrMaterial*	matDefHover;
	static RrMaterial*	matDefDown;
	static RrMaterial*	matDefFont;
	static RrFontTexture*	fntDefDefault;*/

	// Rendering state
	RrRenderTexture* renderBuffer;

	// Element information and handles
	std::vector<CDuskGUIElement*>	vElements;
	Handle	hCurrentElement;
	Handle	hCurrentDialogue;
	Handle	hCurrentMouseover;
	Handle	hCurrentFocus;

	std::vector<Rect>	forceUpdateRects;

private:
	// == Private Routines ==

	//		UpdateCurrentMouseover ( )
	// Updates the hCurrentMouseover variable
	// useful for sub-dialogues that need instant updates
	void UpdateCurrentMouseover ( void );

	//		GetPositionIn ( rectangle )
	// Checks if a position is in a rectangle
	bool GetPositionIn ( const Rect& );
	
	//		InitializeDefaultMaterials ( )
	// Initialize the default static materials
	void InitializeDefaultMaterials ( void );

	//		GetScreenRect ( )
	// Return the rect of the screen that is being used for drawing
	Rect GetScreenRect ( void );

public:
	// Drawing functions
	ENGCOM_API void setDrawDown ( void );
	ENGCOM_API void setDrawHover ( void );
	ENGCOM_API void setDrawDefault ( void );

	ENGCOM_API void setSubdrawSelection ( void );
	ENGCOM_API void setSubdrawDarkSelection ( void );

	ENGCOM_API void setSubdrawTransparent ( void );
	ENGCOM_API void setSubdrawOpaque ( void );

	ENGCOM_API void setSubdrawNotice ( void );
	ENGCOM_API void setSubdrawError ( void );

	ENGCOM_API void setSubdrawPulse ( void );
	ENGCOM_API void setSubdrawOverrideColor ( const Color& color );

	ENGCOM_API void setSubdrawDefault ( void );

	ENGCOM_API void drawRect ( const Rect& rect );
	ENGCOM_API void drawRectWire ( const Rect& rect, bool focused );
	ENGCOM_API void drawLine ( const Real x1, const Real y1, const Real x2, const Real y2 );

	ENGCOM_API void drawText ( const Real x, const Real y, const char* str );
	ENGCOM_API void drawTextWidth ( const Real x, const Real y, const Real w, const char* str );
	ENGCOM_API void drawTextCentered ( const Real x, const Real y, const char* str );

private:
	Color	m_basecolor;
	Color	m_basecolor_hover;
	Color	m_basecolor_down;
	// Drawing modes
	int		mainColorMode;

	bool	drawLight;
	bool	drawDark;
	int		drawOpacity;
	int		drawError;
	bool	drawPulse;
	bool	drawColorOverride;

	Color	m_drawcolor;
	void	SetDrawColor ( void );

	struct textRequest_t
	{
		string		text;
		Vector2f	position;
		Real		width;
		char		mode;
	};

	Vector2f parenting_offset;
	std::vector<Vector2f> offsetList;

	// Drawing queue
	//std::vector<arModelVertex>	modelSolidMeshList;
	//std::vector<arModelVertex>	modelLineMeshList;
	//std::vector<textRequest_t>	modelTextRequestList;

	// Immediate mode renderer:
	rrTextBuilder2D*	m_builder;
};

typedef CDuskGUI DuskGUI;


#include "controls/CDuskGUIPropertyview.h"
template <typename type>
void CDuskGUI::AddPropertyOption ( const Handle& handle, const string& option, type* value )
{
	CDuskGUIPropertyview* pvl = (CDuskGUIPropertyview*)(vElements[int(handle)]);
	pvl->AddToList<type>( value );
	pvl->propertyList[pvl->propertyList.size()-1].str = option;
	pvl->propertyList[pvl->propertyList.size()-1].target = value;
}

#endif//C_DUSK_GUI_H_
