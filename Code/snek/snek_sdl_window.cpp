#include "snek_sdl_window.hpp"

#include "snek_die.hpp"
#include "snek_log.hpp"

namespace snek
{
	
	namespace sdl
	{
		
		window::window(glm::i32vec2 size, std::string const& title, display_mode mode)
		{
			auto handle = SDL_CreateWindow(
				title.c_str(),
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				size.x, size.y,
				get_sdl_window_flags(mode));
			
			if (!handle)
			{
				log_error("SDL_CreateWindow() failed: " + std::string(SDL_GetError()));
				die();
			}

			reset(handle, [] (SDL_Window* w) { SDL_DestroyWindow(w); });
		}

		glm::i32vec2 window::get_size() const
		{
			die_if(!is_valid());

			auto size = glm::i32vec2();
			SDL_GetWindowSize(get_handle(), &size.x, &size.y);
			return size;
		}

		Uint32 window::get_sdl_window_flags(display_mode mode)
		{
			return (mode == display_mode::WINDOWED) ? 0 : (mode == display_mode::FULLSCREEN) ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN;
		}
		
	} // sdl
	
} // snek
