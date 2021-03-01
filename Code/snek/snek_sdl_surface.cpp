#include "snek_sdl_surface.hpp"

#include "snek_die.hpp"
#include "snek_log.hpp"

namespace snek
{
	
	namespace sdl
	{
		
		surface::surface(SDL_Surface* surface)
		{
			if (surface)
				reset(surface, [] (SDL_Surface* s) { SDL_FreeSurface(s); });
		}

		surface::surface(glm::i32vec2 size)
		{
			auto handle = SDL_CreateRGBSurfaceWithFormat(0, size.x, size.y, 32, SDL_PIXELFORMAT_RGBA32);

			if (!handle)
			{
				log_error("SDL_CreateRGBSurfaceWithFormat() failed: " + std::string(SDL_GetError()));
				die();
			}

			reset(handle, [] (SDL_Surface* s) { SDL_FreeSurface(s); });
		}

		glm::i32vec2 surface::get_size() const
		{
			die_if(!is_valid());

			return { get_handle()->w, get_handle()->h };
		}

		void surface::set_blend_mode(blend_mode mode)
		{
			die_if(!is_valid());

			SDL_SetSurfaceBlendMode(get_handle(), get_sdl_blendmode(mode));
		}


		void blit_surface(surface const& src, glm::i32vec2 src_pos, glm::i32vec2 src_size, surface const& dst, glm::i32vec2 dst_pos, glm::i32vec2 dst_size)
		{
			die_if(!src.is_valid());
			die_if(!dst.is_valid());

			auto const src_rect = SDL_Rect{ src_pos.x, src_pos.y, src_size.x, src_size.y };
			auto dst_rect = SDL_Rect{ dst_pos.x, dst_pos.y, dst_size.x, dst_size.y };

			SDL_BlitSurface(src.get_handle(), &src_rect, dst.get_handle(), &dst_rect);
		}
		
	} // sdl
	
} // snek