
#ifndef _C_DUSK_GUI_DIALOGUE_COLORPICKER_
#define _C_DUSK_GUI_DIALOGUE_COLORPICKER_

#include "../controls/CDuskGUIDialogue.h"
#include "core/system/System.h"

namespace Dusk
{
	// Over-the-top color picker class for DUSK ui.
	// Is a dedicated dialogue that takes focus away from everything else
	class DialogueColorpicker : public CDuskGUIDialogue
	{
	public:
		DialogueColorpicker ( const int moverride=41 );

		// Overridable update
		void Update ( void );
		void Render ( void );

	public:
		// Current color selection. All other values are decompositions of this RGBA value.
		Color m_currentColor;

		// Colorpicker source. Set if dialogue created by a colorpicker.
		Handle m_sourcePicker;

	private:
		friend CDuskGUI;

		bool hasSelection;
		bool endMe;

		void drawColorWheel ( const Vector2f& position );
		void drawColorSliders ( const Vector2f& position );

		void updateColorSliders ( const Vector2f& position );

		enum class Mode : uint8_t
		{
			None,
			Outside,
			R,
			G,
			B,
			H,
			S,
			V,
			A
		};
		Mode current_mode_hover;
		Mode current_mode;

		Color color_start;
		Vector2f drag_start;
		Vector2f drag_now;
		bool dragging;
	};
}

#endif//_C_DUSK_GUI_DIALOGUE_COLORPICKER_