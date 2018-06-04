#include "Instruction.h"

namespace Interpreter
{

typedef enum {SXTB16=0, SXTB=2, SXTH=3, UXTB16=4, UXTB=6, UXTH=7} EXT_FUNC;

INLINE void EXTEND(bool thumb, u32 cond, u32 func, u32 reg_n, u32 reg_d, u32 rot_imm, u32 reg_m)
{
	if (!COND(cond))
		return;

	multival a;
	a.uv32 = GPR(reg_m);
	if (rot_imm != 0)
		a.uv32 = _rotr(a.uv32, 8 * rot_imm);

	switch (func)
	{
	case SXTB:
		a.sv32 = (s8)a.sv32;
		break;

	case SXTH:
		a.sv32 = (s16)a.sv32;
		break;
		
	case UXTB:
		a.uv32 = (u8)a.uv32;
		break;

	case UXTH:
		a.uv32 = (u16)a.uv32;
		break;

	case UXTB16:
		a.uv16[0] = (u8)a.uv16[0];
		a.uv16[1] = (u8)a.uv16[1];
		break;

	case SXTB16:
		a.sv16[0] = (s8)a.sv16[0];
		a.sv16[1] = (s8)a.sv16[1];
		break;

	NODEFAULT(func);
	}

	if (reg_n != 15)
	{
		if (func == UXTB16 || func == SXTB16)
		{
			multival b;
			b.uv32 = GPR(reg_n);
			a.uv16[0] += b.uv16[0];
			a.uv16[1] += b.uv16[1];
		}
		else
			a.uv32 += GPR(reg_n);
	}

	setGPR(reg_d, a.uv32);
}

INLINE void REV(bool thumb, u32 cond, u32 sh_op, u32 reg_d, u32 h_op, u32 reg_m)
{
	if (!COND(cond))
		return;

	u32 val = GPR(reg_m);

	if (h_op)
	{
		u8 ll = BITS(val, 0, 8);
		u8 lh = BITS(val, 8, 8);
		u8 hl = BITS(val, 16, 8);
		u8 hh = BITS(val, 24, 8);
		val = (hl << 24) | (hh << 16) | (ll << 8) | lh;
	}
	else if (sh_op)
	{
		Assert(h_op);
		u8 ll = BITS(val, 0, 8);
		u8 lh = BITS(val, 8, 8);
		val = (ll << 8) | lh;
		val = (s16)val;
	}
	else
	{
		u8 ll = BITS(val, 0, 8);
		u8 lh = BITS(val, 8, 8);
		u8 hl = BITS(val, 16, 8);
		u8 hh = BITS(val, 24, 8);
		val = (ll << 24) | (lh << 16) | (hl << 8) | hh;
	}

	setGPR(reg_d, val);	
}

INLINE void CLZ(bool thumb, u32 cond, u32 reg_d, u32 reg_m)
{
	if (!COND(cond))
		return;

	u32 val = GPR(reg_m);

	u32 clz = 0;
	for (int i = 31; i >= 0; i--)
	{
		if (val & (1 << i))
			break;
		clz++;
	}
	setGPR(reg_d, clz);
}

}