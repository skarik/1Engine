#ifndef RENDERER_STATE_PIPELINE_MODES_H_
#define RENDERER_STATE_PIPELINE_MODES_H_

#include "core/types.h"

namespace renderer
{
	//	renderer::PipelineMode - Rendering mode to use.
	// A pipeline often has a different rendering object handling instance associated with it.
	enum class PipelineMode : uint8
	{
		// Default rendering pipeline.
		kNormal,

		// Paletted rendering pipeline.
		// TODO: Where does the palette get input? Each object needs either a gradient map or a LUT. The final output also requires a LUT.
		kPaletted,

		// Pulls information from the attached audio engine.
		// The entirety of the audio list is shoved into the light list.
		// Albedo is dropped. Instead, approximated sound reflections are rendered.
		/*kPipelineModeEcho,

		// Imagination mode. Doesn't actually render.
		kPipelineModeAether,

		// Insprited by Studio SHAFT animation style (particularly Nisemonogatari)
		// Mostly the same as kPipelineModeNormal, however:
		//	* Shadows are fucking dithered
		//	* Sometimes the color palette changes
		//	* Sometimes the scene is flatshaded
		kPipelineModeShaft,*/

		// Default 2D rendering pipeline, with orthographic optimizations.
		// Requires 2D extension to function properly.
		//kPipelineMode2DPaletted 
	};
}

#endif//RENDERER_STATE_PIPELINE_MODES_H_