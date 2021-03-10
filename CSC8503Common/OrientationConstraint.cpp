#include "OrientationConstraint.h"
#include "../../Common/Vector3.h"
#include "../../Common/Matrix3.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace NCL;
using namespace CSC8503;
using namespace NCL::Maths;

void OrientationConstraint::UpdateConstraint(float dt)
{
	Vector3 currentOrientation = objectA->GetTransform().GetUp();

	float dot = Vector3::Dot(currentOrientation.Normalised(), Vector3(0, 1, 0));
	float cosine = dot / (currentOrientation.Length() * Vector3(0, 1, 0).Length());
	float angle = acos(cosine) * 180 / M_PI;


	if (angle >= 1.0f)
	{
		PhysicsObject* physObj = objectA->GetPhysicsObject();
		Vector3 axis = Vector3::Cross(currentOrientation, Vector3(0, 1, 0));
		float biasFactor = 0.01f;
		float bias = -(biasFactor / dt) * angle;
		float lambda = -(dot + bias) / physObj->GetInverseMass();
		physObj->ApplyAngularImpulse(axis * lambda);
	}
}