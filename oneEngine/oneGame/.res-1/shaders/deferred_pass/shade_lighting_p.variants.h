#pragma once
#include "renderer/material/ShaderVariations.h"

RR_SHADER_VARIANT_BEGIN(shade_lighting_p)
	
	static constexpr int VARIANT_PASS_DEBUG_GBUFFERS = 10;
	static constexpr int VARIANT_PASS_DEBUG_SURFACE = 11;
	static constexpr int VARIANT_PASS_DEBUG_LIGHTING = 12;
	
	static constexpr int VARIANT_PASS_DO_INDIRECT_EMISSIVE = 1;
	static constexpr int VARIANT_PASS_DO_DIRECT_DIRECTIONAL = 2;
	static constexpr int VARIANT_PASS_DO_DIRECT_OMNI = 3;
	static constexpr int VARIANT_PASS_DO_DIRECT_SPOTLIGHT = 4;
	SHADER_ENUM(VARIANT_PASS, 1, 2, 3, 4, 10, 11, 12)

	RR_SHADER_VARIANT_COLLECT_BEGIN(shade_lighting_p)
		RR_SHADER_COLLECT(VARIANT_PASS)
	RR_SHADER_VARIANT_COLLECT_END()
RR_SHADER_VARIANT_END();