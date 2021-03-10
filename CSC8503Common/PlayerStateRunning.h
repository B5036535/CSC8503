#pragma once
#include "../../Common/Window.h"
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/Timer.h"
#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PlayerStateJumping.h"
#include "../CSC8503Common/Debug.h"

namespace NCL {
	namespace CSC8503 {
		class PlayerStateRunning : public PushdownState
		{
		public:
			PlayerStateRunning(Player* player, float speed, float jump)
			{
				playerObj = player;
				playerPhys = player->GetPhysicsObject();
				movementForce = speed;
				jumpForce = jump;
			}
			~PlayerStateRunning() {};
		protected:
			Player* playerObj;
			PhysicsObject* playerPhys;
			float movementForce;
			float jumpForce;

			PushdownResult OnUpdate(float dt, PushdownState** newState) override
			{
				playerObj->yaw = playerObj->GetTransform().GetOrientation().ToEuler().y;

				float x = Window::GetMouse()->GetRelativePosition().x;
				if (abs(x) > 0.0f)
				{
					playerPhys->ApplyAngularImpulse(-Vector3(0,1,0) * x * 0.2);
				}
				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
				{
					playerPhys->AddForce(-playerObj->GetTransform().GetForward() * movementForce * dt);
				}
				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
				{
					playerPhys->AddForce(-playerObj->GetTransform().GetLeft() * movementForce * dt);
				}
				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
				{
					playerPhys->AddForce(playerObj->GetTransform().GetForward() * movementForce * dt);
				}
				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
				{
					playerPhys->AddForce(playerObj->GetTransform().GetLeft() * movementForce * dt);
				}
				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE))
				{
					playerObj->canJump = false;
					playerPhys->AddForce(Vector3(0, 1, 0) * jumpForce);
					*newState = new PlayerStateJumping(playerObj, movementForce * 0.8f);
					return PushdownResult::Push;
				}
				return PushdownResult::NoChange;
			}
		};
	}
}