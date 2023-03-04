#include "CRubanCombination.h"

bool CRubanCombination::operator==(const CRubanCombination &b) const
{
	if(_length != b._length) return false;

	for(int i=_length; i--; )
	{
		if(_ops[i] != b._ops[i])
			return false;
	}
	return true;
}

SUnpackedRubanState CRubanCombination::Execute(const SUnpackedRubanState &src) const
{
	SUnpackedRubanState tmp(src);

	for(int i=0;i<_length; i++)
		tmp.ExecuteOp(_ops[i]);
	
	return tmp;
}

void CRubanCombination::GetCombinationInvariant(int invariant_num, CRubanCombination &dst) const
{
	dst.SetLength(_length);
	for(int i=_length; i-- ;)
		dst._ops[i] = _ops[i].GetInvariant(invariant_num);
}

std::ostream& operator<<(std::ostream &stream, const CRubanCombination& cmb)
{
	for(int i=0; i<cmb._length; i++)
		stream << cmb._ops[i];
	return stream;
}
