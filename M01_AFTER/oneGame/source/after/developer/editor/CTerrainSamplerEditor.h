
#ifndef _C_TERRAIN_SAMPLER_EDITOR_H_
#define _C_TERRAIN_SAMPLER_EDITOR_H_

// Includes
#include "engine/behavior/CGameBehavior.h"

#include "renderer/object/CRenderableObject.h"

// boom
class CTerrainSamplerEditor : public CGameBehavior, public CRenderableObject
{

public:
	CTerrainSamplerEditor ( void );
	~CTerrainSamplerEditor ( void );

	void Update ( void );
	bool Render ( const char pass );

protected:

	enum EEditorMode
	{
		EM_TerrainType = 0,
		EM_BiomeType
	};

	EEditorMode		eCurrentEditor;
	string			sTargetFile;
	unsigned char*	pSampler;

	glMaterial*	myMaterial;

	void GenerateDefault_Terrain ( void );
	void GenerateDefault_Biome ( void );
	void SaveToPPM ( void );
	void LoadFromPPM ( void );

};

#endif