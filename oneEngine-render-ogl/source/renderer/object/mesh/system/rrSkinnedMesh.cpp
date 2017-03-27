
#include "rrSkinnedMesh.h"
#include "renderer/material/RrMaterial.h"

#include "renderer/system/glMainSystem.h"

// Constructor
rrSkinnedMesh::rrSkinnedMesh ( void )
	: rrMesh()
{
	iRenderMode = RD_GPU;

	//partNum = 0;

	//iVBOfaces_gpu	= NULL;
	//partTriCount	= NULL;

	//pvSkeleton		= NULL;

	//pvPoseMatrices	= NULL;
	//pvPoseMatrices = new Matrix4x4 [256];
	//for ( int i = 0; i < 256; ++i ) pvPoseMatrices[i] = Matrix4x4();
	//pvSoftMatrices	= NULL;

	//newStreamReady	= false;
	//isStreaming		= false;
	//threadIsValid	= false;

	iQualityType	= QD_MED;

	//isDuplicatedStream = false;

	//m_textureBuffer = 0;
	//m_textureBufferData = 0;
	//m_textureBufferData_Swap = 0;
	/*m_textureSoftBuffer = 0;
	m_textureSoftBufferData = 0;
	m_textureSoftBufferData_Swap = 0;*/
	skinning_data = {0};
}
// Destructor
rrSkinnedMesh::~rrSkinnedMesh ( void )
{
	// Free the created streams
	//FreeSplitMesh();
	//FreeCPUMesh();
	//FreeGPUBuffer();
	// Free the skeleton and matrix streams
	//if ( pvSkeleton ) {
	//	while ( !pvSkeleton->empty() ) {
	//		delete ((*pvSkeleton)[pvSkeleton->size()-1]);
	//		pvSkeleton->pop_back();
	//	}
	//	pvSkeleton = NULL;
	//}
	//if ( pvPoseMatrices ) {
	//	delete [] pvPoseMatrices;
	//	pvPoseMatrices = NULL;
	//}
	/*if ( pvSoftMatrices ) {
		delete [] pvSoftMatrices;
		pvSoftMatrices = NULL;
	}*/
}
//void rrSkinnedMesh::FreeStream ( void )
//{
//	// We can not delete the ibDataType
//	ibDataType = USERDATA_NONE;	// Doesn't allow rrMesh destructor to delete
//	// We can not free the material
//	pmMat = NULL;				// Doesn't allow rrMesh destructor to delete
//	// We can not touch the source data
//	bReady = false;				// Doesn't allow rrMesh destructor to delete
//	// We do need to get rid of the VBOs we use
//	if ( iVBOverts )
//		glDeleteBuffers( 1, &iVBOverts );
//	iVBOverts = 0;
//	if ( iVBOfaces )
//		glDeleteBuffers( 1, &iVBOfaces );
//	iVBOfaces = 0;
//}

void rrSkinnedMesh::Initialize ( const string& nNewName, CModelData* const pNewModelData, unsigned int frames, bool isStreamed )
{
	rrMesh::Initialize( nNewName, pNewModelData, frames, false );

	
	// Find max bone index
	/*partNum = unsigned int( ceil( pvSkeleton->size() / 21.0f ) );

	// Create the split mesh based on that
	CreateSplitMesh();*/

	// Matrix stuff
	//pvPoseMatrices = new Matrix4x4 [pvSkeleton->size()];
	//pvSoftMatrices = new Matrix4x4 [pvSkeleton->size()];

	// Initialize an non-transform for CPU skinning
	//if ( iRenderMode == RD_CPU )
	//{
	//	// Push values to buffer
	//	for ( uint i = 0; i < pvSkeleton->size(); ++i ) {
	//		pvPoseMatrices[i] = Matrix4x4();
	//		//pvSoftMatrices[i] = Matrix4x4();
	//	}
	//	if ( m_textureBufferData )
	//	{
	//		glBindBuffer( GL_UNIFORM_BUFFER, m_textureBufferData );
	//		glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW );
	//		glBufferSubData( GL_UNIFORM_BUFFER, 0,sizeof(Matrix4x4)*pvSkeleton->size(), pvPoseMatrices );
	//		/*GLvoid* p = glMapBuffer( GL_UNIFORM_BUFFER, GL_WRITE_ONLY );
	//		memcpy(p, pvPoseMatrices, sizeof(Matrix4x4)*pvSkeleton->size() );
	//		glUnmapBuffer( GL_UNIFORM_BUFFER );*/

	//	}
	//	/*if ( m_textureSoftBufferData )
	//	{
	//		glBindBuffer( GL_UNIFORM_BUFFER, m_textureSoftBufferData );
	//		glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW );
	//		glBufferSubData( GL_UNIFORM_BUFFER, 0,sizeof(Matrix4x4)*pvSkeleton->size(), pvSoftMatrices );*/
	//		/*GLvoid* p = glMapBuffer( GL_UNIFORM_BUFFER, GL_WRITE_ONLY );
	//		memcpy(p, pvSoftMatrices, sizeof(Matrix4x4)*pvSkeleton->size() );
	//		glUnmapBuffer( GL_UNIFORM_BUFFER );*/
	//	//}
	//}

	// stream stuff
	/*pmStreamData = new CModelData();
	(*pmStreamData) = (*pmData);
	pmStreamData->vertices = new CModelVertex [pmStreamData->vertexNum] ();*/

	// NORMALIZE!
	for ( unsigned int vert = 0; vert < pmData->vertexNum; vert++ )
	{
		// normalize weights
		float ft = (pmData->vertices[vert].weight[0]+
			pmData->vertices[vert].weight[1]+
			pmData->vertices[vert].weight[2]+
			pmData->vertices[vert].weight[3]);
		pmData->vertices[vert].weight[0] /= ft;
		pmData->vertices[vert].weight[1] /= ft;
		pmData->vertices[vert].weight[2] /= ft;
		pmData->vertices[vert].weight[3] /= ft;
	}

	// Check errors
	GL_ACCESS GL.CheckError();
}

rrSkinnedMesh* rrSkinnedMesh::Copy ( void )
{
	rrSkinnedMesh* newmesh = new rrSkinnedMesh();

	newmesh->bReady	= bReady;
	newmesh->iVBOverts	= iVBOverts;
	newmesh->iVBOfaces	= iVBOfaces;
	newmesh->pmData	= pmData;
	newmesh->meshNum	= meshNum;
	newmesh->pbData = pbData;
	newmesh->ibDataType = ibDataType;
	newmesh->pmMat = pmMat;
	newmesh->mName = mName;

	newmesh->iRenderMode = iRenderMode;
	/*partNum = psrcMesh->partNum;
	iVBOfaces_gpu	= psrcMesh->iVBOfaces_gpu;
	partTriCount	= psrcMesh->partTriCount;*/

	//newStreamReady	= psrcMesh->newStreamReady;
	//isStreaming		= psrcMesh->isStreaming;
	//threadIsValid	= psrcMesh->threadIsValid;
	newmesh->iQualityType	= iQualityType;
	//isDuplicatedStream = psrcMesh->isDuplicatedStream;

	//pvSkeleton	= psrcMesh->pvSkeleton;
	//pvPoseMatrices	= psrcMesh->pvPoseMatrices;
	//pvSoftMatrices	= psrcMesh->pvSoftMatrices;

	newmesh->parentBone		= parentBone;
	newmesh->useSkinning	= useSkinning;

	//partNum	= psrcMesh->partNum;
	//iVBOfaces_gpu	= psrcMesh->iVBOfaces_gpu;
	//partTriCount	= psrcMesh->partTriCount;

	//pmStreamData	= psrcMesh->pmStreamData;

	//m_textureBuffer = psrcMesh->m_textureBuffer;
	//m_textureSoftBuffer = psrcMesh->m_textureSoftBuffer;

	//if ( pvSkeleton && pvSkeleton->size() > 1023 )
	//{
	//	throw core::CorruptedDataException();
	//}
	// Create new matrix list
	//pvPoseMatrices = new Matrix4x4 [pvSkeleton->size()];
	//pvPoseMatrices = psrcMesh->pvPoseMatrices;

	// Need to create new stream data
	//pmStreamData = new CModelData();
	//(*pmStreamData) = (*pmData);
	//pmStreamData->vertices = new CModelVertex [pmStreamData->vertexNum] ();

	// Normalize
	for ( unsigned int vert = 0; vert < newmesh->pmData->vertexNum; vert++ )
	{
		// normalize weights
		float ft = (
			newmesh->pmData->vertices[vert].weight[0]+
			newmesh->pmData->vertices[vert].weight[1]+
			newmesh->pmData->vertices[vert].weight[2]+
			newmesh->pmData->vertices[vert].weight[3]
			);
		newmesh->pmData->vertices[vert].weight[0] /= ft;
		newmesh->pmData->vertices[vert].weight[1] /= ft;
		newmesh->pmData->vertices[vert].weight[2] /= ft;
		newmesh->pmData->vertices[vert].weight[3] /= ft;
	}

	glBindVertexArray( 0 );
	// New output buffers need to be generated
	glGenBuffers( 1, &newmesh->iVBOverts );
	glGenBuffers( 1, &newmesh->iVBOfaces );
	{
		// Bind to some buffer objects
		glBindBuffer( GL_ARRAY_BUFFER,			newmesh->iVBOverts ); // for vertex coordinates
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,	newmesh->iVBOfaces ); // for face vertex indexes

															 // Copy data to the buffer
		glBufferData( GL_ARRAY_BUFFER,
			sizeof(CModelVertex) * (pmData->vertexNum),
			pmData->vertices,
			GL_STATIC_DRAW );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER,
			sizeof(CModelTriangle) * (pmData->triangleNum),
			pmData->triangles,
			GL_STATIC_DRAW );

		// bind with 0, so, switch back to normal pointer operation
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}

	// Check errors
	GL_ACCESS GL.CheckError();

	return newmesh;
}

//void rrSkinnedMesh::CopyStream ( rrSkinnedMesh * const psrcMesh )
//{
//	// Need to copy most values over
//	bReady	= psrcMesh->bReady;
//	iVBOverts	= psrcMesh->iVBOverts;
//	iVBOfaces	= psrcMesh->iVBOfaces;
//	pmData	= psrcMesh->pmData;
//	meshNum	= psrcMesh->meshNum;
//	pbData = psrcMesh->pbData;
//	ibDataType = psrcMesh->ibDataType;
//	pmMat = psrcMesh->pmMat;
//	mName = psrcMesh->mName;
//
//	iRenderMode = psrcMesh->iRenderMode;
//	/*partNum = psrcMesh->partNum;
//	iVBOfaces_gpu	= psrcMesh->iVBOfaces_gpu;
//	partTriCount	= psrcMesh->partTriCount;*/
//	
//	newStreamReady	= psrcMesh->newStreamReady;
//	isStreaming		= psrcMesh->isStreaming;
//	threadIsValid	= psrcMesh->threadIsValid;
//	iQualityType	= psrcMesh->iQualityType;
//	//isDuplicatedStream = psrcMesh->isDuplicatedStream;
//
//	//pvSkeleton	= psrcMesh->pvSkeleton;
//	//pvPoseMatrices	= psrcMesh->pvPoseMatrices;
//	//pvSoftMatrices	= psrcMesh->pvSoftMatrices;
//
//	parentBone	= psrcMesh->parentBone;
//	useSkinning	= psrcMesh->useSkinning;
//
//	partNum	= psrcMesh->partNum;
//	iVBOfaces_gpu	= psrcMesh->iVBOfaces_gpu;
//	partTriCount	= psrcMesh->partTriCount;
//
//	pmStreamData	= psrcMesh->pmStreamData;
//
//	//m_textureBuffer = psrcMesh->m_textureBuffer;
//	//m_textureSoftBuffer = psrcMesh->m_textureSoftBuffer;
//
//	//if ( pvSkeleton && pvSkeleton->size() > 1023 )
//	//{
//	//	throw core::CorruptedDataException();
//	//}
//	// Create new matrix list
//	//pvPoseMatrices = new Matrix4x4 [pvSkeleton->size()];
//	//pvPoseMatrices = psrcMesh->pvPoseMatrices;
//
//	// Need to create new stream data
//	pmStreamData = new CModelData();
//	(*pmStreamData) = (*pmData);
//	pmStreamData->vertices = new CModelVertex [pmStreamData->vertexNum] ();
//	// Normalize
//	for ( unsigned int vert = 0; vert < pmData->vertexNum; vert++ )
//	{
//		// normalize weights
//		float ft = (pmData->vertices[vert].weight[0]+
//			pmData->vertices[vert].weight[1]+
//			pmData->vertices[vert].weight[2]+
//			pmData->vertices[vert].weight[3]);
//		pmData->vertices[vert].weight[0] /= ft;
//		pmData->vertices[vert].weight[1] /= ft;
//		pmData->vertices[vert].weight[2] /= ft;
//		pmData->vertices[vert].weight[3] /= ft;
//	}
//
//	glBindVertexArray( 0 );
//	// New output buffers need to be generated
//	glGenBuffers( 1, &iVBOverts );
//	glGenBuffers( 1, &iVBOfaces );
//	{
//		// Bind to some buffer objects
//		glBindBuffer( GL_ARRAY_BUFFER,			iVBOverts ); // for vertex coordinates
//		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,	iVBOfaces ); // for face vertex indexes
//
//		// Copy data to the buffer
//		glBufferData( GL_ARRAY_BUFFER,
//			sizeof(CModelVertex) * (pmData->vertexNum),
//			pmData->vertices,
//			GL_STATIC_DRAW );
//		glBufferData( GL_ELEMENT_ARRAY_BUFFER,
//			sizeof(CModelTriangle) * (pmData->triangleNum),
//			pmData->triangles,
//			GL_STATIC_DRAW );
//
//		// bind with 0, so, switch back to normal pointer operation
//		glBindBuffer( GL_ARRAY_BUFFER, 0 );
//		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
//	}
//
//	{
//		glGenBuffers( 1, &m_textureBufferData );
//		//glBindBuffer( GL_UNIFORM_BUFFER, m_textureBufferData );
//		/*glGenTextures( 1, &m_textureBuffer );
//		glBindTexture( GL_TEXTURE_BUFFER, m_textureBuffer );
//
//		glBindTexture( GL_TEXTURE_BUFFER, m_textureBuffer );
//		glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA32F, m_textureBufferData ); */
//		glBindBuffer( GL_UNIFORM_BUFFER, m_textureBufferData );
//		glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW );
//
//		glGenBuffers( 1, &m_textureBufferData_Swap );
//		glBindBuffer( GL_UNIFORM_BUFFER, m_textureBufferData_Swap );
//		glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW );
//
//		/*glGenBuffers( 1, &m_textureSoftBufferData );
//		//glBindBuffer( GL_UNIFORM_BUFFER, m_textureSoftBufferData );
//		glBindBuffer( GL_UNIFORM_BUFFER, m_textureSoftBufferData );
//		glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW );
//
//		glGenBuffers( 1, &m_textureSoftBufferData_Swap );
//		glBindBuffer( GL_UNIFORM_BUFFER, m_textureSoftBufferData_Swap );
//		glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW );
//		*/
//		glBindBuffer( GL_UNIFORM_BUFFER, 0 );
//	}
//
//	// Initialize an non-transform for CPU skinning
//	if ( iRenderMode == RD_CPU && pvPoseMatrices /*&& pvSoftMatrices*/ )
//	{
//		// Push values to buffer
//		for ( uint i = 0; i < pvSkeleton->size(); ++i ) {
//			pvPoseMatrices[i] = Matrix4x4();
//			//pvSoftMatrices[i] = Matrix4x4();
//		}
//		if ( m_textureBufferData )
//		{
//			glBindBuffer( GL_UNIFORM_BUFFER, m_textureBufferData );
//			glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW );
//			glBufferSubData( GL_UNIFORM_BUFFER, 0,sizeof(Matrix4x4)*pvSkeleton->size(), pvPoseMatrices );
//		}
//		/*if ( m_textureSoftBufferData )
//		{
//			glBindBuffer( GL_UNIFORM_BUFFER, m_textureSoftBufferData );
//			glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW );
//			glBufferSubData( GL_UNIFORM_BUFFER, 0,sizeof(Matrix4x4)*pvSkeleton->size(), pvSoftMatrices );
//		}*/
//	}
//
//	// Set to a duplicated stream
//	isDuplicatedStream = true;
//
//	StartSkinning();
//
//	// Check errors
//	GL_ACCESS GL.CheckError();
//}

void rrSkinnedMesh::SetSkinningData ( const skinningData_t& n_skinning_data )
{
	skinning_data = n_skinning_data;
}

//// Render the mesh
//void rrSkinnedMesh::Render ( void )
//{
//	switch ( iRenderMode )
//	{
//	case RD_CPU:
//		RenderCPU();
//		break;
//	case RD_GPU_FALLBACK:
//		RenderGPUFallback();
//		break;
//	case RD_GPU:
//		throw std::exception();
//		// Draw current Mesh
//		// Bind to the buffer objects
//		/*glBindBuffer( GL_ARRAY_BUFFER,			iVBOverts ); // for vertex coordinates
//		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,	iVBOfaces ); // for face vertex indexes
//	
//		glDisableClientState(GL_VERTEX_ARRAY);             // activate vertex coords array
//		glDisableClientState(GL_NORMAL_ARRAY);
//		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//		glDisableClientState(GL_COLOR_ARRAY);
//
//		if ( RrMaterial::current == NULL ) {
//			throw std::exception();
//		}
//		if ( bShaderSetup == false )
//		{
//			RrMaterial::current->initializeMeshAttributes();
//			GL.CheckError();
//			bShaderSetup = true;
//		}
//		else {
//			RrMaterial::current->forwardMeshAttributes();
//			GL.CheckError();
//			bShaderSetup = true;
//		}
//
//		{
//			RrMaterial::current->bindTextureBuffer( m_textureBuffer, "textureMatrices" );
//			//GL.CheckError();
//			//GL.CheckError();
//		}
//	
//		// Draw the sutff
//		glDrawElements( GL_TRIANGLES, pmData->triangleNum*3, GL_UNSIGNED_INT, 0 );
//		//GL.CheckError();
//
//		//glDisable( GL_TEXTURE_BUFFER );
//		//GL.CheckError();
//
//		// bind with 0, so, switch back to normal pointer operation
//		glBindBuffer( GL_ARRAY_BUFFER, 0 );
//		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );*/
//	}
//}
//
//void rrSkinnedMesh::SendBoneMatrices ( int partOffset )
//{
//	/*unsigned int setSize = pvSkeleton->size() - partOffset;
//	if ( setSize > 24 )
//		setSize = 24;
//	RrMaterial::current->setUniformV( "sys_BoneMatrix", setSize, &(pvPoseMatrices[partOffset]) );
//	RrMaterial::current->setUniform( "sys_BoneOffset", ftype(partOffset) );*/
//}

//void rrSkinnedMesh::GetPoseMatrices ( void )
//{
//	for ( std::vector<glBone*>::iterator bone = pvSkeleton->begin(); bone != pvSkeleton->end(); bone++ )
//	{
//		/*Matrix4x4 temp = ((*bone)->GetPoseMatrix());
//		pvPoseMatrices[(*bone)->index] = !temp;
//		pvPoseMatrices[(*bone)->index].pData[3] = temp.pData[12];
//		pvPoseMatrices[(*bone)->index].pData[12] = temp.pData[3];
//		pvPoseMatrices[(*bone)->index].pData[7] = temp.pData[13];
//		pvPoseMatrices[(*bone)->index].pData[13] = temp.pData[7];
//		pvPoseMatrices[(*bone)->index].pData[11] = temp.pData[14];
//		pvPoseMatrices[(*bone)->index].pData[14] = temp.pData[11];*/
//		pvPoseMatrices[(*bone)->index] = ((*bone)->GetPoseMatrix());
//		//pvPoseMatrices[(*bone)->index] = Matrix4x4();
//		//pvSoftMatrices[(*bone)->index] = ((*bone)->tempMatx);
//	}
//}

// Start skinning
//void rrSkinnedMesh::StartSkinning ( void )
//{
//	/*LARGE_INTEGER tick1, tick2, freq;
//	QueryPerformanceFrequency( &freq );
//	QueryPerformanceCounter( &tick1 );*/
//	if  ( iRenderMode == RD_CPU )
//	{
//		if (( !newStreamReady )&&( !isStreaming ))
//		{
//			//if ( tSkinning.timed_join(boost::posix_time::milliseconds(1)) )
//			//if ( !tSkinning.timed_join(boost::posix_time::milliseconds(0) ) )
//			//if (( tSkinning.joinable() )||( !threadIsValid ))
//			//if (( !tSkinning.timed_join(boost::posix_time::milliseconds(0) ) )||( !threadIsValid ))
//			//if (( tSkinning.joinable() )||( !threadIsValid ))
//			{
//				GetPoseMatrices();
//
//				//tSkinning.join();
//
//				isStreaming = true;
//				threadIsValid = true;
//
//				/*sSkinningUpdater updater;
//				updater.mesh = this;
//				tSkinning = thread( updater );*/
//				DoCPUSkinning();
//			}
//		}
//	}
//	else if ( iRenderMode == RD_GPU )
//	{
//		if (( !newStreamReady )&&( !isStreaming ))
//		{
//			GetPoseMatrices();
//
//			isStreaming = true;
//			threadIsValid = true;
//
//			// Push values to buffer
//			if ( m_textureBufferData )
//			{
//				std::swap( m_textureBufferData, m_textureBufferData_Swap );
//				glBindBuffer( GL_UNIFORM_BUFFER, m_textureBufferData );
//				/*glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW ); // orphaning seems to work better on certain drivers
//				//glBufferSubData( GL_UNIFORM_BUFFER, 0,sizeof(Matrix4x4)*pvSkeleton->size(), pvPoseMatrices );
//				//GLvoid* p = glMapBuffer( GL_UNIFORM_BUFFER, GL_WRITE_ONLY );
//				void* p = glMapBufferRange( GL_UNIFORM_BUFFER, 0, sizeof(Matrix4x4)*pvSkeleton->size(), GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
//				if ( p ) {
//					memcpy( p, pvPoseMatrices, sizeof(Matrix4x4)*pvSkeleton->size() );
//					glUnmapBuffer( GL_UNIFORM_BUFFER );
//				}*/
//				glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW ); // orphaning seems to work better on certain drivers
//				//glBufferSubData( GL_UNIFORM_BUFFER, 0,sizeof(Matrix4x4)*pvSkeleton->size(), pvPoseMatrices );
//				//GLvoid* p = glMapBuffer( GL_UNIFORM_BUFFER, GL_WRITE_ONLY );
//				void* p = glMapBufferRange( GL_UNIFORM_BUFFER, 0, sizeof(Matrix4x4)*pvSkeleton->size(), GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
//				//void* p = glMapBufferRange( GL_UNIFORM_BUFFER, 0, sizeof(Matrix4x4)*pvSkeleton->size(), GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT );
//				if ( p ) {
//					memcpy( p, pvPoseMatrices, sizeof(Matrix4x4)*pvSkeleton->size() );
//					glUnmapBuffer( GL_UNIFORM_BUFFER );
//				}
//			}
//			/*if ( m_textureSoftBufferData )
//			{
//				std::swap( m_textureSoftBufferData, m_textureSoftBufferData_Swap );
//				glBindBuffer( GL_UNIFORM_BUFFER, m_textureSoftBufferData );
//				glBufferData( GL_UNIFORM_BUFFER, sizeof(Matrix4x4)*pvSkeleton->size(), NULL, GL_STREAM_DRAW ); // orphaning seems to work better on certain drivers
//				//glBufferSubData( GL_UNIFORM_BUFFER, 0,sizeof(Matrix4x4)*pvSkeleton->size(), pvSoftMatrices );
//				//GLvoid* p = glMapBuffer( GL_UNIFORM_BUFFER, GL_WRITE_ONLY );
//				void* p = glMapBufferRange( GL_UNIFORM_BUFFER, 0, sizeof(Matrix4x4)*pvSkeleton->size(), GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
//				if ( p ) {
//					memcpy( p, pvSoftMatrices, sizeof(Matrix4x4)*pvSkeleton->size() );
//					glUnmapBuffer( GL_UNIFORM_BUFFER );
//				}
//			}*/
//			/*GLubyte* dest = (GLubyte*)glMapBuffer( GL_TEXTURE_BUFFER, GL_WRITE_ONLY );
//			for (int i = 0; i < pvSkeleton->size(); i++)
//			{
//				*(Matrix4x4*)dest = pvPoseMatrices[i];
//				dest += sizeof(Matrix4x4);
//			}
//			GLboolean success = glUnmapBuffer(GL_TEXTURE_BUFFER);*/
//
//			// new stream!
//			newStreamReady = true;
//			isStreaming = false;
//
//			// Bind back to zero to prevent bitchy shit
//			//glBindTexture( GL_TEXTURE_BUFFER, 0 );
//		}
//	}
//	/*QueryPerformanceCounter( &tick2 );
//	cout << (((ftype)(tick2.QuadPart-tick1.QuadPart))/(freq.QuadPart))*1000.0f << endl;*/
//}
//void rrSkinnedMesh::sSkinningUpdater::operator () ( void )
//{
//	mesh->DoCPUSkinning();
//}

//void rrSkinnedMesh::PrepareStream ( void )
//{
//	memcpy( pmStreamData->vertices, pmData->vertices, sizeof(CModelVertex)*pmData->vertexNum );
//}

//void rrSkinnedMesh::DoCPUSkinning ( void )
//{
//	/*LARGE_INTEGER tick1, tick2, freq;
//	QueryPerformanceFrequency( &freq );
//	QueryPerformanceCounter( &tick1 );*/
//
//	Vector3d ipos, inorm;
//	Vector3d fpos, fnorm;
//	ftype weight;
//	Matrix4x4 fmatx;
//	char bone;
//	//float zeroes [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//	if ( iQualityType == QD_HIGH )
//	{
//		for ( unsigned int vert = 0; vert < pmData->vertexNum; vert++ )
//		{
//			//pmStreamData->vertices[vert] = pmData->vertices[vert];
//			ipos = _VertexToVect(pmStreamData->vertices[vert]);
//			inorm = _NormalToVect(pmStreamData->vertices[vert]);
//
//			fpos = Vector3d(0,0,0);
//			fnorm = Vector3d(0,0,0);
//			//fmatx = Matrix4x4( zeroes );
//			for ( unsigned char b = 0; b < 4; b++ )
//			{
//				bone = pmData->vertices[vert].bone[b];
//				if ( bone != 255 )
//				{
//					fpos += (pvPoseMatrices[bone]*ipos) * (pmData->vertices[vert].weight[b]);
//					fnorm += (pvPoseMatrices[bone].getRotator()*inorm) * (pmData->vertices[vert].weight[b]);
//					//fmatx += pvPoseMatrices[bone] * pmData->vertices[vert].weight[b];
//				}
//			}
//			//fpos = fmatx*ipos;
//			//fnorm = fmatx*inorm;
//
//			pmStreamData->vertices[vert].x = fpos.x;
//			pmStreamData->vertices[vert].y = fpos.y;
//			pmStreamData->vertices[vert].z = fpos.z;
//
//			fnorm = fnorm.normal();
//			pmStreamData->vertices[vert].nx = fnorm.x;
//			pmStreamData->vertices[vert].ny = fnorm.y;
//			pmStreamData->vertices[vert].nz = fnorm.z;
//		}
//	}
//	else if ( iQualityType == QD_MED )
//	{
//		for ( unsigned int vert = 0; vert < pmData->vertexNum; vert++ )
//		{
//			//pmStreamData->vertices[vert] = pmData->vertices[vert];
//			ipos = _VertexToVect(pmStreamData->vertices[vert]);
//			inorm = _NormalToVect(pmStreamData->vertices[vert]);
//			
//			if ( pmData->vertices[vert].bone[1] != 255 )
//			{
//				//fpos = ((ipos*pvPoseMatrices[pmData->vertices[vert].bone[0]])+(ipos*pvPoseMatrices[pmData->vertices[vert].bone[1]]))*0.5f;
//				//fnorm = ((inorm*pvPoseMatrices[pmData->vertices[vert].bone[0]].getRotator())+(inorm*pvPoseMatrices[pmData->vertices[vert].bone[1]].getRotator()))*0.5f;
//				weight = pmData->vertices[vert].weight[0]/(pmData->vertices[vert].weight[0]+pmData->vertices[vert].weight[1]);
//				fpos =   ((pvPoseMatrices[pmData->vertices[vert].bone[0]]*ipos)*weight)
//						+((pvPoseMatrices[pmData->vertices[vert].bone[1]]*ipos)*(1-weight));
//				fnorm =  ((pvPoseMatrices[pmData->vertices[vert].bone[0]].getRotator()*inorm)*weight)
//						+((pvPoseMatrices[pmData->vertices[vert].bone[1]].getRotator()*inorm)*(1-weight));
//
//			}
//			else if ( pmData->vertices[vert].bone[0] != 255 )
//			{
//				fpos = (pvPoseMatrices[pmData->vertices[vert].bone[0]]*ipos);
//				fnorm = (pvPoseMatrices[pmData->vertices[vert].bone[0]].getRotator()*inorm);
//			}
//			else
//			{
//				fpos = ipos;
//				fnorm = inorm;
//			}
//
//			pmStreamData->vertices[vert].x = fpos.x;
//			pmStreamData->vertices[vert].y = fpos.y;
//			pmStreamData->vertices[vert].z = fpos.z;
//
//			fnorm = fnorm.normal();
//			pmStreamData->vertices[vert].nx = fnorm.x;
//			pmStreamData->vertices[vert].ny = fnorm.y;
//			pmStreamData->vertices[vert].nz = fnorm.z;
//		}
//	}
//	else if ( iQualityType == QD_LOW )
//	{
//		for ( unsigned int vert = 0; vert < pmData->vertexNum; vert++ )
//		{
//			//pmStreamData->vertices[vert] = pmData->vertices[vert];
//			ipos = _VertexToVect(pmStreamData->vertices[vert]);
//			inorm = _NormalToVect(pmStreamData->vertices[vert]);
//			
//			fpos = (pvPoseMatrices[pmData->vertices[vert].bone[0]]*ipos);
//			fnorm = (pvPoseMatrices[pmData->vertices[vert].bone[0]].getRotator()*inorm);
//
//			pmStreamData->vertices[vert].x = fpos.x;
//			pmStreamData->vertices[vert].y = fpos.y;
//			pmStreamData->vertices[vert].z = fpos.z;
//
//			fnorm = fnorm.normal();
//			pmStreamData->vertices[vert].nx = fnorm.x;
//			pmStreamData->vertices[vert].ny = fnorm.y;
//			pmStreamData->vertices[vert].nz = fnorm.z;
//		}
//	}
//
//	// new stream!
//	newStreamReady = true;
//	isStreaming = false;
//
//	/*QueryPerformanceCounter( &tick2 );
//	cout << (((ftype)(tick2.QuadPart-tick1.QuadPart))/(freq.QuadPart))*1000.0f << endl;*/
//}
void rrSkinnedMesh::UpdateVBO ( void )
{
	// Only need to update the vertex VBO
	// Bind to some buffer objects
	glBindBuffer( GL_ARRAY_BUFFER,			iVBOverts ); // for vertex coordinates

	// Copy data to the buffer
	glBufferData( GL_ARRAY_BUFFER,
		sizeof(CModelVertex) * (pmData->vertexNum),
		pmData->vertices,
		GL_STREAM_DRAW );

	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
//void rrSkinnedMesh::RenderCPU ( void )
//{
//	rrMesh::Render();
//}
//
//void rrSkinnedMesh::FreeCPUMesh ( void )
//{
//	//tSkinning.join();
//
//	if ( pmStreamData->vertices != NULL )
//		delete [] pmStreamData->vertices;
//	pmStreamData->vertices = NULL;
//
//	if ( pmStreamData != NULL )
//		delete pmStreamData; // TODO: memory error here.
//	pmStreamData = NULL;
//
//	if ( pvPoseMatrices != NULL )
//		delete [] pvPoseMatrices;
//	pvPoseMatrices = NULL;
//
//	/*if ( pvSoftMatrices != NULL )
//		delete [] pvSoftMatrices;
//	pvSoftMatrices = NULL;*/
//}















// GPU Skinning below

//void rrSkinnedMesh::CreateSplitMesh ( void )
//{
//	iVBOfaces_gpu = new GLuint [partNum];
//	partTriCount = new unsigned int [partNum];
//
//	std::vector<CModelTriangle>	triangleList;
//	unsigned int boneIndexOffset;
//	for ( unsigned int i = 0; i < partNum; i++ )
//	{
//		// Clear the  list
//		triangleList.clear();
//
//		// Set the offset
//		boneIndexOffset = i*21;
//
//		// Generate the data set
//		for ( unsigned int tri = 0; tri < pmData->triangleNum; tri++ )
//		{
//			// Look through the triangle verts. If all the verts lock into our range, then add the tri to the draw list
//			bool add = true;
//			for ( char v = 0; v < 3; v++ )
//			{
//				for ( char b = 0; b < 4; b++ )
//				{
//					unsigned char bone = pmData->vertices[pmData->triangles[tri].vert[v]].bone[b];
//					// if the bone index is below 0 or above 23, don't add it.
//					if ( bone < 250 )
//					{
//						if (( bone < boneIndexOffset )||( bone >= boneIndexOffset+24 ))
//						{
//							add = false;
//						}
//					}
//				}
//			}
//			// Add it!
//			if ( add )
//			{
//				triangleList.push_back( pmData->triangles[tri] );
//			}
//		}
//		// Set the vertex count
//		partTriCount[i] = triangleList.size();
//
//		// Create new buffers
//		glGenBuffers( 1, &(iVBOfaces_gpu[i]) );
//
//		// Bind to some buffer objects
//		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,	iVBOfaces ); // for face vertex indexes
//
//		// Send the data to our model
//		if ( partTriCount[i] > 0 )
//		{
//			glBufferData( GL_ELEMENT_ARRAY_BUFFER,
//				sizeof(CModelTriangle) * (partTriCount[i]),
//				&triangleList[0],
//				GL_STATIC_DRAW );
//		}
//		else
//		{
//			glBufferData( GL_ELEMENT_ARRAY_BUFFER,
//				sizeof(CModelTriangle) * (partTriCount[i]),
//				NULL,
//				GL_STATIC_DRAW );
//		}
//	}
//	// bind with 0, so, switch back to normal pointer operation
//	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
//}
//// Clean up the nasty leaks from the split mesh
//void rrSkinnedMesh::FreeSplitMesh ( void )
//{
//	if ( iVBOfaces_gpu != NULL )
//	{
//		for ( unsigned int i = 0; i < partNum; i++ )
//		{
//			glDeleteBuffers( 1, &(iVBOfaces_gpu[i]) );
//		}
//		delete [] iVBOfaces_gpu;
//	}
//	iVBOfaces_gpu = NULL;
//
//	if ( partTriCount != NULL )
//		delete [] partTriCount;
//	partTriCount = NULL;
//
//	if ( pvPoseMatrices != NULL )
//		delete [] pvPoseMatrices;
//	pvPoseMatrices = NULL;
//}
//// Render with shader
//void rrSkinnedMesh::RenderGPUFallback ( void )
//{
//	// Draw current Mesh
//	// Bind to the buffer objects
//	glBindBuffer( GL_ARRAY_BUFFER,			iVBOverts ); // for vertex coordinates
//	
//	// Tell where the vertex coordinates are in the array
//	/*glVertexPointer( 3, GL_FLOAT, sizeof(CModelVertex), 0 ); 
//	glNormalPointer( GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*6) );
//	glTexCoordPointer( 3, GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*3) );*/
//	//glVertexAttribPointer( 1, 3, GL_FLOAT, false, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*12) );
//	//glColorPointer( 3, GL_FLOAT, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*15) );
//
//	// Set skinning striding
//	/*if ( RrMaterial::current->useSkinning )
//	{
//		GLint weightLoc, boneLoc;
//		weightLoc = glGetAttribLocation( RrMaterial::current->pShader->get_program(),"sys_BoneWeights" );
//		glEnableVertexAttribArray( weightLoc );
//		glVertexAttribPointer( weightLoc, 4, GL_FLOAT, false, sizeof(CModelVertex), ((char*)4) + (sizeof(float)*19) );
//		boneLoc = glGetAttribLocation( RrMaterial::current->pShader->get_program(),"sys_BoneIndices" );
//		glEnableVertexAttribArray( boneLoc );
//		glVertexAttribPointer( boneLoc, 4, GL_UNSIGNED_BYTE, false, sizeof(CModelVertex), ((char*)0) + (sizeof(float)*19) );
//		//cout << unsigned int( pmData[0].vertices[pmData[0].vertexNum-1].bone[3] ) << endl;
//	}*/
//
//	for ( unsigned int curPart = 0; curPart < partNum; curPart++ )
//	{
//		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,	iVBOfaces_gpu[curPart] ); // for face vertex indexes
//
//		//SendBoneMatrices( curPart*21 );
//
//		// Draw the sutff
//		glDrawElements( GL_TRIANGLES, partTriCount[curPart]*3, GL_UNSIGNED_INT, 0 );
//	}
//
//	// bind with 0, so, switch back to normal pointer operation
//	glBindBuffer( GL_ARRAY_BUFFER, 0 );
//	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
//}