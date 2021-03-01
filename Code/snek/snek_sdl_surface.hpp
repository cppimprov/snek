#pragma once

#include "snek_sdl_blendmode.hpp"
#include "snek_sdl_object_handle.hpp"

#include <glm/glm.hpp>
#include <SDL.h>

namespace snek
{
	
	namespace sdl
	{
		
		class surface
		{
		public:

			surface() = default;
			explicit surface(SDL_Surface* surface);
			explicit surface(glm::i32vec2 size);

			surface(surface const&) = delete;
			surface& operator=(surface const&) = delete;

			surface(surface&&) = default;
			surface& operator=(surface&&) = default;

			glm::i32vec2 get_size() const;

			void set_blend_mode(blend_mode mode);

			bool is_valid() const;
			void destroy();

			SDL_Surface* get_handle() const;

		private:

			object_handle<SDL_Surface> m_handle;
		};

		void blit_surface(surface const& src, glm::i32vec2 src_pos, glm::i32vec2 src_size, surface const& dst, glm::i32vec2 dst_pos, glm::i32vec2 dst_size);
		
	} // sdl
	
} // snek
