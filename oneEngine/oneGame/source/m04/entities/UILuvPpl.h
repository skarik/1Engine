#ifndef _M04_UI_LUV_PPL_H_
#define _M04_UI_LUV_PPL_H_

#include "engine/behavior/CGameBehavior.h"
#include "render2d/object/sprite/CStreamedRenderable2D.h"

class CTextMesh;

namespace M04
{

	class UILuvPpl : public CGameBehavior, public CStreamedRenderable2D
	{
	public:
		explicit		UILuvPpl ( void );
		~UILuvPpl ( void );

		void			Update ( void ) override;

		
	protected:
		struct ui_heartstate_t
		{
			int current;
			int target;
			Real animation_lerp;
		};
	};
}

#endif//_M04_UI_LUV_PPL_H_