#include "MeshBuilder.h"

//	Quad(model, pos, uvs) : Adds a 2D quad to the given modeldata
// Vertices are in counter-clockwise order around the quad edge.
// Ensure md.position, md.texcoord0, and md.indices are initialized.
void core::meshbuilder::Quad ( arModelData* md, const Rect& position, const Rect& uvs )
{
	int current_vertex = md->vertexNum;
	int current_index = md->indexNum;

	md->position[current_vertex + 0].x = position.pos.x;
	md->position[current_vertex + 0].y = position.pos.y;
	md->texcoord0[current_vertex + 0].x = uvs.pos.x;
	md->texcoord0[current_vertex + 0].y = uvs.pos.y;

	md->position[current_vertex + 1].x = position.pos.x;
	md->position[current_vertex + 1].y = position.pos.y + position.size.y;
	md->texcoord0[current_vertex + 1].x = uvs.pos.x;
	md->texcoord0[current_vertex + 1].y = uvs.pos.y + uvs.size.y;

	md->position[current_vertex + 2].x = position.pos.x + position.size.x;
	md->position[current_vertex + 2].y = position.pos.y + position.size.y;
	md->texcoord0[current_vertex + 2].x = uvs.pos.x + uvs.size.x;
	md->texcoord0[current_vertex + 2].y = uvs.pos.y + uvs.size.y;

	md->position[current_vertex + 3].x = position.pos.x + position.size.x;
	md->position[current_vertex + 3].y = position.pos.y;
	md->texcoord0[current_vertex + 3].x = uvs.pos.x + uvs.size.x;
	md->texcoord0[current_vertex + 3].y = uvs.pos.y;

	md->indices[(current_index + 0) * 3 + 0] = current_vertex + 0; 
	md->indices[(current_index + 0) * 3 + 1] = current_vertex + 1; 
	md->indices[(current_index + 0) * 3 + 2] = current_vertex + 2;
	md->indices[(current_index + 1) * 3 + 0] = current_vertex + 0; 
	md->indices[(current_index + 1) * 3 + 1] = current_vertex + 2;
	md->indices[(current_index + 1) * 3 + 2] = current_vertex + 3;

	md->vertexNum += 4;
	md->indexNum += 6;
}

//	Quad(model, pos, color, uvs) : Adds a 3D quad to the given modeldata
// Vertices are in counter-clockwise order around the quad edge.
// Ensure md.position, md.texcoord0, md.color, and md.indices are initialized.
void core::meshbuilder::Quad ( arModelData* md, const Vector3f* positions, const Color& color, const Rect& uvs )
{
	int current_vertex = md->vertexNum;
	int current_index = md->indexNum;

	md->position[current_vertex + 0] = positions[0];
	md->position[current_vertex + 1] = positions[1];
	md->position[current_vertex + 2] = positions[2];
	md->position[current_vertex + 3] = positions[3];

	md->color[current_vertex + 0] = Vector4f(color.raw);
	md->color[current_vertex + 1] = Vector4f(color.raw);
	md->color[current_vertex + 2] = Vector4f(color.raw);
	md->color[current_vertex + 3] = Vector4f(color.raw);

	md->texcoord0[current_vertex + 0] = uvs.pos;
	md->texcoord0[current_vertex + 1] = Vector2f(uvs.pos.x, uvs.pos.y + uvs.size.y);
	md->texcoord0[current_vertex + 2] = uvs.pos + uvs.size;
	md->texcoord0[current_vertex + 3] = Vector2f(uvs.pos.x + uvs.size.x, uvs.pos.y);

	md->indices[(current_index + 0) * 3 + 0] = current_vertex + 0; 
	md->indices[(current_index + 0) * 3 + 1] = current_vertex + 1; 
	md->indices[(current_index + 0) * 3 + 2] = current_vertex + 2;
	md->indices[(current_index + 1) * 3 + 0] = current_vertex + 0; 
	md->indices[(current_index + 1) * 3 + 1] = current_vertex + 2;
	md->indices[(current_index + 1) * 3 + 2] = current_vertex + 3;

	md->vertexNum += 4;
	md->indexNum += 2;
}
