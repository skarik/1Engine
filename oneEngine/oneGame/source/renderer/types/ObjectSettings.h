#ifndef RENDERER_OBJECT_RENDER_SETTINGS_H_
#define RENDERER_OBJECT_RENDER_SETTINGS_H_

// Includes
#include "core/types/types.h"
#include "core/math/Color.h"

class RrMaterial;

//	eRenderMode - current high level pipeline in use.
// Renderer info
enum eRenderMode : int32_t
{
	// Forward renderer mode. Used as part of deferred passes.
	kRenderModeForward		= 0x1,

	// Default renderer mode for 2D.
	kRenderModeDeferred		= 0x2
};

//	eRenderHint - Sublayer bitmask.
// Defines specific layers to render.
enum eRenderHintBitmask : uint32_t
{
	kRenderHintBitmaskALL		= 0xFFFFFFFF,

	kRenderHintBitmaskWorld		= 0x1,
	kRenderHintBitmaskWarp		= 0x2,
	kRenderHintBitmaskGlow		= 0x4,
	kRenderHintBitmaskSkyglow	= 0x8,
	kRenderHintBitmaskFog		= 0x10,
	kRenderHintBitmaskShadowColor = 0x20,
};
//	eRenderHint - Sublayer bitmask.
// Defines specific layers to render.
enum eRenderHintBitmask : uint8_t
{
	kRenderHintCOUNT = 6,
};

namespace renderer
{
	// Structure definition
	struct objectSettings
	{
		uint32_t	renderHints; // 1 for enable, 0 for disable
	};

	// Enumeration for rendering type
	enum eRenderLayer
	{
		kRL_BEGIN		= 0,

		kRLBackground	= 0,
		kRLSecondary	= 1,
		kRLWorld		= 2,
		kRLForeground	= 3,
		kRLV2D			= 4,

		kRL_MAX,
	};
	// Struct for pass info
	struct passinfo_t
	{
		unsigned char pass;
		unsigned char userpass;
	};

	struct new_passinfo_t
	{
		uint8_t		pass;
		uint32_t	vaoObject;
	};

	//	eClearType - buffer clear type
	// Is not used as certain hard-coded clear types in certain cases have better GPU performance.
	enum eClearType : uint8_t
	{
		kClearNone			= 0x00,
		kClearColor			= 0x01,
		kClearDepth			= 0x02,
		kClearDepthAndColor	= 0x01 | 0x02,
	};

	enum eAlphaMode : uint8_t
	{
		ALPHAMODE_NONE		= 0,
		ALPHAMODE_ALPHATEST	= 1,
		ALPHAMODE_TRANSLUCENT=2
	};

	enum eDrawBlendMode : uint8_t
	{
		BM_NORMAL = 0,
		BM_ADD,
		BM_INV_MULTIPLY,
		BM_MULTIPLY,
		BM_MULTIPLY_X2,
		BM_SOFT_ADD,
		BM_NONE
	};
	enum eDrawLightingMode : uint8_t
	{
		LI_NONE = uchar(-1),
		LI_NORMAL = 0,
		LI_SKIN
	};
	enum eDrawFaceMode : uint8_t
	{
		FM_FRONT = 0,
		FM_BACK,
		FM_FRONTANDBACK
	};

	//	ePipelineMode - Current sublevel pipeline in use.
	// These modes have the power to override the current RenderMode
	enum ePipelineMode : uint8_t
	{
		// Default rendering pipeline. Eat a cock.
		kPipelineModeNormal,

		// Pulls information from the attached audio engine.
		// The entirety of the audio list is shoved into the light list.
		// Albedo is dropped. Instead, approximated sound reflections are rendered.
		kPipelineModeEcho,
		
		// Imagination mode. Doesn't actually render.
		kPipelineModeAether,

		// Insprited by Studio SHAFT animation style (particularly Nisemonogatari)
		// Mostly the same as kPipelineModeNormal, however:
		//	* Shadows are fucking dithered
		//	* Sometimes the color palette changes
		//	* Sometimes the scene is flatshaded
		kPipelineModeShaft,

		// Default 2D rendering pipeline, with orthographic optimizations.
		// Requires 2D extension to function properly.
		kPipelineMode2DPaletted 
	};

	struct _n_hint_rendering_information
	{
		RrMaterial*		mats_default		[kRenderHintCOUNT];
		RrMaterial*		mats_default_skin	[kRenderHintCOUNT];
		RrMaterial*		mats_transparent	[kRenderHintCOUNT];
		RrMaterial*		mats_transparent_skin[kRenderHintCOUNT];
		eClearType		clear_type			[kRenderHintCOUNT];
		Color			clear_color			[kRenderHintCOUNT];

		_n_hint_rendering_information ( void );
	}; //m_default_hint_options; // needs to be initialized when CRenderState is created, so to create default replacement materials.
	extern _n_hint_rendering_information* m_default_hint_options;
};


#endif//RENDERER_OBJECT_RENDER_SETTINGS_H_