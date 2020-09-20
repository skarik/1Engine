// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/Console.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"
// Needed includes
#include "testStringOperations.h"
#include "core/utils/string.h"

#include <iostream>
#include <chrono>
#include <thread>

void testStringOperations::LoadScene(void)
{
	using namespace std;

	cout << "Beginning string operation tests.\n" << endl;

	//=========================================//
	cout << "\nTest 1: core::utils::string::LargestCommonSubstringLength" << endl;
	//=========================================//
	{
		auto start_time = chrono::high_resolution_clock::now();
		
		string a, b;
		size_t target, cm;

		auto showResults = [&]( const char* description )
		{
			// Do test now
			cm = core::utils::string::LargestCommonSubstringLength( a,b );
			// Print test info
			cout << "TEST: " << description << endl
				 << "Input A: \"" << a << "\"" << endl
				 << "Input B: \"" << b << "\"" << endl
				 << "Expecting result of " << target << ", got " << cm << " ";
			// Print test result
			if ( target != cm ) {
				debug::Console->PrintError( "[FAILED]\n" );
			}
			else {
				cout << "[PASS]" << endl;
			}
		};

		// Check simple cases
		{
			a = "Hello";
			b = "Hello World";
			target = 5;
			showResults("Testing starting match 1");
			std::swap(a,b);
			showResults("Testing starting match 2");

			a = "Hello";
			b = a;
			showResults("Testing full equal");

			a = "World";
			b = "Hello World";
			showResults("Testing trailing match 1");
			std::swap(a,b);
			showResults("Testing trailing match 2");

			a = "lo Wo";
			b = "Hello World";
			showResults("Testing inside match 1");
			std::swap(a,b);
			showResults("Testing inside match 2");

			a = "Bip001 L Hand 00";
			b = "L Hand 0";
			target = b.length();
			showResults("Testing practical match 1");
			std::swap(a,b);
			showResults("Testing practical match 2");

			a = "Bip001 Spine01";
			b = "Spine";
			target = b.length();
			showResults("Testing practical match 3");
			std::swap(a,b);
			showResults("Testing practical match 4");

			a = "Bip001 L Arm 10";
			b = "L Arm 01";
			target = b.length()-2;
			showResults("Testing practical match 5");
			std::swap(a,b);
			showResults("Testing practical match 6");

			a = "Right Lower Arm, Right Arm, Left Hand";
			b = "Right Arm";
			target = b.length();
			showResults("Testing practical match 7");
			std::swap(a,b);
			showResults("Testing practical match 8");
		}

		auto end_time = chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end_time - start_time;
		cout << "Time taken was " << diff.count() << " s" << endl;
	}

	while(true) std::this_thread::yield(); // Prevent from hitting the rendering loop
}
