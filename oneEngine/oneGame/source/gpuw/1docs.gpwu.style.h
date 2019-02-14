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
		GPUW_API				ArDocsExample ( void );
		//	Destructor : this documentation is often omitted.
		GPUW_API virtual		~ArDocsExample ( void );

		//	InterfaceImplementation() : implements a specific interface.
		// Performs some sort of bullshit math.
		// Arguments:
		//	a: position to be rotated.
		//	b: rotation to rotate the position by.
		// Returns:
		//	int: Success code.
		GPUW_API int			InterfaceImplementation ( Vector3f a, Quaternion b );

		// common:
		GPUW_API bool			valid ( void );
		GPUW_API gpuHandle		nativePtr ( void );

		// buffers:
		GPUW_API int			init ( void );
		GPUW_API int			allocate ( ... );
		GPUW_API int			free ( void );

		// objects:
		GPUW_API int			create ( void );
		GPUW_API int			destroy ( void );
		GPUW_API int			assemble ( void );

	private:
	};
}

#endif//DOCS_COMMENTS_H_