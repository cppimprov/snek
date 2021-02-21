
#include "snek_die.hpp"
#include "snek_log.hpp"
#include "snek_sdl_context.hpp"
#include "snek_sdl_window.hpp"

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

		std::this_thread::sleep_for(std::chrono::seconds(1)); // temp!

		// ...
	}

	log_info("done!");

	return EXIT_SUCCESS;
}
