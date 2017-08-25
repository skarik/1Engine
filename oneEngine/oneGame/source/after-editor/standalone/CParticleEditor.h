
#ifndef _C_PARTICLE_EDITOR_H_
#define _C_PARTICLE_EDITOR_H_

#include "engine/behavior/CGameBehavior.h"

#include "engine-common/dusk/CDuskGUI.h"

#include <vector>
/*
#include "CInput.h"
#include "CDuskGUI.h"
#include "CCamera.h"
#include "CParticleEmitter.h"
#include "CParticleUpdater.h"
#include "CParticleRenderer.h"
#include "CExtendableGameObject.h"
#include "DayAndNightCycle.h"
	*/
// need a dialogue for each part of particle system
// emitter, updater, renderer

// should have a component list
// selecting a component from the listview will expose its options in the propertyview
//  a particle system then can possible have multiple emitters and renderers

class CCamera;
class Daycycle;
class CLight;
class CModel;
class CExtendableGameObject;
 
class CParticleEditor : public CGameBehavior
{
	ClassName( "CParticleEditor" );
public:
	explicit CParticleEditor ( bool ingame=false );
	~CParticleEditor ( void );

	// Update
	void Update ( void );

private:
	CDuskGUI*	myGUI;
	CCamera*	myCamera;
	bool		bCameraActive;

	ftype		cameraDistance;
	Vector3d	vCameraCenter;
	Vector3d	vCameraAngles;

	void LoadDefaults ( void );

	void SaveSystem ( const string& );
	void LoadSystem ( const string& );

	Color		cBackgroundColor;
	Daycycle*	mDaycycle;
	CLight*		mLight;
	CModel*		mBgSphere;


	CExtendableGameObject* mParticleObject;

	// GUI Handles
	void CreateGUI ( void );

	typedef CDuskGUI::Handle GUIHandle;
	void DoGUIWork ( void );

	struct editorComponent_t
	{
		int type;
		void* ptr;
	};
	std::vector<editorComponent_t> particleComponentList;
	int	currentEdited;

	struct editorComponentList_t
	{
		GUIHandle btnDelete;
		GUIHandle btnEdit;
		GUIHandle list;

		GUIHandle btnSave;
		GUIHandle btnLoad;

		GUIHandle saveDlg;
		GUIHandle openDlg;
	} edtComponentList;

	struct editorNewComponents_t
	{
		GUIHandle btnNew;
		GUIHandle list;
	} edtNewComponents;

	struct editorProperties_t
	{
		GUIHandle view;
	} edtProperties;

	void RecreatePropertyList ( const editorComponent_t & );
};

#endif