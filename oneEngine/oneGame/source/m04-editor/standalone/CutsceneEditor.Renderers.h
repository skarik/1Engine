#ifndef M04_EDITOR_CUTSCENE_EDITOR_RENDERERS_H_
#define M04_EDITOR_CUTSCENE_EDITOR_RENDERERS_H_

#include "CutsceneEditor.h"

#include "renderer/object/CRenderable3D.h"
#include "renderer/object/CStreamedRenderable3D.h"

//=========================================//
// Prototypes
//=========================================//

class CTexture;
class CBitmapFont;

//=========================================//
// Class definitions
//=========================================//

namespace M04
{
	class CutsceneEditor::CLargeTextRenderer : public CStreamedRenderable3D
	{
	private:
		CutsceneEditor*		m_owner;
		CBitmapFont*		m_font_texture;

	public:
		explicit	CLargeTextRenderer ( CutsceneEditor* owner );
					~CLargeTextRenderer ( void );

		void		UpdateMesh ( void );
	};

	class CutsceneEditor::CNormalTextRenderer : public CStreamedRenderable3D
	{
	private:
		CutsceneEditor*		m_owner;
		CBitmapFont*		m_font_texture;

	public:
		explicit	CNormalTextRenderer ( CutsceneEditor* owner );
					~CNormalTextRenderer ( void );

		void		UpdateMesh ( void );
	};

	class CutsceneEditor::CGeometryRenderer : public CStreamedRenderable3D
	{
	private:
		CutsceneEditor*		m_owner;
		CTexture*			m_texture;

	public:
		explicit	CGeometryRenderer ( CutsceneEditor* owner );
					~CGeometryRenderer ( void );

		void		UpdateMesh ( void );
	};
}

#endif//M04_EDITOR_CUTSCENE_EDITOR_RENDERERS_H_