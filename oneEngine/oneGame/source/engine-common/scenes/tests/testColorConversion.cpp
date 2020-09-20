// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/Console.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"
// Needed includes
#include "testColorConversion.h"
#include "core/math/Color.h"
#include "core/math/random/Random.h"

#include <iostream>
#include <chrono>
#include <thread>

void testColorConversion::LoadScene(void)
{
	using namespace std;

	cout << "Beginning color operation tests.\n" << endl;

	//=========================================//
	cout << "\nTest 1: Convert from Color to Color32 to Color" << endl;
	//=========================================//
	{
		auto start_time = chrono::high_resolution_clock::now();
		
		const Real MAX_DELTA = 0.51F / 255.0F;
		Color a;
		uint32_t a_sep;
		Color b;

		bool success;

		auto showResults = [&]( void )
		{
			// Print test info
			cout << "A: (" << a.red << "," << a.green << "," << a.blue << "," << a.alpha << ") "
				<< "B: (" << b.red << "," << b.green << "," << b.blue << "," << b.alpha << ") ";
			// Print test result
			if ( !success ) {
				debug::Console->PrintError( "[FAILED]\n" );
			}
			else {
				cout << "[PASS]" << endl;
			}
		};

		// Check simple cases
		{
			for ( int i = 0; i < 100; ++i )
			{
				a = Color( Random.Range(0.0F, 1.0F), Random.Range(0.0F, 1.0F), Random.Range(0.0F, 1.0F), 1.0F );
				if ( rand() % 9 == 0 ) {
					a.red = 1.0F;
				}
				if ( rand() % 4 == 0 ) {
					a = Color( 1.0F, 0.0F, 1.0F, 1.0F );
				}

				a_sep = a.GetCode();
				b.SetCode(a_sep);

				if (fabs(a[0] - b[0]) < MAX_DELTA && fabs(a[1] - b[1]) < MAX_DELTA && fabs(a[2] - b[2]) < MAX_DELTA && fabs(a[3] - b[3]) < MAX_DELTA)
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

	//=========================================//
	cout << "\nTest 2: Convert from Color32 to Color to Color32" << endl;
	//=========================================//
	{
		auto start_time = chrono::high_resolution_clock::now();

		uint32_t a;
		Color a_sep;
		uint32_t b;

		bool success;

		auto showResults = [&]( void )
		{
			// Print test info
			cout << "A: (" << (a & 0xFF000000) << "-" << (a & 0x00FF0000) << "-" << (a & 0x0000FF00) << "-" << (a & 0x000000FF) << ") "
				<< "B: (" << (b & 0xFF000000) << "-" << (b & 0x00FF0000) << "-" << (b & 0x0000FF00) << "-" << (b & 0x000000FF) << ") ";
			// Print test result
			if ( !success ) {
				debug::Console->PrintError( "[FAILED]\n" );
			}
			else {
				cout << "[PASS]" << endl;
			}
		};

		// Check simple cases
		{
			for ( int i = 0; i < 100; ++i )
			{
				a = Random.Next();
				if ( rand() % 9 == 0 ) {
					a |= 0xFF;
				}
				if ( rand() % 4 == 0 ) {
					a = 0xFF00FFFF;
				}

				a_sep.SetCode(a);
				b = a_sep.GetCode();

				if (a == b)
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
