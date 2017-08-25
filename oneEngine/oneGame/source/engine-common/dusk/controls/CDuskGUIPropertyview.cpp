#include "core/input/CInput.h"

#include "../CDuskGUI.h"
#include "CDuskGUIPropertyview.h"

#include "CDuskGUIFloatfield.h"
#include "CDuskGUITextfield.h"
#include "CDuskGUIColorpicker.h"
#include "CDuskGUIVector3dPicker.h"
//#include "CDuskGUIVector2dPicker.h"
#include "CDuskGUIDropdownList_GO.h"

#include "engine/state/CGameState.h"
#include "renderer/logic/particle/CParticleEmitter.h"

#include "core/math/random/RangeValue.h"

#include "renderer/material/RrMaterial.h"

// A listview
CDuskGUI::Handle CDuskGUI::CreatePropertyview ( const Handle & parent )
{
	vElements.push_back( new CDuskGUIPropertyview() );
	vElements.back()->parent = parent;
	return Handle(vElements.size()-1);
}
/*template <typename type>
void CDuskGUI::AddPropertyOption ( const Handle& handle, const string& option, type* value )
{
	CDuskGUIPropertyview* pvl = (CDuskGUIPropertyview*)(vElements[int(handle)]);
	pvl->AddToList<type>( value );
	pvl->propertyList[pvl->propertyList.size()-1].str = option;
	pvl->propertyList[pvl->propertyList.size()-1].target = value;
}*/
// Clear the propertview completely
void CDuskGUI::ClearPropertyview ( const Handle& handle )
{
	CDuskGUIPropertyview* pvl = (CDuskGUIPropertyview*)(vElements[int(handle)]);

	// Delete all child elements
	for ( uint i = 0; i < pvl->propertyList.size(); ++i ) {
		if ( pvl->propertyList[i].element >= 0 ) {
			DeleteElement( pvl->propertyList[i].element );
		}
	}

	// Clear list
	pvl->propertyList.clear();
}
CDuskGUIPropertyview::~CDuskGUIPropertyview ( void )
{
	// Delete all child elements
	for ( uint i = 0; i < propertyList.size(); ++i ) {
		if ( propertyList[i].element >= 0 ) {
			activeGUI->DeleteElement( propertyList[i].element );
		}
	}
	// Clear list
	propertyList.clear();
}
void CDuskGUI::SetPropertyviewHeight ( const Handle& handle, const Real newheight )
{
	CDuskGUIPropertyview* pvl = (CDuskGUIPropertyview*)(vElements[int(handle)]);
	pvl->field_height = newheight;
}

void CDuskGUI::AddPropertyDivider ( const Handle& handle, const string& label )
{
	CDuskGUIPropertyview* pvl = (CDuskGUIPropertyview*)(vElements[int(handle)]);
	CDuskGUIPropertyview::ListElement_t newProp;
	newProp.type = 0;
	newProp.element = CreatePanel( handle );
	SetElementText( newProp.element, label );
	pvl->propertyList.push_back( newProp );
}

void CDuskGUI::AddPropertyOptionDropdown ( const Handle& handle, const string& option, int* value, const Handle& element )
{
	CDuskGUIPropertyview* pvl = (CDuskGUIPropertyview*)(vElements[int(handle)]);
	CDuskGUIPropertyview::ListElement_t newProp;
	newProp.str = option;
	newProp.target = value;
	pvl->Set<int>( newProp.value, *value );
	pvl->Set<int>( newProp.previous, *value );
	newProp.type = 7;
	newProp.element = element;
	//SetElementText( newProp.element, label );
	SetDropdownValue( newProp.element, *value );
	pvl->propertyList.push_back( newProp );
}


template <typename type> void CDuskGUIPropertyview::AddToList ( type* value )
{
	ListElement_t newProp;
	Set<Real>( newProp.value, *value );
	Set<Real>( newProp.previous, *value );
	newProp.type = 0;
	propertyList.push_back( newProp );
}
template <> ENGCOM_API void CDuskGUIPropertyview::AddToList ( Real* value )
{
	ListElement_t newProp;
	Set<Real>( newProp.value, *value );
	Set<Real>( newProp.previous, *value );
	newProp.type = 1;
	newProp.element = activeGUI->CreateFloatfield( activeGUI->GetFromPointer( this ), *value );
	propertyList.push_back( newProp );
}
template <> ENGCOM_API void CDuskGUIPropertyview::AddToList ( Vector3d* value )
{
	ListElement_t newProp;
	Set<Vector3d>( newProp.value, *value );
	Set<Vector3d>( newProp.previous, *value );
	newProp.type = 2;
	newProp.element = activeGUI->CreateVector3dPicker( activeGUI->GetFromPointer( this ), *value );
	propertyList.push_back( newProp );
}
template <> ENGCOM_API void CDuskGUIPropertyview::AddToList ( Color* value )
{
	ListElement_t newProp;
	Set<Color>( newProp.value, *value );
	Set<Color>( newProp.previous, *value );
	newProp.type = 3;
	newProp.element = activeGUI->CreateColorPicker( activeGUI->GetFromPointer( this ), *value );
	propertyList.push_back( newProp );
}
template <> ENGCOM_API void CDuskGUIPropertyview::AddToList ( string* value )
{
	ListElement_t newProp;
	//Set<Color>( newProp.value, *value );
	//Set<Color>( newProp.previous, *value );
	newProp.type = 4;
	//newProp.element = activeGUI->CreateColorPicker( activeGUI->GetFromPointer( this ), *value );
	newProp.element = activeGUI->CreateTextfield( activeGUI->GetFromPointer( this ), *value );
	propertyList.push_back( newProp );
}
template <> ENGCOM_API void CDuskGUIPropertyview::AddToList ( bool* value )
{
	ListElement_t newProp;
	newProp.type = 5;
	newProp.element = activeGUI->CreateCheckbox( activeGUI->GetFromPointer( this ), *value );
	propertyList.push_back( newProp );
}
template <> ENGCOM_API void CDuskGUIPropertyview::AddToList ( RangeValue<Real>* value )
{
	ListElement_t newProp;
	newProp.type = 6;
	//newProp.element = activeGUI->CreateCheckbox( activeGUI->GetFromPointer( this ) );
	// Use Vector2d picker
	propertyList.push_back( newProp );
}
template <> ENGCOM_API void CDuskGUIPropertyview::AddToList ( CParticleEmitter** value )
{
	ListElement_t newProp;
	Set<CParticleEmitter*>( newProp.value, *value );
	Set<CParticleEmitter*>( newProp.previous, *value );
	newProp.type = 10;
	newProp.element = activeGUI->CreateDropdownGameobjectList( activeGUI->GetFromPointer( this ) );
	activeGUI->SetGameobjectDropdownFilter( newProp.element, "CParticleEmitter" );
	activeGUI->RefreshGameobjectDropdownMenu( newProp.element );
	//activeGUI->SetDropdownValue( newProp.element, Get<CParticleEmitter*>( newProp.value )->GetId() );
	activeGUI->SetDropdownValue( newProp.element, (int)Get<CParticleEmitter*>( newProp.value ) );
	propertyList.push_back( newProp );
}
template <> ENGCOM_API void CDuskGUIPropertyview::AddToList ( RrMaterial* value )
{
	ListElement_t newProp;
	//Set<RrMaterial*>( newProp.value, *value );
	//Set<RrMaterial*>( newProp.previous, *value );
	newProp.type = 9;
	newProp.element = activeGUI->CreateMaterialfield( activeGUI->GetFromPointer( this ), value->getName(), value );
	propertyList.push_back( newProp );
}

void CDuskGUIPropertyview::Update ( void )
{
	CDuskGUIPanel::Update();

	if ( mouseIn ) {
		if ( CInput::DeltaMouseW() != 0 ) {
			scroll_offset -= CInput::DeltaMouseW() * 0.02f;
		}
		else {
			if ( CInput::Keydown( Keys.Down ) ) {
				scroll_offset += 0.2f;
			}
			if ( CInput::Keydown( Keys.Up ) ) {
				scroll_offset -= 0.2f;
			}
		}
	}

	// Calculate max scroll
	max_scroll_offset = propertyList.size() - (rect.size.y/field_height) + 1;
	scroll_offset = std::max<Real>( -1, std::min<Real>( scroll_offset, max_scroll_offset ) );

	// Update specific dialogues
	for ( uint i = 0; i < propertyList.size(); ++i )
	{
		if ( propertyList[i].element >= 0 )
		{
			int selectedId;
			switch ( propertyList[i].type )
			{
			case 1: // Float
				activeGUI->UpdateFloatfield( propertyList[i].element, *((Real*)propertyList[i].target) );
				break;
			case 2: // Vector3d
				activeGUI->UpdateVector3dPicker( propertyList[i].element, *((Vector3d*)propertyList[i].target) );
				break;
			case 3: // Color
				activeGUI->UpdateColorPicker( propertyList[i].element, *((Color*)propertyList[i].target) );
				break;
			case 4: // String
				activeGUI->UpdateTextfield( propertyList[i].element, *((string*)propertyList[i].target) );
				break;
			case 5: // Bool
				activeGUI->UpdateCheckbox( propertyList[i].element, *((bool*)propertyList[i].target) );
				break;
			case 7: // Int dropdownlist
				selectedId = activeGUI->GetDropdownOption( propertyList[i].element );
				if ( Get<int>( propertyList[i].target ) != selectedId ) {
					if ( Get<int>( propertyList[i].target ) != Get<int>( propertyList[i].value ) ) {
						// Target changed, change ddl
						activeGUI->SetDropdownValue( propertyList[i].element, Get<int>( propertyList[i].target ) );
					}
					else {
						// DDL changed, change value
						Set<int>( propertyList[i].value, selectedId );
					}
				}
				// Set target to value
				Set<int>( propertyList[i].target, Get<int>( propertyList[i].value ) );
				break;
			case 10: // CParticleEmitter pointer
				selectedId = activeGUI->GetDropdownOption( propertyList[i].element );
				if ( selectedId == -1 ) {
					std::cout << "Bad List" << std::endl;
				}
				/*else if ( selectedId == -2 )
				{
					// Set null value
					Set<CParticleEmitter*>( propertyList[i].value, NULL );
					Set<CParticleEmitter*>( propertyList[i].target, Get<CParticleEmitter*>( propertyList[i].value ) );
				}*/
				else
				{
					// Dropdown does not match target
					if (( Get<CParticleEmitter*>( propertyList[i].target ) == NULL )||( selectedId != (int)Get<CParticleEmitter*>( propertyList[i].target ) ))
					{
						// Something changed
						if ( Get<CParticleEmitter*>( propertyList[i].target ) == NULL )
						{
							// Dropdown list changed to nonnull, so change value
							if ( selectedId >= 0 )
								Set<CParticleEmitter*>( propertyList[i].value, (CParticleEmitter*) CGameState::Active()->GetBehavior( selectedId ) );
							else
								Set<CParticleEmitter*>( propertyList[i].value, NULL );
						}
						//else if ( Get<CParticleEmitter*>( propertyList[i].target )->GetId() == Get<CParticleEmitter*>( propertyList[i].value )->GetId() ) {
						else if ( (int)Get<CParticleEmitter*>( propertyList[i].target ) == (int)Get<CParticleEmitter*>( propertyList[i].value ) )
						{
							// Dropdown list changed, so change value
							if ( selectedId >= 0 )
								Set<CParticleEmitter*>( propertyList[i].value, (CParticleEmitter*) CGameState::Active()->GetBehavior( selectedId ) );
							else
								Set<CParticleEmitter*>( propertyList[i].value, NULL );
						}
						else {
							// Value changed, so change dropdown selection
							activeGUI->SetDropdownValue( propertyList[i].element, (int)Get<CParticleEmitter*>( propertyList[i].target ) );
						}
					}
					// Set target to value
					Set<CParticleEmitter*>( propertyList[i].target, Get<CParticleEmitter*>( propertyList[i].value ) );
				}
				break;
			case 9: // RrMaterial pointer
				//string dummyVar = 
				//activeGUI->UpdateTextfield( propertyList[i].element, *((string*)propertyList[i].target) );
				// Should be automatically managed
				break;
			}
		}
	}

	// Place the option control rects correctly
	for ( uint i = 0; i < propertyList.size(); ++i ) {
		if ( propertyList[i].element >= 0 ) {
			if (( i-scroll_offset < 0 )||( (i-scroll_offset)*field_height > rect.size.y-field_height )) {
				activeGUI->SetElementVisible( propertyList[i].element, false );
			}
			else {
				activeGUI->SetElementVisible( propertyList[i].element, true );
				activeGUI->SetElementRect( propertyList[i].element,
					Rect(
						activeGUI->parenting_offset.x + rect.size.x*name_width,
						activeGUI->parenting_offset.y + field_height*(i-scroll_offset),
						rect.size.x*(1-name_width-scrollbar_width), field_height )
				);
			}
		}
	}
}
void CDuskGUIPropertyview::Render ( void )
{
	CDuskGUIPanel::Render();

	// Draw the boxes around options
		setDrawDefault();
		for ( uint i = 0; i < propertyList.size(); ++i )
		{
			if (( i-scroll_offset < 0 )||( (i-scroll_offset)*field_height > rect.size.y-field_height )) {
				continue;
			}
			drawRectWire( Rect( rect.pos.x, rect.pos.y + field_height*(i-scroll_offset), rect.size.x*name_width, field_height ) );
		}

	// Now draw property names
		for ( uint i = 0; i < propertyList.size(); ++i )
		{
			if (( i-scroll_offset < 0 )||( (i-scroll_offset)*field_height > rect.size.y-field_height )) {
				continue;
			}
			drawText( rect.pos.x + 0.008f, rect.pos.y + field_height*((i-scroll_offset)+0.6f), propertyList[i].str.c_str() );
		}
}
