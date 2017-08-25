
#include "CAnimationSet.h"
#include "core/utils/StringUtils.h"
#include "core-ext/animation/CAnimAction.h"
#include "core-ext/animation/curve/CAnimationCurve.h"
#include "core-ext/animation/CAnimation.h"
#include "core-ext/system/io/assets/ModelLoader.h"

// Constructor
CAnimationSet::CAnimationSet ( void )
	: framerate(30.0F), filename("")
{}
// Destructor
CAnimationSet::~CAnimationSet ( void )
{}


// Export animation to targets
void CAnimationSet::Export ( std::vector<void*> & referenceList )
{
	throw core::DeprecatedCallException();
	/*
	for ( unsigned int i = 0; i < referenceList.size(); i++ )
	{
		CAnimationCurveBase* currentCurve = animMap[i];
		CAnimationCurveBase::eCurveType curveType = currentCurve->GetType();

		if ( curveType == CAnimationCurveBase::CT_XTRANSFORM )
		{
			//((CArrayAnimationCurve<Matrix4x4>*)(currentCurve))->;
			(*((XTransform*)(referenceList[i]))) = ((CAnimationCurve<XTransform>*)(currentCurve))->GetResult();
		}
		else if ( curveType == CAnimationCurveBase::CT_MATRIX4 )
		{
			//((CArrayAnimationCurve<Matrix4x4>*)(currentCurve))->;
			(*((Matrix4x4*)(referenceList[i]))) = ((CAnimationCurve<Matrix4x4>*)(currentCurve))->GetResult();
		}
	}
	*/
}

// Ownership of the calling CAnimationSet is not transferred. This is why this function exists here and not in CAnimation.
void CAnimationSet::AddActions ( CAnimation* target )
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
		CAnimAction new_action ( StringUtils::ToLower( action->name.c_str() ).c_str() );

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