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

		ENGCOM_API virtual		~SaveFile();

		//	PostCreate() : Called after creation.
		void					PostCreate ( void ) override;
		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

		//	SetOnAccept(Fn) : Sets a callback for when a file is selected and accepted.
		// Function must be the signature: void Callback(const std::string&)
		ENGCOM_API void			SetOnAccept ( std::function<void(const std::string&)>&& Fn )
		{
			m_onAccept = Fn;
		}
		//	SetOnCancel(Fn) : Sets a callback for when user clicks cancel instead.
		// Function must be the signature: void Callback(void)
		ENGCOM_API void			SetOnCancel ( std::function<void(void)>&& Fn )
		{
			m_onCancel = Fn;
		}

		//	Show() : Shows the dialog.
		ENGCOM_API void			Show ( void ) override;

	public:
		bool				hasSelection = false;
		std::string			selectedFilename = "";

		// Directory the filesystem browser starts on
		std::string			m_defaultDirectory = ".";
		// Filetype filter. When empty, allows all files
		std::vector<System::sFileDialogueEntry>
							m_filetypes;

	private:
		bool				m_ready = false;

		std::function<void(const std::string&)>
							m_onAccept = nullptr;
		std::function<void(void)>
							m_onCancel = nullptr;

		std::vector<dusk::Element*>
							m_elements;
		dusk::Element*		m_folderListview;
		dusk::Element*		m_filesListview;

		dusk::Element*		m_filenameField;
		dusk::Element*		m_filetypeField;

		dusk::Element*		m_buttonAccept;
		dusk::Element*		m_buttonCancel;

		dusk::Element*		m_navBackward;
		dusk::Element*		m_navForward;
		dusk::Element*		m_navUp;

		// Current directory browsing to
		std::string			m_currentDirectory = "";

		std::vector<std::string>
							m_fileListing;
		std::vector<std::string>
							m_folderListing;
		void					OnSelectItemInFileview ( const int selection );
		void					OnSelectItemInFolderview ( const int selection );
		void					OnClickItemInFileview ( const int selection );

		std::vector<std::string>
							m_navigationHistory;
		int					m_navigationPosition = 0;

		void					NavigationForward ( void );
		void					NavigationBackward ( void );
		void					NavigationUp ( void );

		//	UpdateDirectoryListing() : Updates the UI's directory listing with the value in m_currentDirectory.
		void					UpdateDirectoryListing ( bool isNavigation = false );
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_DIALOG_SAVE_FILE_H_