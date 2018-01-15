#include <csignal>
void ruban_sigint_handler(int sig_num);		///< Catch Ctrl+C
void ruban_sigusr1_handler(int sig_num);	///< Catch kill -USR1 {pid}
