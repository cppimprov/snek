#pragma once

namespace snek
{
	
	namespace sdl
	{
		
		struct mixer_context
		{
			mixer_context();
			~mixer_context();

			mixer_context(mixer_context const&) = delete;
			mixer_context& operator=(mixer_context const&) = delete;

			mixer_context(mixer_context&&) = delete;
			mixer_context& operator=(mixer_context&&) = delete;
		};
		
	} // sdl
	
} // snek
