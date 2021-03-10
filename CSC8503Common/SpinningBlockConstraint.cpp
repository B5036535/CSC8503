#include "SpinningBlockConstraint.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "../CSC8503Common/Debug.h"

using namespace NCL;
using namespace CSC8503;

void NCL::CSC8503::SpinningBlockConstraint::UpdateConstraint(float dt)
{
	
	Vector3 relativePos = pivot - object->GetTransform().GetPosition();

	Vector3 forward = object->GetTransform().GetForward();

	Vector3 left = object->GetTransform().GetLeft();
	Vector3 pivotDirection = relativePos.Normalised();

	Vector3 up = object->GetTransform().GetUp();
	Vector3 trueUp = Vector3(0, 1, 0);

	PhysicsObject* physObj = object->GetPhysicsObject();

	Rotation(dt, physObj, left, pivotDirection);
	Rotation(dt, physObj, up, trueUp);
	Distance(dt, physObj, pivot, distance);
	Vector3 targetYPos = object->GetTransform().GetPosition();
	targetYPos.y = pivot.y;
	Distance(dt, physObj, targetYPos, distance);

	physObj->SetLinearVelocity(forward * 7);
}

void NCL::CSC8503::SpinningBlockConstraint::Rotation(float dt, PhysicsObject* physObj,Vector3 currentOrientation, Vector3 targetAxis)
{
	float dot = Vector3::Dot(currentOrientation.Normalised(), targetAxis);
	float angle = acos(dot) * 180 / M_PI;

	if (angle >= 0.0f)
	{
		Vector3 axis = Vector3::Cross(currentOrientation, targetAxis);
		float biasFactor = 0.01f;
		float bias = -(biasFactor / dt) * angle;
		float lambda = -(dot + bias) / physObj->GetInverseMass();
		physObj->ApplyAngularImpulse(axis * lambda);
	}
}

void NCL::CSC8503::SpinningBlockConstraint::Distance(float dt, PhysicsObject* physObj, Vector3 target, float dist)
{
	Vector3 relativePos = target - object->GetTransform().GetPosition();
	float currentDistance = relativePos.Length();
	float offset = dist - currentDistance;

	if (abs(offset) > 0.0f)
	{
		//std::cout << "offset = " << offset << std::endl;
		Vector3 offsetDir = relativePos.Normalised();

		float velocityDot = Vector3::Dot(physObj->GetLinearVelocity(), offsetDir);
		float biasFactor = 0.1f;
		float bias = -(biasFactor / dt) * offset;
		float lambda = -(velocityDot + bias) / physObj->GetInverseMass();

		Vector3 aImpulse = offsetDir * lambda;
		Vector3 bImpulse = -offsetDir * lambda;

		physObj->ApplyLinearImpulse(aImpulse);
	}
}
