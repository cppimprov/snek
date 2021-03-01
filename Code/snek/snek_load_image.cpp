#include "snek_load_image.hpp"

#include "snek_die.hpp"
#include "snek_log.hpp"

#include <stb_image.h>
#include <stb_image_write.h>

namespace snek
{
	
	sdl::surface load_sdl_surface_from_file_rgba(std::string const& file)
	{
		stbi_set_flip_vertically_on_load(true);

		int width, height, channels;
		auto pixels = stbi_load(file.c_str(), &width, &height, &channels, 0);

		if (!pixels)
		{
			log_error("stbi_load() failed: " + std::string(stbi_failure_reason()));
			die();
		}

		if (channels != 4)
		{
			log_error("load_sdl_surface_from_file_rgba(): image is not rgba.");
			die();
		}

		auto surface = sdl::surface({ width, height });

		{
			auto stb_surface_handle = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, 32, width * channels * sizeof(stbi_uc), SDL_PIXELFORMAT_RGBA32);

			if (!stb_surface_handle)
			{
				log_error("load_sdl_surface_from_file_rgba(): SDL_CreateRGBSurfaceWithFormatFrom() failed: " + std::string(SDL_GetError()));
				die();
			}

			auto stb_surface = sdl::surface(stb_surface_handle);
			stb_surface.set_blend_mode(sdl::blend_mode::NONE);

			sdl::blit_surface(stb_surface, { 0, 0 }, { width, height }, surface, { 0, 0 }, { width, height });
		}

		stbi_image_free(pixels);

		return surface;
	}
	
} // snek
