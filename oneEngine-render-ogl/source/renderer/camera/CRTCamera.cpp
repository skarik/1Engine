
#include "CRTCamera.h"
#include "core/time/time.h"
#include "renderer/state/Settings.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/texture/CRenderTexture.h"

// Constructor
CRTCamera::CRTCamera (
		CRenderTexture*	targetTexture,
		Real			renderFramerate,
		bool			autoRender
		)
		: CCamera (), fRenderCounter(0)
{
	//m_type = CAMERA_TYPE_RT;
	//transform.name = "RTCamera Transform";

	zNear = 0.1f;
	zFar = 1000.0f;

	// On Default,  this is an RT camera
	bIsRTcamera = true;
	// And on default, do not update
	bNeedsUpdate = false;
	// Also, don't clear
	//clearColor = false;

	// Set default render layer mode
	for ( uint i = 0; i <= renderer::V2D; ++i ) {
		layerVisibility[i] = false;
	}
	layerVisibility[renderer::World] = true;

	// Set RT camera specific settings
	m_renderTexture		= targetTexture;
	fRenderFramerate	= renderFramerate;
	bAutoRender			= autoRender;
}

// Destructor
CRTCamera::~CRTCamera ( void )
{
	// Not sure yet.
}

// Update
void CRTCamera::LateUpdate ( void )
{
	// If autorender, then tell scene to update at given framerate
	if ( bAutoRender )
	{
		if ( bNeedsUpdate ) {
			bNeedsUpdate = false;
		}
		fRenderCounter += CTime::deltaTime;
		if ( fRenderCounter > 1/fRenderFramerate )
		{
			fRenderCounter = 0;
			bNeedsUpdate = true;
		}
	}
	// However, if there's not a valid render target, turn rendering off
	if ( m_renderTexture == NULL )
	{
		bNeedsUpdate = false;
	}
}

// Render set
void CRTCamera::RenderSet ( void )
{
	// Set viewport percents
	if ( m_renderTexture )
	{
		unsigned int iPixelWidth;
		unsigned int iPixelHeight;

		// Grab render target sizes
		iPixelWidth	= m_renderTexture->GetWidth();
		iPixelHeight= m_renderTexture->GetHeight();

		// Update viewport
		viewport.pos.x = viewport_percent.pos.x * iPixelWidth;
		viewport.pos.y = viewport_percent.pos.y * iPixelHeight;
		viewport.size.x = viewport_percent.size.x * iPixelWidth;
		viewport.size.y = viewport_percent.size.y * iPixelHeight;

		// Bind frame buffer
		m_renderTexture->BindBuffer();
	}
	else
	{
		std::cout << "Rendering with no RT on RTCamera " << this << std::endl;
	}

	// Call the parent one
	CCamera::RenderSet();

	// Update the texture matrix
	//UpdateTextureMatrix();

	// Bind frame buffer
	//if ( myRenderTexture )
	//	myRenderTexture->BindBuffer();

}

// Render clean
void CRTCamera::RenderUnset ( void )
{
	// Unbind framebuffer
	if ( m_renderTexture )
		m_renderTexture->UnbindBuffer();
	//else
	//	CRenderTexture::UnbindBuffer( 0 );
}

// Update Camera Matrix
void CRTCamera::UpdateMatrix ( void )
{
	CCamera::UpdateMatrix();

	// Update the texture matrix
	UpdateTextureMatrix();
}

// Update Texture Matrix
void CRTCamera::UpdateTextureMatrix ( void )
{
	/*static double modelView[16];
	static double projection[16];
	
	// Moving from unit cube [-1,1] to [0,1]  
	const GLdouble bias[16] = {	
		0.5, 0.0, 0.0, 0.0, 
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	};
	
	// Grab modelview and transformation matrices
	//glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	//glGetDoublev(GL_PROJECTION_MATRIX, projection); // SO ARE THESE MATRICES UP HERE CORRECT?
	memcpy( modelView,  this->viewTransform.pData, sizeof(Real)*16 );
	memcpy( projection, this->projTransform.pData, sizeof(Real)*16 );

	// THIS COULD BE WRONG
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glPushMatrix();

	glLoadIdentity(); // BUT I LOAD IDENTITY HERE
	glLoadMatrixd (bias);
	// concatating all matrices into one.
	glMultMatrixd (projection);	// IS THIS PROJECTION NEEDED? (hint: YES IT IS.)
	glMultMatrixd (modelView);

	float currentProjection[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, currentProjection);
	textureMatrix = Matrix4x4( currentProjection );
	//textureMatrix = textureMatrix.inverse();

	glPopMatrix();
	glPopMatrix();*/
	//glMatrixMode(GL_MODELVIEW);

	const Real bias[16] = {	
		0.5, 0.0, 0.0, 0.0, 
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	};
	const Matrix4x4 biasMatrix ( bias );
	textureMatrix = viewTransform * projTransform * biasMatrix;
}


void CRTCamera::SetTarget ( CRenderTexture* n_rt )
{
	m_renderTexture = n_rt;
}
void CRTCamera::SetAutorender ( bool n_autorender )
{
	bAutoRender = n_autorender;
}
void CRTCamera::SetUpdateFPS ( Real n_updatefps )
{
	fRenderFramerate = n_updatefps;
}