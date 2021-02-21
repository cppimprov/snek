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
				SDL_WINDOWPOS_CENTERED_DISPLAY(1), SDL_WINDOWPOS_CENTERED_DISPLAY(1),
				size.x, size.y,
				get_sdl_window_flags(mode));
			
			if (!handle)
			{
				log_error("SDL_CreateWindow() failed: " + std::string(SDL_GetError()));
				die();
			}

			m_handle.reset(handle, [] (SDL_Window* w) { SDL_DestroyWindow(w); });
		}

		glm::i32vec2 window::get_size() const
		{
			die_if(!is_open());

			auto size = glm::i32vec2();
			SDL_GetWindowSize(m_handle.get(), &size.x, &size.y);
			return size;
		}

		bool window::is_open() const
		{
			return m_handle.is_valid();
		}

		void window::close()
		{
			if (!is_open())
				return;
			
			m_handle.reset();
		}

		SDL_Window* window::get_handle() const
		{
			return m_handle.get();
		}

		Uint32 window::get_sdl_window_flags(display_mode mode)
		{
			return (mode == display_mode::WINDOWED) ? 0 : (mode == display_mode::FULLSCREEN) ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN;
		}
		
	} // sdl
	
} // snek
