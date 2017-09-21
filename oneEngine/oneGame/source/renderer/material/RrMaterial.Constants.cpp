#include "core/time.h"
#include "core/system/Screen.h"

#include "renderer/material/RrShader.h"
#include "renderer/gpuw/Buffers.h"
#include "renderer/system/glSystem.h"
#include "renderer/state/Settings.h"
#include "renderer/camera/CCamera.h"

#include "RrMaterial.h"

static gpu::ConstantBuffer l_cbufPerFrame;
static gpu::ConstantBuffer l_cbufPerPassLighting;
static gpu::ConstantBuffer l_cbufPerCamera;

// static
void	RrMaterial::pushConstantsPerFrame ( void )
{
	renderer::cbuffer::rrPerFrame perFrame = {};

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
	perFrame.fogColor = Vector4f(
		renderer::Settings.fogColor.r,
		renderer::Settings.fogColor.g,
		renderer::Settings.fogColor.b,
		1.0F);
	Color t_atmoColor = Color::Lerp( renderer::Settings.fogColor, renderer::Settings.ambientColor, 0.5F ) * Color(0.9F,0.9F,1.1F);
	perFrame.atmoColor = Vector4f(
		t_atmoColor.r,
		t_atmoColor.g,
		t_atmoColor.b,
		1.0F);
	perFrame.fogEnd = renderer::Settings.fogEnd;
	perFrame.fogScale = renderer::Settings.fogScale;

	// Upload the constant buffer
	l_cbufPerFrame.upload(&perFrame, sizeof(perFrame), gpu::kTransferStream);
}
// static
void	RrMaterial::pushConstantsPerPass ( void )
{
	renderer::cbuffer::rrPerPassLightingInfo perPassLighting = {};

	// Set light number
	perPassLighting.lightNumber = m_lightCount;
	
	// Set fallback values
	perPassLighting.ambientLightFallback = Vector4f(
		renderer::Settings.ambientColor.r,
		renderer::Settings.ambientColor.g,
		renderer::Settings.ambientColor.b,
		1.0F);

	// Upload the constant buffer
	l_cbufPerPassLighting.upload(&perPassLighting, sizeof(perPassLighting), gpu::kTransferStream);
}
// static
void	RrMaterial::pushConstantsPerCamera ( void )
{
	renderer::cbuffer::rrPerCamera perCamera = {};

	// Set camera constants
	GL_ACCESS;
	perCamera.viewProjection = CCamera::activeCamera->camera_VP; //GL.getProjection();
	perCamera.worldCameraPosition = CCamera::activeCamera->transform.position;
	perCamera.viewportInfo = Vector4f(
		0.0F,
		0.0F,
		(Real32)Screen::Info.width * CCamera::activeCamera->render_scale,
		(Real32)Screen::Info.height * CCamera::activeCamera->render_scale);
	perCamera.screenSize = Vector2f(
		(Real32)Screen::Info.width,
		(Real32)Screen::Info.height);
	perCamera.pixelRatio = Vector2f(
		(Real32)CCamera::activeCamera->ortho_size.x / Screen::Info.width / CCamera::activeCamera->render_scale,
		(Real32)CCamera::activeCamera->ortho_size.x / Screen::Info.width / CCamera::activeCamera->render_scale);

	// Upload the constant buffer
	l_cbufPerCamera.upload(&perCamera, sizeof(perCamera), gpu::kTransferStream);
}

// dynamic
void RrMaterial::pushConstantsPerObject ( const Matrix4x4& modelTRS, const Matrix4x4& modelRS )
{
	struct rrPerObject
	{
		renderer::cbuffer::rrPerObjectMatrices	matrices;
		renderer::cbuffer::rrPerObjectSurface	surface[16];
	} perObject = {};

	// Set up matrices
	GL_ACCESS;
	perObject.matrices.modelTRS = modelTRS;
	perObject.matrices.modelRS = modelRS;
	perObject.matrices.modelViewProjection = modelTRS * CCamera::activeCamera->camera_VP; //GL.getProjection();
	perObject.matrices.modelViewProjectionInverse = perObject.matrices.modelViewProjection.inverse();

	// Todo: optimize update via the passinfo[i].m_dirty and deferredinfo[i].m_dirty flags.

	// Set up surface
	uint offset_forward = 0;
	uint offset_deferred = passinfo.size();
	uint offset_max = offset_deferred + deferredinfo.size();
	for (uint i = 0; i < passinfo.size(); ++i)
	{
		uint surface_index = offset_forward + i;

		perObject.surface[surface_index].diffuseColor  = Vector4f(m_diffuse.r, m_diffuse.g, m_diffuse.b, m_diffuse.a);
		perObject.surface[surface_index].emissiveColor = Vector3f(m_emissive.r, m_emissive.g, m_emissive.b);
		perObject.surface[surface_index].alphaCutoff = -1.0F;
		if (passinfo[i].m_transparency_mode == renderer::ALPHAMODE_ALPHATEST)
			perObject.surface[surface_index].alphaCutoff = passinfo[i].f_alphatest_value;
		perObject.surface[surface_index].specularColor = Vector4f(m_specular.r, m_specular.g, m_specular.b, m_specular.a);

		perObject.surface[surface_index].lightingOverrides = Vector3f(
			0.0F,
			/*passinfo[i].m_rimlight_strength*/ 0.0F,
			0.0F);

		perObject.surface[surface_index].textureScale = Vector4f(
			(Real32)m_texcoordScaling.x, (Real32)m_texcoordScaling.y,
			(Real32)m_texcoordScaling.z, (Real32)m_texcoordScaling.w);
		perObject.surface[surface_index].textureOffset = Vector4f(
			(Real32)m_texcoordOffset.x, (Real32)m_texcoordOffset.y,
			(Real32)m_texcoordOffset.z, (Real32)m_texcoordOffset.w);
	}
	for (uint i = 0; i < deferredinfo.size(); ++i)
	{
		uint surface_index = offset_deferred + i;

		perObject.surface[surface_index].diffuseColor  = Vector4f(m_diffuse.r, m_diffuse.g, m_diffuse.b, m_diffuse.a);
		perObject.surface[surface_index].emissiveColor = Vector3f(m_emissive.r, m_emissive.g, m_emissive.b);
		perObject.surface[surface_index].alphaCutoff = -1.0F;
		if (deferredinfo[i].m_transparency_mode == renderer::ALPHAMODE_ALPHATEST)
			perObject.surface[surface_index].alphaCutoff = 0.5F;
		perObject.surface[surface_index].specularColor = Vector4f(m_specular.r, m_specular.g, m_specular.b, m_specular.a);

		perObject.surface[surface_index].lightingOverrides = Vector3f(
			0.0F,
			deferredinfo[i].m_rimlight_strength,
			0.0F);

		perObject.surface[surface_index].textureScale = Vector4f(
			(Real32)m_texcoordScaling.x, (Real32)m_texcoordScaling.y,
			(Real32)m_texcoordScaling.z, (Real32)m_texcoordScaling.w);
		perObject.surface[surface_index].textureOffset = Vector4f(
			(Real32)m_texcoordOffset.x, (Real32)m_texcoordOffset.y,
			(Real32)m_texcoordOffset.z, (Real32)m_texcoordOffset.w);
	}

	// Upload the constant buffer
	m_cbufPerObject.upload(
		&perObject,
		sizeof(renderer::cbuffer::rrPerObjectMatrices) + sizeof(renderer::cbuffer::rrPerObjectSurface)*offset_max,
		gpu::kTransferStream);
}


void RrMaterial::shader_set_constantbuffers( RrShader* shader, uchar pass, bool forward )
{
	if (!shader->isPrimed() || true)
	{
		uint surface_offset = 0;
		if (forward)
		{	// Forward indexing. They are placed like an array in the buffer.
			surface_offset = pass;
		}
		else
		{	// Deferred indexing. The deferred data is placed after the forward data.
			surface_offset = passinfo.size() + pass;
		}

		//ctx.shader_set_constant_buffer( renderer::CBUFFER_PER_FRAME_INFORMATION, l_cbufPerFrame.buffer(), NIL, sizeof(renderer::cbuffer::rrPerFrame) );
		glBindBufferRange(GL_UNIFORM_BUFFER,
			renderer::CBUFFER_PER_FRAME_INFORMATION,
			l_cbufPerFrame.getGlIndex(),
			NIL,
			sizeof(renderer::cbuffer::rrPerFrame) );

		glBindBufferRange(GL_UNIFORM_BUFFER,
			renderer::CBUFFER_PER_PASS_INFORMATION,
			l_cbufPerPassLighting.getGlIndex(),
			NIL,
			sizeof(renderer::cbuffer::rrPerPassLightingInfo) );

		glBindBufferRange(GL_UNIFORM_BUFFER,
			renderer::CBUFFER_PER_CAMERA_INFORMATION,
			l_cbufPerCamera.getGlIndex(),
			NIL,
			sizeof(renderer::cbuffer::rrPerCamera) );

		glBindBufferRange(GL_UNIFORM_BUFFER,
			renderer::CBUFFER_PER_OBJECT_MATRICES,
			m_cbufPerObject.getGlIndex(),
			NIL,
			sizeof(renderer::cbuffer::rrPerObjectMatrices) );
		glBindBufferRange(GL_UNIFORM_BUFFER,
			renderer::CBUFFER_PER_OBJECT_EXTENDED,
			m_cbufPerObject.getGlIndex(),
			(GLintptr)sizeof(renderer::cbuffer::rrPerObjectMatrices) + sizeof(renderer::cbuffer::rrPerObjectSurface)*surface_offset,
			sizeof(renderer::cbuffer::rrPerObjectSurface) );

		// Now that buffers are set and pointed, prime it.
		shader->prime();
	}
}