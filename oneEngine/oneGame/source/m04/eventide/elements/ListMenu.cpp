#include "ListMenu.h"
#include "m04/eventide/elements/DefaultStyler.h"
#include "./Button.h"

#include "core/time.h"
#include "core/math/Easing.h"

void ui::eventide::elements::ListMenuButton::OnActivated ( void )
{
	m_listMenu->OnActivated(m_choiceIndex);
}

ui::eventide::elements::ListMenu::ListMenu ( ui::eventide::UserInterface* ui )
	: Element(ui)
{
	m_mouseInteract = MouseInteract::kNone;
}

ui::eventide::elements::ListMenu::~ListMenu ( void )
{
	for (Button* button : m_choiceButtons)
	{
		delete button;
	}
	m_choiceButtons.empty();
}

void ui::eventide::elements::ListMenu::BuildMesh ( void )
{
	; // None.
}

void ui::eventide::elements::ListMenu::SetEnabled ( bool enable )
{
	if (m_enabled != enable)
	{
		m_enabled = enable;
		//m_mouseInteract = m_enabled ? MouseInteract::kCapturing : MouseInteract::kNone;

		for (Button* button : m_choiceButtons)
		{
			button->SetEnabled(enable);
		}
	}
}

void ui::eventide::elements::ListMenu::OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame )
{
	if (m_choiceButtons.size() != m_choices.size())
	{
		// Too many buttons, free some
		if (m_choiceButtons.size() > m_choices.size())
		{
			for (size_t i = m_choices.size(); i < m_choiceButtons.size(); ++i)
			{
				delete m_choiceButtons[i];
			}
			m_choiceButtons.resize(m_choices.size());
		}
		// Too few buttons, allocate some
		else
		{
			while (m_choiceButtons.size() < m_choices.size())
			{
				m_choiceButtons.push_back(new Button(m_ui));
			}
		}
	}

	// Get the widest string from all of the buttons
	size_t max_choice_length = 0;
	for (std::string& choice : m_choices)
	{
		max_choice_length = std::max<size_t>(max_choice_length, choice.length());
	}

	// Update all the buttons
	const float buttonWidth = max_choice_length * DefaultStyler.text.buttonSize * 0.8F;
	for (int i = 0; i < m_choiceButtons.size(); ++i)
	{
		Button*			button = m_choiceButtons[i];
		std::string&	choice = m_choices[i];

		button->m_contents = choice;
		button->SetParent(this);
		button->SetBBox(core::math::BoundingBox(
			Matrix4x4(),
			Vector3f(0,				(i + 0) * -(DefaultStyler.text.buttonSize + DefaultStyler.box.menuPadding), 0),
			Vector3f(buttonWidth,	(i + 1) * -(DefaultStyler.text.buttonSize + DefaultStyler.box.menuPadding), 4)
			)
		);

		button->RequestUpdateMesh();
	}

	m_frameUpdate = FrameUpdate::kNone;
}

void ui::eventide::elements::ListMenu::SetListChoices ( const std::vector<std::string>& choices )
{
	m_choices = choices;
	// We use the frame update to update the object. Let's do that next.
	m_frameUpdate = FrameUpdate::kPerFrame;
}

std::vector<std::string>& ui::eventide::elements::ListMenu::GetListChoicesForWrite ( void )
{
	// We use the frame update to update the object. Let's do that next.
	m_frameUpdate = FrameUpdate::kPerFrame;
	// Return our list for edit!
	return m_choices;
}