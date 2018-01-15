#include <iostream>
#include "CRubanHistory.h"

/// SIGINT handler (Ctrl+C -> request to serialize & terminate)
void ruban_sigint_handler(int sig_num)
{
	if(CStatesHistory::history_mode == CStatesHistory::ehmNormal)
	{
		std::cout << "SIGINT catched, store database to file..." << std::endl;
		CStatesHistory::history_mode = CStatesHistory::ehmShutdownRequest;	
	}
	else
		std::cout << "SIGINT catched, but cannot interrupt previous request" << std::endl;
}


/// SIGUSR1 handler (prints statistics)
void ruban_sigusr1_handler(int sig_num)
{
	std::cout << "Current step = " << CStatesHistory::allocator.GetItemsCount() << std::endl;
}
