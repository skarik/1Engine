
// Includes
#include "CIsosphere.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/camera/CCamera.h"
#include <math.h>

//==Constants==
const float CIsosphere::X = .525731112119133606f;
const float CIsosphere::Z = .850650808352039932f;
const float CIsosphere::vdata[12][3] = {    
    {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
    {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
    {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
};
const uint CIsosphere::tindices[20][3] = { 
    {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
    {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
    {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
    {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11}
};

//==Constructor and Destructor==
CIsosphere::CIsosphere ( void )
	: CRenderableObject ()
{
	renderType = renderer::World;
	divisions = 1;
	radius = -1.0f;

	/*myMat = new RrMaterial;
	//myMat->loadTexture( "textures\\test1.tga" );
	myMat->loadTexture( "textures\\starmap.jpg" );
	//myMat->useTexture = true;
	myMat->useLighting = false;*/

	//myMat = &GLdefaultMaterial;

	/*size = 0;
	pSphereData = NULL;
	createsphere( divisions, -radius );*/
}
CIsosphere::~CIsosphere ( void )
{
	if ( pSphereData != NULL )
		delete [] pSphereData;
}
#include "core/time/time.h"
bool CIsosphere::Render ( const char pass )
{
	GL_ACCESS
	//transform.rotation.y += Time::deltaTime * 2.0f;

	//myMat->bind();
	m_material->bindPass(0);

	transform.world.position = CCamera::activeCamera->transform.position;
	GL.Transform( &transform.world );
	//drawList();
	drawsphere( divisions, -radius );

	//myMat->unbind();

	return true;
}

void CIsosphere::drawList ( void )
{
	GLd_ACCESS
	GLd.BeginPrimitive(GL_TRIANGLES);
	/*for ( unsigned int i = 0; i < sphereData.size(); i += 1 )
	{
		glTexCoord3fv( &(sphereData[i].tx) );
		glNormal3fv( &(sphereData[i].nx) );
		glVertex3fv( &(sphereData[i].x) );
	}*/
	/*for ( vector<CModelVertex>::iterator itr = sphereData.begin(); itr != sphereData.end(); itr++ )
	{
		glTexCoord3fv( &(itr->tx) );
		glNormal3fv( &(itr->nx) );
		glVertex3fv( &(itr->x) );
	}*/
	/*for ( unsigned int i = 0; i < size; i += 1 )
	{
		glTexCoord3fv( &(pSphereData[i].tx) );
		glNormal3fv( &(pSphereData[i].nx) );
		glVertex3fv( &(pSphereData[i].x) );
	}*/
	GLd.EndPrimitive();
}

//http://www.3dbuzz.com/vbforum/showthread.php?118279-Quick-solution-for-making-a-sphere-in-OpenGL
//redbook ch2?
void CIsosphere::normalize(float *a) {
    float d=sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
    a[0]/=d; a[1]/=d; a[2]/=d;
}
void CIsosphere::drawtri(float const *a, float const *b, float const *c, int div, float r) {
	GLd_ACCESS
    if (div<=0) {
		float ay = atan2(a[2],a[0])/(float)PI;
		float by = atan2(b[2],b[0])/(float)PI;
		float cy = atan2(c[2],c[0])/(float)PI;

		if ( ay > 0.8f )
		{
			if ( by < 0.2f )
				by += 1.0f;
			if ( cy < 0.2f )
				cy += 1.0f;
		}
		if ( by > 0.8f )
		{
			if ( ay < 0.2f )
				ay += 1.0f;
			if ( cy < 0.2f )
				cy += 1.0f;
		}
		if ( cy > 0.8f )
		{
			if ( ay < 0.2f )
				ay += 1.0f;
			if ( by < 0.2f )
				by += 1.0f;
		}
		
		GLd.P_PushTexcoord( (a[1]+1)*0.5f, ay ); GLd.P_PushNormal(a); GLd.P_AddVertex(a[0]*r, a[1]*r, a[2]*r);
        GLd.P_PushTexcoord( (b[1]+1)*0.5f, by ); GLd.P_PushNormal(b); GLd.P_AddVertex(b[0]*r, b[1]*r, b[2]*r);
        GLd.P_PushTexcoord( (c[1]+1)*0.5f, cy ); GLd.P_PushNormal(c); GLd.P_AddVertex(c[0]*r, c[1]*r, c[2]*r);
    } else {
        float ab[3], ac[3], bc[3];
        for (int i=0;i<3;i++) {
            ab[i]=(a[i]+b[i])/2;
            ac[i]=(a[i]+c[i])/2;
            bc[i]=(b[i]+c[i])/2;
        }
        normalize(ab); normalize(ac); normalize(bc);
        drawtri(a, ab, ac, div-1, r);
        drawtri(b, bc, ab, div-1, r);
        drawtri(c, ac, bc, div-1, r);
        drawtri(ab, bc, ac, div-1, r);  //<--Comment this line and sphere looks really cool!
    }  
}
void CIsosphere::drawsphere(int ndiv, float radius) {
	GLd_ACCESS
    GLd.BeginPrimitive(GL_TRIANGLES);
    for (int i=0;i<20;i++)
        drawtri(
			vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]],
			ndiv, radius);
    GLd.EndPrimitive();
}

void CIsosphere::createtri ( float const *a, float const *b, float const *c, int div, float r )
{
	if (div<=0) {
		createvertex( a,r );
		createvertex( b,r );
		createvertex( c,r );
    } else {
        float ab[3], ac[3], bc[3];
        for (int i=0;i<3;i++) {
            ab[i]=(a[i]+b[i])/2;
            ac[i]=(a[i]+c[i])/2;
            bc[i]=(b[i]+c[i])/2;
        }
        normalize(ab); normalize(ac); normalize(bc);
        createtri(a, ab, ac, div-1, r);
        createtri(b, bc, ab, div-1, r);
        createtri(c, ac, bc, div-1, r);
        createtri(ab, bc, ac, div-1, r);  //<--Comment this line and sphere looks really cool!
    } 
}
void CIsosphere::createvertex ( float const *a, float r )
{
	size += 1;
	CModelVertex* pNewData = new CModelVertex [size];
	for ( int i = 0; i < size-1; i += 1 )
	{
		pNewData[i] = pSphereData[i];
	}
	
	CModelVertex newVertex;
	newVertex.x = a[0]*r;
	newVertex.y = a[1]*r;
	newVertex.z = a[2]*r;
	newVertex.nx = a[0];
	newVertex.ny = a[1];
	newVertex.nz = a[2];
	newVertex.tx = (a[1]+1)*0.5f;
	newVertex.ty = (float)(atan2(a[2],a[0])/PI);

	pNewData[size-1] = newVertex;
	if ( pSphereData != NULL )
		delete [] pSphereData;

	pSphereData = pNewData;
	//sphereData.push_back( newVertex );
}
void CIsosphere::createsphere ( int ndiv, float radius )
{
	//sphereData.clear();
	if ( pSphereData != NULL )
		delete [] pSphereData;
	for (int i=0;i<20;i++)
        createtri(vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]],ndiv, radius);
}