// The CWorldCartographer has routines to render one-shot maps of the target world.
// The cartographer assumes the proper realm has already been set.

#ifndef _C_WORLD_CARTOGRAPHER_H_
#define _C_WORLD_CARTOGRAPHER_H_

// Prototypes
class CRenderTexture;
class CMCCPlanet;
class CBitmapFont;
namespace Terrain
{
	class CWorldGenerator;
}

#include "core/math/Vector2d.h"
#include "core/types/ModelData.h"
#include "core/containers/arstring.h"

// Class
class CWorldCartographer
{
public:
	// Cartographer constructor.
	// Takes the planet's system name and pointer to the render target to render to.
	// Ownership of the render target is NOT transferred to the cartographer, but stays with the callee.
	explicit		CWorldCartographer ( const char* planetName, CRenderTexture* renderTarget );
					~CWorldCartographer ();

	void			ForceReload ( void );

	void			GiveGenerator ( Terrain::CWorldGenerator* );

public:
	struct sRenderProgressState
	{
		sRenderProgressState ( void ) : xindex(0), yindex(0) {;};
		int xindex;
		int yindex;
		uint32_t data [4096];
		bool done;
	};
public:
	//	RenderTerrainMap
	// Renders out a map with colors indicating the dominant terrain at the position.
	void			RenderTerrainMap ( void );
	// Renders out a terrain map a step at a time
	void			StutterRenderTerrainMap ( sRenderProgressState & );
	//	RenderRegionMap
	// Renders out a map showing delimited regions.
	void			RenderRegionMap ( void );
	// Renders out a region map a step at a time
	//void			StutterRenderRegionMap ( sRenderProgressState & );

	//	RenderAreaMap
	// Renders out a map that shows general terrain and province borders.
	void			RenderAreaMap ( void );


	// Renders out a world texture map a step at a time
	void			StutterRenderWorldTextureMap ( sRenderProgressState & );

public:
	// Rendered position
	Vector2d		r_pos;
	// Rendered area halfsize
	Vector2d		r_halfsize;
private:
	CRenderTexture*	m_rt;
	CMCCPlanet*		m_planet;
	arstring<256>	m_targetPlanet;

	Terrain::CWorldGenerator*	m_generator;

	// Vertex array
	//CTerrainVertex	verts [4096];
	CTerrainVertex*	verts;

	// Vertex detail specifier
	int	m_max_x;
	int m_max_y;

private:
	void			SetTerrainVertex ( CTerrainVertex & vert, const Vector2d& samplePoint );

	void			RenderVerts ( void );

private:
	CBitmapFont*	m_font;

};

#endif//_C_WOLRD_CARTOGRAPHER_H_