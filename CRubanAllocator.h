#ifndef __RUBAN_ALLOCATOR__
#define __RUBAN_ALLOCATOR__

//#include <malloc.h>
#include <cstdlib>
#include <new>
#include <cassert>
#include <fstream>

#include "SAllocatorIndex.h"
#include "RubanConfig.h"

#define DEFAULT_ALLOCATOR_BS_EXPONENT 12

/// \brief Container template to store data in large blocks
template<class T>
class CRubanAllocator
{
private:
	const unsigned long _block_size_exponent, _block_size, _offset_in_block_mask;
	
	T **_blocks;	///< array of pointers to blocks
	unsigned long _cur_block, _next_item;

	/// alloc single block
	T	*MallocBlock() throw (std::bad_alloc)
	{
		T *res = (T *) malloc(sizeof(T)*_block_size);
		assert(res != NULL);
		if(res == NULL) throw new std::bad_alloc;
		return res;
	}

	/// resize block array according to new blocks_count
	void ReallocBlocksArray(unsigned long blocks_count) throw (std::bad_alloc)
	{
		_blocks = (T**) realloc(_blocks, blocks_count*sizeof(T*));
		assert(_blocks != NULL);	
		if(_blocks == NULL) throw new std::bad_alloc;
	}
	
	/// Add single block (if no more space left in already alloced)
	void AddBlock() throw (std::bad_alloc)
	{
		_cur_block++;
		ReallocBlocksArray(_cur_block+1);
		_blocks[_cur_block] = MallocBlock();
	}

	/// Routine to prepare receive new items in allocator (used in loading from stet file)
	void ReallocBlocks(unsigned long new_items_count)
	{
		unsigned int _blocks_count = (new_items_count+_offset_in_block_mask) >> _block_size_exponent;
		ReallocBlocksArray(_blocks_count);

		for(unsigned int i=_cur_block+1; i<_blocks_count; i++)
			_blocks[i] = MallocBlock();

		// setup new values
		_cur_block = _blocks_count-1;
		_next_item = new_items_count;
	}
	
public:

	/// Ctor to create empty allocator
	CRubanAllocator(unsigned long block_size_exponent = DEFAULT_ALLOCATOR_BS_EXPONENT)
		: _cur_block(~0), _blocks(NULL), _next_item(0), 
			_block_size_exponent	(block_size_exponent),
			_block_size					( 1<<block_size_exponent   ),
			_offset_in_block_mask((1<<block_size_exponent)-1)
	{
		AddBlock();	// !! NOTE cur_block=-1
	}

	/*
	CRubanAllocator(unsigned char *saved_data_file_name,
			unsigned long _block_size_exponent = DEFAULT_ALLOCATOR_BS_EXPONENT)
	{
		
	}*/

	/// Getter for count of items in container
	unsigned long GetItemsCount()
	{
		return _next_item;
	}

	/// Request to allocate item in container
	SAllocatorIndex AllocItem()
	{
		if((_next_item & _offset_in_block_mask ) == _offset_in_block_mask)
			AddBlock();

		return SAllocatorIndex(_next_item++);
	}

	/// Remap index to pointer to item
	T *GetItem(SAllocatorIndex index)
	{
		assert(index.IsValid());
		
		unsigned long i = (unsigned long) index;
		return _blocks[i>>_block_size_exponent] + (i & _offset_in_block_mask);
	}

	/// Combined: allocation request + remap index to pointer
	T	*AllocItem(SAllocatorIndex &alloced_item_index)
	{
		alloced_item_index = AllocItem();
		return GetItem(alloced_item_index);
	}

	/// Serialize container to stream
	void SaveDataToFile(std::ofstream &resfile)	// resfile HAVE to be OPENED binary file!
	{
		// Store number of items
		resfile.write(reinterpret_cast<const char*>(&_next_item), sizeof(_next_item));

		for(unsigned long i=0;i<_cur_block;i++)
			resfile.write(reinterpret_cast<char*>(_blocks[i]), sizeof(T)*_block_size);
		
		resfile.write(reinterpret_cast<char*>(_blocks[_cur_block]),
				(_next_item & _offset_in_block_mask)*sizeof(T));
		
#if DEBUG_LEVEL>0
		std::cout << "State data have been saved to history file: " << _next_item*sizeof(T)
			<< " bytes, " << _next_item << " states" << std::endl;
#endif
		
	}
	
	/// Serialize container from stream
	bool LoadDataFromFile(std::ifstream &resfile)
	{
		unsigned long items_count;
		// Load number of items
		resfile.read(reinterpret_cast<char*>(&items_count), sizeof(items_count));
		
		// here all memory allocated, all variables restored to back (except states info)
		ReallocBlocks(items_count);

		for(unsigned long i=0;i<_cur_block;i++)
			resfile.read(reinterpret_cast<char*>(_blocks[i]), sizeof(T)*_block_size);
		
		resfile.read(reinterpret_cast<char*>(_blocks[_cur_block]),
				(_next_item & _offset_in_block_mask)*sizeof(T));
		
		return true;
	}

	/// Dtor - free all memory
	~CRubanAllocator()
	{
		for(unsigned long i=0; i<=_cur_block; i++)
		{
			if(_blocks[i] != NULL)
				free(_blocks[i]);
		}
		free(_blocks);
	}

};

#endif // __RUBAN_ALLOCATOR__
