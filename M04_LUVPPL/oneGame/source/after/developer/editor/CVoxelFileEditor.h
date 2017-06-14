
#ifndef _C_VOXEL_FILE_EDITOR_H_
#define _C_VOXEL_FILE_EDITOR_H_

#include "core/types/ModelData.h"

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

#include "physical/physics/raycast/RaycastHit.h"
#include "engine-common/dusk/CDuskGUIHandle.h"
#include "after/types/WorldVector.h"
#include "after/terrain/data/Node.h"

class CCamera;
class CDuskGUI;
class CBlockCursor;
class Daycycle;
class CMeshedStaticProp;

class CVoxelFileEditor : public CGameBehavior, public CRenderableObject
{

public:
						CVoxelFileEditor ( void );
						~CVoxelFileEditor ( void );
						// Update routine. Handles input and GUI work
	void				Update ( void );
						// Render routine. Renders the voxel mesh.
	bool				Render ( const char pass );

private:
	void				CreateGUI ( void );
						// Regernates mesh
	void				BuildBoobMesh ( void );
						// Converts position to index for data access
	bool				PositionToBoobIndex ( const Vector3d& nPosition, uchar& i, uchar& j, short& k );
						
	void				Save ( void );
	void				DoSave ( void );
	void				Load ( void );
	void				DoLoad ( void );
	void				Center ( void );

						// Converts block index to linear array index
	//void				OctreeIndexToLinearIndex ( const uchar i, const uchar j, const short k, char& x, char& y, char& z );
						// Converts linear array index to block index
	//void				LinearIndexToOctreeIndex ( const char x, const char y, const char z, uchar& i, uchar& j, short& k );

						// Calculates the size of the mesh
	void				GetSize ( RangeVector& noutSize );
						// Gets the minimum position of the mesh
	void				GetPosition ( RangeVector& noutPosition );

private:
	Daycycle*		m_daycycle;

	CCamera*		m_camera;

	CDuskGUI*		m_gui;
	struct sGUIParts
	{
		Dusk::Handle	cursor_info;
		Dusk::Handle	block_info;
		Dusk::Handle	block_selection;
		Dusk::Handle	btn_load;
		Dusk::Handle	btn_save;
		Dusk::Handle	btn_center;
		Dusk::Handle	btn_clear;
	} m_guiparts;

	CBlockCursor*	m_cursor;
	struct sCursorState
	{
		Vector3d		cursorPosition;
		RaycastHit		hitResult;
	} m_cursorState;

	Terrain::Payload*	m_boob;
	uint64_t*			m_data;
	CMeshedStaticProp*	m_boob_collision;

	CTerrainVertex*		m_vertex_buffer;
	CModelTriangle*		m_triangle_buffer;

	bool			bUpdateBoob;
};

#endif//_C_VOXEL_FILE_EDITOR_H_