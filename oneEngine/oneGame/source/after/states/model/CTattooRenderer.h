// class CTattooRenderer
//
//	The class that controls the rendering of the character tattoos on the body and sets the material settings properly.
//	Is given access to CMccCharacterModel's private data.
//  CMccCharacterModel uses this class to render all tattoos.
//

#ifndef _C_TATTOO_RENDERER_H_
#define _C_TATTOO_RENDERER_H_

#include "core/types/ModelData.h"
#include "after/states/CRacialStats.h"

class CMccCharacterModel;
class CRenderTexture;

class CTattooRenderer
{
public:
	explicit	CTattooRenderer ( CMccCharacterModel* );
				~CTattooRenderer ( void );


	void		UpdateTattoos ( void );
private:
	void		DrawTattooToTarget ( const bool, CModelData*, CRacialStats::tattoo_t& );

	CMccCharacterModel* model;

	CRenderTexture*		rt_tats_body;
	CRenderTexture*		rt_tats_face;
};

#endif