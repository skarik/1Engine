// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/CDebugConsole.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"
// Needed includes
#include "benchmarkMatrices.h"
#include "core-ext/transform/Transform.h"
#include <iostream>
#include <chrono>

#define ITERATION_COUNT 10000000

void benchmarkMatrices::LoadScene ( void )
{
	using namespace std;

	cout << "Beginning matrix tests.\n" << endl;

	//=========================================//
	cout << "\nTest 1: flying translation" << endl;
	//=========================================//
	{
		auto start_time = chrono::high_resolution_clock::now();
		{
			Matrix4x4 a, b, c;
			a.setTranslation( Vector3d(1,1,1) );
			b.setTranslation( Vector3d(-1,-1,-1) );
			for ( uint64_t i = 0; i < ITERATION_COUNT; ++i )
			{
				c = a*b;
				b = c*a;
			}
		}
		auto end_time = chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end_time-start_time;
		cout << "Time taken was " << diff.count() << " s" << endl;
	}
}

#undef ITERATION_COUNT