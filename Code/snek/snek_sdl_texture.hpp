#pragma once

#include "snek_sdl_blendmode.hpp"
#include "snek_sdl_object_handle.hpp"

#include <glm/glm.hpp>
#include <SDL.h>

namespace snek
{
	
	namespace sdl
	{

		class renderer;
		class surface;
		
		class texture : public object_handle<SDL_Texture>
		{
		public:

			texture() = default;
			explicit texture(renderer const& renderer, surface const& surface);

			glm::i32vec2 get_size() const;

			void set_blend_mode(blend_mode mode);
		};
		
	} // sdl
	
} // snek