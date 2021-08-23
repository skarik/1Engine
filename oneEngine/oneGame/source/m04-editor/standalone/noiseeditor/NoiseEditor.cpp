#include "NoiseEditor.h"

#include "core/debug/Console.h"

#include "core-ext/system/io/Resources.h"
#include "core-ext/system/shell/Inputs.h"
#include "core-ext/system/shell/Status.h"
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
	delete[] m_raw_noise;

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

	// Update taskbar state to show we're working
	core::shell::SetTaskbarProgressState(NIL, core::shell::kTaskbarStateIndeterminate);

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
		noise = new WorleyNoise ((int)(edit_state.frequency * edit_state.frequency * edit_state.frequency), 1.0F / edit_state.frequency, 1.0F, edit_state.seed, edit_state.wrap);
	}

	// Generate noise
	if (noise != nullptr)
	{
		if (!edit_state.is3D)
		{
			if (m_raw_noise_size != edit_state.size * edit_state.size)
			{
				m_raw_noise_size = edit_state.size * edit_state.size;
				delete[] m_raw_noise;
				m_raw_noise = new Vector4f [m_raw_noise_size];
				for (int pixel_i = 0; pixel_i < m_raw_noise_size; ++pixel_i)
				{
					m_raw_noise[pixel_i] = Vector4f(0, 0, 0, 1);
				}
			}
			Vector4f* raw_noise = m_raw_noise;

			core::parallel_for(true,
				0, edit_state.size,
				[this, &noise, raw_noise](int pixel_x)
			{
				for (int pixel_y = 0; pixel_y < edit_state.size; ++pixel_y)
				{
					float noiseValue = noise->Get(pixel_x / (float)edit_state.size, pixel_y / (float)edit_state.size);
					raw_noise[pixel_y * edit_state.size + pixel_x][edit_state.edit_channel] = noiseValue;
				}
			});

			// Now perform the wrapping
			if (edit_state.wrap && edit_state.type != NoiseType::kWorley)
			{
				const int blend_width = (int)(edit_state.size * 0.20F);
				const int dim_size = edit_state.size;
				const int edit_channel = edit_state.edit_channel;

				// Declare the blender
				auto blendSample = [noise, raw_noise, dim_size, edit_channel](const float offset_x, const float offset_y, int pixel_x, int pixel_y, float blend_level)
				{
					float noiseValue = noise->Get(offset_x + pixel_x / (float)dim_size, offset_y + pixel_y / (float)dim_size);
					float sourceValue = raw_noise[pixel_y * dim_size + pixel_x][edit_channel];

					// Simple blend
					float blendValue = math::lerp(1.0F - math::cube(1.0F - blend_level), noiseValue, sourceValue);

					// Blend based on high-points
					/*float blendValue = std::max(
										math::lerp(math::cube(math::cube(blend_level)), noiseValue, -1.0F),
										sourceValue);

					// Blend to the simple blend to smooth hard edges.
					blendValue = math::lerp(
						blend_level,
						blendValue,
						math::lerp(1.0F - math::cube(1.0F - blend_level), noiseValue, sourceValue));*/

					raw_noise[pixel_y * dim_size + pixel_x][edit_channel] = blendValue;
				};

				// Blend on X coordinate
				for (int pixel_x = 0; pixel_x < blend_width; ++pixel_x)
				{
					const float blend_level = pixel_x / (float)blend_width;
					for (int pixel_y = 0; pixel_y < edit_state.size; ++pixel_y)
					{
						blendSample(1.0F, 0.0F, pixel_x, pixel_y, blend_level);
					}
				}

				// Blend on Y coordinate
				for (int pixel_y = 0; pixel_y < blend_width; ++pixel_y)
				{
					const float blend_level = pixel_y / (float)blend_width;
					for (int pixel_x = 0; pixel_x < edit_state.size; ++pixel_x)
					{
						blendSample(0.0F, 1.0F, pixel_x, pixel_y, blend_level);
					}
				}
			}

			for (int pixel_i = 0; pixel_i < edit_state.size * edit_state.size; ++pixel_i)
			{
				float noiseValue = raw_noise[pixel_i][edit_state.edit_channel];

				noiseValue = noiseValue * edit_state.total_scale + edit_state.total_bias;
				if (edit_state.invert_output) noiseValue = 1.0F - noiseValue;
				if (edit_state.clamp_bottom) noiseValue = std::max<float>(0.0F, noiseValue);
				if (edit_state.clamp_top) noiseValue = std::min<float>(1.0F, noiseValue);

				if (edit_state.edit_split)
					raw_noise[pixel_i][edit_state.edit_channel] = noiseValue;
				else
					raw_noise[pixel_i] = Vector4f(Vector3f(1, 1, 1) * noiseValue, 1.0F);
			}

			// Save the new texture
			noise_texture = RrTexture::CreateUnitialized("noise");
			noise_texture->Upload(false, raw_noise, edit_state.size, edit_state.size, core::gfx::tex::kColorFormatRGBA32F);
			noise_texture->AddReference();

			//delete[] raw_noise;
		}
		else
		{
			if (m_raw_noise_size != edit_state.size * edit_state.size * edit_state.size)
			{
				m_raw_noise_size = edit_state.size * edit_state.size * edit_state.size;
				delete[] m_raw_noise;
				m_raw_noise = new Vector4f [m_raw_noise_size];
				for (int pixel_i = 0; pixel_i < m_raw_noise_size; ++pixel_i)
				{
					m_raw_noise[pixel_i] = Vector4f(0, 0, 0, 1);
				}
			}
			Vector4f* raw_noise = m_raw_noise;

			core::parallel_for(true,
				0, edit_state.size,
				[this, &noise, raw_noise](int pixel_x)
			{
				for (int pixel_y = 0; pixel_y < edit_state.size; ++pixel_y)
				{
					for (int pixel_z = 0; pixel_z < edit_state.size; ++pixel_z)
					{
						float noiseValue = noise->Get(pixel_x / (float)edit_state.size, pixel_y / (float)edit_state.size, pixel_z / (float)edit_state.size);
						raw_noise[pixel_z * edit_state.size * edit_state.size + pixel_y * edit_state.size + pixel_x][edit_state.edit_channel] = noiseValue;
					}
				}
			});

			// Now perform the wrapping
			if (edit_state.wrap && edit_state.type != NoiseType::kWorley)
			{
				const int blend_width = (int)(edit_state.size * 0.20F);
				const int dim_size = edit_state.size;
				const int edit_channel = edit_state.edit_channel;

				// Declare the blender
				auto blendSample = [noise, raw_noise, dim_size, edit_channel](const float offset_x, const float offset_y, const float offset_z, int pixel_x, int pixel_y, int pixel_z, float blend_level)
				{
					float noiseValue = noise->Get(offset_x + pixel_x / (float)dim_size, offset_y + pixel_y / (float)dim_size, offset_z + pixel_z / (float)dim_size);
					float sourceValue = raw_noise[pixel_z * dim_size * dim_size + pixel_y * dim_size + pixel_x][edit_channel];

					// Simple blend
					float blendValue = math::lerp(1.0F - math::cube(1.0F - blend_level), noiseValue, sourceValue);

					// Blend based on high-points
					/*float blendValue = std::max(
										math::lerp(math::cube(math::cube(blend_level)), noiseValue, -1.0F),
										sourceValue);

					// Blend to the simple blend to smooth hard edges.
					blendValue = math::lerp(
						blend_level,
						blendValue,
						math::lerp(math::square(blend_level), noiseValue, sourceValue));*/
						
					raw_noise[pixel_z * dim_size * dim_size + pixel_y * dim_size + pixel_x][edit_channel] = blendValue;
				};

				// Blend on X coordinate
				for (int pixel_x = 0; pixel_x < blend_width; ++pixel_x)
				{
					const float blend_level = pixel_x / (float)blend_width;
					for (int pixel_y = 0; pixel_y < edit_state.size; ++pixel_y)
					{
						for (int pixel_z = 0; pixel_z < edit_state.size; ++pixel_z)
						{
							blendSample(1.0F, 0.0F, 0.0F, pixel_x, pixel_y, pixel_z, blend_level);
						}
					}
				}

				// Blend on Y coordinate
				for (int pixel_y = 0; pixel_y < blend_width; ++pixel_y)
				{
					const float blend_level = pixel_y / (float)blend_width;
					for (int pixel_x = 0; pixel_x < edit_state.size; ++pixel_x)
					{
						for (int pixel_z = 0; pixel_z < edit_state.size; ++pixel_z)
						{
							blendSample(0.0F, 1.0F, 0.0F, pixel_x, pixel_y, pixel_z, blend_level);
						}
					}
				}

				// Blend on Z coordinate
				for (int pixel_z = 0; pixel_z < blend_width; ++pixel_z)
				{
					const float blend_level = pixel_z / (float)blend_width;
					for (int pixel_x = 0; pixel_x < edit_state.size; ++pixel_x)
					{
						for (int pixel_y = 0; pixel_y < edit_state.size; ++pixel_y)
						{
							blendSample(0.0F, 0.0F, 1.0F, pixel_x, pixel_y, pixel_z, blend_level);
						}
					}
				}
			}

			for (int pixel_i = 0; pixel_i < edit_state.size * edit_state.size * edit_state.size; ++pixel_i)
			{
				float noiseValue = raw_noise[pixel_i][edit_state.edit_channel];

				noiseValue = noiseValue * edit_state.total_scale + edit_state.total_bias;
				if (edit_state.invert_output) noiseValue = 1.0F - noiseValue;
				if (edit_state.clamp_bottom) noiseValue = std::max<float>(0.0F, noiseValue);
				if (edit_state.clamp_top) noiseValue = std::min<float>(1.0F, noiseValue);

				if (edit_state.edit_split)
					raw_noise[pixel_i][edit_state.edit_channel] = noiseValue;
				else
					raw_noise[pixel_i] = Vector4f(Vector3f(1, 1, 1) * noiseValue, 1.0F);
			}

			// Save the new texture
			noise_texture = RrTexture3D::CreateUnitialized("noise");
			((RrTexture3D*)noise_texture)->Upload(false, raw_noise, edit_state.size, edit_state.size, edit_state.size, core::gfx::tex::kColorFormatRGBA32F);
			noise_texture->AddReference();

			//delete[] raw_noise;
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

	// Finish the progress on taskbar
	core::shell::SetTaskbarProgressValue(NIL, 100, 100);
}

#include "core-ext/system/io/assets/TextureIO.h"
#include "gpuw/Buffers.h"
#include "gpuw/Texture.h"

void m04::editor::NoiseEditor::SaveNoise ( const char* filename )
{
	// TODO: Add a renderer job that we can wait for

	gpu::Texture source_texture = noise_texture->GetTexture();

	gpu::Buffer readback_buffer;
	readback_buffer.initAsTextureReadbackBuffer(NULL,
		edit_state.is3D ? core::gfx::tex::kTextureType3D : core::gfx::tex::kTextureType2D,
		core::gfx::tex::kColorFormatRGBA32F,
		edit_state.size,
		edit_state.size,
		edit_state.is3D ? edit_state.size : 1);

	source_texture.copy(NULL, readback_buffer, 0, 0);

	// TODO: Wait for context to finish

	Vector4f* raw_image = (Vector4f*)readback_buffer.map(NULL, gpu::kTransferStatic);
	if (raw_image)
	{
		if (!edit_state.is3D)
		{
			// Convert image to RGBA8
			struct grPixel
			{
				uint8 r;
				uint8 g;
				uint8 b;
				uint8 a;
			};
			grPixel* raw_rgba8 = new grPixel [edit_state.size * edit_state.size];
			for (int i = 0; i < edit_state.size * edit_state.size; ++i)
			{
				raw_rgba8[i].r = (uint8)math::clamp(raw_image[i].x * 256.0F, 0.0F, 255.0F);
				raw_rgba8[i].g = (uint8)math::clamp(raw_image[i].y * 256.0F, 0.0F, 255.0F);
				raw_rgba8[i].b = (uint8)math::clamp(raw_image[i].z * 256.0F, 0.0F, 255.0F);
				raw_rgba8[i].a = (uint8)math::clamp(raw_image[i].w * 256.0F, 0.0F, 255.0F);
			}

			// Write out a 2D RAW
			FILE* output_file = fopen(filename, "wb");
			ARCORE_ASSERT(output_file != NULL);
			auto write_count = fwrite(raw_rgba8, sizeof(grPixel) * edit_state.size * edit_state.size, 1, output_file);
			ARCORE_ASSERT(write_count > 0);
			fclose(output_file);

			delete[] raw_rgba8;
		}
		else
		{
			// Convert image to RGBA8
			struct grPixel
			{
				uint8 r;
				uint8 g;
				uint8 b;
				uint8 a;
			};
			grPixel* raw_rgba8 = new grPixel [edit_state.size * edit_state.size * edit_state.size];
			for (int i = 0; i < edit_state.size * edit_state.size * edit_state.size; ++i)
			{
				raw_rgba8[i].r = (uint8)math::clamp(raw_image[i].x * 256.0F, 0.0F, 255.0F);
				raw_rgba8[i].g = (uint8)math::clamp(raw_image[i].y * 256.0F, 0.0F, 255.0F);
				raw_rgba8[i].b = (uint8)math::clamp(raw_image[i].z * 256.0F, 0.0F, 255.0F);
				raw_rgba8[i].a = (uint8)math::clamp(raw_image[i].w * 256.0F, 0.0F, 255.0F);
			}

			// Write out a 3D BPD
			core::gfx::tex::arImageInfo imageInfo = {};
			imageInfo.width = edit_state.size;
			imageInfo.height = edit_state.size;
			imageInfo.depth = edit_state.size;
			imageInfo.levels = 1;
			imageInfo.type = core::gfx::tex::kTextureType3D;

			core::BpdWriter writer;
			writer.m_generateMipmaps = false;
			writer.rawImage = raw_rgba8;
			writer.mipmaps[0] = raw_rgba8;
			writer.info = imageInfo;
			writer.datetime = (uint64_t)std::chrono::system_clock::now().time_since_epoch().count();

			auto writeResult = writer.WriteBpd(filename);
			ARCORE_ASSERT(writeResult);

			delete[] raw_rgba8;
		}
		readback_buffer.unmap(NULL);
	}
}