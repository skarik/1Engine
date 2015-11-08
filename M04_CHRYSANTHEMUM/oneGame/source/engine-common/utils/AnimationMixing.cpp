

#include "AnimationMixing.h"

#include "core-ext/animation/CAnimation.h"
#include "core-ext/animation/CAnimAction.h"
#include "physical/skeleton/skeletonBone.h"
#include "renderer/logic/model/CSkinnedModel.h"

int Animation::AddMixingTransform ( CAnimation& anim, CAnimAction& action, const char* boneName, bool recursive )
{
	//anim["idle_relaxed_hover_02"].AddMixingTransform( "Bip001", false ); // Old syntax

	// First, need to get to the model
	CSkinnedModel* model = (CSkinnedModel*)anim.GetOwner();

	// Now need base of skeleton
	//CTransform* skellyRoot = model->GetSkeletonRoot();
	std::vector<skeletonBone_t*>* skellyList = model->GetSkeletonList();

	// Loop through the list to find the bone
	for ( uint32_t i = 0; i < skellyList->size(); ++i )
	{
		if ( (*skellyList)[i]->name.find( boneName ) != string::npos )
		{
			action.AddMixingTransform(i);
			if ( recursive )
			{
				CTransform& target = (*skellyList)[i]->transform;
				for ( uint32_t j = 0; j < target.children.size(); ++j )
				{
					AddMixingTransform( anim, action, target.children[j]->name.c_str(), true );
				}
			}
			return 1;
		}
	}
	return 0;
}