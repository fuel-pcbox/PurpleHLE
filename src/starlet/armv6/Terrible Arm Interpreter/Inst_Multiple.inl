#include "Instruction.h"

namespace Interpreter
{

INLINE void MEM_MULTIPLE(bool thumb, u32 cond, u32 before, u32 up, u32 special, u32 writeback, 
				  u32 load, u32 reg_n, u32 reglist)
{
	if (!COND(cond))
		return;

	Assert(reglist != 0); //invalid

	if (writeback)
		Assert(((1 << reg_n) & reglist) == 0); //mostly invalid

	Assert(!special); //privileged

	u32 addr = GPR(reg_n);
	MakeSure((addr & 3) == 0);

	int increment = (up ? 4 : -4);

	int i = (up ? 0 : 15);
	while (true)
	{
		if (((1 << i) & reglist) != 0)
		{

			if (before)
				addr += increment;

			if (!load) //store
			{
				u32 value = GPR(i, IMPL_SPECIFIC_PC_OFFSET, thumb);
				MEM_STORE( false, 0, addr, value ); //aligned
			}
			else
			{
				u32 memval = MEM_LOAD( false, 0, addr ); //aligned
				if (i != 15)
					setGPR(i, memval);
				else
					setPC(memval);
			}

			if (!before)
				addr += increment;

		}

		i += (up ? 1 : -1);
		if ((up && i == 16) || (!up && i == -1))
			break;
	}
	
	if (writeback)
		setGPR( reg_n, addr );
}

}