#ifndef __RUBAN_COMBINATION__
#define __RUBAN_COMBINATION__

#include "SRubanOp.h"
#include "CRubanState.h"

#define MAX_COMBINATION_LENGTH			15	///< Suppose longer combinations can't be calculated!

/// Combination of operations (from initial)
class CRubanCombination
{
private:
	unsigned char _length;	///< Operations count in combinations
	SRubanOp _ops[MAX_COMBINATION_LENGTH];		///< Operations (!! NOTE static array (don't use heap!)	)
	
public:
	CRubanCombination(unsigned char length=0) : _length(length) {}
	CRubanCombination(const CRubanCombination &b) : _length(b._length)
	{
		for(int i=_length; i--; )	// !! NOTE - don't copy all _ops, only defined
			_ops[i]=b._ops[i];
	}

	void SetOp(int op_num, SRubanOp op) 	{ _ops[op_num] = op; }
	void SetLength(unsigned char _length)	{ _length = _length; }
	
	void IncLength()											{ _length++; }
	void SetLastOp(SRubanOp op)						{ _ops[_length-1]=op; }

	bool operator==(const CRubanCombination &b) const;
	
	/// Execute combination on ruban from source state and return result state
	SUnpackedRubanState Execute(const SUnpackedRubanState &src) const;

	/// Gets Nth invariant of combination
	void GetCombinationInvariant(int invariant_num, CRubanCombination &dst) const;

	//-----------------------

	friend std::ostream& operator<<(std::ostream &stream, const CRubanCombination& cmb);
};

/// Operator for output combination to stream (debug purposes)
std::ostream& operator<<(std::ostream &stream, const CRubanCombination& cmb);

#endif // __RUBAN_COMBINATION__
