#pragma once
#include "../CSC8503Common/GameObject.h"
#include "AABBVolume.h"

namespace NCL {
	namespace CSC8503 {
		class Floor : public GameObject
		{
		public:
			Floor(Vector3 pos, Vector3 halfsize)
			{
				tag = GameObjectTag::FLOOR;

				AABBVolume* volume = new AABBVolume(halfsize);
				SetBoundingVolume((CollisionVolume*)volume);
				GetTransform().SetScale(halfsize * 2).SetPosition(pos);

				SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));
				
				GetPhysicsObject()->SetInverseMass(0.0f);
				GetPhysicsObject()->SetElasticity(0.0f);
				GetPhysicsObject()->InitCubeInertia();

			}
			~Floor() {}

		};
	}
}