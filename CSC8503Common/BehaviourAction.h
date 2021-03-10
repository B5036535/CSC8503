#pragma once
#include "BehaviourNode.h"
#include <functional>

typedef std::function<BehaviourState(float, BehaviourState)> BehaviourActionFunc;

class BehaviourAction : public BehaviourNode
{
public:
	BehaviourAction(const std::string& nodeName, BehaviourActionFunc f) : BehaviourNode(nodeName)
	{
		func = f;
	}

	BehaviourState Execute(float dt) override
	{
		currentState = func(dt, currentState);
		return currentState;
	}
protected:
	BehaviourActionFunc func;
};