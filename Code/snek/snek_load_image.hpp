#pragma once

#include "snek_sdl_surface.hpp"

#include <string>

namespace snek
{
	
	sdl::surface load_sdl_surface_from_file_rgba(std::string const& file);
	
} // snek