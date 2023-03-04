#include <stdio.h>
#include <new>		// placement new
#include "SRubanOp.h"
#include "CRubanHistory.h"
#include "CRubanCombination.h"

CStatesHistory::EHistoryMode CStatesHistory::history_mode = CStatesHistory::ehmNormal;
SAllocatorIndex CStatesHistory::last_processed_node_index;

SHistoryNode *CStatesHistory::root = NULL;
SAllocatorIndex CStatesHistory::root_index;

CRubanAllocator<SHistoryNode> CStatesHistory::allocator;
unsigned char CStatesHistory::current_step = 0;
const int CStatesHistory::res_file_signature = 0x4e425552;	// 'RUBN'

CStatesHistory::CStatesHistory(const SPackedRubanState &init_state)
{
	root = allocator.AllocItem(root_index);	// dummy_root_index have to be 0
	
	// placement new (wanna call init in ctor)
	root = new ((void*)root) SHistoryNode(init_state, 0);
}

/*
SHistoryNode *AllocHistoryNode(SAllocatorIndex &item_index)
{
	SHistoryNode *p = allocator.AllocItem(item_index);
	p = new ((void*)p) SHistoryNode()
}
*/

void SHistoryNode::GetAccomplishedCombination(CRubanCombination &dst)
{
	dst.SetLength(steps);

	SHistoryNode *p = this;
	for(int i = steps; i--; )
	{
		dst.SetOp(i, p->rotation_from_prev);
		p = CStatesHistory::allocator.GetItem(p->prev_state);
	}
}

//-----------------------------------------------------------------

bool CStatesHistory::LookupState(SPackedRubanState &state, SHistoryNode *p)
{
	int cmp_res = p->state.Compare(state);
	if(cmp_res==0) return true;
	if(cmp_res<0)	// p->state < state
	{
		if(p->b.IsValid())
			return LookupState(state, allocator.GetItem(p->b));
		else
			return false;
	}
	// else p->state > state 
	if(p->a.IsValid())
		return LookupState(state, allocator.GetItem(p->a));
	else
		return false;
}

void CStatesHistory::AddState(SPackedRubanState &state, SAllocatorIndex index)
{
	SHistoryNode *p=allocator.GetItem(index);

	int cmp_res = p->state.Compare(state);
	if(cmp_res==0)
	{
//		assert(AM_ADDSTATE_TRIES_TO_ADD_THE_SAME_STATE);	// state already present???
		return;
	}
	SAllocatorIndex *dst_index = NULL;
	if(cmp_res<0) // p->state < state
	{
		if(p->b.IsValid())
			AddState(state, p->b);
		else
			dst_index = &p->b;
	}
	else //p->state > state 
	{
		if(p->a.IsValid())
			AddState(state, p->a);
		else
			dst_index = &p->a;
	}

	if(dst_index != NULL)
	{
		SHistoryNode *n = allocator.AllocItem(*dst_index);	// write alloced item index directly
		n = new ((void*)n) SHistoryNode(state, current_step+1, current_op.GetReverseOp(), current_state);
	}
}

//-----------------------------------------------------------------

void CStatesHistory::ScanForThisStep(SAllocatorIndex index)
{
	SHistoryNode *p=allocator.GetItem(index);

	// Scan right subtree
	if(p->a.IsValid())
		ScanForThisStep( p->a );

	if(history_mode == ehmShutdown)
		return;
	else if(history_mode == ehmRestoring)	// Searching for last processed node?
	{
		if(index == last_processed_node_index)	// If we have find last processed node
		{
			history_mode = ehmNormal;							// switch to normal state
			last_processed_node_index = SAllocatorIndex();	// invalidate var (just wanna...)

#if DEBUG_LEVEL>0
			std::cout << "Last processed node located, state resumed." << std::endl;
#endif
		}
	}
	else	// Normal or ShutdownRequest
	{
		if(p->steps == current_step)	// this step item?
		{
			ProcessNextSteps(index);
			
			if(history_mode == ehmShutdownRequest)
			{
#if DEBUG_LEVEL>=0
				std::cout << "Last processed node ID=" << ((unsigned long) index) << std::endl;
#endif

				last_processed_node_index = index;
				history_mode = ehmShutdown;	
				return;
			}
		}
	}

	// Scan left subtree
	if(p->b.IsValid())
		ScanForThisStep( p->b );
}

void CStatesHistory::ProcessNextSteps(SAllocatorIndex index)
{
	SHistoryNode *p=allocator.GetItem(index);

	CRubanCombination cmb;
	p->GetAccomplishedCombination(cmb);		// get current combination

	SUnpackedRubanState prev_state(				// get current state
		cmb.Execute(inital_unpacked_ruban_state)
	);

	SRubanOp prev_op(p->rotation_from_prev.GetReverseOp());

	cmb.IncLength();	// extend combination by one operation

	for(SRubanOp o=min_ruban_op; o<=max_ruban_op; ++o)	// scan all possible operations
	{
		if(o == prev_op) continue;	// don't do operation reverse to prev!
		cmb.SetLastOp(o);						// set cmb to canidate combination

#if DEBUG_LEVEL > 2
		std::cout << "Processing combination " << cmb << std::endl;
#endif

		bool isAnyInvariantAlreadyExists = false;
		CRubanCombination cmb_invariants[INVARIANTS_NUM];

		// We skipping invariant #0 as trivial one (the same combination)
		for(int i=1;i<INVARIANTS_NUM;i++)	// scan all combination invariants
		{
			cmb.GetCombinationInvariant(i, cmb_invariants[i]);

#if DEBUG_LEVEL > 3
			std::cout << "Invariant[" << i << "]=" << cmb_invariants[i] << std::endl;
#endif
			
			// Now check if this invariant not the same as already searched (kind of optimization)
			bool isSameInvariantSearched=false;
			for(int j=1; j<i; j++)
				if(cmb_invariants[i]==cmb_invariants[j])
				{
					isSameInvariantSearched=true;
					break;
				}					

			if(isSameInvariantSearched) continue;

			SUnpackedRubanState res_state(			// get result of invariant
				cmb_invariants[i].Execute(inital_unpacked_ruban_state)
			);

			SPackedRubanState res_packed_state;	// pack it
			res_state.GetPackedRubanState(res_packed_state);

#if DEBUG_LEVEL > 3
			std::cout << "New combination, search for result state=" << res_packed_state << std::endl;
#endif

			if(LookupState(res_packed_state))	// and search in DB
			{
				isAnyInvariantAlreadyExists=true;
#if DEBUG_LEVEL > 2
				std::cout << "Result state found, SKIP combination" << std::endl;
#endif
				break;
			}

		}

		if(!isAnyInvariantAlreadyExists)	// if no one invariant of this combination exists
		{
#if DEBUG_LEVEL > 2
			std::cout << "Any invariant NOT found, ADD combination" << std::endl;
#endif

			SUnpackedRubanState new_state(prev_state);

			new_state.ExecuteOp(o);

			SPackedRubanState new_packed_state; // pack it
			new_state.GetPackedRubanState(new_packed_state);

#if DEBUG_LEVEL > 1
			std::cout << "ADD NEW STATE: " << new_packed_state <<
				" (combination=" << cmb <<std::endl;
#endif
			AddState(new_packed_state, o, index);	// and add to DB
		}
	}

}

//-----------------------------------------------------------------------------------------

void CStatesHistory::SaveHistory(const char *fname)
{
	std::ofstream resfile(fname, std::ios::out | std::ios::binary | std::ios::trunc);
	if(resfile.is_open())
	{
		// Write file signature
		resfile.write(reinterpret_cast<const char*>(&res_file_signature), sizeof(res_file_signature));

		// Write few state variables
		resfile.write(reinterpret_cast<const char*>(&current_step), sizeof(current_step));
		resfile.write(reinterpret_cast<const char*>(&last_processed_node_index), sizeof(last_processed_node_index));
		
		// Write tree
		allocator.SaveDataToFile(resfile);
	}
	else
		std::cerr << "Cannot open file " << fname << " for write" << std::endl;
}

bool CStatesHistory::LoadHistory(const char *fname)
{
	std::ifstream resfile(fname, std::ios::in | std::ios::binary );
	if(resfile.is_open())
	{
		// Check file signature
		unsigned long signature;
		resfile.read(reinterpret_cast<char*>(&signature), sizeof(signature));
		if(signature != res_file_signature)
		{
			std::cerr << "Wrong file signature" << std::endl;
			return false;
		}

		// Read state variables
		resfile.read(reinterpret_cast<char*>(&current_step), sizeof(current_step));
		resfile.read(reinterpret_cast<char*>(&last_processed_node_index), sizeof(last_processed_node_index));

#if DEBUG_LEVEL>0
		std::cout << "Suspended step=" << (int)current_step << ", last processed node index=" << last_processed_node_index << std::endl;
#endif
		// Read tree
		return allocator.LoadDataFromFile(resfile);
	}
	else
	{
		std::cerr << "Cannot open file " << fname << " for read" << std::endl;
		return false;
	}
}

//-----------------------------------------------------------------------------------------

CStatesHistory::~CStatesHistory()
{

}
