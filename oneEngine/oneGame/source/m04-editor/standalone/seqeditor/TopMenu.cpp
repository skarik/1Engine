#include "TopMenu.h"

#include "engine-common/dusk/controls/Panel.h"
#include "engine-common/dusk/controls/DropdownMenu.h"
#include "engine-common/dusk/layouts/Horizontal.h"
#include "engine-common/dusk/dialogs/SaveFile.h"
#include "engine-common/dusk/dialogs/LoadFile.h"

#include "engine/state/CGameState.h"

#include "core/input/CInput.h"
#include "core-ext/system/shell/Message.h"
#include "core-ext/std/filesystem.h"

#include "./SequenceEditor.h"
#include "./NodeBoardState.h"
#include "./SequenceSerialization.h"

m04::editor::sequence::TopMenu::TopMenu (dusk::UserInterface* ui, m04::editor::SequenceEditor* editor)
	: dusk_interface(ui)
	, main_editor(editor)
{
	auto topPanel = dusk_interface->Add<dusk::elements::Panel>(dusk::ElementCreationDescription(NULL, Rect()));
	auto topLayout = dusk_interface->Add<dusk::layouts::Horizontal>(dusk::LayoutCreationDescription(topPanel));

	auto fileMenu = dusk_interface->Add<dusk::elements::DropdownMenu>(dusk::ElementCreationDescription(topLayout, Rect()));
	fileMenu->m_contents = "File";
	fileMenu->Add("New", 'N', true, [this](){ BeginNewFile(); });
	//fileMenu->Add("Save (Test)", 0, true, [this](){ SaveTest(); });
	fileMenu->Add("Save", 'S', true, [this](){ BeginSaveFile(); });
	fileMenu->Add("Save As", 0, true, [this](){ BeginSaveAsFile(); });
	//fileMenu->Add("Open (Test)", 0, true, [this](){ LoadTest(); });
	fileMenu->Add("Open", 'O', true, [this](){ BeginLoadFile(); });
	fileMenu->Add("Quit", 'Q', true, [](){ CGameState::Active()->EndGame(); }); // TODO: check if things are unsaved first.

	auto EditMenu = dusk_interface->Add<dusk::elements::DropdownMenu>(dusk::ElementCreationDescription(topLayout, Rect()));
	EditMenu->m_contents = "Edit";
	EditMenu->Add("Undo", 'U', true, [](){});
	EditMenu->Add("Redo", 'R', true, [](){});
	EditMenu->Add("Clone", 'L', true, [](){});
	EditMenu->Add("Copy", 'C', true, [](){});
	EditMenu->Add("Cut", 'T', true, [](){});
	EditMenu->Add("Paste", 'P', true, [](){});

	auto ViewMenu = dusk_interface->Add<dusk::elements::DropdownMenu>(dusk::ElementCreationDescription(topLayout, Rect()));
	ViewMenu->m_contents = "View";
	ViewMenu->Add("Horizontal Mode", 0, true, [](){});
	ViewMenu->Add("Vertical Mode", 0, true, [](){});
	ViewMenu->Add("Script Mode", 0, true, [](){});
	ViewMenu->Add("Reset", 'R', true, [](){});
	ViewMenu->AddDivingSpacer();
	ViewMenu->Add("Toggle Grid", 0, true, [](){});
	ViewMenu->Add("Toggle Snap", 0, true, [](){});
	ViewMenu->Add("Larger Grid", 0, true, [](){});
	ViewMenu->Add("Smaller Grid", 0, true, [](){});

	auto HelpMenu = dusk_interface->Add<dusk::elements::DropdownMenu>(dusk::ElementCreationDescription(topLayout, Rect()));
	HelpMenu->m_contents = "Help";
	HelpMenu->Add("About", 'A', true, [](){});
}

m04::editor::sequence::TopMenu::~TopMenu ()
{
}

void m04::editor::sequence::TopMenu::UpdateHotkeys ( void )
{
	if (core::Input::Key(core::kVkControl))
	{
		if (core::Input::Keydown('S'))
		{
			if (!core::Input::Key(core::kVkShift))
			{
				BeginSaveFile();
			}
			else
			{
				BeginSaveAsFile();
			}
		}

		if (core::Input::Keydown('O'))
		{
			BeginLoadFile();
		}
	}
}

void m04::editor::sequence::TopMenu::BeginNewFile ( void )
{
	if (main_editor->GetWorkspaceDirty())
	{
		core::shell::ShowErrorMessage("Save before creating new workspace");
	}
	else
	{
		NewFile();
	}
}
void m04::editor::sequence::TopMenu::NewFile ( void )
{
	auto board = main_editor->GetNodeBoardState();
	board->ClearAllNodes();
	main_editor->SetWorkspaceDirty(false); // Clear workspace dirty flag
}

void m04::editor::sequence::TopMenu::SaveTest ( void )
{
	auto board = main_editor->GetNodeBoardState();
	if (board != NULL)
	{
		m04::editor::sequence::OsfSerializer serializer (".game/testosf.txt");
		board->Save(&serializer);
	}
}

void m04::editor::sequence::TopMenu::LoadTest(void)
{
	auto board = main_editor->GetNodeBoardState();
	if (board != NULL)
	{
		m04::editor::sequence::OsfDeserializer deserializer (".game/testosf.txt");
		board->Load(&deserializer);
	}
}

void m04::editor::sequence::TopMenu::BeginSaveFile ( void )
{
	if (main_editor->GetSaveTargetFilename().length() == 0)
	{
		BeginSaveAsFile();
	}
	else
	{
		SaveFile(main_editor->GetSaveTargetFilename());
	}
}

void m04::editor::sequence::TopMenu::BeginSaveAsFile ( void )
{
	if (savefileDialog == NULL)
	{
		savefileDialog = dusk_interface->AddDialog<dusk::dialogs::SaveFile>(dusk::DialogCreationDescription());
	}

	savefileDialog->as<dusk::dialogs::SaveFile>()->m_defaultDirectory = 
		(!main_editor->GetSaveTargetFilename().empty())
		? fs::path(main_editor->GetSaveTargetFilename()).parent_path().string().c_str()
		: ".";

	savefileDialog->Show();
	savefileDialog->as<dusk::dialogs::SaveFile>()->SetOnAccept([this](const std::string& filename){ SaveFile(filename); });
}

void m04::editor::sequence::TopMenu::SaveFile ( const std::string& filename )
{
	auto board = main_editor->GetNodeBoardState();
	if (board != NULL)
	{
		m04::editor::sequence::OsfSerializer serializer (filename.c_str());
		board->Save(&serializer);
		main_editor->SetWorkspaceDirty(false); // Clear workspace dirty flag
		main_editor->SetSaveTargetFilename(filename.c_str());
	}
}

void m04::editor::sequence::TopMenu::BeginLoadFile ( void )
{
	if (loadfileDialog == NULL)
	{
		loadfileDialog = dusk_interface->AddDialog<dusk::dialogs::LoadFile>(dusk::DialogCreationDescription());
	}

	loadfileDialog->as<dusk::dialogs::LoadFile>()->m_defaultDirectory = 
		(!main_editor->GetSaveTargetFilename().empty())
		? fs::path(main_editor->GetSaveTargetFilename()).parent_path().string().c_str()
		: ".";

	loadfileDialog->Show();
	loadfileDialog->as<dusk::dialogs::LoadFile>()->SetOnAccept([this](const std::string& filename){ LoadFile(filename); });
}

void m04::editor::sequence::TopMenu::LoadFile ( const std::string& filename )
{
	auto board = main_editor->GetNodeBoardState();
	if (board != NULL)
	{
		m04::editor::sequence::OsfDeserializer deserializer (filename.c_str());
		board->Load(&deserializer);
		main_editor->SetWorkspaceDirty(false); // Clear workspace dirty flag
		main_editor->SetSaveTargetFilename(filename.c_str()); // Set the save-target on load for easy & quick saving
	}
}
