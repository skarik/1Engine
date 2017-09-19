
#ifndef _C_ISOSPHERE_H_
#define _C_ISOSPHERE_H_

// Includes
#include "core/math/Vector3d.h"
#include "core/types/ModelData.h"

#include "../CRenderableObject.h"
#include "renderer/material/RrMaterial.h"

#include <vector>

// Class definition
class CIsosphere : public CRenderableObject
{
public:
	// Constructor and Destructor
	CIsosphere ( void );
	~CIsosphere ( void );

	// Render function
	bool Render ( const char pass );
protected:
	float radius;
	int divisions;

	
	RrMaterial* myMat;
private:
	void normalize(float *a);
	void drawtri(float const *a, float const *b, float const *c, int div, float r);
	void drawsphere(int ndiv, float radius=1.0);
	//vector<arModelVertex> sphereData;
	arModelVertex* pSphereData;
	int size;
	void createtri(float const *a, float const *b, float const *c, int div, float r);
	inline void createvertex ( float const *a, float r );
	void createsphere ( int ndiv, float radius = 1.0 );

	static const float X;
	static const float Z;
	static const float vdata[12][3];
	static const float tdata[12][3];
	static const uint tindices[20][3];
};

#endif