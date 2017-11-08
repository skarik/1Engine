
#include "BoneMapper.h"
#include "core/utils/StringUtils.h"

void animation::BoneMapper::CreateFromNameMatching(const Skeleton& source, const Skeleton& target, BoneMapper& o_mapper, const bool true_match)
{
	// Loop through each bone in target, find the best match in the source
	for ( uint8_t i = 0; i < source.names.size(); ++i )
	{
		size_t worst_match = 128;
		size_t best_match = 0;
		uint8_t best_match_index = kSpecialBoneTypeINVALID;
		// Check all bones
		for ( uint8_t j = 0; j < target.names.size(); ++j )
		{
			if ( true_match )
			{
				// Check for an exact match
				if ( source.names[i] == target.names[j] )
				{
					worst_match = 0;
					best_match = 128;
					best_match_index = j; // Set it as the best match
					break;
				}
			}
			else
			{
				// Find the largest and best match
				size_t match_size = StringUtils::LargestCommonSubstringLength( source.names[i].c_str(), target.names[j].c_str() );
				match_size = std::min( match_size, worst_match );
				// Is this match better? Store it as such.
				if ( match_size > best_match )
				{
					best_match = match_size;
					best_match_index = j;
				}
			}
		}
		// Is there a good match that is better than the worst match?
		if ( best_match > worst_match && best_match_index != kSpecialBoneTypeINVALID )
		{
			o_mapper.mapping.push_back( boneMapEntry_t( i, best_match_index ) );
		}
	}

	// Loop through and remove duplicate matches or doubled up matches
	for ( auto mapping = o_mapper.mapping.begin(); mapping != o_mapper.mapping.end(); ++mapping )
	{
		for ( auto comparison = mapping; comparison != o_mapper.mapping.end(); )
		{
			if ( mapping->target == comparison->target )
			{
				comparison = o_mapper.mapping.erase( comparison );
			}
			else
			{
				++comparison;
			}
		}
	}

	// And we have ourselves a fairly simple mapping! :)
}

bool animation::BoneMapper::OneToOne ( void )
{
	for ( auto itr = mapping.begin(); itr != mapping.end(); ++itr )
	{
		if ( itr->source != itr->target ) return false;
	}
	return true;
}
bool animation::BoneMapper::Equivalent ( const BoneMapper& other )
{
	if ( other.mapping.size() != this->mapping.size() )
	{
		return false;
	}
	for ( size_t i = 0; i < mapping.size(); ++i )
	{
		if ( this->mapping[i] != other.mapping[i] )
		{
			return false;
		}
	}
	return true;
}