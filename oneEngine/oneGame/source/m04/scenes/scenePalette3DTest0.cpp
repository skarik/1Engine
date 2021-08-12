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

#include "renderer/logic/model/RrCModel.h"
#include "renderer/object/mesh/Mesh.h"
#include "renderer/light/RrLight.h"

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
		RrRenderer::Active->GetWorld<0>()->pipeline_mode = renderer::PipelineMode::kPaletted;
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
		plane->transform.world.scale = Vector3f(300.0F, 300.0F, 300.0F);

		// Use a default material
		RrPass pass;
		pass.utilSetupAsDefault();
		pass.m_type = kPassTypeDeferred;
		pass.m_alphaMode = renderer::kAlphaModeNone;
		pass.m_cullMode = gpu::kCullModeNone;
		pass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
		pass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
		pass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
		pass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureBlack) );
		pass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );
		pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/deferred_env/simple_vv.spv", "shaders/deferred_env/simple_p.spv"}) );
		renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
												renderer::shader::Location::kUV0,
												renderer::shader::Location::kColor,
												renderer::shader::Location::kNormal,
												renderer::shader::Location::kTangent,
												renderer::shader::Location::kBinormal};
		pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
		pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;

		plane->PassInitWithInput(0, &pass);
	} loadScreen->loadStep();

	// Create a skysphere
	{
		RrShapeIsosphere* sphere = new RrShapeIsosphere();
		sphere->transform.world.scale = Vector3f(1500.0F, 1500.0F, 1500.0F);

		// Use a default material
		RrPass pass;
		pass.utilSetupAsDefault();
		pass.m_type = kPassTypeForward;
		pass.m_alphaMode = renderer::kAlphaModeNone;
		pass.m_cullMode = gpu::kCullModeNone;
		pass.m_layer = renderer::kRenderLayerBackground;
		pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/env/sky_tergo_vv.spv", "shaders/env/sky_tergo_p.spv"}) );
		renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition};
		pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
		pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;

		sphere->PassInitWithInput(0, &pass);
	} loadScreen->loadStep();

	// Create the grass
	{
		InstancedGrassRenderObject* foliage = new InstancedGrassRenderObject();

		const char* l_grassMeshes [2] = {"models/foliage/redgrass_clump_0", "models/foliage/redgrass_clump_1"};
		foliage->LoadGrassMeshes(l_grassMeshes, 2);
		foliage->PassAccess(0).setTexture(rrTextureSlot::TEX_DIFFUSE, RrTexture::Load("textures/foliage/hapgrass0_0.png"));
		foliage->PassAccess(0).setTexture(rrTextureSlot::TEX_SURFACE, RrTexture::Load("textures/foliage/hapgrass0_0_surface.png"));

		gpu::SamplerCreationDescription pointFilter;
		pointFilter.minFilter = core::gfx::tex::kSamplingPoint;
		pointFilter.magFilter = core::gfx::tex::kSamplingPoint;
		foliage->PassAccess(0).setSampler(rrTextureSlot::TEX_DIFFUSE, &pointFilter);
		foliage->PassAccess(0).setSampler(rrTextureSlot::TEX_NORMALS, &pointFilter);
		foliage->PassAccess(0).setSampler(rrTextureSlot::TEX_SURFACE, &pointFilter);
		foliage->PassAccess(0).setSampler(rrTextureSlot::TEX_OVERLAY, &pointFilter);

		for (uint i = 0; i < 120; ++i)
		{
			grInstancedGrassInfo grass;
			grass.color = Vector3f(1.0F, 1.0F, 1.0F);

			grass.variation_index = Random.Next(0, 1);

			Matrix4x4 transform, dump;
			core::TransformUtility::TRSToMatrix4x4(
				Vector3f(Random.Range(-5.0F, +5.0F), Random.Range(-5.0F, +5.0F), 0.0F),
				//Vector3f(Random.Range(-2.0F, +2.0F), Random.Range(-2.0F, +2.0F), 0.0F),
				Rotator(0.0F, 0.0F, Random.Range(0.0F, 360.0F)),
				Vector3f(1, 1, 1),
				transform,
				dump);
			grass.transform = transform;

			foliage->m_grassInfo.push_back(grass);
		}
	} loadScreen->loadStep();

	// Add a model
	//{
	//	RrCModel* model = RrCModel::Load(rrModelLoadParams{"models/blender_default_cube"}, NULL);
	//	//RrCModel* model = RrCModel::Load(rrModelLoadParams{"models/test0"}, NULL);
	//	//RrCModel* model = RrCModel::Load(rrModelLoadParams{"models/Go_blender"}, NULL);
	//	model->transform.position = Vector3f(0, 0, 5.0F);

	//	// Use a default material
	//	RrPass pass;
	//	pass.utilSetupAsDefault();
	//	pass.m_type = kPassTypeDeferred;
	//	pass.m_alphaMode = renderer::kAlphaModeNone;
	//	pass.m_cullMode = gpu::kCullModeNone;
	//	pass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	//	pass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
	//	pass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
	//	pass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureBlack) );
	//	pass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );
	//	pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/deferred_env/simple_vv.spv", "shaders/deferred_env/simple_p.spv"}) );
	//	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
	//											renderer::shader::Location::kUV0,
	//											renderer::shader::Location::kColor,
	//											renderer::shader::Location::kNormal,
	//											renderer::shader::Location::kTangent,
	//											renderer::shader::Location::kBinormal};
	//	pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
	//	pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;

	//	// Give initial pass
	//	for (int i = 0; i < model->GetMeshCount(); ++i)
	//	{
	//		model->GetMesh(i)->PassInitWithInput(0, &pass);
	//	}

	//} loadScreen->loadStep();

	// Add a rock
	{
		RrCModel* model = RrCModel::Load(rrModelLoadParams{"models/desert/rocks_0"}, NULL);
		model->transform.position = Vector3f(5.0F, 1.5F, 0);
		model->transform.scale = Vector3f(1, 1, 1) * 4.2F;
		model->transform.rotation = Rotator(Vector3f(0, 0, 45));

		// Use a default material
		RrPass pass;
		pass.utilSetupAsDefault();
		pass.m_type = kPassTypeDeferred;
		pass.m_alphaMode = renderer::kAlphaModeNone;
		pass.m_cullMode = gpu::kCullModeNone;
		pass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
		pass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
		pass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
		pass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureSurfaceM0) );
		pass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );
		pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/deferred_env/simple_vv.spv", "shaders/deferred_env/simple_p.spv"}) );
		renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
												renderer::shader::Location::kUV0,
												renderer::shader::Location::kColor,
												renderer::shader::Location::kNormal,
												renderer::shader::Location::kTangent,
												renderer::shader::Location::kBinormal};
		pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
		pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;

		// Give initial pass
		for (int i = 0; i < model->GetMeshCount(); ++i)
		{
			model->GetMesh(i)->PassInitWithInput(0, &pass);
		}

	} loadScreen->loadStep();

	// Add a directional light
	/*{
		RrLight* light = new RrLight;
		light->type = kLightTypeDirectional;
		light->direction = -Vector3f(0.7F, 0.2F, 0.7F).normal();
		light->color = Color(1.1, 0.2, 0.3);
	}

	{	// (Add another)
		RrLight* light = new RrLight;
		light->type = kLightTypeDirectional;
		light->direction = -Vector3f(-0.6F, -0.6F, 0.2F).normal();
		light->color = Color(0.3, 0.1, 0.9);
	}*/

	{
		RrLight* light = new RrLight;
		light->type = kLightTypeDirectional;
		light->direction = -Vector3f(0.7F, 0.2F, 0.7F).normal();
		light->color = Color(1.1, 1.0, 0.9) * 0.8F;
	}

	loadScreen->RemoveReference();
	CGameBehavior::DeleteObject(loadScreen);

	  // Print a prompt
	debug::Console->PrintMessage( "You are running a debug build of M04.\n" );
	debug::Console->PrintMessage( "This scene displays a simple 3D scene with paletted visuals.\n" );
}