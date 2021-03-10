#pragma once
#include "../CSC8503Common/Constraint.h"
//#include "../../Common/Vector3.h"
#include "GameObject.h"
#include "PhysicsObject.h"

namespace NCL
{
	namespace CSC8503
	{
		
		class SpinningBlockConstraint : public Constraint
		{
		public:
			SpinningBlockConstraint(GameObject* obj, Vector3 piv, float dist)
			{
				object = obj;
				pivot = piv;
				distance = dist;
			}
			~SpinningBlockConstraint() {}

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* object;
			Vector3 pivot;

			float distance;

			void Rotation(float dt, PhysicsObject* physObj, Vector3 currentAxis, Vector3 targetAxis);
			void Distance(float dt, PhysicsObject* physObj, Vector3 targetPos, float dist);
		};
	}
}