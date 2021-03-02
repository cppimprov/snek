
#include "snek_game.hpp"
#include "snek_game_app.hpp"
#include "snek_log.hpp"

#include <cstdlib>

int main(int, char*[])
{
	using namespace snek;

	{
		auto app = game::app();
		game::run_state({ game::do_countdown }, app);
	}

	log_info("done!");

	return EXIT_SUCCESS;
}