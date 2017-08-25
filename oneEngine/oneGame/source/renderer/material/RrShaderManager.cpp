
#include "RrShaderManager.h"

RrShaderManager ShaderManager;

// REKOMPILE
void RrShaderManager::RecompileAll ( void )
{
	for ( std::vector<RrShader*>::iterator it = vShaderList.begin(); it != vShaderList.end(); it++ )
	{
		(*it)->recompile();
	}
}


void RrShaderManager::AddShader ( RrShader * pNewShader )
{
	/*glShaderReference newRef;
	newRef->shader = pNewShader;
	newRef->ref = 1;*/
	vShaderList.push_back( pNewShader );
}

bool RrShaderManager::RemoveShader ( RrShader * pOldShader )
{
	RrShader* pTargetShader = pOldShader;
	if ( pOldShader->bIsReference )
		pTargetShader = pOldShader->pParentShader;

	for ( std::vector<RrShader*>::iterator it = vShaderList.begin(); it != vShaderList.end(); it++ )
	{
		if ( (*it) == pTargetShader )
		{
			if ( (*it)->ReferenceCount() == 0 ) // If this happens, means there's no references, so we can delete :3
			{
				delete pTargetShader;
				vShaderList.erase( it );
				return true;
			}
			/*else
				(*it)->DecrementReference();*/
		}
	}
	return false;
}

RrShader* RrShaderManager::ShaderExists ( const std::string& a_sShaderName, const renderer::shader_tag_t a_nShaderTag )
{
	for ( std::vector<RrShader*>::iterator it = vShaderList.begin(); it != vShaderList.end(); it++ )
	{
		if ( ((*it)->stTag == a_nShaderTag) && ((*it)->sShaderFilename == a_sShaderName) )
		{
			return *it;
		}
	}
	for ( std::vector<RrShader*>::iterator it = vShaderList.begin(); it != vShaderList.end(); it++ )
	{
		if ( (*it)->sShaderFilename.find( a_sShaderName ) != std::string::npos )
		{
			return *it;
		}
	}
	return NULL;
}