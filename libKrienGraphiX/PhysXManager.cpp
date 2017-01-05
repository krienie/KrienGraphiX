
#include <iostream>
#include <vector>

#include <PxScene.h>
#include <PxPhysics.h>
#include <common\PxSerialFramework.h>
#include <common\PxTolerancesScale.h>
//#include <cooking\PxCooking.h>
#include <extensions\PxExtensionsAPI.h>
#include <foundation\PxFoundation.h>
#include <foundation\PxMat44.h>
#include <pvd\PxVisualDebugger.h>


#include "Filesystem.h"
#include "PhysXManager.h"

namespace kgx
{
	PhysXManager* PhysXManager::m_inst = nullptr;

	PhysXManager* PhysXManager::getInst()
	{
		if ( !m_inst )
			m_inst = new PhysXManager();

		return m_inst;
	}

	void PhysXManager::destroy()
	{
		if ( PhysXManager::m_inst )
			delete PhysXManager::m_inst;
	}


	PhysXManager::PhysXManager()
		: m_defaultErrorCallback(), m_defaultAllocatorCallback(), m_connection(nullptr), m_foundation(nullptr),
			m_physics(nullptr), /*m_cooking(nullptr),*/ m_dispatcher(nullptr), m_scene(nullptr),
			m_isInit(false), m_accumulator(0.0f)
	{
		m_foundation = PxCreateFoundation( PX_PHYSICS_VERSION, m_defaultAllocatorCallback, m_defaultErrorCallback );
		if ( !m_foundation )
		{
			m_foundation = nullptr;

			std::cout << "Error (PhysXManager::PhysXManager): PxCreateFoundation failed." << std::endl;
			return;
		}

		m_physics = PxCreatePhysics( PX_PHYSICS_VERSION, *m_foundation, physx::PxTolerancesScale() );
		if ( !m_physics )
		{
			m_foundation->release();
			m_foundation = nullptr;

			std::cout << "Error (PhysXManager::PhysXManager): PxCreatePhysics failed." << std::endl;
			return;
		}

		/*m_cooking = PxCreateCooking( PX_PHYSICS_VERSION, *m_foundation, physx::PxCookingParams(physx::PxTolerancesScale()) );
		if ( !m_cooking )
		{
			m_physics->release();
			m_physics = nullptr;
			m_foundation->release();
			m_foundation = nullptr;

			std::cout << "Error (PhysXManager::PhysXManager): PxCreateCooking failed." << std::endl;
			return;
		}*/

		/*if ( !PxInitExtensions(*m_physics) )
		{
			//m_cooking->release();
			//m_cooking = nullptr;
			m_physics->release();
			m_physics = nullptr;
			m_foundation->release();
			m_foundation = nullptr;

			std::cout << "Error (PhysXManager::PhysXManager): PxInitExtensions failed." << std::endl;
			return;
		}*/

		if ( m_physics->getPvdConnectionManager() )
		{
			m_physics->getVisualDebugger()->setVisualizeConstraints( true );
			m_physics->getVisualDebugger()->setVisualDebuggerFlag( physx::PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true );
			m_physics->getVisualDebugger()->setVisualDebuggerFlag( physx::PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true );
			m_connection = physx::PxVisualDebuggerExt::createConnection( m_physics->getPvdConnectionManager(), "127.0.0.1", 5425, 10 );
		}

		initScene();

		m_isInit = true;
	}

	PhysXManager::~PhysXManager()
	{
		if ( m_connection )
			m_connection->release();

		if ( m_dispatcher )
			m_dispatcher->release();

		if ( m_scene )
			m_scene->release();

		PxCloseExtensions();

		//if ( m_cooking )
		//	m_cooking->release();

		if ( m_physics )
			m_physics->release();

		if ( m_foundation )
			m_foundation->release();
	}

	bool PhysXManager::getShapeGlobalPosition( const std::string &name, DirectX::XMFLOAT4X4 &result ) const
	{
		if ( !m_isInit || !m_scene )
		{
			std::cout << "(PhysXManager::getShapeGlobalPosition) Error: Scene not initialized." << std::endl;
			return false;
		}

		physx::PxU32 numActors = m_scene->getNbActors( physx::PxActorTypeSelectionFlag::eRIGID_DYNAMIC );
		if ( numActors )
		{
			std::vector<physx::PxRigidActor*> actors(numActors);
			m_scene->getActors( physx::PxActorTypeSelectionFlag::eRIGID_DYNAMIC,
								(physx::PxActor**)&actors[0], numActors);

			std::vector<physx::PxRigidActor*>::const_iterator it;
			for ( it = actors.cbegin(); it != actors.cend(); ++it )
			{
				if ( name == (*it)->getName() )
				{
					physx::PxU32 numShapes = (*it)->getNbShapes();
					std::vector<physx::PxShape*> shapes(numShapes);
					(*it)->getShapes((physx::PxShape**)&shapes[0], numShapes);
					if ( numShapes )		//TODO: support multiple shapes per actor
					{
						const physx::PxMat44 shapePose(physx::PxShapeExt::getGlobalPose(*shapes[0], *(*it)));
						// Don't get confused. The parameter rows are the matrix's columns
						result = DirectX::XMFLOAT4X4( shapePose.column0.x, shapePose.column0.y, shapePose.column0.z, shapePose.column0.w,
													  shapePose.column1.x, shapePose.column1.y, shapePose.column1.z, shapePose.column1.w,
													  shapePose.column2.x, shapePose.column2.y, shapePose.column2.z, shapePose.column2.w,
													  shapePose.column3.x, shapePose.column3.y, shapePose.column3.z, shapePose.column3.w);

						return true;
					}
				}
			}
		}

		return false;
	}

	bool PhysXManager::isInit() const
	{
		return m_isInit;
	}

	bool PhysXManager::advance( float timestep )
	{
		m_accumulator += timestep;

		const float stepSize = 1.0f / 60.0f;
		if ( m_accumulator < stepSize )
			return false;

		while ( m_accumulator > stepSize )
		{
			m_accumulator -= stepSize;

			m_scene->simulate( stepSize );
			m_scene->fetchResults( true );
		}
	
		return true;
	}

	bool PhysXManager::loadPhysXCollection( const std::string &file )
	{
		std::string absFile = filesystem::getFile(file);
		if ( absFile.empty() )
		{
			std::cout << "(PhysXManager::loadPhysXCollection) Error: File " << file << " was not found." << std::endl;
			return false;
		}

		physx::PxSerializationRegistry* registry = physx::PxSerialization::createSerializationRegistry( PxGetPhysics() );

		// Binary
		// Open file and get file size
		FILE* fp;
		fopen_s( &fp, absFile.c_str(), "rb" );
		fseek( fp, 0, SEEK_END );
		unsigned fileSize = ftell( fp );
		fseek( fp, 0, SEEK_SET );

		// Allocate aligned memory, load data and deserialize
		//TODO: fix memory leak...
		//TODO: convert to C++
		void* memory    = malloc( fileSize + PX_SERIAL_FILE_ALIGN );
		void* memory128 = (void*)((size_t( memory ) + PX_SERIAL_FILE_ALIGN)&~(PX_SERIAL_FILE_ALIGN - 1));
		fread( memory128, 1, fileSize, fp );
		fclose( fp );
		physx::PxCollection* collection = physx::PxSerialization::createCollectionFromBinary( memory128, *registry );
		//~Binary

		m_scene->addCollection( *collection );

		registry->release();

		return true;
	}


	bool PhysXManager::sphereSweep( const DirectX::XMFLOAT3 &initialPose, const DirectX::XMFLOAT3 &dir, float &dist, DirectX::XMFLOAT3 &hit )
	{
		if ( !m_isInit || !m_scene )
		{
			std::cout << "(PhysXManager::sphereSweep) Error: Scene not initialized." << std::endl;
			return false;
		}

		physx::PxSphereGeometry sphere(0.5f);
		physx::PxTransform pose( physx::PxVec3( initialPose.x, initialPose.y, initialPose.z ) );
		physx::PxVec3 direction( dir.x, dir.y, dir.z );
		physx::PxSweepBuffer sweepBuff;

		if ( !m_scene->sweep(sphere, pose, direction, dist, sweepBuff) )
			return false;

		std::cout << "---------------------begin------------------------" << std::endl;

		if ( sweepBuff.hasBlock )
		{
			std::cout << "blocked!" << std::endl;
			std::cout << "block hitpoint: " << sweepBuff.block.position.x << ", " << sweepBuff.block.position.y << ", " << sweepBuff.block.position.z << std::endl;
		}

		if ( sweepBuff.hasAnyHits() )
			std::cout << "AnyHits found" << std::endl;

		std::cout << "NbTouches: " << sweepBuff.getNbTouches() << std::endl;
		std::cout << "NbAnyHits: " << sweepBuff.getNbAnyHits() << std::endl;

		physx::PxSweepHit sweepHit = sweepBuff.getAnyHit( 0 );
		hit = DirectX::XMFLOAT3(sweepHit.position.x, sweepHit.position.y, sweepHit.position.z);
		dist = sweepHit.distance;
		std::cout << "---------------------end------------------------" << std::endl;
		return true;
	}


	void PhysXManager::initScene()
	{
		// cleanup if necessary
		if ( m_dispatcher )
		{
			m_dispatcher->release();
			m_dispatcher = nullptr;
		}

		if ( m_scene )
		{
			m_scene->release();
			m_scene = nullptr;
		}

		// create scene
		physx::PxSceneDesc sceneDesc( m_physics->getTolerancesScale() );
		sceneDesc.gravity       = physx::PxVec3( 0.0f, 0.0f, -9.81f );
		m_dispatcher            = physx::PxDefaultCpuDispatcherCreate( 2 );
		sceneDesc.cpuDispatcher = m_dispatcher;
		sceneDesc.filterShader  = &simulationFilterShader;
		m_scene = m_physics->createScene( sceneDesc );
	}
}

physx::PxFilterFlags simulationFilterShader( physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
											 physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
											 physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize )
{
	PX_UNUSED( constantBlock );
	PX_UNUSED( constantBlockSize );

	// let triggers through
	if ( physx::PxFilterObjectIsTrigger( attributes0 ) || physx::PxFilterObjectIsTrigger( attributes1 ) )
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlags();
	}

	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

	return physx::PxFilterFlags();
}
