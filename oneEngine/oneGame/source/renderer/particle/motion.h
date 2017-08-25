#ifndef _RENDERER_PARTICLE_MOTION_H_
#define _RENDERER_PARTICLE_MOTION_H_

#include "RrParticleSystem.h"

namespace renderer
{
	namespace particle
	{


		struct motion_config_t
		{
			int position_offset;
			int velocity_offset;
		};
		PARTICLE_CONFIG_VERIFY(motion_config_t);
		void motion ( char* particle_data, const size_t particle_stride, const size_t particle_count, const void* update_config )
		{
			const int POSITION = ((motion_config_t*)update_config)->position_offset;
			const int VELOCITY = ((motion_config_t*)update_config)->velocity_offset;

			for ( uint i = 0; i < particle_count; ++i )
			{
				// Add velocity
				((float*)particle_data)[POSITION + 0] += ((float*)particle_data)[VELOCITY + 0];
				((float*)particle_data)[POSITION + 1] += ((float*)particle_data)[VELOCITY + 1];
				((float*)particle_data)[POSITION + 2] += ((float*)particle_data)[VELOCITY + 2];

				particle_data += particle_stride;
			}
		}

		struct motion_acceleration_config_t
		{
			int position_offset;
			int velocity_offset;
			int acceleration_offset;
		};
		PARTICLE_CONFIG_VERIFY(motion_acceleration_config_t);
		void motion_acceleration ( char* particle_data, const size_t particle_stride, const size_t particle_count, const void* update_config )
		{
			const int POSITION = ((motion_acceleration_config_t*)update_config)->position_offset;
			const int VELOCITY = ((motion_acceleration_config_t*)update_config)->velocity_offset;
			const int ACCELERA = ((motion_acceleration_config_t*)update_config)->acceleration_offset;

			for ( uint i = 0; i < particle_count; ++i )
			{
				// Add acceleration
				((float*)particle_data)[VELOCITY + 0] += ((float*)particle_data)[ACCELERA + 0];
				((float*)particle_data)[VELOCITY + 1] += ((float*)particle_data)[ACCELERA + 1];
				((float*)particle_data)[VELOCITY + 2] += ((float*)particle_data)[ACCELERA + 2];
				// Add velocity
				((float*)particle_data)[POSITION + 0] += ((float*)particle_data)[VELOCITY + 0];
				((float*)particle_data)[POSITION + 1] += ((float*)particle_data)[VELOCITY + 1];
				((float*)particle_data)[POSITION + 2] += ((float*)particle_data)[VELOCITY + 2];

				particle_data += particle_stride;
			}
		}


	}
}

#endif//_RENDERER_PARTICLE_MOTION_H_