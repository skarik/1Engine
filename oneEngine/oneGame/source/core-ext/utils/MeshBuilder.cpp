
#include "MeshBuilder.h"

void core::meshbuilder::Quad ( arModelData* md, const Rect& position, const Rect& uvs )
{
	int current_vertex = md->vertexNum;
	int current_triangle = md->triangleNum;

	md->vertices[current_vertex + 0].x = position.pos.x;
	md->vertices[current_vertex + 0].y = position.pos.y;
	md->vertices[current_vertex + 0].u = uvs.pos.x;
	md->vertices[current_vertex + 0].v = uvs.pos.y;

	md->vertices[current_vertex + 1].x = position.pos.x;
	md->vertices[current_vertex + 1].y = position.pos.y + position.size.y;
	md->vertices[current_vertex + 1].u = uvs.pos.x;
	md->vertices[current_vertex + 1].v = uvs.pos.y + uvs.size.y;

	md->vertices[current_vertex + 2].x = position.pos.x + position.size.x;
	md->vertices[current_vertex + 2].y = position.pos.y + position.size.y;
	md->vertices[current_vertex + 2].u = uvs.pos.x + uvs.size.x;
	md->vertices[current_vertex + 2].v = uvs.pos.y + uvs.size.y;

	md->vertices[current_vertex + 3].x = position.pos.x + position.size.x;
	md->vertices[current_vertex + 3].y = position.pos.y;
	md->vertices[current_vertex + 3].u = uvs.pos.x + uvs.size.x;
	md->vertices[current_vertex + 3].v = uvs.pos.y;

	md->triangles[current_triangle + 0].vert[0] = current_vertex + 0; 
	md->triangles[current_triangle + 0].vert[1] = current_vertex + 1; 
	md->triangles[current_triangle + 0].vert[2] = current_vertex + 2;
	md->triangles[current_triangle + 1].vert[0] = current_vertex + 0; 
	md->triangles[current_triangle + 1].vert[1] = current_vertex + 2;
	md->triangles[current_triangle + 1].vert[2] = current_vertex + 3;

	md->vertexNum += 4;
	md->triangleNum += 2;
}

void core::meshbuilder::Quad ( arModelData* md, const Vector3f* positions, const Color& color, const Rect& uvs )
{
	int current_vertex = md->vertexNum;
	int current_triangle = md->triangleNum;

	md->vertices[current_vertex + 0].position = positions[0];
	md->vertices[current_vertex + 1].position = positions[1];
	md->vertices[current_vertex + 2].position = positions[2];
	md->vertices[current_vertex + 3].position = positions[3];

	md->vertices[current_vertex + 0].color = Vector4f(color.raw);
	md->vertices[current_vertex + 1].color = Vector4f(color.raw);
	md->vertices[current_vertex + 2].color = Vector4f(color.raw);
	md->vertices[current_vertex + 3].color = Vector4f(color.raw);

	md->vertices[current_vertex + 0].texcoord0 = uvs.pos;
	md->vertices[current_vertex + 1].texcoord0 = Vector2f(uvs.pos.x, uvs.pos.y + uvs.size.y);
	md->vertices[current_vertex + 2].texcoord0 = uvs.pos + uvs.size;
	md->vertices[current_vertex + 3].texcoord0 = Vector2f(uvs.pos.x + uvs.size.x, uvs.pos.y);

	md->triangles[current_triangle + 0].vert[0] = current_vertex + 0; 
	md->triangles[current_triangle + 0].vert[1] = current_vertex + 1; 
	md->triangles[current_triangle + 0].vert[2] = current_vertex + 2;
	md->triangles[current_triangle + 1].vert[0] = current_vertex + 0; 
	md->triangles[current_triangle + 1].vert[1] = current_vertex + 2;
	md->triangles[current_triangle + 1].vert[2] = current_vertex + 3;

	md->vertexNum += 4;
	md->triangleNum += 2;
}
