
#include "snek_die.hpp"
#include "snek_game_app.hpp"
#include "snek_game_assets.hpp"
#include "snek_game_gamestate.hpp"
#include "snek_load_image.hpp"
#include "snek_log.hpp"
#include "snek_sdl_blendmode.hpp"
#include "snek_sdl_context.hpp"
#include "snek_sdl_mixer_context.hpp"
#include "snek_sdl_mixer_chunk.hpp"
#include "snek_sdl_window.hpp"
#include "snek_sdl_renderer.hpp"
#include "snek_sdl_surface.hpp"
#include "snek_sdl_texture.hpp"

#include <glm/glm.hpp>

#include <SDL.h>
#include <SDL_mixer.h>
#include <stb_image.h>

#include <algorithm>
#include <chrono>
#include <deque>
#include <iterator>
#include <random>
#include <vector>

namespace snek
{

	namespace game
	{

		gamestate do_gameplay(app& app)
		{
			auto const start_sound = &app.m_assets.m_sounds.at("start");
			app.m_mixer_context.play_once(*start_sound);

			auto const cell_count = glm::i32vec2{ 24, 24 };
			auto const cell_size = app.m_window.get_size() / cell_count;

			auto const LEFT  = glm::i32vec2{ -1,  0 };
			auto const RIGHT = glm::i32vec2{  1,  0 };
			auto const UP    = glm::i32vec2{  0, -1 };
			auto const DOWN  = glm::i32vec2{  0,  1 };

			auto const snake_start_length = 5;
			auto const snake_start_pos = glm::i32vec2{ 5, 5 };
			auto const snake_start_dir = RIGHT;

			auto snake = std::deque<glm::i32vec2>();
			for (auto x = 0; x != snake_start_length; ++x)
				snake.push_back({ snake_start_pos.x + x, snake_start_pos.y });

			auto constexpr i32vec2_less = [] (glm::i32vec2 a, glm::i32vec2 b)
			{ 
				auto const a_list = { a.y, a.x };
				auto const b_list = { b.y, b.x };
				return std::lexicographical_compare(a_list.begin(), a_list.end(), b_list.begin(), b_list.end());
			};

			auto all_cells = std::vector<glm::i32vec2>();
			all_cells.reserve(cell_count.x * cell_count.y);

			for (auto y = 0; y != cell_count.y; ++y)
				for (auto x = 0; x != cell_count.x; ++x)
					all_cells.push_back({ x, y });
			
			std::sort(all_cells.begin(), all_cells.end(), i32vec2_less); // (should do nothing!)
			
			auto snake_dir = snake_start_dir;
			auto const snake_color = glm::u8vec4{ 0, 128, 0, 255 };
			auto const snake_head_color = glm::u8vec4(255);
			auto const snake_head_border_size = 6;

			auto const bg_color = glm::u8vec4{ 191, 191, 191, 255 };
			
			//auto score = 0;
			//auto const food_score = 10;

			auto get_food_pos = [&] (std::mt19937& rng, std::vector<glm::i32vec2> const& all_cells, std::deque<glm::i32vec2> snake_copy)
			{
				std::sort(snake_copy.begin(), snake_copy.end(), i32vec2_less);

				auto const size = all_cells.size() - snake_copy.size();
				die_if(size == 0);

				auto empty = std::vector<glm::i32vec2>();
				empty.reserve(size);

				std::set_difference(all_cells.begin(), all_cells.end(), snake_copy.begin(), snake_copy.end(), std::back_inserter(empty), i32vec2_less);

				die_if(empty.empty());
				auto const dist = std::uniform_int_distribution<std::size_t>(0, empty.size() - 1);

				return empty[dist(rng)];
			};

			auto rng = std::mt19937(std::random_device()());
			auto food_pos = get_food_pos(rng, all_cells, snake);

			auto const food_color = glm::u8vec4{ 255, 106, 0, 255 };

			while (true)
			{
				// input:
				{
					SDL_Event e;

					while (SDL_PollEvent(&e))
					{
						if (e.type == SDL_QUIT)
							return { };

						if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
							return { };

						if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_W) snake_dir = UP;
						if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_UP) snake_dir = UP;
						if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_S) snake_dir = DOWN;
						if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_DOWN) snake_dir = DOWN;
						if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_A) snake_dir = LEFT;
						if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_LEFT) snake_dir = LEFT;
						if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_D) snake_dir = LEFT;
						if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_RIGHT) snake_dir = RIGHT;
					}
				}

				// update:
				{
					// ... update snake

					// check time
					// move head + grow snake
					// check for collision w/ snake
						// restart game
					// check for collision w/ food
						// speed up
						// check for "win" -> no squares left
							// restart game
						// update food (check we have empty squares first)
				}

				// render:
				{
					app.m_renderer.clear(bg_color);

					// draw snake
					{
						// body
						for (auto p : snake)
							app.m_renderer.draw_quad_fill(p * cell_size, cell_size, snake_color, sdl::blend_mode::NONE);

						// head
						app.m_renderer.draw_quad_fill(snake.back() * cell_size + snake_head_border_size / 2, cell_size - snake_head_border_size, snake_head_color, sdl::blend_mode::NONE);
					}

					// draw food
					{
						app.m_renderer.draw_quad_fill(food_pos * cell_size, cell_size, food_color, sdl::blend_mode::NONE);
					}

					// ... draw score

					app.m_renderer.present();
				}
			}

			return { };
		}

		gamestate do_countdown(app& app)
		{
			auto const bg_color = glm::u8vec4{ 191, 191, 191, 255 };

			auto count = 3;
			auto count_texture = &app.m_assets.m_textures.at(std::to_string(count));
			auto const count_sound = &app.m_assets.m_sounds.at("count");

			app.m_mixer_context.play_once(*count_sound);

			auto const count_period = std::chrono::seconds(1);
			auto count_period_start = std::chrono::high_resolution_clock::now();

			while (true)
			{
				// input:
				{
					SDL_Event e;

					while (SDL_PollEvent(&e))
					{
						if (e.type == SDL_QUIT)
							return { };

						if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
							return { };
					}
				}

				// update:
				{
					auto const now = std::chrono::high_resolution_clock::now();

					if (now - count_period_start >= count_period)
					{
						--count;

						if (count == 0)
							return { do_gameplay };

						count_texture = &app.m_assets.m_textures.at(std::to_string(count));
						app.m_mixer_context.play_once(*count_sound);

						count_period_start = now;
					}
				}

				// render:
				{
					app.m_renderer.clear(bg_color);

					app.m_renderer.draw_quad_texture({ 0, 0 }, app.m_window.get_size(), *count_texture, { 0, 0 }, count_texture->get_size());

					app.m_renderer.present();
				}
			}

			return { };
		}

	} // game
	
} // snek

#include <cstdlib>

int main(int, char*[])
{
	using namespace snek;

	{
		auto app = game::app();
		game::run_state({ game::do_countdown }, app);
	}

	log_info("done!");

	return EXIT_SUCCESS;
}
