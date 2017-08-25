
#include "CTreeLeafRenderer.h"

/*
CTreeLeafRenderer::CTreeLeafRenderer ( void )
	: CRenderableObject(), CGameBehavior()
{
	GL.CreateBuffer( &trisBuffer, 4 );

	bIsUpdating = false;
	bNeedRegen = false;
	bVBOsInvalid = false;
	iTriangleCount = 0;

	SetMaterial( CTreeBase::pTreeLeafMaterial );
}

CTreeLeafRenderer::~CTreeLeafRenderer ( void )
{
	GL.FreeBuffer( &trisBuffer, 4 );
}

void CTreeLeafRenderer::Update ( void )
{
	if ( bVBOsInvalid ) {
		GL.FreeBuffer( &trisBuffer, 4 );
		GL.CreateBuffer( &trisBuffer, 4 );

		bNeedRegen = true;
		bIsUpdating = false;
		bVBOsInvalid = false;
	}

	if ( CTreeBase::bNeedRegen ) {
		bNeedRegen = true;
	}
	if ( bNeedRegen || bIsUpdating ) 
	{
		GL.BindBuffer( GL_ARRAY_BUFFER, vertBuffer );
		GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER, trisBuffer );

		// Reset state
		if ( !bIsUpdating ) 
		{
			// Grab current tree list
			vTreeList = CTreeBase::vTreeList;
	
			// Reset values
			estimateVert = 0;
			estimateTris = 0;
			iVertexOffset = 0;
			iTriangleOffset = 0;
			iCurrentUpdateIndex = 0;

			vector<CTreeBase*>::iterator it;
			for ( it = CTreeBase::vTreeList.begin(); it != CTreeBase::vTreeList.end(); ++it )
			{
				// First go through everyone to get the rough VBO size
				//estimateVert += 12 * (*it)->part_list.size() + (*it)->part_list.size()*7;
				//estimateVert += 16 * (*it)->part_list.size();
				//estimateTris += 15 * (*it)->part_list.size();
				estimateVert += 24 * (*it)->leaf_cloud.size();
				estimateTris += 12 * (*it)->leaf_cloud.size();
			}

			// Buffer with that size
			glBufferData( GL_ARRAY_BUFFER, sizeof(CModelVertex)*estimateVert, NULL, GL_STREAM_DRAW );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(CModelTriangle)*estimateTris, NULL, GL_STREAM_DRAW );

			bIsUpdating = true;
		}

		// Update 15 trees
		CTreeBase* tree;
		for ( uint i = 0; (i < 4)&&(iCurrentUpdateIndex < vTreeList.size()); ++i )
		{
			tree = vTreeList[iCurrentUpdateIndex];

			// Need to generate the mesh for the tree, and then copy it to a VBO
			tree->GenerateLeafMesh( tree->leaf_cloud );
			// Do not attempt map if the pieces to get mapped are too small
			if (( tree->modelData.vertexNum <= 1 )||( tree->modelData.triangleNum <= 1 ))
			{
				iCurrentUpdateIndex += 1;
				continue;
			}
			else 
			{
				// Loop through the vertices and transform by pos and rotation
				for ( uint v = 0; v < tree->modelData.vertexNum; ++v ) {	
					CTreeBase::pVertexBuffer[v].r = CTreeBase::pVertexBuffer[v].x;
					CTreeBase::pVertexBuffer[v].g = CTreeBase::pVertexBuffer[v].y;
					CTreeBase::pVertexBuffer[v].b = CTreeBase::pVertexBuffer[v].z; // Copy local model space over to color
					// alpha is....
					CTreeBase::pVertexBuffer[v].x += tree->transform.position.x;
					CTreeBase::pVertexBuffer[v].y += tree->transform.position.y;
					CTreeBase::pVertexBuffer[v].z += tree->transform.position.z;
					// todo: stream rotation
				}
				// Loop through the given indices and offset them
				for ( uint t = 0; t < tree->modelData.triangleNum; ++t ) {
					CTreeBase::pTriangleBuffer[t].vert[0] += iVertexOffset;
					CTreeBase::pTriangleBuffer[t].vert[1] += iVertexOffset;
					CTreeBase::pTriangleBuffer[t].vert[2] += iVertexOffset;
				}

				//cout << " new leafy buffer: " << ceil((tree->modelData.vertexNum*sizeof(CModelVertex))/1024.0f) << " KB" << endl;
				//cout << " estimate size is: " << ceil((estimateVert*sizeof(CModelVertex))/1024.0f) << " KB" << endl;


				// Set data to VBO
				void* vertexMap = glMapBufferRange( GL_ARRAY_BUFFER,
					iVertexOffset*sizeof(CModelVertex),
					tree->modelData.vertexNum*sizeof(CModelVertex),
					GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_RANGE_BIT|GL_MAP_UNSYNCHRONIZED_BIT ); //synchronous full replace, will not query vram (todo, look at async)
				if ( vertexMap ) {
					memcpy( vertexMap, CTreeBase::pVertexBuffer, tree->modelData.vertexNum*sizeof(CModelVertex) );
					glUnmapBuffer( GL_ARRAY_BUFFER );
				}
				else {
					uint err = glGetError();
					if (err == GL_INVALID_VALUE ) {
						cout << "leafsys: v GL_INVALID_VALUE" << endl;
					}
					else if ( err == GL_INVALID_OPERATION ) {
						cout << "leafsys: v GL_INVALID_OPERATION" << endl;
					}
					bVBOsInvalid = true;
				}
				// Queue OpenGL for buffer ranges and overwrite those ranges
				void* triangleMap = glMapBufferRange( GL_ELEMENT_ARRAY_BUFFER,
					iTriangleOffset*sizeof(CModelTriangle),
					tree->modelData.triangleNum*sizeof(CModelTriangle),
					GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_RANGE_BIT|GL_MAP_UNSYNCHRONIZED_BIT ); //synchronous full replace, will not query vram
				if ( triangleMap ) {
					memcpy( triangleMap, CTreeBase::pTriangleBuffer, tree->modelData.triangleNum*sizeof(CModelTriangle) );
					glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
				}
				else {
					uint err = glGetError();
					if (err == GL_INVALID_VALUE ) {
						cout << "leafsys: t GL_INVALID_VALUE" << endl;
					}
					else if ( err == GL_INVALID_OPERATION ) {
						cout << "leafsys: t GL_INVALID_OPERATION" << endl;
					}
					bVBOsInvalid = true;
				}

				iVertexOffset += tree->modelData.vertexNum;
				iTriangleOffset += tree->modelData.triangleNum;

				iCurrentUpdateIndex += 1;
			}
		}

		GL.UnbindBuffer( GL_ARRAY_BUFFER );
		GL.UnbindBuffer( GL_ELEMENT_ARRAY_BUFFER );

		if ( iCurrentUpdateIndex >= vTreeList.size() ) {
			bIsUpdating = false;
		}
	}
}
void CTreeLeafRenderer::PostUpdate ( void )
{
	if ( bNeedRegen && !bIsUpdating )
	{
		// Set triangle count
		iTriangleCount = iTriangleOffset;

		// Swap front and back buffers
		uint temp;
		temp = trisFrontBuffer;
		trisFrontBuffer = trisBuffer;
		trisBuffer = temp;
		temp = vertFrontBuffer;
		vertFrontBuffer = vertBuffer;
		vertBuffer = temp;

		// Done updating
		bNeedRegen = false;
	}
}

void CTreeLeafRenderer::Interrupt ( void )
{
	vTreeList = CTreeBase::vTreeList;
	iCurrentUpdateIndex = 0;
	bIsUpdating = false;
}


bool CTreeLeafRenderer::Render ( const char pass )
{
	if ( iTriangleCount <= 0 ) {
		return false;
	}

	GL.Transform( &transform );

	glEnableClientState(GL_VERTEX_ARRAY);             // activate vertex coords array
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	CTreeBase::pTreeLeafMaterial->bind();
	CTreeBase::pTreeLeafMaterial->setShaderConstants( this );

	GL.BindBuffer( GL_ARRAY_BUFFER, vertFrontBuffer );
	GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER, trisFrontBuffer );

	// Tell where the vertex coordinates are in the array
	glVertexPointer( 3, GL_FLOAT, sizeof(CModelVertex), 0 ); 
	glNormalPointer( GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*6) );
	glTexCoordPointer( 3, GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*3) );
	//glVertexAttribPointer( 1, 3, GL_FLOAT, false, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*12) );
	glColorPointer( 4, GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*15) );

	// Draw the sutff
	glDrawElements( GL_TRIANGLES, iTriangleCount*3, GL_UNSIGNED_INT, 0 );

	GL.UnbindBuffer( GL_ARRAY_BUFFER );
	GL.UnbindBuffer( GL_ELEMENT_ARRAY_BUFFER );

	// begin move to postrender
	glDisableClientState(GL_VERTEX_ARRAY);            // deactivate vertex array
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	// end move to postrender

	return true;
}*/