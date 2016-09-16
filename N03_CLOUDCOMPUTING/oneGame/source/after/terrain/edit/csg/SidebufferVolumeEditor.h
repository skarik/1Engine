
#ifndef _SIDEBUFFER_VOLUME_EDITOR_H_
#define _SIDEBUFFER_VOLUME_EDITOR_H_

// Volume editor optimized for terrain generation

#include "after/terrain/generation/CWorldGenerator.h"

namespace Terrain
{
	class SidebufferAccessor;

	class SidebufferVolumeEditor
	{
	public:
		explicit SidebufferVolumeEditor( SidebufferAccessor* n_accessor, const Vector3d_d& n_zeropoint );
		~SidebufferVolumeEditor ( void );

		void Add_Sphere ( const Vector3d_d& n_centerpoint, const Real_d& n_radius, const ushort n_block );
		void Sub_Sphere ( const Vector3d_d& n_centerPoint, const Real_d& n_radius );

		// Chooses add or sub based on block info
		void Mak_Line ( const Vector3d_d& n_sourcepoint, const Vector3d_d& n_raydir, const Real_d& n_radius, const ushort n_block );


		void Sub_Line ( const Vector3d_d& n_sourcepoint, const Vector3d_d& n_raydir );


		void Add_Box ( const Vector3d_d& n_centerpoint, const Vector3d_d& n_halfextents, const ushort n_block );
	private:
		SidebufferAccessor*	m_accessor;
		Vector3d_d			m_zeropoint;
	};
};

#endif//_SIDEBUFFER_VOLUME_EDITOR_H_