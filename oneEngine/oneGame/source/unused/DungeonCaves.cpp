// class DungeonCaves
// 
// Cavern system, by Joshua Boren
// 
//

#include "DungeonCaves.h"

using std::min;
using std::max;

void DungeonCaves::Initialize ( const RangeVector & seedposition )
{
	m_gentable.rarity = 0.1f;
	m_gentable.difficulty = 0.1f;
	m_gentable.itemquality = (m_generator->Trd_Noise_Sample((ftype)seedposition.x,(ftype)seedposition.y,(ftype)seedposition.z)+0.5f) * 1.414f;
		m_gentable.itemquality *= m_gentable.itemquality; // Bigger density down low, over 0 to 2
	m_gentable.type = 0;
	m_gentable.element = Dungeon::ElementNeutral;
}


void DungeonCaves::Generate ( void )
{
	// Start with a cave at the elevation height.
	{
		// Choose a random position to set as the center
		Vector3d caveStartPos;
		caveStartPos.x = m_centerindex.x + m_generator->Trd_Noise_HF( m_centerindex.x*0.6f, m_centerindex.y*0.7f-0.3f, m_centerindex.z*0.8f ) + 0.5f;
		caveStartPos.y = m_centerindex.y + m_generator->Trd_Noise_HF( m_centerindex.y*0.6f, m_centerindex.z*0.7f-0.3f, m_centerindex.x*0.8f ) + 0.5f;
		caveStartPos.z = gen_elevationLine + 0.08f;

		m_system.center_position = caveStartPos;

		Dungeon::Hallway sourceHallway;
		// Set the start of the cave
		sourceHallway.ray.pos = caveStartPos;
		// With a random height from radius 1.5 to 2.9
		sourceHallway.height = 2.2f + 1.4f*m_generator->Trd_Noise_HF( m_centerindex.x*1.3f+0.4f,m_centerindex.z*1.6f,m_centerindex.y*1.4f );
		// With a random width from radius 1.0 to 3.0
		sourceHallway.width = 2.0f + 2.0f*m_generator->Trd_Noise_HF( m_centerindex.x*1.7f-0.4f,m_centerindex.z*1.2f-0.2f,m_centerindex.y*1.6f );
		// With a random roughness from 0.0 to 0.2
		sourceHallway.noise = 0.1f + 0.2f*m_generator->Trd_Noise_HF( m_centerindex.x+m_centerindex.y*0.2f,m_centerindex.z+0.4f );
		// And a random direction from +15 degrees to -90 degrees
		/*sourceHallway.ray.dir = Vector3d(
			m_generator->Trd_Noise_HF( m_centerindex.y+m_centerindex.z*1.3f-0.3f, m_centerindex.x*1.2f+m_centerindex.y+0.5f, m_centerindex.x*2.3f ),
			m_generator->Trd_Noise_HF( m_centerindex.x+m_centerindex.y*1.5f-0.2f, m_centerindex.y*1.7f+m_centerindex.z+0.3f, m_centerindex.y*1.4f ),
			m_generator->Trd_Noise_HF( m_centerindex.y+m_centerindex.z*1.3f-0.3f, m_centerindex.x*1.4f+m_centerindex.z+0.5f )*1.25f-0.375f
		);*/
		// And a random direction from 0 degrees to -90 degrees
		sourceHallway.ray.dir = Vector3d(
			m_generator->Trd_Noise_HF( m_centerindex.y+m_centerindex.z*1.3f-0.3f, m_centerindex.x*1.2f+m_centerindex.y+0.5f, m_centerindex.x*2.3f ),
			m_generator->Trd_Noise_HF( m_centerindex.x+m_centerindex.y*1.5f-0.2f, m_centerindex.y*1.7f+m_centerindex.z+0.3f, m_centerindex.y*1.4f ),
			m_generator->Trd_Noise_HF( m_centerindex.y+m_centerindex.z*1.3f-0.3f, m_centerindex.x*1.4f+m_centerindex.z+0.5f ) - 0.5f
		);
		// And extend that from 10 to 50 percent of the boob width
		sourceHallway.ray.dir = sourceHallway.ray.dir.normal() * ( 0.3f + 0.4f*m_generator->Trd_Noise_HF( m_centerindex.z*0.6f, m_centerindex.y*2.1f+m_centerindex.x*0.7f-0.3f ) );

		// Add the hallway to the system
		m_system.hallways.push_back( sourceHallway );
	}

	// Using the seed hallway, generate the rest of the cave system.
	// Note that caves will tend to be quite random, yet with a lot of repeated formations, so the code becomes
	//  rather complicated. Almost needlessly complicated.
	{
		// Because of that complicated and long code, the generation is separated out into another function.
		// It is unfortunately recursive, which I'm not a fan of, as the stack gets stretched thin already.
		AddSegment(0,0);
	}
}

// AddSegment( source_hallway_index, failure_seed )
// The meaty part of the cave generation
void DungeonCaves::AddSegment ( const unsigned int iHallway, const unsigned int seed )
{
	ftype fSeed = seed*2.3f+iHallway*2.4f;

	// Get the previous hallway
	Dungeon::Hallway prevSegment = m_system.hallways[iHallway];

	// Make a new tunnel
	Dungeon::Hallway newSegment;
	newSegment.ray.pos = prevSegment.ray.pos + prevSegment.ray.dir;
	
	// If the previous index is even, then we want to go horizontal or slightly down
	if ( iHallway % 2 == 0 )
	{
		// Choose a random direction that's related to the current direction
		newSegment.ray.dir.x = 2.0f * m_generator->Trd_Noise_Sample( m_centerindex.z*1.2f+newSegment.ray.pos.x-0.4f,  m_centerindex.y * 1.2f + prevSegment.ray.pos.y - iHallway*1.8f );
		newSegment.ray.dir.y = 2.0f * m_generator->Trd_Noise_Sample( m_centerindex.z*1.3f+newSegment.ray.pos.y-0.5f,  m_centerindex.x * 1.3f + prevSegment.ray.pos.x - iHallway*1.7f );

		newSegment.ray.dir += prevSegment.ray.dir.normal()*0.4f;
		newSegment.ray.dir.z *= 0.03f;

		// So randomly go horizontal or down
		if ( m_generator->Trd_Noise_Sample( iHallway*0.7f, m_centerindex.x * 1.2f ) > 0.0f )
			newSegment.ray.dir.z = -0.1f + 0.2f+m_generator->Trd_Noise_Sample( m_centerindex.x*1.2f+iHallway*0.8f-0.4f, m_centerindex.z * 1.2f + prevSegment.ray.pos.y );
		// But go downwards if we're on the surface
		if (( iHallway <= 12 )&&( newSegment.ray.pos.z > gen_elevationLine-0.5f ))
			newSegment.ray.dir.z -= 0.4f;

		// Normalize it and extend it from 10 to 30 percent of the boob width
		newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.2f + 0.2f*m_generator->Trd_Noise_HF( newSegment.ray.pos.z*0.7f+0.2f, newSegment.ray.pos.y*2.2f+newSegment.ray.pos.x*0.6f-0.7f ) );

		// Now make sure the ending position is in the valid boob area (-3 to 3)
		Vector3d endPos = newSegment.ray.pos + newSegment.ray.dir;
		if (( fabs( endPos.x-m_system.center_position.x ) < 2.3f )&&( fabs( endPos.y-m_system.center_position.y ) < 2.3f )&&( fabs( endPos.z-m_system.center_position.z ) < 2.3f ))
		{
			// If it's in the area, then we can continue generation and add this tunnel.

			// With a random height from the previous segment, limited at 1.5 and 5.0
			newSegment.height = prevSegment.height + 1.4f*m_generator->Trd_Noise_HF( endPos.x*1.3f+0.4f,endPos.z*1.6f,endPos.y*1.4f );
			newSegment.height = max( 1.5f, min( 5.0f, newSegment.height ) );
			// With a random width from the previous segment, limited at 1.0 and 6.0
			newSegment.width = prevSegment.width + 2.0f*m_generator->Trd_Noise_HF( endPos.x*1.7f-0.4f,endPos.z*1.2f-0.2f,endPos.y*1.6f );
			newSegment.width = max( 1.0f, min( 6.0f, newSegment.width ) );
			// With a random roughness from the previous segment, limited at 0.0 and 0.5
			newSegment.noise = prevSegment.noise + 0.2f*m_generator->Trd_Noise_HF( endPos.x+endPos.y*0.2f,endPos.z+0.4f );
			newSegment.noise = max( 0.0f, min( 0.5f, newSegment.noise ) );

			// Add the tunnel to the list and continue generating
			m_system.hallways.push_back( newSegment );
			AddSegment( m_system.hallways.size()-1, seed );
		}
	}
	// If the previous index is odd, then we want to go horizontal, directly downwards, directly upwards, or split
	else
	{
		// Choose a type of generation
		ftype randomVal = m_generator->Trd_Noise_HF( fSeed*2.1f-newSegment.ray.pos.x*0.8f+0.2f,newSegment.ray.pos.z*0.8f,newSegment.ray.pos.y*0.8f-fSeed*0.4f );

		// Do a horizontal tunnel
		if (( randomVal > -0.15f )||(( randomVal < -0.2 )&&( m_system.hallways.size() > 20 )))
		{
			// Choose a random direction that's related to the current direction
			newSegment.ray.dir.x = 2.0f * m_generator->Trd_Noise_Sample( m_system.center_position.z*1.2f+newSegment.ray.pos.x-0.4f,  m_system.center_position.y * 1.2f + prevSegment.ray.pos.y - fSeed*1.8f );
			newSegment.ray.dir.y = 2.0f * m_generator->Trd_Noise_Sample( m_system.center_position.z*1.3f+newSegment.ray.pos.y-0.5f,  m_system.center_position.x * 1.3f + prevSegment.ray.pos.x - fSeed*1.7f );

			newSegment.ray.dir += prevSegment.ray.dir.normal();
			newSegment.ray.dir.z *= 0.03f;

			// So randomly go horizontal or slightly down
			if ( m_generator->Trd_Noise_Sample( iHallway*0.7f, m_system.center_position.x * 1.2f ) > 0.0f )
				newSegment.ray.dir.z = -0.1f + 0.2f+m_generator->Trd_Noise_Sample( m_system.center_position.x*1.2f+iHallway*0.8f-0.4f, m_system.center_position.z * 1.2f + prevSegment.ray.pos.y );
			// But go downwards more if we're on the surface
			if (( iHallway <= 12 )&&( newSegment.ray.pos.z > gen_elevationLine-0.3f ))
				newSegment.ray.dir.z -= 0.35f;

			// Normalize it and extend it from 10 to 30 percent of the boob width
			newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.2f + 0.2f*m_generator->Trd_Noise_HF( newSegment.ray.pos.z*0.7f+0.2f, newSegment.ray.pos.y*2.2f+newSegment.ray.pos.x*0.6f-0.7f ) );
		}
		// Do a tunnel split
		else if ( randomVal < -0.2f )
		{
			// Choose a random direction that's related to the current direction
			newSegment.ray.dir.x = 2.0f * m_generator->Trd_Noise_Sample( m_system.center_position.z*1.2f+newSegment.ray.pos.x-0.4f,  m_system.center_position.y * 1.2f + prevSegment.ray.pos.y - fSeed*1.8f );
			newSegment.ray.dir.y = 2.0f * m_generator->Trd_Noise_Sample( m_system.center_position.z*1.3f+newSegment.ray.pos.y-0.5f,  m_system.center_position.x * 1.3f + prevSegment.ray.pos.x - fSeed*1.7f );

			newSegment.ray.dir += prevSegment.ray.dir.normal()*0.2f;
			newSegment.ray.dir.z *= 0.03f;

			// So randomly go horizontal or down
			if ( m_generator->Trd_Noise_Sample( iHallway*0.7f, m_system.start_position.x * 1.2f ) > 0.0f )
				newSegment.ray.dir.z = -0.1f + 0.2f+m_generator->Trd_Noise_Sample( m_system.center_position.x*1.2f+iHallway*0.8f-0.4f,  m_system.center_position.z * 1.2f + prevSegment.ray.pos.y );

			// Normalize it and extend it from 10 to 30 percent of the boob width
			newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.2f + 0.2f*m_generator->Trd_Noise_HF( newSegment.ray.pos.z*0.7f+0.2f, newSegment.ray.pos.y*2.2f+newSegment.ray.pos.x*0.6f-0.7f ) );

			// Also do another expansion
			AddSegment( iHallway, seed+1 );
		}
		// Do a vertical up or down spike
		else
		{
			// Choose a random direction that's related to the current direction
			newSegment.ray.dir.x = 2.0f * m_generator->Trd_Noise_Sample( m_system.center_position.z*1.2f+newSegment.ray.pos.x-0.4f,  m_system.center_position.y * 1.2f + prevSegment.ray.pos.y - fSeed*1.8f );
			newSegment.ray.dir.y = 2.0f * m_generator->Trd_Noise_Sample( m_system.center_position.z*1.3f+newSegment.ray.pos.y-0.5f,  m_system.center_position.x * 1.3f + prevSegment.ray.pos.x - fSeed*1.7f );

			newSegment.ray.dir += prevSegment.ray.dir.normal();
			newSegment.ray.dir.z *= 0.03f;

			// So randomly go down -10 to -40
			newSegment.ray.dir.z = -25.0f + 30.0f*m_generator->Trd_Noise_HF( m_system.center_position.x*1.3f+iHallway*0.9f+0.4f,  m_system.center_position.z * 1.3f + prevSegment.ray.pos.y );

			// Randomly flip it
			//if ( noise->Get( iTunnel*0.7f, pCave->startPosition.x * 1.2f ) > 0.3f )
			//	newSegment.ray.dir.z *= -1;

			// Normalize it and extend it from 30 to 120 percent of the boob width
			// 30 to 120 = 0.75 + 0.9
			// 30 to 90 = 0.6 + 0.6
			newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.6f + 0.6f*m_generator->Trd_Noise_HF( newSegment.ray.pos.z*0.7f+0.2f+fSeed*2.1f, newSegment.ray.pos.y*2.2f+newSegment.ray.pos.x*0.6f-0.7f-fSeed*2.1f ) );
		}

		// Now make sure the ending position is in the boob area
		Vector3d endPos = newSegment.ray.pos + newSegment.ray.dir;
		if (( fabs( endPos.x-m_system.center_position.x ) < 2.3f )&&( fabs( endPos.y-m_system.center_position.y ) < 2.3f )&&( fabs( endPos.z-m_system.center_position.z ) < 2.3f ))
		{
			// If it's in the area, then we can continue generation and add this tunnel.

			// With a random height from the previous segment, limited at 1.5 and 5.0
			newSegment.height = prevSegment.height + 1.4f*m_generator->Trd_Noise_HF( endPos.x*1.3f+0.4f,endPos.z*1.6f,endPos.y*1.4f );
			newSegment.height = max( 1.5f, min( 5.0f, newSegment.height ) );
			// With a random width from the previous segment, limited at 1.0 and 6.0
			newSegment.width = prevSegment.width + 2.0f*m_generator->Trd_Noise_HF( endPos.x*1.7f-0.4f,endPos.z*1.2f-0.2f,endPos.y*1.6f );
			newSegment.width = max( 1.0f, min( 6.0f, newSegment.width ) );
			// With a random roughness from the previous segment, limited at 0.0 and 0.5
			newSegment.noise = prevSegment.noise + 0.2f*m_generator->Trd_Noise_HF( endPos.x+endPos.y*0.2f,endPos.z+0.4f );
			newSegment.noise = max( 0.0f, min( 0.5f, newSegment.noise ) );

			// Add the tunnel to the list and continue generating
			m_system.hallways.push_back( newSegment );
			AddSegment( m_system.hallways.size()-1, seed );
		}
	}

}