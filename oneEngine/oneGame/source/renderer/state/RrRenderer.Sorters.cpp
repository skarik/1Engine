#include "RrRenderer.h"
#include "renderer/material/RrPass.h"
#include "renderer/object/RrRenderObject.h"

using namespace renderer;

FORCE_INLINE
static int PassOrder ( RrPass* pass, RrRenderObject* object )
{
	int order = pass->m_orderOffset;
	if (pass->m_depthWrite == false)
		order += kPassOrder_DepthMaskDisabled;
	/*if (pass->m_depthWrite == false)
	{
		order += kPassOrder_DepthMaskDisabled;
		// Back-to-front on transparent
	}
	else
	{
		// Front-to-back on opaque
		order += kPassOrder_DepthMaskDisabled - (int)(kPassOrder_DepthMaskDisabled / (object->renderDistance * 0.5F + 1.0F));
	}*/

	return order;
}

// Render order control
// Returns true if I is supposed to come before J.
bool rrRenderRequestSorter::operator() ( const rrRenderRequest& i, const rrRenderRequest& j )
{
	if ( j.obj == NULL ) {
		throw std::invalid_argument("Null renderRequest");
	}
	if ( i.obj == NULL ) {
		throw std::invalid_argument("Null renderRequest");
	}

	// Returning true means i comes first.
	// Returning false means j comes first.

	// Sort first based on layers
	/*if (i.obj->renderLayer != j.obj->renderLayer)
	{
		return i.obj->renderLayer < j.obj->renderLayer;
	}*/
	if (i.obj->m_passes[i.pass].m_layer != j.obj->m_passes[j.pass].m_layer)
	{
		return i.obj->m_passes[i.pass].m_layer < j.obj->m_passes[j.pass].m_layer;
	}

	/*if ( i.obj->renderLayer == kRLV2D )
	{	// Simply render back-to-front on 2D
		return (i.obj->renderDistance) > (j.obj->renderDistance);
	}*/
	
	// Check for depth writing (depth write comes first)
	//bool imat_depthmask = (imat->m_transparency_mode!=ALPHAMODE_TRANSLUCENT) && (imat->b_depthmask);
	//bool jmat_depthmask = (jmat->m_transparency_mode!=ALPHAMODE_TRANSLUCENT) && (jmat->b_depthmask);

	int i_order = PassOrder(&i.obj->m_passes[i.pass], i.obj);
	int j_order = PassOrder(&j.obj->m_passes[j.pass], j.obj);

	// Sort on the pass order:
	if (i_order != j_order)
	{
		return i_order < j_order;
	}

	// If the orders are equal, then we have different behavior for different order ranges

	// Opaque sorting:
	if (i_order < kPassOrder_DepthMaskDisabled)
	{
		// If the objects are the same, sort via order pass
		if (i.obj == j.obj)
		{
			return i.pass < j.pass;
		}
		// Sort by distance from camera, front-to-back
		return i.obj->renderDistance < j.obj->renderDistance;
	}
	// Transparent sorting:
	else if (j_order < kPassOrder_PostProcess)
	{
		// If the objects are the same, sort via order pass
		if (i.obj == j.obj)
		{
			return i.pass < j.pass;
		}
		// Sort by distance from camera, back-to-front
		return j.obj->renderDistance < i.obj->renderDistance;
	}
	// Post process sorting:
	else
	{
		// Sort by Z depth. Lower depth means rendered first.
		return i.obj->transform.local.position.z < j.obj->transform.local.position.z;
	}


	//// Sort on depth-write: depth comes first, always.
	//if (imat->m_depthWrite != jmat->m_depthWrite)
	//{
	//	return imat->m_depthTest;
	//}

	//// Opaque sorting:
	//if (imat->m_depthWrite)
	//{
	//	// If 

	//	// If the objects are the same, 
	//}
	//// Transparent sorting:
	//else
	//{
	//}

	///*if ( imat_depthmask != jmat_depthmask )
	//{
	//	// Sort first by depthmasking
	//	return imat_depthmask;
	//}
	//else*/
	//{
	//	// Perform opaque ordering
	//	if ( imat_depthmask )
	//	{
	//		// If the objects are the same, sort through increasing delay and increasing pass number
	//		if ( i.obj == j.obj )
	//		{
	//			if ( imat->m_delay != jmat->m_delay ) {
	//				return imat->m_delay < jmat->m_delay;
	//			}
	//			else {
	//				return i.pass < j.pass;
	//			}
	//		}
	//		else
	//		{
	//			// Check for screenshader
	//			if ( i.obj->m_material->m_isScreenShader || j.obj->m_material->m_isScreenShader )
	//			{
	//				// Render screenshader last.
	//				return !(i.obj->m_material->m_isScreenShader);
	//			}
	//			else
	//			{
	//				// Render front-to-back on opaque
	//				return ( (i.obj->renderDistance) < (j.obj->renderDistance) );
	//			}
	//		}
	//	}
	//	// Perform transparent ordering
	//	else 
	//	{
	//		// Check for screenshader
	//		if ( i.obj->m_material->m_isScreenShader || j.obj->m_material->m_isScreenShader )
	//		{
	//			// Render screenshader last.
	//			return !(i.obj->m_material->m_isScreenShader);
	//		}
	//		else
	//		{
	//			// Sort through render delay first
	//			if ( imat->m_delay != jmat->m_delay )
	//			{
	//				return imat->m_delay < jmat->m_delay;
	//			}

	//			// If the objects are the same, sort through increasing pass number
	//			if ( i.obj == j.obj )
	//			{
	//				return i.pass < j.pass;
	//			}
	//			else
	//			{
	//				// Render Back-to-front on transparent
	//				if ( fabs((i.obj->renderDistance)-(j.obj->renderDistance)) > FLOAT_PRECISION ) {
	//					return ( (i.obj->renderDistance) > (j.obj->renderDistance) );
	//				}
	//				else {
	//					return i.obj < j.obj;
	//				}
	//			}
	//		}
	//	}
	//}
}
// Render order control
//bool RrRenderer::render_deferred_comparator_t::operator() ( RrRenderer::tRenderRequest& i, RrRenderer::tRenderRequest& j )
//{
//	if ( j.obj == NULL ) {
//		throw std::invalid_argument("Null renderRequest");
//	}
//	if ( i.obj == NULL ) {
//		throw std::invalid_argument("Null renderRequest");
//	}
//
//	RrPassForward* imat, * jmat;
//	imat = i.obj->GetPass( i.pass );
//	jmat = j.obj->GetPass( j.pass );
//
//	// Returning true means i comes first.
//	// Returning false means j comes first.
//
//	// Sort first based on layers
//	if ( i.obj->renderLayer != j.obj->renderLayer ) {
//		return i.obj->renderLayer < j.obj->renderLayer;
//	}
//	// Sort next based on forward or not forward
//	if ( i.forward != j.forward ) {
//		return j.forward; // i comes first if j is forward
//	}
//	if ( i.obj->renderLayer == kRLV2D )
//	{	// Render back-to-front on 2D
//		return (i.obj->renderDistance) > (j.obj->renderDistance);
//	}
//	
//	// Check for depth writing (depth write comes first)
//	bool imat_depthmask = (imat->m_transparency_mode!=ALPHAMODE_TRANSLUCENT)&&(imat->b_depthmask);
//	bool jmat_depthmask = (jmat->m_transparency_mode!=ALPHAMODE_TRANSLUCENT)&&(jmat->b_depthmask);
//	/*
//	// Set transparency really quick here
//	i.transparent = !imat_depthmask;
//	j.transparent = !jmat_depthmask;
//	i.screenshader = i.obj->m_material->m_isScreenShader;
//	j.screenshader = j.obj->m_material->m_isScreenShader;
//	*/
//	if ( imat_depthmask != jmat_depthmask )
//	{
//		// Sort first by depthmasking
//		return imat_depthmask;
//	}
//	else
//	{
//		// Perform opaque ordering
//		if ( imat_depthmask )
//		{
//			// If the objects are the same, sort through increasing delay and increasing pass number
//			if ( i.obj == j.obj )
//			{
//				if ( imat->m_delay != jmat->m_delay ) {
//					return imat->m_delay < jmat->m_delay;
//				}
//				else {
//					return i.pass < j.pass;
//				}
//			}
//			else
//			{
//				// Check for screenshader
//				if ( i.obj->m_material->m_isScreenShader || j.obj->m_material->m_isScreenShader )
//				{
//					// Render screenshader last.
//					return !(i.obj->m_material->m_isScreenShader);
//				}
//				else
//				{
//					// Render front-to-back on opaque
//					return ( (i.obj->renderDistance) < (j.obj->renderDistance) );
//				}
//			}
//		}
//		// Perform transparent ordering
//		else 
//		{
//			// Check for screenshader
//			if ( i.obj->m_material->m_isScreenShader || j.obj->m_material->m_isScreenShader )
//			{
//				// Render screenshader last.
//				return !(i.obj->m_material->m_isScreenShader);
//			}
//			else
//			{
//				// Sort through render delay first
//				if ( imat->m_delay != jmat->m_delay )
//				{
//					return imat->m_delay < jmat->m_delay;
//				}
//
//				// If the objects are the same, sort through increasing pass number
//				if ( i.obj == j.obj )
//				{
//					return i.pass < j.pass;
//				}
//				else
//				{
//					// Render Back-to-front on transparent
//					if ( fabs((i.obj->renderDistance)-(j.obj->renderDistance)) > FLOAT_PRECISION ) {
//						return ( (i.obj->renderDistance) > (j.obj->renderDistance) );
//					}
//					else {
//						return i.obj < j.obj;
//					}
//				}
//			}
//		}
//	}
//}