#include "Instruction.h"

namespace Interpreter
{

typedef enum {AND=0, EOR, SUB, RSB, ADD, ADC, SBC, RSC, TST, TEQ, CMP, CMN, ORR, MOV, BIC, MVN} DATA_FUNC;

INLINE bool is_logical(u32 func)
{
	return (func == AND || func == EOR || func == TST || func == TEQ ||
		func == ORR || func == MOV || func == BIC || func == MVN);
}
INLINE bool is_compare(u32 func)
{
	return (func == TST || func == TEQ || func == CMP || func == CMN);
}
INLINE bool is_move(u32 func)
{
	return (func == MOV || func == MVN);
}

INLINE void DATA_IMM(bool thumb, u32 cond, u32 func, u32 flags, u32 reg_n, u32 reg_d, u32 rot_imm, u32 imm)
{
	if (!COND(cond))
		return;

	u32 imm32 = _rotr(imm, rot_imm * 2);

	if (flags && is_logical(func) && rot_imm != 0)
		cpu.cf = (imm32 >> 31);

	DATA(thumb, true, func, flags, reg_n, reg_d, imm32);
}

INLINE void DATA_SHIFT_IMM(bool thumb, u32 cond, u32 func, u32 flags, u32 reg_n, u32 reg_d, u32 shift_imm, u32 shift, u32 reg_m)
{
	if (!COND(cond))
		return;

	bool shift_c_out = flags && is_logical(func);

	u32 shval = SHIFT_IMM(thumb, true, shift_c_out, shift_imm, shift, reg_m);

	DATA(thumb, true, func, flags, reg_n, reg_d, shval);
}

INLINE void DATA_SHIFT_REG(bool thumb, u32 cond, u32 func, u32 flags, u32 reg_n, u32 reg_d, u32 reg_s, u32 shift, u32 reg_m)
{
	if (!COND(cond))
		return;

	bool shift_c_out = flags && is_logical(func);

	u32 shval = SHIFT_REG(thumb, shift_c_out, reg_s, shift, reg_m);

	DATA(thumb, false, func, flags, reg_n, reg_d, shval);
}

INLINE void DATA_THUMB_PC(bool thumb, u32 reg_d, u32 imm8)
{
	setGPR(reg_d, (PCVAL(2, thumb) & ~0x3) + (imm8 * 4));
}

INLINE void DATA(bool thumb, bool allowPC, u32 func, u32 flags, u32 reg_n, u32 reg_d, u32 shval) 
{
	if (is_move(func))
		Assert(reg_n == 0); //invalid
	if (is_compare(func))
		Assert(reg_d == 0 && flags); //invalid
	if (flags)
		Assert(reg_d != 15); //privileged

	u32 val;

	if (allowPC)
		val = GPR(reg_n, 2, thumb);
	else
		val = GPR(reg_n);

	switch (func)
	{
	case AND:
	case TST:
		val = val & shval;
		break;

	case EOR:
	case TEQ:
		val = val ^ shval;
		break;

	case SUB:
	case CMP:
		if (flags)
		{
			u8 cf, vf;
			__asm
			{
				mov eax, [val];
				sub eax, [shval];
				setnc [cf];
				seto [vf];
				mov [val], eax;
			}
			cpu.cf = cf;
			cpu.vf = vf;
		}
		else
			val = val - shval;
		break;

	case RSB:
		if (flags)
		{
			u8 cf, vf;
			__asm
			{
				mov eax, [shval];
				sub eax, [val];
				setnc [cf];
				seto [vf];
				mov [val], eax;
			}
			cpu.cf = cf;
			cpu.vf = vf;
		}
		else
			val = shval - val;
		break;

	case ADD:
	case CMN:
		if (flags)
		{
			u8 cf, vf;
			__asm
			{
				mov eax, [val];
				add eax, [shval];
				setc [cf];
				seto [vf];
				mov [val], eax;
			}
			cpu.cf = cf;
			cpu.vf = vf;
		}
		else
			val = val + shval;
		break;
		
	case ADC:
		if (flags)
		{
			u8 cf = cpu.cf, vf;
			__asm
			{
				cmp [cf], 1;
				cmc;
				mov eax, [val];
				adc eax, [shval];
				setc [cf];
				seto [vf];
				mov [val], eax;
			}
			cpu.cf = cf;
			cpu.vf = vf;
		}
		else
			val = val + shval + cpu.cf;
		break;
		
	case SBC:
		if (flags)
		{
			u8 cf = cpu.cf, vf;
			__asm
			{
				cmp [cf], 1;
				mov eax, [val];
				sbb eax, [shval];
				setnc [cf];
				seto [vf];
				mov [val], eax;
			}
			cpu.cf = cf;
			cpu.vf = vf;
		}
		else
			val = val - shval - (cpu.cf ^ 1);
		break;
		
	case RSC:
		if (flags)
		{
			u8 cf = cpu.cf, vf;
			__asm
			{
				cmp [cf], 1;
				mov eax, [shval];
				sbb eax, [val];
				setnc [cf];
				seto [vf];
				mov [val], eax;
			}
			cpu.cf = cf;
			cpu.vf = vf;
		}
		else
			val = shval - val - (cpu.cf ^ 1);
		break;
		
	case ORR:
		val = val | shval;
		break;
		
	case MOV:
		val = shval;
		break;

	case BIC:
		val = val & ~shval;
		break;
		
	case MVN:
		val = ~shval;
		break;
	}

	if (flags)
	{
		cpu.zf = (val == 0) ? 1 : 0;
		cpu.nf = ((s32)val < 0) ? 1 : 0;
	}

	if (!is_compare(func))
	{
		if (reg_d != 15) 
			setGPR(reg_d, val);
		else
		{
			Assert(allowPC);
			cpu.nia = (val & ~1); //doesn't change processor state
		}
	}
}

}