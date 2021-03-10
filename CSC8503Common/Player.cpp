#include "Player.h"
#include "AABBVolume.h"
#include "../CSC8503Common/PlayerStateRunning.h"
using namespace NCL;
using namespace CSC8503;

Player::Player(Vector3 pos)
{
	tag = GameObjectTag::PLAYER;

	canJump = true;

	speed = 1000.0f;
	jump = 1000.0f;

	float meshSize = 3.0f;
	float inverseMass = 0.6f;

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.85f, 0.3f) * meshSize);

	SetBoundingVolume((CollisionVolume*)volume);

	GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize)).SetPosition(pos).SetOrientation(Matrix4::Rotation(180,Vector3(0, 1, 0)));
	
	SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));

	GetPhysicsObject()->SetInverseMass(inverseMass);
	GetPhysicsObject()->InitSphereInertia();
	movementMachine = new PushdownMachine(new PlayerStateRunning(this, speed, jump));
	spawnPoint = transform.GetPosition();

	timer = new Timer();
}

Player::~Player()
{
	//Machine deleted at StateSystem
	//delete movementMachine;
	delete timer;
}

void Player::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetObjectTag() == GameObjectTag::FLOOR)
		canJump = true;
	if (otherObject->GetObjectTag() == GameObjectTag::POWERUP)
		timer->PowerUp();
	
}