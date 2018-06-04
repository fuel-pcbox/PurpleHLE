#include "Instruction.h"
#include "Memory.h"

namespace Interpreter
{


INLINE u32 GPR(u32 reg)
{
	Assert( reg != 15 );
	return cpu.gpr[reg];
}

INLINE void setGPR(u32 reg, u32 val)
{
	Assert( reg != 15 );
	cpu.gpr[reg] = val;
}

INLINE u32 GPR(u32 reg, int inst_offset, bool thumb) 
{
	if (reg != 15)
		return cpu.gpr[reg];
	else
		return PCVAL(inst_offset, thumb);
}

INLINE u32 PCVAL(int inst_offset, bool thumb)
{
	return cpu.nia + ((inst_offset - 1) * (thumb ? 2 : 4));
}

INLINE u8 COND(u32 cond)
{
	Assert( cond != 15 );
	if (cond == 14) //AL
		return 1;
	switch (cond)
	{
	case 0: //EQ
		return cpu.zf;
	case 1: //NE
		return !cpu.zf;
	case 2: //HS/CS
		return cpu.cf;
	case 3: //LO/CC
		return !cpu.cf;
	case 4: //MI
		return cpu.nf;
	case 5: //PL
		return !cpu.nf;
	case 6: //VS
		return cpu.vf;
	case 7: //VC
		return !cpu.vf;
	case 8: //HI
		return cpu.cf && !cpu.zf;
	case 9: //LS
		return !cpu.cf || cpu.zf;
	case 10: //GE
		return cpu.nf == cpu.vf;
	case 11: //LT
		return cpu.nf != cpu.vf;
	case 12: //GT
		return !cpu.zf && (cpu.nf == cpu.vf);
	case 13: //LE
		return cpu.zf || (cpu.nf != cpu.vf);
	NODEFAULT(cond);
	}
	return 0;
}

INLINE void setPC(u32 target)
{
	cpu.nia = target & ~1;
	cpu.thumb = target & 1;
}

INLINE u64 FPR_U(u32 reg, u32 dbl)
{
	Assert(reg < 32);
	if (dbl)
		return cpu.fpr64[reg/2];
	else
		return cpu.fpr32[reg];
}

INLINE void setFPR_U(u32 reg, u64 val, u32 dbl)
{
	Assert(reg < 32);
	if (dbl)
		cpu.fpr64[reg/2] = val;
	else
		cpu.fpr32[reg] = (u32)val;
}

INLINE double FPR_F(u32 reg, u32 dbl) //bad but i don't care
{
	if (dbl)
		return cpu.fprd[reg/2];
	else
		return cpu.fprs[reg];
}
INLINE void setFPR_F(u32 reg, double val, u32 dbl) //bad but i don't care
{
	if (dbl)
		cpu.fprd[reg/2] = val;
	else
		cpu.fprs[reg] = (float)val;
}

}