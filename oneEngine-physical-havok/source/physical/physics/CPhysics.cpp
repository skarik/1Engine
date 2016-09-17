
// Include class definition
#include "CPhysics.h"

// == Static Variable Delarations ==
//bool					Physics::bActive = false;
CPhysics*	Physics::Instance = NULL;
/*
PHYS_API hkMallocAllocator	Physics::baseMalloc;
PHYS_API hkMemoryRouter*		Physics::memoryRouter;
PHYS_API hkpWorldCinfo		Physics::worldInfo;
PHYS_API hkpWorld*			Physics::pWorld;

PHYS_API hkJobThreadPool*		Physics::threadPool;
PHYS_API hkJobQueue*			Physics::jobQueue;
PHYS_API int					Physics::totalNumThreadsUsed;

PHYS_API Vector3d				Physics::vWorldCenter = Vector3d( 0,0,0 );

// Debugger
#ifdef _HAVOK_VISUAL_DEBUGGER_
PHYS_API hkpPhysicsContext*	Physics::physicsContext;
PHYS_API hkVisualDebugger*	Physics::vdb;
#endif
*/
// Platform specific initialization
#include <Common/Base/System/Init/PlatformInit.cxx>

// Include registration registration
#include <Common/Base/keycode.cxx>

// We're not using anything product specific yet. We undef these so we don't get the usual
// product initialization for the products.
#undef HK_FEATURE_PRODUCT_AI
//#undef HK_FEATURE_PRODUCT_ANIMATION		// TODO: comment out in future.
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_BEHAVIOR
//#undef HK_FEATURE_PRODUCT_PHYSICS

// Also we're not using any serialization/versioning so we don't need any of these.
#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
#define HK_EXCLUDE_FEATURE_RegisterVersionPatches
/*#ifndef _HAVOK_VISUAL_DEBUGGER_
#define HK_EXCLUDE_FEATURE_RegisterReflectedClasses //NEEDED FOR DEBUGGER
#endif*/
#define HK_EXCLUDE_FEATURE_MemoryTracker

// Serialization config
#include <Common/Base/KeyCode.h>
#define HK_CLASSES_FILE <Common/Serialize/ClassList/hkKeyCodeClasses.h>

// This include generates an initialization function based on the products
// and the excluded features.
#include <Common/Base/Config/hkProductFeatures.cxx>

//===Function Definitions===
void Physics::Init ( void )
{
	if ( Instance )
		throw Core::InvalidInstantiationException();
	//bActive = true;
	Instance = new CPhysics();
	Instance->_Init();
}

void Physics::_Init ( void )
{
	// Perfrom platform specific initialization for this demo - you should already have something similar in your own code.
	PlatformInit();

	// Need to have memory allocated for the solver. Allocate 32mb for it. // NOTE: NOW .5 GB ALLOCATED
	//memoryRouter = hkMemoryInitUtil::initDefault( &baseMalloc, hkMemorySystem::FrameInfo(1024 * 1024 * 512) ); //Allocates the amount of Bytes.
	//memoryRouter = hkMemoryInitUtil::initDefault( &baseMalloc, hkMemorySystem::FrameInfo(1024 * 1024 * 1024) ); //1 GB allocated
	memoryRouter = hkMemoryInitUtil::initDefault( &baseMalloc, hkMemorySystem::FrameInfo(1024 * 1024 * 200) ); //200 MB allocated
	hkBaseSystem::init( memoryRouter, errorReport );
	/*{
		HK_WARN_ALWAYS(0x417ffd72, "\n\
------------------------------------------------------------------\n\
 Havok - Build (20120119)\n\
 Version 2011.3.0-r1\n\
 Base system initialized.\n\
------------------------------------------------------------------\n");
	}*/
	std::cout << "Havok 2011 base systems started!" << std::endl;

#ifdef _PHYSICS_MULTITHREADED_
	// Initialize the multithreading classes
	InitMultithreading();
#endif

	// Set the physics world properties
	//worldInfo.m_gravity.set( 0,0, -9.81f );
	worldInfo.m_gravity.set( 0,0, -9.81f*2.717f );
	worldInfo.setBroadPhaseWorldSize( 1000.0f );
	worldInfo.setupSolverInfo( hkpWorldCinfo::SOLVER_TYPE_4ITERS_MEDIUM );
	// Set the simulation type
#ifdef _PHYSICS_MULTITHREADED_
	worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;
#else
	worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_CONTINUOUS; //SIMULATION_TYPE_DISCRETE if we get speed issues
	//worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_DISCRETE; //SIMULATION_TYPE_DISCRETE if we get speed issues
#endif
	// Create the Physics World
	pWorld = new hkpWorld( worldInfo );
	{
		HK_WARN_ALWAYS(0x417ffd72, " Havok physics world created.\n");
		pWorld->markForWrite();
	}

	// Register all collision agents
	// It's important to register collision agents before adding any entities to the world.
	hkpAgentRegisterUtil::registerAllAgents( pWorld->getCollisionDispatcher() );

	// Initialize the collision layers used
	InitLayers();

#ifdef _PHYSICS_MULTITHREADED_
	// Register physics job handling functions with job queue
    pWorld->registerWithJobQueue( jobQueue );
#endif

#ifdef _HAVOK_VISUAL_DEBUGGER_
	//==DEBUGGER==
	physicsContext = new hkpPhysicsContext;
	physicsContext->addWorld( pWorld ); // add all worlds as you have

	hkpPhysicsContext::registerAllPhysicsProcesses();

	hkArray<hkProcessContext*> contexts;
	contexts.pushBack( physicsContext );

	vdb = new hkVisualDebugger( contexts );
	vdb->serve();
#endif

	pWorld->unmarkForWrite();
}
/*
void Physics::TestFunc ( float time )
{
	pWorld->setFrameTimeMarker(time);
	pWorld->advanceTime();
}
*/
void Physics::InitMultithreading ( void )
{
	const int streamCollectionSize = 200000; //200 kb
	//hkJobThreadPool* threadPool;

	// We can cap the number of threads used - here we use the maximum for whatever multithreaded platform we are running on. This variable is
	// set in the following code sections.
	totalNumThreadsUsed = 0;

	// Depending on the platform, grab the correct thread pool
	#if defined HK_PLATFORM_PS3_PPU

		hkSpuJobThreadPoolCinfo threadPoolCinfo;

		extern CellSpurs* initSpurs();
		HK_CELL_SPURS* spurs = initSpurs();

		hkSpuUtil* spuUtil = new hkSpuUtil( spurs );

		threadPoolCinfo.m_spuUtil = spuUtil;
		threadPoolCinfo.m_maxNumSpus = 5; // Use 5 SPUs for this example

		totalNumThreadsUsed = 1; // only use one CPU thread for PS3.

		// This line enables timers collection, by allocating 200 Kb per thread.  If you leave this at its default (0),
		// timer collection will not be enabled.
		threadPoolCinfo.m_perSpuMonitorBufferSize = streamCollectionSize;
		threadPool = new hkSpuJobThreadPool( threadPoolCinfo );
		spuUtil->removeReference();

	#else

		// Get the number of physical threads available on the system
		hkHardwareInfo hwInfo;
		hkGetHardwareInfo(hwInfo);
		totalNumThreadsUsed = hwInfo.m_numThreads;

		// We use one less than this for our thread pool, because we must also use this thread for our simulation
		hkCpuJobThreadPoolCinfo threadPoolCinfo;
		threadPoolCinfo.m_numThreads = totalNumThreadsUsed - 1;

		// This line enables timers collection, by allocating 200 Kb per thread.  If you leave this at its default (0),
		// timer collection will not be enabled.
		threadPoolCinfo.m_timerBufferPerThreadAllocation = streamCollectionSize;
		threadPool = new hkCpuJobThreadPool( threadPoolCinfo );

	#endif

	// We also need to create a Job queue. This job queue will be used by all Havok modules to run multithreaded work.
	// Here we only use it for physics.
	hkJobQueueCinfo info;
	info.m_jobQueueHwSetup.m_numCpuThreads = totalNumThreadsUsed;
	//hkJobQueue* jobQueue = new hkJobQueue(info);
	jobQueue = new hkJobQueue(info);

	// Monitors have been enabled for thread pool threads already (see above comment).
	hkMonitorStream::getInstance().resize(streamCollectionSize);
}

void Physics::Free ( void )
{
	Instance->_Free();
	delete Instance;
	Instance = NULL;
}

void Physics::_Free ( void )
{
	EXCEPTION_CATCH_BEGIN
#ifdef _HAVOK_VISUAL_DEBUGGER_
	// Debugger
	vdb->removeReference();
	physicsContext->removeReference();
#endif
	// Havok
	{
		HK_WARN_ALWAYS(0x417ffd72, "Quitting Havok.\n");
	}
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
	EXCEPTION_CATCH_END
}


CPhysics* Physics::Active ( void )
{
	return Instance;
}
void Physics::SetActive ( CPhysics* inst )
{
	Instance = inst;
}

hkpWorld* Physics::World ( void )
{
	return Instance->pWorld;
}
hkJobQueue* Physics::JobQueue ( void )
{
	return Instance->jobQueue;
}
hkJobThreadPool* Physics::ThreadPool ( void )
{
	return Instance->threadPool;
}

#ifdef _HAVOK_VISUAL_DEBUGGER_
hkVisualDebugger* Physics::VDB ( void )
{
	return Instance->vdb;
}
#endif
