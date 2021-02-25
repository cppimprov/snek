
#include "snek_die.hpp"
#include "snek_log.hpp"
#include "snek_sdl_context.hpp"
#include "snek_sdl_window.hpp"
#include "snek_sdl_renderer.hpp"
#include "snek_sdl_mixer_context.hpp"

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
		
		// ...
		
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

		auto mixer_context = sdl::mixer_context();
		// load and play chunks...

		while (true)
		{
			// ... input
			// ... update

			renderer.clear({ 255, 240, 16, 255 });

			// ... render
			renderer.draw_quad_fill(window.get_size() / 4, window.get_size() / 4, { 255, 0, 0, 255 }, sdl::renderer::blend_mode::NONE);
			renderer.draw_quad_outline(window.get_size() / 2, window.get_size() / 4, { 0, 255, 0, 255 }, sdl::renderer::blend_mode::NONE);

			renderer.present();
		}
	}

	log_info("done!");

	return EXIT_SUCCESS;
}
