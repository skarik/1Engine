//===============================================================================================//
//
//		Dusk UI : class UIRenderer
//
// UIRenderer is the main renderer class. It's instantiated internally by UserInterface.
// All style options are configured through the UIRenderer object.
//
//===============================================================================================//
#ifndef ENGINE_COMMON_DUSK_UI_RENDERER_H_
#define ENGINE_COMMON_DUSK_UI_RENDERER_H_

#include "renderer/object/CRenderableObject.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"

class RrFontTexture;
class rrMeshBuffer;
class rrMeshBuilder2D;
namespace gpu
{
	class RenderTarget;
	class Texture;
}

namespace dusk
{
	class UserInterface;
	class Element;

	// Color state for a single element
	struct UIRendererBlendedColor
	{
		Color					getCurrentColor ( void )
			{ return color; }

		//	setColor() : Sets next color to blend to
		void					setColor ( const Color& next_color, const float blend_time );
		//	setPulse() : Sets start of animated pulse
		void					setPulse ( const Color& in_pulse_color );

		//	update() : Update the current color
		void					update ( void );

	public:
		Color				color;

		Color				main_color_prev;
		Color				main_color_next;
		float				main_color_blend = 0.0F; 
		float				main_color_blend_time = 0.0F; 

		Color				pulse_color;
		bool				pulse_up = false;
		float				pulse_blend = 0.0F;
	};

	// State of all element colors
	class UIRendererElementColors
	{
	public:
		void					setSize ( const size_t size );
		void					update ( void );

		void					setBackgroundColor ( Element* element, const Color& color );
		void					setBackgroundClickPulse ( Element* element, const Color& click_pulse );

		Color					getBackgroundColor ( Element* element );

	private:

		std::vector<UIRendererBlendedColor>
							m_elementBackgroundColors;
	};

	// Renders UI
	class UIRenderer : public CRenderableObject
	{
	public:
		explicit				UIRenderer ( UserInterface* ui );
								~UIRenderer ( void );

		//	PreRender() : Called before the internal render-loop executes.
		// Can be called multiple times per frame, but generally only once per camera.
		// Use to calculate transformation matrices w/ the given camera before sending to the GPU.
		ENGCOM_API bool			PreRender ( rrCameraPass* cameraPass ) override;
		//	Render(const rrRenderParams* params) : Current pass
		ENGCOM_API bool			Render ( const rrRenderParams* params ) override;

		//	BeginRender() : Called before the render-loop executes.
		// Called once per frame.
		ENGCOM_API bool			BeginRender ( void ) override;
		//	EndRender() : Called after the render-loop executes.
		// Called once per frame.
		ENGCOM_API bool			EndRender ( void ) override;

	private:

		//	ERUpdateRenderList() : Updates the render list.
		void					ERUpdateRenderList ( std::vector<Element*>* renderList );

		//	ERUpdateRenderTarget() : Updates & resizes render target.
		// Returns true when there has been a change to the render target.
		bool					ERUpdateRenderTarget ( void );

		//	ERRenderElements() : Renders the elements to buffer.
		void					ERRenderElements ( const std::vector<Element*>& renderList, const Rect& scissorArea );

	public:

		bool				m_renderMouselessFocus = false;

	private:

		// Rendering state
		gpu::RenderTarget*	m_renderTarget;
		gpu::Texture*		m_renderTargetTexture;
		Vector2i			m_renderTargetSize;
		gpu::Buffer*		m_vbufScreenQuad;

		UserInterface*		m_interface;
		RrFontTexture*		m_fontTexture;

		arModelData			m_modeldata;
		rrMeshBuffer		m_meshBuffer;

		UIRendererElementColors
							m_colors;

	};

	enum ColorStyle : uint8_t
	{
		kColorStyleBackground	= 0x01,
		kColorStyleElement		= 0x02,
		kColorStyleLabel		= 0x03,
	};

	enum FocusStyle : uint16_t
	{
		// Automatic style, pulls from the calling element properties
		kFocusStyleAutomatic	= 0x00,
		// Automatic style, but no glow on hover, pulls from calling element properties
		kFocusStyleAutomaticNoHover	= 0x04,
		// Active style, default look
		kFocusStyleActive		= 0x10,
		// Disabled style, ensure the element cannot be used
		kFocusStyleDisabled		= 0x20,
		// Focused style, has been tabbed to. Make the element have a glowing outline.
		kFocusStyleFocused		= 0x30,
		// Hovered style, mouse is over. Makes the element itself slightly glow.
		kFocusStyleHovered		= 0x40,

		// Special style, for notice elements. Element pulses and glows.
		kFocusStyleNotice		= 0x100,
	};

	enum TextAlignStyleHorizontal
	{
		kTextAlignLeft,
		kTextAlignCenter,
		kTextAlignRight,
	};
	enum TextAlignStyleVertical
	{
		kTextAlignTop,
		kTextAlignMiddle,
		kTextAlignBottom,
	};

	enum TextFontStyle
	{
		kTextFontTitle,
		kTextFontButton,
		kTextFontParagraphs,
	};

	struct TextStyleSettings
	{
		TextFontStyle				font;
		TextAlignStyleHorizontal	align_horizontal;
		TextAlignStyleVertical		align_vertical;
	};

	class UIRendererContext
	{
	private:
		friend UIRenderer;

	public:
		ENGCOM_API void			setFocus ( FocusStyle style );
		ENGCOM_API void			setColor ( ColorStyle style );
		ENGCOM_API void			setTextSettings ( TextStyleSettings settings );

		ENGCOM_API float		getTextHeight ( TextFontStyle font );

		ENGCOM_API void			drawRectangle ( Element* source, const Rect& rectangle );
		ENGCOM_API void			drawText ( Element* source, const Vector2f& position, const char* str );

	private:
		
		void					generateColor( Element* source );
		void					generateDrawState( Element* source );

	private:
		FocusStyle			m_focusType;
		ColorStyle			m_colorType;
		TextStyleSettings	m_textType;

		bool				m_dsDrawBackground;
		Color				m_dsColorBackground;
		bool				m_dsDrawOutline;
		Color				m_dsColorOutline;
		bool				m_dsDrawGlow;
		Color				m_dsColorGlow;

		UIRenderer*			m_uir;
		rrMeshBuilder2D*	m_mb2;
		arModelData*		m_modeldata;
		UIRendererElementColors*
							m_colors;
	};
}

#endif//ENGINE_COMMON_DUSK_UI_RENDERER_H_