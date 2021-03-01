#pragma once

#include "snek_sdl_blendmode.hpp"
#include "snek_sdl_object_handle.hpp"

#include <glm/glm.hpp>
#include <SDL.h>

namespace snek
{
	
	namespace sdl
	{
		
		class surface : public object_handle<SDL_Surface>
		{
		public:

			surface() = default;
			explicit surface(SDL_Surface* surface);
			explicit surface(glm::i32vec2 size);

			glm::i32vec2 get_size() const;

			void set_blend_mode(blend_mode mode);
		};

		void blit_surface(surface const& src, glm::i32vec2 src_pos, glm::i32vec2 src_size, surface const& dst, glm::i32vec2 dst_pos, glm::i32vec2 dst_size);
		
	} // sdl
	
} // snek
