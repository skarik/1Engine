#include "core/time/time.h"
#include "renderer/state/Settings.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/texture/RrRenderTexture.h"

#include "RrRTCamera.h"

// Constructor
RrRTCamera::RrRTCamera (
		RrRenderTexture*	targetTexture,
		Real				renderFramerate,
		bool				autoRender
		) : RrCamera (),
	m_renderCounter(0),
	m_renderStepTime(1.0F / renderFramerate),
	m_autoRender(autoRender)
{
	layerVisibility[renderer::kRLSecondary] = false;
	layerVisibility[renderer::kRLV2D] = false;

	// Set render texture immediately
	m_renderTexture		= targetTexture;
}

// Destructor
RrRTCamera::~RrRTCamera ( void )
{
	// Not sure yet.
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
		m_renderCounter += CTime::deltaTime;
		if ( m_renderCounter > m_renderStepTime )
		{
			m_renderCounter = 0;
			m_needsNewPasses = true;
		}
	}
	// However, if there's not a valid render target, turn rendering off
	if ( m_renderTexture == NULL )
	{
		m_needsNewPasses = false;
	}

	// Perform late update
	RrCamera::LateUpdate();
}

// Render set
void RrRTCamera::RenderBegin ( void )
{
	// Set viewport percents
	/*if ( m_renderTexture )
	{
		unsigned int iPixelWidth;
		unsigned int iPixelHeight;

		// Grab render target sizes
		iPixelWidth	= m_renderTexture->GetWidth();
		iPixelHeight= m_renderTexture->GetHeight();

		// Update viewport
		viewport.pos.x = viewportPercent.pos.x * iPixelWidth;
		viewport.pos.y = viewportPercent.pos.y * iPixelHeight;
		viewport.size.x = viewportPercent.size.x * iPixelWidth;
		viewport.size.y = viewportPercent.size.y * iPixelHeight;

		// Bind frame buffer
		m_renderTexture->BindBuffer();
	}
	else
	{
		std::cout << "Rendering with no RT on RTCamera " << this << std::endl;
	}*/

	// Call the parent one
	RrCamera::RenderBegin();

	// bind to m_renderTexture ???
	// todo
}

// Render clean
void RrRTCamera::RenderEnd ( void )
{
	// Unbind framebuffer
	//if ( m_renderTexture )
	//	m_renderTexture->UnbindBuffer();
}

// Update Camera Matrix
void RrRTCamera::UpdateMatrix ( void )
{
	RrCamera::UpdateMatrix();
	// Update the texture matrix after the camera matrix is updated
	UpdateTextureMatrix();
}

// Update Texture Matrix
void RrRTCamera::UpdateTextureMatrix ( void )
{
	const Real bias[16] = {	
		0.5, 0.0, 0.0, 0.0, 
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	};

	const Matrix4x4 biasMatrix ( bias );
	textureMatrix = viewTransform * projTransform * biasMatrix;
}

void RrRTCamera::SetAutorender ( bool n_autorender )
{
	m_autoRender = n_autorender;
}

void RrRTCamera::SetUpdateFPS ( Real n_updatefps )
{
	m_renderStepTime = 1.0F / n_updatefps;
}