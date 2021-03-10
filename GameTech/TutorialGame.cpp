#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/BridgeConstraint.h"
#include "../CSC8503Common/OrientationConstraint.h"
#include "../CSC8503Common/Floor.h"
#include "../CSC8503Common/SpinningBlockConstraint.h"
#include "../CSC8503Common/Platform.h"
#include "../CSC8503Common/PowerUp.h"
#include "../CSC8503Common/MovingBlock.h"
#include "../CSC8503Common/KillPlane.h"
#include "../CSC8503Common/FinishLine.h"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	physics		= new PhysicsSystem(*world);
	physics->UseGravity(true);
	machines	= new StateSystem();


	forceMagnitude	= 10.0f;
	useGravity		= true;
	inSelectionMode = false;
	paused			= false;
	withFriends		= false;
	inMainMenu		= true;
	finished		= false;


	gameState = new StateMachine();

	State* MainMenu = new State([&](float dt)->void
	{

		this->MainMenu();
		renderer->Update(dt);
		Debug::FlushRenderables(dt);
		renderer->Render();
	});

	State* GamePlay = new State([&](float dt)->void
	{
		this->UpdateGame(dt);
	});

	State* Finished = new State([&](float dt)->void
	{
		this->FinishGame();
		renderer->Update(dt);
		Debug::FlushRenderables(dt);
		renderer->Render();
	});


	gameState->AddState(MainMenu);
	gameState->AddState(GamePlay);
	gameState->AddState(Finished);

	gameState->AddTransition(new StateTransition(MainMenu, GamePlay,
		[&]()->bool
	{
		return !inMainMenu;
	}));
	gameState->AddTransition(new StateTransition(GamePlay, MainMenu,
		[&]()->bool
	{
		return inMainMenu;
	}));
	gameState->AddTransition(new StateTransition(GamePlay, Finished,
		[&]()->bool
	{
		return finished;
	}));

	gameState->AddTransition(new StateTransition(Finished, MainMenu,
		[&]()->bool
	{
		return inMainMenu;
	}));


	Debug::SetRenderer(renderer);

	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh"		 , &cubeMesh);
	loadFunc("sphere.msh"	 , &sphereMesh);
	loadFunc("Male1.msh"	 , &charMeshA);
	loadFunc("courier.msh"	 , &charMeshB);
	loadFunc("security.msh"	 , &enemyMesh);
	loadFunc("coin.msh"		 , &bonusMesh);
	loadFunc("capsule.msh"	 , &capsuleMesh);

	basicTex	= (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	grid = new NavigationGrid("TestGrid3.txt");

	InitCamera();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete grid;
	delete machines;
	if (player != nullptr)
		delete player;
	delete world;
	delete gameState;
}

void TutorialGame::UpdateGame(float dt) {


	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}

	UpdateKeys();
	UpdateTimerDisplay();
	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);

		//Debug::DrawAxisLines(lockedObject->GetTransform().GetMatrix(), 2.0f);
	}
	else if (player != nullptr)
	{
		Vector3 objPos = player->GetTransform().GetPosition();
		Vector3 camPos = objPos + player->GetTransform().GetForward() * 20 + Vector3(0, 10, 0);

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		//world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(player->yaw);
	}

	if (paused)
	{
		renderer->DrawString("PAUSED", Vector2(45.f, 50.f));
		renderer->DrawString("Unpause = P", Vector2(80.f, 5.f));
		renderer->DrawString("Quit = 0", Vector2(45.f, 55.f));
		SelectObject();
		DisplaySelectedObjectInfo();
		machines->Update(0);
		physics->Update(0);
		world->UpdateWorld(0);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM0))
		{
			machines->Clear();
			world->ClearAndErase();
			physics->Clear();
			inMainMenu = true;
			paused = false;
			selectionObject = nullptr;
			lockedObject = nullptr;
		}
	}
	else
	{
		if (player->timer->points <= 0)
		{
			finished = true;
		}
		renderer->DrawString("Pause = P", Vector2(80.f, 5.f));
		machines->Update(dt);
		physics->Update(dt);
		world->UpdateWorld(dt);
	}


	
	renderer->Update(dt);

	Debug::FlushRenderables(dt);
	renderer->Render();
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(withFriends); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
		lockedObject	= nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
	{
		paused = !paused;
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void NCL::CSC8503::TutorialGame::UpdateTimerDisplay()
{
	Debug::Print(std::to_string(player->timer->points), Vector2(45, 5));
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();

	Vector3 charForward  = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);
	Vector3 charForward2 = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);

	float force = 100.0f;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		lockedObject->GetPhysicsObject()->AddForce(-rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		//Vector3 worldPos = selectionObject->GetTransform().GetPosition();
		lockedObject->GetPhysicsObject()->AddForce(rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		lockedObject->GetPhysicsObject()->AddForce(fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		lockedObject->GetPhysicsObject()->AddForce(-fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE))
	{
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0, 1000, 0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}

}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld(bool friends) {
	world->ClearAndErase();
	physics->Clear();

	AddGameMap();
	AddPlayerToWorld(Vector3(50, 5, 50));

	if (friends)
	{
		AddEnemyToWorld(Vector3(40, 5, 60));
		AddEnemyToWorld(Vector3(60, 5, 60));
		AddEnemyToWorld(Vector3(40, 5, 40));
		AddEnemyToWorld(Vector3(60, 5, 40));
	}
}




void TutorialGame::AddSpinningBlocks(Vector3 pivot, float distance,bool clockwise)
{

	Vector3 cubeSize = Vector3(5, 5, 5);

	for (int i = 0; i < 4; i++)
	{
		float angle = 90 * i ;
		float x = sin(angle * M_PI / 180);
		float z = cos(angle * M_PI / 180);
		Vector3 start = Vector3(x * distance, 0, z * distance) + pivot;
		Platform* p = new Platform(cubeSize);
		p->SetRenderObject(new RenderObject(&p->GetTransform(), cubeMesh, basicTex, basicShader));
		p->GetTransform().SetPosition(start).SetOrientation(Quaternion(Matrix4::Rotation(angle + 90, Vector3(0, 1, 0))));
		world->AddGameObject(p);

		//Debug::DrawLine(p->GetTransform().GetPosition(), (p->GetTransform().GetLeft() * 10) + p->GetTransform().GetPosition(), Vector4(0, 1, 0, 1), 50.f);
		//Debug::DrawLine(p->GetTransform().GetPosition(), (p->GetTransform().GetForward() * 10) + p->GetTransform().GetPosition(), Vector4(1, 0, 0, 1), 50.f);
		SpinningBlockConstraint* constraint = new SpinningBlockConstraint(p, pivot, distance);
		world->AddConstraint(constraint);
	}
}


void NCL::CSC8503::TutorialGame::AddFinishLine(Vector3 pos, float sizeOfFloor)
{
	AddCubeToWorld(pos + Vector3(-sizeOfFloor, sizeOfFloor /2, 0), Vector3(5.0f, sizeOfFloor/2, 5.0f), 0.f);
	AddCubeToWorld(pos + Vector3(sizeOfFloor, sizeOfFloor /2, 0), Vector3(5.0f, sizeOfFloor/2, 5.0f), 0.f);
	AddCubeToWorld(pos + Vector3(0, sizeOfFloor, 0), Vector3(sizeOfFloor, 10.0f, 10.0f), 0.f);
	FinishLine* finish = new FinishLine(Vector3(sizeOfFloor - 7 , sizeOfFloor / 3 , 3),pos + Vector3(0, sizeOfFloor / 2 - 5, 5), &finished);
	
	world->AddGameObject(finish);
}

void NCL::CSC8503::TutorialGame::AddGameMap()
{
	float sizeOfFloor = 50.f;

	Vector3 planeSize = Vector3(15, 0, 15) * sizeOfFloor;
	planeSize.y = 1.f;

	KillPlane* killPlane = new KillPlane(planeSize);
	//killPlane->SetRenderObject(new RenderObject(&killPlane->GetTransform(), cubeMesh, basicTex, basicShader));

	world->AddGameObject(killPlane);



	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			AddFloorToWorld(Vector3(j * sizeOfFloor * 2, 0, i * sizeOfFloor * 4) + Vector3(50, 0, 50), sizeOfFloor);
			if (i + 1 != 3)
			{
				AddPowerUpToWorld(Vector3(j * sizeOfFloor * 2, 5, i * sizeOfFloor * 4 + sizeOfFloor * 2) + Vector3(50, 0, 50));
			}
		}
		
		bool side = i % 2;
		if (side)
		{
			AddFloorToWorld(Vector3(0, 0, i * sizeOfFloor * 4 + sizeOfFloor * 2) + Vector3(50, 0, 50), sizeOfFloor);

			AddMovingBlockToWorld(Vector3(sizeOfFloor + 75.f, 10, i * sizeOfFloor * 4 + sizeOfFloor * 2 -20.f) + Vector3(50, 0, 50), Vector3(10.f, 4, 10.f));
			AddMovingBlockToWorld(Vector3(-sizeOfFloor + 100.f, 10, i * sizeOfFloor * 4 + sizeOfFloor * 2 + 20.f) + Vector3(50, 0, 50), Vector3(10.f, 4, 10.f));
		}
		else
		{
			AddFloorToWorld(Vector3(4 * sizeOfFloor, 0, i * sizeOfFloor * 4 + sizeOfFloor * 2) + Vector3(50, 0, 50), sizeOfFloor);
			AddMovingBlockToWorld(Vector3(4 * sizeOfFloor + sizeOfFloor + 75.f, 10, i * sizeOfFloor * 4 + sizeOfFloor * 2 - 20.f) + Vector3(50, 0, 50), Vector3(10.f, 4, 10.f));
			AddMovingBlockToWorld(Vector3(4 * sizeOfFloor  - sizeOfFloor + 100.f, 10, i * sizeOfFloor * 4 + sizeOfFloor * 2 + 20.f) + Vector3(50, 0, 50), Vector3(10.f, 4, 10.f));
		}
	}

	AddSpinningBlocks(Vector3(0, 0, sizeOfFloor * 2) + Vector3(50, 0, 50), 20.f, true);
	AddSpinningBlocks(Vector3(sizeOfFloor * 2, 0, sizeOfFloor * 4 + sizeOfFloor * 2) + Vector3(50, 0, 50), 20.f, true);
	AddBridgeToTheWorld(Vector3(4* sizeOfFloor, 0, sizeOfFloor * 4 + sizeOfFloor + 10.f) + Vector3(50, 0, 50));
	AddBridgeToTheWorld(Vector3(2* sizeOfFloor, 0, sizeOfFloor + 10.f) + Vector3(50, 0, 50));

	AddFloorToWorld(Vector3(sizeOfFloor * 4, 0, sizeOfFloor * 8 + sizeOfFloor * 4) + Vector3(50, 0, 50), sizeOfFloor);
	AddFinishLine(Vector3(sizeOfFloor * 4, 0, sizeOfFloor * 8 + sizeOfFloor * 4) + Vector3(50, 0, 50), sizeOfFloor);
}

void NCL::CSC8503::TutorialGame::VisualiseNavGrid()
{

	for(int i = 0; i < grid->numOfNodes; i++)
	{
		if (grid->allNodes[i].type == '.')
		{
			//std::cout << grid->allNodes[i].position << std::endl;
			//Debug::DrawLine(grid->allNodes[i].position + Vector3(5, 10, 0), grid->allNodes[i].position - Vector3(5, -10, 0), Debug::CYAN, 100.f);
			//Debug::DrawLine(grid->allNodes[i].position + Vector3(0, 10, 5), grid->allNodes[i].position - Vector3(0, -10, 5), Debug::CYAN, 100.f);
			GameObject* sphere = new GameObject();

			Vector3 sphereSize = Vector3(1.f, 1.f, 1.f);


			sphere->GetTransform()
				.SetScale(sphereSize)
				.SetPosition(grid->allNodes[i].position + Vector3(0, 10, 0));

			sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));



			world->AddGameObject(sphere);

		}
	}
}

void NCL::CSC8503::TutorialGame::MainMenu()
{

	renderer->DrawString("Main Menu", Vector2(45.f, 5.f));
	renderer->DrawString("Practice Mode = 1", Vector2(25.f, 50.f));
	renderer->DrawString("Full Game = 2", Vector2(65.f, 50.f));

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1))
	{
		withFriends = false;
		inMainMenu = false;
		InitWorld(withFriends);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2))
	{
		withFriends = true;
		inMainMenu = false;
		InitWorld(withFriends);
	}
}

void NCL::CSC8503::TutorialGame::FinishGame()
{
	renderer->DrawString("FINISHED", Vector2(40.f, 5.f));

	player->timer->points > 0 ? renderer->DrawString("YOU WIN!!!", Vector2(35.f, 10.f)) : renderer->DrawString("YOU LOSE!!!", Vector2(35.f, 10.f));
	renderer->DrawString("Players score = " + std::to_string(player->timer->points), Vector2(35.f, 15.f));
	if (withFriends)
	{
		int e = 1;
		for (vector<Enemy*>::iterator i = enemies.begin(); i != enemies.end(); i++)
		{
			renderer->DrawString("Enemy " + std::to_string(e)  +" score = " + std::to_string((*i)->timer->points), Vector2(35.f, 15.f) + Vector2 (0, 5.f) * e);
			e++;
		}
	}

	renderer->DrawString("Quit = 0", Vector2(5.f, 95.f));
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM0))
	{
		machines->Clear();
		world->ClearAndErase();
		physics->Clear();
		inMainMenu = true;
		paused = false;
		selectionObject = nullptr;
		lockedObject = nullptr;
	}


}

void NCL::CSC8503::TutorialGame::AddBridgeToTheWorld(const Vector3& position)
{
	Vector3 cubeSize = Vector3(4, 4, 4);

	float invCubeMass = 0.5;
	int numLinks = 6;
	float maxDistance = 12;
	float maxAngle = 0.2f;
	float cubeDistance = 10.5;

	Vector3 startPos = position;

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	start->tag = GameObjectTag::FLOOR;
	GameObject* end = AddCubeToWorld(startPos + Vector3(0, 0, (numLinks + 2) * cubeDistance), cubeSize, 0);
	end->tag = GameObjectTag::FLOOR;

	GameObject* previous = start;

	for (int i = 0; i < numLinks; i++)
	{
		GameObject* block = AddCubeToWorld(startPos + Vector3(0, 0, (i + 1) * cubeDistance), cubeSize, invCubeMass);
		block->tag = GameObjectTag::FLOOR;
		BridgeConstraint* constraint = new BridgeConstraint(previous, block, maxDistance, maxAngle);
		world->AddConstraint(constraint);
		previous = block;
	}

	BridgeConstraint* constraint = new BridgeConstraint(previous, end, maxDistance, maxAngle);
	world->AddConstraint(constraint);

}

void TutorialGame::AddMovingBlockToWorld(Vector3 position, Vector3 halfsize)
{
	MovingBlock* block = new MovingBlock(position, halfsize);
	block->SetRenderObject(new RenderObject(&block->GetTransform(), cubeMesh, basicTex, basicShader));
	machines->AddMachine(block->stateMachine);
	world->AddGameObject(block);
}


/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position, float size) 
{
	
	Vector3 floorSize	= Vector3(size, 2, size);

	Floor* floor = new Floor(position, floorSize);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass, bool hollow) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	
	hollow ? sphere->GetPhysicsObject()->InitHollowSphereInertia() : sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass) {
	GameObject* capsule = new GameObject();

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(Vector3(radius* 2, halfHeight, radius * 2))
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(capsule);

	return capsule;

}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	//AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

void TutorialGame::InitDefaultFloor() {
	//AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	//float meshSize = 3.0f;
	//float inverseMass = 0.5f;
	//
	//GameObject* character = new GameObject();
	//
	//AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.85f, 0.3f) * meshSize);
	//
	//character->SetBoundingVolume((CollisionVolume*)volume);
	//
	//character->GetTransform()
	//	.SetScale(Vector3(meshSize, meshSize, meshSize))
	//	.SetPosition(position);
	//
	//if (rand() % 2) {
	//	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	//}
	//else {
	//	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	//}
	//character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));
	//
	//character->GetPhysicsObject()->SetInverseMass(inverseMass);
	//character->GetPhysicsObject()->InitSphereInertia();

	Player* character = new Player(position);

	if (rand() % 2) {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	}
	else {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	}
	
	machines->AddMachine(character->movementMachine);
	world->AddGameObject(character);

	player = character;
	machines->AddMachine(player->timer->stateMachine);
	//lockedObject = character;

	return character;
}

GameObject* NCL::CSC8503::TutorialGame::AddPowerUpToWorld(const Vector3& position)
{
	GameObject* powerup = new PowerUp(position, 0.5f);
	powerup->SetRenderObject(new RenderObject(&powerup->GetTransform(), sphereMesh, basicTex, basicShader));
	world->AddGameObject(powerup);
	return powerup;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {

	Enemy* character = new Enemy(position, grid);
	
	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));

	machines->AddMachine(character->stateMachine);
	machines->AddMachine(character->timer->stateMachine);
	world->AddGameObject(character);

	enemies.push_back(character);
	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.25f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(0.25, 0.25, 0.25))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
				lockedObject	= nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		renderer->DrawString("Press Q to change to select mode!", Vector2(5, 85));
	}

	if (lockedObject) {
		renderer->DrawString("Press L to unlock object!", Vector2(5, 80));
	}

	else if(selectionObject){
		renderer->DrawString("Press L to lock selected object object!", Vector2(5, 80));
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		if (selectionObject) {
			if (lockedObject == selectionObject) {
				lockedObject = nullptr;
			}
			else {
				lockedObject = selectionObject;
			}
		}

	}

	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/
void TutorialGame::MoveSelectedObject() {
	renderer->DrawString("Click Force:" + std::to_string(forceMagnitude), Vector2(10, 20));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject)
		return;

	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT))
	{
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		RayCollision closestCollision;

		if (world->Raycast(ray, closestCollision, true))
		{
			//if (closestCollision.node == selectionObject)
			//{
			//	selectionObject->GetPhysicsObject()->AddForce(ray.GetDirection() * forceMagnitude);
			//}
			if (closestCollision.node == selectionObject)
			{
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}

void NCL::CSC8503::TutorialGame::DisplaySelectedObjectInfo()
{
	if (selectionObject != nullptr)
	{
		renderer->DrawString("Object tag = " + std::to_string(selectionObject->tag), Vector2(2.0f, 5.0f));
		Vector3 force = selectionObject->GetPhysicsObject()->GetForce();
		renderer->DrawString("Current force = " + std::to_string(force.x) + ", " + std::to_string(force.y) + ", " + std::to_string(force.z), Vector2(2.f, 7.5f));
		Vector3 linVel = selectionObject->GetPhysicsObject()->GetLinearVelocity();
		renderer->DrawString("Current linear velociti = " + std::to_string(linVel.x) + ", " + std::to_string(linVel.y) + ", " + std::to_string(linVel.z), Vector2(2.f, 10.f));
		Vector3 angVel = selectionObject->GetPhysicsObject()->GetAngularVelocity();
		renderer->DrawString("Current angular velocity = " + std::to_string(angVel.x) + ", " + std::to_string(angVel.y) + ", " + std::to_string(angVel.z), Vector2(2.f, 12.5f));
	}
}
