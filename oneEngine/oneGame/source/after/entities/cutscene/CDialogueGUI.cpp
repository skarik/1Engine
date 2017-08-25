#include "CDialogueGUI.h"

#include <string>

#include "core/input/CInput.h"
#include "core/system/Screen.h"
#include "core/debug/CDebugConsole.h"

#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "core-ext/input/CInputControl.h"

using namespace std;

//.8888 is the horizonal center if going by height and 16:9 screen ratio
CDialogueGUI::CDialogueGUI (void) : CGameBehavior(), CRenderableObject()
{
	//Initialize some very important variables
	sMouseX = 0.0;
	sMouseY = 0.0;
	sChoice = -1;
	mConversation = NULL;//new CDialogueLoader ();
	bActive = false;
	
	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	//Initialize the font drawing stuff
	fntDebug	= new CBitmapFont ( "HVD_Comic_Serif_Pro.otf", 16, FW_BOLD );
	matFntDebug = new glMaterial;
	matFntDebug->m_diffuse = Color( 0.0f,0,0 );
	matFntDebug->passinfo.push_back( glPass() );
	matFntDebug->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matFntDebug->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );
	matFntDebug->passinfo[0].b_depthmask = false;

	//Initialize the other drawing stuff
	matDrawDebug = new glMaterial;
	matDrawDebug->m_diffuse = Color( 0.0f,0,0 );
	matDrawDebug->passinfo.push_back( glPass() );
	matDrawDebug->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matDrawDebug->setTexture( 0, new CTexture("null") );
	matDrawDebug->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );
	matDrawDebug->passinfo[0].b_depthmask = false;

	SetMaterial( matDrawDebug );

	rSize = Vector2d ( 0.56f, 0.05f );
	for (int i = 0; i < 9; i++) {
		rPositions.push_back (
			Vector2d ( 0.22f, 0.72f + (i*0.04f) )
			);
	}

	//xCenter = .888;

	//vBackgroundLineP = Vector2d (.3, .6);
	//vBackgroundLineS = Vector2d (1.11, .11);
	mBackgroundDialogue = Rect( 0.2f, 0.6f, 0.6f, 0.11f );

	//vBackgroundChoicesP = Vector2d (.3, .74);
	//vBackgroundChoicesS = Vector2d (1.11, .5);
	mBackgroundChoices	= Rect( 0.2f, 0.71f, 0.6f, 0.5f );

	input = new CInputControl (this);

	transform.position.z = -30.0f;
	//OpenConversation (".res/conversations/test.txt");
}

CDialogueGUI::~CDialogueGUI (void)
{
	if (mConversation) {
		delete mConversation;
	}
	mConversation = NULL;

	delete fntDebug;
	matFntDebug->removeReference();
	delete matFntDebug;
	//matDrawDebug->removeReference();
	//delete matDrawDebug; // is main material
	delete input;
}

void CDialogueGUI::Update (void)
{
	if ( bActive )
	{
		GetMouseInfo();

		if ( mConversation->sDialogueState == DIALOGUE_STATE_LUACODE )
		{
			mConversation->PostLua();
		}
		/*if (mConversation->sDialogueState == DIALOGUE_STATE_PLAYERTALKING)
		{
			//Go send morph data somewhere. 
			//Go send audio data somewhere.
			//CNpcBase/CMccPlayer::Speakfile
			//SpeakDialogue (voicteactor/race/soundname.mp3)
		}*/
		
		if ( mConversation != NULL )
		{
			// Mousedown input.
			if ( CInput::MouseDown(CInput::MBLeft) )
			{
				switch ( mConversation->sDialogueState )
				{
				case DIALOGUE_STATE_LUACODE:
					Debug::Console->PrintMessage( "Lua state. Input skipped.\n" );
					break;
				case DIALOGUE_STATE_PLAYERTALKING:
					mConversation->GoNextLine();
					break;
				case DIALOGUE_STATE_NPCTALKING:
					mConversation->GoNextLine();
					break;
				//case DIALOGUE_STATE_CHOICES:
				//	mConversation->sDialogueState = DIALOGUE_STATE_RESPONSE; // Basically waits for a click to continue.
				//	break;
				case DIALOGUE_STATE_RESPONSE:
					mConversation->ReportDecision(sChoice);
					sChoice = -1;
					choices.clear();
					break;
				default:
					Debug::Console->PrintWarning( "Unhandled dialogue option\n" );
					break;
				}
			}

			// Step code
			switch ( mConversation->sDialogueState )
			{
			case DIALOGUE_STATE_PLAYERTALKING:
			case DIALOGUE_STATE_NPCTALKING:
				mConversation->SoundAndFace();
				mBackgroundDialogue.pos.y = 0.75f;
				break;
			case DIALOGUE_STATE_CHOICES:
			case DIALOGUE_STATE_RESPONSE:
				mConversation->sDialogueState = DIALOGUE_STATE_RESPONSE; // Basically waits for a click to continue.
				mConversation->ChoiceUpdate();
				mBackgroundDialogue.pos.y = 0.55f;
				break;
			}

			if ( mConversation->sDialogueState != DIALOGUE_STATE_CHOICES && mConversation->sDialogueState != DIALOGUE_STATE_RESPONSE )
			{
				sChoice = -1;
				choices.clear();
			}
		}

		/*if (mConversation->sDialogueState == CDialogueLoader::CHOICES)
		{
			mConversation->sDialogueState = CDialogueLoader::RESPONSE;
		}*/
	}
}

void CDialogueGUI::LateUpdate (void)
{
	if (bActive)
	{
		if (mConversation->sDialogueState == DIALOGUE_STATE_ENDED)
		{
			bActive = !bActive;
			input->Release();
			delete mConversation;
			mConversation = NULL;
		}
	}
}

bool CDialogueGUI::Render (const char pass)
{
	GL_ACCESS;
	GL.beginOrtho ();

	if (bActive && mConversation != NULL)
	{
		DrawBackground();
		DrawHighlighted();
		DrawDialogue();
		DrawChoices();
	}
	//	DrawDialogue();

	GL.endOrtho ();
	
	return true;
}

void CDialogueGUI::GetMouseInfo (void)
{
	sMouseX = CInput::MouseX();
	sMouseY = CInput::MouseY();

	sMouseX /= Screen::Info.height;
	sMouseY /= Screen::Info.height;

	sChoice = -1;

	for (int i = 0; i < 9; i++)
	{
		if (sMouseX >= rPositions[i].x && sMouseX < rPositions[i].x + rSize.x)
			if (sMouseY >= rPositions[i].y && sMouseY < rPositions[i].y + rSize.y)
				sChoice = i;
	}
	//cout << sChoice << endl;
}

void CDialogueGUI::OpenConversation (string filename)
{
	mConversation = new CDialogueLoader (filename);
}

void CDialogueGUI::OpenConversation (string filename, vector<CCharacter*> characters)
{
	mConversation = new CDialogueLoader (filename.c_str(), characters);
}

void CDialogueGUI::SetActive (bool active )
{
	if ( bActive != active ) {
		if ( active ) {
			input->Capture();
		}
		else {
			input->Release();
		}
	}
	bActive = active;
}

bool CDialogueGUI::GetIsActive (void)
{
	return bActive;
}

void CDialogueGUI::DrawDialogue (void)
{
	GLd_ACCESS;
	GLd.DrawSet2DMode( GLd.D2D_FLAT );
	GLd.DrawSet2DScaleMode(GLd.SCALE_DEFAULT);
	
	//if (mConversation->sDialogueState >= DIALOGUE_STATE_PLAYERTALKING && mConversation->sDialogueState < DIALOGUE_STATE_LUACODE)
	if ( mConversation->sDialogueState == DIALOGUE_STATE_PLAYERTALKING || mConversation->sDialogueState == DIALOGUE_STATE_NPCTALKING )
	{
		string line;
		line = mConversation->GetCurrentLine();

		// Draw the line of dialgoue
		if (mConversation->sDialogueState == DIALOGUE_STATE_PLAYERTALKING) {
			matFntDebug->m_diffuse = Color (1.0, 1.0, 1.0);
		}
		else if (mConversation->sDialogueState == DIALOGUE_STATE_NPCTALKING) {
			matFntDebug->m_diffuse = Color (1.0, 1.0, 1.0);
		}
		else {
			matFntDebug->m_diffuse = Color (1.0, 1.0, 1.0);
		}
		matFntDebug->setTexture( 0, fntDebug );
		matFntDebug->bindPass(0);
		GLd.DrawAutoTextCentered ( 0.5, mBackgroundDialogue.pos.y+0.06f, line.c_str() );

		// Draw 'name' of who is talking
		if ( mConversation->sDialogueState == DIALOGUE_STATE_PLAYERTALKING ) {
			matFntDebug->m_diffuse = Color (0.6, 0.6, 0.6);
			matFntDebug->bindPass(0);
			GLd.DrawAutoText ( mBackgroundDialogue.pos.x+0.01f, mBackgroundDialogue.pos.y+0.02f, "You" );
		}
		else {
			CCharacter* target = mConversation->GetParticipant( mConversation->GetCurrentSpeaker() );
			if ( target ) {
				matFntDebug->m_diffuse = Color (0.6, 0.6, 0.6);
				matFntDebug->bindPass(0);
				GLd.DrawAutoText ( mBackgroundDialogue.pos.x+0.01f, mBackgroundDialogue.pos.y+0.02f, target->GetName() );
			}
		}
	}
}

void CDialogueGUI::DrawChoices (void)
{
	GLd_ACCESS;
	GLd.DrawSet2DMode( GLd.D2D_FLAT );
	GLd.DrawSet2DScaleMode(GLd.SCALE_DEFAULT);
	if (mConversation->sDialogueState == DIALOGUE_STATE_CHOICES || mConversation->sDialogueState == DIALOGUE_STATE_RESPONSE)
	{
		choices = mConversation->GetChoices();
		sNumChoices = choices.size();

		// Draw the timer behind the default selection
		if ( mConversation->IsTimedChoice() )
		{
			float timePercent = mConversation->CurrentChoiceTimer();
			//matDrawDebug->m_diffuse = Color (1.0, 0.0, 0.0, 1.0);
			//matDrawDebug->bindPass(0);
			//GLd.DrawRectangleA ( mBackgroundDialogue.pos.x+(mBackgroundDialogue.size.x*0.5*(1-timePercent)), mBackgroundDialogue.pos.y, mBackgroundDialogue.size.x*timePercent, 0.01f );
			short tChoice = mConversation->GetDefaultChoice();

			matDrawDebug->m_diffuse = Color (0.3, 0.0, 0.0, 0.5);
			matDrawDebug->bindPass(0);
			GLd.DrawRectangleA ( (rPositions[tChoice].x + mBackgroundChoices.pos.x)*0.5f , rPositions[tChoice].y, (rSize.x+mBackgroundChoices.size.x)*0.5f , rSize.y );

			matDrawDebug->m_diffuse = Color (0.5, 0.0, 0.0, 1.0);
			matDrawDebug->bindPass(0);
			GLd.DrawRectangleA ( mBackgroundDialogue.pos.x+(mBackgroundDialogue.size.x*0.5f*(1-timePercent)), rPositions[tChoice].y, mBackgroundDialogue.size.x*timePercent, rSize.y );
		}

		// Draw the line of dialgoue
		string line;
		line = mConversation->GetCurrentLine();
		matFntDebug->m_diffuse = Color (1.0, 1.0, 1.0);
		matFntDebug->setTexture( 0, fntDebug );
		matFntDebug->bindPass(0);
		GLd.DrawAutoTextCentered ( 0.5, mBackgroundDialogue.pos.y+0.06f, line.c_str() );

		// Draw the choices
		for (short i = 0; i < sNumChoices; i++)
		{
			if (sChoice == i) {
				matFntDebug->m_diffuse = Color (1.0f, 0.0, 0.0);
			}
			else {
				matFntDebug->m_diffuse = Color (1.0f, 1.0, 1.0);
			}
			matFntDebug->bindPass(0);
			GLd.DrawAutoTextWrapped ( rPositions[i].x, rPositions[i].y + .03f, rSize.x, choices[i].choice.c_str() );
		}
	}
}

void CDialogueGUI::DrawBackground (void)
{
	/*GLd.DrawSet2DMode( GLd.D2D_FLAT );
	GLd.DrawSet2DScaleMode(GL.SCALE_HEIGHT);

	matDrawDebug->m_diffuse = Color (0.0, 0.0, 0.0, 0.7);
	matDrawDebug->bindPass(0);
		GLd.DrawRectangleA(vBackgroundLineP.x, vBackgroundLineP.y, vBackgroundLineS.x, vBackgroundLineS.y);

	if (mConversation->sDialogueState == DIALOGUE_STATE_CHOICES || 
		mConversation->sDialogueState == DIALOGUE_STATE_RESPONSE)
	{
	matDrawDebug->m_diffuse = Color (0.0, 0.0, 0.0, 0.7);
	matDrawDebug->bindPass(0);
		GLd.DrawRectangleA(vBackgroundChoicesP.x, vBackgroundChoicesP.y, vBackgroundChoicesS.x, vBackgroundChoicesS.y);
	}*/
	GLd_ACCESS;
	GLd.DrawSet2DMode( GLd.D2D_FLAT );
	GLd.DrawSet2DScaleMode(GLd.SCALE_DEFAULT);

	if ( mConversation->sDialogueState == DIALOGUE_STATE_CHOICES || mConversation->sDialogueState == DIALOGUE_STATE_RESPONSE )
	{
		// Draw the dialogue background
		matDrawDebug->m_diffuse = Color (0.0, 0.0, 0.0, 0.7);
		matDrawDebug->bindPass(0);
		GLd.DrawRectangleA(mBackgroundDialogue.pos.x, mBackgroundDialogue.pos.y, mBackgroundDialogue.size.x, mBackgroundDialogue.size.y);

		// Draw the choices background
		GLd.DrawRectangleA(mBackgroundChoices.pos.x, mBackgroundChoices.pos.y, mBackgroundChoices.size.x, rPositions[sNumChoices-1].y-mBackgroundChoices.pos.y+0.01f+rSize.y );
	}

	if ( mConversation->sDialogueState == DIALOGUE_STATE_PLAYERTALKING || mConversation->sDialogueState == DIALOGUE_STATE_NPCTALKING )
	{
		// Draw the dialogue background
		matDrawDebug->m_diffuse = Color (0.0, 0.0, 0.0, 0.7);
		matDrawDebug->bindPass(0);
		GLd.DrawRectangleA(mBackgroundDialogue.pos.x, mBackgroundDialogue.pos.y, mBackgroundDialogue.size.x, mBackgroundDialogue.size.y);
	}
}

void CDialogueGUI::DrawHighlighted (void)
{
	GLd_ACCESS;
	GLd.DrawSet2DMode( GLd.D2D_FLAT );
	GLd.DrawSet2DScaleMode(GLd.SCALE_DEFAULT);
	
	if (sChoice >= 0 && sChoice < (short)choices.size() && choices.size() > 0)
	{
		/*matFntDebug->m_diffuse = Color (1.0f, 0.0, 0.0);
		matFntDebug->setTexture( 0, fntDebug );
		matFntDebug->bindPass(0);
			GLd.DrawAutoTextCentered ( 0.5f, 0.7f,  choices[sChoice].choice.c_str());*/
			
		matDrawDebug->m_diffuse = Color (.3, .3, .3, .4);
		matDrawDebug->bindPass(0);
		GLd.DrawRectangleA( (rPositions[sChoice].x + mBackgroundChoices.pos.x)*0.5f , rPositions[sChoice].y, (rSize.x+mBackgroundChoices.size.x)*0.5f , rSize.y );

		// Draw highlight for default choice
		if ( mConversation->IsTimedChoice() )
		{
		/*	short tChoice = mConversation->GetDefaultChoice();
			GLd.DrawSet2DMode( GL.D2D_WIRE );
			matDrawDebug->m_diffuse = Color (1.0, 1.0, 1.0, .4);
			matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( (rPositions[tChoice].x + mBackgroundChoices.pos.x)*0.5 , rPositions[tChoice].y, (rSize.x+mBackgroundChoices.size.x)*0.5 , rSize.y );*/
		}
	}
}