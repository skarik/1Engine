#include "FloatField.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

void dusk::elements::FloatField::Update ( const UIStepInfo* stepinfo )
{
	Element::Update(stepinfo);

	if ( !m_isEnabled )
	{
	}
	else
	{
		if ( m_isFocused )
		{
			const auto& inputString = core::Input::FrameInputString();
			for (const auto& input : inputString)
			{
				if ( input && isprint(input) && (isdigit(input) || input == '.') )
				{
					m_contents += input;
				}
				if ( input == core::kVkBackspace )
				{
					m_contents = m_contents.substr(0, m_contents.length() - 1);
				}
			}
		}

		if ( m_isMouseIn && m_wasDrawn )
		{
			// Mouse controls
			if ( core::Input::MouseDown(core::kMBLeft) )
			{
			}
		}
		else
		{
		}
	}
}

float dusk::elements::FloatField::GetValue ( void )
{
	return std::strtof(m_contents.c_str(), NULL);
}