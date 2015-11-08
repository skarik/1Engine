#include "CSkillTreeGUI.h"
#include "core/input/CInput.h"
#include "core/system/Screen.h"
#include "core/math/Rect.h"

#include "core/math/Math.h"
#include "core/time/time.h"

#include "after/states/skilltree/CSkillTree.h"

#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CSkillTreeGUI::CSkillTreeGUI(CAfterPlayer* p_Player, CSkillTree* p_SkillTree, CSkillTree* p_RaceTree, CPlayerInventoryGUI* p_inventoryGUI): CGameBehavior(), CRenderableObject()
{	
	bDrawSkillGUI = false;

	pPlayer = p_Player;
	pSkillTree = p_SkillTree;
	pRaceTree  = p_RaceTree;
	pInventory = p_inventoryGUI;

	texNull = new CTexture( "null" );
	texSkillblob = new CTexture( ".res/textures/hud/skillblob_circlet.png" );
	texSkillline = new CTexture( ".res/textures/hud/skillblob_line.png" );

	testMat = new glMaterial;
	testMat->m_diffuse = Color(0.0f,0,0);
	testMat->setTexture( 0, texNull );
	testMat->passinfo.push_back( glPass() );
	testMat->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	testMat->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	testMat->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	testFnt = new glMaterial;
	testFnt->m_diffuse= Color(0.0f,0,0);
	testFnt->setTexture( 0, texNull );
	testFnt->passinfo.push_back( glPass() );
	testFnt->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	testFnt->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	testFnt->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	fntSkilltitle	= new CBitmapFont( "HVD_Comic_Serif_Pro.otf", 33 );
	fntSkillnames	= new CBitmapFont( "HVD_Comic_Serif_Pro.otf", 17 );
	//fntSkilldesc	= new CBitmapFont( "YanoneKaffeesatz-B.otf", 16 );
	fntSkilldesc	= new CBitmapFont( "ComicNeue-Angular-Bold.ttf", 16 );

	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	SetMaterial( testMat );

	visible = true;
	bDrawSkillGUI = false;

	m_mouseoverNode = NULL;
	m_mouseoverButton = -1;
	m_viewposition = Vector2d(200,200);
	m_viewzoom = 4;
	m_viewzoom_state.lerp = 1;
	m_viewzoom_state.next = 1;
	m_viewzoom_state.prev = 1;
	m_dragging = false;

	bSetSkillPoint = false;
	skillPoint = 10;

	m_showing_race_tree = false;

	m_previous_viewzoom	= m_viewzoom;
	m_previous_viewposition = m_viewposition;
	m_previous_viewzoom_state = m_viewzoom_state;
}

CSkillTreeGUI::~CSkillTreeGUI()
{
	//delete testMat; // is set as main material
	delete testFnt;
	delete fntSkillnames;
	delete fntSkilldesc;
	delete fntSkilltitle;

	delete texNull;
	delete texSkillblob;
	delete texSkillline;
}

bool CSkillTreeGUI::Render(const char pass)
{
	GL_ACCESS;
	GLd_ACCESS;
	GL.beginOrtho();

	const ftype t_zoom = 1/m_viewzoom;
	ftype t_prev;
	if ( bDrawSkillGUI )
	{
		Vector2d cursor_position( CInput::MouseX() / (ftype)Screen::Info.height, CInput::MouseY() / (ftype)Screen::Info.height );
		skillNode* currentNode;
		//Creates a grey background for the inventory over the rendered world.
		/*GL.DrawSet2DMode( GL.D2D_FLAT );
		GL.DrawSet2DScaleMode(GL.SCALE_HEIGHT);
		testMat->m_diffuse = Color (.35f, .35, .35, 1.0);
		testMat->bindPass(0);
			GL.DrawRectangleA( 0, 0, 1.0, 1.0);*/
		GLd.DrawSet2DScaleMode(GLd.SCALE_HEIGHT);
		GLd.DrawSet2DMode(GLd.D2D_FLAT);

		// Draw a solid box for the skill tree bg.
		testMat->setTexture( 0, texNull );
		testMat->m_diffuse = pSkillTree->treeBackground;
		testMat->bindPass(0);
			GLd.DrawRectangleA( 0,0,2,2 ); // Cover entire screen.
			//GL.DrawRectangleA(.2, .15, 1.2, 1);

		// Draw the lines between the nodes
		testMat->setTexture( 0, texSkillline );
		testMat->m_diffuse = Color( 1.0f, 1.0f, 1.0f, 1.0);
		testMat->bindPass(0);
		//glLineWidth(2);
		//GL.BeginPrimitive( GL_LINES );
		GLd.BeginPrimitive( GL_TRIANGLES );
		for ( auto node = pSkillTree->nodes.begin(); node != pSkillTree->nodes.end(); ++node )
		{
			currentNode = *node;

			// Draw lines to the parents
			skillNode* parentNode;
			//for ( auto pnode = currentNode->parentSkill.begin(); pnode != currentNode->parentSkill.end(); ++pnode )
			for ( uint pnode = 0; pnode < currentNode->parentSkill.size(); ++pnode )
			{
				parentNode = currentNode->parentSkill[pnode];
				if ( parentNode )
				{
					Vector2d parentPosition = parentNode->skillPos.pos + parentNode->skillPos.size*0.5f;
					Vector2d targetPosition = currentNode->skillPos.pos + currentNode->skillPos.size*0.5f;

					parentPosition += m_viewposition/720.0f;
					targetPosition += m_viewposition/720.0f;

					parentPosition *= (Real)Screen::Info.height;
					targetPosition *= (Real)Screen::Info.height;

					// Create the offset for drawing the quad
					Vector2d offset = (parentPosition-targetPosition).normal();
					{
						Real tmp = offset.x;
						offset.x = -offset.y;
						offset.y = tmp;
					}
					offset *= 10/720.0f * t_zoom * 680;

					if ( currentNode->parentSkillProxy[pnode] ) {
						Vector2d t_targetPos;
						t_targetPos = parentPosition + ( (targetPosition-parentPosition).normal()*parentNode->skillPos.size.y*Screen::Info.height*2.0f );
						//GL.P_AddVertex( parentPosition*t_zoom );
						//GL.P_AddVertex( t_targetPos*t_zoom );
						{
							GLd.P_PushColor( 0.5,0.5,0.5,0 );
							GLd.P_PushTexcoord( 1,0 );
							GLd.P_AddVertex( t_targetPos*t_zoom + offset );
							GLd.P_PushColor( 0.5,0.5,0.5,1 );
							GLd.P_PushTexcoord( 0,0 );
							GLd.P_AddVertex( parentPosition*t_zoom + offset );
							GLd.P_PushTexcoord( 0,1 );
							GLd.P_AddVertex( parentPosition*t_zoom - offset );

							GLd.P_PushTexcoord( 0,1 );
							GLd.P_AddVertex( parentPosition*t_zoom - offset );
							GLd.P_PushColor( 0.5,0.5,0.5,0 );
							GLd.P_PushTexcoord( 1,1 );
							GLd.P_AddVertex( t_targetPos*t_zoom - offset );
							GLd.P_PushTexcoord( 1,0 );
							GLd.P_AddVertex( t_targetPos*t_zoom + offset );
						}
						parentPosition = targetPosition + ( (parentPosition-targetPosition).normal()*parentNode->skillPos.size.y*Screen::Info.height*2.0f );
						{
							GLd.P_PushColor( 0.5,0.5,0.5,1 );
							GLd.P_PushTexcoord( 1,0 );
							GLd.P_AddVertex( targetPosition*t_zoom + offset );
							GLd.P_PushColor( 0.5,0.5,0.5,0 );
							GLd.P_PushTexcoord( 0,0 );
							GLd.P_AddVertex( parentPosition*t_zoom + offset );
							GLd.P_PushTexcoord( 0,1 );
							GLd.P_AddVertex( parentPosition*t_zoom - offset );

							GLd.P_PushTexcoord( 0,1 );
							GLd.P_AddVertex( parentPosition*t_zoom - offset );
							GLd.P_PushColor( 0.5,0.5,0.5,1 );
							GLd.P_PushTexcoord( 1,1 );
							GLd.P_AddVertex( targetPosition*t_zoom - offset );
							GLd.P_PushTexcoord( 1,0 );
							GLd.P_AddVertex( targetPosition*t_zoom + offset );
						}
					}
					else {
						Real onC = 1.0f;
						if ( currentNode->currentSkill->m_level && parentNode->currentSkill->m_level ) {
							onC = 1.7f;
						}
						if ( currentNode->currentSkill->m_level ) {
							GLd.P_PushColor( onC,onC,onC,1.0 );
						}
						else {
							GLd.P_PushColor( 0.25,0.25,0.25,1.0 );
						}
						GLd.P_PushTexcoord( 1,0 );
						GLd.P_AddVertex( targetPosition*t_zoom + offset );
						if ( parentNode->currentSkill->m_level ) {
							GLd.P_PushColor( onC,onC,onC,1.0 );
						}
						else {
							GLd.P_PushColor( 0.25,0.25,0.25,1.0 );
						}
						GLd.P_PushTexcoord( 0,0 );
						GLd.P_AddVertex( parentPosition*t_zoom + offset );
						GLd.P_PushTexcoord( 0,1 );
						GLd.P_AddVertex( parentPosition*t_zoom - offset );

						GLd.P_PushTexcoord( 0,1 );
						GLd.P_AddVertex( parentPosition*t_zoom - offset );
						if ( currentNode->currentSkill->m_level ) {
							GLd.P_PushColor( onC,onC,onC,1.0 );
						}
						else {
							GLd.P_PushColor( 0.25,0.25,0.25,1.0 );
						}
						GLd.P_PushTexcoord( 1,1 );
						GLd.P_AddVertex( targetPosition*t_zoom - offset );
						GLd.P_PushTexcoord( 1,0 );
						GLd.P_AddVertex( targetPosition*t_zoom + offset );
					}

					//GL.P_AddVertex( parentPosition*t_zoom );
					//GL.P_AddVertex( targetPosition*t_zoom );

				}
			}
		}
		GLd.EndPrimitive();
		testMat->setTexture( 0, texNull );
		//glLineWidth(1);

		// Draw the titles
		testFnt->m_diffuse = Color(0.2,0.1,0.1,1.0);
		testFnt->setTexture(0,fntSkillnames);
		testFnt->bindPass(0);
		t_prev = Screen::Info.scale;
		Screen::Info.scale *= t_zoom;
		for ( auto element = pSkillTree->elements.begin(); element != pSkillTree->elements.end(); ++element )
		{
			GLd.DrawAutoText(
				(element->position.x + m_viewposition.x/720.0f)*t_zoom,
				(element->position.y + m_viewposition.y/720.0f)*t_zoom,
				element->content.c_str() );
		}
		Screen::Info.scale = t_prev;

		// Go through all the nodes and draw them
		for ( auto node = pSkillTree->nodes.begin(); node != pSkillTree->nodes.end(); ++node )
		{
			currentNode = *node;

			CTexture* icon = currentNode->icon;
			testFnt->m_diffuse = Color(1.0,1.0,1.0,1.0);
			testFnt->setTexture(0,icon);
			testFnt->bindPass(0);
			/*GL.DrawRectangleA(
				(currentNode->skillPos.pos.x + m_viewposition.x/720.0f)*t_zoom, (currentNode->skillPos.pos.y + m_viewposition.y/720.0f)*t_zoom,
				currentNode->skillPos.size.x * t_zoom, currentNode->skillPos.size.y * t_zoom);*/
			//GL.BeginPrimitive( GL_TRIANGLE_STRIP );
			Real radius = (currentNode->skillPos.size.x * t_zoom) * 0.5f * Screen::Info.height;
			Vector2d center = (currentNode->skillPos.pos + (m_viewposition/720.0f) + currentNode->skillPos.size*0.5f)*t_zoom * Screen::Info.height;
			DrawSkillNode( radius, center, currentNode->currentSkill->m_level, currentNode->parentSkill.empty() );
		}

		// Go through all the nodes and draw their names
		testFnt->m_diffuse = Color(0.3,0.3,0.3,1.0);
		testFnt->setTexture(0,fntSkillnames);
		testFnt->bindPass(0);
		t_prev = Screen::Info.scale;
		Screen::Info.scale *= t_zoom;
		for ( auto node = pSkillTree->nodes.begin(); node != pSkillTree->nodes.end(); ++node )
		{
			currentNode = *node;
			//Real width = GL.GetAutoTextWidth( currentNode->currentSkill->GetName().c_str() );
			GLd.DrawAutoTextCentered(
				(currentNode->skillPos.pos.x + m_viewposition.x/720.0f + currentNode->skillPos.size.x*0.5f)*t_zoom,
				(currentNode->skillPos.pos.y + m_viewposition.y/720.0f - currentNode->skillPos.size.y*0.2f)*t_zoom,
				currentNode->currentSkill->GetName().c_str() );
			/*GL.DrawAutoText(
				(currentNode->skillPos.pos.x + m_viewposition.x/720.0f + currentNode->skillPos.size.x*1.05f)*t_zoom,
				(currentNode->skillPos.pos.y + m_viewposition.y/720.0f + currentNode->skillPos.size.y*0.45f)*t_zoom,
				currentNode->currentSkill->GetName().c_str() );*/
				//currentNode->currentSkill->GetItemName() );
		}
		Screen::Info.scale = t_prev;

		// Draw specific tooltip for the mouse-over node
		if ( m_mouseoverNode )
		{
			currentNode = m_mouseoverNode;

			// First, draw any proxy requirements (any parent skills likely not in view)
			skillNode* parentNode;
			for ( uint pnode = 0; pnode < currentNode->parentSkill.size(); ++pnode )
			{
				if ( currentNode->parentSkillProxy[pnode] )
				{
					parentNode = currentNode->parentSkill[pnode];
					// Create a position for the skill
					Rect t_drawRect = parentNode->skillPos;
					t_drawRect.pos = currentNode->skillPos.pos + ( (t_drawRect.pos-currentNode->skillPos.pos).normal()*t_drawRect.size.y*2 );

					CTexture* icon = parentNode->icon;
					//testFnt->m_diffuse = Color(1.0,1.0,1.0,1.0);
					testFnt->setTexture(0,icon);
					//testFnt->bindPass(0);
					/*GL.DrawRectangleA(
						(t_drawRect.pos.x + m_viewposition.x/720.0f)*t_zoom, (t_drawRect.pos.y + m_viewposition.y/720.0f)*t_zoom,
						t_drawRect.size.x * t_zoom, t_drawRect.size.y * t_zoom);*/
					Real radius = (t_drawRect.size.x * t_zoom) * 0.5f * Screen::Info.height;
					Vector2d center = (t_drawRect.pos + (m_viewposition/720.0f) + t_drawRect.size*0.5f)*t_zoom * Screen::Info.height;
					DrawSkillNode( radius * 0.7f, center, parentNode->currentSkill->m_level, false );
				}
			}
			// Go through all the proxy parents and draw their names
			testFnt->m_diffuse = Color(0.2,0.1,0.1,1.0);
			testFnt->setTexture(0,fntSkillnames);
			testFnt->bindPass(0);
			t_prev = Screen::Info.scale;
			Screen::Info.scale *= t_zoom;
			for ( uint pnode = 0; pnode < currentNode->parentSkill.size(); ++pnode )
			{
				if ( currentNode->parentSkillProxy[pnode] )
				{
					parentNode = currentNode->parentSkill[pnode];
					// Create a position for the skill
					Rect t_drawRect = parentNode->skillPos;
					t_drawRect.pos = currentNode->skillPos.pos + ( (t_drawRect.pos-currentNode->skillPos.pos).normal()*t_drawRect.size.y*2 );
					
					/*GL.DrawAutoText(
						(t_drawRect.pos.x + m_viewposition.x/720.0f + t_drawRect.size.x*1.05f)*t_zoom,
						(t_drawRect.pos.y + m_viewposition.y/720.0f + t_drawRect.size.y*0.45f)*t_zoom,
						parentNode->currentSkill->GetName().c_str() );*/
					GLd.DrawAutoTextCentered(
						(t_drawRect.pos.x + m_viewposition.x/720.0f + t_drawRect.size.x*0.5f)*t_zoom,
						(t_drawRect.pos.y + m_viewposition.y/720.0f - t_drawRect.size.y*0.12f)*t_zoom,
						parentNode->currentSkill->GetName().c_str() );
				}
			}
			Screen::Info.scale = t_prev;

			// Calculate mouse over position (does not scale with the view)
			//Vector2d t_cornerPos ((currentNode->skillPos.pos.x + m_viewposition.x/720.0f + currentNode->skillPos.size.x)*t_zoom, (currentNode->skillPos.pos.y + m_viewposition.y/720.0f)*t_zoom);
			Vector2d t_bgSize ( 200,100 );
			t_bgSize /= 720.0f;
			Vector2d t_cornerPos ((currentNode->skillPos.pos.x + m_viewposition.x/720.0f + currentNode->skillPos.size.x*0.5f)*t_zoom - t_bgSize.x*0.5f, (currentNode->skillPos.pos.y + m_viewposition.y/720.0f + currentNode->skillPos.size.y*1.3f)*t_zoom);

			// Draw a square for the mouse-over guy
			testMat->m_diffuse = Color( 0.8,0.8,0.8, 0.6);
			testMat->bindPass(0);
			GLd.DrawRectangleA( t_cornerPos.x, t_cornerPos.y, t_bgSize.x,t_bgSize.y );

			// Draw the skill name again
			testFnt->m_diffuse = Color( 0.2,0.1,0.1,1.0 );
			testFnt->setTexture( 0, fntSkillnames );
			testFnt->bindPass(0);
			GLd.DrawAutoTextCentered(
				(currentNode->skillPos.pos.x + m_viewposition.x/720.0f + currentNode->skillPos.size.x*0.5f)*t_zoom,
				(currentNode->skillPos.pos.y + m_viewposition.y/720.0f - currentNode->skillPos.size.y*0.2f)*t_zoom,
				currentNode->currentSkill->GetName().c_str()
				);

			// Draw the skill description
			testFnt->m_diffuse = Color( 0.1,0.1,0.1,1.0 );
			testFnt->setTexture( 0, fntSkilldesc );
			testFnt->bindPass(0);
			if ( currentNode->currentSkill->m_level ) {
				GLd.DrawAutoText( t_cornerPos.x + 3/720.0f, t_cornerPos.y + 16/720.0f, "Level %d", currentNode->currentSkill->m_level );
			}
			else {
				GLd.DrawAutoText( t_cornerPos.x + 3/720.0f, t_cornerPos.y + 16/720.0f, "No points allocated", currentNode->currentSkill->m_level );
			}
			GLd.DrawAutoTextWrapped( t_cornerPos.x + 3/720.0f, t_cornerPos.y + 32/720.0f, t_bgSize.x - 6/720.0f, currentNode->currentSkill->GetDescription().c_str() );
			//GL.DrawAutoTextWrapped( t_cornerPos.x + 3/720.0f, t_cornerPos.y + 16/720.0f, t_bgSize.x - 6/720.0f, currentNode->currentSkill->GetDescription().c_str() );

			// Draw "Add Point" tooltip if adding points
			if ( bSetSkillPoint )
			{
				// Draw a square for the mouse-over guy
				testMat->m_diffuse = Color( 0.1,0.1,0.1, 0.6);
				testMat->bindPass(0);
				GLd.DrawRectangleA( cursor_position.x + 2/720.0f, cursor_position.y - 18/720.0f, 0.1f,16/720.0f );

				testFnt->m_diffuse = Color( 1.0,1.0,1.0,1.0 );
				testFnt->setTexture( 0, fntSkilldesc );
				testFnt->bindPass(0);
				GLd.DrawAutoText( cursor_position.x + 3/720.0f, cursor_position.y - 4/720.0f, "Add skill point" );
			}
		}

		// Draw the zoom on the right
		testMat->m_diffuse = Color( 1,1,1,0.6 );
		testMat->bindPass(0);
		GLd.DrawSet2DMode(GLd.D2D_WIRE);
			GLd.DrawRectangleA( Screen::Info.aspect - 0.05f, 0.2f, 0.015f,0.6f ); 
		GLd.DrawSet2DMode(GLd.D2D_FLAT);
			GLd.DrawRectangleA( Screen::Info.aspect - 0.05f - 0.02f, 0.2f-0.01f + 0.6f*((m_viewzoom-1)/3), 0.015f + 0.04f,0.02f ); 

		testFnt->m_diffuse = Color( 0.3,0.3,0.3,0.7 );
		testFnt->setTexture( 0, fntSkilldesc );
		testFnt->bindPass(0);
			GLd.DrawAutoTextCentered( Screen::Info.aspect - 0.05f + 0.015f/2,0.2f-0.015f, "Zoom" );
			GLd.DrawAutoTextCentered( Screen::Info.aspect - 0.05f + 0.015f/2,0.2f-0.01f + 0.6f*((m_viewzoom-1)/3) + 0.02f - 1/720.0f, "1/%.0f", m_viewzoom );

		// Draw the title
		testMat->m_diffuse = Color( 0.1,0.1,0.1, 0.6);
		testMat->bindPass(0);
			GLd.DrawRectangleA( 0, 0, Screen::Info.aspect, 0.07f );
		testFnt->m_diffuse = Color( 1.0,1.0,1.0,1.0 );
		testFnt->setTexture( 0, fntSkilltitle );
		testFnt->bindPass(0);
		GLd.DrawAutoTextCentered( Screen::Info.aspect*0.5f,0.06f,pSkillTree->treeName.c_str() );
		/*if ( !m_showing_race_tree ) {
		}
		else {
			GL.DrawAutoTextCentered( Screen::Info.aspect*0.5f,0.06f,"Racial Skill Tree" );
		}*/

		// Draw state header at the top
		if ( bSetSkillPoint ) {
			testMat->m_diffuse = Color( 0.1,0.1,0.1, 0.6);
			testMat->bindPass(0);
			Real t_width = GLd.GetAutoTextWidth( "Allocating Skill Points" ) / Screen::Info.width;
			//GL.DrawRectangleA( Screen::Info.aspect*0.5f - t_width*0.55f, 0.85f, t_width*1.1f, 18/720.0f );
			GLd.DrawRectangleA( 0, 0.10f, Screen::Info.aspect, 18/720.0f );

			testFnt->m_diffuse = Color( 1.0,1.0,1.0,1.0 );
			testFnt->setTexture( 0, fntSkillnames );
			testFnt->bindPass(0);
			//GL.DrawAutoTextCentered( Screen::Info.aspect*0.5f, 0.85f + 16/720.0f, "Allocating Skill Points" );
			GLd.DrawAutoTextCentered( Screen::Info.aspect*0.5f, 0.10f + 16/720.0f, "Allocating Skill Points" );
		}

		// Draw the buttons
		// Tree toggle
		testMat->m_diffuse = pSkillTree->treeBackground*0.7f;
		testMat->m_diffuse.alpha = 0.8f;
		testMat->bindPass(0);
			GLd.DrawRectangleA( 0.25f,0.07f,0.18f,.05f );
		testFnt->m_diffuse = Color(0.2,0.1,0.1,1.0);
		testFnt->setTexture(0,fntSkillnames);
		testFnt->bindPass(0);
		if ( !m_showing_race_tree ) {
			GLd.DrawAutoText( 0.26f,0.07f+0.04f, "Racial Tree" );
		}
		else {
			GLd.DrawAutoText( 0.26f,0.07f+0.04f, "General Tree" );
		}

		testMat->m_diffuse = pSkillTree->treeBackground*0.7f;
		testMat->m_diffuse.alpha = 0.8f;
		testMat->bindPass(0);
			GLd.DrawRectangleA( 0.45f,0.07f,0.18f,0.05f );
		testFnt->m_diffuse = Color(0.2,0.1,0.1,1.0);
		testFnt->setTexture(0,fntSkillnames);
		testFnt->bindPass(0);
			GLd.DrawAutoText( 0.46f,0.07f+0.04f, "%d skill points", this->skillPoint );
	}
	GL.endOrtho();

	// At the end, render the hotbar
	pInventory->Render(0);

	return true;
}

void CSkillTreeGUI::DrawSkillNode ( const Real radius, const Vector2d center, const int levels, const bool source )
{
	GLd_ACCESS;
	if ( levels ) {
		testFnt->m_diffuse = Color(1.0,1.0,1.0,1.0);
	}
	else {
		testFnt->m_diffuse = Color(0.5f,0.5f,0.5f,1.0);
	}
	testFnt->bindPass(0);
	GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
	uint i;
	for ( i = 0; i <= 18; ++i )
	{
		GLd.P_PushTexcoord( 0.5f+cos((i*2*PI)/18)*0.55f, 0.5f+sin((i*2*PI)/18)*0.55f );
		GLd.P_AddVertex( center.x+cos((i*2*PI)/18)*radius, center.y+sin((i*2*PI)/18)*radius );
		GLd.P_PushTexcoord( 0.5f, 0.5f );
		GLd.P_AddVertex( center.x, center.y );
	}
	GLd.P_PushTexcoord( 0.5f+cos((i*2*PI)/18)*0.55f, 0.5f+sin((i*2*PI)/18)*0.55f );
	GLd.P_AddVertex( center.x+cos((i*2*PI)/18)*radius, center.y+sin((i*2*PI)/18)*radius );
	GLd.EndPrimitive();

	testFnt->setTexture(0,texSkillblob);
	testFnt->bindPass(0);
	GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
	GLd.P_PushTexcoord( 0,0 );
	GLd.P_AddVertex( center.x - radius*1.4, center.y - radius*1.4 );
	GLd.P_PushTexcoord( 0,1 );
	GLd.P_AddVertex( center.x - radius*1.4, center.y + radius*1.4 );
	GLd.P_PushTexcoord( 1,0 );
	GLd.P_AddVertex( center.x + radius*1.4, center.y - radius*1.4 );
	GLd.P_PushTexcoord( 1,1 );
	GLd.P_AddVertex( center.x + radius*1.4, center.y + radius*1.4 );
	GLd.EndPrimitive();
	if ( source ) 
	{
		GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
		GLd.P_PushTexcoord( 0,0 );
		GLd.P_AddVertex( center.x - radius*2.0, center.y - radius*2.0 );
		GLd.P_PushTexcoord( 0,1 );
		GLd.P_AddVertex( center.x - radius*2.0, center.y + radius*2.0 );
		GLd.P_PushTexcoord( 1,0 );
		GLd.P_AddVertex( center.x + radius*2.0, center.y - radius*2.0 );
		GLd.P_PushTexcoord( 1,1 );
		GLd.P_AddVertex( center.x + radius*2.0, center.y + radius*2.0 );
		GLd.EndPrimitive();
	}

	testFnt->m_diffuse = Color(1.0,1.0,1.0,1.0);
	testFnt->setTexture(0,texNull);
	testFnt->bindPass(0);
	for ( uint l = 0; l < levels; ++l )
	{
		Real radi = radius*1.1f + (l * radius*0.05f);
		GLd.BeginPrimitive( GL_LINE_STRIP );
		for ( i = 0; i <= 18; ++i )
		{
			GLd.P_AddVertex( center.x+cos((i*2*PI)/18)*radi, center.y+sin((i*2*PI)/18)*radi );
		}
		GLd.P_AddVertex( center.x+cos((i*2*PI)/18)*radi, center.y+sin((i*2*PI)/18)*radi );
		GLd.EndPrimitive();
	}
}

void CSkillTreeGUI::Click(void)
{
	if ( m_mouseoverNode )
	{
		if ( !bSetSkillPoint )
		{
			// Only grab skill if points put into it
			if ( m_mouseoverNode->currentSkill->m_level > 0 && !m_mouseoverNode->currentSkill->GetIsPassive() ) 
			{
				CSkill* duplicate;
				duplicate = (CSkill*)m_mouseoverNode->currentSkill->Instantiate();
				duplicate->SetOwner(pPlayer);
				pInventory->ReceiveSkill(duplicate);
			}
		}
		else
		{
			UpgradeClick();
		}
	}
	else if ( m_mouseoverButton == 1 )
	{
		bSetSkillPoint = !bSetSkillPoint;
	}
	else if ( m_mouseoverButton == 2 )
	{
		m_showing_race_tree = !m_showing_race_tree;
		std::swap( m_viewzoom, m_previous_viewzoom );
		std::swap( m_viewzoom_state, m_previous_viewzoom_state );
		std::swap( m_viewposition, m_previous_viewposition );
		std::swap( pSkillTree, pRaceTree );
	}
	else
	{
		m_dragging = true;
	}
}

void CSkillTreeGUI::UpgradeClick(void)
{
	if ( this->skillPoint <= 0 )
	{
		// No skill points to allocate
		return;
	}
	else
	{
		//m_mouseoverNode->currentSkill->SetLevel((m_mouseoverNode->currentSkill->GetLevel()) + 1);
		m_mouseoverNode->currentSkill->m_level += 1;
		this->skillPoint -= 1;

		SyncLevels();
	}
}

void CSkillTreeGUI::SyncLevels ( void )
{
	//((CMccPlayer*)pPlayer)->UpdateSkillReferences
	CPlayerInventory* playerInventory = (CPlayerInventory*)pPlayer->GetInventory();
	if ( playerInventory )
	{
		// Loop through inventory for any skills. If have skill, match level to our level
		CWeaponItem** hotbar = playerInventory->GetHotbar();
		for ( uint i = 0; i < 6; ++i )
		{
			if ( hotbar[i] && hotbar[i]->GetIsSkill() )
			{
				// Find a node with an equal ID
				for ( uint n = 0; n < pSkillTree->nodes.size(); ++n )
				{
					if ( pSkillTree->nodes[n]->currentSkill->GetID() == hotbar[i]->GetID() )
					{
						// Set level of skill to the skilltree's level
						((CSkill*)hotbar[i])->SetLevel( pSkillTree->nodes[n]->currentSkill->m_level );
						break;
					}
				}
				// Find a node with an equal ID
				for ( uint n = 0; n < pRaceTree->nodes.size(); ++n )
				{
					if ( pRaceTree->nodes[n]->currentSkill->GetID() == hotbar[i]->GetID() )
					{
						// Set level of skill to the skilltree's level
						((CSkill*)hotbar[i])->SetLevel( pRaceTree->nodes[n]->currentSkill->m_level );
						break;
					}
				}
			}
		}
		CWeaponItem** inventory = playerInventory->GetItems();
		for ( uint i = 0; i < playerInventory->GetWidth()*playerInventory->GetHeight(); ++i )
		{
			if ( inventory[i] && inventory[i]->GetIsSkill() )
			{
				// Find a node with an equal ID
				for ( uint n = 0; n < pSkillTree->nodes.size(); ++n )
				{
					if ( pSkillTree->nodes[n]->currentSkill->GetID() == inventory[i]->GetID() )
					{
						// Set level of skill to the skilltree's level
						((CSkill*)inventory[i])->SetLevel( pSkillTree->nodes[n]->currentSkill->m_level );
						break;
					}
				}
				// Find a node with an equal ID
				for ( uint n = 0; n < pRaceTree->nodes.size(); ++n )
				{
					if ( pRaceTree->nodes[n]->currentSkill->GetID() == inventory[i]->GetID() )
					{
						// Set level of skill to the skilltree's level
						((CSkill*)inventory[i])->SetLevel( pRaceTree->nodes[n]->currentSkill->m_level );
						break;
					}
				}
			}
		}
		//
	}
	// Now loop through both the trees and mark them as passive ready
	for ( uint n = 0; n < pSkillTree->nodes.size(); ++n )
	{
		if ( pSkillTree->nodes[n]->currentSkill->m_level )
		{
			pSkillTree->nodes[n]->currentSkill->DoPassiveSet( pPlayer );
		}
	}
	// Find a node with an equal ID
	for ( uint n = 0; n < pRaceTree->nodes.size(); ++n )
	{
		if ( pRaceTree->nodes[n]->currentSkill->m_level )
		{
			pRaceTree->nodes[n]->currentSkill->DoPassiveSet( pPlayer );
		}
	}
}

void CSkillTreeGUI::SetVisibility(bool visibility)
{
	bDrawSkillGUI = visibility;
}

void CSkillTreeGUI::Update(void)
{
	const ftype t_zoom = 1/m_viewzoom;
	if ( bDrawSkillGUI )
	{
		if ( !m_dragging )
		{
			// Do zoom-in and zoom-out control
			{
				if ( CInput::MouseDown(CInput::MBMiddle) )
				{
					m_viewzoom_state.lerp = 0;
					m_viewzoom_state.prev = m_viewzoom;
					m_viewzoom_state.next = 1.0f;
				}
				else
				{
					if ( CInput::DeltaMouseW() > 0 ) {
						//m_viewzoom_state.next = std::max<ftype>( 1.0f, m_viewzoom_state.next - 0.25f );
						m_viewzoom_state.next = std::max<ftype>( 1.0f, m_viewzoom_state.next - 0.5f );
						m_viewzoom_state.lerp = 0;
						m_viewzoom_state.prev = m_viewzoom;
					}
					else if ( CInput::DeltaMouseW() < 0 ) {
						//m_viewzoom_state.next = std::min<ftype>( 4.0f, m_viewzoom_state.next + 0.25f );
						m_viewzoom_state.next = std::min<ftype>( 4.0f, m_viewzoom_state.next + 0.5f );
						m_viewzoom_state.lerp = 0;
						m_viewzoom_state.prev = m_viewzoom;
					}
				}
				// Interpolate to the next zoom state
				if ( m_viewzoom_state.lerp < 1 ) {
					m_viewzoom_state.lerp += Time::deltaTime*6.0f;
					if ( m_viewzoom_state.lerp > 1 ) {
						m_viewzoom_state.lerp = 1;
					}
					m_viewzoom = Math.Smoothlerp( m_viewzoom_state.lerp, m_viewzoom_state.prev, m_viewzoom_state.next );
					// Offset the position by the zoom
					ftype t_zoomOffset = (1/t_zoom - m_viewzoom) * Screen::Info.width * 0.5f;
					m_viewposition.x -= t_zoomOffset;
					m_viewposition.y -= t_zoomOffset / Screen::Info.aspect;
				}
			}
			// Go through all the nodes and buttons and check mouse positions
			{
				Vector2d cursor_position( CInput::MouseX() / (ftype)Screen::Info.height, CInput::MouseY() / (ftype)Screen::Info.height );
				cursor_position /= t_zoom;
				skillNode* currentNode;
				m_mouseoverNode = NULL;
				m_mouseoverButton = -1;
				for ( auto node = pSkillTree->nodes.begin(); node != pSkillTree->nodes.end(); ++node )
				{
					currentNode = *node;
					if ( currentNode->skillPos.Contains( cursor_position-(m_viewposition/720.0f) ) ) {
						m_mouseoverNode = currentNode;
					}
				}
				// Check mouse buttons
				cursor_position *= t_zoom;
				if ( Rect(0.45f,0.07f,0.18f,0.05f).Contains( cursor_position ) ) {
					m_mouseoverButton = 1;
				}
				if ( Rect(0.25f,0.07f,0.18f,0.05f).Contains( cursor_position ) ) {
					m_mouseoverButton = 2;
				}
			}
			// Grab selected item from inventory for now
			pSelected = pInventory->GetSelected();
			// Perform skill items
			if ( CInput::MouseDown(CInput::MBLeft) )
			{
				Click();
			}
			// Turn off skill point application if no skill points left
			if ( skillPoint <= 0 )
			{
				bSetSkillPoint = false;
			}
		}
		else
		{
			// Perform view dragging
			if ( CInput::MouseUp(CInput::MBLeft) )
			{
				m_dragging = false;
			}
			else
			{
				m_viewposition.x += CInput::DeltaMouseX() / t_zoom;
				m_viewposition.y += CInput::DeltaMouseY() / t_zoom;
			}
		}
	}
}
/*
void CSkillTreeGUI::drawSelected (void)
{
	//Get the location of the mouse
	float x = CInput::mouseX;
	float y = CInput::mouseY;
	//Make some calculations
	float width_spacing = .1;
	short screen_height = Screen::Info.height;
	x /= screen_height;
	y /= screen_height;

	//Draw the icon to follow the mouse
	testFnt->m_diffuse = Color (1.0f, 1.0, 1.0);
	if (pSelected != NULL)
	{
		CTexture* icon = pSelected->GetInventoryIcon();

		testFnt->setTexture( 0, icon );
		testFnt->bindPass(0);
		GL.DrawRectangleA(x, y, width_spacing * .9, width_spacing * .9);
	}
	*/
	//Draws the basic item info for the picked item
	/*testMat->m_diffuse = Color (1.0f, 0.0, 0.0);
	testMat->bindPass(0);
	//fntDebug->Set();
	//Draw the info for the picked item
	if (pSelected != NULL)
	{
		//string cheese = pSelected->GetItemName();
		//GL.DrawAutoTextWrapped (x, y + .02, width_spacing * .9, cheese.c_str());
		
		if (pSelected->GetCanStack())
		{
			short stack = pSelected->GetStackSize();
			GL.DrawAutoTextWrapped ( x, y + .07, width_spacing * .9, "%d", stack );
		}
	}
	//fntDebug->Unbind();
	*/
//}