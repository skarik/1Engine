#include "RrRenderer.h"
#include "Settings.h"

#include "core/settings/CGameSettings.h"
#include "core/system/Screen.h"
#include "core-ext/threads/Jobs.h"
#include "core/debug/console.h"

#include "renderer/object/RrRenderObject.h"
#include "renderer/logic/RrLogicObject.h"

#include "renderer/texture/RrTexture.h"
#include "renderer/texture/RrRenderTexture.h"

#include "renderer/debug/RrDebugDrawer.h"
#include "renderer/debug/RrDebugRTInspector.h"

#include "core-ext/resources/ResourceManager.h"
#include "renderer/texture/RrTextureMasterSubsystem.h"
#include "renderer/material/RrShaderMasterSubsystem.h"
#include "renderer/logic/model/RrModelMasterSubsystem.h"

#include "gpuw/Texture.h"
#include "gpuw/RenderTarget.h"
#include "gpuw/Public/Error.h"

#include "renderer/windowing/RrWindow.h"

#include "renderer/material/RrPass.h"
#include "renderer/material/RrPass.Presets.h"
#include "renderer/material/RrShaderProgram.h"

//===Class static member data===

RrRenderer* RrRenderer::Active	= NULL;
RrRenderer* SceneRenderer		= NULL;

//===Class functions===

// Class constructor
//  saves current instance into pActive
//  initializes list of renderers
RrRenderer::RrRenderer ( void )
{
	ARCORE_ASSERT(Active == nullptr); // Only allow a single renderer.

	Active = this;
	SceneRenderer = this;

	// Create the device
#ifdef _ENGINE_DEBUG
	uint32_t layerCount = 1;
	gpu::DeviceLayer layers [] = {gpu::kDeviceLayerDebug};
#else
	uint32_t layerCount = 0;
	gpu::DeviceLayer* layers = NULL;
#endif

	gpu_device = new gpu::Device();
	int gpuErrorCode = gpu_device->create(nullptr, 0, layers, layerCount);
	if (gpuErrorCode != gpu::kError_SUCCESS)
	{
		if (gpuErrorCode == gpu::kErrorCreationFailed)
		{
			ARCORE_ERROR("Couldn't start to create device.");
		}
		else if (gpuErrorCode == gpu::kErrorInvalidDevice)
		{
			ARCORE_ERROR("Couldn't create device.");
		}
		else
		{
			ARCORE_ERROR("Couldn't create device for some unknown reason.");
		}
	}

	InitializeResourcesWithDevice(gpu_device);
}

void RrRenderer::InitializeResourcesWithDevice ( gpu::Device* device )
{
	// We need a new context specifically created for the resource manager
	gpu::GraphicsContext* gfx = new gpu::GraphicsContext(device, true); // We need the resource command context to work immediately.
	// TODO: Fix up the texture streaming so we can use deferred contexts one day.

	// Set up resource manangers
	auto resourceManager = core::ArResourceManager::Active();
	ARCORE_ASSERT(resourceManager->GetSubsystem(core::kResourceTypeRrTexture) == nullptr);
	ARCORE_ASSERT(resourceManager->GetSubsystem(core::kResourceTypeRrShader) == nullptr);
	ARCORE_ASSERT(resourceManager->GetSubsystem(core::kResourceTypeRrMeshGroup) == nullptr);

	RrTextureMasterSubsystem* resource_subsystem_texture = new RrTextureMasterSubsystem(gfx);
	ARCORE_ASSERT(resource_subsystem_texture->GetGraphicsContext() == gfx);

	resourceManager->SetSubsystem(core::kResourceTypeRrTexture, resource_subsystem_texture);
	resourceManager->SetSubsystem(core::kResourceTypeRrShader, new RrShaderMasterSubsystem());
	resourceManager->SetSubsystem(core::kResourceTypeRrMeshGroup, new RrModelMasterSubsystem());

	ARCORE_ASSERT(gfx->validate() == 0);

	// Create default textures
	{
		RrTexture* white_texture = RrTexture::CreateUnitialized(renderer::kTextureWhite); //new RrTexture("");
		{
			core::gfx::arPixel white (255, 255, 255, 255);
			white_texture->Upload( false, &white, 1,1, core::gfx::tex::kColorFormatRGBA8,
				core::gfx::tex::kWrappingRepeat, core::gfx::tex::kWrappingRepeat,
				core::gfx::tex::kMipmapGenerationNone, core::gfx::tex::kSamplingPoint );
		}
		ARCORE_ASSERT(gfx->validate() == 0);

		RrTexture* black_texture = RrTexture::CreateUnitialized(renderer::kTextureBlack);
		{
			core::gfx::arPixel black (0, 0, 0, 255);
			black_texture->Upload( false, &black, 1,1, core::gfx::tex::kColorFormatRGBA8,
				core::gfx::tex::kWrappingRepeat, core::gfx::tex::kWrappingRepeat,
				core::gfx::tex::kMipmapGenerationNone, core::gfx::tex::kSamplingPoint );
		}
		ARCORE_ASSERT(gfx->validate() == 0);

		RrTexture* gray0_texture = RrTexture::CreateUnitialized(renderer::kTextureGrayA0);
		{
			core::gfx::arPixel gray0 (127, 127, 127, 0);
			gray0_texture->Upload( false, &gray0, 1,1, core::gfx::tex::kColorFormatRGBA8,
				core::gfx::tex::kWrappingRepeat, core::gfx::tex::kWrappingRepeat,
				core::gfx::tex::kMipmapGenerationNone, core::gfx::tex::kSamplingPoint );
		}
		ARCORE_ASSERT(gfx->validate() == 0);

		RrTexture* normal0_texture = RrTexture::CreateUnitialized(renderer::kTextureNormalN0);
		{
			core::gfx::arPixel normal0 (127, 127, 255, 0);
			normal0_texture->Upload( false, &normal0, 1,1, core::gfx::tex::kColorFormatRGBA8,
				core::gfx::tex::kWrappingRepeat, core::gfx::tex::kWrappingRepeat,
				core::gfx::tex::kMipmapGenerationNone, core::gfx::tex::kSamplingPoint );
		}
		ARCORE_ASSERT(gfx->validate() == 0);
	}
	ARCORE_ASSERT(gfx->validate() == 0);

	// Create the default material
	//if ( renderer::pass::Default == NULL )
	//{
	//	// The default material must be a single pass in both modes.
	//	// This to provide compatibility with the default system implementation. (A lot of early engine code is implemented lazily)
	//	renderer::pass::Default = new RrPass;
	//	renderer::pass::Default->setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
	//	renderer::pass::Default->setTexture( TEX_NORMALS, RrTexture::Load( "textures/default_normals.jpg" ) );
	//	renderer::pass::Default->setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureBlack) );
	//	renderer::pass::Default->setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );
	//	renderer::pass::Default->m_type = kPassTypeForward;
	//	renderer::pass::Default->m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/d/diffuse_vv.spv", "shaders/d/diffuse_p.spv"});
	//	// Setup deferred pass
	//	// todo?
	//}
	if ( renderer::pass::Copy == NULL )
	{
		renderer::pass::Copy = new RrPass;
		renderer::pass::Copy->m_type = kPassTypeForward;
		renderer::pass::Copy->m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/copy_buffer_simple_vv.spv", "shaders/sys/copy_buffer_simple_p.spv"});
		renderer::pass::Copy->m_overrideDepth = true;
		renderer::pass::Copy->m_overrideDepthTest = gpu::kCompareOpAlways;
		renderer::pass::Copy->m_cullMode = gpu::kCullModeNone;
	}
	// Create the fallback shader
	if ( renderer::pass::Fullbright == NULL )
	{
		renderer::pass::Fullbright = new RrPass;
		renderer::pass::Fullbright->m_type = kPassTypeForward;
		renderer::pass::Fullbright->m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/fullbright_vv.spv", "shaders/sys/fullbright_p.spv"});
	}
	
	ARCORE_ASSERT(gfx->validate() == 0);

	// Create the pipeline utils
	InitializeCommonPipelineResources(device);

	// Create the debug tools
	new debug::RrDebugDrawer;
	new debug::RrDebugRTInspector;

	ARCORE_ASSERT(gfx->validate() == 0);

	// Submit and release the gfx
	gfx->submit();
}

void RrOutputState::Update ( RrOutputInfo* output_info, rrRenderFrameState* frame_state )
{
	this->output_info = output_info; // This can change from frame-to-frame, so we force update.
	this->frame_state = frame_state;

	// Create the graphics context:
	if (graphics_context == nullptr)
	{
		graphics_context = new gpu::GraphicsContext(SceneRenderer->GetGpuDevice(), false);
	}

	// Initialize the buffer chain:
	Vector2i l_requested_size = output_info->GetOutputSize();
	if (output_size != l_requested_size)
	{
		output_size = l_requested_size;

		// Mark this is the first frame after creation.
		first_frame_after_creation = true;
	}
}

void RrOutputState::FreeContexts ( void )
{
	delete graphics_context;
	graphics_context = nullptr;
}

// Class destructor
//  sets pActive pointer to null
//  frees list of renderers, but not the renderers
RrRenderer::~RrRenderer ( void )
{
	// Delete the debug tools
	delete debug::Drawer;
	delete debug::RTInspector;

	// Free the worlds
	for (RrWorld* world : worlds)
	{
		delete world;
	}
	// Free the output states
	for (auto& output : render_outputs)
	{
		delete output.state;
	}

	// Free state info
	Active			= NULL;
	SceneRenderer	= NULL;

	// Free system default material
	//delete_safe(renderer::pass::Default);
	delete_safe(renderer::pass::Copy);
	delete_safe(renderer::pass::Fullbright);

	// Free the other materials
	FreeCommonPipelineResources(gpu_device);

	// Clear out the context for the resource manager
	auto resourceManager = core::ArResourceManager::Active();

	// Free graphics context for textures
	gpu::GraphicsContext* texture_context = static_cast<RrTextureMasterSubsystem*>(resourceManager->GetSubsystem(core::kResourceTypeRrTexture))->GetGraphicsContext();
	delete texture_context;

	// Free the resource subsystems
	delete resourceManager->GetSubsystem(core::kResourceTypeRrTexture);
	delete resourceManager->GetSubsystem(core::kResourceTypeRrShader);
	delete resourceManager->GetSubsystem(core::kResourceTypeRrMeshGroup);
	
	resourceManager->SetSubsystem(core::kResourceTypeRrTexture, nullptr);
	resourceManager->SetSubsystem(core::kResourceTypeRrShader, nullptr);
	resourceManager->SetSubsystem(core::kResourceTypeRrMeshGroup, nullptr);

	// TODO

	// Free GPU device
	gpu_device->free();
	delete_safe(gpu_device);
}

//-Adding and Removing Renderable/Logic Objects-

rrId RrWorld::AddObject ( RrRenderObject* object )
{
	objects.push_back(object);

	rrId new_id = rrId();
	new_id.world_index = world_index;
	new_id.object_index = (uint16)(objects.size() - 1);
	ARCORE_ASSERT(new_id.object_index < objects.size());
	return new_id;
}

bool RrWorld::RemoveObject ( RrRenderObject* object )
{
	return RemoveObject(object->GetId());
}

bool RrWorld::RemoveObject ( const rrId& object_id )
{
	ARCORE_ASSERT(object_id.world_index == this->world_index);

	// Grab the object & then null it from the list.
	ARCORE_ASSERT(object_id.object_index < objects.size());
	RrRenderObject*& object = objects[object_id.object_index];
	ARCORE_ASSERT(object->GetId() == object_id);

	object = nullptr;

	return true;
}


rrId RrWorld::AddLogic ( RrLogicObject* logic )
{
	logics.push_back(logic);
	
	rrId new_id = rrId();
	new_id.world_index = world_index;
	new_id.object_index = (uint16)(logics.size() - 1);
	ARCORE_ASSERT(new_id.object_index < logics.size());
	return new_id;
}

bool RrWorld::RemoveLogic ( RrLogicObject* logic )
{
	return RemoveLogic(logic->GetId());
}

bool RrWorld::RemoveLogic ( const rrId& logic_id )
{
	core::jobs::System::Current::WaitForJobs( core::jobs::kJobTypeRenderStep ); // TODO: This better lmao

	ARCORE_ASSERT(logic_id.world_index == this->world_index);

	// Grab the object & then null it from the list.
	ARCORE_ASSERT(logic_id.object_index < logics.size());
	RrLogicObject*& logic = logics[logic_id.object_index];
	ARCORE_ASSERT(logic->GetId() == logic_id);

	logic = nullptr;

	return true;
}


void RrRenderer::AddQueuedToWorld ( void )
{
	ARCORE_ASSERT(!worlds.empty());
	RrWorld* target_world = worlds[0];

	for (RrRenderObject* object : objects_to_add)
	{
		if (object != nullptr)
		{
			object->Access_id_From_RrRenderer().Set(target_world->AddObject(object));
		}
	}

	for (RrLogicObject* logic : logics_to_add)
	{
		if (logic != nullptr)
		{
			logic->Access_id_From_RrRenderer().Set(target_world->AddLogic(logic));
		}
	}

	objects_to_add.clear();
	logics_to_add.clear();
}

// ================================
// Output Management
// ================================

uint RrRenderer::AddOutput ( const RrOutputInfo& info )
{
	ARCORE_ASSERT(info.type != RrOutputInfo::Type::kUinitialized);
	ARCORE_ASSERT(info.camera != nullptr);

	render_outputs.push_back(rrOutputPair{info});

	return (uint16)(render_outputs.size() - 1);
}

uint RrRenderer::FindOutputWithTarget ( RrWindow* window )
{
	for (uint i = 0; i < (uint)render_outputs.size(); ++i)
	{
		if (render_outputs[i].info.type == RrOutputInfo::Type::kWindow
			&& render_outputs[i].info.output_window == window)
		{
			return i;
		}
	}
	return UINT16_MAX;
}

uint RrRenderer::FindOutputWithTarget ( RrRenderTexture* target )
{
	for (uint i = 0; i < (uint)render_outputs.size(); ++i)
	{
		if (render_outputs[i].info.type == RrOutputInfo::Type::kRenderTarget
			&& render_outputs[i].info.output_target == target)
		{
			return i;
		}
	}
	return UINT16_MAX;
}

void RrRenderer::RemoveOutput ( const uint Index )
{
	render_outputs.erase(render_outputs.begin() + Index);
}

// ================================
// World Management
// ================================

uint RrRenderer::AddWorld ( RrWorld* world )
{
	worlds.push_back(world);
	world->world_index = (uint16)(worlds.size() - 1);

	return world->world_index;
}

uint RrRenderer::AddWorldDefault ( void )
{
	RrWorld* world = new RrWorld();
	return AddWorld(world);
}

// Rendering configuration
// ================================

Vector2i RrOutputInfo::GetOutputSize ( void ) const
{
	if (type == Type::kWindow)
	{
		ARCORE_ASSERT(output_window != nullptr && output_window->GpuSurface() != nullptr);
		return Vector2i(output_window->GpuSurface()->getWidth(), output_window->GpuSurface()->getHeight());
	}
	else if (type == Type::kRenderTarget)
	{
		ARCORE_ASSERT(output_target != nullptr);
		return Vector2i(output_target->GetWidth(), output_target->GetHeight());
	}
	else
	{
		ARCORE_ERROR("Uninitialized output being used.");
		return viewport.size;
	}
}

gpu::RenderTarget* RrOutputInfo::GetRenderTarget ( void ) const
{
	if (type == Type::kWindow)
	{
		ARCORE_ASSERT(output_window != nullptr && output_window->GpuSurface() != nullptr);
		return output_window->GpuSurface()->getRenderTarget();
	}
	else if (type == Type::kRenderTarget)
	{
		ARCORE_ASSERT(output_target != nullptr);
		return output_target->GetRenderTarget();
	}
	else
	{
		ARCORE_ERROR("Uninitialized output being used.");
		return nullptr;
	}
}