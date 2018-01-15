/// \file
/// Processing executable (calculation server)

/// \mainpage
///	Rubest, kube-rube combinations solver.
/// - Scans all distinct ruban combinations from initial state (completed state)
///   to show useful combinations with their results.
/// - Main target of project is to find short combination which can help
///   to solve ruban on 3rd (last) facet according to standard solve algorithm.
/// - Caclulation server stores all calculated data in state file, which can be
///   processed manually by graphical client.
/// \author Volodymyr Khomenko, Ukraine, 2006

#include <cstring>
#include <sstream>
#include "CRubanState.h"
#include "CRubanHistory.h"
#include "RubanSignal.h"

#define DEFAULT_OPERATIONS_NUM 7
const char *default_res_fname = "RubanStates.bin";
const char *inp_res_fname=default_res_fname, *out_res_fname = default_res_fname;  

int main(int argc, char *argv[])
{
	int begin_step = 0, operations_num = DEFAULT_OPERATIONS_NUM;

	// Process help request
	if(argc==2 && (strcmp(argv[1], "--help")==0 || strcmp(argv[1], "-h")==0))
	{
		std::cout << "Usage: " << argv[0] << " {options}" << std::endl;
		std::cout << "{options} is a set of:" << std::endl;
		std::cout << "\t-s <out_filename.bin> - save result to specified file" << std::endl;
		std::cout << "\t-l <inp_filename.bin> - load result from specified file" << std::endl;
		std::cout << "\t-n <operations_num> - process specified number of operations" << std::endl;
		std::cout << "Default filename - " << default_res_fname << ", default operations_num=" << DEFAULT_OPERATIONS_NUM << std::endl;
		return 0;
	}
	
#if DEBUG_LEVEL>=0
	std::cout << "Rubest, kube-rube combinations solver" << std::endl;
#endif

	//-----------------------------------------------------------------------------
	// Process arguments
	
	for(int i=1; i<argc; i++)	// Parse options if present
	{
		if(strcmp(argv[i], "-n")==0)
		{
			i++;
			std::istringstream intstream(argv[i]);
			intstream >> operations_num;
#if DEBUG_LEVEL>0
			std::cout << "Process " << operations_num << " operations" << std::endl;
#endif
		}
		else if(strcmp(argv[i], "-s")==0)
		{
			i++;
			out_res_fname = argv[i];

#if DEBUG_LEVEL>0
			std::cout << "Output result to file '" << out_res_fname << "'" << std::endl;
#endif
		}
		else if(strcmp(argv[i], "-l")==0)
		{
			i++;
			inp_res_fname=argv[i];
			CStatesHistory::history_mode = CStatesHistory::ehmRestoring;

#if DEBUG_LEVEL>0
			std::cout << "Load previous result from file '" << inp_res_fname << "'" << std::endl;
#endif
		}
	}

	//-----------------------------------------------------------------------------------
	
//	std::cout << sizeof(SHistoryNode) << std::endl;
	signal(SIGINT, ruban_sigint_handler);

#ifdef SIGUSR1	// Microsoft don't know this signal...
	signal(SIGUSR1, ruban_sigusr1_handler);
#endif

	//----------------------------------------------------------------------------------
	
	SRubanOp::InitInvariantsTable();	// Create hash table for invariants constructing
	
	inital_unpacked_ruban_state.GetPackedRubanState(inital_packed_ruban_state);	// Pack initial state

	CStatesHistory hist(inital_packed_ruban_state);															// Begin history

	if(CStatesHistory::history_mode == CStatesHistory::ehmRestoring)
	{
		if(!hist.LoadHistory(inp_res_fname))
			return -1;

		begin_step = hist.current_step; 
	}

	//-----------------------------------------------------------------

	for(int i=begin_step;i<operations_num;i++)
	{
		hist.ScanForNextStep();
		if(CStatesHistory::history_mode == CStatesHistory::ehmShutdown)
			break;
	}
	hist.SaveHistory(out_res_fname);
	
	return 0;
}
