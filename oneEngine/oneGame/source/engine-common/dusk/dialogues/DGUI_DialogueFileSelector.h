
#ifndef _C_DUSK_GUI_DIALOGUE_TEXT_
#define _C_DUSK_GUI_DIALOGUE_TEXT_

#include "../controls/CDuskGUIDialogue.h"
#include "core/system/System.h"

namespace Dusk
{

	class DialogueFileSelector : public CDuskGUIDialogue
	{
	public:
		DialogueFileSelector ( const int moverride=40 );

		// Overridable update
		void Update ( void );
		void Render ( void );

	public:
		// 0 if load, 1 if save
		int io_mode;

		std::vector<System::sFileDialogueEntry> m_fileTypes;

		string m_currentPath;

	private:
		friend CDuskGUI;

		bool hasSelection;
		bool endMe;

		struct filelist_entry_t
		{
			string	filename;
			bool	isFolder;
		};
		std::vector<filelist_entry_t> m_filelist;
		int		m_mouseover;
		int		m_selected;

		bool	m_namebox_focus;
		string	m_namebox_value;

		bool	m_context_showsave;
		bool	m_context_exists;
		bool	m_context_nameconflict;

		bool	m_button_mousedown;
		int		m_button_mouseover;

	};

}

#endif