
#ifndef _GL_SHADER_MANAGER_
#define _GL_SHADER_MANAGER_

#include <vector>
//using std::vector;
#include "RrShader.h"

class RrShaderManager 
{
	//static RrShader* pShaderList;
public:
	RENDER_API void RecompileAll ( void );
	RENDER_API void InvalidateAll ( void );

	void AddShader ( RrShader* );
	bool RemoveShader ( RrShader* );
	RrShader* ShaderExists ( const std::string& a_sShaderName, const renderer::rrShaderTag a_nShaderTag );
private:
	std::vector<RrShader*> vShaderList;
};

// Global Class
RENDER_API extern RrShaderManager ShaderManager;

#endif