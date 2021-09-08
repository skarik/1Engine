#ifndef RENDERER_OBJECT_RENDER_SETTINGS_H_
#define RENDERER_OBJECT_RENDER_SETTINGS_H_

// Includes
#include "core/types/types.h"
#include "core/math/Color.h"

//	eRenderMode - current high level pipeline in use.
// Renderer info
enum eRenderMode : int32_t
{
	// Forward renderer mode. Used as part of deferred passes.
	kRenderModeForward		= 0x1,

	// Default renderer mode for 2D.
	kRenderModeDeferred		= 0x2
};

//	eRenderHintBitmask - Sublayer bitmask.
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
enum eRenderHint : uint8_t
{
	kRenderHintCOUNT = 6,
};

namespace renderer
{
	// Structure definition
	/*struct objectSettings
	{
		uint32_t	renderHints; // 1 for enable, 0 for disable
	};*/

	// Enumeration for rendering type
	enum rrRenderLayer
	{
		kRenderLayer_BEGIN		= 0,

		// Layer used for skyboxes. Has depth buffer available.
		kRenderLayerBackground	= 0,
		//kRenderLayerSecondary	= 1,
		// Layer used for the main world. Has depth buffer available.
		kRenderLayerWorld		= 1,
		//kRenderLayerForeground	= 3,
		// Layer used for 2D UI.
		kRenderLayerV2D			= 2,

		kRenderLayer_MAX,

		// Skipped layer, will not appear in the normal rendering loop.
		// Used for storing pipelines for off-screen rendering that must occur out of the main loop.
		kRenderLayerSkip		= 0xFF,
	};

	//	eClearType - buffer clear type
	// Is not used as certain hard-coded clear types in certain cases have better GPU performance.
	enum rrClearType : uint8_t
	{
		kClearNone			= 0x00,
		kClearColor			= 0x01,
		kClearDepth			= 0x02,
		kClearDepthAndColor	= 0x01 | 0x02,
	};

	enum rrAlphaMode : uint8_t
	{
		kAlphaModeNone			= 0x00,
		kAlphaModeAlphatest		= 0x01,
		kAlphaModeTranslucent	= 0x02,
	};

	enum rrHLBlendMode : uint8_t
	{
		kHLBlendModeNone		= 0xFF,
		kHLBlendModeNormal		= 0x00,
		kHLBlendModeAdd,
		kHLBlendModeInvMultiply,
		kHLBlendModeMultiply,
		kHLBlendModeMultiplyX2,
		kHLBlendModeSoftAdd,
	};
	enum rrHLLightMode : uint8_t
	{
		kHLLightModeNone	= 0xFF,
		kHLLightModeNormal	= 0x00,
	};
	enum rrCullMode : uint8_t
	{
		kCullNone			= 0x00,
		kCullBackface		= 0x01,
		kCullFrontface		= 0x02,
	};
};


#endif//RENDERER_OBJECT_RENDER_SETTINGS_H_