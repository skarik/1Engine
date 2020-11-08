#ifndef UI_EVENTIDE_ELEMENT_H_
#define UI_EVENTIDE_ELEMENT_H_

#include "../eventide/Common.h"

#include "core/math/Color.h"
#include "core/math/Cubic.h"
#include "core/types/ModelData.h"

namespace ui {
namespace eventide {

	class UserInterface;
	struct Texture;

	class Element
	{
	public:

		enum class MouseInteract
		{
			// The mouse can pass through this element
			kNone = 0,
			// The mouse is blocked by the element, but the element does not capture clicks.
			kBlocking = 1,
			// The mouse is blocked by the element, and captures mouse events.
			kCapturing = 2,
		};

	public:
		EVENTIDE_API			Element ( UserInterface* ui = NULL );
		EVENTIDE_API			~Element ( void );

		//	virtual BuildMesh() : Called to build the mesh that is actually drawn.
		//	The build***() functions are used for building within this function.
		EVENTIDE_API virtual void
								BuildMesh ( void ) =0;

	protected:
		// Stores current state of mesh creation
		struct MeshCreationState
		{
			bool				building_mesh = false;

			arModelData			mesh_data;
			// Actual number of indicies to draw
			uint32_t			index_count = 0;
			// Actual number of vertices to draw
			uint32_t			vertex_count = 0;

			// todo: for now we just have one big mesh for the entire UI. so we need to store the current mesh on CPU side
			bool				has_change = false; // For now we use this to check if we need to regen the UI's mesh
		};

		enum class VertexElements
		{
			kUV1_Slot6_R_TextureEnableBlend = 0,
			kUV1_Slot6_G_TextureIndex = 1,
			kUV1_Slot6_B_TextureIndex = 2,
		};

		// Current state of mesh creation
		MeshCreationState		mesh_creation_state;

		struct ParamsForCube
		{
			core::math::Cubic	box			= core::math::Cubic(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
			Rotator				rotation	= Rotator();
			Color				color		= Color(1.0F, 1.0F, 1.0F, 1.0F);
			Texture*			texture		= nullptr;
		};
		//	buildCube( params ) : Adds a cube to the build.
		EVENTIDE_API void		buildCube ( const ParamsForCube& params );

		struct ParamsForText
		{
			const char*			string		= nullptr;
			Real				size		= 12.0F;
			Color				color		= Color(1.0F, 1.0F, 1.0F, 1.0F);
			Vector3f			position	= Vector3f(0, 0, 0);
			Rotator				rotation	= Rotator();
			Texture*			font_texture= nullptr;
		};
		//	buildText( params ) : Adds text to the build.
		EVENTIDE_API void		buildText ( const ParamsForText& params );

	public:
		//	RequestUpdateMesh() : Requests an update of the mesh. Calls BuildMesh to recreate.
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
			};

			Type				type = Type::kInvalid;
			Vector3f			position_world;
			Vector2f			position_screen;
			Vector2f			velocity_screen;
		};

		//	virtual OnEventMouse() : Called when mouse events occur with this element.
		// The values of m_mouseInside are updated before this is executed by the UI manager.
		// Override to handle mouse-control events.
		EVENTIDE_API virtual void
								OnEventMouse ( const EventMouse& mouse_event )
			{}

		// manager->LoadTexture
		// manager->ReleaseTexture

		EVENTIDE_API const core::math::BoundingBox&
								GetBBox ( void ) const
			{ return m_bbox; }

		EVENTIDE_API void		SetParent ( Element* parent ) const;

		EVENTIDE_API const Element*
								GetParent ( void ) const
			{ return m_parent; }

	protected:
		core::math::BoundingBox	m_bbox;
		UserInterface*			m_ui = NULL;

		// Parent element to float position on
		Element*				m_parent = NULL;
		
		// How does the mouse interact with this element?
		MouseInteract			m_mouseInteract = MouseInteract::kNone;

		// Is the mouse inside this element? Updated by the UI manager
		bool					m_mouseInside = false;
		// Are we locking the mouse to this element for now?
		bool					m_mouseLocked = false;
	};

}}

#endif//UI_EVENTIDE_ELEMENT_H_