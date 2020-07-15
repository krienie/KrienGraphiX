
#pragma once

#include <string>

#include <DirectXMath.h>

#include <PxFiltering.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>
//#include <physxvisualdebuggersdk/PvdConnection.h>

namespace physx
{
    class PxFoundation;
    class PxPhysics;
    class PxScene;
    class PxDefaultCpuDispatcher;
}

namespace kgx
{
    class PhysXManager
    {
        public:
            static PhysXManager* getInst();
            static void destroy();

            bool getShapeGlobalPosition( const std::string &name, DirectX::XMFLOAT4X4 &result ) const;

            bool isInit() const;
            bool advance( float timestep );
            bool loadPhysXCollection( const std::string &file );

            bool sphereSweep( const DirectX::XMFLOAT3 &initialPose, const DirectX::XMFLOAT3 &dir, float &dist, DirectX::XMFLOAT3 &hit ) const;

        private:
            PhysXManager();
            ~PhysXManager();

            // no copying allowed
            PhysXManager( const PhysXManager& ) = delete;
            PhysXManager& operator=( const PhysXManager& ) = delete;

            void initScene();

            static PhysXManager *m_inst;

            physx::PxDefaultErrorCallback m_defaultErrorCallback;
            physx::PxDefaultAllocator m_defaultAllocatorCallback;
            //physx::PxVisualDebuggerConnection *m_connection;
            physx::PxFoundation *m_foundation;
            physx::PxPhysics *m_physics;
            physx::PxDefaultCpuDispatcher *m_dispatcher;
            physx::PxScene *m_scene;
            //physx::PxCooking *m_cooking;
            void *m_collectionMemory;

            bool m_isInit;
            float m_accumulator;
    };
}

// workaround for a crash caused by 3DS max PhysX plugin
physx::PxFilterFlags simulationFilterShader( physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
                                             physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
                                             physx::PxPairFlags &pairFlags, const void *constantBlock, physx::PxU32 constantBlockSize );
