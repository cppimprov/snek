#pragma once

#include "snek_sdl_object_handle.hpp"

#include <glm/glm.hpp>
#include <SDL.h>

namespace snek
{
	
	namespace sdl
	{

		class renderer
		{
		public:

			enum class blend_mode { NONE, BLEND, ADDITIVE, MODULATE };

			renderer() = default;
			explicit renderer(SDL_Window* window);

			renderer(renderer const&) = delete;
			renderer& operator=(renderer const&) = delete;

			renderer(renderer&& other) = default;
			renderer& operator=(renderer&& other) = default;

			void clear(glm::u8vec4 color) const;

			void draw_quad_fill(glm::i32vec2 position, glm::i32vec2 size, glm::u8vec4 color, blend_mode blending);
			void draw_quad_outline(glm::i32vec2 position, glm::i32vec2 size, glm::u8vec4 color, blend_mode blending);

			void present() const;

			bool is_valid() const;
			void destroy();

		private:

			static SDL_BlendMode get_sdl_blendmode(blend_mode mode);

			object_handle<SDL_Renderer> m_handle;
		};

	} // sdl
	
} // snek
