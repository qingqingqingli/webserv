#include "helper.hpp"

void	signal_handler(int signal)
{
	if (signal == SIGINT)
	{
		std::cout << CYAN << " Stopping server..." << RESET << std::endl;
		exit(0);
	}
}
