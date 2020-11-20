#include "Element.h"

#include "../eventide/UserInterface.h"

#include "renderer/utils/rrMeshBuilder.h"
#include "renderer/utils/rrTextBuilder2D.h"
#include "renderer/texture/RrFontTexture.h"

ui::eventide::Element::Element ( UserInterface* ui )
	: m_ui((ui != NULL) ? ui : ui::eventide::UserInterface::Get())
{
	m_ui->AddElement(this);
}

ui::eventide::Element::~Element ( void )
{
	m_ui->RemoveElement(this);
}

void ui::eventide::Element::RequestUpdateMesh ( void )
{
	mesh_creation_state.building_mesh = true;

	mesh_creation_state.index_count = 0;
	mesh_creation_state.vertex_count = 0;

	// begin mesh
	BuildMesh();
	// end mesh

	mesh_creation_state.building_mesh = false;

	// upload mesh
	// TODO: don't update for now
	mesh_creation_state.has_change = true;
}

void ui::eventide::Element::buildCube ( const ParamsForCube& params )
{
	// TODO: need a 3d mesh builder
	rrMeshBuilder meshBuilder (&mesh_creation_state.mesh_data, mesh_creation_state.vertex_count, mesh_creation_state.index_count);
	meshBuilder.enableAttribute(renderer::shader::kVBufferSlotPosition);
	meshBuilder.enableAttribute(renderer::shader::kVBufferSlotColor);
	meshBuilder.enableAttribute(renderer::shader::kVBufferSlotNormal);
	meshBuilder.enableAttribute(renderer::shader::kVBufferSlotUV0);
	meshBuilder.enableAttribute(renderer::shader::kVBufferSlotUV1);

	uint16_t initialVertexCount = mesh_creation_state.vertex_count;

	meshBuilder.addCube(params.box, params.rotation, params.color);

	mesh_creation_state.vertex_count = meshBuilder.getModelDataVertexCount();
	mesh_creation_state.index_count = meshBuilder.getModelDataIndexCount();

	for (uint16_t i = initialVertexCount; i < mesh_creation_state.vertex_count; ++i)
	{
		// apply color
		mesh_creation_state.mesh_data.color[i] = Vector4f(params.color.x, params.color.y, params.color.z, params.color.w);

		// apply texture index, texture strength
		mesh_creation_state.mesh_data.texcoord1[i][(int)VertexElements::kUV1_Slot6_R_TextureEnableBlend] = (params.texture && params.texture->reference) ? kVETextureEnableOn : kVETextureEnableOff;
		mesh_creation_state.mesh_data.texcoord1[i][(int)VertexElements::kUV1_Slot6_G_TextureIndex] = (Real)params.texture->index;
	}
}

void ui::eventide::Element::buildText ( const ParamsForText& params )
{
	ARCORE_ASSERT(params.font_texture != nullptr);

	RrFontTexture* l_font = (RrFontTexture*)params.font_texture->reference;
	rrTextBuilder2D textBuilder (l_font, &mesh_creation_state.mesh_data, mesh_creation_state.vertex_count, mesh_creation_state.index_count);
	textBuilder.setScreenMapping(core::math::Cubic(Vector3f(1, 1, 1), Vector3f(2, 2, 2)));
	textBuilder.enableAttribute(renderer::shader::kVBufferSlotPosition);
	textBuilder.enableAttribute(renderer::shader::kVBufferSlotColor);
	textBuilder.enableAttribute(renderer::shader::kVBufferSlotNormal);
	textBuilder.enableAttribute(renderer::shader::kVBufferSlotUV0);
	textBuilder.enableAttribute(renderer::shader::kVBufferSlotUV1);

	uint16_t initialVertexCount = mesh_creation_state.vertex_count;

	textBuilder.addText(Vector3f(0, 0, 0), params.color, params.string);

	mesh_creation_state.vertex_count = textBuilder.getModelDataVertexCount();
	mesh_creation_state.index_count = textBuilder.getModelDataIndexCount();

	const Real kTextScale = params.size / l_font->GetFontInfo()->height;
	for (uint16_t i = initialVertexCount; i < mesh_creation_state.vertex_count; ++i)
	{
		// first, zero out the Z
		mesh_creation_state.mesh_data.position[i].z = 0.0F;

		// next, apply position, rotation, size
		mesh_creation_state.mesh_data.position[i] *= kTextScale;
		mesh_creation_state.mesh_data.position[i] = params.rotation * mesh_creation_state.mesh_data.position[i];
		mesh_creation_state.mesh_data.position[i] += params.position;

		// apply texture index, texture strength
		mesh_creation_state.mesh_data.texcoord1[i][(int)VertexElements::kUV1_Slot6_R_TextureEnableBlend] = kVETextureEnableOn;
		mesh_creation_state.mesh_data.texcoord1[i][(int)VertexElements::kUV1_Slot6_G_TextureIndex] = (Real)params.font_texture->index;
	}
}