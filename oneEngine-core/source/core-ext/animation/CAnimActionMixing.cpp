
#include "CAnimation.h"
#include "CAnimAction.h"
//#include "CSkinnedModel.h"

// Vertex skinned models ONLY!
		// Searches for the given transform in the skeleton and adds it to the mix list.
//void CAnimAction::AddMixingTransform ( string const& transformName, bool recursive )
void CAnimAction::AddMixingTransform ( const uint32_t skippedIndex )
{
	// First, need to get to the model
	/*CSkinnedModel* model = (CSkinnedModel*)owner->pOwner;

	// Now need base of skeleton
	//CTransform* skellyRoot = model->GetSkeletonRoot();
	std::vector<skeletonBone_t*>* skellyList = model->GetSkeletonList();

	// Loop through the list to find the bone
	for ( uint32_t i = 0; i < skellyList->size(); ++i )
	{
		if ( (*skellyList)[i]->name.find( transformName ) != string::npos )
		{
			mixingList.push_back( i );
			if ( recursive )
			{
				CTransform& target = (*skellyList)[i]->transform;
				for ( uint32_t j = 0; j < target.children.size(); ++j )
				{
					AddMixingTransform( target.children[j]->name, true );
				}
			}

			// Make sure there are no doubles
			{
				for ( uint32_t k1 = 0; k1 < mixingList.size()-1; ++k1 )
				{
					for ( uint32_t k2 = k1+1; k2 < mixingList.size(); ++k2 )
					{
						if ( mixingList[k1] == mixingList[k2] )
						{
							mixingList.erase( mixingList.begin()+k1 );
							k1 = 0;
							k2 = 1;
						}
					}
				}
			}

			return;
		}
	}*/
	mixingList.push_back( skippedIndex );

	// Make sure there are no doubles
	{
		for ( uint32_t k1 = 0; k1 < mixingList.size()-1; ++k1 )
		{
			for ( uint32_t k2 = k1+1; k2 < mixingList.size(); ++k2 )
			{
				if ( mixingList[k1] == mixingList[k2] )
				{
					mixingList.erase( mixingList.begin()+k1 );
					k1 = 0;
					k2 = 1;
				}
			}
		}
	}
}