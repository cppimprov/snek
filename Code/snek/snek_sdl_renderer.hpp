#pragma once

#include "snek_sdl_blendmode.hpp"
#include "snek_sdl_object_handle.hpp"

#include <glm/glm.hpp>
#include <SDL.h>

namespace snek
{
	
	namespace sdl
	{

		class texture;
		class window;

		class renderer : public object_handle<SDL_Renderer>
		{
		public:

			renderer() = default;
			explicit renderer(window const& window);

			void clear(glm::u8vec4 color) const;

			void draw_quad_fill(glm::i32vec2 position, glm::i32vec2 size, glm::u8vec4 color, blend_mode blending);
			void draw_quad_outline(glm::i32vec2 position, glm::i32vec2 size, glm::u8vec4 color, blend_mode blending);
			void draw_quad_texture(glm::i32vec2 dst_pos, glm::i32vec2 dst_size, texture const& src, glm::i32vec2 src_pos, glm::i32vec2 src_size);

			void present() const;
		};

	} // sdl
	
} // snek
