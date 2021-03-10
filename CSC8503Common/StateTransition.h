#pragma once
#include <functional>
namespace NCL {
	namespace CSC8503 {

		class State;
		typedef std::function<bool()> StateTransitionFunction;
		class StateTransition
		{
		public:
			StateTransition(State* source, State* dest, StateTransitionFunction f)
			{
				soureState = source;
				destinationState = dest;
				function = f;
			}

			bool CanTransition() const { return function(); }
			State* GetDestState() const { return destinationState; }
			State* GetSourceState() const { return soureState; }
		protected:
			State* soureState;
			State* destinationState;
			StateTransitionFunction function;
		};
	}
}

