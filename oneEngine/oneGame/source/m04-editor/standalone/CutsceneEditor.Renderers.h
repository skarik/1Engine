#ifndef M04_EDITOR_CUTSCENE_EDITOR_RENDERERS_H_
#define M04_EDITOR_CUTSCENE_EDITOR_RENDERERS_H_

#include "CutsceneEditor.h"

#include "renderer/object/CRenderable3D.h"
#include "renderer/object/CStreamedRenderable3D.h"
#include "renderer/logic/RrLogicObject.h"

//=========================================//
// Prototypes
//=========================================//

class RrTexture;
class RrFontTexture;

//=========================================//
// Class definitions
//=========================================//

namespace M04
{
	class CutsceneEditor::CLargeTextRenderer : public CStreamedRenderable3D
	{
	private:
		CutsceneEditor*		m_owner;
		RrFontTexture*		m_font_texture;

	public:
		explicit	CLargeTextRenderer ( CutsceneEditor* owner );
					~CLargeTextRenderer ( void );

		void		UpdateMesh ( void );
	};

	class CutsceneEditor::CNormalTextRenderer : public CStreamedRenderable3D
	{
	private:
		CutsceneEditor*		m_owner;
		RrFontTexture*		m_font_texture;

	public:
		explicit	CNormalTextRenderer ( CutsceneEditor* owner );
					~CNormalTextRenderer ( void );

		void		UpdateMesh ( void );
	};

	class CutsceneEditor::CGeometryRenderer : public CStreamedRenderable3D
	{
	private:
		CutsceneEditor*		m_owner;
		//RrTexture*			m_texture;

	public:
		explicit	CGeometryRenderer ( CutsceneEditor* owner );
					~CGeometryRenderer ( void );

		void		UpdateMesh ( void );
	};

	class CutsceneEditor::CGeometryUpdater : public RrLogicObject
	{
	private:
		CutsceneEditor*		m_owner;

	public:
		explicit	CGeometryUpdater ( CutsceneEditor* owner );
		void		PostStep ( void ) override;
	};
}

#endif//M04_EDITOR_CUTSCENE_EDITOR_RENDERERS_H_