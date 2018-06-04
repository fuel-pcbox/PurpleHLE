#include "Instruction.h"
#include "Swi.h"
#include "MachOHLE.h"

namespace Interpreter
{

void BRANCH_IMM(bool thumb, u32 cond, u32 link, u32 imm24)
{	
	if (!COND(cond))
		return;

	u32 real_offset = SIGNEX(imm24, 24) * (thumb ? 2 : 4);

	u32 target = PCVAL(2, thumb) + real_offset;

	if (link)
		cpu.lr = PCVAL(1, thumb) | (thumb ? 1 : 0); 

	cpu.nia = target; //doesn't change processor state
}

void BRANCH_LX_ARM(bool thumb, u32 hbit, u32 imm24)
{
	Assert(!thumb);

	u32 real_offset = SIGNEX(imm24, 24) * 4 + (hbit * 2);

	u32 target = PCVAL(2, thumb) + real_offset;

	cpu.lr = PCVAL(1, thumb); 

	cpu.nia = target;
	cpu.thumb = 1;
}

void BRANCH_L_THUMB(bool thumb, u32 imm24)
{
	Assert(thumb);

	u32 real_offset = SIGNEX(imm24, 24) * 2;

	u32 target = PCVAL(2, thumb) + real_offset;

	cpu.lr = PCVAL(2, thumb) | 1; 

	cpu.nia = target;
	cpu.thumb = 1;
}

void BRANCH_LX_THUMB(bool thumb, u32 imm24)
{
	Assert(thumb);

	u32 real_offset = SIGNEX(imm24, 24) * 2;

	u32 target = (PCVAL(2, thumb) + real_offset) & ~0x3;

	cpu.lr = PCVAL(2, thumb) | 1; 

	cpu.nia = target;
	cpu.thumb = 0;
}

void BRANCH_CHANGE_REG(bool thumb, u32 cond, u32 link, u32 reg_m)
{
	if (!COND(cond))
		return;

	MakeSure(reg_m != 15); // actually okay (if obsolete) when (!link)

	u32 target = GPR(reg_m); 

	if (link)
		cpu.lr = PCVAL(1, thumb) | (thumb ? 1 : 0);

	setPC(target); // set thumb and nia
}

INLINE void SWI(bool thumb, u32 cond, u32 swi)
{
	if (!COND(cond))
		return;

	if (thumb)
		Fail(thumb); //???

	::Swi(swi);
}

INLINE void BKPT(bool thumb, u32 cond, u32 bkpt)
{
	if (!COND(cond))
		return;

	if (bkpt == MachO::HLE::BkptVal())
		MachO::HLE::Do( PCVAL(0, thumb) );
	else
		Fail(bkpt);
}

}