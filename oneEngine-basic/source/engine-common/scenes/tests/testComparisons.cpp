// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/CDebugConsole.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"
// Needed includes
#include "testComparisons.h"
#include "core/math/Math3d.h"
#include "core/math/vect2d_template.h"

#include <iostream>
#include <chrono>
#include <thread>

void testComparisons::LoadScene(void)
{
	using namespace std;

	cout << "Beginning string operation tests.\n" << endl;

	//=========================================//
	cout << "\nTest 1: Random comparisons of Vector2i" << endl;
	//=========================================//
	{
		auto start_time = chrono::high_resolution_clock::now();
		
		Vector2i a, b;
		bool success;

		auto showResults = [&]( void )
		{
			// Print test info
			cout << "A: (" << a.x << "," << a.y << ") "
				 << "B: (" << b.x << "," << b.y << ") ";
			// Print test result
			if ( !success ) {
				Debug::Console->PrintError( "[FAILED]\n" );
			}
			else {
				cout << "[PASS]" << endl;
			}
		};

		// Check simple cases
		{
			for ( int i = 0; i < 100; ++i )
			{
				a = Vector2i( rand()%100, rand()%100 );
				b = Vector2i( rand()%100, rand()%100 );
				if ( rand() % 9 == 0 ) {
					a.x = b.x;
				}
				if ( rand() % 4 == 0 ) {
					a = b;
				}
				if ((a == b)||(a < b && !(b < a))||(!(a < b) && b < a))
					success = true;
				else
					success = false;
				showResults();
			}
		}

		auto end_time = chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end_time - start_time;
		cout << "Time taken was " << diff.count() << " s" << endl;
	}

	while(true) std::this_thread::yield(); // Prevent from hitting the rendering loop
}
