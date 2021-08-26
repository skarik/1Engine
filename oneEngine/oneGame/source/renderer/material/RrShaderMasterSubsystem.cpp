#include "renderer/material/RrShaderMasterSubsystem.h"
#include "renderer/material/RrShaderProgram.h"
#include "core/debug/console.h"

#include "core-ext/system/io/FileWatcher.h"
#include "renderer/windowing/RrWindow.h"

RrShaderMasterSubsystem::RrShaderMasterSubsystem ( void )
{

}
RrShaderMasterSubsystem::~RrShaderMasterSubsystem ( void )
{

}

void RrShaderMasterSubsystem::Update ( void )
{
#if BUILD_DEVELOPMENT
	if (RrWindow::Main()->GetFocused()) // Defer any reloading until the main window is in focus
	{
		std::unique_lock<std::mutex> lock( m_shaderReloadQueueMutex );

		if (!m_shaderReloadQueue.empty())
		{
			std::vector<RrShaderProgram*> reload_list = {};
			std::swap(m_shaderReloadQueue, reload_list);
			lock.unlock();

			for (auto& program : reload_list)
			{
				printf("shader to reload: %p\n", program);

				program->FreeProgram();
				RrShaderProgram::rrStageToLoad stages [] = {
					{gpu::kShaderStageVs, program->file_vv.c_str()},
					{gpu::kShaderStageHs, program->file_h.c_str()},
					{gpu::kShaderStageDs, program->file_d.c_str()},
					{gpu::kShaderStageGs, program->file_g.c_str()},
					{gpu::kShaderStagePs, program->file_p.c_str()},
					{gpu::kShaderStageCs, program->file_c.c_str()},
				};
				program->LoadProgramFromDisk(stages, 6);
			}
		}
	}
#endif
}

void RrShaderMasterSubsystem::OnAdd(IArResource* resource)
{
	RrShaderProgram* program = (RrShaderProgram*)resource;
	//texture->AddReference();

#if BUILD_DEVELOPMENT
	// Check program's file handles and open up file watchers
	std::string* file_paths [6] = {
		&program->file_vv,
		&program->file_h,
		&program->file_d,
		&program->file_g,
		&program->file_p,
		&program->file_c,
	};
	
	for ( int i = 0; i < 6; ++i )
	{
		m_fileToShaderMapping[*file_paths[i]] = program;
		core::io::file::BeginFileWatch(file_paths[i]->c_str(), [this, program](const std::string& filename, const core::io::file::Event event_type)
			{
				if (event_type == core::io::file::Event::kModified)
				{
					printf("shader modded: %s\n", filename.c_str());

					std::lock_guard<std::mutex> lock( m_shaderReloadQueueMutex );
					if (std::find(m_shaderReloadQueue.begin(), m_shaderReloadQueue.end(), program) == m_shaderReloadQueue.end())
					{
						m_shaderReloadQueue.push_back(program);
					}
				}
			});
	}

#endif
}

void RrShaderMasterSubsystem::OnRemove(IArResource* resource)
{
	RrShaderProgram* program = (RrShaderProgram*)resource;
	//texture->RemoveReference();
}

void RrShaderMasterSubsystem::Reload ( void )
{
	auto listing = this->GetListing();
	for (IArResource* resource : listing)
	{
		RrShaderProgram* program = (RrShaderProgram*)resource;
		if (program != NULL)
		{
			/*if (texture->GetIsProcedural() == false
				&& texture->ClassType() != core::gfx::tex::kTextureClassRenderTarget
				&& texture->ClassType() != core::gfx::tex::kTextureClassRenderTarget_Cube
				&& texture->ClassType() != core::gfx::tex::kTextureClassRenderTarget_MRT)
			{
				// Ask for a reload.
				texture->Reload();
			}*/
			// todo
		}
	}
}