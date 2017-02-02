
// == Includes ==
#include "core/settings/CGameSettings.h"
#include "renderer/exceptions/exceptions.h"
#include "renderer/state/CRenderState.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "CRenderableObject.h"

// ==Static Members
//bool CRenderableObject::bStaticMaterialsInit = false;
//glMaterial* CRenderableObject::GLoutlineMaterial = NULL;
Vector3d CRenderableObject::_activeCameraPosition = Vector3d::zero;

// ==Constructor
//  adds render object to the list of RO in CRenderState
//  sets visibility to true
CRenderableObject::CRenderableObject ( void )
	: m_material(NULL), m_vao_info(NULL), m_vao_count(0), m_vao_maxcount(0)
{
	transform.owner = this;
	transform.ownerType = Transform::TYPE_RENDERER_OBJECT;
	transform.name = "Transform(Renderable)";
	//transform.name = this->name; // TODO: ADD NAME

	// Set default render settings
	//renderSettings.fOutlineWidth = -1;
	// Set default layer mode
	/*for ( char i = 0; i < 16; ++i )
		renderSettings.layers[i] = false;
	renderSettings.layers[RL_ALL] = true;*/
	renderSettings.renderHints = RL_ALL;

	//InitMaterials();
	CRenderableObject::SetMaterial( glMaterial::Default );

	renderType = Renderer::World;
	id = CRenderState::Active->AddRO( this );
	visible = true;
}
// ==Destructor
//  removes render object from the list of RO in CRenderState
CRenderableObject::~CRenderableObject ( void )
{
	// Remove material reference
	SetMaterial( NULL );
	// Remove VAOs
	PassinfoClear();
	//transform.RemoveReference();
	CRenderState::Active->RemoveRO( id );
}

// == Private Setters ==
// Private ID 'safe' set
void CRenderableObject::SetId( unsigned int nId )
{
	id = nId;
}

// == Public Setters ==
// Set the rendering type to change draw order and stuff
void CRenderableObject::SetRenderType ( RenderingType newRenderType )
{
	renderType = newRenderType;
}
// Set Materials
void CRenderableObject::SetMaterial ( glMaterial* n_pNewMaterial )
{
	// Go through current materials, delete them if they have no more owner
	/*for ( vector<glMaterial*>::iterator mat = vMaterials.begin(); mat != vMaterials.end(); mat++ )
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
		else if ( m_material != glMaterial::Default && !m_material->isStatic() )
		{
			throw Core::MemoryLeakException();
		}
#endif
	}

	m_material = n_pNewMaterial;
	// Add reference to new material
	if ( m_material )
	{
		m_material->addReference();
		//if ( CGameSettings::Active()->i_ro_RendererMode /*== RENDER_MODE_FORWARD*/ )
		//{
		// Check for a valid pass count
		if ( m_material->passinfo.size() > 16 || m_material->deferredinfo.size() > 16 ) {
			throw Renderer::TooManyPassesException();
		}
		// Check all passes have a shader
		for ( uint pass = 0; pass < m_material->passinfo.size(); ++pass )
		{
			if ( m_material->passinfo[pass].shader == NULL ) {
				throw Renderer::InvalidPassException();
			}
		}
		// Match up passes to the replacement system
		for ( uint layer = 0; layer < RL_LAYER_COUNT; layer += 1 )
		{
			// Make sure this object renders to the layer
			if ( !(renderSettings.renderHints & (1<<layer)) ) {
				continue;
			}
			// Check that this material has a pass for this layer
			bool hasPass = false;
			for ( uint pass = 0; pass < m_material->passinfo.size(); ++pass )
			{
				if ( m_material->passinfo[pass].m_hint & (1<<layer) ) {
					hasPass = true;
				}
			}
			if ( !hasPass ) // TODO: Search for ALL matching passes. (That actually won't work, but don't care right now.)
			{
				glPass* pass = &(m_material->passinfo[0]);
				glPass* source;
				if ( !pass ) continue;
				// Search for a matching pass
				if ( pass->m_transparency_mode != Renderer::ALPHAMODE_TRANSLUCENT )
				{
					if ( !m_material->m_isSkinnedShader )
					{
						// Check for objects on the current layer.
						if ( Renderer::m_default_hint_options->mats_default[layer] != NULL )
						{
							source = &(Renderer::m_default_hint_options->mats_default[layer]->passinfo[0]);
							// Add a pass on the current layer
							m_material->passinfo.push_back( glPass() );
							m_material->passinfo.back() = *source;
							std::swap( m_material->passinfo.back().shader, source->shader );
							m_material->passinfo.back().shader = new glShader( source->shader->GetFilename() );
							m_material->passinfo.back().m_procedural = true;
							m_material->passinfo.back().m_hint = (1<<layer);
						}
					}
					else
					{
						// Check for objects on the current layer.
						if ( Renderer::m_default_hint_options->mats_default_skin[layer] != NULL )
						{
							source = &(Renderer::m_default_hint_options->mats_default_skin[layer]->passinfo[0]);
							// Add a pass on the current layer
							m_material->passinfo.push_back( glPass() );
							m_material->passinfo.back() = *source;
							std::swap( m_material->passinfo.back().shader, source->shader );
							m_material->passinfo.back().shader = new glShader( source->shader->GetFilename(), GLE::SHADER_TAG_SKINNING );
							m_material->passinfo.back().m_procedural = true;
							m_material->passinfo.back().m_hint = (1<<layer);
						}
					}
					/*if ( Renderer::m_default_hint_options->mats_default[layer] != NULL )
					{
						// End
						// Add a pass on the current layer
						m_material->passinfo.push_back( glPass() );
						source = &(m_material->passinfo[0]);
						m_material->passinfo.back() = *source;
						std::swap( m_material->passinfo.back().shader, source->shader );
						m_material->passinfo.back().shader = source->shader;
						source->shader->GrabReference();
						//m_material->passinfo.back().shader = new glShader( source->shader->GetFilename() );
						m_material->passinfo.back().m_procedural = true;
						m_material->passinfo.back().m_hint = (1<<layer);
					}*/
				}
			}
			// End has pass
		}
		// End for loop
		//}
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
glPass* CRenderableObject::GetPass ( const uchar pass ) {
	return &(m_material->passinfo[pass]);
}
// Returns the associated deferred rendering pass. This is used for ordering.
glPass_Deferred* CRenderableObject::GetPassDeferred ( const uchar pass ) {
	return &(m_material->deferredinfo[pass]);
}

bool CRenderableObject::BindVAO ( const uchar pass, const uint vbo, const uint eab, const bool userDefinedAttribs )
{
	uint t_targetPass = pass;
	if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_DEFERRED ) {
		t_targetPass += m_material->getPassCountForward();
	}
	if ( t_targetPass >= m_vao_maxcount ) {
		throw Renderer::InvalidPassException();
	}
	if ( m_vao_info[t_targetPass] == 0 )
	{
//#ifdef _ENGINE_DEBUG
//		std::cout << "generating VAO for " << this << " on pass " << ((int)pass) << std::endl;
//#endif//_ENGINE_DEBUG
		// If there's no VAO, generate it
		glGenVertexArrays( 1, &(m_vao_info[t_targetPass]) );
		glBindVertexArray( m_vao_info[t_targetPass] );
		// Bind target buffers
		glBindBuffer( GL_ARRAY_BUFFER, vbo );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, eab );
		if ( !userDefinedAttribs ) {
			m_material->bindPassAtrribs( t_targetPass );
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

//===============================================================================================//
// Render Status
//===============================================================================================//
float CRenderableObject::GetRenderDistance ( void )
{
	return renderDistance;
}