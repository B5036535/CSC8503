#pragma once
#include "../CSC8503Common/GameObject.h"
#include "AABBVolume.h"

namespace NCL {
	namespace CSC8503 {
		class FinishLine : public GameObject
		{
		public:
			FinishLine(Vector3 halfsize, Vector3 pos, bool* fin)
			{

				tag = GameObjectTag::FINISH;

				AABBVolume* volume = new AABBVolume(halfsize);

				SetBoundingVolume((CollisionVolume*)volume);

				GetTransform().SetPosition(pos).SetScale(halfsize * 2);

				SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));

				GetPhysicsObject()->SetInverseMass(0);
				GetPhysicsObject()->InitCubeInertia();
				finish = fin;
			}
			~FinishLine() {}


			virtual void OnCollisionBegin(GameObject* otherObject) override
			{
				if (otherObject->tag == GameObjectTag::PLAYER || otherObject->tag == GameObjectTag::ENEMY)
				{
					*finish = true;
				}
			}

		protected:

			bool* finish;
		};
	}
}