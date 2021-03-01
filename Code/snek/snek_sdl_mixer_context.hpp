#pragma once

namespace snek
{
	
	namespace sdl
	{
		
		class mixer_chunk;

		struct mixer_context
		{
			mixer_context();
			~mixer_context();

			mixer_context(mixer_context const&) = delete;
			mixer_context& operator=(mixer_context const&) = delete;

			mixer_context(mixer_context&&) = delete;
			mixer_context& operator=(mixer_context&&) = delete;

			void play_once(mixer_chunk const& chunk) const;
		};
		
	} // sdl
	
} // snek
