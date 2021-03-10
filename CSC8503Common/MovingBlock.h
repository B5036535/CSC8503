#pragma once
#include "..\CSC8503Common\GameObject.h"
#include "..\CSC8503Common\StateTransition.h"
#include "..\CSC8503Common\StateMachine.h"
#include "..\CSC8503Common\State.h"
#include "AABBVolume.h"

namespace NCL {
	namespace CSC8503 {
		class MovingBlock : public GameObject
		{
		public:
			MovingBlock(Vector3 pos, Vector3 halfsize)
			{
				tag = GameObjectTag::OBSTACLE;
				counter = 0.0f;
				stateMachine = new StateMachine();

				State* stateA = new State([&](float dt)->void
				{
					this->MoveLeft(dt);
				});
				State* stateB = new State([&](float dt)->void
				{
					this->MoveRight(dt);
				});

				stateMachine->AddState(stateA);
				stateMachine->AddState(stateB);

				stateMachine->AddTransition(new StateTransition(stateA, stateB,
					[&]()->bool
				{
					return this->counter > 1.5f;
				}));
				stateMachine->AddTransition(new StateTransition(stateB, stateA,
					[&]()->bool
				{
					return this->counter < 0.0f;
				}));

				AABBVolume* volume = new AABBVolume(halfsize);
				SetBoundingVolume((CollisionVolume*)volume);
				GetTransform().SetScale(halfsize * 2).SetPosition(pos);

				SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));

				GetPhysicsObject()->SetInverseMass(0.05f);
				GetPhysicsObject()->SetElasticity(0.0f);
				GetPhysicsObject()->SetEffectedByGravity(false);
				GetPhysicsObject()->InitCubeInertia();
			}
			~MovingBlock() {}

			StateMachine* stateMachine;

		protected:
			void MoveLeft(float dt)
			{
				GetPhysicsObject()->AddForce({ -1000,0,0 });
				counter += dt;
			}
			void MoveRight(float dt)
			{
				GetPhysicsObject()->AddForce({ 1000,0,0 });
				counter -= dt;
			}

			float counter;
		};
	}
}