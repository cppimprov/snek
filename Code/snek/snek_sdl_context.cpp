#include "snek_sdl_context.hpp"

#include "snek_die.hpp"
#include "snek_log.hpp"

#include <SDL.h>

#include <string>

namespace snek
{
	
	namespace sdl
	{
		
		context::context()
		{
			if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
			{
				log_error("SDL_Init() failed: " + std::string(SDL_GetError()));
				die();
			}
		}

		context::~context()
		{
			SDL_Quit();
		}
		
	} // sdl
	
} // snek