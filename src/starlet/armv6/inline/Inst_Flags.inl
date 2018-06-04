#include "Instruction.h"

namespace Interpreter
{

INLINE void TO_FLAGS_IMM(bool thumb, u32 cond, u32 freg, u32 mask, u32 rot_imm, u32 imm)
{
	if (!COND(cond))
		return;

	u32 imm32 = _rotr(imm, rot_imm * 2);
	TO_FLAGS(thumb, freg, mask, imm32);
}

INLINE void TO_FLAGS_REG(bool thumb, u32 cond, u32 freg, u32 mask, u32 reg_m)
{
	if (!COND(cond))
		return;

	TO_FLAGS(thumb, freg, mask, GPR(reg_m));
}

INLINE void TO_FLAGS(bool thumb, u32 freg, u32 mask, u32 value)
{
	Assert(freg == 0); //unprivileged
	MakeSure((mask & 0xc) == mask); 
	
	if (mask & 0x8)
	{
		cpu.nf = BITS(value, 31, 1);
		cpu.zf = BITS(value, 30, 1);
		cpu.cf = BITS(value, 29, 1);
		cpu.vf = BITS(value, 28, 1);
		cpu.qf = BITS(value, 27, 1);
	}
	if (mask & 0x4)
	{
		cpu.ge = BITS(value, 16, 4);
	}
}

INLINE void FROM_FLAGS(bool thumb, u32 cond, u32 freg, u32 reg_d)
{
	if (!COND(cond))
		return;

	Assert(freg == 0); //unprivileged
	
	u32 value = (cpu.nf << 31) | (cpu.zf << 30) | (cpu.cf << 29) | (cpu.vf << 28) | (cpu.qf << 27) |
		(cpu.ge << 16) | (cpu.thumb << 5) | 0x10;
	setGPR(reg_d, value);
}

INLINE void FROM_COP15(bool thumb, u32 cond, u32 opc1, u32 crn, u32 reg_d, u32 opc2, u32 crm)
{
	if (!COND(cond))
		return;

	if (opc1 == 0 && crn == 13 && opc2 == 3 && crm == 0) //get thread data
		setGPR(reg_d, cpu.threadData);
	
}

}