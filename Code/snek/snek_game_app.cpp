#include "snek_game_app.hpp"


namespace snek
{

	namespace game
	{

		app::app():
			m_context(),
			m_mixer_context(),
			m_window({ 480, 480 }, "snek!", sdl::window::display_mode::WINDOWED),
			m_renderer(m_window),
			m_assets(game::load_assets(m_renderer, { "1", "2", "3", "score" }, { "count", "start", "step", "eat", "die" }))
		{

		}
	
	} // game

} // snek
