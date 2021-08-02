#include "scenePalette3DTest0.h"

// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/Console.h"

// Include resource system in case want to muck around with manual tileset loading
#include "core-ext/system/io/Resources.h"
// Include renderer to change the pipeline
#include "renderer/state//RrRenderer.h"
// Include loading screen
#include "engine-common/entities/CLoadingScreen.h"


#include "engine-common/entities/CPlayer.h"
#include "renderer/object/shapes/RrShapePlane.h"
#include "renderer/object/shapes/RrShapeIsosphere.h"
#include "renderer/texture/RrTexture.h"
#include "renderer/material/RrShaderProgram.h"

#include "m04/entities_test/InstancedGrassRenderObject.h"

#include "core/math/random/Random.h"


void scenePalette3DTest0::LoadScene ( void )
{
	debug::Console->PrintMessage( "Loading scene: paletted 3d scene test.\n" );

	// Enable cursor
	//ActiveCursor->SetVisible(true);

	CLoadingScreen* loadScreen = new CLoadingScreen();
	loadScreen->loadStep();

	// Set up pipeline
	{
		RrRenderer::Active->GetWorld<0>()->pipeline_mode = renderer::kPipelineModePaletted;
		RrRenderer::Active->GetWorld<0>()->pipeline_options = nullptr; // TODO. Needs a Paletted pipeline.
	} loadScreen->loadStep();

	// Create player
	{
		CPlayer* player = new CPlayer();
		player->RemoveReference();
	} loadScreen->loadStep();

	// Create a plane to start with
	{
		RrShapePlane* plane = new RrShapePlane();

		// Use a default material
		RrPass pass;
		pass.utilSetupAsDefault();
		pass.m_type = kPassTypeForward;
		pass.m_alphaMode = renderer::kAlphaModeNone;
		pass.m_cullMode = gpu::kCullModeNone;
		pass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
		pass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
		pass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
		pass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureBlack) );
		pass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );
		pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/fullbright_vv.spv", "shaders/sys/fullbright_p.spv"}) );
		renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
												renderer::shader::Location::kUV0,
												renderer::shader::Location::kColor,
												renderer::shader::Location::kNormal,
												renderer::shader::Location::kTangent};
		pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
		pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;

		plane->PassInitWithInput(0, &pass);
	} loadScreen->loadStep();

	// Create a skysphere
	{
		RrShapeIsosphere* sphere = new RrShapeIsosphere();
		sphere->transform.world.scale = Vector3f(20.0F, 20.0F, 20.0F);

		// Use a default material
		RrPass pass;
		pass.utilSetupAsDefault();
		pass.m_type = kPassTypeForward;
		pass.m_alphaMode = renderer::kAlphaModeNone;
		pass.m_cullMode = gpu::kCullModeNone;
		pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/env/sky_hosek_wilkie_vv.spv", "shaders/env/sky_hosek_wilkie_p.spv"}) );
		renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition};
		pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
		pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;

		sphere->PassInitWithInput(0, &pass);
	} loadScreen->loadStep();

	// Create the grass
	{
		InstancedGrassRenderObject* foliage = new InstancedGrassRenderObject();

		for (uint i = 0; i < 100; ++i)
		{
			grInstancedGrassInfo grass;
			grass.color = Vector3f(1.0F, 1.0F, 1.0F);

			Matrix4x4 transform, dump;
			core::TransformUtility::TRSToMatrix4x4(
				Vector3f(Random.Range(-128, +128), Random.Range(-128, +128), 0.0F),
				Rotator(),
				Vector3f(1, 1, 1),
				transform,
				dump);
			grass.transform = transform;

			foliage->m_grassInfo.push_back(grass);
		}
	} loadScreen->loadStep();

	loadScreen->RemoveReference();
	CGameBehavior::DeleteObject(loadScreen);

	  // Print a prompt
	debug::Console->PrintMessage( "You are running a debug build of M04.\n" );
	debug::Console->PrintMessage( "This scene displays a simple 3D scene with paletted visuals.\n" );
}