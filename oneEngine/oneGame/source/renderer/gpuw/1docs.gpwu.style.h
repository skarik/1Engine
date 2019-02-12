//===============================================================================================//
//
//		1docs Style Instruction
//
// A quick references on coding and documentatation style for consistency.
//
//===============================================================================================//
#ifdef  DOCS_COMMENTS_H_

#include <core/types.h>
#include <core-ext/example.h>
#include <engine/types.h>
#include <engine/interfaces/IExampleInterface.h>
#include <renderer/example.h> // renderer::RrExampleClass
#include <game/KeepThingsOrdered.h>

class EngineImplementation;

// Namespaces should always be kept lowercase.
namespace gpuw
{
	// Quick documentation summary. It isn't always needed.
	class ExampleGpuClass
	{
	public:
		//	Constructor : default creation.
		// The indentation should be kept consistent with destructor name.
		RENDER_API				ArDocsExample ( void );
		//	Destructor : this documentation is often omitted.
		RENDER_API virtual		~ArDocsExample ( void );

		//	InterfaceImplementation() : implements a specific interface.
		// Performs some sort of bullshit math.
		// Arguments:
		//	a: position to be rotated.
		//	b: rotation to rotate the position by.
		// Returns:
		//	int: Success code.
		RENDER_API int			InterfaceImplementation ( Vector3f a, Quaternion b );

		// common:
		RENDER_API bool			valid ( void );
		RENDER_API gpuHandle	nativePtr ( void );

		// buffers:
		RENDER_API int			init ( void );
		RENDER_API int			allocate ( ... );
		RENDER_API int			free ( void );

		// objects:
		RENDER_API int			create ( void );
		RENDER_API int			destroy ( void );
		RENDER_API int			assemble ( void );

	private:
	};
}

#endif//DOCS_COMMENTS_H_