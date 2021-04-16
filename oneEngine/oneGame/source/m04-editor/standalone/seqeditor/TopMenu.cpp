#include "TopMenu.h"

#include "engine-common/dusk/controls/Panel.h"
#include "engine-common/dusk/controls/DropdownMenu.h"
#include "engine-common/dusk/layouts/Horizontal.h"
#include "engine-common/dusk/dialogs/SaveFile.h"

#include "engine/state/CGameState.h"

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
	fileMenu->Add("New", 'N', true, [](){});
	fileMenu->Add("Save (Test)", 0, true, [this](){ SaveTest(); });
	fileMenu->Add("Save", 'S', true, [this](){ BeginSaveFile(); });
	fileMenu->Add("Save As", 0, true, [](){});
	fileMenu->Add("Open", 'O', true, [](){});
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

	auto HelpMenu = dusk_interface->Add<dusk::elements::DropdownMenu>(dusk::ElementCreationDescription(topLayout, Rect()));
	HelpMenu->m_contents = "Help";
	HelpMenu->Add("About", 'A', true, [](){});
}

m04::editor::sequence::TopMenu::~TopMenu ()
{
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

void m04::editor::sequence::TopMenu::BeginSaveFile ( void )
{
	if (savefileDialog == NULL)
	{
		savefileDialog = dusk_interface->AddDialog<dusk::dialogs::SaveFile>(dusk::DialogCreationDescription());
	}

	savefileDialog->Show();

	//dusk_interface->EnterDialogue(savefileDialog);
	savefileDialog->as<dusk::dialogs::SaveFile>()->SetOnAccept([this](const std::string& filename){ SaveFile(filename); });
}

void m04::editor::sequence::TopMenu::SaveFile ( const std::string& filename )
{
	auto board = main_editor->GetNodeBoardState();
	if (board != NULL)
	{
		m04::editor::sequence::OsfSerializer serializer (filename.c_str());
		board->Save(&serializer);
	}
}

void m04::editor::sequence::TopMenu::LoadTest(void)
{
	auto board = main_editor->GetNodeBoardState();
	if (board != NULL)
	{
		//Go fucking do something, I don't know.
	}
}