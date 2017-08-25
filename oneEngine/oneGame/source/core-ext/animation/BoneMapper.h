//
//	BoneMapper.h
// Provides methods to build mapping of mismatched skeletons.
// 
#ifndef CORE_ANIMATION_BONE_MAPPER_H_
#define CORE_ANIMATION_BONE_MAPPER_H_

#include "core/types.h"
#include "core-ext/animation/Skeleton.h"
#include <vector>

namespace animation
{
	//	Bone index constants
	enum eSpecialBoneTypes_t : uint8_t
	{
		INVALID_BONE = 255
	};

	//	struct boneMapEntry_t
	// Stores a single mapping of a bone from one skeleton to another
	struct boneMapEntry_t
	{
		// Index of bone on the source skeleton
		uint8_t source;
		// Index of bone on the target skeleton
		uint8_t target;

		// Inline construction
		boneMapEntry_t ( const uint8_t n_source, const uint8_t n_target )
			: source(n_source), target(n_target)
		{
			;
		}
		// Default construction: hold invalid values
		boneMapEntry_t(void)
			: source(INVALID_BONE), target(INVALID_BONE)
		{
			;
		}

		// Comparison (exact)
		bool operator== ( const boneMapEntry_t& other ) const
		{
			return source == other.source && target == other.target;
		}
		// Inequality (exact)
		bool operator!= ( const boneMapEntry_t& other ) const
		{
			return source != other.source || target != other.target;
		}
	};

	//	class CBoneMapper
	// Holds mapping information of one animation to another, used to copy animations.
	class BoneMapper
	{
	public:
		std::vector<boneMapEntry_t> mapping;

		CORE_API static void CreateFromNameMatching ( const Skeleton& source, const Skeleton& target, BoneMapper& o_mapper, const bool true_match=false );

		//	OneToOne() : returns true if the mapping is one-to-one for each bone and therefore not needed
		CORE_API bool OneToOne ( void );
		//	Equivalent() : returns true if the mapping is equivalent to the given mapping
		CORE_API bool Equivalent ( const BoneMapper& other );
	};
}

#endif//CORE_ANIMATION_BONE_MAPPER_H_