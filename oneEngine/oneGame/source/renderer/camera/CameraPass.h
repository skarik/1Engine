#ifndef RENDERER_CAMERA_PASS_H_
#define RENDERER_CAMERA_PASS_H_

#include "core/math/Frustum.h"
#include "core/math/Rect.h"
#include "core/math/matrix/CMatrix.h"

enum rrCameraRenderType
{
	// Will run the 5x5 forward+deferred hybrid renderer
	kCameraRenderWorld,

	// Will run the forward+ shadow color renderer
	kCameraRenderShadow,
};

class RrHybridBufferChain;

struct rrCameraPass
{
	// How does this camera act for this given pass?
	rrCameraRenderType	m_passType;

	// target viewport rendering to
	Rect				m_viewport;

	// NULL to use the main engine's buffer chain.
	// Otherwise, points to the buffer chain to use for rendering.
	// An RT camera will properly create their own buffer chain
	RrHybridBufferChain*m_bufferChain;

	// rendering properties
	Matrix4x4			m_viewTransform;
	Matrix4x4			m_projTransform;
	Matrix4x4			m_viewprojTransform;
	core::math::Frustum	m_frustum;
};

#endif//RENDERER_CAMERA_PASS_H_