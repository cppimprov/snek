#include "snek_sdl_renderer.hpp"

#include "snek_die.hpp"
#include "snek_log.hpp"
#include "snek_sdl_texture.hpp"

#include <string>

namespace snek
{

	namespace sdl
	{

		renderer::renderer(SDL_Window* window)
		{
			auto handle = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

			if (!handle)
			{
				log_error("SDL_CreateRenderer() failed: " + std::string(SDL_GetError()));
				die();
			}

			m_handle.reset(handle, [] (SDL_Renderer* r) { SDL_DestroyRenderer(r); });
		}

		void renderer::clear(glm::u8vec4 color) const
		{
			die_if(!is_valid());

			SDL_SetRenderDrawColor(m_handle.get(), color.x, color.y, color.z, color.w);
			SDL_RenderClear(m_handle.get());
		}
		
		void renderer::draw_quad_fill(glm::i32vec2 position, glm::i32vec2 size, glm::u8vec4 color, blend_mode blending)
		{
			die_if(!is_valid());
			
			SDL_SetRenderDrawColor(m_handle.get(), color.x, color.y, color.z, color.w);
			SDL_SetRenderDrawBlendMode(m_handle.get(), get_sdl_blendmode(blending));

			auto const rect = SDL_Rect{ position.x, position.y, size.x, size.y };
			SDL_RenderFillRect(m_handle.get(), &rect);
		}

		void renderer::draw_quad_outline(glm::i32vec2 position, glm::i32vec2 size, glm::u8vec4 color, blend_mode blending)
		{
			die_if(!is_valid());
			
			SDL_SetRenderDrawColor(m_handle.get(), color.x, color.y, color.z, color.w);
			SDL_SetRenderDrawBlendMode(m_handle.get(), get_sdl_blendmode(blending));
			
			auto const rect = SDL_Rect{ position.x, position.y, size.x, size.y };
			SDL_RenderDrawRect(m_handle.get(), &rect);
		}
		
		void renderer::draw_quad_texture(glm::i32vec2 dst_pos, glm::i32vec2 dst_size, texture const& src, glm::i32vec2 src_pos, glm::i32vec2 src_size)
		{
			die_if(!is_valid());
			die_if(!src.is_valid());

			auto const src_rect = SDL_Rect{ src_pos.x, src_pos.y, src_size.x, src_size.y };
			auto const dst_rect = SDL_Rect{ dst_pos.x, dst_pos.y, dst_size.x, dst_size.y };
			SDL_RenderCopy(m_handle.get(), src.get(), &src_rect, &dst_rect);
		}
		
		void renderer::present() const
		{
			die_if(!is_valid());

			SDL_RenderPresent(m_handle.get());
		}

		bool renderer::is_valid() const
		{
			return m_handle.is_valid();
		}

		void renderer::destroy()
		{
			if (!is_valid())
				return;
			
			m_handle.reset();
		}

		SDL_Renderer* renderer::get_handle() const
		{
			return m_handle.get();
		}

	} // sdl

} // snek
