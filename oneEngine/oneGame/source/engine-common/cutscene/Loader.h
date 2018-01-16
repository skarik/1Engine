#ifndef COMMON_CUTSCENE_LOADER_H_
#define COMMON_CUTSCENE_LOADER_H_

#include "core/types/types.h"

#include "engine-common/cutscene/Node.h"
#include "engine-common/cutscene/EditorNode.h"

#include <vector>

namespace common {
namespace cts
{
	typedef Node* (*nodeInstantiator_t)();

	class Loader
	{
	public:
		ENGCOM_API explicit	Loader ( void );
		ENGCOM_API			~Loader ( void );

		//	LoadCutscene ( resource name )
		// Attempts to load a cutscene with the given resource name.
		// If the file could not be loaded, will return false, and leftover data will be kept in the loader.
		ENGCOM_API bool		LoadCutscene ( const char* n_resourcename );

	public:
		//	Load options:

		// Instantiators for creating nodes. If these are not set, the input nodes will remain NULL.
		nodeInstantiator_t	m_instantiators [kNodeType_MAX];
		// Should nodes be loaded?
		bool				m_loadNodes;
		// Should editor data also be loaded?
		bool				m_loadEditorNodes;

		//	Loaded data:

		int							nodeCount;
		std::vector<Node>			nodes;
		std::vector<EditorNode>		editornodes;

	private:
	};
}}

#endif//COMMON_CUTSCENE_LOADER_H_