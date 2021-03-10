#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"
#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "../CSC8503Common/BehaviourAction.h"
#include "../CSC8503Common/BehaviourSequence.h"

#include "TutorialGame.h"
#include "../CSC8503Common/BehaviourSelector.h"

using namespace NCL;
using namespace CSC8503;

class PauseScreen : public PushdownState
{
public:
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::U))
		{
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}

	void OnAwake() override
	{
		std::cout << "Press U to unpause game!" << std::endl;
	}
};

class GameScreen : public PushdownState
{
public:
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		pauseReminder -= dt;

		if (pauseReminder < 0)
		{
			std::cout << "Coins mined: " << coinsMined << std::endl;
			std::cout << "Press P to pause game, or F1 to return to the main menu!" << std::endl;

			pauseReminder += 1.0f;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::P))
		{
			*newState = new PauseScreen();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F1))
		{
			std::cout << "Returning to main menu!" << std::endl;
			return PushdownResult::Pop;
		}
		if (rand() % 7 == 0)
		{
			coinsMined++;
		}
		return PushdownResult::NoChange;
	}

	void OnAwake() override
	{
		std::cout << "Preparing to mine coins!" << std::endl;
	}

protected:
	int coinsMined = 0;
	float pauseReminder = 1.0f;
};

class IntroScreen : public PushdownState
{
public:
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE))
		{
			*newState = new GameScreen();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}

	void OnAwake() override
	{
		std::cout << "Welcome to a really awesome game" << std::endl;
		std::cout << "Press space to begin or escape to quit" << std::endl;
	}
};

void TestStateMachine()
{
	StateMachine* testMachine = new StateMachine();
	int data = 0;


	State* A = new State([&](float dt)->void
	{
		std::cout << "I'm in state A!" << std::endl;
		data++;
	});

	State* B = new State([&](float dt)-> void
	{
		std::cout << "I'm in state B!" << std::endl;
		data--;
	});

	StateTransition* stateAB = new StateTransition(A, B, [&](void)->bool
	{
		return data > 10;
	});

	StateTransition* stateBA = new StateTransition(B, A, [&](void)->bool
	{
		return data < 0;
	});

	testMachine->AddState(A);
	testMachine->AddState(B);
	testMachine->AddTransition(stateAB);
	testMachine->AddTransition(stateBA);

	for (int i = 0; i < 100; i++)
	{
		testMachine->Update(1.0f);
	}
}

vector<Vector3> testNodes;



void TestPushdownautomata(Window* w)
{
	PushdownMachine machine(new IntroScreen());

	while (w->UpdateWindow())
	{
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (!machine.Update(dt))
			return;
	}
}

void TestBehaviourTree()
{
	float behaviourTimer;
	float distanceToTarget;

	BehaviourAction* findKey = new BehaviourAction("Find Key", [&](float dt, BehaviourState state)->BehaviourState
	{
		if (state == Initialise)
		{
			std::cout << "Looking for a key!" << std::endl;
			behaviourTimer = rand() % 100;
			state = Ongoing;
		}
		else if (state == Ongoing)
		{
			behaviourTimer -= dt;
			if (behaviourTimer <= 0.0f)
			{
				std::cout << "Found a key!" << std::endl;
				return Success;
			}
		}
		return state;
	});

	BehaviourAction* goToRoom = new BehaviourAction("Go To Room", [&](float dt, BehaviourState state)->BehaviourState
	{
		if (state == Initialise)
		{
			std::cout << "Going to the loot room" << std::endl;
			state = Ongoing;
		}
		else if (state == Ongoing)
		{
			distanceToTarget -= dt;
			if (distanceToTarget <= 0.0f)
			{
				std::cout << "Reached room" << std::endl;
				return Success;
			}
		}
		return state;
	});

	BehaviourAction* openDoor = new BehaviourAction("Open Door", [&](float dt, BehaviourState state)->BehaviourState
	{
		if (state == Initialise)
		{
			std::cout << "Opening Door" << std::endl;
			state = Success;
		}
		return state;
	});

	BehaviourAction* lookForTreasure = new BehaviourAction("Look for Treasure", [&](float dt, BehaviourState state)->BehaviourState
	{
		if (state == Initialise)
		{
			std::cout << "Looking for treasure" << std::endl;
			state = Ongoing;
		}
		else if (state == Ongoing)
		{
			bool found = rand() % 2;
			if (found)
			{
				std::cout << "Treasure found" << std::endl;
				return Success;
			}
			std::cout << "Treasure not found" << std::endl;
			return Failure;
		}
		return state;
	});

	BehaviourAction* lookForItems = new BehaviourAction("Look for Items", [&](float dt, BehaviourState state)->BehaviourState
	{
		if (state == Initialise)
		{
			std::cout << "Looking for Items" << std::endl;
			state = Ongoing;
		}
		else if (state == Ongoing)
		{
			bool found = rand() % 2;
			if (found)
			{
				std::cout << "Items found" << std::endl;
				return Success;
			}
			std::cout << "Items not found" << std::endl;
			return Failure;
		}
		return state;
	});

	BehaviourSequence* sequence = new BehaviourSequence("Room Sequence");
	sequence->AddChild(findKey);
	sequence->AddChild(goToRoom);
	sequence->AddChild(openDoor);

	BehaviourSelector* selection = new BehaviourSelector("Loot Selection");
	selection->AddChild(lookForTreasure);
	selection->AddChild(lookForItems);

	BehaviourSequence* rootSequence = new BehaviourSequence("Root Sequence");
	sequence->AddChild(sequence);
	sequence->AddChild(selection);

	for (int i = 0; i < 5; i++)
	{
		rootSequence->Reset();

		behaviourTimer = 0.0f;
		distanceToTarget = rand() % 250;
		BehaviourState state = Ongoing;
		std::cout << "We are going on an adventure" << std::endl;

		while (state == Ongoing)
		{
			state = rootSequence->Execute(1.0f);
		}
		if (state == Success)
		{
			std::cout << "What a successful adventure" << std::endl;
		}
		else if (state == Failure)
		{
			std::cout << "What a waste of time" << std::endl;
		}
	}
}
void TestPathfinding()
{
	NavigationGrid grid("TestGrid3.txt");

	NavigationPath outPath;

	Vector3 startPos(50, 0, 50);
	Vector3 endPos(250, 0, 650);

	bool  found = grid.FindPath(startPos, endPos, outPath);

	Vector3 pos;

	while (outPath.PopWaypoint(pos))
	{
		testNodes.push_back(pos);
	}
}

void DisplayPathFinding()
{
	for (int i = 1; i < testNodes.size(); i++)
	{
		Vector3 a = testNodes[i - 1] + Vector3(0, 7, 0);
		Vector3 b = testNodes[i] + Vector3(0, 7, 0);

		Debug::DrawLine(a, b, Vector4(0, 5, 0, 1));
	}
}

/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/
int main() {
	Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);

	if (!w->HasInitialised()) {
		return -1;
	}	
	srand(time(0));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	TutorialGame* g = new TutorialGame();
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!

	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();

		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		//g->UpdateGame(dt);
		g->gameState->Update(dt);
	}
	Window::DestroyGameWindow();
}
