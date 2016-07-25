
#include "CVoxelFileEditor.h"

#include "core/input/CInput.h"
#include "core/system/Screen.h"
#include "core/system/System.h"

#include "core-ext/system/io/mccvxg.h"
#include "core-ext/system/io/Resources.h"

#include "physical/physics/CPhysics.h"
#include "engine/physics/raycast/Raycaster.h"

#include "engine-common/dusk/CDuskGUI.h"

#include "renderer/camera/CCamera.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CBitMapFont.h"
#include "renderer/window/COglWindow.h"
#include "renderer/debug/CDebugDrawer.h"

#include "after/renderer/objects/hud/CBlockCursor.h"
#include "after/entities/world/environment/DayAndNightCycle.h"
#include "after/states/CWorldState.h"
#include "after/types/WorldVector.h"
#include "after/types/terrain/BlockType.h"
#include "after/types/terrain/BlockData.h"
#include "after/terrain/data/node.h"

#include "after/terrain/version.h"
#include "after/terrain/utils/indexing.h"

#include "after/terrain/system/CVoxelMesher.h"

#include "after/physics/CMeshedStaticProp.h"

#include "renderer/system/glMainSystem.h"
//#include "unused/CVoxelTerrain.h"
#include "after/terrain/edit/SidebufferAccessor.h"
#include "after/terrain/edit/csg/SidebufferVolumeEditor.h"

CVoxelFileEditor::CVoxelFileEditor ( void )
{
	m_daycycle	= NULL;

	m_camera	= new CCamera();
	m_gui		= new CDuskGUI();
	CreateGUI();

	m_cursorState.cursorPosition = Vector3d(0,0,0);
	m_cursorState.hitResult.hit = false;
	m_cursor	= new CBlockCursor();

	m_data		= new uint64_t [BLOCK_COUNT];

	m_boob		= new Terrain::Payload();
	m_boob->data	= (Terrain::terra_b*)m_data;
	// Reset all data to meaningful nothingness
	for ( int i = 0; i < BLOCK_COUNT; ++i )
	{
		Terrain::terra_b block;

		// Reset block
		block.raw = 0;

		// Set the normals
		int normal_x, normal_y, normal_z;
		normal_x = Terrain::_normal_unbias( 0.0F );
		normal_y = Terrain::_normal_unbias( 0.0F );
		normal_z = Terrain::_normal_unbias( 1.0F );
		block.normal_y_x = normal_x;
		block.normal_z_x = normal_x;
		block.normal_x_y = normal_y;
		block.normal_z_y = normal_y;
		block.normal_x_z = normal_z;
		block.normal_y_z = normal_z;
		// Set block offset
		block.normal_x_w = Terrain::_depth_unbias( 1.0f );
		block.normal_y_w = Terrain::_depth_unbias( 1.0f );
		block.normal_z_w = Terrain::_depth_unbias( 1.0f );

		// Set block lighting
		block.light_r = 0xF;
		block.light_g = 0xF;
		block.light_b = 0xF;

		// Set data
		m_boob->data[i] = block;
	}

	m_boob_collision= new CMeshedStaticProp;
	m_boob_collision->SetOwner( NULL );

	/*m_boob->pMesh	= new CBoobMesh();
	m_boob->pMesh->pOwner	= m_boob;
	m_boob->pCollision	= new CBoobCollision;
	m_boob->pCollision->pOwner	= m_boob;
	{
		m_boob->front = NULL;
		m_boob->back = NULL;
		m_boob->top = NULL;
		m_boob->bottom = NULL;
		m_boob->left = NULL;
		m_boob->right = NULL;

		m_boob->current_resolution = 1;

		m_boob->bitinfo[0] = true; // Visible flag
		m_boob->bitinfo[1] = false; // AlreadyDrawn flag
		m_boob->bitinfo[2] = false; // AlreadyChecked flag
		m_boob->bitinfo[3] = false; // Generation flag
		m_boob->bitinfo[5] = false; // VBO Generation flag
		m_boob->bitinfo[6] = false; // Movement check flag
		m_boob->bitinfo[7] = false; // Deletion flag
		// Set the block data to all non-solid air
		m_boob->hasPropData = true;
		m_boob->hasBlockData = true;
		m_boob->loadPropData = true;
		for ( char i = 0; i < 8; i += 1 ) {
			m_boob->solid[i] = false;
		}

		m_boob->version = (TERRAIN_VERSION_MAJOR | TERRAIN_VERSION_MINOR);
		m_boob->biome = 0;
		m_boob->terrain = 0;
		m_boob->li_position = RangeVector( 0,0,0 );
		m_boob->position = Vector3d( 0,0,32 );
	}*/
	bUpdateBoob = true;

	// create temp
	m_vertex_buffer		= new CTerrainVertex [1024*32*8];
	m_triangle_buffer	= new CModelTriangle [1024*16*8];

	// Load a texture
	{
		/*glMaterial* mNewMat;
		mNewMat = new glMaterial;
		mNewMat->loadFromFile( "nature_terra" );
		this->SetMaterial( mNewMat );*/
		glMaterial* mNewMat = glMaterial::Default->copy();
		mNewMat->loadFromFile( "nature_terra" );
		//mNewMat->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
		this->SetMaterial( mNewMat );
	}

	transform.position = Vector3d(-32,-32,0);
	transform.scale = Vector3d(1,1,1) * 2.0F;
}

CVoxelFileEditor::~CVoxelFileEditor ( void )
{
	delete_safe_decrement( m_daycycle );
	delete_safe( m_camera );
	delete_safe_decrement( m_gui );
	delete_safe( m_cursor );
	delete_safe( m_boob );
	delete [] m_data;
	m_data = NULL;

	delete [] m_vertex_buffer;
	m_vertex_buffer = NULL;
	delete [] m_triangle_buffer;
	m_triangle_buffer = NULL;
}

void CVoxelFileEditor::CreateGUI ( void )
{
	// Create and config GUI
	Dusk::Handle prntHandle;
	Dusk::Handle tempHandle;

	m_gui->SetPixelMode( true );
	m_gui->SetDefaultFont( new CBitmapFont( "YanoneKaffeesatz-R.otf", 14 ) );

	prntHandle = m_gui->CreateDraggablePanel();
	prntHandle.SetRect( Rect( 0, 40, 150, 550 ) );
	prntHandle.SetText( "Blocks" );
	{
		m_guiparts.block_selection = m_gui->CreateListview( prntHandle );
		m_guiparts.block_selection.SetRect( Rect( 25, 50, 100, 400) );
		m_gui->SetListviewFieldHeight( m_guiparts.block_selection, 30 );
		m_gui->AddListviewOption( m_guiparts.block_selection, "Dirt",			Terrain::EB_DIRT );
		m_gui->AddListviewOption( m_guiparts.block_selection, "Stone",			Terrain::EB_STONE );
		m_gui->AddListviewOption( m_guiparts.block_selection, "Stone Brick",	Terrain::EB_STONEBRICK );
		m_gui->AddListviewOption( m_guiparts.block_selection, "Wood Block",		Terrain::EB_WOOD );
		m_gui->AddListviewOption( m_guiparts.block_selection, "Roof 0",			Terrain::EB_ROOF_0 );
		m_gui->AddListviewOption( m_guiparts.block_selection, "Roof 1",			Terrain::EB_ROOF_1 );
		m_gui->AddListviewOption( m_guiparts.block_selection, "Sandstone",		Terrain::EB_SANDSTONE );
	}

	prntHandle = m_gui->CreateEdgePanel();
	prntHandle.SetRect( Rect( 0,0, 1280,40 ) );
	{
		m_guiparts.btn_center = m_gui->CreateButton( prntHandle );
		m_guiparts.btn_center.SetRect( Rect( 100,5,45,30 ) );
		m_guiparts.btn_center.SetText( "Center" );
	
		m_guiparts.btn_save = m_gui->CreateButton( prntHandle );
		m_guiparts.btn_save.SetRect( Rect( 150,5,45,30 ) );
		m_guiparts.btn_save.SetText( "Save" );

		m_guiparts.btn_load = m_gui->CreateButton( prntHandle );
		m_guiparts.btn_load.SetRect( Rect( 200,5,45,30 ) );
		m_guiparts.btn_load.SetText( "Open" );

		m_guiparts.btn_clear = m_gui->CreateButton( prntHandle );
		m_guiparts.btn_clear.SetRect( Rect( 250,5,45,30 ) );
		m_guiparts.btn_clear.SetText( "Clear" );
	}

	prntHandle = m_gui->CreateDraggablePanel();
	prntHandle.SetRect( Rect( 1000,80,180,180 ) );
	prntHandle.SetText( "Cursor position" );
	{
		m_guiparts.cursor_info = m_gui->CreateText( prntHandle, "ff" );
		m_guiparts.cursor_info.SetRect( Rect( 10,20,160,30 ) );

		m_guiparts.block_info = m_gui->CreateText( prntHandle, "None" );
		m_guiparts.block_info.SetRect( Rect( 10,60,160,30 ) );
	}
}

void CVoxelFileEditor::BuildBoobMesh ( void )
{
	/*Vector3d position ( -0.5f,-0.5f,0.0f );
	Vector3d baseOffset, pos;
	char i, j, k;
	short b_index;
	subblock8* block;

	// Following needs to be completely recreated
	// with a Terrain::Meshbuilder::BuildMesh ( rasterdata )

	if ( m_boob->pMesh->PrepareRegen() )
	{
		m_boob->pMesh->vCameraPos = m_camera->transform.position;

		// Loop through all data
		for ( char index = 0; index < 8; index += 1 )
		{
			for ( char subindex = 0; subindex < 8; subindex += 1 )
			{
				// Get block
				block = &(m_boob->data[index].data[subindex]);
				// Draw current block
				baseOffset = Vector3d(
					position.x*32.0f+((index%2)*16)+((subindex%2)*8),
					position.y*32.0f+(((index/2)%2)*16)+(((subindex/2)%2)*8),
					position.z*32.0f+((index/4)*16)+((subindex/4)*8) );
				// Loop through current subblock8
				for ( k = 0; k < 8; k += 1 )
				{
					for ( j = 0; j < 8; j += 1 )
					{
						for ( i = 0; i < 8; i += 1 )
						{
							b_index = i+j*8+k*64;
							block->data[b_index].light = 7;
							if (( block->data[b_index].block == Terrain::EB_NONE )||( block->data[b_index].block == Terrain::EB_WATER ))
							{
								pos.x = (baseOffset.x + (ftype)i)*2.0f;
								pos.y = (baseOffset.y + (ftype)j)*2.0f;
								pos.z = (baseOffset.z + (ftype)k)*2.0f;
								try
								{
									m_boob->pMesh->AddBlockMesh( m_boob, index, subindex, b_index, pos, 1 );
								}
								catch ( C_xcp_vertex_overflow& exception )
								{
									if (exception.what())
										MessageBox ( NULL,"Vertex indexes have had an overflow. Report such exceptions immediately.","FACK FACK FACK MEMORY FACK",MB_OK | MB_ICONINFORMATION );
								}
							}
						}
					}
				}
				// End block area
			}// End subblock 8
		}// End subblock 16

		if (( m_boob->pMesh->vertexCount >= 4 )&&( m_boob->pMesh->faceCount >= 2 )) {
			// Generate the mesh
			m_boob->pMesh->UpdateVBOMesh();
		}
		// Clean up the mesh
		m_boob->pMesh->CleanupRegen();
		// It's been updated
		m_boob->pMesh->needUpdateOnVBO = false;

		// Update collision
		m_boob->pCollision->update.set();
		m_boob->pCollision->needUpdateOnCollider = true;
		m_boob->pCollision->Update();
	}*/

	// Assuming BLOCK_COUNT is 32^3
	Terrain::SidebufferAccessor t_buffer ( NULL, (Terrain::terra_b*) m_data, WorldVector( 32,32,32 ) );

	CVoxelMesher mesher ( NULL );
	mesher.SetSourceData( &t_buffer );

	// Build the mesh
	uint t_vertex_count = 0, t_triangle_count = 0;

	CTerrainVertex* t_vertex_buffer = m_vertex_buffer;
	CModelTriangle* t_triangle_buffer = m_triangle_buffer;
	mesher.CreateMesh( t_vertex_buffer, t_triangle_buffer, t_vertex_count,t_triangle_count );

	if ( t_vertex_count > 0 ) {
		printf( "Built a mesh with counts: Verts: %d, Tris: %d\n", t_vertex_count,t_triangle_count );
	}

	// Give the collision that mesh
	m_boob_collision->UpdateScale( 1.0f );
	m_boob_collision->UpdateCollider( t_vertex_buffer, t_triangle_buffer, t_vertex_count,t_triangle_count );

	// Give the terrain renderer object that mesh
	m_drawmesh.UploadVerts( t_vertex_buffer, t_vertex_count );
	m_drawmesh.UploadFaces( t_triangle_buffer, t_triangle_count );
}

bool CVoxelFileEditor::Render ( const char pass )
{
	/*glMaterial* drawMat = vMaterials[0];

	drawMat->bind();
	drawMat->setShaderConstants( this );
	if ( ActiveGameWorld ) {
		drawMat->setUniform( "terra_BaseAmbient", ActiveGameWorld->cBaseAmbient );
	}
	else {
		drawMat->setUniform( "terra_BaseAmbient", Color(0.5f,0.5f,0.5f,1) );
	}

	// Disable face culling in shadow mode
	if ( CCamera::activeCamera->shadowCamera )
		glDisable( GL_CULL_FACE );

	// Re-enable culling
	if ( CCamera::activeCamera->shadowCamera )
		glEnable( GL_CULL_FACE );
		*/
	if ( m_drawmesh.GetElementCount() > 0 )
	{
		GL_ACCESS;

		GL.Transform( &transform );

		m_material->bindPass(pass);
		m_material->setShaderConstants( this );

		if ( BindVAO( pass, m_drawmesh.GetVBOverts(), m_drawmesh.GetVBOfaces(), true ) )
		{
			m_material->bindAttribute( "mdl_Vertex",	3, GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*0) );
			m_material->bindAttribute( "mdl_TexCoord",	3, GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*3) );
			m_material->bindAttribute( "mdl_Color",		4, GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*12) );
			m_material->bindAttribute( "mdl_Normal",	3, GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*6) );
			m_material->bindAttribute( "mdl_Tangents",	3, GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*9) );
		}

		GL.DrawElements( GL_TRIANGLES, m_drawmesh.GetElementCount(), GL_UNSIGNED_INT, 0 );
		GL.CheckError();
	}

	return true;
}


void CVoxelFileEditor::Update ( void )
{
	DoSave();
	DoLoad();
	if ( m_gui->GetMouseInGUI() )
	{
		// GUI work
		if ( m_guiparts.btn_clear.GetButtonClicked() ) {
			memset( m_data, 0, cub(32)*4 ); // Clear data.
		}
		if ( m_guiparts.btn_center.GetButtonClicked() ) Center();
		if ( m_guiparts.btn_load.GetButtonClicked() ) Load();
		if ( m_guiparts.btn_save.GetButtonClicked() ) Save();
	}
	else
	{
		Matrix4x4 rotMatx;
		rotMatx.setRotation( m_camera->transform.rotation );

		if ( (CInput::Mouse(CInput::MBLeft) && CInput::Mouse(CInput::MBRight))||(CInput::Mouse(CInput::MBMiddle))||(CInput::Mouse(CInput::MBRight)&&CInput::Key(Keys.Alt)) )
		{
			//rotMatx.setRotation( myCamera->transform.rotation );
			//vCameraCenter += rotMatx * Vector3d( 0, (ftype)CInput::deltaMouseX, (ftype)CInput::deltaMouseY ) * ( 1.2f + sqrt(cameraDistance) ) * 0.02f;
			m_camera->transform.position -= rotMatx * Vector3d( CInput::DeltaMouseY(), CInput::DeltaMouseX(), 0 ) * 0.2f;
		}
		else if ( CInput::Mouse( CInput::MBLeft ) )
		{
			//vCameraAngles += Vector3d( 0, (ftype)CInput::deltaMouseY*0.7f, -(ftype)CInput::deltaMouseX*0.7f );
			//m_camera->transform.rotation = Rotator( Vector3d( 0, (ftype)CInput::deltaMouseY*0.7f, -(ftype)CInput::deltaMouseX*0.7f ) ) * m_camera->transform.rotation;
			Vector3d targetAngles = m_camera->transform.rotation.getEulerAngles()-Vector3d( 0, CInput::DeltaMouseY()*0.7f, -CInput::DeltaMouseX()*0.7f );
			if ( targetAngles.y < -88.0f ) {
				targetAngles.y = -88.0f;
			}
			else if ( targetAngles.y > 88.0f ) {
				targetAngles.y = 88.0f;
			}
			m_camera->transform.rotation.Euler( targetAngles );
			
		}
		else if ( CInput::Mouse( CInput::MBRight ) )
		{
			/*cameraDistance += (ftype)CInput::deltaMouseY * ( 1.2f + sqrt(cameraDistance) ) * 0.02f;
			if ( cameraDistance < 0 ) {
				cameraDistance = 0;
			}*/
			rotMatx.setRotation( m_camera->transform.rotation );
			m_camera->transform.position -= rotMatx * Vector3d( 0, CInput::DeltaMouseX(), CInput::DeltaMouseY() ) * 0.3f;
		}

		// Update mouse cursor
		{
			Vector2d cursor_pos = Vector2d( Input::MouseX() / (Real)Screen::Info.width, Input::MouseY() / (Real)Screen::Info.height );
			Ray tracedir ( m_camera->transform.position, m_camera->ScreenToWorldDir( cursor_pos ) );
			/*if ( Raycaster.Raycast( tracedir, 500.0f, &m_cursorState.hitResult, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31) ) )
			{
				m_cursorState.cursorPosition = m_cursorState.hitResult.hitPos;
			}
			else*/
			{
				// Cast it to the water if it's going that way.
				m_cursorState.hitResult.hit = false;
				if ( (tracedir.pos.z * tracedir.dir.z) < 0 ) {
					m_cursorState.cursorPosition = tracedir.pos + tracedir.dir*(0 - (tracedir.pos.z/tracedir.dir.z)); // 0 because constant sea level TODO: add get wave height for half-water rooms
					m_cursorState.cursorPosition.z += 1.0f;
				}
				//m_cursorState.cursorPosition = tracedir.pos + tracedir.dir * 10.0f;
			}

			// Draw cursor position
			m_cursor->transform.position = m_cursorState.cursorPosition;
			m_cursor->transform.position.x = floorf( m_cursor->transform.position.x * 0.5f ) * 2.0f;
			m_cursor->transform.position.y = floorf( m_cursor->transform.position.y * 0.5f ) * 2.0f;
			m_cursor->transform.position.z = floorf( m_cursor->transform.position.z * 0.5f ) * 2.0f;
			m_cursor->transform.position += Vector3d( 1.0f,1.0f,1.0f );

			// Set mouseover text
			char str_temp [256];
			{
				// Print cursor position
				sprintf( str_temp, "X: %.1lf Y: %.1lf Z: %.1lf", m_cursorState.cursorPosition.x, m_cursorState.cursorPosition.y, m_cursorState.cursorPosition.z );
				m_guiparts.cursor_info.SetText( string(str_temp) );
				// Print cursor block type
				if ( m_cursorState.hitResult.hit )
				{
					Vector3d target = m_cursorState.cursorPosition + Vector3d(32,32,0);
					target -= m_cursorState.hitResult.hitNormal;
					short k;
					if ( PositionToBoobIndex( target,k ) )
					{
						sprintf( str_temp, "Block: %s", Terrain::blockName[m_boob->data[k].block] );	
						m_guiparts.block_info.SetText( string(str_temp) );
					}
				}
				else {
					m_guiparts.block_info.SetText( "Block: None" );
				}
			}
			
		}

		// Place or delete a block
		if ( Input::Keydown('X') )
		{
			// Get the block index
			Vector3d target = m_cursorState.cursorPosition + Vector3d(32,32,0);
			if ( m_cursorState.hitResult.hit ) {
				target -= m_cursorState.hitResult.hitNormal;
			}
			short k;
			if ( PositionToBoobIndex( target,k ) )
			{
				// Delete block at position
				//m_boob->data[k].block = Terrain::EB_NONE;
				//m_boob->data[k].block = blockType;
				Terrain::SidebufferAccessor accessor ( NULL, (Terrain::terra_b*) m_data, WorldVector( 32,32,32 ) );
				Terrain::SidebufferVolumeEditor editor ( &accessor, Vector3d_d(0,0,0) );
				//editor.Add_Box( Vector3d_d(target.x,target.y,target.z+3), Vector3d_d(1,1,1), blockType );
				editor.Sub_Sphere( Vector3d_d(target.x,target.y,target.z+5), 2 );
			}
			// Regenernate mesh
			bUpdateBoob = true;
		}
		else if ( Input::Keydown(Keys.Space) )
		{
			// Get the block index
			Vector3d target = m_cursorState.cursorPosition + Vector3d(32,32,0);
			if ( m_cursorState.hitResult.hit ) {
				target += m_cursorState.hitResult.hitNormal;
			}
			short k;
			if ( PositionToBoobIndex( target,k ) )
			{
				// Add block at position
				short blockType = m_gui->GetListviewSelection( m_guiparts.block_selection );
				if ( blockType != -1 )
				{
					//m_boob->data[k].block = blockType;
					Terrain::SidebufferAccessor accessor ( NULL, (Terrain::terra_b*) m_data, WorldVector( 32,32,32 ) );
					Terrain::SidebufferVolumeEditor editor ( &accessor, Vector3d_d(0,0,0) );
					float random_value = random_range( 0.0F, 4.0F );
					editor.Add_Box( Vector3d_d(target.x,target.y,target.z+3+random_value), Vector3d_d(1,1,1), blockType );
					//editor.Add_Sphere( Vector3d_d(target.x,target.y,target.z+5+random_value), 2, blockType );
				}
			}
			// Regenernate mesh
			bUpdateBoob = true;
		}
	}

	if ( bUpdateBoob ) {
		BuildBoobMesh();
		bUpdateBoob = false;
	}

	// Set background
	/*if ( !mDaycycle ) {
		RenderSettings.clearColor = cBackgroundColor;
	}*/
	if ( !m_daycycle ) { 
		m_daycycle = new Daycycle;
		m_daycycle->SetTimeOfDay( 60*60*4 );
	}
	//RenderSettings.clearColor = Color( 0.4f,0.4f,0.4f );
	// Draw grid
	for ( int i = -32; i <= 32; i += 2 ) {
		Debug::Drawer->DrawLine( Vector3d( 32,(ftype)i,0 ), Vector3d( -32,(ftype)i,0 ), Color(0.2f,0.2f,0.2f) );
		Debug::Drawer->DrawLine( Vector3d( (ftype)i,32,0 ), Vector3d( (ftype)i,-32,0 ), Color(0.2f,0.2f,0.2f) );
	}
}

bool CVoxelFileEditor::PositionToBoobIndex ( const Vector3d& nPosition, short& k )
{
	Vector3d target = nPosition;
	target *= 0.5f;
	int x,y,z;
	x = (int)target.x;
	y = (int)target.y;
	z = (int)target.z;
	if ( x < 0 || x >= 32 || y < 0 || y >= 32 || z < 0 || z >= 32 )
	{
		return false;
	}
	else
	{
		// Convert position into index
		k = Terrain::Indexing::XyzToLinear( x,y,z );
		return true;
	}
}


void CVoxelFileEditor::DoLoad ( void ){}
void CVoxelFileEditor::Load ( void )
{
	System::sFileDialogueEntry entry;
	strcpy( entry.extension, "*.mcc.VXG" );
	strcpy( entry.filetype, "MCC Variant aligned voxel graphic" );
/*	m_gui->DialogueOpenFilename( &entry, 1, "terra/", "Open voxel file" );
}
void CVoxelFileEditor::DoLoad ( void )
{*/
	char str_result [1024];
	if ( System::GetOpenFilename( str_result, &entry, 1, (Core::Resources::GetPrimaryResourcePath() + "terra/").c_str(), "Open voxel file" ) )
	//if ( m_gui->GetOpenFilename( str_result ) )
	{
		std::cout << "Opening: " << str_result << std::endl;
		
		// Open file for loading
		FILE* file = fopen( str_result, "rb" );
		mccVXG_file_infop vxg_infop = mccvxg_begin_io( file );
		{
			// Clear off area
			//memset( m_data, 0, cub(32)*4 ); // Clear data.
			memset( m_data, 0, BLOCK_COUNT * sizeof(Terrain::terra_b) );

			// Load info
			mccvxg_read_info( vxg_infop );

			// Load image size
			int bitdepth = 4;
			RangeVector image_size;
			bitdepth = mccvxg_get_bit_depth( vxg_infop );
			image_size.x = mccvxg_get_image_size( vxg_infop, MCCVXG_XSIZE );
			image_size.y = mccvxg_get_image_size( vxg_infop, MCCVXG_YSIZE );
			image_size.z = mccvxg_get_image_size( vxg_infop, MCCVXG_ZSIZE );

			// Create buffer
			char* buffer = new char [image_size.x*image_size.y*image_size.z*bitdepth];
			// Read in data
			mccvxg_read_image( vxg_infop, buffer );

			// Copy data to the area
			Terrain::terra_b* linearBuffer = (Terrain::terra_b*)buffer;
			for ( int x = 0; x < image_size.x; ++x ) {
				for ( int y = 0; y < image_size.y; ++y ) {
					for ( int z = 0; z < image_size.z; ++z ) {
						//uchar i,j;
						short k;
						//LinearIndexToOctreeIndex( (char)x,(char)y,(char)z, i,j,k );
						k = Terrain::Indexing::XyzToLinear( (char)x,(char)y,(char)z );
						//m_boob->data[i].data[j].data[k] = linearBuffer[x + (y*image_size.x) + (z*image_size.x*image_size.y)];
						m_boob->data[k] = linearBuffer[x + (y*image_size.x) + (z*image_size.x*image_size.y)];
					}
				}
			}
			// free memory
			delete [] buffer;
			
			// Center the object to the ground now
			Center(); Center();
		}
		mccvxg_end_io( vxg_infop );

		// Mark for update
		bUpdateBoob = true;
	}
}
void CVoxelFileEditor::DoSave ( void ){}
void CVoxelFileEditor::Save ( void )
{
	System::sFileDialogueEntry entry;
	strcpy( entry.extension, "*.mcc.VXG" );
	strcpy( entry.filetype, "MCC Variant aligned voxel graphic" );
/*	m_gui->DialogueSaveFilename( &entry, 1, "terra/", "Save voxel file" );
}
void CVoxelFileEditor::DoSave ( void )
{*/
	char str_result [1024];

	if ( System::GetSaveFilename( str_result, &entry, 1, (Core::Resources::GetPrimaryResourcePath() + "terra/").c_str(), "Save voxel file" ) )
	//if ( m_gui->GetSaveFilename( str_result ) )
	{
		string resultFile = str_result;
		if ( resultFile.find( ".mcc.VXG" ) == string::npos ) {
			resultFile += ".mcc.VXG";
		}
		std::cout << "Saving: " << resultFile << std::endl;

		// Open file for saving
		FILE* file = fopen( resultFile.c_str(), "wb" );
		mccVXG_file_infop vxg_infop = mccvxg_begin_io( file );
		{
			RangeVector obj_size;
			RangeVector obj_position;
		
			// Get the size.
			GetSize( obj_size );
			// Get the corner position
			GetPosition( obj_position );

			// Write the file header
			mccvxg_write_header( vxg_infop );
			// Write the file info
			mccvxg_write_bit_depth( vxg_infop, sizeof(Terrain::terra_b) );
			mccvxg_write_image_size( vxg_infop, obj_size.x, obj_size.y, obj_size.z );
			mccvxg_write_info( vxg_infop );

			// Create buffer
			char* buffer = new char [obj_size.x*obj_size.y*obj_size.z*sizeof(Terrain::terra_b)];
			// Copy data from the area
			Terrain::terra_b* linearBuffer = (Terrain::terra_b*)buffer;
			for ( int x = 0; x < obj_size.x; ++x ) {
				for ( int y = 0; y < obj_size.y; ++y ) {
					for ( int z = 0; z < obj_size.z; ++z ) {
						//uchar i,j;
						short k;
						//LinearIndexToOctreeIndex( (char)(x+obj_position.x),(char)(y+obj_position.y),(char)(z+obj_position.z), i,j,k );
						k = Terrain::Indexing::XyzToLinear( (char)(x+obj_position.x),(char)(y+obj_position.y),(char)(z+obj_position.z) );
						//linearBuffer[x + (y*obj_size.x) + (z*obj_size.x*obj_size.y)] = m_boob->data[i].data[j].data[k];
						linearBuffer[x + (y*obj_size.x) + (z*obj_size.x*obj_size.y)] = m_boob->data[k];
					}
				}
			}
			// Write data
			mccvxg_write_image( vxg_infop, buffer );

			// Free buffer
			delete [] buffer;
		}
		mccvxg_end_io( vxg_infop );

		// Mark for update
		bUpdateBoob = true;
	}
}

void CVoxelFileEditor::Center ( void )
{
	RangeVector obj_size;
	RangeVector obj_position;
	RangeVector	obj_offset;

	// Get the size.
	GetSize( obj_size );
	// Get the center offset
	GetPosition( obj_position );

	obj_offset.x = 16 - (obj_position.x+obj_size.x/2);
	obj_offset.y = 16 - (obj_position.y+obj_size.y/2);
	obj_offset.z = 16 - (obj_position.z+obj_size.z/2);

	// If it is centered, then look at dropping it.
	if ( obj_offset.x == 0 && obj_offset.y == 0 && obj_offset.z == 0 ) {
		obj_offset.z = -obj_position.z;
	}

	// Create temp data
	/*uint32_t pTempRawData [cub(32)];
	subblock16* pTempData = (subblock16*)pTempRawData;
	memcpy( pTempRawData, m_data, cub(32)*4 );
	memset( m_data, 0, cub(32)*4 );*/
	Terrain::terra_b pTempData [BLOCK_COUNT];
	memcpy( pTempData, m_data, BLOCK_COUNT * sizeof(Terrain::terra_b) );
	memset( m_data, 0, BLOCK_COUNT * sizeof(Terrain::terra_b) );

	// Loop through data to copy over data
	uchar x,y,z;
	uchar sx,sy,sz;
	uchar tx,ty,tz;
	//uchar si, sj;
	short sk;
	//uchar ti, tj;
	short tk;
	for ( x = 0; x < obj_size.x; ++x ) {
		sx = x+obj_position.x;
		tx = sx+obj_offset.x;
		for ( y = 0; y < obj_size.y; ++y ) {
			sy = y+obj_position.y;
			ty = sy+obj_offset.y;
			for ( z = 0; z < obj_size.z; ++z ) {
				sz = z+obj_position.z;
				tz = sz+obj_offset.z;

				if ( tx >= 0 && tx < 32 && ty >= 0 && ty < 32 && tz >= 0 && tz < 32 )
				{	// Turn to octtree indices
					//LinearIndexToOctreeIndex( sx,sy,sz, si,sj,sk );
					//LinearIndexToOctreeIndex( tx,ty,tz, ti,tj,tk );
					sk = Terrain::Indexing::XyzToLinear( sx,sy,sz );
					tk = Terrain::Indexing::XyzToLinear( tx,ty,tz );
					// Copy data over
					//m_boob->data[ti].data[tj].data[tk] = pTempData[si].data[sj].data[sk];
					m_boob->data[tk] = pTempData[sk];
				}
			}
		}
	}

	// Mark for update
	bUpdateBoob = true;
}

// Calculates the size of the mesh
void CVoxelFileEditor::GetSize ( RangeVector& noutSize )
{
	//uchar i,j;
	//short k;
	char x,y,z;

	char minx=-1, miny=-1, minz=-1;
	char maxx=-1, maxy=-1, maxz=-1;

	// Set initial size to zero
	noutSize.x = 0;
	noutSize.y = 0;
	noutSize.z = 0;

	// Loop through all the data
	/*for ( i = 0; i < 8; ++i ) {
		for ( j = 0; j < 8; ++j ) {
			for ( k = 0; k < 512; ++k ) {
				OctreeIndexToLinearIndex( i,j,k, x,y,z );
				
				if ( m_boob->data[i].data[j].data[k].block != Terrain::EB_NONE )
				{
					if (( minx == -1 )||( x < minx )) minx = x;
					if (( miny == -1 )||( y < miny )) miny = y;
					if (( minz == -1 )||( z < minz )) minz = z;
					if (( maxx == -1 )||( x > maxx )) maxx = x;
					if (( maxy == -1 )||( y > maxy )) maxy = y;
					if (( maxz == -1 )||( z > maxz )) maxz = z;
				}
			}
		}
	}*/
	for ( int i = 0; i < BLOCK_COUNT; ++i )
	{
		Terrain::Indexing::LinearToXyz( i, x,y,z );
		if ( m_boob->data[i].block != Terrain::EB_NONE )
		{
			if (( minx == -1 )||( x < minx )) minx = x;
			if (( miny == -1 )||( y < miny )) miny = y;
			if (( minz == -1 )||( z < minz )) minz = z;
			if (( maxx == -1 )||( x > maxx )) maxx = x;
			if (( maxy == -1 )||( y > maxy )) maxy = y;
			if (( maxz == -1 )||( z > maxz )) maxz = z;
		}
	}

	// Set the size found
	if ( minx >= 0 && maxx >= 0 && miny >= 0 && maxy >= 0 && minz >= 0 && maxz >= 0 )
	{
		noutSize.x = 1 + maxx-minx;
		noutSize.y = 1 + maxy-miny;
		noutSize.z = 1 + maxz-minz;
	}
}
// Gets the minimum position of the mesh
void CVoxelFileEditor::GetPosition ( RangeVector& noutPosition )
{
	//uchar i,j;
	//short k;
	char x,y,z;

	char minx=-1, miny=-1, minz=-1;

	// Set initial position to invalid
	noutPosition.x = -1;
	noutPosition.y = -1;
	noutPosition.z = -1;

	// Loop through all the data
	/*for ( i = 0; i < 8; ++i ) {
		for ( j = 0; j < 8; ++j ) {
			for ( k = 0; k < 512; ++k ) {
				OctreeIndexToLinearIndex( i,j,k, x,y,z );
				
				if ( m_boob->data[i].data[j].data[k].block != Terrain::EB_NONE )
				{
					if (( minx == -1 )||( x < minx )) minx = x;
					if (( miny == -1 )||( y < miny )) miny = y;
					if (( minz == -1 )||( z < minz )) minz = z;
				}
			}
		}
	}*/
	for ( int i = 0; i < BLOCK_COUNT; ++i )
	{
		Terrain::Indexing::LinearToXyz( i, x,y,z );
		if ( m_boob->data[i].block != Terrain::EB_NONE )
		{
			if (( minx == -1 )||( x < minx )) minx = x;
			if (( miny == -1 )||( y < miny )) miny = y;
			if (( minz == -1 )||( z < minz )) minz = z;
		}
	}
	
	// Set the position found
	if ( minx >= 0 && miny >= 0 && minz >= 0 )
	{
		noutPosition.x = minx;
		noutPosition.y = miny;
		noutPosition.z = minz;
	}
}
/*
// Converts block index to linear array index
void CVoxelFileEditor::OctreeIndexToLinearIndex ( const uchar i, const uchar j, const short k, char& x, char& y, char& z )
{
	x = (i%2)*16 + (j%2)*8 + (k%8);
	y = ((i/2)%2)*16 + ((j/2)%2)*8 + ((k/8)%8);
	z = (i/4)*16 + (j/4)*8 + (k/64);
}
// Converts linear array index to block index
void CVoxelFileEditor::LinearIndexToOctreeIndex ( const char x, const char y, const char z, uchar& i, uchar& j, short& k )
{
	i = x/16 + (y/16)*2 + (z/16)*4;
	j = ((x/8)%2) + ((y/8)%2)*2 + ((z/8)%2)*4;
	k = (x%8) + (y%8)*8 + (z%8)*64;
}*/