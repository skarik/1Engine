
#ifndef _GL_SHADER_MANAGER_
#define _GL_SHADER_MANAGER_

#include <vector>
//using std::vector;
#include "glShader.h"

class glShaderManager 
{
	//static glShader* pShaderList;
public:
	void RecompileAll ( void );
	void AddShader ( glShader* );
	bool RemoveShader ( glShader* );
	glShader* ShaderExists ( const string& a_sShaderName, const GLE::shader_tag_t a_nShaderTag );
private:
	std::vector<glShader*> vShaderList;
};

// Global Class
extern glShaderManager ShaderManager;

#endif