#pragma once
#include "../../Common/Window.h"
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/Timer.h"
#include "../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/NavigationGrid.h"

namespace NCL {
	namespace CSC8503 {
		class Enemy : public GameObject
		{
		public:
			Enemy(Vector3 pos, NavigationGrid* g);
			~Enemy() { delete timer; }

			float yaw;

			Timer* timer;
	
			NavigationGrid* grid;
			NavigationPath path;

			StateMachine* stateMachine;

			virtual void Respawn() override
			{
				transform.SetPosition(spawnPoint);
				currentNode = path.waypoints.size() - 1;
			}
		protected:
			float speed;
			float jump;

			int currentNode;
			const float waypointDist = 20.0f;
		};
	}
}