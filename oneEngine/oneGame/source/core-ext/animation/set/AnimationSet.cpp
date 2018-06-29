#include "AnimationSet.h"
#include "core/utils/string.h"
#include "core-ext/animation/AnimationAction.h"
#include "core-ext/animation/curve/AnimationCurve.h"
#include "core-ext/animation/AnimationControl.h"
#include "core-ext/system/io/assets/ModelLoader.h"

// Constructor
AnimationSet::AnimationSet ( void )
	: framerate(30.0F), filename("")
{}
// Destructor
AnimationSet::~AnimationSet ( void )
{}


// Export animation to targets
void AnimationSet::Export ( std::vector<void*> & referenceList )
{
	throw core::DeprecatedCallException();
	/*
	for ( unsigned int i = 0; i < referenceList.size(); i++ )
	{
		AnimationCurveBase* currentCurve = animMap[i];
		AnimationCurveBase::eCurveType curveType = currentCurve->GetType();

		if ( curveType == AnimationCurveBase::CT_XTRANSFORM )
		{
			//((ArrayAnimationCurve<Matrix4x4>*)(currentCurve))->;
			(*((XTransform*)(referenceList[i]))) = ((AnimationCurve<XTransform>*)(currentCurve))->GetResult();
		}
		else if ( curveType == AnimationCurveBase::CT_MATRIX4 )
		{
			//((ArrayAnimationCurve<Matrix4x4>*)(currentCurve))->;
			(*((Matrix4x4*)(referenceList[i]))) = ((AnimationCurve<Matrix4x4>*)(currentCurve))->GetResult();
		}
	}
	*/
}

// Ownership of the calling AnimationSet is not transferred. This is why this function exists here and not in AnimationControl.
void AnimationSet::AddActions ( AnimationControl* target )
{
	// Load up the model file first
	core::ModelLoader loader;
	loader.m_loadActions = true;
	loader.m_loadSkeleton = true;
	if ( !loader.LoadModel( filename.c_str() ) )
	{
		throw core::InvalidCallException();
	}
	
	int8_t targetMapping = -1;
	int8_t targetSource = (int8_t)target->GetAnimationSource().size();

	// Add this to the animation sources
	target->GetAnimationSource().push_back(this);
	// Check if a new mapping is going to be needed
	{
		animation::Skeleton skeleton;
	}

	// Pull up and create the actions based on the loaded data
	for ( auto action = loader.actions.begin(); action != loader.actions.end(); ++action )
	{	
		// Create new action
		AnimationAction new_action ( core::utils::string::GetLower( action->name.c_str() ).c_str() );

		new_action.SetRange( (Real)(action->start), (Real)(action->end) );
		new_action.framesPerSecond = this->framerate;
		new_action.index = (uint16_t)target->GetActionMap().size();
		new_action.loop = (action->flags & 0x01) > 0;
		new_action.extrapolateMotion[0] = (action->flags & 0x02) > 0;
		new_action.extrapolateMotion[1] = (action->flags & 0x04) > 0;
		new_action.extrapolateMotion[2] = (action->flags & 0x08) > 0;
		new_action.sampleSource = targetSource;
		new_action.sampleMapping = targetMapping;

		target->AddAction( new_action );
	}
}