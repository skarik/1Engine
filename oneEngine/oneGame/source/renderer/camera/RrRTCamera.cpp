#include "core/time/time.h"
#include "renderer/state/Settings.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/texture/RrRenderTexture.h"
#include "renderer/state/RrHybridBufferChain.h"

#include "RrRTCamera.h"

RrRTCamera::RrRTCamera (
	renderer::rrInternalSettings* const targetSettings,
	Vector2i const& targetSize,
	Real renderFramerate,
	bool autoRender
	) : RrCamera (),
	m_renderCounter(0),
	m_renderStepTime(1.0F / renderFramerate),
	m_autoRender(autoRender)
{
	layerVisibility[renderer::kRenderLayerSecondary] = false;
	layerVisibility[renderer::kRenderLayerV2D] = false;

	// Set render texture immediately
	//m_renderTexture		= targetTexture;

	// Create the buffer chain immediately.
	SetTargetInfo(targetSettings, targetSize);
}

RrRTCamera::~RrRTCamera ( void )
{
	;
}

//	SetTargetInfo(settings, size) : Sets up render target for the camera.
// Will free previously created buffers.
// Returns true on successful creation.
bool RrRTCamera::SetTargetInfo ( renderer::rrInternalSettings* const settings, Vector2i const& size )
{
	m_usedTargetSettings = *settings;
	m_targetSize = size;

	gpu::ErrorCode status = m_chain.CreateTargetBufferChain(&m_usedTargetSettings, size);
	return status == gpu::kError_SUCCESS;
}
//	FreeTarget() : Frees up the render target.
bool RrRTCamera::FreeTarget ( void )
{
	bool status = m_chain.FreeTargetBufferChain();
	return status;
}

// Update
void RrRTCamera::LateUpdate ( void )
{
	// If autorender, then tell scene to update at given framerate
	if ( m_autoRender )
	{
		if ( m_needsNewPasses ) {
			m_needsNewPasses = false;
		}
		m_renderCounter += Time::deltaTime;
		if ( m_renderCounter > m_renderStepTime )
		{
			m_renderCounter = 0.0F; // TODO: why not a subtract+limit
			m_needsNewPasses = true;
		}
	}
	// However, if there's not a valid render target, turn rendering off
	//if ( m_renderTexture == NULL )
	if ( !active )
	{
		m_needsNewPasses = false;
	}

	// Perform late update, which will update the viewport size to an incorrect value:
	RrCamera::LateUpdate();

	// So we set the proper viewport now:
	viewport.pos.x  = viewportPercent.pos.x * m_targetSize.x;
	viewport.pos.y  = viewportPercent.pos.y * m_targetSize.y;
	viewport.size.x = viewportPercent.size.x * m_targetSize.x;
	viewport.size.y = viewportPercent.size.y * m_targetSize.y;
}

// Update Camera Matrix
void RrRTCamera::UpdateMatrix ( void )
{
	// Update the projection matrix normally.
	RrCamera::UpdateMatrix();

	// Update the texture matrix after the camera matrix is updated
	UpdateTextureMatrix();
}

// Update Texture Matrix
void RrRTCamera::UpdateTextureMatrix ( void )
{
	const Real bias[16] = {	
		0.5F, 0.0F, 0.0F, 0.0F, 
		0.0F, 0.5F, 0.0F, 0.0F,
		0.0F, 0.0F, 0.5F, 0.0F,
		0.5F, 0.5F, 0.5F, 1.0F
	};

	const Matrix4x4 biasMatrix ( bias );
	//textureMatrix = viewTransform * projTransform * biasMatrix;
	textureMatrix = viewprojMatrix * biasMatrix;
}

//	PassCount() : Returns number of passes this camera will render
// Must be 1 or greater in order to render.
int RrRTCamera::PassCount ( void )
{
	return 1;
}
//	PassRetrieve(array, array_size) : Writes pass information into the array given in
// Will write either PassCount() or maxPasses passes, whatever is smaller.
void RrRTCamera::PassRetrieve ( const rrCameraPassInput* input, rrCameraPass* passList )
{
	RrCamera::PassRetrieve(input, passList); // TODO: Proper buffer chain!
	if (input->m_maxPasses > 0)
	{
		passList[0].m_bufferChain = &m_chain;
	}
}

void RrRTCamera::RenderBegin ( void )
{
	// Call the parent one
	RrCamera::RenderBegin();

	// bind to m_renderTexture ???
	// todo
}

void RrRTCamera::RenderEnd ( void )
{
	RrCamera::RenderEnd();
}

