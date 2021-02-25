#include "snek_sdl_renderer.hpp"

#include "snek_die.hpp"
#include "snek_log.hpp"

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
		
		SDL_BlendMode renderer::get_sdl_blendmode(blend_mode mode)
		{
			if (mode == blend_mode::NONE) return SDL_BLENDMODE_NONE;
			if (mode == blend_mode::BLEND) return SDL_BLENDMODE_BLEND;
			if (mode == blend_mode::ADDITIVE) return SDL_BLENDMODE_ADD;
			if (mode == blend_mode::MODULATE) return SDL_BLENDMODE_MOD;

			die();
			return SDL_BLENDMODE_NONE;
		}

	} // sdl

} // snek
