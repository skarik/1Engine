#include "EditPanel.h"

#include "engine-common/dusk/controls/Label.h"
#include "engine-common/dusk/controls/Panel.h"
#include "engine-common/dusk/controls/DropdownMenu.h"
#include "engine-common/dusk/controls/TextField.h"
#include "engine-common/dusk/controls/Slider.h"
#include "engine-common/dusk/controls/Checkbox.h"
#include "engine-common/dusk/layouts/Horizontal.h"
#include "engine-common/dusk/layouts/Vertical.h"
#include "engine-common/dusk/dialogs/SaveFile.h"
#include "engine-common/dusk/dialogs/LoadFile.h"

#include "engine/state/CGameState.h"

#include "core/input/CInput.h"
#include "core-ext/system/shell/Message.h"
#include "core-ext/std/filesystem.h"

#include "./NoiseEditor.h"

m04::editor::noise::EditPanel::EditPanel (dusk::UserInterface* ui, m04::editor::NoiseEditor* editor)
	: dusk_interface(ui)
	, main_editor(editor)
{

	auto sidePanel = dusk_interface->Add<dusk::elements::Panel>(dusk::ElementCreationDescription(NULL, Rect(0, 0, 100, 250)));
	auto sideLayout = dusk_interface->Add<dusk::layouts::Vertical>(dusk::LayoutCreationDescription(sidePanel));
	sideLayout->m_padding = Vector2f(5, 10);
	sideLayout->m_margin = Vector2f(5, 10);
	sideLayout->m_localRect = Rect(0, 0, 100, 250);

	{
		auto hLayout = dusk_interface->Add<dusk::layouts::HorizontalFit>(dusk::LayoutCreationDescription(sideLayout));
		
		auto button_2d = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(hLayout, Rect(0, 0, 40, 20)));
		button_2d->m_contents = "2D";
		button_2d->SetOnActivation([this]()
		{
			auto& noise_params = main_editor->GetState();
			noise_params.is3D = false;
			main_editor->UpdateNoise();
		});

		auto button_3d = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(hLayout, Rect(0, 0, 40, 20)));
		button_3d->m_contents = "3D";
		button_3d->SetOnActivation([this]()
		{
			auto& noise_params = main_editor->GetState();
			noise_params.is3D = true;
			main_editor->UpdateNoise();
		});
	}

	{
		auto vLayout = dusk_interface->Add<dusk::layouts::Vertical>(dusk::LayoutCreationDescription(sideLayout));

		auto label = dusk_interface->Add<dusk::elements::Label>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)));
		label->m_contents = "SIZE";
		label->m_style = dusk::elements::kLabelStyle_Heading1;

		auto hLayout0 = dusk_interface->Add<dusk::layouts::HorizontalFit>(dusk::LayoutCreationDescription(vLayout));
		{
			auto button_32 = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(hLayout0, Rect(0, 0, 40, 20)));
			button_32->m_contents = "32";
			button_32->SetOnActivation([this]()
			{
				auto& noise_params = main_editor->GetState();
				noise_params.size = 32;
				main_editor->UpdateNoise();
			});

			auto button_64 = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(hLayout0, Rect(0, 0, 40, 20)));
			button_64->m_contents = "64";
			button_64->SetOnActivation([this]()
			{
				auto& noise_params = main_editor->GetState();
				noise_params.size = 64;
				main_editor->UpdateNoise();
			});
		}

		auto hLayout1 = dusk_interface->Add<dusk::layouts::HorizontalFit>(dusk::LayoutCreationDescription(vLayout));
		{
			auto button_128 = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(hLayout1, Rect(0, 0, 40, 20)));
			button_128->m_contents = "128";
			button_128->SetOnActivation([this]()
			{
				auto& noise_params = main_editor->GetState();
				noise_params.size = 128;
				main_editor->UpdateNoise();
			});

			auto button_256 = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(hLayout1, Rect(0, 0, 40, 20)));
			button_256->m_contents = "256";
			button_256->SetOnActivation([this]()
			{
				auto& noise_params = main_editor->GetState();
				noise_params.size = 256;
				main_editor->UpdateNoise();
			});
		}

		auto hLayout2 = dusk_interface->Add<dusk::layouts::HorizontalFit>(dusk::LayoutCreationDescription(vLayout));
		{
			auto button_512 = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(hLayout2, Rect(0, 0, 40, 20)));
			button_512->m_contents = "512";
			button_512->SetOnActivation([this]()
			{
				auto& noise_params = main_editor->GetState();
				noise_params.size = 512;
				main_editor->UpdateNoise();
			});

			auto button_1024 = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(hLayout2, Rect(0, 0, 40, 20)));
			button_1024->m_contents = "1024";
			button_1024->SetOnActivation([this]()
			{
				auto& noise_params = main_editor->GetState();
				noise_params.size = 1024;
				main_editor->UpdateNoise();
			});
		}
	}

	{
		auto vLayout = dusk_interface->Add<dusk::layouts::Vertical>(dusk::LayoutCreationDescription(sideLayout));

		dusk_interface->Add<dusk::elements::Label>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)))
			->m_contents = "Type";

		auto button_noise_perlin = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)));
		button_noise_perlin->m_contents = "Perlin";
		button_noise_perlin->SetOnActivation([this]()
		{
			auto& noise_params = main_editor->GetState();
			noise_params.type = m04::editor::NoiseType::kPerlin;
			main_editor->UpdateNoise();
		});

		auto button_noise_simplex = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)));
		button_noise_simplex->m_contents = "Simplex";
		button_noise_simplex->SetOnActivation([this]()
		{
			auto& noise_params = main_editor->GetState();
			noise_params.type = m04::editor::NoiseType::kSimplex;
			main_editor->UpdateNoise();
		});

		auto button_noise_midpoint = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)));
		button_noise_midpoint->m_contents = "Midpoint";
		button_noise_midpoint->SetOnActivation([this]()
		{
			auto& noise_params = main_editor->GetState();
			noise_params.type = m04::editor::NoiseType::kMidpoint;
			main_editor->UpdateNoise();
		});

		auto button_noise_worley = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)));
		button_noise_worley->m_contents = "Worley Cell";
		button_noise_worley->SetOnActivation([this]()
		{
			auto& noise_params = main_editor->GetState();
			noise_params.type = m04::editor::NoiseType::kWorleyCell;
			main_editor->UpdateNoise();
		});

		auto button_noise_worley2 = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)));
		button_noise_worley2->m_contents = "Worley";
		button_noise_worley2->SetOnActivation([this]()
		{
			auto& noise_params = main_editor->GetState();
			noise_params.type = m04::editor::NoiseType::kWorley;
			main_editor->UpdateNoise();
		});
	}


	{
		auto vLayout = dusk_interface->Add<dusk::layouts::Vertical>(dusk::LayoutCreationDescription(sideLayout));

		auto label = dusk_interface->Add<dusk::elements::Label>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)));
		label->m_contents = "COMMON";
		label->m_style = dusk::elements::kLabelStyle_Heading1;

		dusk_interface->Add<dusk::elements::Label>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)))
			->m_contents = "Octaves";
		auto slider_octaves = dusk_interface->Add<dusk::elements::Slider<int>>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 40)));
		slider_octaves->m_range_min = 1;
		slider_octaves->m_range_max = 11;
		slider_octaves->m_snap_divisor = 1;
		slider_octaves->m_snap = true;
		slider_octaves->m_value = main_editor->GetState().octaves;
		slider_octaves->SetOnValueChange([this](int value)
		{
			auto& noise_params = main_editor->GetState();
			noise_params.octaves = value;
			main_editor->UpdateNoise();
		});

		dusk_interface->Add<dusk::elements::Label>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)))
			->m_contents = "Frequency";
		auto slider_frequency = dusk_interface->Add<dusk::elements::Slider<float>>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 40)));
		slider_frequency->m_range_min = 0.25F;
		slider_frequency->m_range_max = 20.0F;
		slider_frequency->m_snap_divisor = 0.25F;
		slider_frequency->m_snap = true;
		slider_frequency->m_value = main_editor->GetState().frequency;
		slider_frequency->SetOnValueChange([this](float value)
		{
			auto& noise_params = main_editor->GetState();
			noise_params.frequency = value;
			main_editor->UpdateNoise();
		});
	}

	{
		auto vLayout = dusk_interface->Add<dusk::layouts::Vertical>(dusk::LayoutCreationDescription(sideLayout));

		auto label = dusk_interface->Add<dusk::elements::Label>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)));
		label->m_contents = "OUTPUT";
		label->m_style = dusk::elements::kLabelStyle_Heading1;

		dusk_interface->Add<dusk::elements::Label>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)))
			->m_contents = "Bias";
		auto slider_total_bias = dusk_interface->Add<dusk::elements::Slider<float>>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 40)));
		slider_total_bias->m_range_min = -1.0F;
		slider_total_bias->m_range_max = +1.0F;
		slider_total_bias->m_snap_divisor = 0.05F;
		slider_total_bias->m_snap = true;
		slider_total_bias->m_value = 0.5F;
		slider_total_bias->SetOnValueChange([this](float value)
		{
			auto& noise_params = main_editor->GetState();
			noise_params.total_bias = value;
			main_editor->UpdateNoise();
		});

		dusk_interface->Add<dusk::elements::Label>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)))
			->m_contents = "Scale";
		auto slider_total_scale = dusk_interface->Add<dusk::elements::Slider<float>>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 40)));
		slider_total_scale->m_range_min = -2.0F;
		slider_total_scale->m_range_max = +2.0F;
		slider_total_scale->m_snap_divisor = 0.05F;
		slider_total_scale->m_snap = true;
		slider_total_scale->m_value = 0.5F;
		slider_total_scale->SetOnValueChange([this](float value)
		{
			auto& noise_params = main_editor->GetState();
			noise_params.total_scale = value;
			main_editor->UpdateNoise();
		});

		auto hLayout0 = dusk_interface->Add<dusk::layouts::HorizontalFit>(dusk::LayoutCreationDescription(vLayout));
		dusk_interface->Add<dusk::elements::Label>(dusk::ElementCreationDescription(hLayout0, Rect(0, 0, 60, 20)))
			->m_contents = "Invert";
		auto checkbox_invert = dusk_interface->Add<dusk::elements::Checkbox>(dusk::ElementCreationDescription(hLayout0, Rect(0, 0, 20, 20)));
		checkbox_invert->m_value = main_editor->GetState().invert_output;
		checkbox_invert->SetOnValueChange([this](bool value)
		{
			auto& noise_params = main_editor->GetState();
			noise_params.invert_output = value;
			main_editor->UpdateNoise();
		});

		auto hLayout1 = dusk_interface->Add<dusk::layouts::HorizontalFit>(dusk::LayoutCreationDescription(vLayout));
		dusk_interface->Add<dusk::elements::Label>(dusk::ElementCreationDescription(hLayout1, Rect(0, 0, 60, 20)))
			->m_contents = "Clamp 0";
		auto checkbox_clamp_bottom = dusk_interface->Add<dusk::elements::Checkbox>(dusk::ElementCreationDescription(hLayout1, Rect(0, 0, 20, 20)));
		checkbox_clamp_bottom->m_value = main_editor->GetState().clamp_bottom;
		checkbox_clamp_bottom->SetOnValueChange([this](bool value)
		{
			auto& noise_params = main_editor->GetState();
			noise_params.clamp_bottom = value;
			main_editor->UpdateNoise();
		});

		auto hLayout2 = dusk_interface->Add<dusk::layouts::HorizontalFit>(dusk::LayoutCreationDescription(vLayout));
		dusk_interface->Add<dusk::elements::Label>(dusk::ElementCreationDescription(hLayout2, Rect(0, 0, 60, 20)))
			->m_contents = "Clamp 1";
		auto checkbox_clamp_top = dusk_interface->Add<dusk::elements::Checkbox>(dusk::ElementCreationDescription(hLayout2, Rect(0, 0, 20, 20)));
		checkbox_clamp_top->m_value = main_editor->GetState().clamp_top;
		checkbox_clamp_top->SetOnValueChange([this](bool value)
		{
			auto& noise_params = main_editor->GetState();
			noise_params.clamp_top = value;
			main_editor->UpdateNoise();
		});
	}

	{
		auto vLayout = dusk_interface->Add<dusk::layouts::Vertical>(dusk::LayoutCreationDescription(sideLayout));

		auto label = dusk_interface->Add<dusk::elements::Label>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 80, 20)));
		label->m_contents = "FILE";
		label->m_style = dusk::elements::kLabelStyle_Heading1;

		auto button_2d = dusk_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(vLayout, Rect(0, 0, 40, 20)));
		button_2d->m_contents = "Save";
		button_2d->SetOnActivation([this]()
		{
			BeginSaveAsFile();
		});
	}
}

m04::editor::noise::EditPanel::~EditPanel ()
{

}

void m04::editor::noise::EditPanel::Update ( void )
{
	
}

void m04::editor::noise::EditPanel::BeginSaveAsFile ( void )
{
	if (savefileDialog == NULL)
	{
		savefileDialog = dusk_interface->AddDialog<dusk::dialogs::SaveFile>(dusk::DialogCreationDescription());
	}

	savefileDialog->as<dusk::dialogs::SaveFile>()->m_defaultDirectory = ".";

	savefileDialog->Show();
	savefileDialog->as<dusk::dialogs::SaveFile>()->SetOnAccept([this](const std::string& filename){ SaveFile(filename); });
}

void m04::editor::noise::EditPanel::SaveFile ( const std::string& filename )
{
	/*auto board = main_editor->GetNodeBoardState();
	if (board != NULL)
	{
		m04::editor::sequence::OsfSerializer serializer (filename.c_str());
		board->Save(&serializer);
		main_editor->SetWorkspaceDirty(false); // Clear workspace dirty flag
		main_editor->SetSaveTargetFilename(filename.c_str());
	}*/
	// TODO:
}
