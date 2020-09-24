#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_DIALOG_LOAD_FILE_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_DIALOG_LOAD_FILE_H_

#include "engine-common/dusk/dialogs/FileViewer.h"
#include "engine-common/dusk/dialogs/SaveFile.h"

namespace dusk {
namespace dialogs {

	class LoadFile : public dusk::dialogs::SaveFile
	{
	public:
		ENGCOM_API explicit		LoadFile()
			: dusk::dialogs::SaveFile()
			{}

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_DIALOG_LOAD_FILE_H_