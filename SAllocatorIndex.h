#ifndef __RUBAN_ALLOCATOR_INDEX__
#define __RUBAN_ALLOCATOR_INDEX__

#define INVALID_ITEM_INDEX (~0)

/// \brief Container struct for index in CRubanAllocator
struct SAllocatorIndex
{
	unsigned long value;	///< incapsulate DWORD index
	
	SAllocatorIndex() : value(INVALID_ITEM_INDEX) {}
	SAllocatorIndex(unsigned long _value) : value(_value) { assert(IsValid()); }

	/// Check if index valid
	bool IsValid()
	{ return (value != INVALID_ITEM_INDEX); }

	/// Transfer to DWORD
	operator unsigned long()
	{
		return value;
	}

};

#endif // __RUBAN_ALLOCATOR_INDEX__
