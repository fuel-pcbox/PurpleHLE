#include "Instruction.h"

namespace Interpreter
{

typedef enum {LSL=0, LSR, ASR, ROR} SHIFT_FUNC;

INLINE u32 SHIFT_IMM(bool thumb, bool allowPC, bool c_out, u32 shift_imm, u32 shift, u32 reg_m) 
{
	u32 val;
	if (allowPC)
		val = GPR(reg_m, 2, thumb);
	else
		val = GPR(reg_m);

	if (shift == 0 && shift_imm == 0)
		return val;
	switch (shift)
	{
	case LSL:
		Assert (shift_imm != 0);
		if (c_out)
			cpu.cf = (val >> (32 - shift_imm)) & 1;
		return val << shift_imm;

	case LSR:
		if (shift_imm != 0)
		{
			if (c_out)
				cpu.cf = (val >> (shift_imm - 1)) & 1;
			return val >> shift_imm;
		}
		else //lsr 32
		{
			if (c_out)
				cpu.cf = (val >> 31);
			return 0;
		}

	case ASR:
		if (shift_imm != 0)
		{
			if (c_out)
				cpu.cf = (val >> (shift_imm - 1)) & 1;
			return (s32)val >> shift_imm;
		}
		else //asr 32
		{
			if (c_out)
				cpu.cf = (val >> 31);
			return (s32)val >> 31;
		}

	case ROR:
		if (shift_imm != 0)
		{
			if (c_out)
				cpu.cf = (val >> (shift_imm - 1)) & 1;
			return _rotr(val, shift_imm);
		}
		else //rrx
		{
			u32 cf = cpu.cf;
			if (c_out)
				cpu.cf = val & 1;
			return (val >> 1) | (cf << 31);
		}

	NODEFAULT(shift);
	}
	return 0;
}

INLINE u32 lsl_reg(bool carry, u8 shamt, u32 val)
{
	if (carry)
	{
		if (shamt > 32)
			cpu.cf = 0;
		else if (shamt != 0)
			cpu.cf = (val >> (32 - shamt)) & 1;
	}

	if (shamt < 32)
		return val << shamt;
	else
		return 0;
}
INLINE u32 lsr_reg(bool carry, u8 shamt, u32 val)
{
	if (carry)
	{
		if (shamt > 32)
			cpu.cf = 0;
		else if (shamt != 0)
			cpu.cf = (val >> (shamt - 1)) & 1;
	}

	if (shamt < 32)
		return val >> shamt;
	else
		return 0;
}
INLINE u32 asr_reg(bool carry, u8 shamt, u32 val)
{
	if (carry)
	{
		if (shamt > 32)
			cpu.cf = (val >> 31) & 1;
		else if (shamt != 0)
			cpu.cf = (val >> (shamt - 1)) & 1;
	}

	if (shamt < 32)
		return (s32)val >> shamt;
	else
		return (s32)val >> 31;
}
INLINE u32 ror_reg(bool carry, u8 shamt, u32 val)
{
	if (carry)
		if (shamt != 0)
			cpu.cf = (val >> ((shamt - 1) & 0x1f)) & 1;

	return _rotr(val, shamt & 0x1f);
}

INLINE u32 SHIFT_REG(bool thumb, bool c_out, u32 reg_s, u32 shift, u32 reg_m)
{
	u32 val = GPR(reg_m);
	u8 shval = (u8)GPR(reg_s);
	switch (shift)
	{
	case LSL:
		return lsl_reg(c_out, shval, val);

	case LSR:
		return lsr_reg(c_out, shval, val);

	case ASR:
		return asr_reg(c_out, shval, val);

	case ROR:
		return ror_reg(c_out, shval, val);

	NODEFAULT(shift);
	}
	return 0;
}

}