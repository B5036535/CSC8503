#pragma once
#include "Constraint.h"

namespace NCL
{
	namespace CSC8503
	{
		class GameObject;

		class OrientationConstraint : public Constraint
		{
		public:
			OrientationConstraint(GameObject* a)
			{
				objectA = a;
			}
			~OrientationConstraint() {};

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;
		};
	}
}