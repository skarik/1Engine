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
#include "renderer/texture/RrTexture3D.h"
#include "renderer/texture/RrTextureCube.h"
#include "renderer/material/RrShaderProgram.h"

#include "renderer/logic/model/RrCModel.h"
#include "renderer/object/mesh/Mesh.h"
#include "renderer/light/RrLight.h"

#include "m04/entities_test/InstancedGrassRenderObject.h"

#include "core/math/random/Random.h"

#include "renderer/state/pipeline/RrPipelinePaletted.h"


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
		
		//RrRenderer::Active->GetWorld<0>()->pipeline_options = nullptr; // TODO. Needs a Paletted pipeline.
		RrPipelinePalettedOptions* options = new RrPipelinePalettedOptions;
		options->m_celShadeLighting = true;
		RrRenderer::Active->GetWorld<0>()->pipeline_options = options;

	} loadScreen->loadStep();

	// Create player
	{
		CPlayer* player = new CPlayer();
		player->RemoveReference();
	} loadScreen->loadStep();

	// Create a plane to start with
	/*{
		RrShapePlane* plane = new RrShapePlane();
		plane->transform.world.scale = Vector3f(300.0F, 300.0F, 300.0F);

		// Use a default material
		RrPass pass;
		pass.utilSetupAsDefault();
		pass.m_type = kPassTypeDeferred;
		pass.m_alphaMode = renderer::kAlphaModeNone;
		pass.m_cullMode = gpu::kCullModeNone;
		pass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
		//pass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
		pass.setTexture( TEX_DIFFUSE, RrTexture::Load("textures/desert/flatsand0.png") );
		pass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
		//pass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureBlack) );
		pass.setTexture( TEX_SURFACE, RrTexture::Load("textures/desert/flatsand0_surf.png") );
		pass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );

		pass.m_surface.textureScale = Vector4f(1, 1, 1, 1) * 300.0F / 2.0F;

		gpu::SamplerCreationDescription pointFilter;
		pointFilter.minFilter = core::gfx::tex::kSamplingPoint;
		pointFilter.magFilter = core::gfx::tex::kSamplingPoint;
		pass.setSampler(rrTextureSlot::TEX_DIFFUSE, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_NORMALS, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_SURFACE, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_OVERLAY, &pointFilter);

		pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/deferred_env/simple_vv.spv", "shaders/deferred_env/tilebomber_p.spv"}) );
		renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
												renderer::shader::Location::kUV0,
												renderer::shader::Location::kColor,
												renderer::shader::Location::kNormal,
												renderer::shader::Location::kTangent,
												renderer::shader::Location::kBinormal};
		pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
		pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
		
		pass.m_renderCallback = [](gpu::GraphicsContext* gfx)
		{
			renderer::cbuffer::tilebomber::rrTilebombParams params;
			{
				params.base_texture_uv_position = Vector2f(0, 0);
				params.base_texture_uv_size = Vector2f(0.25F, 0.25F);
				params.bomb_chance = 0.5F;
				params.bomb_scale = 1;

				params.bomb_count = 4;

				// bricks
				params.bomb_texture[0].uv_position = Vector2f(0.25F, 0.50F);
				params.bomb_texture[0].uv_size = Vector2f(0.25F, 0.25F);
				params.bomb_texture[0].min_repeat = 2;
				params.bomb_texture[0].max_repeat = 4;
				// sand details
				params.bomb_texture[1].uv_position = Vector2f(0.0F, 0.25F);
				params.bomb_texture[1].uv_size = Vector2f(0.25F, 0.25F);
				params.bomb_texture[1].occurrence = Vector4f(1, 3, 3, 3).normal();
				// dirt & dirt
				params.bomb_texture[2].uv_position = Vector2f(0.25F, 0.0F);
				params.bomb_texture[2].uv_size = Vector2f(0.25F, 0.25F);
				params.bomb_texture[2].min_repeat = 1;
				params.bomb_texture[2].max_repeat = 3;
				params.bomb_texture[2].occurrence = Vector4f(2, 1, 4, 3).normal();
				// black plants
				params.bomb_texture[3].uv_position = Vector2f(0.25F, 0.25F);
				params.bomb_texture[3].uv_size = Vector2f(0.25F, 0.25F);
				params.bomb_texture[3].min_repeat = 1;
				params.bomb_texture[3].max_repeat = 5;
				params.bomb_texture[3].occurrence = Vector4f(1, 1, 1, 0).normal();
			}
			gpu::Buffer tilebomber_buffer;
			tilebomber_buffer.initAsConstantBuffer(NULL, sizeof(params));
			tilebomber_buffer.upload(gfx, &params, sizeof(params), gpu::kTransferStream);
			gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_USER0, &tilebomber_buffer);
			tilebomber_buffer.free(NULL); // It is currently in use, so this will be deferred.
		};

		plane->PassInitWithInput(0, &pass);
	} loadScreen->loadStep();*/

	{
		RrCModel* model = RrCModel::Load(rrModelLoadParams{"models/sky/floatrock_0"}, NULL);

		// Use a default material
		RrPass pass;
		pass.utilSetupAsDefault();
		pass.m_type = kPassTypeDeferred;
		pass.m_alphaMode = renderer::kAlphaModeNone;
		pass.m_cullMode = gpu::kCullModeNone;
		pass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
		pass.setTexture( TEX_DIFFUSE, RrTexture::Load("textures/desert/flatsand0.png") );
		pass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
		pass.setTexture( TEX_SURFACE, RrTexture::Load("textures/desert/flatsand0_surf.png") );
		pass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );

		pass.m_surface.textureScale = Vector4f(1, 1, 1, 1) * 3.0F / 2.0F;

		gpu::SamplerCreationDescription pointFilter;
		pointFilter.minFilter = core::gfx::tex::kSamplingPoint;
		pointFilter.magFilter = core::gfx::tex::kSamplingPoint;
		pass.setSampler(rrTextureSlot::TEX_DIFFUSE, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_NORMALS, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_SURFACE, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_OVERLAY, &pointFilter);

		pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/deferred_env/simple_vv.spv", "shaders/deferred_env/tilebomber_p.spv"}) );
		renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
												renderer::shader::Location::kUV0,
												renderer::shader::Location::kColor,
												renderer::shader::Location::kNormal,
												renderer::shader::Location::kTangent,
												renderer::shader::Location::kBinormal};
		pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
		pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
		
		pass.m_renderCallback = [](gpu::GraphicsContext* gfx)
		{
			renderer::cbuffer::tilebomber::rrTilebombParams params;
			{
				params.base_texture_uv_position = Vector2f(0, 0);
				params.base_texture_uv_size = Vector2f(0.25F, 0.25F);
				params.bomb_chance = 0.5F;
				params.bomb_scale = 1;

				params.bomb_count = 4;

				// bricks
				params.bomb_texture[0].uv_position = Vector2f(0.25F, 0.50F);
				params.bomb_texture[0].uv_size = Vector2f(0.25F, 0.25F);
				params.bomb_texture[0].min_repeat = 2;
				params.bomb_texture[0].max_repeat = 4;
				// sand details
				params.bomb_texture[1].uv_position = Vector2f(0.0F, 0.25F);
				params.bomb_texture[1].uv_size = Vector2f(0.25F, 0.25F);
				params.bomb_texture[1].occurrence = Vector4f(1, 3, 3, 3).normal();
				// dirt & dirt
				params.bomb_texture[2].uv_position = Vector2f(0.25F, 0.0F);
				params.bomb_texture[2].uv_size = Vector2f(0.25F, 0.25F);
				params.bomb_texture[2].min_repeat = 1;
				params.bomb_texture[2].max_repeat = 3;
				params.bomb_texture[2].occurrence = Vector4f(2, 1, 4, 3).normal();
				// black plants
				params.bomb_texture[3].uv_position = Vector2f(0.25F, 0.25F);
				params.bomb_texture[3].uv_size = Vector2f(0.25F, 0.25F);
				params.bomb_texture[3].min_repeat = 1;
				params.bomb_texture[3].max_repeat = 5;
				params.bomb_texture[3].occurrence = Vector4f(1, 1, 1, 0).normal();
			}
			gpu::Buffer tilebomber_buffer;
			tilebomber_buffer.initAsConstantBuffer(NULL, sizeof(params));
			tilebomber_buffer.upload(gfx, &params, sizeof(params), gpu::kTransferStream);
			gfx->setShaderCBuffer(gpu::kShaderStagePs, renderer::CBUFFER_USER0, &tilebomber_buffer);
			tilebomber_buffer.free(NULL); // It is currently in use, so this will be deferred.
		};

		// Give initial pass
		for (int i = 0; i < model->GetMeshCount(); ++i)
		{
			model->GetMesh(i)->PassInitWithInput(0, &pass);
		}
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
		//pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/env/sky_cubemap_vv.spv", "shaders/env/sky_cubemap_p.spv"}) );
		pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/env/sky_tergo_vv.spv", "shaders/env/sky_tergo_p.spv"}) );

		pass.setTexture( TEX_SLOT0, RrTextureCube::Load("textures/sky/sky0") );
		pass.setTexture( TEX_SLOT1, RrTexture3D::Load("textures/sky/worley") );

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
				Vector3f(Random.PointInUnitCircle() * 2.4F, 0.3F),
				Rotator(0.0F, 0.0F, Random.Range(0.0F, 360.0F)),
				Vector3f(1, 1, Random.Range(0.8F, 1.1F)) * 0.8F,
				transform,
				dump);
			grass.transform = transform;

			foliage->m_grassInfo.push_back(grass);
		}
	} loadScreen->loadStep();

	// Add a model
	/*{
		//RrCModel* model = RrCModel::Load(rrModelLoadParams{"models/blender_default_cube"}, NULL);
		//RrCModel* model = RrCModel::Load(rrModelLoadParams{"models/test0"}, NULL);
		RrCModel* model = RrCModel::Load(rrModelLoadParams{"models/Go_blender"}, NULL);
		model->transform.position = Vector3f(0, 0, 1.0F);

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

		// Give initial pass
		for (int i = 0; i < model->GetMeshCount(); ++i)
		{
			model->GetMesh(i)->PassInitWithInput(0, &pass);
		}

	} loadScreen->loadStep();*/

	// Add a rock
	{
		RrCModel* model = RrCModel::Load(rrModelLoadParams{"models/desert/rocks_0"}, NULL);
		model->transform.position = Vector3f(-2.3F, 1.6F, 0.35F);
		model->transform.scale = Vector3f(1, 1, 1) * 1.7F;
		model->transform.rotation = Rotator(Vector3f(0, 0, 45));

		// Use a default material
		RrPass pass;
		pass.utilSetupAsDefault();
		pass.m_type = kPassTypeDeferred;
		pass.m_alphaMode = renderer::kAlphaModeNone;
		pass.m_cullMode = gpu::kCullModeBack;
		pass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
		pass.setTexture( TEX_DIFFUSE, RrTexture::Load("textures/desert/rock0_ts0.png") );
		pass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
		pass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureSurfaceM0) );
		pass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );

		gpu::SamplerCreationDescription pointFilter;
		pointFilter.minFilter = core::gfx::tex::kSamplingPoint;
		pointFilter.magFilter = core::gfx::tex::kSamplingPoint;
		pass.setSampler(rrTextureSlot::TEX_DIFFUSE, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_NORMALS, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_SURFACE, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_OVERLAY, &pointFilter);

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

	// Add another rock
	{
		RrCModel* model = RrCModel::Load(rrModelLoadParams{"models/desert/rocks_0"}, NULL);
		model->transform.position = Vector3f(2.5F, 1.1F, 0.3F);
		model->transform.scale = Vector3f(1, 1, 1) * 2.6F;
		model->transform.rotation = Rotator(Vector3f(0, 0, -150));

		// Use a default material
		RrPass pass;
		pass.utilSetupAsDefault();
		pass.m_type = kPassTypeDeferred;
		pass.m_alphaMode = renderer::kAlphaModeNone;
		pass.m_cullMode = gpu::kCullModeBack;
		pass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
		pass.setTexture( TEX_DIFFUSE, RrTexture::Load("textures/desert/rock0_ts0.png") );
		pass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
		pass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureSurfaceM0) );
		pass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );

		gpu::SamplerCreationDescription pointFilter;
		pointFilter.minFilter = core::gfx::tex::kSamplingPoint;
		pointFilter.magFilter = core::gfx::tex::kSamplingPoint;
		pass.setSampler(rrTextureSlot::TEX_DIFFUSE, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_NORMALS, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_SURFACE, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_OVERLAY, &pointFilter);

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

	// Add a tree
	{
		RrCModel* model = RrCModel::Load(rrModelLoadParams{"models/foliage/tree_0"}, NULL);
		model->transform.position = Vector3f(-1.4F, 3.1F, 0.3F);

		{
			// Set up tree material
			RrPass pass;
			pass.utilSetupAsDefault();
			pass.m_type = kPassTypeDeferred;
			pass.m_alphaMode = renderer::kAlphaModeNone;
			pass.m_cullMode = gpu::kCullModeBack;
			pass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
			pass.setTexture( TEX_DIFFUSE, RrTexture::Load("textures/foliage/tree0_ts0.png") );
			pass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
			pass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureSurfaceM0) );
			pass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );

			gpu::SamplerCreationDescription pointFilter;
			pointFilter.minFilter = core::gfx::tex::kSamplingPoint;
			pointFilter.magFilter = core::gfx::tex::kSamplingPoint;
			pass.setSampler(rrTextureSlot::TEX_DIFFUSE, &pointFilter);
			pass.setSampler(rrTextureSlot::TEX_NORMALS, &pointFilter);
			pass.setSampler(rrTextureSlot::TEX_SURFACE, &pointFilter);
			pass.setSampler(rrTextureSlot::TEX_OVERLAY, &pointFilter);

			pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/deferred_env/simple_vv.spv", "shaders/deferred_env/simple_p.spv"}) );
			renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
				renderer::shader::Location::kUV0,
				renderer::shader::Location::kColor,
				renderer::shader::Location::kNormal,
				renderer::shader::Location::kTangent,
				renderer::shader::Location::kBinormal};
			pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
			pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;

			model->GetMesh(0)->PassInitWithInput(0, &pass);
		}

		{
			// Set up tree material
			RrPass pass;
			pass.utilSetupAsDefault();
			pass.m_type = kPassTypeDeferred;
			pass.m_alphaMode = renderer::kAlphaModeAlphatest;
			pass.m_cullMode = gpu::kCullModeNone;
			pass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
			pass.m_surface.shadingModel = kShadingModelThinFoliage;
			pass.m_surface.alphaCutoff = 0.5F;
			pass.m_surface.baseSmoothness = 0.1F;
			pass.setTexture( TEX_DIFFUSE, RrTexture::Load("textures/foliage/tree0_ts1.png") );
			pass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
			pass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureSurfaceM0) );
			pass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );

			gpu::SamplerCreationDescription pointFilter;
			pointFilter.minFilter = core::gfx::tex::kSamplingPoint;
			pointFilter.magFilter = core::gfx::tex::kSamplingPoint;
			pass.setSampler(rrTextureSlot::TEX_DIFFUSE, &pointFilter);
			pass.setSampler(rrTextureSlot::TEX_NORMALS, &pointFilter);
			pass.setSampler(rrTextureSlot::TEX_SURFACE, &pointFilter);
			pass.setSampler(rrTextureSlot::TEX_OVERLAY, &pointFilter);

			pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/deferred_env/simple_vv.spv", "shaders/deferred_env/simple_p.spv"}) );
			renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
				renderer::shader::Location::kUV0,
				renderer::shader::Location::kColor,
				renderer::shader::Location::kNormal,
				renderer::shader::Location::kTangent,
				renderer::shader::Location::kBinormal};
			pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
			pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;

			model->GetMesh(1)->PassInitWithInput(0, &pass);
		}
	} loadScreen->loadStep();

	// Add go
	{
		RrCModel* model = RrCModel::Load(rrModelLoadParams{"models/chara/go_posed"}, NULL);
		model->transform.position = Vector3f(-1.1F, 3.3F, 0.43F);
		model->transform.scale = Vector3f(1, 1, 1);
		model->transform.rotation = Rotator(Vector3f(0, 0, -15));

		// Use a default material
		RrPass pass;
		pass.utilSetupAsDefault();
		pass.m_type = kPassTypeDeferred;
		pass.m_alphaMode = renderer::kAlphaModeNone;
		pass.m_cullMode = gpu::kCullModeNone;
		pass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
		//pass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
		pass.setTexture( TEX_DIFFUSE, RrTexture::Load("textures/chara/go_diffuse.png") );
		pass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
		pass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureSurfaceM0) );
		pass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );

		gpu::SamplerCreationDescription pointFilter;
		pointFilter.minFilter = core::gfx::tex::kSamplingPoint;
		pointFilter.magFilter = core::gfx::tex::kSamplingPoint;
		pass.setSampler(rrTextureSlot::TEX_DIFFUSE, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_NORMALS, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_SURFACE, &pointFilter);
		pass.setSampler(rrTextureSlot::TEX_OVERLAY, &pointFilter);

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
		light->color = Color(1.1, 1.0, 0.9) * 0.6F;

		light->shadows.enabled = true;
	}

	loadScreen->RemoveReference();
	CGameBehavior::DeleteObject(loadScreen);

	  // Print a prompt
	debug::Console->PrintMessage( "You are running a debug build of M04.\n" );
	debug::Console->PrintMessage( "This scene displays a simple 3D scene with paletted visuals.\n" );
}