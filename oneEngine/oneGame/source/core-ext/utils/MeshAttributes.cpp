#include "MeshAttributes.h"
#include <vector>

// Recalculate Normals
void core::mesh::RecalculateNormals ( arModelData* md )
{
	// Some standard mesh information that you should have lying around.
	// vertex is your vertex structure that just contains a normal and position here.
	// vertices is a pointer to the first vertex
	// indices is a pointer to the first index
	// num_verts is number of vertices
	// num_indices is number of indices
	// each face of the mesh is made up of three vertices.

	/*std::vector<Vector3d>* normal_buffer = new std::vector<Vector3d> [md->vertexNum];

	//for( int i = 0; i < modelData->vertexNum; i += 3 )
	for ( unsigned int i = 0; i < md->triangleNum; i += 1 )
	{
		// get the three vertices that make the face
		Vector3d p1 = Vector3d( &md->vertices[md->triangles[i].vert[0]].x );
		Vector3d p2 = Vector3d( &md->vertices[md->triangles[i].vert[1]].y );
		Vector3d p3 = Vector3d( &md->vertices[md->triangles[i].vert[2]].z );

		// Calculate the face normal
		Vector3d v1 = p2 - p1;
		Vector3d v2 = p3 - p1;
		Vector3d normal = v1.cross( v2 );

		normal = normal.normal();

		// Store the face's normal for each of the vertices that make up the face.
		normal_buffer[md->triangles[i].vert[0]].push_back( normal );
		normal_buffer[md->triangles[i].vert[1]].push_back( normal );
		normal_buffer[md->triangles[i].vert[2]].push_back( normal );
	}

	// Now loop through each vertex vector, and avarage out all the normals stored.
	for( unsigned int i = 0; i < md->vertexNum; ++i )
	{
		Vector3d normalResult ( 0,0,0 );
		for( unsigned int j = 0; j < normal_buffer[i].size(); ++j )
		{
			normalResult += normal_buffer[i][j];
		}
		normalResult = normalResult.normal();
		md->vertices[i].nx = normalResult.x;
		md->vertices[i].ny = normalResult.y;
		md->vertices[i].nz = normalResult.z;
	}
	
	delete[] normal_buffer;*/

	// Create the normals
	if (md->normal == NULL)
		md->normal = new Vector3f [md->vertexNum];
	// Zero out the normals
	for (uint16_t vertex = 0; vertex < md->vertexNum; ++vertex)
	{
		md->normal[vertex] = Vector3f(0, 0, 0);
	}

	for (uint16_t index = 0; index < md->indexNum; index += 3)
	{
		// get the three vertices that make the face
		Vector3d p1 = md->position[md->indices[index + 0]];
		Vector3d p2 = md->position[md->indices[index + 1]];
		Vector3d p3 = md->position[md->indices[index + 2]];

		// Calculate the face normal
		Vector3d v1 = p2 - p1;
		Vector3d v2 = p3 - p1;
		Vector3d normal = v1.cross( v2 );

		// Don't normalize it: the larger the face, the more dominant this normal should be.

		// Store the face's normal for each of the vertices
		md->normal[md->indices[index + 0]] += normal;
		md->normal[md->indices[index + 1]] += normal;
		md->normal[md->indices[index + 2]] += normal;
	}

	// Normalize all the collected normals now
	for (uint16_t vertex = 0; vertex < md->vertexNum; ++vertex)
	{
		md->normal[vertex].normalize();
	}
}


//	RecalculateTangents(model) : Recalculates the tangent and binormals.
// Trashes existing tangent and binormal data as they are used for temporary data.
void core::mesh::RecalculateTangents ( arModelData* md )
{
	// Like many things in this engine, this uses the method detailed by Eric Lengyel.
	// Sourced from: http://www.terathon.com/code/tangent.html

	Vector3f* tangent1 = new Vector3f[md->vertexNum * 2];
	Vector3f* tangent2 = tangent1 + md->vertexNum;
	memset(tangent1, 0, md->vertexNum * sizeof(Vector3d) * 2);

	// Allocate tangents & binormals
	if (md->tangent == NULL)
		md->tangent = new Vector3f [md->vertexNum];
	if (md->binormal == NULL)
		md->binormal = new Vector3f [md->vertexNum];

	// Zero out data
	for (uint16_t v = 0; v < md->vertexNum; ++v)
	{
		md->tangent[v]  = Vector3f(0,0,0);
		md->binormal[v] = Vector3f(0,0,0);
	}

	// Collect the tangents
	for (uint16_t index = 0; index < md->indexNum; index += 3)
	{
		//arModelTriangle tri = md->triangles[t];
		uint16_t indices [3] = {
			md->indices[index + 0],
			md->indices[index + 1],
			md->indices[index + 2],
		};

		Vector3f pos [3] = {
			md->position[indices[0]],
			md->position[indices[1]],
			md->position[indices[2]]
		};
		Vector2f tex [3] = {
			md->texcoord0[indices[0]],
			md->texcoord0[indices[1]],
			md->texcoord0[indices[2]]
		};

		Vector3f dpos0 = pos[1] - pos[0];
		Vector3f dpos1 = pos[2] - pos[0];

		Vector2f dtex0 = tex[1] - tex[0];
		Vector2f dtex1 = tex[2] - tex[0];

		float r = 1.0F / dtex0.cross(dtex1);
		Vector3f sdir = (dpos0 * dtex1.y - dpos1 * dtex0.y) * r;
		Vector3f tdir = (dpos1 * dtex0.x - dpos0 * dtex1.x) * r;

		tangent1[indices[0]] += sdir;
		tangent1[indices[1]] += sdir;
		tangent1[indices[2]] += sdir;

		tangent2[indices[0]] += tdir;
		tangent2[indices[1]] += tdir;
		tangent2[indices[2]] += tdir;
	}

	// Orthonagonalize and save tangent result.
	for (uint16_t v = 0; v < md->vertexNum; ++v)
	{
		const Vector3f& n = md->normal[v];
		const Vector3f& t = tangent1[v];

		// Gram-Schmidt orthogonalize
		md->tangent[v] = (t - n * n.dot(t)).normal();

		// Calculate handedness
		Real handedness = (n.cross(t).dot(tangent2[v]) < 0.0F) ? -1.0F : 1.0F;
		md->binormal[v] = n.cross(md->tangent[v]) * handedness;
	}

	// Free temp data
	tangent2 = NULL;
	delete[] tangent1;
}