
#include "core/settings/CGameSettings.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/material/glMaterial.h"
#include "renderer/camera/CCamera.h"

#include "CParticleRenderer.h"

CParticleVertex*	CParticleRenderer::pVertices	= NULL;
CModelTriangle*		CParticleRenderer::pTriangles	= NULL;
unsigned int		CParticleRenderer::iRendererCount = 0;

CParticleRenderer::CParticleRenderer( CParticleEmitter* pInEmitter )
	: CRenderableObject()
{
	myEmitter = pInEmitter;

	iRenderMethod = P_FLAT_BILLBOARD_PRECISE;

	// Set initial variable values
	iTriangleCount = 0;

	// Set default rendering options
	fR_SpeedScale = 0.1f;

	// Create CPU-side buffers
	if ( pVertices == NULL ) {
		pVertices = new CParticleVertex [8192];
		pTriangles = new CModelTriangle [4096];
	}
	iRendererCount += 1;

	//hVertices = GL.GetNewBuffer();
	glGenBuffers( 1, &hVertices );		// TODO: use GL wrapper
	glGenBuffers( 1, &hTriangles );
}

CParticleRenderer::~CParticleRenderer ( void )
{
	// If VBOs are valid, remove them
	if ( hVertices )
		glDeleteBuffers( 1, &hVertices );
	if ( hTriangles )
		glDeleteBuffers( 1, &hTriangles );

	// Decrement count on the renderer
	iRendererCount -= 1;
	if ( iRendererCount == 0 ) {
		delete [] pVertices;
		delete [] pTriangles;
		pVertices = NULL;
		pTriangles = NULL;
	}
}
#include "core-ext/profiler/CTimeProfiler.h"
#include "renderer/debug/CDebugDrawer.h"
bool CParticleRenderer::BeginRender ( void )
{
	if ( myEmitter == NULL ) {
		return false;
	}

	/*for ( auto it = myEmitter->vParticles.begin(); it != myEmitter->vParticles.end(); ++it )
	{
		Color tColor = it->cColor * m_material->m_diffuse;
		Debug::Drawer->DrawLine( it->vPosition + Vector3d( it->fSize,0,0 ), it->vPosition + Vector3d( -it->fSize,0,0 ), tColor );
		Debug::Drawer->DrawLine( it->vPosition + Vector3d( 0,it->fSize,0 ), it->vPosition + Vector3d( 0,-it->fSize,0 ), tColor );
		Debug::Drawer->DrawLine( it->vPosition + Vector3d( 0,0,it->fSize ), it->vPosition + Vector3d( 0,0,-it->fSize ), tColor );
	}*/

	TimeProfiler.BeginTimeProfile( "rs_particle_renderer_begin" );

	uint32_t vertNum, triNum;
	CreateMesh( vertNum, triNum );
	TimeProfiler.EndAddTimeProfile( "rs_particle_renderer_begin" );
	TimeProfiler.BeginTimeProfile( "rs_particle_renderer_push" );

	glBindVertexArray( 0 );
	// Create VBO if needed
	if ( !hVertices )
		glGenBuffers( 1, &hVertices );
	if ( !hTriangles )
		glGenBuffers( 1, &hTriangles );

	// Pass verts into VBO
	// Bind the buffers, and send the data
	glBindBuffer( GL_ARRAY_BUFFER, hVertices );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, hTriangles );

	glBufferData( GL_ARRAY_BUFFER, sizeof(CParticleVertex)*vertNum, NULL, GL_STREAM_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER,sizeof(CModelTriangle)*triNum, NULL, GL_STREAM_DRAW );

	glBufferSubData( GL_ARRAY_BUFFER, 0,sizeof(CParticleVertex)*vertNum, pVertices );
	glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0,  sizeof(CModelTriangle)*triNum, pTriangles );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	iTriangleCount = triNum;
	
	TimeProfiler.EndAddTimeProfile( "rs_particle_renderer_push" );

	return true;
}

// MESH MUST BE MADE IN START RENDER
bool CParticleRenderer::PreRender ( const char pass ) // CANNOT MAKE MESH IN PRERENDER
{
	return true;
}
bool CParticleRenderer::Render ( const char pass )
{
	GL_ACCESS
	// Do a pass for each material
	if ( iTriangleCount > 0 )
	{
		m_material->bindPass(pass);
		GL.CheckError();
		m_material->setShaderConstants( this, true );
		GL.CheckError();
		if ( BindVAO( pass, hVertices, hTriangles, true ) )
		{
			GL.CheckError();
			// Define vertex attributes
			m_material->bindAttribute( "mdl_Vertex", 3, GL_FLOAT, false, sizeof(CParticleVertex), ((char*)0) + (sizeof(float)*0) );
			m_material->bindAttribute( "mdl_TexCoord", 2, GL_FLOAT, false, sizeof(CParticleVertex), ((char*)0) + (sizeof(float)*6) );
			m_material->bindAttribute( "mdl_Color", 4, GL_FLOAT, false, sizeof(CParticleVertex), ((char*)0) + (sizeof(float)*8) );
			m_material->bindAttribute( "mdl_Normal", 3, GL_FLOAT, false, sizeof(CParticleVertex), ((char*)0) + (sizeof(float)*3) );
		}
		GL.CheckError();

		// do same as vertex array except pointer
		// Draw the stuff
		glDrawElements( GL_TRIANGLES, iTriangleCount*3, GL_UNSIGNED_INT, 0 );
		GL.CheckError();

		// deactivate vertex array
	}

	return true;
}

void CParticleRenderer::CreateMesh ( uint32_t& out_vertCount, uint32_t& out_triCount )
{
	// Draw properly aligned quads
	if ( iRenderMethod == P_FLAT_BILLBOARD_PRECISE )
	{
		// Set needed calculation values
		Vector3d temp;
		CTransform& camtrans = CCamera::activeCamera->transform;
		Quaternion cameraRot = camtrans.rotation.getQuaternion();

		Vector3d vParticlePosition;
		Vector3d vRight (0,1,0), vUp(0,0,1);
		Vector3d corners[5];

		Vector3d vrRight, vrUp;
		Vector3d vCamForward;
		vCamForward = camtrans.Forward();

		Matrix4x4 cameraMatxRot;
		cameraMatxRot.setRotation( cameraRot );
		vRight = cameraMatxRot*vRight;	// Get general up and right vector in camera space
		vUp = cameraMatxRot*vUp;

		Quaternion particleRot;
		Matrix4x4 particleRotMatx;

		uint32_t i = 0;
		uint32_t t = 0;
		// Loop through all the particles and add to the list
		for ( uint16_t pi = 0; pi < myEmitter->m_max_particle_index; ++pi )
		{
			const CParticle& particle = myEmitter->m_particles[pi];
			if ( !particle.alive ) continue;

			// Generate true position
			vParticlePosition = (myEmitter->bSimulateInWorldspace) ? particle.vPosition :( particle.vPosition + myEmitter->transform.position );

			// Get corner positions
			particleRot.AxisAngle( vCamForward, particle.fAngle );
			vrRight = (particleRot*vRight) *particle.fSize;
			vrUp = (particleRot*vUp) * particle.fSize;
			corners[3] = (-vrRight+vrUp);
			corners[2] = (-vrRight-vrUp);
			corners[1] = (vrRight-vrUp);
			corners[0] = (vrRight+vrUp);
			corners[4] = -vCamForward;

			// Set the colors and normals
			temp = cameraRot*(-corners[4]);
			for ( int j = 0; j < 4; ++j ) {
				pVertices[i+j].r = particle.cColor.red;
				pVertices[i+j].g = particle.cColor.green;
				pVertices[i+j].b = particle.cColor.blue;
				pVertices[i+j].a = particle.cColor.alpha;
				pVertices[i+j].nx = temp.x;
				pVertices[i+j].ny = temp.y;
				pVertices[i+j].nz = temp.z;
			}

			// Set the positions and texture coordinates
			temp = vParticlePosition+corners[0];
			pVertices[i+0].x = temp.x;
			pVertices[i+0].y = temp.y;
			pVertices[i+0].z = temp.z;
			pVertices[i+0].u = 0;
			pVertices[i+0].v = 0;

			temp = vParticlePosition+corners[1];
			pVertices[i+1].x = temp.x;
			pVertices[i+1].y = temp.y;
			pVertices[i+1].z = temp.z;
			pVertices[i+1].u = 0;
			pVertices[i+1].v = 1;

			temp = vParticlePosition+corners[2];
			pVertices[i+2].x = temp.x;
			pVertices[i+2].y = temp.y;
			pVertices[i+2].z = temp.z;
			pVertices[i+2].u = 1;
			pVertices[i+2].v = 1;

			temp = vParticlePosition+corners[3];
			pVertices[i+3].x = temp.x;
			pVertices[i+3].y = temp.y;
			pVertices[i+3].z = temp.z;
			pVertices[i+3].u = 1;
			pVertices[i+3].v = 0;

			// Set the triangles
			pTriangles[t+0].vert[0] = i+0;
			pTriangles[t+0].vert[1] = i+1;
			pTriangles[t+0].vert[2] = i+2;
			pTriangles[t+1].vert[0] = i+0;
			pTriangles[t+1].vert[1] = i+2;
			pTriangles[t+1].vert[2] = i+3;

			// Increment step
			i += 4;
			t += 2;
		}

		// Set output count
		out_vertCount = i;
		out_triCount = t;
	}
	else if ( iRenderMethod == P_STRETCHED_BILLBOARD )
	{
		// Set needed calculation values
		Vector3d temp;
		CTransform& camtrans = CCamera::activeCamera->transform;
		Quaternion cameraRot = camtrans.rotation.getQuaternion();

		Vector3d vParticlePosition;
		Vector3d vRight (0,1,0), vUp(0,0,1);
		Vector3d corners[5];

		Vector3d vrRight, vrUp;
		Vector3d vCamForward;
		vCamForward = camtrans.Forward();

		Matrix4x4 cameraMatxRot;
		cameraMatxRot.setRotation( cameraRot );

		Quaternion particleRot;
		Matrix4x4 particleRotMatx;

		uint32_t i = 0;
		uint32_t t = 0;
		// Loop through all the particles and add to the list
		for ( uint16_t pi = 0; pi < myEmitter->m_max_particle_index; ++pi )
		{
			const CParticle& particle = myEmitter->m_particles[pi];
			if ( !particle.alive ) continue;

			// Get corner positions// Generate true position
			vParticlePosition = (myEmitter->bSimulateInWorldspace) ? particle.vPosition :( particle.vPosition + myEmitter->transform.position );

			vrRight = particle.vVelocity * fR_SpeedScale;
			if ( vrRight.sqrMagnitude() < sqr(particle.fSize*0.5f) ) {
				vrRight = vrRight.normal() * particle.fSize*0.5f;
			}
			vrUp = vCamForward.cross( vrRight.normal() );
			if ( vrUp.sqrMagnitude() < 0.01f ) {
				particleRot.AxisAngle( vCamForward, particle.fAngle );
				vrUp = (particleRot*vRight);
			}
			vrUp *= particle.fSize;

			corners[3] = (-vrRight+vrUp);
			corners[2] = (-vrRight-vrUp);
			corners[1] = (vrRight-vrUp);
			corners[0] = (vrRight+vrUp);
			corners[4] = -vCamForward;

			// Set the colors and normals
			temp = cameraRot*(-corners[4]);
			for ( int j = 0; j < 4; ++j ) {
				pVertices[i+j].r = particle.cColor.red;
				pVertices[i+j].g = particle.cColor.green;
				pVertices[i+j].b = particle.cColor.blue;
				pVertices[i+j].a = particle.cColor.alpha;
				pVertices[i+j].nx = temp.x;
				pVertices[i+j].ny = temp.y;
				pVertices[i+j].nz = temp.z;
			}

			// Set the positions and texture coordinates
			temp = vParticlePosition+corners[0];
			pVertices[i+0].x = temp.x;
			pVertices[i+0].y = temp.y;
			pVertices[i+0].z = temp.z;
			pVertices[i+0].u = 0;
			pVertices[i+0].v = 0;

			temp = vParticlePosition+corners[1];
			pVertices[i+1].x = temp.x;
			pVertices[i+1].y = temp.y;
			pVertices[i+1].z = temp.z;
			pVertices[i+1].u = 0;
			pVertices[i+1].v = 1;

			temp = vParticlePosition+corners[2];
			pVertices[i+2].x = temp.x;
			pVertices[i+2].y = temp.y;
			pVertices[i+2].z = temp.z;
			pVertices[i+2].u = 1;
			pVertices[i+2].v = 1;

			temp = vParticlePosition+corners[3];
			pVertices[i+3].x = temp.x;
			pVertices[i+3].y = temp.y;
			pVertices[i+3].z = temp.z;
			pVertices[i+3].u = 1;
			pVertices[i+3].v = 0;

			// Set the triangles
			pTriangles[t+0].vert[0] = i+0;
			pTriangles[t+0].vert[1] = i+1;
			pTriangles[t+0].vert[2] = i+2;
			pTriangles[t+1].vert[0] = i+0;
			pTriangles[t+1].vert[1] = i+2;
			pTriangles[t+1].vert[2] = i+3;

			pTriangles[t+2].vert[0] = i+0;
			pTriangles[t+2].vert[1] = i+2;
			pTriangles[t+2].vert[2] = i+1;
			pTriangles[t+3].vert[0] = i+0;
			pTriangles[t+3].vert[1] = i+3;
			pTriangles[t+3].vert[2] = i+2;

			// Increment step
			i += 4;
			t += 4;
		}

		// Set output count
		out_vertCount = i;
		out_triCount = t;
	}
	else if ( iRenderMethod == P_FLAT_TRAILS )
	{
		// TIME TO DRAW SOME SICK TRAILS

		// Set needed calculation values
		Vector3d temp;
		CTransform& camtrans = CCamera::activeCamera->transform;
		Quaternion cameraRot = camtrans.rotation.getQuaternion();

		Vector3d vParticlePosition;
		Vector3d vPrevParticlePosition;
		Vector3d vRight (0,1,0), vUp(0,0,1);
		Vector3d corners[5];

		Vector3d vrUp, vrNext, vrPrevUp;
		Vector3d vCamForward;
		vCamForward = camtrans.Forward();	// Get the direction to the camera
		
		uint32_t i = 0;
		uint32_t t = 0;

		CParticle* particle = NULL;
		CParticle* prevparticle = NULL;

		// TODO: Unroll loop
		if ( myEmitter->m_particle_count > 0 )
		for ( uint16_t pi = 0; pi <= myEmitter->m_max_particle_index; ++pi )
		{
			//const CParticle& particle = myEmitter->m_particles[pi];

			// Save previous iteration's information
			vrPrevUp = vrUp;
			prevparticle = particle; 

			// Get current particle
			if ( pi < myEmitter->m_max_particle_index )
			{
				particle = &myEmitter->m_particles[pi];
				if ( particle != NULL && !particle->alive ) continue;
			}
			else
			{	// Need to finish off the trail w/ last iteration
				particle = NULL;
			}

			if ( particle == NULL ) continue;

			//if ( p < myEmitter->vParticles.size() )
			//	particle = &(myEmitter->vParticles[p]);

			// Generate true position
			vPrevParticlePosition = vParticlePosition;
			vParticlePosition = (myEmitter->bSimulateInWorldspace) ? particle->vPosition :( particle->vPosition + myEmitter->transform.position );

			// First need to get the vector offsets for the position.
			/*vrNext = Vector3d( 0,0,0 );
			if ( p > 0 ) {
				vrNext += vParticlePosition - myEmitter->vParticles[p-1].vPosition;
			}
			if ( p < myEmitter->vParticles.size()-1 ) {
				vrNext += myEmitter->vParticles[p+1].vPosition - vParticlePosition;
			}*/
			if ( prevparticle != NULL )
			{
				vrNext = vParticlePosition - prevparticle->vPosition;
				vrNext.normalize();
			}

			// With an approximate vector of the particle direction, now generate point offset
			vrUp = vCamForward.cross( vrNext ).normal();
			//vrNext = vrUp.cross( vCamForward );
			
			// First particle uses emitter origin
			//if ( p == 0 ) {
			if ( prevparticle == NULL ) {
				continue;
			}
			else if ( particle != NULL )
			{
				// Create a quad that works off the vrUp and vrPrevUp
				corners[0] = vParticlePosition + vrUp*particle->fSize;
				corners[2] = vParticlePosition - vrUp*particle->fSize;
				corners[1] = vPrevParticlePosition + vrPrevUp*prevparticle->fSize;
				corners[3] = vPrevParticlePosition - vrPrevUp*prevparticle->fSize;

				// Set vertice position information
				pVertices[i+0].x = corners[0].x;
				pVertices[i+0].y = corners[0].y;
				pVertices[i+0].z = corners[0].z;
				pVertices[i+0].u = 0;
				pVertices[i+0].v = particle->fLife / particle->fStartLife;

				pVertices[i+2].x = corners[2].x;
				pVertices[i+2].y = corners[2].y;
				pVertices[i+2].z = corners[2].z;
				pVertices[i+2].u = 1;
				pVertices[i+2].v = particle->fLife / particle->fStartLife;

				pVertices[i+1].x = corners[1].x;
				pVertices[i+1].y = corners[1].y;
				pVertices[i+1].z = corners[1].z;
				pVertices[i+1].u = 0;
				pVertices[i+1].v = prevparticle->fLife / prevparticle->fStartLife;

				pVertices[i+3].x = corners[3].x;
				pVertices[i+3].y = corners[3].y;
				pVertices[i+3].z = corners[3].z;
				pVertices[i+3].u = 1;
				pVertices[i+3].v = prevparticle->fLife / prevparticle->fStartLife;

				// Set the colors
				pVertices[i+0].r = particle->cColor.red;
				pVertices[i+0].g = particle->cColor.green;
				pVertices[i+0].b = particle->cColor.blue;
				pVertices[i+0].a = particle->cColor.alpha;

				pVertices[i+2].r = particle->cColor.red;
				pVertices[i+2].g = particle->cColor.green;
				pVertices[i+2].b = particle->cColor.blue;
				pVertices[i+2].a = particle->cColor.alpha;

				pVertices[i+1].r = prevparticle->cColor.red;
				pVertices[i+1].g = prevparticle->cColor.green;
				pVertices[i+1].b = prevparticle->cColor.blue;
				pVertices[i+1].a = prevparticle->cColor.alpha;

				pVertices[i+3].r = prevparticle->cColor.red;
				pVertices[i+3].g = prevparticle->cColor.green;
				pVertices[i+3].b = prevparticle->cColor.blue;
				pVertices[i+3].a = prevparticle->cColor.alpha;
			}
			else {
				// Create a quad that works off the vrUp and vrPrevUp
				corners[0] = myEmitter->transform.position + vrPrevUp*particle->fSize;
				corners[2] = myEmitter->transform.position - vrPrevUp*particle->fSize;
				corners[1] = vPrevParticlePosition + vrPrevUp*prevparticle->fSize;
				corners[3] = vPrevParticlePosition - vrPrevUp*prevparticle->fSize;

				// Set vertice position information
				pVertices[i+0].x = corners[0].x;
				pVertices[i+0].y = corners[0].y;
				pVertices[i+0].z = corners[0].z;
				pVertices[i+0].u = 0;
				pVertices[i+0].v = 1;

				pVertices[i+2].x = corners[2].x;
				pVertices[i+2].y = corners[2].y;
				pVertices[i+2].z = corners[2].z;
				pVertices[i+2].u = 1;
				pVertices[i+2].v = 1;

				pVertices[i+1].x = corners[1].x;
				pVertices[i+1].y = corners[1].y;
				pVertices[i+1].z = corners[1].z;
				pVertices[i+1].u = 0;
				pVertices[i+1].v = prevparticle->fLife / prevparticle->fStartLife;

				pVertices[i+3].x = corners[3].x;
				pVertices[i+3].y = corners[3].y;
				pVertices[i+3].z = corners[3].z;
				pVertices[i+3].u = 1;
				pVertices[i+3].v = prevparticle->fLife / prevparticle->fStartLife;

				// Set the colors
				pVertices[i+0].r = prevparticle->cColor.red;
				pVertices[i+0].g = prevparticle->cColor.green;
				pVertices[i+0].b = prevparticle->cColor.blue;
				pVertices[i+0].a = prevparticle->cColor.alpha;

				pVertices[i+2].r = prevparticle->cColor.red;
				pVertices[i+2].g = prevparticle->cColor.green;
				pVertices[i+2].b = prevparticle->cColor.blue;
				pVertices[i+2].a = prevparticle->cColor.alpha;

				pVertices[i+1].r = prevparticle->cColor.red;
				pVertices[i+1].g = prevparticle->cColor.green;
				pVertices[i+1].b = prevparticle->cColor.blue;
				pVertices[i+1].a = prevparticle->cColor.alpha;

				pVertices[i+3].r = prevparticle->cColor.red;
				pVertices[i+3].g = prevparticle->cColor.green;
				pVertices[i+3].b = prevparticle->cColor.blue;
				pVertices[i+3].a = prevparticle->cColor.alpha;
			}

			// Set the triangles
			pTriangles[t+0].vert[0] = i+0;
			pTriangles[t+0].vert[1] = i+1;
			pTriangles[t+0].vert[2] = i+3;
			pTriangles[t+1].vert[0] = i+0;
			pTriangles[t+1].vert[1] = i+3;
			pTriangles[t+1].vert[2] = i+2;

			pTriangles[t+2].vert[0] = i+3;
			pTriangles[t+2].vert[1] = i+1;
			pTriangles[t+2].vert[2] = i+0;
			pTriangles[t+3].vert[0] = i+2;
			pTriangles[t+3].vert[1] = i+3;
			pTriangles[t+3].vert[2] = i+0;

			// Increment step
			i += 4;
			t += 4;
		}


		// Set output count
		out_vertCount = i;
		out_triCount = t;
	}
	else {
		out_vertCount = 0;
		out_triCount = 0;
	}
}

#include "core-ext/system/io/serializer/CBaseSerializer.h"
void	CParticleRenderer::serialize ( Serializer & ser, const uint ver )
{
	ser & iRenderMethod;
	ser & fR_SpeedScale;
	std::string dummy;
	ser & dummy;
	m_material->loadFromFile(dummy.c_str());
	//ser & (*(vMaterials[0])); 
	//ser & (*m_material); // TODO
}
