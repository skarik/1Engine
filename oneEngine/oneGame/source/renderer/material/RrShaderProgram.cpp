#include "RrShaderProgram.h"
#include "RrShaderMasterSubsystem.h"

#include "gpuw/ShaderPipeline.h"
#include "gpuw/Shader.h"
#include "gpuw/Public/Error.h"

#include "core/debug.h"
#include "core/containers/arstring.h"
#include "core/utils/string.h"
#include "core-ext/resources/ResourceManager.h"

#include "core-ext/system/io/Resources.h"

#include "core/math/hash.h"

#include <inttypes.h>

RrShaderProgram*
RrShaderProgram::Load ( const rrShaderProgramVsPs& params )
{
	auto resm = core::ArResourceManager::Active();

	// Generate the resource name from the filenames:
	arstring256 resource_str_id;
	snprintf(resource_str_id.data, 256, "VsPs_%08" PRIx32 "_%08" PRIx32, math::hash::fnv1a_32(params.file_vv), math::hash::fnv1a_32(params.file_p));
	core::utils::string::ToResourceName(resource_str_id);

	// First, find the texture in the resource system:
	IArResource* existingResource = resm->Find(core::kResourceTypeRrShader, resource_str_id);
	if (existingResource != NULL)
	{
		// Found it! Add a reference and return it.
		RrShaderProgram* existingShader = (RrShaderProgram*)existingResource;
		existingShader->AddReference();
		return existingShader;
	}

	// We need to create a new texture:
	rrStageToLoad stages[] = {
		{gpu::kShaderStageVs, params.file_vv},
		{gpu::kShaderStagePs, params.file_p}
	};
	RrShaderProgram* shaderProgram = new RrShaderProgram(resource_str_id.c_str(), stages, 2);

	// Add it to the resource system:
	resm->Add(shaderProgram);

	return shaderProgram;
}

RrShaderProgram*
RrShaderProgram::Load ( const rrShaderProgramCs& params )
{
	auto resm = core::ArResourceManager::Active();

	// Generate the resource name from the filenames:
	arstring256 resource_str_id;
	snprintf(resource_str_id.data, 256, "Cs_%08" PRIx32, math::hash::fnv1a_32(params.file_c));
	core::utils::string::ToResourceName(resource_str_id);

	// First, find the texture in the resource system:
	IArResource* existingResource = resm->Find(core::kResourceTypeRrShader, resource_str_id);
	if (existingResource != NULL)
	{
		// Found it! Add a reference and return it.
		RrShaderProgram* existingShader = (RrShaderProgram*)existingResource;
		existingShader->AddReference();
		return existingShader;
	}

	// We need to create a new texture:
	rrStageToLoad stages[] = {
		{gpu::kShaderStageCs, params.file_c},
	};
	RrShaderProgram* shaderProgram = new RrShaderProgram(resource_str_id.c_str(), stages, 1);

	// Add it to the resource system:
	resm->Add(shaderProgram);

	return shaderProgram;
}

RrShaderProgram::RrShaderProgram (
	const char* s_resourceId,
	const rrStageToLoad* stages,
	const int stageCount)
{
	// Assign resource name
	m_resourceName = s_resourceId;

	// Create program
	LoadProgramFromDisk(stages, stageCount);
}

RrShaderProgram::~RrShaderProgram ( void )
{
	FreeProgram();
}

void RrShaderProgram::LoadProgramFromDisk (
	const rrStageToLoad* stages,
	const int stageCount)
{
	gpu::Shader* shaderStages [gpu::kShaderStageMAX] = {};

		// Do not allow this to be called without destroying the previous pipeline.
	ARCORE_ASSERT(!m_pipeline.valid());

	// Loop through all the input stages and load them one-by-one.
	// We will save each stage into the array "shaderStages[]" then pass all into the RrShaderProgram constructor.
	for (int i = 0; i < stageCount; ++i)
	{
		// Skip empty stages. This can happen if someone is loading lazily.
		if (stages[i].filename == NULL || strlen(stages[i].filename) == 0)
			continue;

		std::string sb_filename = stages[i].filename;
		// Fix filename if running with DirectX target
#	ifdef GPU_API_DIRECTX11
		sb_filename = sb_filename.substr(0, sb_filename.find_last_of('.')) + ".dxc";
#	endif

		// Find the file to source data from:
		bool file_found = core::Resources::MakePathTo(sb_filename.c_str(), sb_filename);
		ARCORE_ASSERT(file_found);

		if (file_found)
		{
			// Create and load the shader from disk.
			gpu::Shader* shader = new gpu::Shader;
			int ret = shader->createFromFile(stages[i].stage, sb_filename.c_str());
			ARCORE_ASSERT(ret ==  gpu::kError_SUCCESS);

			// Save the shader for now.
			shaderStages[stages[i].stage] = shader;

#		if BUILD_DEVELOPMENT
			switch (stages[i].stage)
			{
			case gpu::kShaderStageVs:	file_vv = sb_filename; break;
			case gpu::kShaderStageHs:	file_h = sb_filename; break;
			case gpu::kShaderStageDs:	file_d = sb_filename; break;
			case gpu::kShaderStageGs:	file_g = sb_filename; break;
			case gpu::kShaderStagePs:	file_p = sb_filename; break;
			case gpu::kShaderStageCs:	file_c = sb_filename; break;
			}
#		endif
		}
	}

	// Create the shader program
	CreateProgram(
		shaderStages[gpu::kShaderStageVs],
		shaderStages[gpu::kShaderStageHs],
		shaderStages[gpu::kShaderStageDs],
		shaderStages[gpu::kShaderStageGs],
		shaderStages[gpu::kShaderStagePs],
		shaderStages[gpu::kShaderStageCs]
	);
}

void RrShaderProgram::CreateProgram (
	gpu::Shader* vs,
	gpu::Shader* hs,
	gpu::Shader* ds,
	gpu::Shader* gs,
	gpu::Shader* ps,
	gpu::Shader* cs)
{
	// Attach all valid inputs to the shader
	if (vs)
		m_pipeline.attach(vs, "main");
	if (hs)
		m_pipeline.attach(hs, "main");
	if (ds)
		m_pipeline.attach(ds, "main");
	if (gs)
		m_pipeline.attach(gs, "main");
	if (ps)
		m_pipeline.attach(ps, "main");
	if (cs)
		m_pipeline.attachCompute(cs, "main");

	// Assemble the shader pipeline now that everything has been added to it.
	int ret = m_pipeline.assemble();

	ARCORE_ASSERT(ret == gpu::kError_SUCCESS);
	ARCORE_ASSERT(m_pipeline.valid());
}

void RrShaderProgram::FreeProgram ( void )
{
	m_pipeline.destroy();
}