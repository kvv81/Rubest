#ifndef __RUBAN_HISTORY__
#define __RUBAN_HISTORY__

#include <iostream>
#include "CRubanState.h"
#include "CRubanAllocator.h"
#include "CRubanCombination.h"
#include "RubanConfig.h"

/// Binary state-tree node
struct SHistoryNode
{
	SPackedRubanState	state;			///< state of ruban
	SAllocatorIndex		a, b,				///< Left and right leaf of binary tree
										prev_state;	///< index to prev state (to have ability walk up the tree)
	
	unsigned char			steps;			///< steps from root
	SRubanOp					rotation_from_prev;	///< operation from prev node

	/// Ctor can init the most values as undefined
	SHistoryNode(const SPackedRubanState &_state, unsigned char _steps,
			SRubanOp _rotation_from_prev	= SRubanOp(),
			SAllocatorIndex _prev_state	= SAllocatorIndex()
		)
		:	state(_state), steps(_steps),
			prev_state(_prev_state), rotation_from_prev(_rotation_from_prev)
	{}

	/// walk up the tree from node to root and build accomplished combination
	void GetAccomplishedCombination(CRubanCombination &dst);

};

/// Tree manager class
class CStatesHistory
{
private:
	static const int res_file_signature;	///< signatire 'RUBN' for history file

 	SRubanOp current_op;									///< member variable to exclude it from recursive AddState froc
	SAllocatorIndex current_state;				///< member variable to exclude it from recursive AddState froc

public:
	static unsigned char current_step;								///< curent step from initial state
	static CRubanAllocator<SHistoryNode> allocator;		///< allocator to boost memory IO (allocs large blocks) 
//	SHistoryNode *AllocHistoryNode(SAllocatorIndex &item_index);

public:
	enum EHistoryMode {ehmNormal, ehmRestoring, ehmShutdownRequest, ehmShutdown};

	static EHistoryMode			history_mode;
	static SAllocatorIndex	last_processed_node_index;	///< index of last processed node before shutdown

	static SAllocatorIndex	root_index;			///< index of tree root (have to be 0) - inited in ctor
	static SHistoryNode			*root;					///< to don't call GetItem(root_index) again and again - inited in ctor

	CStatesHistory(const SPackedRubanState &init_state);

	void	ScanForNextStep()
	{
		ScanForThisStep(root_index);
		
		if(history_mode != ehmShutdown)
			current_step++;		// !! NOTE - shutdown mode don't finilize step, so don't report finish!

#if DEBUG_LEVEL>0
		std::cout << "Step #" << (int)current_step << ": "
			<< allocator.GetItemsCount() << " states" << std::endl;
#endif
	}
	void	ScanForThisStep(SAllocatorIndex index);
	void	ProcessNextSteps(SAllocatorIndex index);
	
	bool	LookupState(SPackedRubanState &state, SHistoryNode *p);
	bool	LookupState(SPackedRubanState &state)
	{
		return LookupState(state, root);
	}

	void	AddState(SPackedRubanState &state, SAllocatorIndex index);
	void	AddState(SPackedRubanState &state, SRubanOp _current_op, SAllocatorIndex _current_state)
	{
		// Here we push two variables to members, 'cause don't want to repeat them in recursive call!
		current_op=_current_op;
		current_state=_current_state;

		AddState(state, root_index);
	}

	/// sign that state is desired as result state
	int		IsDesiredState(SPackedRubanState &state);
	
	bool	LoadHistory(const char *fname);
	void	SaveHistory(const char *fname);

	~CStatesHistory();
};

#endif // __RUBAN_HISTORY__


