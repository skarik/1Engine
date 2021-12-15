//=onengine/core=================================================================================//
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
namespace core
{
	// Quick documentation summary. It isn't always needed.
	class ArDocsExample : IExampleInterface
	{
	public:
		//	Constructor : default creation.
		// The indentation should be kept consistent with destructor name.
		EXAMPLE_API				ArDocsExample ( void );
		//	Destructor : this documentation is often omitted.
		EXAMPLE_API virtual		~ArDocsExample ( void );

		//	InterfaceImplementation() : implements a specific interface.
		// Performs some sort of bullshit math.
		// Arguments:
		//	a: position to be rotated.
		//	b: rotation to rotate the position by.
		// Returns:
		//	int: Success code.
		EXAMPLE_API int			InterfaceImplementation ( Vector3f a, Quaternion b ) override;
	private:
		int					exampleVariable;
	};
}

#endif//DOCS_COMMENTS_H_