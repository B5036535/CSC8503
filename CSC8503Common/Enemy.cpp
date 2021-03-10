#include "../CSC8503Common/Enemy.h"
#include "AABBVolume.h"

using namespace NCL;
using namespace CSC8503;

Enemy::Enemy(Vector3 pos, NavigationGrid* g)
{
	tag = GameObjectTag::ENEMY;

	speed = 1000.0f;
	jump = 1000.0f;


	float meshSize = 3.0f;
	float inverseMass = 0.6f;

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.85f, 0.3f) * meshSize);

	SetBoundingVolume((CollisionVolume*)volume);

	GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize)).SetPosition(pos);
	
	SetPhysicsObject(new PhysicsObject(&GetTransform(), GetBoundingVolume()));

	GetPhysicsObject()->SetInverseMass(inverseMass);
	GetPhysicsObject()->InitSphereInertia();
	spawnPoint = transform.GetPosition();
	grid = g;
	grid->FindPath(spawnPoint, Vector3(250, 5, 690), path);
	currentNode = path.waypoints.size() - 1;

	stateMachine = new StateMachine();

	State* stateA = new State([&](float dt)->void
	{
		Vector3 currentWaypointPos = path.waypoints[currentNode];
		Vector3 vect = (currentWaypointPos - transform.GetPosition());
		Vector3 direction = vect.Normalised();
		direction.y = 0.0f;
		physicsObject->AddForce(direction * speed * dt);

		if (vect.Length() <= waypointDist && currentNode >= 0)
			currentNode--;
	});

	stateMachine->AddState(stateA);


	timer = new Timer();
}