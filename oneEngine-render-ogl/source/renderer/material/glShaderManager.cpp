
#include "glShaderManager.h"

glShaderManager ShaderManager;

// REKOMPILE
void glShaderManager::RecompileAll ( void )
{
	for ( std::vector<glShader*>::iterator it = vShaderList.begin(); it != vShaderList.end(); it++ )
	{
		(*it)->recompile();
	}
}


void glShaderManager::AddShader ( glShader * pNewShader )
{
	/*glShaderReference newRef;
	newRef->shader = pNewShader;
	newRef->ref = 1;*/
	vShaderList.push_back( pNewShader );
}

bool glShaderManager::RemoveShader ( glShader * pOldShader )
{
	glShader* pTargetShader = pOldShader;
	if ( pOldShader->bIsReference )
		pTargetShader = pOldShader->pParentShader;

	for ( std::vector<glShader*>::iterator it = vShaderList.begin(); it != vShaderList.end(); it++ )
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

glShader* glShaderManager::ShaderExists ( const string& a_sShaderName, const GLE::shader_tag_t a_nShaderTag )
{
	for ( std::vector<glShader*>::iterator it = vShaderList.begin(); it != vShaderList.end(); it++ )
	{
		if ( ((*it)->stTag == a_nShaderTag) && ((*it)->sShaderFilename == a_sShaderName) )
		{
			return *it;
		}
	}
	for ( std::vector<glShader*>::iterator it = vShaderList.begin(); it != vShaderList.end(); it++ )
	{
		if ( (*it)->sShaderFilename.find( a_sShaderName ) != string::npos )
		{
			return *it;
		}
	}
	return NULL;
}