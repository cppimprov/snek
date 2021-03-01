#pragma once

#include "snek_game_assets.hpp"
#include "snek_sdl_context.hpp"
#include "snek_sdl_mixer_context.hpp"
#include "snek_sdl_renderer.hpp"
#include "snek_sdl_window.hpp"

namespace snek
{

	namespace game
	{

		class app
		{
		public:
			
			app();

			app(app const&) = delete;
			app& operator=(app const&) = delete;
			
			app(app&&) = delete;
			app& operator=(app&&) = delete;


			sdl::context m_context;
			sdl::mixer_context m_mixer_context;

			sdl::window m_window;
			sdl::renderer m_renderer;

			game::assets m_assets;
		};

	} // game

} // snek
