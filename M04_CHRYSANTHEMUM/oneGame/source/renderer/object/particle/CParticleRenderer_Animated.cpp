
#include "CParticleRenderer_Animated.h"

#include "renderer/material/glMaterial.h"
#include "renderer/camera/CCamera.h"

CParticleRenderer_Animated::CParticleRenderer_Animated( CParticleEmitter* pInEmitter )
	: CParticleRenderer( pInEmitter )
{
	iFrameCount		= -1;
	iHorizontalDivs = 4;
	iVerticalDivs	= 4;

	fFramesPerSecond	= 10.0f;

	bStretchAnimationToLifetime = false;
	bClampToFrameCount			= false;
}

CParticleRenderer_Animated::~CParticleRenderer_Animated ( void )
{
	;
}

void CParticleRenderer_Animated::CreateMesh ( uint32_t& out_vertCount, uint32_t& out_triCount )
{
	if ( ((int)iHorizontalDivs) == 0 ) {
		//iHorizontalDivs = 1;
		out_vertCount = 0;
		out_triCount = 0;
		return;
	}
	if ( ((int)iVerticalDivs) == 0 ) {
		//iVerticalDivs = 1;
		out_vertCount = 0;
		out_triCount = 0;
		return;
	}

	// Draw properly aligned quads
	if ( iRenderMethod == P_FLAT_BILLBOARD_PRECISE )
	{
		// Set needed calculation values
		Vector3d temp;
		CTransform& camtrans = CCamera::activeCamera->transform;
		Quaternion cameraRot = camtrans.rotation.getQuaternion();

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

		Vector2d frameOffset;
		Vector2d frameSize ( 1.0f/iHorizontalDivs, 1.0f/iVerticalDivs );
		int maxFrameCount = (int)iFrameCount;
		if ( maxFrameCount <= 0 ) {
			maxFrameCount = (int)(iHorizontalDivs*iVerticalDivs);
		}

		uint32_t i = 0;
		uint32_t t = 0;
		int frame;
		// Loop through all the particles and add to the list
		for ( std::vector<CParticle>::iterator it = myEmitter->vParticles.begin(); it != myEmitter->vParticles.end(); ++it )
		{
			// Get corner positions
			particleRot.AxisAngle( vCamForward, it->fAngle );
			vrRight = (particleRot*vRight) *it->fSize;
			vrUp = (particleRot*vUp) *it->fSize;
			corners[3] = (-vrRight+vrUp);
			corners[2] = (-vrRight-vrUp);
			corners[1] = (vrRight-vrUp);
			corners[0] = (vrRight+vrUp);
			corners[4] = -vCamForward;

			// Set the colors and normals
			temp = cameraRot*(-corners[4]);
			for ( int j = 0; j < 4; ++j ) {
				pVertices[i+j].r = it->cColor.red;
				pVertices[i+j].g = it->cColor.green;
				pVertices[i+j].b = it->cColor.blue;
				pVertices[i+j].a = it->cColor.alpha;
				pVertices[i+j].nx = temp.x;
				pVertices[i+j].ny = temp.y;
				pVertices[i+j].nz = temp.z;
			}

			// Calculate the frame
			if ( !bStretchAnimationToLifetime ) {
				frame = (int)((it->fStartLife-it->fLife)*fFramesPerSecond);
			}
			else {
				frame = (int)(maxFrameCount*(1-(it->fLife/it->fStartLife)));
			}
			if ( bClampToFrameCount ) {
				if ( frame >= maxFrameCount ) {
					frame = maxFrameCount-1;
				}
			}
			// Calculate the frame offset
			frameOffset.x = (int(frame) % int(iHorizontalDivs)) / (ftype)(iHorizontalDivs);
			frameOffset.y = (int(frame) / int(iVerticalDivs)) / (ftype)(iVerticalDivs);

			// Set the positions and texture coordinates
			temp = it->vPosition+corners[0];
			pVertices[i+0].x = temp.x;
			pVertices[i+0].y = temp.y;
			pVertices[i+0].z = temp.z;
			pVertices[i+0].u = frameOffset.x;
			pVertices[i+0].v = frameOffset.y;

			temp = it->vPosition+corners[1];
			pVertices[i+1].x = temp.x;
			pVertices[i+1].y = temp.y;
			pVertices[i+1].z = temp.z;
			pVertices[i+1].u = frameOffset.x;
			pVertices[i+1].v = frameOffset.y+frameSize.y;

			temp = it->vPosition+corners[2];
			pVertices[i+2].x = temp.x;
			pVertices[i+2].y = temp.y;
			pVertices[i+2].z = temp.z;
			pVertices[i+2].u = frameOffset.x+frameSize.x;
			pVertices[i+2].v = frameOffset.y+frameSize.y;

			temp = it->vPosition+corners[3];
			pVertices[i+3].x = temp.x;
			pVertices[i+3].y = temp.y;
			pVertices[i+3].z = temp.z;
			pVertices[i+3].u = frameOffset.x+frameSize.x;
			pVertices[i+3].v = frameOffset.y;

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
		out_vertCount = myEmitter->vParticles.size() * 4;
		out_triCount = myEmitter->vParticles.size() * 2;
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

		Vector2d frameOffset;
		Vector2d frameSize ( 1.0f/iHorizontalDivs, 1.0f/iVerticalDivs );
		int maxFrameCount = (int)iFrameCount;
		if ( maxFrameCount <= 0 ) {
			maxFrameCount = (int)(iHorizontalDivs*iVerticalDivs);
		}

		uint32_t i = 0;
		uint32_t t = 0;
		int frame;
		// Loop through all the particles and add to the list
		for ( std::vector<CParticle>::iterator it = myEmitter->vParticles.begin(); it != myEmitter->vParticles.end(); ++it )
		{
			// Get corner positions// Generate true position
			vParticlePosition = (myEmitter->bSimulateInWorldspace) ? it->vPosition :( it->vPosition + myEmitter->transform.position );

			vrRight = it->vVelocity * fR_SpeedScale;
			if ( vrRight.sqrMagnitude() < sqr(it->fSize*0.5f) ) {
				vrRight = vrRight.normal() * it->fSize*0.5f;
			}
			vrUp = vCamForward.cross( vrRight.normal() );
			if ( vrUp.sqrMagnitude() < 0.01f ) {
				particleRot.AxisAngle( vCamForward, it->fAngle );
				vrUp = (particleRot*vRight);
			}
			vrUp *= it->fSize;

			corners[3] = (-vrRight+vrUp);
			corners[2] = (-vrRight-vrUp);
			corners[1] = (vrRight-vrUp);
			corners[0] = (vrRight+vrUp);
			corners[4] = -vCamForward;

			// Set the colors and normals
			temp = cameraRot*(-corners[4]);
			for ( int j = 0; j < 4; ++j ) {
				pVertices[i+j].r = it->cColor.red;
				pVertices[i+j].g = it->cColor.green;
				pVertices[i+j].b = it->cColor.blue;
				pVertices[i+j].a = it->cColor.alpha;
				pVertices[i+j].nx = temp.x;
				pVertices[i+j].ny = temp.y;
				pVertices[i+j].nz = temp.z;
			}

			// Calculate the frame
			if ( !bStretchAnimationToLifetime ) {
				frame = (int)((it->fStartLife-it->fLife)*fFramesPerSecond);
			}
			else {
				frame = (int)(maxFrameCount*(1-(it->fLife/it->fStartLife)));
			}
			if ( bClampToFrameCount ) {
				if ( frame >= maxFrameCount ) {
					frame = maxFrameCount-1;
				}
			}
			// Calculate the frame offset
			frameOffset.x = (int(frame) % int(iHorizontalDivs)) / (ftype)(iHorizontalDivs);
			frameOffset.y = (int(frame) / int(iVerticalDivs)) / (ftype)(iVerticalDivs);

			// Set the positions and texture coordinates
			temp = vParticlePosition+corners[0];
			pVertices[i+0].x = temp.x;
			pVertices[i+0].y = temp.y;
			pVertices[i+0].z = temp.z;
			pVertices[i+0].u = frameOffset.x;
			pVertices[i+0].v = frameOffset.y;

			temp = vParticlePosition+corners[1];
			pVertices[i+1].x = temp.x;
			pVertices[i+1].y = temp.y;
			pVertices[i+1].z = temp.z;
			pVertices[i+1].u = frameOffset.x;
			pVertices[i+1].v = frameOffset.y + frameSize.y;

			temp = vParticlePosition+corners[2];
			pVertices[i+2].x = temp.x;
			pVertices[i+2].y = temp.y;
			pVertices[i+2].z = temp.z;
			pVertices[i+2].u = frameOffset.x + frameSize.x;
			pVertices[i+2].v = frameOffset.y + frameSize.y;

			temp = vParticlePosition+corners[3];
			pVertices[i+3].x = temp.x;
			pVertices[i+3].y = temp.y;
			pVertices[i+3].z = temp.z;
			pVertices[i+3].u = frameOffset.x + frameSize.x;
			pVertices[i+3].v = frameOffset.y;

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
		out_vertCount = myEmitter->vParticles.size() * 4;
		out_triCount = myEmitter->vParticles.size() * 4;
	}
	else {
		out_vertCount = 0;
		out_triCount = 0;
	}
}

#include "core-ext/system/io/serializer/CBaseSerializer.h"
void	CParticleRenderer_Animated::serialize ( Serializer & ser, const uint ver )
{
	CParticleRenderer::serialize( ser, ver );
	ser & iFrameCount;
	ser & iHorizontalDivs;
	ser & iVerticalDivs;
	ser & fFramesPerSecond;
	ser & bStretchAnimationToLifetime;
	ser & bClampToFrameCount;
}
