
#include "snek_die.hpp"
#include "snek_load_image.hpp"
#include "snek_log.hpp"
#include "snek_sdl_blendmode.hpp"
#include "snek_sdl_context.hpp"
#include "snek_sdl_mixer_context.hpp"
#include "snek_sdl_window.hpp"
#include "snek_sdl_renderer.hpp"
#include "snek_sdl_surface.hpp"
#include "snek_sdl_texture.hpp"

#include <glm/glm.hpp>

#include <SDL.h>
#include <SDL_mixer.h>
#include <stb_image.h>

#include <cstdlib>
#include <iostream>

namespace snek
{
	
	namespace sdl
	{

	} // sdl
	
} // snek

#include <chrono>
#include <thread>

int main(int, char*[])
{
	using namespace snek;

	{
		auto sdl_context = sdl::context();
		auto window = sdl::window({ 480, 480 }, "snek!", sdl::window::display_mode::WINDOWED);
		auto renderer = sdl::renderer(window.get_handle());

		auto surface = load_sdl_surface_from_file_rgba("data/1.png");
		auto texture = sdl::texture(renderer.get_handle(), surface.get_handle());
		texture.set_blend_mode(sdl::blend_mode::BLEND);

		auto mixer_context = sdl::mixer_context();
		// sdl chunk

		while (true)
		{
			// ... input
			// ... update

			renderer.clear({ 255, 240, 16, 255 });

			// ... render
			renderer.draw_quad_fill(window.get_size() / 4, window.get_size() / 4, { 255, 0, 0, 255 }, sdl::blend_mode::NONE);
			renderer.draw_quad_outline(window.get_size() / 2, window.get_size() / 4, { 0, 255, 0, 255 }, sdl::blend_mode::NONE);
			renderer.draw_quad_texture({ 0, 0 }, texture.get_size(), texture, { 0, 0 }, texture.get_size());

			renderer.present();
		}
	}

	log_info("done!");

	return EXIT_SUCCESS;
}

// todo:
	// change sdl classes to inherit object_handle
	// tidy object_handle:
		// "get" to "get_handle", 
		// "reset" to "destroy",
		// make reset with parameters protected, make constructor with parameters protected?
