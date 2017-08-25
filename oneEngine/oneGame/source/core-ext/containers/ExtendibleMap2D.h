//===============================================================================================//
// ExtendibleMap2D
//
// 2D data set that extends to enclose any set values out of range.
//===============================================================================================//

#ifndef _C_AR_EXTENDIBLE_MAP_2D_
#define _C_AR_EXTENDIBLE_MAP_2D_

#include "core/types/types.h"
#include "core/exceptions/exceptions.h"

namespace core
{
	template <typename Data>
	class ExtendibleMap2D
	{
	public:
		//=========================================//
		// Constructor + Destructor

		ExtendibleMap2D ( void )
		{
			// Start with an empty data set
			m_dataw = 0;
			m_datah = 0;
			m_data_offsetx = 0;
			m_data_offsety = 0;
			m_data = NULL;
		}
		~ExtendibleMap2D( void )
		{
			if ( m_data ) {
				delete [] m_data;
			}
			m_data = NULL;
		}

		//=========================================//
		// Get/Set of data

		//	SetData ( x, y, data )
		// Sets data at the given position. Will extend the buffer if it doesn't exist.
		void SetData ( const int n_x, const int n_y, const Data& n_data ) {
			int x = -m_data_offsetx;
			int y = -m_data_offsety;
			int w = m_dataw;
			int h = m_datah;
			// Check for bounds
			if ( n_x < -m_data_offsetx ) {
				x = n_x; // The seek is too far to the left.
			}
			if ( n_y < -m_data_offsety ) {
				y = n_y; // The seek is too far to the NORTH REMEMBERS
			}
			if ( n_x >= m_dataw - m_data_offsetx ) {
				w = n_x - m_data_offsetx; // The seek is too far to the right
			}
			if ( n_y >= m_datah - m_data_offsety ) {
				h = n_y - m_data_offsety; // The seek is too far to the bottom
			}
			// Resize if needed
			if ( x != -m_data_offsetx || y != -m_data_offsety || w != m_dataw || h != m_datah )
			{
				Resize( w,h, x-m_data_offsetx, y-m_data_offsety ); // Offsets will always be positive
			}
			// Set the data
			m_data[ (n_x + m_data_offsetx) + (n_y + m_data_offsety)*m_dataw ] = n_data;
		}

		//	GetData ( x, y )
		// Returns data at position.
		// Throws NullReferenceException if the set is empty.
		// Throws InvalidArgumentException if the input is out of range.
		Data& GetData ( const int n_x, const int n_y ) {
			if ( m_data == NULL ) {
				throw core::NullReferenceException;
			}
			else if ( n_x + m_data_offsetx < 0 || n_x + m_data_offsetx >= m_dataw || n_y + m_data_offsety < 0 || n_y + m_data_offsety >= m_datah ) {
				throw core::InvalidArgumentException;
			}
			return m_data[ (n_x + m_data_offsetx) + (n_y + m_data_offsety)*m_dataw ];
		}

	private:

		//=========================================//
		// Internal helpers

		//	Resize
		// Resizes the internal data, sets default values, then copies over old set with a given shift.
		void Resize ( const int n_width, const int n_height, const int n_offsetX, const int n_offsetY )
		{
			// Create new data
			Data*	t_newData = new Data [ n_width * n_height ];
			// Set default values
			for ( int i = 0; i < n_width*n_height; ++i ) {
				t_newData[i] = Data();
			}
			// Copy over old data
			for ( int x = 0; x < m_dataw; ++x ) {
				for ( int y = 0; y < m_datah; ++y ) {
					t_newData[ x+n_offsetX + (y+n_offsetY)*n_width ] = m_data[ x + y*m_dataw ];
				}
			}
			// Delete old data
			delete [] m_data;
			// Set new data to current data
			m_data = t_newData;
			m_dataw = n_width;
			m_datah = n_height;
			m_data_offsetx += n_offsetX;
			m_data_offsety += n_offsetY;
		}

		//=========================================//
		// Internal state

		// Current data
		Data*	m_data;
		// Data Sizes
		uint	m_dataw;
		uint	m_datah;
		int		m_data_offsetx;
		int		m_data_offsety;
	};
}

#endif//_C_AR_EXTENDIBLE_MAP_2D_