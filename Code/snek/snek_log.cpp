#include "snek_log.hpp"

#include <iostream>

namespace snek
{
	
	void log_info(std::string const& message)
	{
		std::cerr << "INFO: " << message << std::endl;
	}

	void log_error(std::string const& message)
	{
		std::cerr << "ERROR: " << message << std::endl;
	}

} // snek
