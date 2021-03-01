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

		class renderer
		{
		public:

			renderer() = default;
			explicit renderer(SDL_Window* window);

			renderer(renderer const&) = delete;
			renderer& operator=(renderer const&) = delete;

			renderer(renderer&& other) = default;
			renderer& operator=(renderer&& other) = default;

			void clear(glm::u8vec4 color) const;

			void draw_quad_fill(glm::i32vec2 position, glm::i32vec2 size, glm::u8vec4 color, blend_mode blending);
			void draw_quad_outline(glm::i32vec2 position, glm::i32vec2 size, glm::u8vec4 color, blend_mode blending);
			void draw_quad_texture(glm::i32vec2 dst_pos, glm::i32vec2 dst_size, texture const& src, glm::i32vec2 src_pos, glm::i32vec2 src_size);

			void present() const;

			bool is_valid() const;
			void destroy();
			
			SDL_Renderer* get_handle() const;

		private:

			object_handle<SDL_Renderer> m_handle;
		};

	} // sdl
	
} // snek
