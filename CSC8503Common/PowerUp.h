#pragma once
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/SphereVolume.h"

namespace NCL {
	namespace CSC8503 {
		class PowerUp : public GameObject
		{
		public:
			PowerUp(Vector3 pos, float radius)
			{
				tag = GameObjectTag::POWERUP;

				SphereVolume* volume = new SphereVolume(radius);
				SetBoundingVolume((CollisionVolume*)volume);
				GetTransform().SetScale(Vector3(radius, radius, radius) * 2).SetPosition(pos);

				SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));
				
				GetPhysicsObject()->InitSphereInertia();
				GetPhysicsObject()->SetEffectedByGravity(false);

			}

			virtual void OnCollisionBegin(GameObject* otherObject) override
			{
				if (otherObject->tag == GameObjectTag::PLAYER)
				{
					isActive = false;
				}
			}

			~PowerUp() {};

		};
	}
}