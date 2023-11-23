#include "ListMenuHierarchical.h"
#include "m04/eventide/elements/DefaultStyler.h"
#include "./Button.h"

#include "core/time.h"
#include "core/math/Easing.h"

#include <functional>

void ui::eventide::elements::ListMenuHierarchicalButton::OnActivated ( void )
{
	if (m_choiceIndex >= 0 && !m_isGroup)
	{
		m_listMenu->OnActivated(m_choiceIndex);
	}
}

void ui::eventide::elements::ListMenuHierarchicalButton::OnEventMouse ( const EventMouse& mouse_event )
{
	if (mouse_event.type == EventMouse::Type::kEnter)
	{
		m_listMenu->ShowChoiceSubgroup(m_choiceIndex, m_isGroup);
	}
	else if (mouse_event.type == EventMouse::Type::kExit)
	{
	}
	// Percolate the messages to button items as well.
	Button::OnEventMouse(mouse_event);
}


ui::eventide::elements::ListMenuHierarchical::ListMenuHierarchical ( ui::eventide::UserInterface* ui )
	: ui::eventide::elements::ListMenu(ui)
{
	m_mouseInteract = MouseInteract::kNone;
}

ui::eventide::elements::ListMenuHierarchical::~ListMenuHierarchical ( void )
{
	m_choiceLevels.clear();
}

void ui::eventide::elements::ListMenuHierarchical::SetListChoices ( const std::vector<std::string>& choices )
{
	m_choiceLevels.resize(choices.size(), {0, 0});
	ui::eventide::elements::ListMenu::SetListChoices(choices);
}

void ui::eventide::elements::ListMenuHierarchical::SetListChoices ( const std::vector<HeirarchicalChoice>& choices )
{
	std::function<void(const HeirarchicalChoice& choice, uint8_t level)> AddChoice;

	AddChoice = [&](const HeirarchicalChoice& choice, uint8_t level)
	{
		m_choices.push_back(choice.name);
		m_choiceLevels.push_back({level, choice.choices ? (uint8_t)choice.choices->size() : 0u });
		if (choice.choices)
		{
			for (const auto& subchoice : *choice.choices)
			{
				AddChoice(subchoice, level + 1);
			}
		}
	};

	for (const auto& choice : choices)
	{
		AddChoice(choice, 0);
	}

	// We use the frame update to update the object. Let's do that next.
	m_frameUpdate = FrameUpdate::kPerFrame;
}

void ui::eventide::elements::ListMenuHierarchical::ShowChoiceSubgroup ( int choiceIndex, bool visible )
{
	const auto& choiceLevel = m_choiceLevels[choiceIndex];

	/*ARCORE_ASSERT(m_choiceLevels[choiceIndex].m_childrenCount > 0);
	for (uint32_t i = 0; i < m_choiceLevels[choiceIndex].m_childrenCount; ++i)
	{
		m_choiceButtons[choiceIndex + 1 + (int)i]->SetVisible(visible);
	}*/

	if (visible)
	{
		ARCORE_ASSERT(choiceLevel.m_childrenCount > 0);
		// Make everything at the same level invisible.
		for (uint32_t i = 0; i < m_choiceLevels.size(); ++i)
		{
			if (m_choiceLevels[i].m_level > choiceLevel.m_level)
			{
				m_choiceButtons[i]->SetVisible(false);
			}
		}
		// Make our child visible
		for (uint32_t i = 0; i < choiceLevel.m_childrenCount; ++i)
		{
			m_choiceButtons[choiceIndex + 1 + (int)i]->SetVisible(visible);
		}
	}
	else
	{
		ARCORE_ASSERT(m_choiceLevels[choiceIndex].m_childrenCount == 0);
		// Make everything at the same level invisible.
		for (uint32_t i = 0; i < m_choiceLevels.size(); ++i)
		{
			if (m_choiceLevels[i].m_level > choiceLevel.m_level)
			{
				m_choiceButtons[i]->SetVisible(false);
			}
		}
	}
}

void ui::eventide::elements::ListMenuHierarchical::OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame )
{
	if (m_choiceButtons.size() != m_choices.size())
	{
		// Too many buttons, free some
		if (m_choiceButtons.size() > m_choices.size())
		{
			for (size_t i = m_choices.size(); i < m_choiceButtons.size(); ++i)
			{
				m_choiceButtons[i]->Destroy();
			}
			m_choiceButtons.resize(m_choices.size());
		}
		// Too few buttons, allocate some
		else
		{
			while (m_choiceButtons.size() < m_choices.size())
			{
				m_choiceButtons.push_back(new ListMenuHierarchicalButton(
					this,
					(int)m_choiceButtons.size(),
					m_choiceLevels[m_choiceButtons.size()].m_childrenCount > 0 ? true : false,
					m_ui));
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
	struct LevelTracker
	{
		int height;
		int remainingItems;
	};
	std::vector<LevelTracker> heightTracker;
	heightTracker.push_back({0, (int)m_choiceButtons.size()});

	const float buttonWidth = max_choice_length * DefaultStyler.text.buttonSize * 0.4F;
	for (int i = 0; i < m_choiceButtons.size(); ++i)
	{
		Button*			button = m_choiceButtons[i];
		std::string&	choice = m_choices[i];

		button->m_contents = (m_choiceLevels[i].m_childrenCount > 0) ? (choice + ">") : choice;
		button->SetParent(this);
		button->SetBBox(core::math::BoundingBox(
			Matrix4x4(),
			Vector3f(buttonWidth * ((int)heightTracker.size() - 1),
									(heightTracker.back().height + 0) * -(DefaultStyler.text.buttonSize + DefaultStyler.box.menuPadding), 0),
			Vector3f(buttonWidth * (int)heightTracker.size(),
									(heightTracker.back().height + 1) * -(DefaultStyler.text.buttonSize + DefaultStyler.box.menuPadding), 4)
			)
		);
		button->SetVisible(heightTracker.size() == 1);
		button->RequestUpdateMesh();

		// Count down the remaining items
		heightTracker.back().remainingItems--;
		// Count up the height
		heightTracker.back().height++;
		// If we have children, then we expect the next items to be children.
		if (m_choiceLevels[i].m_childrenCount != 0)
		{
			heightTracker.push_back({heightTracker.back().height - 1, m_choiceLevels[i].m_childrenCount});
		}
		else
		{
			// If we have no more remaining items, go back up a level
			while (heightTracker.back().remainingItems == 0)
				heightTracker.pop_back();
		}
		
	}

	m_frameUpdate = FrameUpdate::kNone;
}