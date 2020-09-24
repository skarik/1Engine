#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_DIALOG_FILE_VIEWER_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_DIALOG_FILE_VIEWER_H_

#include "engine-common/dusk/Element.h"

namespace dusk {
namespace dialogs {

	class FileViewer : public dusk::DialogElement
	{
	public:
		ENGCOM_API explicit		FileViewer()
			: dusk::DialogElement()
			{}

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_DIALOG_FILE_VIEWER_H_