#include "snek_sdl_mixer_chunk.hpp"

#include "snek_die.hpp"
#include "snek_log.hpp"

namespace snek
{
	
	namespace sdl
	{

		mixer_chunk::mixer_chunk(std::string const& file)
		{
			auto handle = Mix_LoadWAV(file.c_str());

			if (!handle)
			{
				log_error("Mix_LoadWAV() failed: " + std::string(Mix_GetError()));
				die();
			}

			reset(handle, [] (Mix_Chunk* c) { Mix_FreeChunk(c); });
		}

	} // sdl
	
} // snek
