#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/StateSystem.h"
#include "../CSC8503Common/Player.h"
#include "../CSC8503Common/Enemy.h"
#include "../CSC8503Common/NavigationGrid.h"


namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();
			StateMachine* gameState;

			virtual void UpdateGame(float dt);

		protected:


			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();
			void UpdateTimerDisplay();

			void InitWorld(bool withFrnds);

			void TestPlayerMovement();

			void TestOrientationConstraint();

			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitDefaultFloor();

			bool SelectObject();
			void MoveSelectedObject();
			void DisplaySelectedObjectInfo();
			void DebugObjectMovement();
			void LockedObjectMovement();
			
			void AddMovingBlockToWorld(Vector3 position, Vector3 halfsize);
			void AddBridgeToTheWorld(const Vector3& position);
			void AddSpinningBlocks(Vector3 pivot, float distance,bool clockwise);
			void AddFinishLine(Vector3 pos, float sizeOfFloor);

			void AddGameMap();

			void VisualiseNavGrid();

			void MainMenu();
			void FinishGame();
			GameObject* AddFloorToWorld(const Vector3& position, float size);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f, bool hollow = false);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddPowerUpToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);

			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			StateSystem*		machines;
			GameWorld*			world;
			NavigationGrid*		grid;

			bool useGravity;
			bool inSelectionMode;
			bool inMainMenu;
			bool paused;
			bool withFriends;
			bool finished;

			float		forceMagnitude;

			Player* player;
			vector<Enemy*> enemies;

			GameObject* selectionObject = nullptr;

			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			//Coursework Meshes
			OGLMesh*	charMeshA	= nullptr;
			OGLMesh*	charMeshB	= nullptr;
			OGLMesh*	enemyMesh	= nullptr;
			OGLMesh*	bonusMesh	= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 10, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}
		};
	}
}

