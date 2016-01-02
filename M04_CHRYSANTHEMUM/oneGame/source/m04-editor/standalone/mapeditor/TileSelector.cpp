
#include "TileSelector.h"
#include "TileSelectorUI.h"

#include "core/input/CInput.h"

#include "engine2d/entities/map/TileMap.h"

using namespace M04;

TileSelector::TileSelector ( void )
{
	m_tileselection = 0;
	ui = new TileSelectorUI();
}
TileSelector::~TileSelector ( void )
{
	delete_safe(ui);
}

bool TileSelector::Update ( void )
{
	// Update mouseover for the tile select
	int nextTileSelect = ui->UIMouseoverTiletype();
	ui->UISetSelection( m_tileselection );

	if ( nextTileSelect >= 0 )
	{
		if ( Input::MouseDown( Input::MBLeft ) )
		{
			m_tileselection = nextTileSelect;
			ui->UISetSelection( m_tileselection );
		}
		return true;
	}
	else
	{
		return false;
	}
}

//		SetTileMap ( )
// Sets the tilemap containing the tileset data that need to pull from
void TileSelector::SetTileMap ( Engine2D::TileMap* target )
{
	ui->SetTileMap( target );
}

// return the tile selection currently used for the selector
int	TileSelector::GetTileSelection ( void ) const
{
	return m_tileselection;
}

// sets if the UI is visible or not
void TileSelector::SetVisible ( const bool visibility )
{
	ui->SetVisible( visibility );
}
