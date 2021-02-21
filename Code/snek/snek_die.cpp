#include "snek_die.hpp"

namespace snek
{
	
	void die()
	{
		__debugbreak();
	}

	void die_if(bool condition)
	{
		if (condition)
			die();
	}
	
} // snek
