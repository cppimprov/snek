#pragma once

#include "snek_sdl_blendmode.hpp"
#include "snek_sdl_object_handle.hpp"

#include <glm/glm.hpp>
#include <SDL.h>

namespace snek
{
	
	namespace sdl
	{
		
		class texture : public object_handle<SDL_Texture>
		{
		public:

			explicit texture(SDL_Renderer* renderer, SDL_Surface* surface);

			glm::i32vec2 get_size() const;

			void set_blend_mode(blend_mode mode);
		};
		
	} // sdl
	
} // snek