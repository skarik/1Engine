// The struct controlling 

#ifndef _OBJECT_RENDER_SETTINGS_H_
#define _OBJECT_RENDER_SETTINGS_H_

// Includes
#include "core/types/types.h"
#include "core/math/Color.h"

class glMaterial;

// Renderer info
enum eRenderMode : int32_t
{
	RENDER_MODE_FORWARD		= 0x1,
	RENDER_MODE_DEFERRED	= 0x2
};

// Layer definition
enum eRenderHint : uint32_t
{
	RL_ALL		= 0xFFFFFFFF,

	RL_WORLD	= 0x1,
	RL_WARP		= 0x2,
	RL_GLOW		= 0x4,
	RL_SKYGLOW	= 0x8,
	RL_FOG		= 0x10,

	RL_SHADOW_COLOR = 0x20,


	RL_LAYER_COUNT = 6
};

namespace Renderer
{
	// Structure definition
	struct objectSettings
	{
		uint32_t	renderHints; // 1 for enable, 0 for disable
	};

	// Enumeration for rendering type
	enum RenderingType
	{
		Background=0,
		Secondary,
		World,
		Foreground,
		V2D
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

	enum eClearType : uint16_t
	{
		CLEAR_NONE			= 0,
		CLEAR_COLOR			= 1,
		CLEAR_DEPTH			= 2,
		CLEAR_DEPTH_COLOR	= 3,
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

	namespace Deferred
	{
		enum eDiffuseMethod : uint8_t
		{
			DIFFUSE_DEFAULT,
			
			// Special shaders
			DIFFUSE_TERRAIN,
			DIFFUSE_SKIN, // applies tattoos and shadow darken
			DIFFUSE_EYES,
			DIFFUSE_SKYSPHERE,
			DIFFUSE_CLOUDSPHERE,
			DIFFUSE_TREESYS,
			DIFFUSE_LIGHT_BILLBOARD,
			DIFFUSE_ALPHA_ADDITIVE,
			DIFFUSE_HAIR // slight change w/ alpha and glow
		};

	};

	enum eSpecialModes : uint8_t
	{
		SP_MODE_NORMAL,
		SP_MODE_ECHO,
		SP_MODE_AETHER,
		SP_MODE_SHAFT
	};

	struct _n_hint_rendering_information
	{
		glMaterial*		mats_default		[RL_LAYER_COUNT];
		glMaterial*		mats_default_skin	[RL_LAYER_COUNT];
		glMaterial*		mats_transparent	[RL_LAYER_COUNT];
		glMaterial*		mats_transparent_skin[RL_LAYER_COUNT];
		eClearType		clear_type			[RL_LAYER_COUNT];
		Color			clear_color			[RL_LAYER_COUNT];

		_n_hint_rendering_information ( void );
	}; //m_default_hint_options; // needs to be initialized when CRenderState is created, so to create default replacement materials.
	extern _n_hint_rendering_information* m_default_hint_options;
};


#endif