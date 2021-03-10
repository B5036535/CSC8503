#pragma once
#include "..\CSC8503Common\StateTransition.h"
#include "..\CSC8503Common\StateMachine.h"
#include "..\CSC8503Common\State.h"

namespace NCL {
	namespace CSC8503 {
		class Timer
		{
		public:
			Timer()
			{
				points = 1000;
				timer = 1.0f;
				won = false;
				stateMachine = new StateMachine();

				State* game_playing = new State
				(
					[&](float dt) -> void
					{
						timer -= dt;
						if (timer <= 0.0f)
						{
							timer += 1.0f;
							points -= 10;
						}
					}
				);


				State* game_ended = new State
				(
					[&](float dt) -> void
					{
						//win game
					}
				);

				State* game_paused = new State
				(
					[&](float dt) -> void
					{
						//do nowte
					}
				);

				stateMachine->AddState(game_playing);
				stateMachine->AddState(game_ended);
				stateMachine->AddState(game_paused);

				stateMachine->AddTransition(new StateTransition(game_playing, game_paused,
					[&]() -> bool
				{
					return paused;
				}
				));

				stateMachine->AddTransition(new StateTransition(game_playing, game_ended,
					[&]() -> bool
				{
					return points <= 0 || won;
				}
				));

				stateMachine->AddTransition(new StateTransition(game_paused, game_playing,
					[&]() -> bool
				{
					return !paused;
				}
				));
			}
			~Timer() {}

			int points;
			bool paused;
			StateMachine* stateMachine;

			void PowerUp()
			{
				points += 25;
			}

		protected:
			float timer;
			bool won;
		};
	}
}