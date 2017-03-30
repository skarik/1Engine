
#include "core/types/ModelData.h"

#include "CBillboard.h"
#include "renderer/camera/CCamera.h"

#include "renderer/system/glMainSystem.h"

//GLuint CBillboard::m_vbo	=0;

CBillboard::CBillboard ( void )
	: CRenderableObject(), m_size(1.0), m_curvetowards(false)
{
	//SetMaterial( &GLdefaultMaterial );
	m_vbo = 0;
}
CBillboard::~CBillboard ( void )
{
	GL_ACCESS;
	// Free any materials we now own
	//ClearMaterialList();
	if ( m_vbo != 0 ) {
		//glDeleteBuffers( 1, &m_vbo );
		GL.FreeBuffer( &m_vbo );
		m_vbo = 0;
	}
}

bool CBillboard::Render ( const char pass )
{
	GL_ACCESS;
	// Get the active camera and take its transform
	Vector3d lookPos (0,0,0);
	Vector3d vRight (0,1,0), vUp(0,0,1);
	if ( CCamera::activeCamera )
	{
		if ( m_curvetowards ) {
			lookPos = (CCamera::activeCamera->transform.position - transform.world.position).normal();
		}
		else {
			lookPos = CCamera::activeCamera->transform.rotation * Vector3d(1,0,0);
		}
		transform.world.rotation = Vector3d(0,0,0);
		vRight = CCamera::activeCamera->GetUp().cross( lookPos );
		vUp = lookPos.cross( vRight );
	}
	vRight *= m_size;
	vUp *= m_size;
	lookPos *= m_size;

	// Set transform data
	{
		transform.world.rotation.pData[0] = lookPos.x;
		transform.world.rotation.pData[3] = lookPos.y;
		transform.world.rotation.pData[6] = lookPos.z;

		transform.world.rotation.pData[1] = vRight.x;
		transform.world.rotation.pData[4] = vRight.y;
		transform.world.rotation.pData[7] = vRight.z;

		transform.world.rotation.pData[2] = vUp.x;
		transform.world.rotation.pData[5] = vUp.y;
		transform.world.rotation.pData[8] = vUp.z;

		transform.world.scale = Vector3d( 1,1,1 );
	}

	// Genereate the mesh if it doesn't exist
	if ( m_vbo == 0 )
	{
		glGenBuffers( 1, &m_vbo );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo );

		CModelVertex model [4];
		model[0].r = 1;		model[0].g = 1;		model[0].b = 1;		model[0].a = 1;
		model[0].x = 0;		model[0].y = -1;	model[0].z = +1;
		model[0].u = 0;		model[0].v = 0;

		model[1].r = 1;		model[1].g = 1;		model[1].b = 1;		model[1].a = 1;
		model[1].x = 0;		model[1].y = -1;	model[1].z = -1;
		model[1].u = 0;		model[1].v = 1;

		model[3].r = 1;		model[3].g = 1;		model[3].b = 1;		model[3].a = 1;
		model[3].x = 0;		model[3].y = +1;	model[3].z = -1;
		model[3].u = 1;		model[3].v = 1;

		model[2].r = 1;		model[2].g = 1;		model[2].b = 1;		model[2].a = 1;
		model[2].x = 0;		model[2].y = +1;	model[2].z = +1;
		model[2].u = 1;		model[2].v = 0;

		glBufferData( GL_ARRAY_BUFFER, sizeof(CModelVertex)*(4), model, GL_STATIC_DRAW );
	}


	GL.Transform( &transform.world );

	m_material->bindPass(pass);
	m_material->setShaderConstants(this);
	BindVAO( pass, m_vbo );
	glDisable( GL_CULL_FACE );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	GL.CheckError();

	/*glBindBuffer( GL_ARRAY_BUFFER, m_vbo );

	vMaterials[pass]->bind();
	vMaterials[pass]->setShaderConstants( this );
	vMaterials[pass]->forwardMeshAttributes();
	glDisable( GL_CULL_FACE );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	glEnable( GL_CULL_FACE );
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );*/

	return true;
}

// Set the material, clearing the list of materials
/*void CBillboard::SetMaterial ( RrMaterial* pNewMaterial )
{
	ClearMaterialList();
	vMaterials.push_back( pNewMaterial );
}

// Clear up the material list.
void CBillboard::ClearMaterialList ( void )
{
	for ( unsigned int i = 0; i < vMaterials.size(); i += 1 )
	{
		// If the material has no external owner, free it.
		if ( vMaterials[i]->canFree() )
			delete vMaterials[i];
	}
	vMaterials.clear();
}*/