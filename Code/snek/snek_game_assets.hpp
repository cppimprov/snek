#pragma once

#include "snek_sdl_mixer_chunk.hpp"
#include "snek_sdl_renderer.hpp"
#include "snek_sdl_texture.hpp"

#include <map>
#include <string>
#include <vector>

namespace snek
{

	namespace game
	{
		
		struct assets
		{
			std::map<std::string, sdl::texture> m_textures;
			std::map<std::string, sdl::mixer_chunk> m_sounds;
		};

		assets load_assets(sdl::renderer const& renderer, std::vector<std::string> texture_files, std::vector<std::string> sound_files);
		
	} // game
	
} // snek