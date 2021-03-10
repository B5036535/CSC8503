#pragma once
#include "Constraint.h"
#include "PhysicsObject.h"

namespace NCL
{
	namespace CSC8503
	{
		class GameObject;

		class BridgeConstraint : public Constraint
		{
		public:
			BridgeConstraint(GameObject* a, GameObject* b, float d, float ang)
			{
				objectA = a;
				objectB = b;
				distance = d;
				angle = ang;
			}
			~BridgeConstraint() {};

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;
			GameObject* objectB;

			float distance;
			float angle;

			void Distance(float dt, float constraintMass, PhysicsObject* physA, PhysicsObject* physB);
			void Rotation(float dt, float constraintMass, PhysicsObject* physA, PhysicsObject* physB, bool axis);
		};
	}
}