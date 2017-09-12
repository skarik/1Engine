#include "core/time.h"

#include "renderer/material/RrShader.h"
#include "renderer/gpuw/Buffers.h"

#include "RrMaterial.h"

static gpu::ConstantBuffer l_cbufPerFrame;
static gpu::ConstantBuffer l_cbufPerPassLighting;
static gpu::ConstantBuffer l_cbufPerCamera;

// static
void	RrMaterial::pushConstantsPerFrame ( void )
{
	renderer::cbuffer::rrPerFrame perFrame = {0};

	// Set time constants
	perFrame.time = Vector4f(
		Time::currentTime / 20.0F,
		Time::currentTime,
		Time::currentTime * 2.0F,
		Time::currentTime * 3.0F);
	perFrame.sinTime = Vector4f(
		sinf( Time::currentTime / 8.0F ),
		sinf( Time::currentTime / 4.0F ),
		sinf( Time::currentTime / 2.0F ),
		sinf( Time::currentTime / 1.0F ));
	perFrame.cosTime = Vector4f(
		cosf( Time::currentTime / 8.0F ),
		cosf( Time::currentTime / 4.0F ),
		cosf( Time::currentTime / 2.0F ),
		cosf( Time::currentTime / 1.0F ));

	// Set distance fog constants

	// Upload the constant buffer
	l_cbufPerFrame.upload(&perFrame, sizeof(perFrame), gpu::kTransferStream);
}
// static
void	RrMaterial::pushConstantsPerPass ( void )
{
	renderer::cbuffer::rrPerPassLightingInfo perPassLighting = {0};

	// Upload the constant buffer
	l_cbufPerPassLighting.upload(&perPassLighting, sizeof(perPassLighting), gpu::kTransferStream);
}
// static
void	RrMaterial::pushConstantsPerCamera ( void )
{
	renderer::cbuffer::rrPerCamera perCamera = {0};

	// Upload the constant buffer
	l_cbufPerCamera.upload(&perCamera, sizeof(perCamera), gpu::kTransferStream);
}



void RrMaterial::shader_set_constantbuffers( RrShader* shader )
{
	if (!shader->isPrimed() || true)
	{
		/*glBindBufferRange(GL_UNIFORM_BUFFER,
			renderer::CBUFFER_PER_FRAME_INFORMATION,
			l_cbufPerFrame.m_buffer,
			NIL,
			sizeof(perFrame) );*/

		//ctx.shader_set_constant_buffer( renderer::CBUFFER_PER_FRAME_INFORMATION, l_cbufPerFrame.buffer(), NIL, sizeof(renderer::cbuffer::rrPerFrame) );

		// Now that buffers are set and pointed, prime it.
		shader->prime();
	}
}