// 
// glMaterial.h
// 
// Definition for a single rendering pass.
// 

// Can contain more than one texture

#ifndef _GL_MATERIAL_H_
#define _GL_MATERIAL_H_

// Includes
#include "CTexture.h"
#include "CRenderTexture.h"
#include "glShader.h"
#include "glMainSystem.h"
#include "Color.h"
#include "arstring.h"

#include <string>
using std::string;
#include <vector>
using std::vector;

// Prototype
class CRenderableObject;

// Class Definition
class glMaterial
{
public:
	enum EDrawBlendMode
	{
		BM_NORMAL = 0,
		BM_ADD,
		BM_SUBTRACT,
		BM_SOFT_ADD
	};
	enum EDrawLighting
	{
		LI_NORMAL = 0,
		LI_SKIN,
		LI_CELSHADED
	};
	enum EBindMode
	{
		BN_DEFAULT = 0,
		BN_DEPTH,
		BN_WARP
	};
	enum EFaceMode
	{
		FM_FRONT = 0,
		FM_BACK,
		FM_FRONTANDBACK
	};
public:
	glMaterial ( void );
	~glMaterial ( void );

	// == Copy material ==
	glMaterial& operator= ( glMaterial const* );
	glMaterial& operator= ( glMaterial const& );
	glMaterial* Copy ( void ) const;

	// == Load from file ==
	void loadFromFile ( const string& );
	static bool isValidFile ( const string& );

	// == Material Usage ==
	void bind ( void );
	void unbind ( void );

	// == Texture Setting == 
	void loadTexture ( const string& );
	void setTexture ( CTexture* );

	void bindTexture ( CTexture*, string const& );
	void bindDepth ( CRenderTexture*, string const& );
	void bindTextureBuffer ( GLuint, string const& );

	// == Shader Setting ==
	void setShader ( glShader* );

	// == Shader Constants ==
	void setShaderConstants ( CRenderableObject* );

	void setUniform ( string const&, float const );
	void setUniform ( string const&, Vector2d const& );
	void setUniform ( string const&, Vector3d const& );
	void setUniform ( string const&, Color const& );
	void setUniform ( string const&, Matrix4x4 const& );
	void setUniformV ( string const&, unsigned int, const Matrix4x4* );
	void setUniformV ( string const&, unsigned int, const Matrix3x3* );
	void setUniformV ( string const&, unsigned int, const Vector3d* );

	void setShaderAttributesDefault ( void );

	void setAttribute ( string const&, uint, GLsizei, const void* );

	/*template<class type>
	struct UniformEntry {
		string	id;
		type	value;
	};*/
	//vector<UniformEntry<float>> uniformQueueFloats;
	//unordered_map<
	//unordered_map<int,UniformEntry<float>> uniformMapFloats;

	// == Memory Management ==
	// Release ownership
	void releaseOwnership ( void ) {
		bSaveMe = false;
	};
	// Check ownership
	bool canFree ( void ) {
		return !bSaveMe;
	}

	// == Model Management ==
	void initializeMeshAttributes ( void );
	bool hasInitializedMeshAttributes ( void ) const;
	void forwardMeshAttributes ( void );

	void initializeParticleMeshAttributes ( void );
	void forwardParticleMeshAttributes ( bool initialize=false );

private:
	void bind_default ( void );
	void bind_shader ( void );

	inline void calculateRenderQueue ( void );

	bool	m_initialized_mesh;

public:
	bool	useTexture;
	bool	useColors;
	bool	useBlendedDiffuse;
	bool	useVertexColors; // unused
	bool	useLighting;
	bool	useSpecular;

	//bool	writeZbuffer;
	bool	isTransparent;
	bool	useTwoPassAlpha;
	bool	useDepthMask;
	bool	useAlphaTest;
	//bool	writeZbuffer;
	bool	useSkinning;

	bool	isScreenShader;

	Color	diffuse;
	Color	emissive;

	EDrawBlendMode	iBlendMode;
	EDrawLighting	iLightingMode;
	EBindMode		iBindMode;
	EFaceMode		iFaceMode;

	glShader* pShader;
	CTexture** pTextures;

	unsigned short	iRenderQueue;
	//bool			bNeedsReorder;

	static vector<glMaterial*>	materialList;
	string	sMaterialFilename;
	arstring<128>	sDrawHint;

private:
	bool	bSaveMe;
	short	iRefNumber;
	unsigned short	iRenderQueuePrev;

	static short	iCurrentTextureCount;
};

// Set Default Materials
extern glMaterial GLdefaultMaterial;
extern glMaterial* pGLcurrentMaterial;

#endif