
#include "CTreeRenderer.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/material/glMaterial.h"

#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/system/TerrainRenderer.h"

CTreeRenderer::CTreeRenderer ( void )
	: CRenderableObject(), CGameBehavior()
{
	vertexData = NULL;
	treeTriData	= NULL;
	leafTriData	= NULL;
	treeTriTable = NULL;
	leafTriTable = NULL;

	maxVertCount = 1024*512;
	maxTreeTriCount = 1024*384;
	maxLeafTriCount = 1024*256;
	maxTreeCount = 512;

	while ( vertexData == NULL ) {
		AttemptMemoryAllocation();	
	}

	memset( vertexData, 0, sizeof(CModelVertex)*maxVertCount );
	for ( uint i = 0; i < maxTreeCount; ++i ) {
		treeTriTable[i] = -1;
		leafTriTable[i] = -1;
	}

	openVertex = 0;
	lastRecordedVertex = 0;

	treeTriCount = 0;
	leafTriCount = 0;
	
	GL_ACCESS;
	
	GL.CreateBuffer( &vertBuffer, 3 );

	/*ClearMaterialList();
	vMaterials.push_back( CTreeBase::pTreeBarkMaterial );
	vMaterials.push_back( CTreeBase::pTreeLeafMaterial );*/
	SetMaterial( CTreeBase::pTreeBarkMaterial );
}

void CTreeRenderer::AttemptMemoryAllocation ( void )
{
	try {
		vertexData = new CModelVertex [maxVertCount];
		treeTriData	= new CModelTriangle [maxTreeTriCount];
		leafTriData	= new CModelTriangle [maxLeafTriCount];
		treeTriTable = new int32_t [maxTreeCount];
		leafTriTable = new int32_t [maxTreeCount];
	}
	catch ( std::bad_alloc _bad ) {
		Debug::Console->PrintError( "TREESYS: Could not allocate memory for the vertex data!\n" );
		if ( vertexData ) delete [] vertexData;
		if ( treeTriData ) delete [] treeTriData;
		if ( leafTriData ) delete [] leafTriData;
		if ( treeTriTable ) delete [] treeTriTable;
		if ( leafTriTable ) delete [] leafTriTable;
		vertexData = NULL;
		treeTriData = NULL;
		leafTriData = NULL;
		treeTriTable = NULL;
		leafTriTable = NULL;
		// Dec max tree count
		maxVertCount = (uint32_t)(maxVertCount*0.75f);
		maxTreeTriCount = (uint32_t)(maxTreeTriCount*0.75f);
		maxLeafTriCount = (uint32_t)(maxLeafTriCount*0.75f);
		maxTreeCount = (uint32_t)(maxLeafTriCount*0.8f);;
	}
}


CTreeRenderer::~CTreeRenderer ( void )
{
	delete [] vertexData;
	delete [] treeTriData;
	delete [] leafTriData;
	delete [] treeTriTable;
	delete [] leafTriTable;

	GL_ACCESS;
	GL.FreeBuffer( &vertBuffer, 3 );
}

void CTreeRenderer::LateUpdate ( void )
{
	if ( CTreeBase::bNeedRegen ) 
	{
		if (( treeTriCount > 1 )||( leafTriCount > 1 ))
		{
			GL_ACCESS;
			GL.BindVertexArray(0);
			GL.BindBuffer( GL_ARRAY_BUFFER, vertBuffer );

			// Copy data to the buffer
			/*glBufferData( GL_ARRAY_BUFFER, 
				sizeof(CModelVertex) * (lastRecordedVertex), NULL,
				GL_STREAM_DRAW );
			glBufferSubData( GL_ARRAY_BUFFER,
				0,
				sizeof(CModelVertex) * (lastRecordedVertex),
				vertexData );*/
			GL.UploadBuffer( GL_ARRAY_BUFFER, sizeof(CModelVertex) * (lastRecordedVertex), vertexData, GL_STREAM_DRAW );

			if ( treeTriCount > 1 ) {
				GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER, treeTrisBuffer );
				/*glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
					sizeof(CModelTriangle) * (treeTriCount), NULL,
					GL_STREAM_DRAW );
				glBufferSubData( GL_ELEMENT_ARRAY_BUFFER,
					0,
					sizeof(CModelTriangle) * (treeTriCount),
					treeTriData );*/
				GL.UploadBuffer( GL_ELEMENT_ARRAY_BUFFER, sizeof(CModelTriangle) * (treeTriCount), treeTriData, GL_STREAM_DRAW );
			}
			if ( leafTriCount > 1 ) {
				GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER, leafTrisBuffer );
				/*glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
					sizeof(CModelTriangle) * (leafTriCount), NULL,
					GL_STREAM_DRAW );
				glBufferSubData( GL_ELEMENT_ARRAY_BUFFER,
					0,
					sizeof(CModelTriangle) * (leafTriCount),
					leafTriData );*/
				GL.UploadBuffer( GL_ELEMENT_ARRAY_BUFFER, sizeof(CModelTriangle) * (leafTriCount), leafTriData, GL_STREAM_DRAW );
			}
			GL.UnbindBuffer( GL_ARRAY_BUFFER );
			if (( treeTriCount > 1 )||( leafTriCount > 1 )) {
				GL.UnbindBuffer( GL_ELEMENT_ARRAY_BUFFER );
			}
		}

	}

	transform.position *= 0.04f;
}
void CTreeRenderer::PostUpdate ( void )
{
	if ( CTreeBase::bNeedRegen )
	{
		// Done updating
		CTreeBase::bNeedRegen = false;
	}

	// Visibility depends on terrain visibility
	if ( CVoxelTerrain::GetActive() ) {
		visible = CVoxelTerrain::GetActive()->Renderer->GetVisible();
	}
}
bool CTreeRenderer::Render ( const char pass )
{
	if ( (pass%2) == 0 && treeTriCount <= 1 )	return true;
	if ( (pass%2) == 1 && leafTriCount <= 1 )	return true;

	GL_ACCESS;
	GL.Transform( &transform );

	/*glEnableClientState(GL_VERTEX_ARRAY);             // activate vertex coords array
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);*/

	/*if ( pass == 0 ) {
		//CTreeBase::pTreeBarkMaterial->bind();
		//CTreeBase::pTreeBarkMaterial->setShaderConstants( this );
	}
	else {
		//CTreeBase::pTreeLeafMaterial->bind();
		//CTreeBase::pTreeLeafMaterial->setShaderConstants( this );
	}*/
	/*
	if ( !bCanRender )
		return true;

	GL.Transform( &transform );

	if ( bUseSkinning ) {
		m_material->m_bufferMatricesSkinning = ((glSkinnedMesh*)m_glMesh)->getTextureBuffer();
		m_material->m_bufferMatricesSoftbody = NULL;
	}
	else {
		m_material->m_bufferMatricesSkinning = NULL;
		m_material->m_bufferMatricesSoftbody = NULL;
	}
	m_material->bindPass(pass);
	m_parent->SendShaderUniforms(this);
	BindVAO( pass, m_glMesh->GetVBOverts(), m_glMesh->GetVBOfaces() );
	glDrawElements( GL_TRIANGLES, m_glMesh->pmData->triangleNum*3, GL_UNSIGNED_INT, 0 );

	return true;
	*/

	m_material->bindPass(pass);
	m_material->setShaderConstants( this );
	if ( (pass%2) == 0 ) {
		BindVAO( pass, vertBuffer, treeTrisBuffer );
		//glDrawElements( GL_TRIANGLES, treeTriCount*3, GL_UNSIGNED_INT, 0 );
		GL.DrawElements( GL_TRIANGLES, treeTriCount*3, GL_UNSIGNED_INT, 0 );
		return true;
	}
	else if ( (pass%2) == 1 ) {
		BindVAO( pass, vertBuffer, leafTrisBuffer );
		//glDrawElements( GL_TRIANGLES, leafTriCount*3, GL_UNSIGNED_INT, 0 );
		GL.DrawElements( GL_TRIANGLES, leafTriCount*3, GL_UNSIGNED_INT, 0 );
		return true;
	}
	else {
		return true; // TODO: Fix this for shadow casters. this is troublesome. Muchly so.
	}
	return false;
	/*GL.BindBuffer( GL_ARRAY_BUFFER, vertBuffer );
	if ( pass == 0 )	GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER, treeTrisBuffer );
	else				GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER, leafTrisBuffer );

	// Tell where the vertex coordinates are in the array
	glVertexPointer( 3, GL_FLOAT, sizeof(CModelVertex), 0 ); 
	glNormalPointer( GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*6) );
	glTexCoordPointer( 3, GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*3) );
	glMaterial::current->setShaderAttributesDefault();
	glColorPointer( 4, GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*15) );

	// Draw the sutff
	if ( pass == 0 )	glDrawElements( GL_TRIANGLES, treeTriCount*3, GL_UNSIGNED_INT, 0 );
	else				glDrawElements( GL_TRIANGLES, leafTriCount*3, GL_UNSIGNED_INT, 0 );

	GL.UnbindBuffer( GL_ARRAY_BUFFER );
	GL.UnbindBuffer( GL_ELEMENT_ARRAY_BUFFER );

	// begin move to postrender
	glDisableClientState(GL_VERTEX_ARRAY);            // deactivate vertex array
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	// end move to postrender

	return true;*/
}




void CTreeRenderer::FreeTreeBuffer ( sTreeBufferData* treeBuffer )
{
	if (( treeBuffer->tree_vert == -1 )||( treeBuffer->tree_triTable == -1 ))
	{	// has no data in the first place
		return;
	}

	{
		int32_t pos;
		int32_t step;

		// Make last vertex point at first open vertex
		pos = treeBuffer->tree_vert_last;
		step = openVertex-pos;
		*((int32_t*)(vertexData[pos].bone)) = step;
		// Make first vertex the new first open vertex (FIFO memory)
		openVertex = treeBuffer->tree_vert;
		// Clear pointer
		treeBuffer->tree_vert = -1;
	}

	{
		int32_t pos, i, maxi;
		//cout << "Shifting" << endl;

		// Look up tri position, and shift everything above it downward
		pos = treeTriTable[treeBuffer->tree_triTable];
		maxi = treeTriCount - treeBuffer->tree_triCount;
		for ( i = pos; i < maxi; ++i ) {
			treeTriData[i] = treeTriData[i+treeBuffer->tree_triCount];
		}
		// Look up all tris in the table and shift them as well if needed
		for ( i = 0; i < (int32_t)maxTreeCount; ++i ) {
			if ( treeTriTable[i] > pos ) {
				treeTriTable[i] -= treeBuffer->tree_triCount;
			}
			else if ( treeTriTable[i] == pos ) {
				treeTriTable[i] = -1;
			}
		}
		// Decrement tri position
		treeTriCount = maxi;

		// Clear table entry
		treeBuffer->tree_triTable = -1;
	}
}
void CTreeRenderer::FreeLeafBuffer ( sTreeBufferData* treeBuffer )
{
	if (( treeBuffer->leaf_vert == uint32_t(-1) )||( treeBuffer->leaf_triTable == uint32_t(-1) ))
	{	// has no data in the first place
		return;
	}

	{
		int32_t pos;
		int32_t step;

		// Make last vertex point at first open vertex
		pos = treeBuffer->leaf_vert_last;
		step = openVertex-pos;
		*((int32_t*)(vertexData[pos].bone)) = step;
		// Make first vertex the new first open vertex (FIFO memory)
		openVertex = treeBuffer->leaf_vert;
		// Clear pointer
		treeBuffer->leaf_vert = -1;
	}

	{
		int32_t pos, i, maxi;

		// Look up tri position, and shift everything above it downward
		pos = leafTriTable[treeBuffer->leaf_triTable];
		maxi = leafTriCount - treeBuffer->leaf_triCount;
		for ( i = pos; i < maxi; ++i ) {
			leafTriData[i] = leafTriData[i+treeBuffer->leaf_triCount];
		}
		// Look up all tris in the table and shift them as well if needed
		for ( i = 0; i < (int32_t)maxTreeCount; ++i ) {
			if ( leafTriTable[i] > pos ) {
				leafTriTable[i] -= treeBuffer->leaf_triCount;
			}
			else if ( leafTriTable[i] == pos ) {
				leafTriTable[i] = -1;
			}
		}
		// Decrement tri position
		leafTriCount = maxi;

		// Clear table entry
		treeBuffer->leaf_triTable = -1;
	}
}

void CTreeRenderer::UpdateTreeBuffer( sTreeBufferData *treeBuffer, const CModelData *md )
{
	uint32_t table_index;
	{
		// Grab first invalid table index
		for ( table_index = 0; table_index < maxTreeCount; ++table_index ) {
			if ( treeTriTable[table_index] == -1 ) {
				break;
			}
		}
		//if ( table_index >= maxTreeCount ) throw std::out_of_range( "Beyond tree count" );
		if ( table_index >= maxTreeCount ) { // Check for out of range
			treeBuffer->tree_triTable = -1;
			return;
		}

		// Reserve tris
		treeTriTable[table_index] = treeTriCount;
		treeTriCount += md->triangleNum;

		//if ( treeTriCount >= maxTreeTriCount ) throw std::out_of_range( "Beyond tree tri count" );
		if ( treeTriCount >= maxTreeTriCount ) { // Check for out of range
			treeTriTable[table_index] = -1;
			treeTriCount -= md->triangleNum;
			treeBuffer->tree_triTable = -1;
			return;
		}
	}

	{
		int32_t pos;
		int32_t step;
		int32_t vert;

		pos = openVertex;
		treeBuffer->tree_vert = pos;
		vert = 0;
		try
		{
			do
			{
				step = *((int32_t*)(vertexData[pos].bone));
				if ( step == 0 ) step = 1;
				// Set spot
				*((int32_t*)(md->vertices[vert].bone)) = pos;
				// Copy vert over
				vertexData[pos] = md->vertices[vert];
				// Set step
				*((int32_t*)(vertexData[pos].bone)) = step;
				// Take step
				treeBuffer->tree_vert_last = pos;
				pos += step;
				vert += 1;
				if ( pos >= (int32_t)maxVertCount ) throw std::out_of_range( "Beyond vertex count" );
			}
			while ( vert < (int32_t)md->vertexNum );
		}
		catch ( std::out_of_range rangeErr )
		{
			// Reset vert state
			treeBuffer->tree_vert = -1;
			// Reset tri state as well
			treeTriTable[table_index] = -1;
			treeTriCount -= md->triangleNum;
			treeBuffer->tree_triTable = -1;
			return;
		}
		// Set new open vert
		openVertex = pos;
		// Set last vert to have stop signal
		*((int32_t*)(vertexData[treeBuffer->tree_vert_last].bone)) = 0;

		if ( openVertex > lastRecordedVertex ) {
			lastRecordedVertex = openVertex;
		}
	}
	{
		uint32_t tri, i, maxi;
		char v;

		// Save reserved data info
		treeBuffer->tree_triCount = md->triangleNum;
		treeBuffer->tree_triTable = table_index;

		// Loop through reserved data, and copy over data
		maxi = treeTriCount;
		tri = 0;
		for ( i = treeTriTable[table_index]; i < maxi; ++i )
		{
			for ( v = 0; v < 3; ++v ) 
			{	// The bone has been set to the target position's new vertex.
				treeTriData[i].vert[v] = (uint32_t) (*((int32_t*)(md->vertices[md->triangles[tri].vert[v]].bone)));
			}
			tri += 1;
		}
	}
}

void CTreeRenderer::UpdateLeafBuffer( sTreeBufferData *treeBuffer, const CModelData *md )
{
	uint32_t table_index;
	{
		// Grab first invalid table index
		for ( table_index = 0; table_index < maxTreeCount; ++table_index ) {
			if ( leafTriTable[table_index] == -1 ) {
				break;
			}
		}
		//if ( table_index >= maxTreeCount ) throw std::out_of_range( "Beyond tree count" );
		if ( table_index >= maxTreeCount ) { // Check for out of range
			treeBuffer->leaf_triTable = -1;
			return;
		}

		// Reserve tris
		leafTriTable[table_index] = leafTriCount;
		leafTriCount += md->triangleNum;

		//if ( treeTriCount >= maxTreeTriCount ) throw std::out_of_range( "Beyond tree tri count" );
		if ( leafTriCount >= maxLeafTriCount ) { // Check for out of range
			leafTriTable[table_index] = -1;
			leafTriCount -= md->triangleNum;
			treeBuffer->leaf_triTable = -1;
			return;
		}
	}

	{
		int32_t pos;
		int32_t step;
		int32_t vert;

		pos = openVertex;
		treeBuffer->leaf_vert = pos;
		vert = 0;
		try
		{
			do
			{
				step = *((int32_t*)(vertexData[pos].bone));
				if ( step == 0 ) step = 1;
				// Set spot
				*((int32_t*)(md->vertices[vert].bone)) = pos;
				// Copy vert over
				vertexData[pos] = md->vertices[vert];
				// Set step
				*((int32_t*)(vertexData[pos].bone)) = step;
				// Take step
				treeBuffer->leaf_vert_last = pos;
				pos += step;
				vert += 1;
				if ( pos >= (int32_t)maxVertCount ) throw std::out_of_range( "Beyond vertex count" );
			}
			while ( vert < (int32_t)md->vertexNum );
		}
		catch ( std::out_of_range rangeErr )
		{
			// Reset vert state
			treeBuffer->leaf_vert = -1;
			// Reset tri state as well
			leafTriTable[table_index] = -1;
			leafTriCount -= md->triangleNum;
			treeBuffer->leaf_triTable = -1;
			return;
		}
		// Set new open vert
		openVertex = pos;
		// Set last vert to have stop signal
		*((int32_t*)(vertexData[treeBuffer->leaf_vert_last].bone)) = 0;

		if ( openVertex > lastRecordedVertex ) {
			lastRecordedVertex = openVertex;
		}
	}
	{
		uint32_t tri, i, maxi;
		char v;

		// Save reserved data info
		treeBuffer->leaf_triCount = md->triangleNum;
		treeBuffer->leaf_triTable = table_index;

		// Loop through reserved data, and copy over data
		maxi = leafTriCount;
		tri = 0;
		for ( i = leafTriTable[table_index]; i < maxi; ++i )
		{
			for ( v = 0; v < 3; ++v ) 
			{	// The bone has been set to the target position's new vertex.
				leafTriData[i].vert[v] = (uint32_t) (*((int32_t*)(md->vertices[md->triangles[tri].vert[v]].bone)));
			}
			tri += 1;
		}
	}
}


/*
CTreeRenderer::CTreeRenderer ( void )
	: CRenderableObject(), CGameBehavior()
{
	GL.CreateBuffer( &trisBuffer, 4 );

	bIsUpdating = false;
	bVBOsInvalid = false;
	iTriangleCount = 0;

	SetMaterial( CTreeBase::pTreeBarkMaterial );
}

CTreeRenderer::~CTreeRenderer ( void )
{
	GL.FreeBuffer( &trisBuffer, 4 );
}

void CTreeRenderer::Update ( void )
{
	if ( bVBOsInvalid ) {
		GL.FreeBuffer( &trisBuffer, 4 );
		GL.CreateBuffer( &trisBuffer, 4 );

		CTreeBase::bNeedRegen = true;
		bIsUpdating = false;
		bVBOsInvalid = false;
	}

	if ( CTreeBase::bNeedRegen || bIsUpdating ) 
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
			CTreeBase::iVertexOffset = 0;
			CTreeBase::iTriangleOffset = 0;
			iCurrentUpdateIndex = 0;

			vector<CTreeBase*>::iterator it;
			for ( it = CTreeBase::vTreeList.begin(); it != CTreeBase::vTreeList.end(); ++it )
			{
				// First go through everyone to get the rough VBO size
				//estimateVert += 12 * (*it)->part_list.size() + (*it)->part_list.size()*7;
				estimateVert += 18 * (*it)->part_list.size();
				estimateTris += 17 * (*it)->part_list.size();
			}

			// Buffer with that size
			glBufferData( GL_ARRAY_BUFFER, sizeof(CModelVertex)*estimateVert, NULL, GL_STREAM_DRAW );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(CModelTriangle)*estimateTris, NULL, GL_STREAM_DRAW );

			bIsUpdating = true;
		}

		// Update 15 trees
		CTreeBase* tree;
		for ( uint i = 0; (!bVBOsInvalid)&&(i < 4)&&(iCurrentUpdateIndex < vTreeList.size()); ++i )
		{
			tree = vTreeList[iCurrentUpdateIndex];

			// Need to generate the mesh for the tree, and then copy it to a VBO
			tree->GenerateTreeMesh();
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
					CTreeBase::pVertexBuffer[v].a = CTreeBase::pVertexBuffer[v].z + (sqr(CTreeBase::pVertexBuffer[v].x) + sqr(CTreeBase::pVertexBuffer[v].y)) * 0.1f;
					CTreeBase::pVertexBuffer[v].x += tree->transform.position.x;
					CTreeBase::pVertexBuffer[v].y += tree->transform.position.y;
					CTreeBase::pVertexBuffer[v].z += tree->transform.position.z;
					// todo: stream rotation
				}
				// Loop through the given indices and offset them
				for ( uint t = 0; t < tree->modelData.triangleNum; ++t ) {
					CTreeBase::pTriangleBuffer[t].vert[0] += CTreeBase::iVertexOffset;
					CTreeBase::pTriangleBuffer[t].vert[1] += CTreeBase::iVertexOffset;
					CTreeBase::pTriangleBuffer[t].vert[2] += CTreeBase::iVertexOffset;
				}

				//cout << "new vertex buffer: " << ceil((tree->modelData.vertexNum*sizeof(CModelVertex))/1024.0f) << " KB" << endl;
				//cout << " estimate size is: " << ceil((estimateVert*sizeof(CModelVertex))/1024.0f) << " KB" << endl;

				// Set data to VBO via direct mapping memory
				void* vertexMap = glMapBufferRange( GL_ARRAY_BUFFER,
					CTreeBase::iVertexOffset*sizeof(CModelVertex),
					tree->modelData.vertexNum*sizeof(CModelVertex),
					GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_RANGE_BIT );
				if ( vertexMap ) {
					memcpy( vertexMap, CTreeBase::pVertexBuffer, tree->modelData.vertexNum*sizeof(CModelVertex) );
					glUnmapBuffer( GL_ARRAY_BUFFER );
				}
				else {
					uint err = glGetError();
					if (err == GL_INVALID_VALUE ) {
						cout << "treesys: v GL_INVALID_VALUE" << endl;
					}
					else if ( err == GL_INVALID_OPERATION ) {
						cout << "treesys: v GL_INVALID_OPERATION" << endl;
					}
					bVBOsInvalid = true;
				}
				// Queue OpenGL for buffer ranges and overwrite those ranges
				void* triangleMap = glMapBufferRange( GL_ELEMENT_ARRAY_BUFFER,
					CTreeBase::iTriangleOffset*sizeof(CModelTriangle),
					tree->modelData.triangleNum*sizeof(CModelTriangle),
					GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_RANGE_BIT );
				if ( triangleMap ) {
					memcpy( triangleMap, CTreeBase::pTriangleBuffer, tree->modelData.triangleNum*sizeof(CModelTriangle) );
					glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
				}
				else {
					uint err = glGetError();
					if (err == GL_INVALID_VALUE ) {
						cout << "treesys: t GL_INVALID_VALUE" << endl;
					}
					else if ( err == GL_INVALID_OPERATION ) {
						cout << "treesys: t GL_INVALID_OPERATION" << endl;
					}
					bVBOsInvalid = true;
				}

				CTreeBase::iVertexOffset += tree->modelData.vertexNum;
				CTreeBase::iTriangleOffset += tree->modelData.triangleNum;

				if ( CTreeBase::iVertexOffset >= estimateVert ) {
					cout << "treesys: v overflow" << endl;
				}
				if ( CTreeBase::iTriangleOffset >= estimateTris ) {
					cout << "treesys: t overflow" << endl;
				}

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
void CTreeRenderer::PostUpdate ( void )
{
	if ( CTreeBase::bNeedRegen && !bIsUpdating )
	{
		// Set triangle count
		iTriangleCount = CTreeBase::iTriangleOffset;

		// Swap front and back buffers
		uint temp;
		temp = trisFrontBuffer;
		trisFrontBuffer = trisBuffer;
		trisBuffer = temp;
		temp = vertFrontBuffer;
		vertFrontBuffer = vertBuffer;
		vertBuffer = temp;

		// Done updating
		CTreeBase::bNeedRegen = false;
	}
}

void CTreeRenderer::Interrupt ( void )
{
	vTreeList = CTreeBase::vTreeList;
	iCurrentUpdateIndex = 0;
	bIsUpdating = false;
}


bool CTreeRenderer::Render ( const char pass )
{
	if ( iTriangleCount <= 0 ) {
		return false;
	}

	GL.Transform( &transform );

	glEnableClientState(GL_VERTEX_ARRAY);             // activate vertex coords array
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	CTreeBase::pTreeBarkMaterial->bind();
	CTreeBase::pTreeBarkMaterial->setShaderConstants( this );

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
}
*/