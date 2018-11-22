// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2018 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

// ****************************************************************************
// This snippet illustrates simple use of physx
//
// It creates a number of box stacks on a plane, and if rendering, allows the
// user to create new stacks and fire a ball from the camera position
// ****************************************************************************

#ifndef SIMULATION_H

#define SIMULATION_H

#include "commonImport.h"
#include "Incopiable.h"
#include "SimulationRender.h"
#include "MyCallback.h"
#include "Minuteur.h"
#include "Balle.h"
#include "Vehicule.h"
#include "Goal.h"

#include <chrono>
#include <vector>
#include <map>

class Balle;
class Vehicule;
class Goal;

class Simulation : public Incopiable, SimulationRender {

   physx::PxDefaultAllocator		gAllocator;
   physx::PxDefaultErrorCallback	gErrorCallback;

   std::unique_ptr<physx::PxFoundation, std::function<void(physx::PxFoundation*)>>			             gFoundation;
   std::unique_ptr<physx::PxPhysics, std::function<void(physx::PxPhysics*)>>				             gPhysics;

   std::unique_ptr<physx::PxDefaultCpuDispatcher, std::function<void(physx::PxDefaultCpuDispatcher*)>>	 gDispatcher;
   std::unique_ptr<physx::PxScene, std::function<void(physx::PxScene*)>>				                 gScene;

   std::unique_ptr<physx::PxMaterial, std::function<void(physx::PxBase*)>>				                 gMaterial;

   std::unique_ptr<physx::PxPvd, std::function<void(physx::PxPvd*)>>                                     gPvd;

   physx::PxReal stackZ;

   physx::PxTransform positionVehiculeInit;
   physx::PxTransform positionCargoInit;
   physx::PxTransform positionGoalInit;

   bool sceneCleanUp = false;

   unsigned short dTBalls;
   signed short vitesseBall;
   physx::PxReal rayonBall;

   Minuteur minuteur;

   std::vector<std::unique_ptr<Balle>> listBall;
   std::vector<unsigned short> listIdBallTouched;

   bool goalTouch = false;
   bool tirDemande = false;

   std::unique_ptr<Vehicule> pVehicule;
   std::unique_ptr<physx::PxRigidDynamic, std::function<void(physx::PxBase*)>> pCargo;
   std::unique_ptr<Goal> pGoal;

   MyCallback filterShader;

   // SINGLETON
   Simulation() :
      gAllocator{},
      gErrorCallback{},
      stackZ{ 10.0f },
      positionVehiculeInit{ physx::PxTransform(physx::PxVec3(0.f, 0.f, 0.f)) },
      positionCargoInit{ physx::PxTransform(physx::PxVec3(20.f, 0.f, 20.f)) },
      positionGoalInit{ physx::PxTransform(physx::PxVec3(100.f, 0.f, 100.f)) },
      dTBalls{ 1000/3 }, //3 balle par seconde
      vitesseBall{ 20 },
      rayonBall{ 5 },
      minuteur{true},
      filterShader { MyCallback::MyCallback() }
   {
      //initPhysics(true);
   }

public:
   static Simulation& get() {
      static Simulation singleton; // definition
      return singleton;
   }

   physx::PxRigidDynamic* createVehicule(const physx::PxTransform& t, physx::PxReal halfExtendX, physx::PxReal halfExtendY, physx::PxReal halfExtendZ);
   physx::PxRigidDynamic* createCargo(const physx::PxTransform& t, physx::PxReal halfExtendX, physx::PxReal halfExtendY, physx::PxReal halfExtendZ);
   physx::PxRigidDynamic* createGoal(const physx::PxTransform& t, physx::PxReal rayon, physx::PxReal hauteur);
   //physx::PxRigidDynamic* createBall(const physx::PxTransform& t,
      //const physx::PxReal& rayon,
      //const physx::PxVec3& velocity = physx::PxVec3(0),
      //const unsigned short indexBall = 0);
public:

   struct FilterGroup
   {
      enum TypeObject
      {
         eVEHICULE   = (1 << 0),
         eCARGO      = (1 << 1),
         eGOAL       = (1 << 2),
         eBALLE      = (1 << 3)
      };
   };

public:
   //physx::PxRigidDynamic* createWall(const physx::PxTransform& t, physx::PxReal halfExtendX, physx::PxReal halfExtendY, physx::PxReal halfExtendZ);

   void initPhysics(bool interactive);

   void stepPhysics(bool interactive);

   void cleanupPhysics(bool interactive);

   void keyPress(unsigned char key, const physx::PxTransform& camera);

   // DESTRUCTEUR

   ~Simulation() {
      cleanupPhysics(true);
   }

   static void elementPhysXFoundationDeleter(physx::PxFoundation* elementPhysX) {
      elementPhysX->release();
   }
   static void elementPhysXPhysicsDeleter(physx::PxPhysics* elementPhysX) {
      elementPhysX->release();
   }
   static void elementPhysXDispatcherDeleter(physx::PxDefaultCpuDispatcher* elementPhysX) {
      elementPhysX->release();
   }
   static void elementPhysXSceneDeleter(physx::PxScene* elementPhysX) {
      elementPhysX->release();
   }
   static void elementPhysXBaseDeleter(physx::PxBase* elementPhysX) {
      elementPhysX->release();
   }
   static void elementPhysXPvdDeleter(physx::PxPvd* elementPhysX) {
      elementPhysX->release();
   }


   // FILTERING

   void setupFiltering(physx::PxRigidActor* actor, physx::PxU32 filterGroup, physx::PxU32 filterMask);

   static physx::PxFilterFlags myFilterShader(
      physx::PxFilterObjectAttributes attributes0,
      physx::PxFilterData filterData0,
      physx::PxFilterObjectAttributes attributes1,
      physx::PxFilterData filterData1,
      physx::PxPairFlags& pairFlags,
      const void* constantBlock,
      physx::PxU32 constantBlockSize);

   //MAIN
public:
   int snippetMain(int, const char*const*);

   Vehicule* getVehicule() {
      return pVehicule.get();
   }

   physx::PxTransform changeGoalPosition(physx::PxTransform* _positionGoal) {
      physx::PxTransform newPositionGoal{_positionGoal->p.x + 100.f, _positionGoal->p.y, _positionGoal->p.z + 100.f };
      *_positionGoal = newPositionGoal;
      return newPositionGoal;
   }

   void contactDetected(const unsigned short indexBall) {
      listIdBallTouched.push_back(indexBall);
   }

   void goalAtteint() {
      goalTouch = true;
      pGoal->changePosition();
   }

   physx::PxRigidDynamic* createBall(const physx::PxTransform& t, const physx::PxReal& rayon, const physx::PxVec3& velocity);
//private:
   //void gestionCollision(const unsigned short indexBall);
};
#endif