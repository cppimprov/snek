#include "snek_game_gamestate.hpp"

namespace snek
{
	
	namespace game
	{
		
		void run_state(gamestate state, app& app)
		{
			while (state.m_function)
				state = state.m_function(app);
		}
		
	} // game
	
} // snek
