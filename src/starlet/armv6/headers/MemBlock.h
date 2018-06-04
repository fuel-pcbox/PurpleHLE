/*#pragma once
#include "Memory.h"
#include <list>

class MemBlock
{
public:
	MemBlock(u32 start, u32 size);
	u32 Alloc(u32 size);
	void Free(u32 start);
	void ForceAlloc(u32 start, u32 size); //TEMPORARY!

private:
	typedef struct
	{
		u32 start, size;
	} Block;

	std::list<Block> m_memBlocks;
	std::list<Block> m_freeBlocks;

	typedef std::list<Block> BlockVec;
	typedef std::list<Block>::iterator BlockIter;
};

extern MemBlock* memBlock;*/