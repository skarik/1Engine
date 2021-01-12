#include "SaveFile.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

#include "engine-common/dusk/layouts/Horizontal.h"
#include "engine-common/dusk/layouts/Vertical.h"
#include "engine-common/dusk/controls/Label.h"
#include "engine-common/dusk/controls/Button.h"
#include "engine-common/dusk/controls/TextField.h"

void dusk::dialogs::SaveFile::PostCreate ( void )
{
	auto fullLayout = m_interface->Add<dusk::layouts::Vertical>(dusk::LayoutCreationDescription(this));
	m_elements.push_back(fullLayout);

	auto topLayout = m_interface->Add<dusk::layouts::HorizontalFit>(dusk::LayoutCreationDescription(fullLayout));
	topLayout->m_spaceItems = false;
	m_elements.push_back(topLayout);

	auto middleLayout = m_interface->Add<dusk::layouts::HorizontalFit>(dusk::LayoutCreationDescription(fullLayout));
	middleLayout->m_spaceItems = true;
	m_elements.push_back(middleLayout);

	auto bottomLayout = m_interface->Add<dusk::layouts::HorizontalFit>(dusk::LayoutCreationDescription(fullLayout));
	bottomLayout->m_spaceItems = false;
	bottomLayout->m_justify = dusk::layouts::AlignStyleHorizontal::kRight;
	m_elements.push_back(bottomLayout);

	auto bottomestLayout = m_interface->Add<dusk::layouts::HorizontalFit>(dusk::LayoutCreationDescription(fullLayout));
	bottomestLayout->m_spaceItems = false;
	bottomestLayout->m_justify = dusk::layouts::AlignStyleHorizontal::kRight;
	m_elements.push_back(bottomestLayout);

	// Create top row items
	{
		auto prevButton = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(topLayout, Rect(0, 0, 40, 40)));
		prevButton->m_contents = "<";
		m_elements.push_back(prevButton);

		auto nextButton = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(topLayout, Rect(0, 0, 40, 40)));
		nextButton->m_contents = ">";
		m_elements.push_back(nextButton);

		auto pathField = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(topLayout, Rect(0, 0, 100, 40)));
		pathField->m_contents = "TEST";
		m_elements.push_back(pathField);
	}

	// Create the file browser area
	{
		auto folderListview = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(middleLayout, Rect(0, 0, 100, 350)));
		folderListview->m_contents = "FOLDERS";
		m_elements.push_back(folderListview);

		auto fileListview = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(middleLayout, Rect(0, 0, 300, 350)));
		fileListview->m_contents = "FOLDERS";
		m_elements.push_back(fileListview);
	}

	// Create the bottoms on the bottom
	{
		auto pathField = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(bottomLayout, Rect(0, 0, 200, 40)));
		pathField->m_contents = "FILE";
		m_elements.push_back(pathField);

		auto typeField = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(bottomLayout, Rect(0, 0, 100, 40)));
		typeField->m_contents = "TYPES";
		m_elements.push_back(typeField);
	}

	// Create the bottoms on the bottom
	{
		auto acceptButton = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(bottomestLayout, Rect(0, 0, 100, 40)));
		acceptButton->m_contents = "ACCEPT";
		m_elements.push_back(acceptButton);

		auto cancelButton = m_interface->Add<dusk::elements::Button>(dusk::ElementCreationDescription(bottomestLayout, Rect(0, 0, 100, 40)));
		cancelButton->m_contents = "CANCEL";
		m_elements.push_back(cancelButton);
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

void dusk::dialogs::SaveFile::Update ( const UIStepInfo* stepinfo )
{
	FileViewer::Update(stepinfo);
}

void dusk::dialogs::SaveFile::Render ( UIRendererContext* uir )
{
	uir->setFocus(dusk::kFocusStyleAutomaticNoHover);
	uir->setColor(dusk::kColorStyleBackground);
	uir->drawRectangle(this, m_absoluteRect);
}