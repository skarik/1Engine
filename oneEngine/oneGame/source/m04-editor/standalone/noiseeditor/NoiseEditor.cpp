#include "NoiseEditor.h"

#include "core/debug/Console.h"

#include "core-ext/system/io/Resources.h"
#include "core-ext/system/shell/Inputs.h"
#include "core-ext/threads/ParallelFor.h"
#include "core-ext/containers/arStringEnum.h"
#include "core-ext/settings/PersistentSettings.h"

#include "renderer/object/RrRenderObject.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/windowing/RrWindow.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/texture/RrTexture.h"
#include "renderer/texture/RrTexture3D.h"

#include "m04/eventide/UserInterface.h"

#include "./AxesGizmo.h"
#include "./EditPanel.h"

m04::editor::NoiseEditor::NoiseEditor ( void )
	: CGameBehavior()
{
	// Set up window and output for this
	window = new RrWindow(RrRenderer::Active, NIL);
	window->Resize(800, 800);

	editor_world = new RrWorld();
	RrRenderer::Active->AddWorld(editor_world);

	RrOutputInfo output (editor_world, window);
	output.name = "NoiseEditor";
	{
		editor_camera = new RrCamera(false);
		// Override certain camera aspects to get the right projection
		editor_camera->transform.rotation = Rotator( 0.0, -90, -90 );
		editor_camera->transform.position.z = 800;
		editor_camera->zNear = 1;
		editor_camera->zFar = 4000;
		//editor_camera->fieldOfView = 40;
		//editor_camera->orthographic = true;
	}
	output.camera = editor_camera;

	// Add the output to the renderer now that it's somewhat ready.
	RrRenderer::Active->AddOutput(output);

	// Create editor now that we have something
	dusk_interface = new dusk::UserInterface(window, editor_world);
	user_interface = new ui::eventide::UserInterface(window, dusk_interface, NULL, editor_world, editor_camera);

	axes_gizmo = new m04::editor::noise::AxesGizmo(user_interface);
	edit_panel = new m04::editor::noise::EditPanel(dusk_interface, this);
	
	// Update the noise now
	UpdateNoise();

	// Update windowing options
	window->SetWantsClipCursor(false);
	window->SetWantsHideCursor(false);
	window->SetWantsSystemCursor(true);
	window->SetZeroInputOnLoseFocus(true);

	// Everything is set up, show the window now.
	window->Show();
	window->SetTitle("Noise Editor");
}

m04::editor::NoiseEditor::~NoiseEditor ( void )
{
	delete edit_panel;

	if (noise_texture)
	{
		noise_texture->RemoveReference();
	}
	delete preview_primitive_2d;
	delete preview_primitive_3d;

	DeleteObject(dusk_interface);
	DeleteObject(user_interface);

	RrRenderer::Active->RemoveOutput(RrRenderer::Active->FindOutputWithTarget(window));
	RrRenderer::Active->RemoveWorld(editor_world);
	window->Close();
	delete window;
}

void m04::editor::NoiseEditor::Update ( void )
{
	// Update panel logic
	edit_panel->Update();

	UpdateViewDrag();
	
	// Check if we want closing
	if (window->IsDone() || window->WantsClose())
	{
		DeleteObject(this);
	}
}

void m04::editor::NoiseEditor::UpdateViewDrag ( void )
{
	// We cannot do camera control if the mouse is locked
	if (user_interface->IsMouseLocked())
	{
		// Need to notify mouse gizmo to change to normal pointer state.
	}
	else
	{
		// Do classic 3D controls with mouse
		if (is_3d_mode)
		{
			if (moving_view)
			{
				if (core::Input::Mouse(core::kMBLeft) && core::Input::Mouse(core::kMBRight))
				{
					// Zoom
					camera_zoom += core::Input::DeltaMouseY();
				}
				else if (core::Input::Mouse(core::kMBLeft))
				{
					// Rotate
					camera_rotation.z += core::Input::DeltaMouseX();
					camera_rotation.y -= core::Input::DeltaMouseY();
					camera_rotation.y = math::clamp(camera_rotation.y, -90.0F, 90.0F);
				}
				else if (core::Input::Mouse(core::kMBRight))
				{
					// Panning
					camera_panning += editor_camera->transform.rotation * Vector3f(0, 1, 0) * core::Input::DeltaMouseX() * camera_zoom / 512.0F;
					camera_panning += editor_camera->transform.rotation * Vector3f(0, 0, 1) * core::Input::DeltaMouseY() * camera_zoom / 512.0F;
				}
				else
				{
					moving_view = false;
				}
				core::shell::SetSystemMousePosition((int)mouse_position_reference.x, (int)mouse_position_reference.y);
			}
			else if (!dusk_interface->IsMouseInside())
			{
				if (core::Input::MouseDown(core::kMBLeft) || core::Input::MouseDown(core::kMBRight))
				{
					moving_view = true;
					mouse_position_reference = Vector2f((float)core::Input::SysMouseX(), (float)core::Input::SysMouseY());
				}
			}
		}
		// Do easy 2D controls with mouse
		else
		{
			if (moving_view)
			{
				if (core::Input::Mouse(core::kMBLeft) && core::Input::Mouse(core::kMBRight))
				{
					// Zoom
					camera_zoom += core::Input::DeltaMouseY();
				}
				/*else if (core::Input::Mouse(core::kMBLeft))
				{
					// Rotate
					camera_rotation.z += core::Input::DeltaMouseX();
					camera_rotation.y -= core::Input::DeltaMouseY();
					camera_rotation.y = math::clamp(camera_rotation.y, -90.0F, 90.0F);
				}*/
				else if (core::Input::Mouse(core::kMBRight))
				{
					// Panning
					camera_panning.x += core::Input::DeltaMouseX() * camera_zoom / 512.0F;
					camera_panning.y += core::Input::DeltaMouseY() * camera_zoom / 512.0F;
				}
				else
				{
					moving_view = false;
				}
				core::shell::SetSystemMousePosition((int)mouse_position_reference.x, (int)mouse_position_reference.y);
			}
			else if (!dusk_interface->IsMouseInside())
			{
				if (core::Input::MouseDown(core::kMBLeft) || core::Input::MouseDown(core::kMBRight))
				{
					moving_view = true;
					mouse_position_reference = Vector2f(core::Input::SysMouseX(), core::Input::SysMouseY());
				}
			}
		}
	}

	// Update camera
	if (is_3d_mode)
	{
		editor_camera->transform.rotation = Rotator(camera_rotation);
		editor_camera->transform.position = editor_camera->transform.rotation * Vector3f(-camera_zoom, 0, 0) + camera_panning;
	}
	else
	{
		editor_camera->transform.rotation = Rotator(0, -90, -90);
		editor_camera->transform.position = editor_camera->transform.rotation * Vector3f(-camera_zoom, camera_panning.x, camera_panning.y);
	}
}

#include "core/math/noise/PerlinNoise.h"
#include "core/math/noise/SimplexNoise.h"
#include "core-ext/math/noise/MidpointNoiseBuffer.h"
#include "core-ext/math/noise/MidpointWrappedNoise.h"
#include "core/math/noise/WorleyCellNoise.h"
#include "core/math/noise/WorleyNoise.h"

#include "renderer/object/shapes/RrShapePlane.h"
#include "renderer/object/shapes/RrShapeCube.h"
#include "renderer/material/RrShaderProgram.h"

void m04::editor::NoiseEditor::UpdateNoise ( void )
{
	if (noise_texture != nullptr)
	{
		noise_texture->RemoveReference();
	}

	BaseNoise* noise = nullptr;
	if (edit_state.type == NoiseType::kPerlin)
	{
		noise = new PerlinNoise (edit_state.octaves, edit_state.frequency, 1.0F, edit_state.seed);
	}
	else if (edit_state.type == NoiseType::kSimplex)
	{
		noise = new SimplexNoise (edit_state.octaves, edit_state.frequency, 1.0F, (float)edit_state.seed);
	}
	else if (edit_state.type == NoiseType::kMidpoint)
	{
		noise = new MidpointWrappedNoise (edit_state.octaves, edit_state.frequency, edit_state.seed);
	}
	else if (edit_state.type == NoiseType::kWorleyCell)
	{
		noise = new WorleyCellNoise (edit_state.frequency, 1.0F, edit_state.seed);
	}
	else if (edit_state.type == NoiseType::kWorley)
	{
		noise = new WorleyNoise ((int)(edit_state.frequency * edit_state.frequency * edit_state.frequency), 1.0F / edit_state.frequency, 1.0F, edit_state.seed);
	}

	// Generate noise
	if (noise != nullptr)
	{
		if (!edit_state.is3D)
		{
			Vector4f* raw_noise = new Vector4f [edit_state.size * edit_state.size];

			core::parallel_for(true,
				0, edit_state.size,
				[this, &noise, raw_noise](int pixel_x)
			{
				for (int pixel_y = 0; pixel_y < edit_state.size; ++pixel_y)
				{
					float noiseValue = noise->Get(pixel_x / (float)edit_state.size, pixel_y / (float)edit_state.size);

					noiseValue = noiseValue * edit_state.total_scale + edit_state.total_bias;
					if (edit_state.invert_output) noiseValue = 1.0F - noiseValue;
					if (edit_state.clamp_bottom) noiseValue = std::max<float>(0.0F, noiseValue);
					if (edit_state.clamp_top) noiseValue = std::min<float>(1.0F, noiseValue);

					raw_noise[pixel_y * edit_state.size + pixel_x] =
						Vector4f(Vector3f(1, 1, 1) * noiseValue, 1.0F);
				}
			});

			// Save the new texture
			noise_texture = RrTexture::CreateUnitialized("noise");
			noise_texture->Upload(false, raw_noise, edit_state.size, edit_state.size, core::gfx::tex::kColorFormatRGBA32F);
			noise_texture->AddReference();

			delete[] raw_noise;
		}
		else
		{
			Vector4f* raw_noise = new Vector4f [edit_state.size * edit_state.size * edit_state.size];

			core::parallel_for(true,
				0, edit_state.size,
				[this, &noise, raw_noise](int pixel_x)
			{
				for (int pixel_y = 0; pixel_y < edit_state.size; ++pixel_y)
				{
					for (int pixel_z = 0; pixel_z < edit_state.size; ++pixel_z)
					{
						float noiseValue = noise->Get(pixel_x / (float)edit_state.size, pixel_y / (float)edit_state.size, pixel_z / (float)edit_state.size);

						noiseValue = noiseValue * edit_state.total_scale + edit_state.total_bias;
						if (edit_state.invert_output) noiseValue = 1.0F - noiseValue;
						if (edit_state.clamp_bottom) noiseValue = std::max<float>(0.0F, noiseValue);
						if (edit_state.clamp_top) noiseValue = std::min<float>(1.0F, noiseValue);

						raw_noise[pixel_z * edit_state.size * edit_state.size + pixel_y * edit_state.size + pixel_x] =
							Vector4f(Vector3f(1, 1, 1) * noiseValue, 1.0F);
					}
				}
			});

			// Save the new texture
			noise_texture = RrTexture3D::CreateUnitialized("noise");
			((RrTexture3D*)noise_texture)->Upload(false, raw_noise, edit_state.size, edit_state.size, edit_state.size, core::gfx::tex::kColorFormatRGBA32F);
			noise_texture->AddReference();

			delete[] raw_noise;
		}
	}
	delete noise;

	// Create the previewer: either a plane with a specific material, or a 3d cube with specific material. Create both and toggle between them.
	if (!edit_state.is3D)
	{
		if (preview_primitive_2d == nullptr)
		{
			preview_primitive_2d = new RrShapePlane();
			preview_primitive_2d->AddToWorld(editor_world);

			// Set up pass
			RrPass pass;
			pass.utilSetupAsDefault();
			pass.m_type = kPassTypeForward;
			pass.m_alphaMode = renderer::kAlphaModeNone;
			pass.m_cullMode = gpu::kCullModeNone;
			pass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );

			gpu::SamplerCreationDescription pointFilter;
			pointFilter.minFilter = core::gfx::tex::kSamplingPoint;
			pointFilter.magFilter = core::gfx::tex::kSamplingPoint;
			pass.setSampler(rrTextureSlot::TEX_DIFFUSE, &pointFilter);

			pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/fullbright_vv.spv", "shaders/sys/fullbright_p.spv"}) );
			renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
													renderer::shader::Location::kUV0,
													renderer::shader::Location::kColor,
													renderer::shader::Location::kNormal,
													renderer::shader::Location::kTangent,
													renderer::shader::Location::kBinormal};
			pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
			pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
			pass.m_renderCallback = nullptr;

			preview_primitive_2d->PassInitWithInput(0, &pass);
		}

		// Update the preview prim
		preview_primitive_2d->transform.world.position = Vector3f((float)edit_state.size, (float)edit_state.size, 0) / 2;
		preview_primitive_2d->transform.world.scale = Vector3f(1.0F, 1.0F, 1.0F) * (float)edit_state.size;
		preview_primitive_2d->PassAccess(0).setTexture(rrTextureSlot::TEX_DIFFUSE, noise_texture);
		preview_primitive_2d->SetVisible(true);

		// Hide the 3D preview prim
		if (preview_primitive_3d)
		{
			preview_primitive_3d->SetVisible(false);
		}
	}
	else
	{
		if (preview_primitive_3d == nullptr)
		{
			preview_primitive_3d = new RrShapeCube();
			preview_primitive_3d->AddToWorld(editor_world);

			// Set up pass
			RrPass pass;
			pass.utilSetupAsDefault();
			pass.m_type = kPassTypeForward;
			pass.m_alphaMode = renderer::kAlphaModeNone;
			pass.m_cullMode = gpu::kCullModeNone;
			pass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );

			gpu::SamplerCreationDescription pointFilter;
			pointFilter.minFilter = core::gfx::tex::kSamplingPoint;
			pointFilter.magFilter = core::gfx::tex::kSamplingPoint;
			pointFilter.wrapmodeX = core::gfx::tex::kWrappingClamp;
			pointFilter.wrapmodeY = core::gfx::tex::kWrappingClamp;
			pointFilter.wrapmodeZ = core::gfx::tex::kWrappingClamp;
			pass.setSampler(rrTextureSlot::TEX_DIFFUSE, &pointFilter);

			pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/tool/texture3d_vv.spv", "shaders/tool/texture3d_p.spv"}) );
			renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
													renderer::shader::Location::kUV0,
													renderer::shader::Location::kColor,
													renderer::shader::Location::kNormal,
													renderer::shader::Location::kTangent,
													renderer::shader::Location::kBinormal};
			pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
			pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
			pass.m_renderCallback = nullptr;

			preview_primitive_3d->PassInitWithInput(0, &pass);
		}

		// Update the preview prim
		preview_primitive_3d->transform.world.position = Vector3f(1.0F, 1.0F, 1.0F) * (float)edit_state.size / 2;
		preview_primitive_3d->transform.world.scale = Vector3f(1.0F, 1.0F, 1.0F) * (float)edit_state.size;
		preview_primitive_3d->PassAccess(0).setTexture(rrTextureSlot::TEX_DIFFUSE, noise_texture);
		preview_primitive_3d->SetVisible(true);

		// Hide the 3D preview prim
		if (preview_primitive_2d)
		{
			preview_primitive_2d->SetVisible(false);
		}
	}

	// Set up camera mode
	is_3d_mode = edit_state.is3D;
}