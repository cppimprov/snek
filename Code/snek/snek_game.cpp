#include "snek_game.hpp"

#include "snek_die.hpp"
#include "snek_game_app.hpp"

#include <glm/glm.hpp>

#include <algorithm>
#include <chrono>
#include <deque>
#include <iterator>
#include <random>
#include <string>

namespace snek
{
	
	namespace game
	{
		
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
				// input
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

				// update
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

				// render
				{
					app.m_renderer.clear(bg_color);

					app.m_renderer.draw_quad_texture({ 0, 0 }, app.m_window.get_size(), *count_texture, { 0, 0 }, count_texture->get_size());

					app.m_renderer.present();
				}
			}

			return { };
		}

		gamestate do_gameplay(app& app)
		{
			// grid
			auto const cell_count = glm::i32vec2{ 24, 24 };
			auto const cell_size = app.m_window.get_size() / cell_count;

			// directions
			auto const LEFT  = glm::i32vec2{ -1,  0 };
			auto const RIGHT = glm::i32vec2{  1,  0 };
			auto const UP    = glm::i32vec2{  0, -1 };
			auto const DOWN  = glm::i32vec2{  0,  1 };

			// rendering
			auto const bg_color = glm::u8vec4{ 191, 191, 191, 255 };

			auto const snake_color = glm::u8vec4{ 0, 128, 0, 255 };
			auto const snake_collided_color = glm::u8vec4{ 191, 0, 0, 255 };
			auto const snake_won_color = glm::u8vec4{ 255, 191, 64, 255 };
			auto const snake_head_color = glm::u8vec4(255);
			auto const snake_head_border_size = 6;

			auto const food_color = glm::u8vec4{ 255, 106, 0, 255 };

			auto const score_char_size = glm::i32vec2{ 8, 8 };
			auto const score_draw_scale = 2;
			auto const score_texture = &app.m_assets.m_textures.at("score");

			// snake
			auto growth = 0;
			auto const food_growth = 5;

			auto const snake_start_length = 5;
			auto const snake_start_pos = glm::i32vec2{ 5, 5 };
			auto const snake_start_dir = RIGHT;
			auto snake_dir = snake_start_dir;

			auto snake = std::deque<glm::i32vec2>();
			for (auto x = 0; x != snake_start_length; ++x)
				snake.push_back({ snake_start_pos.x + x, snake_start_pos.y });

			// food
			auto score = 0;
			auto const food_score = 10;
			
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

			// timing
			auto snake_update_period = std::chrono::milliseconds(250);
			auto const snake_update_period_speedup = std::chrono::milliseconds(25);
			auto const snake_update_period_max_speedups = 5;
			auto snake_update_period_speedups = 0;
			auto snake_update_period_start = std::chrono::high_resolution_clock::now();

			// move types
			enum class move_type { moved, grew, ate, collided, won };
			auto last_move = move_type::moved;
			
			// sound setup
			auto const start_sound = &app.m_assets.m_sounds.at("start");
			
			auto sounds = std::map<move_type, sdl::mixer_chunk*>
			{
				{ move_type::moved, &app.m_assets.m_sounds.at("step") },
				{ move_type::grew, &app.m_assets.m_sounds.at("grow") },
				{ move_type::ate, &app.m_assets.m_sounds.at("eat") },
				{ move_type::collided, &app.m_assets.m_sounds.at("die") },
				{ move_type::won, &app.m_assets.m_sounds.at("win") },
			};

			// start!
			app.m_mixer_context.play_once(*start_sound);

			while (true)
			{
				// input
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

				// update
				{
					auto const now = std::chrono::high_resolution_clock::now();

					if (now - snake_update_period_start >= snake_update_period)
					{
						// reset the update period
						while (now - snake_update_period_start >= snake_update_period)
							snake_update_period_start += snake_update_period;

						// end the game if we collided or won
						if (last_move == move_type::collided || last_move == move_type::won)
							return { do_countdown };

						// update head position
						snake.push_back((((snake.back() + snake_dir) % cell_count) + cell_count) % cell_count);

						// update tail position
						if (growth == 0)
						{
							snake.pop_front();
							last_move = move_type::moved;
						}
						else
						{
							--growth;
							last_move = move_type::grew;
						}

						// check collision w/ self
						if (snake.size() > 1)
						{
							if (std::find(snake.begin(), snake.end() - 1, snake.back()) != snake.end() - 1)
								last_move = move_type::collided;
						}

						// check collision w/ food
						if (snake.back() == food_pos)
						{
							score += food_score;
							growth += food_growth;

							if (snake.size() == all_cells.size())
							{
								last_move = move_type::won;
							}
							else
							{
								food_pos = get_food_pos(rng, all_cells, snake);

								last_move = move_type::ate;

								if (snake_update_period_speedups < snake_update_period_max_speedups)
								{
									++snake_update_period_speedups;
									snake_update_period -= snake_update_period_speedup;
								}
							}
						}

						app.m_mixer_context.play_once(*sounds.at(last_move));
					}
				}

				// render
				{
					app.m_renderer.clear(bg_color);

					// snake
					{
						auto color = (last_move == move_type::collided) ? snake_collided_color : (last_move == move_type::won) ? snake_won_color : snake_color;

						// body
						for (auto p : snake)
							app.m_renderer.draw_quad_fill(p * cell_size, cell_size, color, sdl::blend_mode::NONE);

						// head
						app.m_renderer.draw_quad_fill(snake.back() * cell_size + snake_head_border_size / 2, cell_size - snake_head_border_size, snake_head_color, sdl::blend_mode::NONE);
					}

					// food
					{
						app.m_renderer.draw_quad_fill(food_pos * cell_size, cell_size, food_color, sdl::blend_mode::NONE);
					}

					// score
					{
						auto score_str = std::to_string(score);
						auto const score_pos = glm::i32vec2{ app.m_window.get_size().x - 5, 5 };

						for (auto i = std::size_t{ 0 }; i != score_str.size(); ++i)
						{
							auto digit = score_str[i] - '0';
							die_if(digit < 0 || digit > 9);

							auto offset = glm::i32vec2{ score_char_size.x * digit, 0 };
							auto align = i - score_str.size();
							auto pos = glm::i32vec2{ score_pos.x + align * score_char_size.x * score_draw_scale, score_pos.y };

							app.m_renderer.draw_quad_texture(pos, score_char_size * score_draw_scale, *score_texture, offset, score_char_size);
						}
					}

					app.m_renderer.present();
				}
			}

			return { };
		}
		
	} // game
	
} // snek
