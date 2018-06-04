/*#include "MemBlock.h"

MemBlock::MemBlock(u32 start, u32 size)
{
	Block block = {start, size};
	m_freeBlocks.push_front(block);
}

u32 MemBlock::Alloc(u32 size)
{
	for (BlockIter i = m_freeBlocks.begin(); i != m_freeBlocks.end(); i++)
	{
		if (i->size >= size) //block big enough
		{
			u32 addr = i->start;
			Block block = {addr, size};
			m_memBlocks.push_front(block); //insert new block

			i->start += size; 
			i->size -= size; //trim free block
			if (i->size == 0)
				m_freeBlocks.erase(i);
			
			return addr;
		}			
	}
	Ouch(0);
	return 0;
}

void MemBlock::Free(u32 start)
{
	for (BlockIter i = m_memBlocks.begin(); i != m_memBlocks.end(); i++)
	{
		if (i->start == start) //block matches
		{
			u32 size = i->size;
			m_memBlocks.erase(i);

			Block block = {start, size};
			m_freeBlocks.push_front(block); //FRAGMENTATION PROBLEM (later)
			return;
		}
	}
}

void MemBlock::ForceAlloc(u32 start, u32 size) //TEMPORARY!
{
	for (BlockIter i = m_freeBlocks.begin(); i != m_freeBlocks.end(); i++)
	{
		if (i->start <= start && i->start + i->size >= start + size)
		{
			Block block = {start, size};
			m_memBlocks.push_front(block); //insert new block

			Block freeBefore = {i->start, start - i->start};
			Block freeAfter = {start + size, i->size - size - freeBefore.size};

			m_freeBlocks.erase(i);
			if (freeBefore.size > 0)
				m_freeBlocks.push_front(freeBefore);
			if (freeAfter.size > 0)
				m_freeBlocks.push_front(freeAfter);
			
			return;
		}			
	}
	Fail(0);
}

MemBlock* memBlock = NULL;*/