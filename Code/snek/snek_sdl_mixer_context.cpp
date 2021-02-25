#include "snek_sdl_mixer_context.hpp"

#include "snek_die.hpp"
#include "snek_log.hpp"

#include <SDL_mixer.h>

#include <string>

namespace snek
{
	
	namespace sdl
	{
		
		mixer_context::mixer_context()
		{
			auto init_flags = 0;
			if (Mix_Init(init_flags) != init_flags)
			{
				log_error("Mix_Init() failed: " + std::string(Mix_GetError()));
				die();
			}

			if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) != 0)
			{
				log_error("Mix_OpenAudio() failed: " + std::string(Mix_GetError()));
				die();
			}
		}

		mixer_context::~mixer_context()
		{
			Mix_CloseAudio();
			Mix_Quit();
		}
		
	} // sdl
	
} // snek
