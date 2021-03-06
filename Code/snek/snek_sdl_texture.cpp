#include "snek_sdl_texture.hpp"

#include "snek_die.hpp"
#include "snek_log.hpp"
#include "snek_sdl_renderer.hpp"
#include "snek_sdl_surface.hpp"

namespace snek
{
	
	namespace sdl
	{
		
		texture::texture(renderer const& renderer, surface const& surface)
		{
			auto handle = SDL_CreateTextureFromSurface(renderer.get_handle(), surface.get_handle());

			if (!handle)
			{
				log_error("SDL_CreateTextureFromSurface() failed: " + std::string(SDL_GetError()));
				die();
			}

			reset(handle, [] (SDL_Texture* t) { SDL_DestroyTexture(t); });
		}

		glm::i32vec2 texture::get_size() const
		{
			die_if(!is_valid());

			auto size = glm::i32vec2();
			if (SDL_QueryTexture(get_handle(), nullptr, nullptr, &size.x, &size.y) != 0)
			{
				log_error("SDL_QueryTexture() failed: " + std::string(SDL_GetError()));
				die();
			}

			return size;
		}

		void texture::set_blend_mode(blend_mode mode)
		{
			die_if(!is_valid());

			SDL_SetTextureBlendMode(get_handle(), get_sdl_blendmode(mode));
		}
		
	} // sdl
	
} // snek
