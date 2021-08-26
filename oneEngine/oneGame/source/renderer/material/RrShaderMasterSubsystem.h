#ifndef RENDERER_SHADER_MASTER_SUBSYSTEM_H_
#define RENDERER_SHADER_MASTER_SUBSYSTEM_H_

#include "core-ext/resources/IArResourceSubsystem.h"

#if BUILD_DEVELOPMENT
// TODO: the filewatcher mechanism should go into the root resource manager.
#include "core-ext/system/io/FileWatcher.h"
#include <map>
#include <mutex>
#endif

class RrShaderProgram;
class RrShaderMasterSubsystem : public core::IArResourceSubsystem
{
public:
	explicit			RrShaderMasterSubsystem ( void );
						~RrShaderMasterSubsystem ( void );

protected:
	// Step per frame for updating the streaming system
	void				Update ( void ) override;

	//	OnAdd() : Called when adding resource to be tracked by resource system.
	void				OnAdd ( IArResource* resource ) override;
	//	OnRemove() : Called when removing resource from tracking.
	void				OnRemove ( IArResource* resource ) override;

public:
	//	Reload() : Called when clients want to reload subsytem's values.
	void				Reload ( void ) override;

#if BUILD_DEVELOPMENT
private:
	std::map<std::string, RrShaderProgram*>
						m_fileToShaderMapping;
	std::vector<RrShaderProgram*>
						m_shaderReloadQueue;
	std::mutex			m_shaderReloadQueueMutex;
#endif
};

#endif//RENDERER_TEXTURE_MASTER_SUBSYSTEM_H_