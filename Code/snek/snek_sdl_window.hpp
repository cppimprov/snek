#pragma once

#include "snek_sdl_object_handle.hpp"

#include <glm/glm.hpp>
#include <SDL.h>

#include <string>

namespace snek
{
	
	namespace sdl
	{
		
		
		class window
		{
		public:

			enum class display_mode { WINDOWED, FULLSCREEN, FULLSCREEN_EXCLUSIVE };

			window() = default;
			window(glm::i32vec2 size, std::string const& title, display_mode mode);

			window(window const&) = delete;
			window& operator=(window const&) = delete;

			window(window&&) = default;
			window& operator=(window&&) = default;

			glm::i32vec2 get_size() const;

			bool is_open() const;
			void close();

			SDL_Window* get_handle() const;

		private:

			static Uint32 get_sdl_window_flags(display_mode mode);

			object_handle<SDL_Window> m_handle;
		};
		
	} // sdl
	
} // snek