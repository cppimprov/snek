#pragma once

#include "snek_game_gamestate.hpp"

namespace snek
{
	
	namespace game
	{

		class app;
		
		gamestate do_countdown(app& app);
		gamestate do_gameplay(app& app);
		
	} // game
	
} // snek
