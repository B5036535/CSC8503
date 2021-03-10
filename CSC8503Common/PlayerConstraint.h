#pragma once
#include "Constraint.h"
#include "PhysicsObject.h"

namespace NCL
{
	namespace CSC8503
	{
		class Player;

		class PlayerConstaint : public Constraint
		{
		public:
			PlayerConstaint(Player* p)
			{
				player = p;

			}
			~PlayerConstaint();

			void UpdateConstraint(float dt) override;

		protected:

			Player* player;

		};
	}
}