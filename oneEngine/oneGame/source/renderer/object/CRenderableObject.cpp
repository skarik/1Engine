#include "renderer/exceptions/exceptions.h"
#include "renderer/state/RrRenderer.h"
//#include "renderer/material/RrMaterial.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
//#include "renderer/system/glMainSystem.h"
#include "CRenderableObject.h"

#include "renderer/gpuw/Pipeline.h"

// ==Static Members
//bool CRenderableObject::bStaticMaterialsInit = false;
//RrMaterial* CRenderableObject::GLoutlineMaterial = NULL;
Vector3d CRenderableObject::_activeCameraPosition = Vector3d::zero;

// ==Constructor
//  adds render object to the list of RO in RrRenderer
//  sets visibility to true
CRenderableObject::CRenderableObject ( void )
	: m_material(NULL), m_vao_info(NULL), m_vao_count(0), m_vao_maxcount(0)
{
	// Set default render settings
	//renderSettings.fOutlineWidth = -1;
	// Set default layer mode
	/*for ( char i = 0; i < 16; ++i )
		renderSettings.layers[i] = false;
	renderSettings.layers[kRenderHintALL] = true;*/
	renderSettings.renderHints = kRenderHintALL;

	//InitMaterials();
	CRenderableObject::SetMaterial( RrMaterial::Default );

	renderLayer = renderer::kRLWorld;
	id = RrRenderer::Active->AddRO( this );
	visible = true;
}

// ==Destructor

//  removes render object from the list of RO in RrRenderer
CRenderableObject::~CRenderableObject ( void )
{
	// Remove the object from the list immediately
	RrRenderer::Active->RemoveRO( id );

	// Remove material reference
	SetMaterial( NULL );

	// Remove VAOs
	//PassinfoClear();
	//transform.RemoveReference();
	FreePipelines();
}

// == Private Setters ==

// Private ID 'safe' set
void CRenderableObject::SetId( unsigned int nId )
{
	id = nId;
}

// == Public Setters ==

//// Set the rendering type to change draw order and stuff
//void CRenderableObject::SetRenderType ( eRenderLayer newRenderType )
//{
//	renderLayer = newRenderType;
//}

// Set Materials
void CRenderableObject::SetMaterial ( RrMaterial* n_pNewMaterial )
{
	// Go through current materials, delete them if they have no more owner
	/*for ( vector<RrMaterial*>::iterator mat = vMaterials.begin(); mat != vMaterials.end(); mat++ )
	{
		if ((*mat)->canFree() )
		{
			delete (*mat);
		}
	}
	vMaterials.clear();*/
	/*ClearMaterialList();

	// Add new materials
	for ( int i = 0; i < iMaterialCount; i += 1 )
	{
		vMaterials.push_back( &(pMaterialToUse[i]) );
	}*/

	// Free old material usage
	if ( m_material )
	{
		m_material->removeReference();
		if ( !m_material->hasReference() )
		{
			delete m_material;
		}
#ifdef _ENGINE_DEBUG
		else if ( m_material != RrMaterial::Default && !m_material->isStatic() )
		{
			throw core::MemoryLeakException();
		}
#endif
	}

	m_material = n_pNewMaterial;
	// Add reference to new material
	if ( m_material )
	{
		m_material->addReference();
		// Check for a valid pass count
		if ( m_material->passinfo.size() > 16 || m_material->deferredinfo.size() > 16 ) {
			throw renderer::TooManyPassesException();
		}
		// Check all passes have a shader
		for ( uint pass = 0; pass < m_material->passinfo.size(); ++pass )
		{
			if ( m_material->passinfo[pass].shader == NULL ) {
				throw renderer::InvalidPassException();
			}
		}
		// Match up passes to the replacement system
		for ( uint layer = 0; layer < kRenderHintCOUNT; layer += 1 )
		{
			// Make sure this object renders to the layer
			if ( !(renderSettings.renderHints & (1<<layer)) ) {
				continue;
			}
			// Check that this material has a pass for this layer
			bool hasPass = false;
			for ( uint pass = 0; pass < m_material->passinfo.size(); ++pass )
			{
				if ( m_material->getPassForward(pass).m_hint & (1 << layer) )
				{
					hasPass = true;
					break;
				}
			}
			// If this material does have a pass for this layer, we want to actually cache that.
			if ( !hasPass ) // TODO: Search for ALL matching passes. (That actually won't work, but don't care right now.)
			{
				RrPassForward* pass = &(m_material->passinfo[0]);
				RrPassForward* source;
				if ( !pass ) continue;
				// Search for a matching pass
				if ( pass->m_transparency_mode != renderer::ALPHAMODE_TRANSLUCENT )
				{
					if ( !m_material->m_isSkinnedShader )
					{
						// Check for objects on the current layer.
						if ( renderer::m_default_hint_options->mats_default[layer] != NULL )
						{
							source = &(renderer::m_default_hint_options->mats_default[layer]->passinfo[0]);
							// Add a pass on the current layer
							m_material->passinfo.push_back( RrPassForward() );
							m_material->passinfo.back() = *source;
							std::swap( m_material->passinfo.back().shader, source->shader );
							m_material->passinfo.back().shader = new RrShader( source->shader->GetFilename() );
							m_material->passinfo.back().m_procedural = true;
							m_material->passinfo.back().m_hint = (1<<layer);
						}
					}
					else
					{
						// Check for objects on the current layer.
						if ( renderer::m_default_hint_options->mats_default_skin[layer] != NULL )
						{
							source = &(renderer::m_default_hint_options->mats_default_skin[layer]->passinfo[0]);
							// Add a pass on the current layer
							m_material->passinfo.push_back( RrPassForward() );
							m_material->passinfo.back() = *source;
							std::swap( m_material->passinfo.back().shader, source->shader );
							m_material->passinfo.back().shader = new RrShader( source->shader->GetFilename(), renderer::SHADER_TAG_SKINNING );
							m_material->passinfo.back().m_procedural = true;
							m_material->passinfo.back().m_hint = (1<<layer);
						}
					}
					/*if ( renderer::m_default_hint_options->mats_default[layer] != NULL )
					{
						// End
						// Add a pass on the current layer
						m_material->passinfo.push_back( RrPassForward() );
						source = &(m_material->passinfo[0]);
						m_material->passinfo.back() = *source;
						std::swap( m_material->passinfo.back().shader, source->shader );
						m_material->passinfo.back().shader = source->shader;
						source->shader->GrabReference();
						//m_material->passinfo.back().shader = new RrShader( source->shader->GetFilename() );
						m_material->passinfo.back().m_procedural = true;
						m_material->passinfo.back().m_hint = (1<<layer);
					}*/
				}
			}
			// End has pass
		}
		// End for loop

		// Clear pass info
		PassinfoRegenerate();
	}
}

void CRenderableObject::PassinfoClear ( void )
{
	// Delete all the vertex arrays
	for ( uint i = 0; i < m_vao_maxcount; ++i )
	{
		if ( m_vao_info[i] ) {
			glDeleteVertexArrays( 1, &(m_vao_info[i]) );
			m_vao_info[i] = 0;
		}
	}
	// Delete the array
	delete [] m_vao_info;
	m_vao_info = NULL;
	// Reset the counts
	m_vao_count = 0;
	m_vao_maxcount = 0;
}
void CRenderableObject::PassinfoGenerate ( void )
{
	// Set new VAO count
	m_vao_maxcount = m_material->getPassCountForward() + m_material->getPassCountDeferred();
	m_vao_count = m_vao_maxcount;
	// Set empty VAOs
	m_vao_info = new uint[m_vao_maxcount];
	for ( uint i = 0; i < m_vao_maxcount; ++i )
	{
		m_vao_info[i] = 0;
	}
}

void CRenderableObject::PassinfoRegenerate ( void )
{
	if ( m_vao_count == 0 ) {
		// If no VAOs, just generate
		PassinfoGenerate();
	}
	else {
		// If need more VAOs than we have
		uint t_targetcount = m_material->getPassCountForward() + m_material->getPassCountDeferred();
		if ( m_vao_maxcount < t_targetcount ) {
			// Clear and generate
			PassinfoClear();
			PassinfoGenerate();
		}
		else {
			// Otherwise, destroy all current VAOs
			for ( uint i = 0; i < m_vao_maxcount; ++i )
			{
				if ( m_vao_info[i] ) {
					glDeleteVertexArrays( 1, &(m_vao_info[i]) );
					m_vao_info[i] = 0;
				}
			}
			// And set new vao count
			m_vao_count = t_targetcount;
		}
	}
}

//===============================================================================================//
// Material system
//===============================================================================================//
// Get the number of passes (as models will need a lot more than one pass)
uchar CRenderableObject::GetPassNumber ( void ) {
	return m_material->getPassCount();
}
// Returns the associated pass. This is used for ordering.
RrPassForward* CRenderableObject::GetPass ( const uchar pass ) {
	return &(m_material->passinfo[pass]);
}
// Returns the associated deferred rendering pass. This is used for ordering.
RrPassDeferred* CRenderableObject::GetPassDeferred ( const uchar pass ) {
	return &(m_material->deferredinfo[pass]);
}

bool CRenderableObject::BindVAO ( const uchar pass, const uint vbo, const uint eab, const bool userDefinedAttribs )
{
	uint t_targetPass = pass;
	if ( RrRenderer::Active->GetRenderMode() == kRenderModeDeferred ) {
		t_targetPass += m_material->getPassCountForward();
	}
	if ( t_targetPass >= m_vao_maxcount ) {
		throw renderer::InvalidPassException();
	}
	if ( m_vao_info[t_targetPass] == 0 )
	{
		// If there's no VAO, generate it
		glGenVertexArrays( 1, &(m_vao_info[t_targetPass]) );
		glBindVertexArray( m_vao_info[t_targetPass] );
		// Bind target buffers
		glBindBuffer( GL_ARRAY_BUFFER, vbo );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, eab );
		if ( !userDefinedAttribs )
		{
			//m_material->bindPassAtrribs( t_targetPass );
			m_material->bindPassAtrribs();
		}
		GL_ACCESS GL.CheckError();
		return true;
	}
	else
	{
		// If there is a VAO, bind it
		glBindVertexArray( m_vao_info[t_targetPass] );
		GL_ACCESS GL.CheckError();
		return false;
	}
}

void CRenderableObject::PushCbufferPerObject ( const XrTransform& worldTransform )
{
	Matrix4x4 modelTRS, modelRS;
	core::TransformUtility::TRSToMatrix4x4(worldTransform, modelTRS, modelRS);

	//// Update matrix constants
	//pushConstantsPerObject(!modelTRS, !modelRS);
	modelTRS = !modelTRS;
	modelRS  = !modelRS;

	renderer::cbuffer::rrPerObjectMatrices matrices;
	matrices.modelTRS = modelTRS;
	matrices.modelRS  = modelRS;
	matrices.modelViewProjection = modelTRS * RrCamera::activeCamera->camera_VP;
	matrices.modelViewProjectionInverse = matrices.modelViewProjection.inverse();
}

//===============================================================================================//
// Render Status
//===============================================================================================//
float CRenderableObject::GetRenderDistance ( void )
{
	return renderDistance;
}