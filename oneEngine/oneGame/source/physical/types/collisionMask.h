#ifndef PHYSICAL_TYPES_COLLISION_MASK_H_
#define PHYSICAL_TYPES_COLLISION_MASK_H_

#include "core/types/types.h"
#include "physical/types/layers.h"

namespace physical
{
	struct prCollisionMask
	{
		unsigned int layer : 16;
		unsigned int group : 8;
		unsigned int id : 8;

		prCollisionMask ( void )
			: layer(-1), group(0), id(0)
			{}

		prCollisionMask ( prLayerMask col_layers, uint8_t col_group, uint8_t m_id )
			: layer(col_layers), group(col_group), id(m_id)
			{}
	};
}

#endif//PHYSICAL_TYPES_COLLISION_MASK_H_