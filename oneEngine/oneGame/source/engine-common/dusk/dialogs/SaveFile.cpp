#include "SaveFile.h"

#include "core-ext/std/filesystem.h"
#include "core-ext/system/io/Volumes.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

#include "engine-common/dusk/layouts/Horizontal.h"
#include "engine-common/dusk/layouts/Vertical.h"
#include "engine-common/dusk/layouts/Grid.h"
#include "engine-common/dusk/controls/Label.h"
#include "engine-common/dusk/controls/Button.h"
#include "engine-common/dusk/controls/TextField.h"
#include "engine-common/dusk/controls/TextListView.h"
#include "engine-common/dusk/controls/Spacer.h"

void dusk::dialogs::SaveFile::PostCreate ( void )
{
	auto fullLayout = m_interface->Add<dusk::layouts::StretchGridRows>(dusk::LayoutCreationDescription(this));
	m_elements.push_back(fullLayout);

	auto topLayout = m_interface->Add<dusk::layouts::StretchGridRow>(dusk::LayoutCreationDescription(fullLayout, Vector2f(500, 40)));
	topLayout->m_layoutLockHeight = true;
	m_elements.push_back(topLayout);

	auto middleLayout = m_interface->Add<dusk::layouts::StretchGridRow>(dusk::LayoutCreationDescription(fullLayout, Vector2f(500, 350)));
	middleLayout->m_layoutLockHeight = false;
	middleLayout->m_horizontalScale = dusk::layouts::JustifyScaleStyle::kStretch;
	m_elements.push_back(middleLayout);

	auto bottomLayout = m_interface->Add<dusk::layouts::StretchGridRow>(dusk::LayoutCreationDescription(fullLayout, Vector2f(500, 40)));
	bottomLayout->m_justify = dusk::layouts::AlignStyleHorizontal::kRight;
	bottomLayout->m_layoutLockHeight = true;
	m_elements.push_back(bottomLayout);

	auto bottomestLayout = m_interface->Add<dusk::layouts::StretchGridRow>(dusk::LayoutCreationDescription(fullLayout, Vector2f(500, 40)));
	bottomestLayout->m_justify = dusk::layouts::AlignStyleHorizontal::kRight;
	bottomestLayout->m_layoutLockHeight = true;
	m_elements.push_back(bottomestLayout);

	// Create top row items
	{
		auto prevButton = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(topLayout, Rect(0, 0, 40, 40)));
		prevButton->m_contents = "<";
		m_elements.push_back(prevButton);

		auto nextButton = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(topLayout, Rect(0, 0, 40, 40)));
		nextButton->m_contents = ">";
		m_elements.push_back(nextButton);

		auto upButton = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(topLayout, Rect(0, 0, 40, 40)));
		upButton->m_contents = "..";
		m_elements.push_back(upButton);

		auto pathField = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(topLayout, Rect(0, 0, 100, 40)));
		pathField->m_contents = "TEST";
		m_elements.push_back(pathField);

		m_navBackward = prevButton;
		m_navForward = nextButton;
		m_navUp = upButton;
	}

	// Create the file browser area
	{
		auto folderListview = m_interface->Add<dusk::elements::TextListView>(dusk::ElementCreationDescription(middleLayout, Rect(0, 0, 100, 350)));
		folderListview->m_contents = "FOLDERS";
		folderListview->AddMenuEntry("test");
		folderListview->AddMenuEntry("entries");
		folderListview->AddMenuEntry("in the magical");
		folderListview->AddMenuEntry("LISTVIEW");
		folderListview->SetOnClickEntry([this](dusk::elements::TextListView*, int selection) { OnSelectItemInFolderview(selection); });
		m_elements.push_back(folderListview);

		auto fileListview = m_interface->Add<dusk::elements::TextListView>(dusk::ElementCreationDescription(middleLayout, Rect(0, 0, 300, 350)));
		fileListview->m_contents = "FOLDERS";
		fileListview->AddMenuEntry("test");
		fileListview->AddMenuEntry("entries");
		fileListview->AddMenuEntry("in the magical");
		fileListview->AddMenuEntry("LISTVIEW");
		fileListview->SetOnClickEntry([this](dusk::elements::TextListView*, int selection) { OnClickItemInFileview(selection); });
		fileListview->SetOnDoubleClickEntry([this](dusk::elements::TextListView*, int selection) { OnSelectItemInFileview(selection); });
		m_elements.push_back(fileListview);

		m_folderListview = folderListview;
		m_filesListview = fileListview;
	}

	// Create the bottoms on the bottom
	{
		auto sideSpacer = m_interface->Add<dusk::elements::Spacer>(dusk::ElementCreationDescription(bottomLayout, Rect(0, 0, 500 - 400 - 5, 40)));
		m_elements.push_back(sideSpacer);

		auto pathField = m_interface->Add<dusk::elements::TextField>(dusk::ElementCreationDescription(bottomLayout, Rect(0, 0, 300, 40)));
		pathField->m_contents = "";
		m_elements.push_back(pathField);

		auto middleSpacer = m_interface->Add<dusk::elements::Spacer>(dusk::ElementCreationDescription(bottomLayout, Rect(0, 0, 5, 40)));
		m_elements.push_back(middleSpacer);

		auto typeField = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(bottomLayout, Rect(0, 0, 100, 40)));
		typeField->m_contents = "TYPES";
		typeField->m_layoutLockWidth = true;
		m_elements.push_back(typeField);

		m_filenameField = pathField;
		m_filetypeField = typeField;
	}

	// Create the bottoms on the bottom
	{
		auto acceptButton = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(bottomestLayout, Rect(0, 0, 100, 40)));
		acceptButton->m_contents = "ACCEPT";
		m_elements.push_back(acceptButton);

		auto cancelButton = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(bottomestLayout, Rect(0, 0, 100, 40)));
		cancelButton->m_contents = "CANCEL";
		m_elements.push_back(cancelButton);

		m_buttonAccept = acceptButton;
		m_buttonCancel = cancelButton;
	}
}

dusk::dialogs::SaveFile::~SaveFile()
{
	for (dusk::Element* element : m_elements)
	{
		delete element;
	}
	m_elements.clear();
}

void dusk::dialogs::SaveFile::Show ( void )
{
	FileViewer::Show();

	if (m_defaultDirectory != ".")
	{
		m_currentDirectory = m_defaultDirectory;
	}
	m_filenameField->as<dusk::elements::TextField>()->m_contents = "";
	UpdateDirectoryListing();
}

void dusk::dialogs::SaveFile::Update ( const UIStepInfo* stepinfo )
{
	// Do delayed initialization
	if (!m_ready)
	{
		m_currentDirectory = m_defaultDirectory;
		UpdateDirectoryListing();

		m_ready = true;
	}

	FileViewer::Update(stepinfo);

	if (m_isEnabled && m_wasDrawn)
	{
		if (m_navBackward->m_isActivated
			|| (core::Input::Key(VK_MENU, stepinfo->input_index) && core::Input::Keydown(VK_LEFT, stepinfo->input_index)))
		{
			NavigationBackward();
		}
		else if (m_navForward->m_isActivated
			|| (core::Input::Key(VK_MENU, stepinfo->input_index) && core::Input::Keydown(VK_RIGHT, stepinfo->input_index)))
		{
			NavigationForward();
		}
		else if (m_navUp->m_isActivated
			|| (core::Input::Key(VK_MENU, stepinfo->input_index) && core::Input::Keydown(VK_UP, stepinfo->input_index)))
		{
			NavigationUp();
		}

		// Refresh.
		if (core::Input::Keydown(VK_F5, stepinfo->input_index))
		{
			UpdateDirectoryListing();
		}

		// Check buttons
		if (m_buttonCancel->m_isActivated)
		{
			if (m_onCancel != nullptr)
			{
				m_onCancel();
			}
			Hide();
		}
		if (m_buttonAccept->m_isActivated)
		{
			if (m_filenameField->as<dusk::elements::TextField>()->m_contents.empty())
			{
				// If the input is empty, we don't do anything.
			}
			else
			{
				fs::path currentPath = fs::absolute(m_currentDirectory);
				fs::path selectionPath = currentPath / m_filenameField->as<dusk::elements::TextField>()->m_contents;
				if (fs::exists(selectionPath))
				{
					// If selected a directory, move into the directory instead.
					if (fs::is_directory(selectionPath))
					{
						m_currentDirectory = selectionPath.string();
						UpdateDirectoryListing();
					}
					// If selected a file, then check that either the file is a normal file
					else if (fs::is_regular_file(selectionPath))
					{
						if (m_onAccept != nullptr)
						{
							m_onAccept(selectionPath.string());
						}
						Hide();
					}
				}
				// If the path exists, let's just select the path properly.
				else if (fs::exists(currentPath))
				{
					if (m_onAccept != nullptr)
					{
						m_onAccept(selectionPath.string());
					}
					Hide();
				}
			}
		}
	}
}

void dusk::dialogs::SaveFile::Render ( UIRendererContext* uir )
{
	uir->setFocus(dusk::kFocusStyleAutomaticNoHover);
	uir->setColor(dusk::kColorStyleBackground);
	uir->drawRectangle(this, m_absoluteRect);
}

void dusk::dialogs::SaveFile::UpdateDirectoryListing ( bool isNavigation )
{
	auto folderListview = m_folderListview->as<dusk::elements::TextListView>();
	auto filesListview = m_filesListview->as<dusk::elements::TextListView>();

	folderListview->ClearMenuEntries();
	filesListview->ClearMenuEntries();

	// Need to get everything in the current directory:
	m_fileListing.clear();
	for (auto& p : fs::directory_iterator(m_currentDirectory))
	{
		auto& path = p.path();
		m_fileListing.push_back(path.string());
	}
	
	// Now we generate the unique stuff for the folder view:
	m_folderListing.clear();
	// Add in volumes
	{
		std::vector<std::string> volumes;
		core::io::EnumerateVolumes(volumes);
		for (auto& volume : volumes)
		{
			m_folderListing.push_back(volume);
		}
	}
	// Add in current directory and everything parent to it
	{
		fs::path l_absolutePath = fs::canonical(m_currentDirectory);
		std::string l_rootPrefix = "";
		if (l_absolutePath.has_root_name())
		{
			l_rootPrefix = (l_absolutePath.root_name() / "/").string();
			std::string smallified = l_absolutePath.string().substr(l_rootPrefix.length());
			l_absolutePath = smallified;
		}

		fs::path l_currentPath = l_rootPrefix;
		for (auto& itr : l_absolutePath)
		{
			l_currentPath /= itr;
			m_folderListing.push_back(l_currentPath.string());
		}	
	}
	// Sort alphabetically so that items stick with the drives they're on.
	std::sort(m_folderListing.begin(), m_folderListing.end());

	// Set up the listviews now
	for (auto& str : m_fileListing)
	{
		fs::path filename (str);
		filesListview->AddMenuEntry(filename.filename().string());
	}
	for (auto& str : m_folderListing)
	{
		fs::path filename = str;
		if (filename.has_relative_path())
		{
			folderListview->AddMenuEntry(std::string(std::count(str.begin(), str.end(), '\\'), ' ') + filename.filename().string());
		}
		else
		{
			folderListview->AddMenuEntry(str);
		}
	}

	// Update navigation history with the current directory.
	if (!isNavigation)
	{
		if (m_navigationHistory.empty()
			|| fs::canonical(m_navigationHistory.back()) != fs::canonical(m_currentDirectory))
		{
			// Resize to the currnet nav point.
			if (!m_navigationHistory.empty())
			{
				m_navigationHistory.resize(m_navigationPosition + 1);
			}
			// Add new nav point
			m_navigationHistory.push_back(m_currentDirectory);
			// Save new nav position
			m_navigationPosition = (int)m_navigationHistory.size() - 1;
		}
	}
	// Update navigation button enable states
	m_navBackward->m_isEnabled = m_navigationPosition > 0;
	m_navForward->m_isEnabled = m_navigationPosition < m_navigationHistory.size() - 1;
}

void dusk::dialogs::SaveFile::OnSelectItemInFileview ( const int selection )
{
	fs::path l_nextPath = fs::absolute(m_fileListing[selection]);

	if (fs::is_directory(l_nextPath))
	{
		m_currentDirectory = l_nextPath.string();
		UpdateDirectoryListing();
	}
}

void dusk::dialogs::SaveFile::OnSelectItemInFolderview ( const int selection )
{
	m_currentDirectory = fs::absolute(m_folderListing[selection]).string();
	UpdateDirectoryListing();
}

void dusk::dialogs::SaveFile::OnClickItemInFileview ( const int selection )
{
	fs::path l_nextPath = fs::absolute(m_fileListing[selection]);

	if (fs::is_regular_file(l_nextPath))
	{
		m_filenameField->as<dusk::elements::TextField>()->m_contents = l_nextPath.filename().string();
	}
}

void dusk::dialogs::SaveFile::NavigationForward ( void )
{
	m_navigationPosition += 1;
	m_currentDirectory = m_navigationHistory[m_navigationPosition];
	UpdateDirectoryListing(true);
}
void dusk::dialogs::SaveFile::NavigationBackward ( void )
{
	m_navigationPosition -= 1;
	m_currentDirectory = m_navigationHistory[m_navigationPosition];
	UpdateDirectoryListing(true);
}
void dusk::dialogs::SaveFile::NavigationUp ( void )
{
	m_currentDirectory = fs::canonical(m_currentDirectory + "/..").string();
	UpdateDirectoryListing();
}