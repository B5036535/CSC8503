#include "BridgeConstraint.h"
#include "../../Common/Vector3.h"
#include "../../Common/Matrix3.h"
#include "GameObject.h"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace NCL;
using namespace CSC8503;
using namespace NCL::Maths;

void BridgeConstraint::UpdateConstraint(float dt)
{
	PhysicsObject* physA = objectA->GetPhysicsObject();
	PhysicsObject* physB = objectB->GetPhysicsObject();


	float constraintMass = physA->GetInverseMass() + physB->GetInverseMass();

	if (constraintMass > 0.0f)
	{
		Distance(dt, constraintMass, physA, physB);
		Rotation(dt, constraintMass, physA, physB, true);
		Rotation(dt, constraintMass, physA, physB, false);
	}
}

void BridgeConstraint::Distance(float dt, float constraintMass, PhysicsObject* physA, PhysicsObject* physB)
{
	Vector3 relativePos = objectA->GetTransform().GetPosition() - objectB->GetTransform().GetPosition();

	float currentDistance = relativePos.Length();
	float offset = distance - currentDistance;

	if (abs(offset) > 0.0f)
	{
		Vector3 offsetDir = relativePos.Normalised();
		Vector3 relativeVelocity = physA->GetLinearVelocity() - physB->GetLinearVelocity();

		float velocityDot = Vector3::Dot(relativeVelocity, offsetDir);
		float biasFactor = 0.01f;
		float bias = -(biasFactor / dt) * offset;
		float lambda = -(velocityDot + bias) / constraintMass;

		Vector3 aImpulse = offsetDir * lambda;
		Vector3 bImpulse = -offsetDir * lambda;

		physA->ApplyLinearImpulse(aImpulse);
		physB->ApplyLinearImpulse(bImpulse);
		
	}
}

void BridgeConstraint::Rotation(float dt, float constraintMass, PhysicsObject* physA, PhysicsObject* physB, bool axis)
{
	Vector3 currentOrientationA = axis ? objectA->GetTransform().GetUp() : objectA->GetTransform().GetForward();
	Vector3 currentOrientationB = axis ? objectB->GetTransform().GetUp() : objectB->GetTransform().GetForward();

	float dot = Vector3::Dot(currentOrientationA, currentOrientationB);	//no need to divide by the lengths of the vectors due to both vectors having a length of 1
	float currentAngle = acos(dot) * 180 / M_PI;
	float offset = angle - currentAngle;

	if (abs(offset) > 0.0f)
	{
		Vector3 axis = Vector3::Cross(currentOrientationA, currentOrientationB);
		float biasFactor = 0.01f;
		float bias = -(biasFactor / dt) * angle;
		float lambda = -(dot + bias) / constraintMass;

		physA->ApplyAngularImpulse(axis * lambda);
		physB->ApplyAngularImpulse(-axis * lambda);
	}
}

