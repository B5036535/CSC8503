#pragma once
#include "../CSC8503Common/GameObject.h"
#include "AABBVolume.h"

namespace NCL {
	namespace CSC8503 {
		class KillPlane : public GameObject
		{
		public:
			KillPlane(Vector3 halfsize)
			{
				AABBVolume* volume = new AABBVolume(halfsize);

				SetBoundingVolume((CollisionVolume*)volume);

				GetTransform().SetPosition(Vector3(0, -50.f, 0)).SetScale(halfsize * 2);

				SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));

				GetPhysicsObject()->SetInverseMass(0);
				GetPhysicsObject()->InitCubeInertia();
			}
			~KillPlane() {}


			virtual void OnCollisionBegin(GameObject* otherObject) override
			{
				otherObject->Respawn();
			}

		protected:


		};
	}
}