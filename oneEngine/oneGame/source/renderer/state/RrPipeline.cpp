#include "RrPipeline.h"
#include "RrPipelinePaletted.h"

#include "renderer/state/RrRenderer.h"
#include "renderer/object/RrRenderObject.h"
#include "renderer/camera/RrCamera.h"

//=====================================
// Pipeline instantiators
//=====================================

RrPipelineStateRenderer* renderer::CreatePipelineRenderer ( const renderer::PipelineMode mode )
{
	switch (mode)
	{
	case renderer::PipelineMode::kNormal:
		return new RrPipelineStandardRenderer();

	case renderer::PipelineMode::kPaletted:
		return new RrPipelinePalettedRenderer();
	}
	return nullptr;
}

RrPipelineOptions* renderer::CreatePipelineOptions ( const renderer::PipelineMode mode )
{
	switch (mode)
	{
	case renderer::PipelineMode::kNormal:
		return new RrPipelineStandardOptions();

	case renderer::PipelineMode::kPaletted:
		return new RrPipelinePalettedOptions();
	}
	return nullptr;
}

//=====================================
// Standard renderer pipeline
//=====================================

void RrPipelineStandardRenderer::CullObjects ( gpu::GraphicsContext* gfx, const RrOutputInfo& output, RrOutputState* state, RrWorld* world )
{
	// Update the render distance of objects against this camera:
	for ( int objectIndex = 0; objectIndex < world->objects.size(); objectIndex += 1 )
	{	
		// Put into it's own loop, since it's the same calculation across all objects
		if ( world->objects[objectIndex] != nullptr )
		{ 
			// TODO: Handle 2D mode properly. Postprocess is entirely sorted by transform.position.z Should this be made more consistent?
			world->objects[objectIndex]->renderDistance = (output.camera->transform.position - world->objects[objectIndex]->transform.world.position).sqrMagnitude();
		}
	}
}