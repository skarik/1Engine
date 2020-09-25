#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_DIALOG_SAVE_FILE_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_DIALOG_SAVE_FILE_H_

#include "engine-common/dusk/dialogs/FileViewer.h"
#include "core/system/System.h"
#include <vector>

namespace dusk {
namespace dialogs {

	// Dialog that allows to browse through the filesystem and find a filename of result.
	class SaveFile : public dusk::dialogs::FileViewer
	{
	public:
		ENGCOM_API explicit		SaveFile()
			: dusk::dialogs::FileViewer()
			{}

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

	public:
		bool				hasSelection = false;
		std::string			selectedFilename = "";

		// Directory the filesystem browser starts on
		std::string			m_defaultDirectory = ".";
		// Filetype filter. When empty, allows all files
		std::vector<System::sFileDialogueEntry>
							m_filetypes;
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_DIALOG_SAVE_FILE_H_