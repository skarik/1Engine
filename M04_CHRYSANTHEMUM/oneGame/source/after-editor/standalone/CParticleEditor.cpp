
#include "CParticleEditor.h"
//#include "COglWindow.h"
#include "core/system/System.h"
#include "renderer/debug/CDebugDrawer.h"

#include "core/system/io/CBinaryFile.h"
#include "core-ext/system/io/serializer/ISerialBinary.h"
#include "core-ext/system/io/serializer/OSerialBinary.h"
#include "core/system/io/CSegmentedFile.h"

#include "renderer/logic/particle/modifiers/CParticleMod_Windmotion.h"
#include "renderer/logic/particle/modifiers/CParticleMod_Spiral.h"
#include "renderer/object/particle/CParticleRenderer_Animated.h"

#include "renderer/camera/CCamera.h"

#include "after/entities/world/environment/DayAndNightCycle.h"
#include "engine/behavior/CExtendableGameObject.h"

#include "renderer/state/Settings.h"
#include "renderer/logic/model/CModel.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/glMaterial.h"

#include <sstream>

using namespace std;

// Constructor
CParticleEditor::CParticleEditor ( bool ingame )
	: CGameBehavior ()
{
	bCameraActive = true;
	if ( ingame ) {
		bCameraActive = false;
	}

	if ( bCameraActive ) {
		myCamera = new CCamera();
		myCamera->fov = 80;
		cameraDistance = 5;
		vCameraCenter = Vector3d(0,0,0);
		vCameraAngles = Vector3d( 0,-45,45 );
	}
	
	cBackgroundColor = Color( 0.4f,0.4f,0.4f );
	mDaycycle = NULL;
	//mDaycycle = new Daycycle();
	mLight = NULL;
	mBgSphere = NULL;
	/*{
		mBgSphere = new CModel( ".res/models/geosphere.FBX" );
		mBgSphere->transform.scale = Vector3d(1,1,-1) * 8;
		mBgSphere->SetRenderType( Renderer::Background );

		glMaterial* bgGray = glMaterial::Default->copy();
		bgGray->m_diffuse = Color(0,0,0,1);
		bgGray->m_emissive = cBackgroundColor;
		bgGray->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
		bgGray->deferredinfo[0].m_lighting_mode = Renderer::LI_NONE;

		mBgSphere->SetMaterial( bgGray );
		bgGray->removeReference();
	}*/
	mDaycycle = new Daycycle();
	mDaycycle->SetTimeOfDay( 60*60*18 );

	// Create particle system
	LoadDefaults();

	// Create and config GUI
	CreateGUI();
}

// Destructor
CParticleEditor::~CParticleEditor ( void )
{
	delete myCamera;
	delete myGUI;

	//delete [] hFFRendererColors;
	if ( mParticleObject ) {
		//mParticleObject->RemoveReference();
		//DeleteObject( mParticleObject );
		delete mParticleObject;
	}
}

// Initialize default particle system
void CParticleEditor::LoadDefaults ( void )
{
	mParticleObject = new CExtendableGameObject;
	mParticleObject->RemoveReference();
}

// Update
void CParticleEditor::Update ( void )
{
	DoGUIWork();

	if ( bCameraActive )
	{
		Matrix4x4 rotMatx;

		// Camera controls
		if ( !myGUI->GetMouseInGUI() )
		{
			if ( !CInput::Key(Keys.Control) )
			{
				if ( (CInput::Mouse(CInput::MBLeft) && CInput::Mouse(CInput::MBRight))||(CInput::Mouse(CInput::MBMiddle))||(CInput::Mouse(CInput::MBRight)&&CInput::Key(Keys.Alt)) ) {
					rotMatx.setRotation( myCamera->transform.rotation );
					vCameraCenter += rotMatx * Vector3d( 0, (ftype)CInput::DeltaMouseX(), (ftype)CInput::DeltaMouseY() ) * ( 1.2f + sqrt(cameraDistance) ) * 0.02f;
				}
				if ( CInput::Mouse( CInput::MBLeft ) ) {
					vCameraAngles += Vector3d( 0, -(ftype)CInput::DeltaMouseY()*0.7f, (ftype)CInput::DeltaMouseX()*0.7f );
				}
				else if ( CInput::Mouse( CInput::MBRight ) ) {
					cameraDistance += (ftype)CInput::DeltaMouseY() * ( 1.2f + sqrt(cameraDistance) ) * 0.02f;
					if ( cameraDistance < 0 ) {
						cameraDistance = 0;
					}
				}
			}
			else if ( mParticleObject )
			{
				if ( (CInput::Mouse(CInput::MBLeft) && CInput::Mouse(CInput::MBRight))||(CInput::Mouse(CInput::MBMiddle))||(CInput::Mouse(CInput::MBRight)&&CInput::Key(Keys.Alt)) ) {
					rotMatx.setRotation( myCamera->transform.rotation );
					mParticleObject->transform.position -= rotMatx * Vector3d( 0, (ftype)CInput::DeltaMouseX(), (ftype)CInput::DeltaMouseY() ) * ( 1.2f + sqrt(cameraDistance) ) * 0.02f;
				}
				else if ( CInput::Key('C') ) {
					mParticleObject->transform.position = Vector3d(0,0,0);
				}
				/*for ( vector<CParticleEmitter*>::iterator it = mParticleObject->AddComponent.begin(); it != vpEmitters.end(); it++ )
				{
					(*it)->transform.Get( this->transform );
					(*it)->active = enabled;
				}
				for ( vector<CRenderableObject*>::iterator it = vpRComponents.begin(); it != vpRComponents.end(); it++ )
				{
					(*it)->transform.Get( this->transform );
				}*/
			}
		}
		myCamera->transform.rotation.Euler( vCameraAngles );
		rotMatx.setRotation( myCamera->transform.rotation );
		myCamera->transform.position = vCameraCenter + rotMatx*Vector3d( -cameraDistance,0,0 );
	}

	// Reset emitters if a one-shot
	bool canReset = true;
	for ( uint i = 0; i < particleComponentList.size(); ++i )
	{
		if ( particleComponentList[i].type == 0 ) {
			CParticleEmitter* emitter = (CParticleEmitter*) particleComponentList[i].ptr;
			if ( !emitter->vParticles.empty() ) {
				canReset = false;
			}
		}
	}
	if ( canReset )
	{
		for ( uint i = 0; i < particleComponentList.size(); ++i )
		{
			if ( particleComponentList[i].type == 0 ) {
				CParticleEmitter* emitter = (CParticleEmitter*) particleComponentList[i].ptr;
				emitter->bHasEmitted = false;
			}
		}
	}

	// Set background
	if ( !mDaycycle ) {
		Renderer::Settings.clearColor = cBackgroundColor;
	}
	if ( mBgSphere ) {
		mBgSphere->transform.position = myCamera->transform.position;
	}
	// Draw grid
	for ( int i = -20; i <= 20; i += 2 ) {
		Debug::Drawer->DrawLine( Vector3d( 20,(ftype)i,0 ), Vector3d( -20,(ftype)i,0 ), Color(0.2f,0.2f,0.2f) );
		Debug::Drawer->DrawLine( Vector3d( (ftype)i,20,0 ), Vector3d( (ftype)i,-20,0 ), Color(0.2f,0.2f,0.2f) );
	}

}

// Create + Config GUI
void CParticleEditor::CreateGUI ( void )
{
	// Create and config GUI
	GUIHandle prntHandle;
	GUIHandle tempHandle;
	myGUI = new CDuskGUI();

	myGUI->SetDefaultFont( new CBitmapFont( "YanoneKaffeesatz-R.otf", 13 ) );


	prntHandle = myGUI->CreateDraggablePanel();
	myGUI->SetElementRect( prntHandle, Rect( 0.02f, 0.02f, 0.27f, 0.68f ) );
	myGUI->SetElementText( prntHandle, "Components" );
	{
		tempHandle = myGUI->CreateListview( prntHandle );
		myGUI->SetElementRect( tempHandle, Rect( 0.04f, 0.07f, 0.23f, 0.50f ) );
		edtComponentList.list = tempHandle;
		
		tempHandle = myGUI->CreateButton( prntHandle );
		myGUI->SetElementRect( tempHandle, Rect( 0.04f, 0.59f, 0.10f, 0.04f ) );
		myGUI->SetElementText( tempHandle, "Delete selected" );
		edtComponentList.btnDelete = tempHandle;

		tempHandle = myGUI->CreateButton( prntHandle );
		myGUI->SetElementRect( tempHandle, Rect( 0.16f, 0.59f, 0.09f, 0.04f ) );
		myGUI->SetElementText( tempHandle, "Edit selected" );
		edtComponentList.btnEdit = tempHandle;

		tempHandle = myGUI->CreateButton( prntHandle );
		myGUI->SetElementRect( tempHandle, Rect( 0.04f, 0.64f, 0.09f, 0.04f ) );
		myGUI->SetElementText( tempHandle, "Load System" );
		edtComponentList.btnLoad = tempHandle;

		tempHandle = myGUI->CreateButton( prntHandle );
		myGUI->SetElementRect( tempHandle, Rect( 0.16f, 0.64f, 0.09f, 0.04f ) );
		myGUI->SetElementText( tempHandle, "Save System" );
		edtComponentList.btnSave = tempHandle;
	}

	prntHandle = myGUI->CreateDraggablePanel();
	myGUI->SetElementRect( prntHandle, Rect( 0.32f, 0.02f, 0.24f, 0.44f ) );
	myGUI->SetElementText( prntHandle, "Add Component" );
	{
		tempHandle = myGUI->CreateListview( prntHandle );
		myGUI->SetElementRect( tempHandle, Rect( 0.34f, 0.07f, 0.20f, 0.30f ) );
		myGUI->AddListviewOption( tempHandle, "Default Emitter", 0 );
		myGUI->AddListviewOption( tempHandle, "Default Updater", 1 );
		myGUI->AddListviewOption( tempHandle, "Default Renderer", 2 );
		myGUI->AddListviewOption( tempHandle, "Emitter - Cloud", 3 );
		myGUI->AddListviewOption( tempHandle, "Renderer - Animation", 5 );
		myGUI->AddListviewOption( tempHandle, "Modifier - Wind", 4 );
		myGUI->AddListviewOption( tempHandle, "Modifier - Spiral", 6 );
		edtNewComponents.list = tempHandle;

		tempHandle = myGUI->CreateButton( prntHandle );
		myGUI->SetElementRect( tempHandle, Rect( 0.34f, 0.39f, 0.09f, 0.04f ) );
		myGUI->SetElementText( tempHandle, "Add selected" );
		edtNewComponents.btnNew = tempHandle;
	}

	prntHandle = myGUI->CreateDraggablePanel();
	myGUI->SetElementRect( prntHandle, Rect( 0.40f, 0.50f, 0.58f, 0.48f ) );
	myGUI->SetElementText( prntHandle, "Component Properties" );
	{
		tempHandle = myGUI->CreatePropertyview( prntHandle );
		myGUI->SetElementRect( tempHandle, Rect( 0.42f, 0.55f, 0.54f, 0.40f ) );
		edtProperties.view = tempHandle;
	}

}
// GUI Work
void CParticleEditor::DoGUIWork ( void )
{
	// ==Component List==
	if ( myGUI->GetListviewCount( edtComponentList.list ) != particleComponentList.size() )
	{
		// clear the list and recreate it
		myGUI->ClearListview( edtComponentList.list );

		for ( uint i = 0; i < particleComponentList.size(); ++i ) {
			switch ( particleComponentList[i].type ) {
			case 0:
				myGUI->AddListviewOption( edtComponentList.list, "Default Emitter",particleComponentList[i].type );	break;
			case 1:
				myGUI->AddListviewOption( edtComponentList.list, "Default Updater",particleComponentList[i].type );	break;
			case 2:
				myGUI->AddListviewOption( edtComponentList.list, "Default Renderer",particleComponentList[i].type );break;	
			case 3:
				myGUI->AddListviewOption( edtComponentList.list, "Emitter - Cloud",particleComponentList[i].type );break;	
			case 4:
				myGUI->AddListviewOption( edtComponentList.list, "Modifier - Wind",particleComponentList[i].type );break;
			case 5:
				myGUI->AddListviewOption( edtComponentList.list, "Renderer - Animation",particleComponentList[i].type );break;
			case 6:
				myGUI->AddListviewOption( edtComponentList.list, "Modifier - Spiral",particleComponentList[i].type );break;
			}
		}
	}
	if ( myGUI->GetButtonClicked( edtComponentList.btnDelete ) ) {
		// Remove the current selected object
		int selection = myGUI->GetListviewIndex( edtComponentList.list );
		if ( selection >= 0 ) {
			if ( particleComponentList[selection].ptr != NULL ) {
				mParticleObject->RemoveComponent( particleComponentList[selection].ptr );
				switch ( particleComponentList[selection].type ) {
				case 0: case 1: case 3:
					delete (CGameBehavior*)(particleComponentList[selection].ptr);
					break;
				case 4: case 6:
					delete (CParticleModifier*)(particleComponentList[selection].ptr);
					break;
				default:
					delete (CRenderableObject*)(particleComponentList[selection].ptr);
					break;
				}
			}
			particleComponentList.erase( particleComponentList.begin()+selection );
		}
		// Clear off property view in case
		myGUI->ClearPropertyview( edtProperties.view );
	}
	if ( myGUI->GetButtonClicked( edtComponentList.btnEdit ) ) {
		// Edit the current selected object, populating property list based on type.
		int selection = myGUI->GetListviewIndex( edtComponentList.list );
		if ( selection >= 0 ) {
			//int type = myGUI->GetListviewSelection( edtComponentList.list );
			RecreatePropertyList( particleComponentList[selection] );
		}
	}

	if ( myGUI->GetButtonClicked( edtComponentList.btnSave ) ) 
	{
		// Show the save GUI
		System::sFileDialogueEntry entry;
		strcpy( entry.extension, "*.PCF" );
		strcpy( entry.filetype, "Particle system files" );
		edtComponentList.saveDlg = myGUI->DialogueSaveFilename( &entry, 1, ".res/particlesystems/", "Save particle system" );
	}
	if ( (edtComponentList.saveDlg > 0) && myGUI->SaveDialogueHasSelection( edtComponentList.saveDlg ) == 1 )
	{
		char str_result [1024];
		//if ( System::GetSaveFilename( str_result, &entry, 1, ".res/particlesystems/", "Save particle system" ) )
		if ( myGUI->GetSaveFilename( str_result ) )
		{
			string resultFile = str_result;
			if ( resultFile.find( ".pcf" ) == string::npos ) {
				resultFile += ".pcf";
			}
			cout << "Saving: " << resultFile << endl;
			SaveSystem( resultFile );
		}
	}

	if ( myGUI->GetButtonClicked( edtComponentList.btnLoad ) ) {
		char str_result [1024];
		System::sFileDialogueEntry entry;
		strcpy( entry.extension, "*.PCF" );
		strcpy( entry.filetype, "Particle system files" );
		if ( System::GetOpenFilename( str_result, &entry, 1, ".res/particlesystems/", "Load particle system" ) )
		{
			cout << "Opening: " << str_result << endl;
			LoadSystem( str_result );
			// clear the list and recreate it
			myGUI->ClearListview( edtComponentList.list );
		}
	}

	// ==Add Component==
	if ( myGUI->GetButtonClicked( edtNewComponents.btnNew ) ) {
		int selection = myGUI->GetListviewSelection( edtNewComponents.list );
		if ( selection >= 0 ) {
			editorComponent_t newComponentEntry;
			newComponentEntry.type = selection;
			newComponentEntry.ptr = NULL;

			CParticleEmitter* emitter;
			CParticleUpdater* updater;
			CParticleRenderer* renderer;
			CParticleModifier* modifier;
			switch ( selection ) {
				case 0:
					emitter = new CParticleEmitter();
					newComponentEntry.ptr = emitter;
					mParticleObject->AddComponent( emitter );
					emitter->vcColors.clear();
					emitter->transform.SetParent( &(mParticleObject->transform) );
					for ( uint cn = 0; cn < 5; ++ cn ) {
						emitter->vcColors.push_back( Color(1.0f,1.0f,1.0f,1.0f) );
					}
					break;
				case 1:
					// look for first emitter in the list
					emitter = NULL;
					for ( uint i = 0; i < particleComponentList.size(); ++i ) {
						if ( particleComponentList[i].type == 0 ) {
							emitter = (CParticleEmitter*)particleComponentList[i].ptr;
							break;
						}
					}
					updater = new CParticleUpdater( emitter );
					newComponentEntry.ptr = updater;
					mParticleObject->AddComponent( updater );
					break;
				case 2:
					// look for first emitter in the list
					emitter = NULL;
					for ( uint i = 0; i < particleComponentList.size(); ++i ) {
						if ( particleComponentList[i].type == 0 ) {
							emitter = (CParticleEmitter*)particleComponentList[i].ptr;
							break;
						}
					}
					renderer = new CParticleRenderer( emitter );
					renderer->SetMaterial( new glMaterial );
					renderer->GetMaterial()->passinfo.push_back( glPass() );
					renderer->GetMaterial()->passinfo[0].shader = new glShader( ".res/shaders/particles/colorBlended.glsl" );
					newComponentEntry.ptr = renderer;
					mParticleObject->AddComponent( renderer );
					renderer->transform.SetParent( &(mParticleObject->transform) );
					break;
				case 5:
					// look for first emitter in the list
					emitter = NULL;
					for ( uint i = 0; i < particleComponentList.size(); ++i ) {
						if ( particleComponentList[i].type == 0 ) {
							emitter = (CParticleEmitter*)particleComponentList[i].ptr;
							break;
						}
					}
					renderer = new CParticleRenderer_Animated( emitter );
					renderer->SetMaterial( new glMaterial );
					newComponentEntry.ptr = renderer;
					mParticleObject->AddComponent( renderer );
					renderer->transform.SetParent( &(mParticleObject->transform) );
					break;
				case 6:
					// look for first updater in the list
					updater = NULL;
					for ( uint i = 0; i < particleComponentList.size(); ++i ) {
						if ( particleComponentList[i].type == 1 ) {
							updater = (CParticleUpdater*)particleComponentList[i].ptr;
							break;
						}
					}
					modifier = new CParticleMod_Spiral();
					newComponentEntry.ptr = modifier;
					updater->AddModifier( modifier );
					break;
			}
			particleComponentList.push_back( newComponentEntry );
		}
	}
}

void CParticleEditor::RecreatePropertyList ( const editorComponent_t &component )
{
	// First, clear off property view
	myGUI->ClearPropertyview( edtProperties.view );

	// Now, set the possible values
	if ( component.type == 0 ) {
		myGUI->AddPropertyOption<string>( edtProperties.view, "Component Name", &(((CGameBehavior*)component.ptr)->name) );
		myGUI->AddPropertyDivider( edtProperties.view, "Emitter Properties" );
		myGUI->AddPropertyOption<Vector3d>( edtProperties.view, "Emitter Size", &(((CParticleEmitter*)component.ptr)->vEmitterSize) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Max Particles", &(((CParticleEmitter*)component.ptr)->fMaxParticles) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Emit per sec (min)", &(((CParticleEmitter*)component.ptr)->rfParticlesSpawned.mMinVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Emit per sec (max)", &(((CParticleEmitter*)component.ptr)->rfParticlesSpawned.mMaxVal) );
		myGUI->AddPropertyOption<bool>( edtProperties.view, "One shot", &(((CParticleEmitter*)component.ptr)->bOneShot) );
		myGUI->AddPropertyOption<bool>( edtProperties.view, "Use worldspace", &(((CParticleEmitter*)component.ptr)->bSimulateInWorldspace) );
		myGUI->AddPropertyOption<bool>( edtProperties.view, "Smooth emitter", &(((CParticleEmitter*)component.ptr)->bSmoothEmitter) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "World velocity scale", &(((CParticleEmitter*)component.ptr)->fVelocityScale) );
		myGUI->AddPropertyDivider( edtProperties.view, "Motion Properties" );
		myGUI->AddPropertyOption<Vector3d>( edtProperties.view, "Velocity (min)", &(((CParticleEmitter*)component.ptr)->rvVelocity.mMinVal) );
		myGUI->AddPropertyOption<Vector3d>( edtProperties.view, "Velocity (max)", &(((CParticleEmitter*)component.ptr)->rvVelocity.mMaxVal) );
		myGUI->AddPropertyOption<Vector3d>( edtProperties.view, "Acceleration (min)", &(((CParticleEmitter*)component.ptr)->rvAcceleration.mMinVal) );
		myGUI->AddPropertyOption<Vector3d>( edtProperties.view, "Acceleration (max)", &(((CParticleEmitter*)component.ptr)->rvAcceleration.mMaxVal) );
		myGUI->AddPropertyOption<Vector3d>( edtProperties.view, "Linear Damping (min)", &(((CParticleEmitter*)component.ptr)->rvLinearDamping.mMinVal) );
		myGUI->AddPropertyOption<Vector3d>( edtProperties.view, "Linear Damping (max)", &(((CParticleEmitter*)component.ptr)->rvLinearDamping.mMaxVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Angular Velocity (min)", &(((CParticleEmitter*)component.ptr)->rfAngularVelocity.mMinVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Angular Velocity (max)", &(((CParticleEmitter*)component.ptr)->rfAngularVelocity.mMaxVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Angular Aceleration (min)", &(((CParticleEmitter*)component.ptr)->rfAngularAcceleration.mMinVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Angular Aceleration (max)", &(((CParticleEmitter*)component.ptr)->rfAngularAcceleration.mMaxVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Angular Damping (min)", &(((CParticleEmitter*)component.ptr)->rfAngularDamping.mMinVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Angular Damping (max)", &(((CParticleEmitter*)component.ptr)->rfAngularDamping.mMaxVal) );
		myGUI->AddPropertyDivider( edtProperties.view, "Particle Properties" );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Start Size (min)", &(((CParticleEmitter*)component.ptr)->rfStartSize.mMinVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Start Size (max)", &(((CParticleEmitter*)component.ptr)->rfStartSize.mMaxVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "End Size (min)", &(((CParticleEmitter*)component.ptr)->rfEndSize.mMinVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "End Size (max)", &(((CParticleEmitter*)component.ptr)->rfEndSize.mMaxVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Lifetime (min)", &(((CParticleEmitter*)component.ptr)->rfLifetime.mMinVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Lifetime (max)", &(((CParticleEmitter*)component.ptr)->rfLifetime.mMaxVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Start Angle (min)", &(((CParticleEmitter*)component.ptr)->rfStartAngle.mMinVal) );
		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Start Angle (max)", &(((CParticleEmitter*)component.ptr)->rfStartAngle.mMaxVal) );
		myGUI->AddPropertyDivider( edtProperties.view, "Particle Color" );
		for ( uint cn = 0; cn < ((CParticleEmitter*)component.ptr)->vcColors.size(); ++cn ) {
			myGUI->AddPropertyOption<Color>( edtProperties.view, "Color", &(((CParticleEmitter*)component.ptr)->vcColors[cn]) );
		}
	}
	else if ( component.type == 1 ) {
		myGUI->AddPropertyOption<string>( edtProperties.view, "Component Name", &(((CGameBehavior*)component.ptr)->name) );
		myGUI->AddPropertyOption<CParticleEmitter*>( edtProperties.view, "Target Emitter", &(((CParticleUpdater*)component.ptr)->myEmitter) );
	}
	else if ( component.type == 2 || component.type == 5 ) {
		//myGUI->AddPropertyOption<string>( edtProperties.view, "Object Name", &(((CRenderableObject*)component.ptr)->) );
		myGUI->AddPropertyOption<CParticleEmitter*>( edtProperties.view, "Target Emitter", &(((CParticleRenderer*)component.ptr)->myEmitter) );
		//myGUI->AddPropertyOption<glMaterial*>( edtProperties.view, "Material", &(((CParticleRenderer*)component.ptr)->GetPassMaterial(0)) );
		//myGUI->AddPropertyOption<glMaterial*>( edtProperties.view, "Material", (((CParticleRenderer*)component.ptr)->vM) );
		myGUI->AddPropertyOption<glMaterial>( edtProperties.view, "Material", ((CParticleRenderer*)component.ptr)->GetMaterial() );

		DuskGUI::Handle tempDropdownHandle = myGUI->CreateDropdownList( edtProperties.view );
		myGUI->AddDropdownOption( tempDropdownHandle, "Precise Billboards", CParticleRenderer::P_FLAT_BILLBOARD_PRECISE );
		myGUI->AddDropdownOption( tempDropdownHandle, "Stretched Billboards", CParticleRenderer::P_STRETCHED_BILLBOARD );
		myGUI->AddDropdownOption( tempDropdownHandle, "Trail", CParticleRenderer::P_FLAT_TRAILS );
		myGUI->AddPropertyOptionDropdown( edtProperties.view, "Particle Type", &(((CParticleRenderer*)component.ptr)->iRenderMethod), tempDropdownHandle );

		myGUI->AddPropertyOption<ftype>( edtProperties.view, "Stretched Speed Scale", &(((CParticleRenderer*)component.ptr)->fR_SpeedScale) );

		if ( component.type == 5 ) {
			myGUI->AddPropertyOption<ftype>( edtProperties.view, "Frames", &(((CParticleRenderer_Animated*)component.ptr)->iFrameCount) );
			myGUI->AddPropertyOption<ftype>( edtProperties.view, "Horizontal Divs", &(((CParticleRenderer_Animated*)component.ptr)->iHorizontalDivs) );
			myGUI->AddPropertyOption<ftype>( edtProperties.view, "Vertical Divs", &(((CParticleRenderer_Animated*)component.ptr)->iVerticalDivs) );
			myGUI->AddPropertyOption<ftype>( edtProperties.view, "Frames per second", &(((CParticleRenderer_Animated*)component.ptr)->fFramesPerSecond) );
			myGUI->AddPropertyOption<bool>( edtProperties.view, "Stretch animation?", &(((CParticleRenderer_Animated*)component.ptr)->bStretchAnimationToLifetime) );
			myGUI->AddPropertyOption<bool>( edtProperties.view, "Clamp frame count?", &(((CParticleRenderer_Animated*)component.ptr)->bClampToFrameCount) );
		}

	}
	else if ( component.type == 6 ) {
		myGUI->AddPropertyOption<Vector3d>( edtProperties.view, "Frequency", &(((CParticleMod_Spiral*)component.ptr)->m_rotaryFrequency) );
		myGUI->AddPropertyOption<Vector3d>( edtProperties.view, "Offset", &(((CParticleMod_Spiral*)component.ptr)->m_rotaryOffset) );
		myGUI->AddPropertyOption<Vector3d>( edtProperties.view, "Position Edit", &(((CParticleMod_Spiral*)component.ptr)->m_rotaryPosition) );
		myGUI->AddPropertyOption<Vector3d>( edtProperties.view, "Velocity Edit", &(((CParticleMod_Spiral*)component.ptr)->m_rotaryVelocity) );
		myGUI->AddPropertyOption<Vector3d>( edtProperties.view, "Acceleration Edit", &(((CParticleMod_Spiral*)component.ptr)->m_rotaryAcceleration) );
	}
}

void CParticleEditor::SaveSystem ( const string& fname )
{
	// First open file
	CBinaryFile outFile;
	outFile.Open( fname.c_str(), CBinaryFile::IO_WRITE );

	if ( !outFile.IsOpen() )
	{
		Debug::Console->PrintWarning( "Could not save particle system!\n" );
	}
	else
	{
		// Write header with version information
		outFile.WriteData( "<<begin,name<header>,", 21 );
		outFile.WriteUInt32( 4 );	// 4 bytes	
		outFile.WriteUInt32( 3 );	// Version 3
		outFile.WriteData( "-end>>", 6 );

		// Start up serializer
		OSerialBinary output ( &outFile, 3 );

		// Loop through emitters
		for ( uint i = 0; i < particleComponentList.size(); ++i )
		{
			if ( particleComponentList[i].type == 0 )
			{
				// Serialize emitter
				outFile.WriteUInt32( 0 );
				output << (CParticleEmitter*)(particleComponentList[i].ptr);

				// Serialize updaters
				for ( uint j = 0; j < particleComponentList.size(); ++j )
				{
					if ( particleComponentList[j].type == 1 )
					{
						if ( ((CParticleUpdater*)(particleComponentList[j].ptr))->myEmitter == (CParticleEmitter*)(particleComponentList[i].ptr) )
						{
							outFile.WriteUInt32( 1 );
							output << ((CParticleUpdater*)(particleComponentList[j].ptr));
						}
						// Serialize modifiers that use this updater
						/*for ( uint k = 0; k < particleComponentList.size(); ++k )
						{
							if ( particleComponentList[k].type == 4 ) {
								if ( ((CParticle*)(particleComponentList[k].ptr))->myEmitter == (CParticleEmitter*)(particleComponentList[i].ptr) )
							}
						}*/
						// need to loop thru updater's list
					}
				}

				// Serialize renderers
				for ( uint j = 0; j < particleComponentList.size(); ++j )
				{
					if (( particleComponentList[j].type == 2 )||( particleComponentList[j].type == 5 ))
					{
						if ( ((CParticleRenderer*)(particleComponentList[j].ptr))->myEmitter == (CParticleEmitter*)(particleComponentList[i].ptr) )
						{
							outFile.WriteUInt32( particleComponentList[j].type );
							output << (CParticleRenderer*)(particleComponentList[j].ptr);
						}
					}
				}
			}
			else if ( particleComponentList[i].type == 3 ) {
				// UEHHHH
			}
			else {
				continue;
			}
		}
	}
}

void CParticleEditor::LoadSystem ( const string& sSystemFile )
{
	uint32_t iVersion = 0;
	{
		// Input
		CSegmentedFile inFile ( sSystemFile );
		inFile.ReadData();

		// Check header for version number
		{
			// Get data
			string str;
			inFile.GetSectionData( string("header"), str );
			std::stringstream sin ( str, ios_base::binary|ios_base::in|ios_base::out );

			// Read version
			sin.read( (char*)(&iVersion), sizeof(uint32_t) );
		}
	}

	cout << "Particle system is version " << iVersion << endl;

	switch ( iVersion )
	{
	case 1:
		// Create a version 1 particle system attached to this object
		{
			CParticleEmitter* emitter = new CParticleEmitter();
			emitter->LoadFromFile( sSystemFile );
			CParticleUpdater* updater = new CParticleUpdater( emitter );
			CParticleRenderer* renderer = new CParticleRenderer( emitter );

			DeleteObject( mParticleObject );
			mParticleObject = new CExtendableGameObject;
			mParticleObject->RemoveReference();

			particleComponentList.clear();
			editorComponent_t cem, cup, cre;
			cem.ptr = emitter;
			cem.type = 0;
			cup.ptr = updater;
			cup.type = 1;
			cre.ptr = renderer;
			cre.type = 2;
			particleComponentList.push_back( cem );
			particleComponentList.push_back( cup );
			particleComponentList.push_back( cre );

			mParticleObject->AddComponent( emitter );
			mParticleObject->AddComponent( updater );
			mParticleObject->AddComponent( renderer );

			emitter->transform.SetParent( &(mParticleObject->transform) );
			renderer->transform.SetParent( &(mParticleObject->transform) );
		}
		break;
	case 2:
	case 3:
		// Create a version 2 particle system attached to this object
		{
			CBinaryFile inFile;
			inFile.Open( sSystemFile.c_str(), CBinaryFile::IO_READ );
			// First read in past the ascii header
			{
				string gaurdSearch = "";
				bool endGaurd = false;
				while ( !endGaurd ) {
					gaurdSearch += inFile.ReadChar();
					if ( string("-end>>").find( gaurdSearch ) == string::npos ) {
						gaurdSearch = "";
					}
					else if ( gaurdSearch == "-end>>" ) {
						endGaurd = true;
					}
				}
			}
			// Now that the header is read in, read in component type, and run serializer for each class
			{
				DeleteObject( mParticleObject );
				mParticleObject = new CExtendableGameObject;
				mParticleObject->RemoveReference();
				particleComponentList.clear();

				ISerialBinary deserializer ( &inFile, iVersion );

				void* newComponent = NULL;
				CParticleEmitter* lastEmitter = NULL;
				CParticleUpdater* lastUpdater = NULL;

				uint32_t currentObjType = inFile.ReadUInt32();
				while ( !feof( inFile.GetFILE() ) )
				{
					cout << "Reading in object of type " << currentObjType << endl;
					switch ( currentObjType )
					{
					case 0:	// Default emitter
						newComponent = new CParticleEmitter();
						deserializer >> ((CParticleEmitter*)(newComponent));
						mParticleObject->AddComponent( (CParticleEmitter*)(newComponent) );
						lastEmitter = (CParticleEmitter*)(newComponent);
						lastEmitter->transform.SetParent( &(mParticleObject->transform) );
						break;
					case 1: // Default updater
						newComponent = new CParticleUpdater(lastEmitter);
						deserializer >> ((CParticleUpdater*)(newComponent));
						mParticleObject->AddComponent( (CParticleUpdater*)(newComponent) );
						break;
					case 2: // Default renderer
						newComponent = new CParticleRenderer(lastEmitter);
						((CParticleRenderer*)(newComponent))->SetMaterial( new glMaterial );
						deserializer >> ((CParticleRenderer*)(newComponent));
						mParticleObject->AddComponent( (CParticleRenderer*)(newComponent) );
						((CParticleRenderer*)newComponent)->transform.SetParent( &(mParticleObject->transform) );
						break;
					case 3: // Emitter - Clouds; Skip this
						break;
					case 5: // Renderer - Animation
						newComponent = new CParticleRenderer_Animated(lastEmitter);
						((CParticleRenderer_Animated*)(newComponent))->SetMaterial( new glMaterial );
						deserializer >> ((CParticleRenderer_Animated*)(newComponent));
						mParticleObject->AddComponent( (CParticleRenderer_Animated*)(newComponent) );
						((CParticleRenderer*)newComponent)->transform.SetParent( &(mParticleObject->transform) );
						break;
					case 4: // Modifier - Wind
						newComponent = new CParticleMod_Windmotion();
						deserializer >> ((CParticleMod_Windmotion*)(newComponent));
						lastUpdater->AddModifier( ((CParticleMod_Windmotion*)(newComponent)) );
						break;
					default:
						Debug::Console->PrintError( "particle system: unrecognized component type!" );
						break;
					}
					editorComponent_t cem;
					cem.ptr = newComponent;
					cem.type = currentObjType;
					particleComponentList.push_back( cem );

					currentObjType = inFile.ReadUInt32();
				}
			}

		}
		break;
	default:
		Debug::Console->PrintError( "could not read " );
		Debug::Console->PrintError( sSystemFile.c_str() );
		Debug::Console->PrintError( ": unsupported file version or invalid\n" );
		break;
	}
}