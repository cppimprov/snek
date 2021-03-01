#pragma once

#include "snek_sdl_object_handle.hpp"

#include <SDL_mixer.h>

#include <string>

namespace snek
{
	
	namespace sdl
	{
		
		class mixer_chunk : public object_handle<Mix_Chunk>
		{
		public:

			mixer_chunk() = default;
			explicit mixer_chunk(std::string const& file);
		};
		
	} // sdl
	
} // snek