#pragma once
#include "GameObject.h"

namespace NCL
{
	namespace CSC8503
	{
		class Platform : public GameObject
		{
		public:
			Platform(Vector3 pos, Vector3 halfsize)
			{
				tag = GameObjectTag::FLOOR;
				AABBVolume* volume = new AABBVolume(halfsize);
				SetBoundingVolume((CollisionVolume*)volume);
				GetTransform().SetScale(halfsize * 2).SetPosition(pos);

				SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));

				GetPhysicsObject()->SetInverseMass(0.8f);
				GetPhysicsObject()->SetElasticity(0.0f);
				GetPhysicsObject()->InitCubeInertia();
				physicsObject->SetEffectedByGravity(false);
			}

			Platform(Vector3 halfsize)
			{
				tag = GameObjectTag::FLOOR;
				AABBVolume* volume = new AABBVolume(halfsize);
				SetBoundingVolume((CollisionVolume*)volume);
				GetTransform().SetScale(halfsize * 2);

				SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));

				GetPhysicsObject()->SetInverseMass(0.8f);
				GetPhysicsObject()->SetElasticity(0.0f);
				GetPhysicsObject()->InitCubeInertia();
				physicsObject->SetEffectedByGravity(false);
			}
			~Platform() {}
		};
	}
}