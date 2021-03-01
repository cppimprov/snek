#include "snek_sdl_renderer.hpp"

#include "snek_die.hpp"
#include "snek_log.hpp"
#include "snek_sdl_texture.hpp"
#include "snek_sdl_window.hpp"

#include <string>

namespace snek
{

	namespace sdl
	{

		renderer::renderer(window const& window)
		{
			auto handle = SDL_CreateRenderer(window.get_handle(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

			if (!handle)
			{
				log_error("SDL_CreateRenderer() failed: " + std::string(SDL_GetError()));
				die();
			}

			reset(handle, [] (SDL_Renderer* r) { SDL_DestroyRenderer(r); });
		}

		void renderer::clear(glm::u8vec4 color) const
		{
			die_if(!is_valid());

			SDL_SetRenderDrawColor(get_handle(), color.x, color.y, color.z, color.w);
			SDL_RenderClear(get_handle());
		}
		
		void renderer::draw_quad_fill(glm::i32vec2 position, glm::i32vec2 size, glm::u8vec4 color, blend_mode blending)
		{
			die_if(!is_valid());
			
			SDL_SetRenderDrawColor(get_handle(), color.x, color.y, color.z, color.w);
			SDL_SetRenderDrawBlendMode(get_handle(), get_sdl_blendmode(blending));

			auto const rect = SDL_Rect{ position.x, position.y, size.x, size.y };
			SDL_RenderFillRect(get_handle(), &rect);
		}

		void renderer::draw_quad_outline(glm::i32vec2 position, glm::i32vec2 size, glm::u8vec4 color, blend_mode blending)
		{
			die_if(!is_valid());
			
			SDL_SetRenderDrawColor(get_handle(), color.x, color.y, color.z, color.w);
			SDL_SetRenderDrawBlendMode(get_handle(), get_sdl_blendmode(blending));
			
			auto const rect = SDL_Rect{ position.x, position.y, size.x, size.y };
			SDL_RenderDrawRect(get_handle(), &rect);
		}
		
		void renderer::draw_quad_texture(glm::i32vec2 dst_pos, glm::i32vec2 dst_size, texture const& src, glm::i32vec2 src_pos, glm::i32vec2 src_size)
		{
			die_if(!is_valid());
			die_if(!src.is_valid());

			auto const src_rect = SDL_Rect{ src_pos.x, src_pos.y, src_size.x, src_size.y };
			auto const dst_rect = SDL_Rect{ dst_pos.x, dst_pos.y, dst_size.x, dst_size.y };
			SDL_RenderCopy(get_handle(), src.get_handle(), &src_rect, &dst_rect);
		}
		
		void renderer::present() const
		{
			die_if(!is_valid());

			SDL_RenderPresent(get_handle());
		}

	} // sdl

} // snek
