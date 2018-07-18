#ifndef RENDERER_SHADER_PROGRAM_H_
#define RENDERER_SHADER_PROGRAM_H_

struct rrShaderProgramVsPs
{
	const char*			file_vv;
	const char*			file_p;
};

struct rrShaderProgramVsGsPs
{
	const char*			file_vv;
	const char*			file_g;
	const char*			file_p;
};

struct rrShaderProgramVsHsDsPs
{
	const char*			file_vv;
	const char*			file_h;
	const char*			file_d;
	const char*			file_p;
};

struct rrShaderProgramVsHsDsGsPs
{
	const char*			file_vv;
	const char*			file_h;
	const char*			file_d;
	const char*			file_g;
	const char*			file_p;
};

class RrShaderProgram
{
public:
	static RrShaderProgram*	Load ( const rrShaderProgramVsPs& params );
	static RrShaderProgram*	Load ( const rrShaderProgramVsGsPs& params );
	static RrShaderProgram*	Load ( const rrShaderProgramVsHsDsPs& params );
	static RrShaderProgram*	Load ( const rrShaderProgramVsHsDsGsPs& params );
};

#endif//RENDERER_SHADER_PROGRAM_H_