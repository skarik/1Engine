
#include "CRenderState.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/object/CRenderableObject.h"

using namespace renderer;

// Render order control
bool CRenderState::render_forward_comparator_t::operator() ( CRenderState::tRenderRequest& i, CRenderState::tRenderRequest& j )
{
	if ( j.obj == NULL ) {
		throw std::invalid_argument("Null renderRequest");
	}
	if ( i.obj == NULL ) {
		throw std::invalid_argument("Null renderRequest");
	}

	RrPassForward* imat, * jmat;
	imat = i.obj->GetPass( i.pass );
	jmat = j.obj->GetPass( j.pass );

	// Returning true means i comes first.
	// Returning false means j comes first.

	// Sort first based on layers
	if ( i.obj->renderType != j.obj->renderType ) {
		return i.obj->renderType < j.obj->renderType;
	}

	if ( i.obj->renderType == kRLV2D )
	{	// Render back-to-front on 2D
		return (i.obj->renderDistance) > (j.obj->renderDistance);
	}
	
	// Check for depth writing (depth write comes first)
	bool imat_depthmask = (imat->m_transparency_mode!=ALPHAMODE_TRANSLUCENT)&&(imat->b_depthmask);
	bool jmat_depthmask = (jmat->m_transparency_mode!=ALPHAMODE_TRANSLUCENT)&&(jmat->b_depthmask);

	if ( imat_depthmask != jmat_depthmask )
	{
		// Sort first by depthmasking
		return imat_depthmask;
	}
	else
	{
		// Perform opaque ordering
		if ( imat_depthmask )
		{
			// If the objects are the same, sort through increasing delay and increasing pass number
			if ( i.obj == j.obj )
			{
				if ( imat->m_delay != jmat->m_delay ) {
					return imat->m_delay < jmat->m_delay;
				}
				else {
					return i.pass < j.pass;
				}
			}
			else
			{
				// Check for screenshader
				if ( i.obj->m_material->m_isScreenShader || j.obj->m_material->m_isScreenShader )
				{
					// Render screenshader last.
					return !(i.obj->m_material->m_isScreenShader);
				}
				else
				{
					// Render front-to-back on opaque
					return ( (i.obj->renderDistance) < (j.obj->renderDistance) );
				}
			}
		}
		// Perform transparent ordering
		else 
		{
			// Check for screenshader
			if ( i.obj->m_material->m_isScreenShader || j.obj->m_material->m_isScreenShader )
			{
				// Render screenshader last.
				return !(i.obj->m_material->m_isScreenShader);
			}
			else
			{
				// Sort through render delay first
				if ( imat->m_delay != jmat->m_delay )
				{
					return imat->m_delay < jmat->m_delay;
				}

				// If the objects are the same, sort through increasing pass number
				if ( i.obj == j.obj )
				{
					return i.pass < j.pass;
				}
				else
				{
					// Render Back-to-front on transparent
					if ( fabs((i.obj->renderDistance)-(j.obj->renderDistance)) > FLOAT_PRECISION ) {
						return ( (i.obj->renderDistance) > (j.obj->renderDistance) );
					}
					else {
						return i.obj < j.obj;
					}
				}
			}
		}
	}
}
// Render order control
bool CRenderState::render_deferred_comparator_t::operator() ( CRenderState::tRenderRequest& i, CRenderState::tRenderRequest& j )
{
	if ( j.obj == NULL ) {
		throw std::invalid_argument("Null renderRequest");
	}
	if ( i.obj == NULL ) {
		throw std::invalid_argument("Null renderRequest");
	}

	RrPassForward* imat, * jmat;
	imat = i.obj->GetPass( i.pass );
	jmat = j.obj->GetPass( j.pass );

	// Returning true means i comes first.
	// Returning false means j comes first.

	// Sort first based on layers
	if ( i.obj->renderType != j.obj->renderType ) {
		return i.obj->renderType < j.obj->renderType;
	}
	// Sort next based on forward or not forward
	if ( i.forward != j.forward ) {
		return j.forward; // i comes first if j is forward
	}
	if ( i.obj->renderType == kRLV2D )
	{	// Render back-to-front on 2D
		return (i.obj->renderDistance) > (j.obj->renderDistance);
	}
	
	// Check for depth writing (depth write comes first)
	bool imat_depthmask = (imat->m_transparency_mode!=ALPHAMODE_TRANSLUCENT)&&(imat->b_depthmask);
	bool jmat_depthmask = (jmat->m_transparency_mode!=ALPHAMODE_TRANSLUCENT)&&(jmat->b_depthmask);
	/*
	// Set transparency really quick here
	i.transparent = !imat_depthmask;
	j.transparent = !jmat_depthmask;
	i.screenshader = i.obj->m_material->m_isScreenShader;
	j.screenshader = j.obj->m_material->m_isScreenShader;
	*/
	if ( imat_depthmask != jmat_depthmask )
	{
		// Sort first by depthmasking
		return imat_depthmask;
	}
	else
	{
		// Perform opaque ordering
		if ( imat_depthmask )
		{
			// If the objects are the same, sort through increasing delay and increasing pass number
			if ( i.obj == j.obj )
			{
				if ( imat->m_delay != jmat->m_delay ) {
					return imat->m_delay < jmat->m_delay;
				}
				else {
					return i.pass < j.pass;
				}
			}
			else
			{
				// Check for screenshader
				if ( i.obj->m_material->m_isScreenShader || j.obj->m_material->m_isScreenShader )
				{
					// Render screenshader last.
					return !(i.obj->m_material->m_isScreenShader);
				}
				else
				{
					// Render front-to-back on opaque
					return ( (i.obj->renderDistance) < (j.obj->renderDistance) );
				}
			}
		}
		// Perform transparent ordering
		else 
		{
			// Check for screenshader
			if ( i.obj->m_material->m_isScreenShader || j.obj->m_material->m_isScreenShader )
			{
				// Render screenshader last.
				return !(i.obj->m_material->m_isScreenShader);
			}
			else
			{
				// Sort through render delay first
				if ( imat->m_delay != jmat->m_delay )
				{
					return imat->m_delay < jmat->m_delay;
				}

				// If the objects are the same, sort through increasing pass number
				if ( i.obj == j.obj )
				{
					return i.pass < j.pass;
				}
				else
				{
					// Render Back-to-front on transparent
					if ( fabs((i.obj->renderDistance)-(j.obj->renderDistance)) > FLOAT_PRECISION ) {
						return ( (i.obj->renderDistance) > (j.obj->renderDistance) );
					}
					else {
						return i.obj < j.obj;
					}
				}
			}
		}
	}
}