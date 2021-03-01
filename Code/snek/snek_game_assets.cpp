#include "snek_game_assets.hpp"

#include "snek_die.hpp"
#include "snek_load_image.hpp"
#include "snek_log.hpp"

namespace snek
{
	
	namespace game
	{
		
		assets load_assets(sdl::renderer const& renderer, std::vector<std::string> texture_files, std::vector<std::string> sound_files)
		{
			auto out = assets();

			// load textures:
			{
				for (auto const& file : texture_files)
				{
					auto surface = load_sdl_surface_from_file_rgba("data/" + file + ".png");
					auto texture = sdl::texture(renderer, surface);
					texture.set_blend_mode(sdl::blend_mode::BLEND);

					if (!out.m_textures.insert({ file, std::move(texture) }).second)
					{
						log_error("load_assets(): duplicate texture id: " + file);
						die();
					}
				}
			}

			// load sounds:
			{
				for (auto const& file : sound_files)
				{
					auto chunk = sdl::mixer_chunk("data/" + file + ".wav");

					if (!out.m_sounds.insert({ file, std::move(chunk) }).second)
					{
						log_error("load_assets(): duplicate sound id: " + file);
						die(); 
					}
				}
			}

			return out;
		}
		
	} // game
	
} // snek