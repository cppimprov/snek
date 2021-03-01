#pragma once

#include "snek_sdl_object_handle.hpp"

#include <glm/glm.hpp>
#include <SDL.h>

#include <string>

namespace snek
{
	
	namespace sdl
	{
		
		
		class window : public object_handle<SDL_Window>
		{
		public:

			enum class display_mode { WINDOWED, FULLSCREEN, FULLSCREEN_EXCLUSIVE };

			window() = default;
			explicit window(glm::i32vec2 size, std::string const& title, display_mode mode);

			glm::i32vec2 get_size() const;

		private:

			static Uint32 get_sdl_window_flags(display_mode mode);
		};
		
	} // sdl
	
} // snek