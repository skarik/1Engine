#ifndef RENDERER_C_LOAD_SCREEN_INJECTOR_H_
#define RENDERER_C_LOAD_SCREEN_INJECTOR_H_

#include "renderer/object/RrRenderObject.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"

class RrFontTexture;
class IrrMeshBuilder;

class CLoadScreenInjector : public RrRenderObject
{
public:
	RENDER_API explicit		CLoadScreenInjector ( void );
	RENDER_API				~CLoadScreenInjector ( void );

	//	StepScreen() : Displays the loading screen
	RENDER_API void			StepScreen ( void );
	//	setAlpha( alpha ) : Updates the alpha for the loading screen
	// Used for fading the screen out smoothly.
	RENDER_API void			setAlpha ( Real new_alpha );

	//	BeginRender() : Create the mesh & stream to the GPU
	bool					BeginRender ( void ) override;
	//	PreRender() : Prepare for the camera
	bool					PreRender ( rrCameraPass* pass ) override;
	//	Render() : Rneder the created mesh
	bool					Render ( const rrRenderParams* params ) override;
private:
	RrFontTexture*		m_fntNotifier;
	Real				m_currentAlpha;
	IrrMeshBuilder*		m_meshBuilder;
	arModelData			m_mesh;
	rrMeshBuffer		m_meshBuffer;
};


#endif//RENDERER_C_LOAD_SCREEN_INJECTOR_H_