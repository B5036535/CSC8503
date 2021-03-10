#pragma once
#include "../../Common/Window.h"
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/Timer.h"
#include "../CSC8503Common/PushdownMachine.h"


namespace NCL {
	namespace CSC8503 {
		class Player : public GameObject
		{
		public:
			Player(Vector3 pos);
			~Player();

			float yaw;
			bool canJump;

			Timer* timer;
			PushdownMachine* movementMachine;

			virtual void OnCollisionBegin(GameObject* otherObject) override;


		protected:
			
			float speed;
			float jump;

		};
	}
}