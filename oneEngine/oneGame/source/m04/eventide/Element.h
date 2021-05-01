#ifndef UI_EVENTIDE_ELEMENT_H_
#define UI_EVENTIDE_ELEMENT_H_

#include <atomic>

#include "../eventide/Common.h"

#include "core/math/Color.h"
#include "core/math/Rect.h"
#include "core/math/Cubic.h"
#include "core/types/ModelData.h"

namespace ui {
namespace eventide {

	class UserInterface;
	struct Texture;

	class Element
	{
	public:
		friend UserInterface; // User interface updates various mouse & focus states.

		enum class FrameUpdate
		{
			// The element does not have a frame-update behavior
			kNone = 0,
			// The element does have frame-update behavior
			kPerFrame = 1,
		};

		enum class MouseInteract
		{
			// The mouse can pass through this element
			kNone = 0,
			// The mouse is blocked by the element, but the element does not capture clicks.
			kBlocking = 1,
			// The mouse is blocked by the element, and captures mouse events.
			kCapturing = 2,
			// The can pass through the element. However, it will also receive all events.
			kCatchAll = 3,
			// The mouse is blocked by the element, and captures mouse events. It will also receive events when not focused.
			kCapturingCatchAll = 4,
		};

		enum class FocusInteract
		{
			// The element can not be focus-cycled, nor focused.
			kNone = 0,
			// The element can be focus-cycled and focused.
			kFocusable = 1,
			// The element can not be focus-cycled, but can be focused in other means.
			kFocusableNoCycle = 2,
		};

		struct InputInteractMasks 
		{
			enum Type : uint32_t
			{
				kNone = 0,
				// Enter is forwarded as an Activate event.
				kActivateEnter = 0x01,
				// Space is forwarded as an Activate event.
				kActivateSpace = 0x02,
				// Enter and Space is forwarded as an Activate event.
				kActivateButton = kActivateEnter | kActivateSpace,

				// Allows keystrokes to be forwarded to the element
				kKeyboardPress = 0x04,

				// Does this get inputs even when not focused
				kCatchAll = 0xF000,
			};
		};

	public:
		EVENTIDE_API			Element ( UserInterface* ui = NULL );

	protected:
		EVENTIDE_API virtual	~Element ( void );

	public:
		//	Destroy() : Requests this element to be destroyed. Is added to the ignore status.
		EVENTIDE_API void		Destroy ( void );

		//	virtual BuildMesh() : Called to build the mesh that is actually drawn.
		//	The build***() functions are used for building within this function.
		EVENTIDE_API virtual void
								BuildMesh ( void ) =0;

	protected:
		// Stores current state of mesh creation
		struct MeshCreationState
		{
			// Has a rebuild been requested
			std::atomic_bool	rebuild_requested = false;

			// Is the mesh currently being put together?
			std::atomic_bool	building_mesh = false;

			arModelData			mesh_data;
			// Actual number of indicies to draw
			uint32_t			index_count = 0;
			// Actual number of vertices to draw
			uint32_t			vertex_count = 0;

			// todo: for now we just have one big mesh for the entire UI. so we need to store the current mesh on CPU side
			std::atomic_bool	has_change = false; // For now we use this to check if we need to regen the UI's mesh
		};

		enum class VertexElements
		{
			kUV1_Slot6_R_TextureEnableBlend = 0,
			kUV1_Slot6_G_TextureIndex = 1,
			kUV1_Slot6_B_AlphaCutoff = 2,
		};
		static constexpr float	kVETextureEnableOff = 0.0F;
		static constexpr float	kVETextureEnableOn = 1.0F;

	private:
		// Current state of mesh creation
		MeshCreationState		mesh_creation_state;

		//	RebuildMesh() : Rebuilds the mesh data.
		EVENTIDE_API void		RebuildMesh ( void );

	protected:

		struct ParamsForCube
		{
			core::math::Cubic	box			= core::math::Cubic(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
			Rotator				rotation	= Rotator();
			Color				color		= Color(1.0F, 1.0F, 1.0F, 1.0F);
			Texture*			texture		= nullptr;
			bool				wireframe	= false;
		};
		//	buildCube( params ) : Adds a cube to the build.
		EVENTIDE_API void		buildCube ( const ParamsForCube& params );

		enum class AlignHorizontal
		{
			kLeft,
			kCenter,
			kRight,
		};

		struct ParamsForText
		{
			const char*			string		= nullptr;
			Real				size		= 12.0F;
			AlignHorizontal		alignment	= AlignHorizontal::kLeft;
			Color				color		= Color(1.0F, 1.0F, 1.0F, 1.0F);
			Vector3f			position	= Vector3f(0, 0, 0);
			Rotator				rotation	= Rotator();
			Texture*			font_texture= nullptr;
		};
		//	buildText( params ) : Adds text to the build.
		EVENTIDE_API void		buildText ( const ParamsForText& params );
		//	predictText( params ) : Adds text to the build.
		EVENTIDE_API Vector2f	predictText ( const ParamsForText& params );

		struct ParamsForQuad
		{
			Vector3f			position	= Vector3f(0, 0, 0);
			Vector2f			size		= Vector2f(10, 10); // Halfsize
			Rotator				rotation	= Rotator();
			Color				color		= Color(1.0F, 1.0F, 1.0F, 1.0F);
			Texture*			texture		= nullptr;
			Rect				uvs			= Rect(0, 0, 1, 1);
			bool				wireframe	= false;
		};
		//	buildQuad( params ) : Adds a quad to the build.
		EVENTIDE_API void		buildQuad ( const ParamsForQuad& params );

		struct ParamsForPath
		{
			uint32_t			pointCount	= 0;
			Vector3f*			points		= nullptr;
			Real*				widths		= nullptr;
			bool				width_solo	= false;
			Color*				colors		= nullptr;
			bool				color_solo	= false;
			Texture*			texture		= nullptr;
			Rect				uvs			= Rect(0, 0, 1, 1);
			bool				wireframe	= false;
		};
		//	buildPath( params ) : Adds a trianglestrip path to the build.
		EVENTIDE_API void		buildPath ( const ParamsForPath& params );

	public:
		// Class for external access to an element's rendering context. Use with care.
		// Should only be used within BuildMesh implementations.
		class RenderContext
		{
		public:
			EVENTIDE_API explicit	RenderContext ( Element* element )
				: m_element(element)
				{}

			typedef ui::eventide::Element::ParamsForCube ParamsForCube;

			//	buildCube( params ) : Adds a cube to the build.
			EVENTIDE_API void		buildCube ( const ParamsForCube& params )
			{
				m_element->buildCube(params);
			}

			typedef ui::eventide::Element::AlignHorizontal AlignHorizontal;
			typedef ui::eventide::Element::ParamsForText ParamsForText;

			//	buildText( params ) : Adds text to the build.
			EVENTIDE_API void		buildText ( const ParamsForText& params )
			{
				m_element->buildText(params);
			}
			//	predictText( params ) : Adds text to the build.
			EVENTIDE_API Vector2f	predictText ( const ParamsForText& params )
			{
				return m_element->predictText(params);
			}

			typedef ui::eventide::Element::ParamsForQuad ParamsForQuad;

			//	buildQuad( params ) : Adds a quad to the build.
			EVENTIDE_API void		buildQuad ( const ParamsForQuad& params )
			{
				m_element->buildQuad(params);
			}

			typedef ui::eventide::Element::ParamsForPath ParamsForPath;

			//	buildPath( params ) : Adds a trianglestrip path to the build.
			EVENTIDE_API void		buildPath ( const ParamsForPath& params )
			{
				m_element->buildPath(params);
			}

		private:
			Element*			m_element;
		};

	public:
		//	RequestUpdateMesh() : Requests an update of the mesh.
		// The request eventually BuildMesh to recreate.
		EVENTIDE_API void		RequestUpdateMesh ( void );

		struct EventMouse
		{
			enum class Type
			{
				kInvalid,
				kEnter,
				kExit,
				kClicked,
				kDragged,
				kReleased,
			};

			Type				type = Type::kInvalid;
			uint8_t				button = 0;
			Vector3f			position_world;
			Vector2f			position_screen;
			Vector2f			velocity_world;
			Vector2f			velocity_screen;
			Element*			element = nullptr;
		};

		//	virtual OnEventMouse() : Called when mouse events occur with this element.
		// The values of m_mouseInside are updated before this is executed by the UI manager.
		// Override to handle mouse-control events.
		EVENTIDE_API virtual void
								OnEventMouse ( const EventMouse& mouse_event )
			{}

		struct EventInput
		{
			enum class Type
			{
				kInvalid,
				kActivate,
			};

			Type				type = Type::kInvalid;
		};

		//	virtual OnEventInput() : Called when the given input occurs.
		EVENTIDE_API virtual void
								OnEventInput ( const EventInput& input_event )
			{}

		struct GameFrameUpdateInput
		{
			enum class Type
			{
				kGameThread,
			};

			Type				type = Type::kGameThread;
		};

		//	virtual OnGameFrameUpdate() : Called when frame update is enabled.
		// Called on the game thread. BuildMesh can be happening concurrently, so do not rely on mesh data.
		EVENTIDE_API virtual void
								OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame )
			{}

	protected:

		//	LoadTexture() : Loads a texture, using ui->LoadTexture().
		EVENTIDE_API ui::eventide::Texture
								LoadTexture ( const char* filename );

		//	LoadTextureFont() : Loads a font, using ui->LoadTextureFont().
		EVENTIDE_API ui::eventide::Texture
								LoadTextureFont ( const char* filename );

		//	ReleaseTexture() : Frees a texture, using ui->ReleaseTexture().
		EVENTIDE_API void		ReleaseTexture ( const ui::eventide::Texture& texture );

	public:
		EVENTIDE_API const core::math::BoundingBox&
								GetBBox ( void ) const
			{ return m_bbox; }
		EVENTIDE_API const core::math::BoundingBox&
								GetBBoxAbsolute ( void ) const
			{ return m_bboxAbsolute; }

		//	SetBBox(bbox) : Sets the local-space bbox, in relation to the parent.
		EVENTIDE_API void		SetBBox ( core::math::BoundingBox& bbox )
		{
			m_bbox = bbox;
			m_bboxDirty = true;
		}
		//	SetParent(parent) : Sets the parent of the element.
		EVENTIDE_API void		SetParent ( Element* parent );

		EVENTIDE_API const Element*
								GetParent ( void ) const
			{ return m_parent; }

		EVENTIDE_API const FrameUpdate
								GetFrameUpdate ( void ) const
			{ return m_frameUpdate; }
		EVENTIDE_API const MouseInteract
								GetMouseInteract ( void ) const
			{ return m_mouseInteract; }
		EVENTIDE_API const FocusInteract
								GetFocusInteract ( void ) const
			{ return m_focusInteract; }
		EVENTIDE_API const uint32_t
								GetInputInteractMask ( void ) const
			{ return m_inputInteractMask; }

		EVENTIDE_API virtual const bool
								GetMouseInside ( void ) const
			{ return m_mouseInside; }
		EVENTIDE_API const bool	GetFocused ( void ) const
			{ return m_focused; }

		EVENTIDE_API UserInterface*
								GetUserInterface ( void ) const
			{ return m_ui; }

	protected:
		// Local-space bounding box of the element. Used for mouse collision.
		// The offset is calculated against center of the bounding box of the parent.
		core::math::BoundingBox
							m_bbox;

	private:
		// Does the bbox need a full update?
		bool				m_bboxDirty = false;
		// "World"-space bounding box of the element.
		core::math::BoundingBox
							m_bboxAbsolute;

	protected:
		// UI this element is associated with.
		UserInterface*		m_ui = NULL;

		// Parent element to float position on
		Element*			m_parent = NULL;
		
		// How does per-frame updates work with this element?
		FrameUpdate			m_frameUpdate = FrameUpdate::kNone;
		// How does the mouse interact with this element?
		MouseInteract		m_mouseInteract = MouseInteract::kNone;
		// How does the tab & inputs focus work with this element?
		FocusInteract		m_focusInteract = FocusInteract::kNone;
		// How does the various inputs work with this element?
		uint32_t			m_inputInteractMask = InputInteractMasks::kNone;

	private:
		// Is the mouse inside this element? Updated by the UI manager
		bool				m_mouseInside = false;

	protected:
		// Are we locking the mouse to this element for now?
		bool				m_mouseLocked = false;

	private:
		// Is this element focused? Updated by the UI manager
		bool				m_focused = false;
	};

	class ScopedCriticalGameThread
	{
	public:
		EVENTIDE_API explicit	ScopedCriticalGameThread ( Element* element );
		EVENTIDE_API			~ScopedCriticalGameThread ( void );
	private:
		Element*			calling_element;
	};
}}

#endif//UI_EVENTIDE_ELEMENT_H_