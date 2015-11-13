
#include "CFoliageTree.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"

// Constructor
CFoliageTree::CFoliageTree ( void )
	: CFoliage ( )
{
	pTreeModel = new CModel ( string("models/tree01.FBX") );
	pTreeModel->transform.scale = Vector3d( 1,1,1 ) * 0.2f;

	glMaterial* treeMaterial = new glMaterial;
	treeMaterial->m_diffuse = Color( 0.7f,0.4f,0.2f ) * 0.6f;
	treeMaterial->m_diffuse.alpha = 1.0f;
	treeMaterial->m_emissive = Color( 0,0,0.0f );
	treeMaterial->setTexture( 0, new CTexture("textures/terraTexture.jpg") );
	pTreeModel->SetMaterial( treeMaterial );
	treeMaterial->removeReference();
}

// Destructor
CFoliageTree::~CFoliageTree ( void )
{
	delete pTreeModel;
}

// Position
void CFoliageTree::Update ( void )
{
	pTreeModel->transform.Get( transform ); 
}