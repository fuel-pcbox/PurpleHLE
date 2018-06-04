#include "Instruction.h"

namespace Interpreter
{

typedef enum {MUL=0, MLA=1, UMAAL=2, UMULL=4, UMLAL=5, SMULL=6, SMLAL=7} MUL_FUNC;

INLINE void MULTIPLY_WORD(bool thumb, u32 cond, u32 func, u32 flags, u32 reg_dh, u32 reg_dl, 
				   u32 reg_s, u32 reg_m)
{
	if (!COND(cond))
		return;

	if (func == MUL)
		Assert( reg_dl == 0 ); //invalid
	if (func >= UMULL)
		Assert( reg_dl != reg_dh ); //invalid

	u32 oper1 = GPR(reg_m);
	u32 oper2 = GPR(reg_s);
	u32 val;
	u64 lval;
	u64 acc;

	switch (func)
	{
	case MUL:
		val = oper1 * oper2;
		setGPR( reg_dh, val );
		break;
	case MLA:
		val = oper1 * oper2 + GPR( reg_dl );
		setGPR( reg_dh, val );
		break;

	case UMAAL:
		Assert(!flags);
		lval = (u64)oper1 * (u64)oper2 + GPR( reg_dl ) + GPR( reg_dh );
		setGPR( reg_dh, lval >> 32 );
		setGPR( reg_dl, (u32)lval );
		break;

	case UMULL:
		lval = (u64)oper1 * (u64)oper2;
		setGPR( reg_dh, lval >> 32 );
		setGPR( reg_dl, (u32)lval );
		break;
	case UMLAL:
		acc = MAKE64(GPR( reg_dl ), GPR( reg_dh ));
		lval = (u64)oper1 * (u64)oper2 + acc;
		setGPR( reg_dh, lval >> 32 );
		setGPR( reg_dl, (u32)lval );
		break;
		
	case SMULL:
		lval = (s64)(s32)oper1 * (s64)(s32)oper2;
		setGPR( reg_dh, lval >> 32 );
		setGPR( reg_dl, (u32)lval );
		break;
	case SMLAL:
		acc = MAKE64(GPR( reg_dl ), GPR( reg_dh ));
		lval = (s64)(s32)oper1 * (s64)(s32)oper2 + acc;
		setGPR( reg_dh, lval >> 32 );
		setGPR( reg_dl, (u32)lval );
		break;
	default:
		Assert(0);
	}
	if (flags)
	{
		if (func == MUL || func == MLA)
		{
			cpu.zf = (val == 0) ? 1 : 0;
			cpu.nf = ((s32)val < 0) ? 1 : 0;
		}
		else
		{
			cpu.zf = (lval == 0) ? 1 : 0;
			cpu.nf = ((s64)lval < 0) ? 1 : 0;
		}
	}
}

INLINE void MULTIPLY_SHORT(bool thumb, u32 cond, u32 func, u32 reg_dh, u32 reg_dl, u32 reg_s, u32 x, u32 y, u32 reg_m)
{
	if (!COND(cond))
		return;

	multival oper1, oper2;
	oper1.uv32 = GPR(reg_m);
	oper2.uv32 = GPR(reg_s);

	if (func == 0) //SMLA
	{
		s32 acc = GPR(reg_dl);
		s32 prod = oper1.sv16[x] * oper2.sv16[y];
		s32 res = acc + prod;

		setGPR(reg_dh, res);
		if ((s64)prod + (s64)acc != res )
			cpu.qf = 1;
	}
	else if (func == 1 && x == 0) //SMLAW
	{
		s32 acc = GPR(reg_dl);
		s32 prod = (s32)(((s64)oper1.sv32 * (s64)oper2.sv16[y]) >> 16);
		s32 res = acc + prod;

		setGPR(reg_dh, res);
		if ((s64)prod + (s64)acc != res )
			cpu.qf = 1;
	}
	else if (func == 1 && x == 1) //SMULW
	{
		s32 prod = (s32)(((s64)oper1.sv32 * (s64)oper2.sv16[y]) >> 16);
		setGPR(reg_dh, prod);
	}
	else if (func == 2) //SMLAL
	{
		s64 acc = MAKE64(GPR( reg_dl ), GPR( reg_dh ));
		s64 prod = oper1.sv16[x] * oper2.sv16[y];
		s64 res = acc + prod;

		setGPR( reg_dh, res >> 32 );
		setGPR( reg_dl, (u32)res );
	}
	else if (func == 3) //SMUL
	{
		s32 prod = oper1.sv16[x] * oper2.sv16[y];
		setGPR(reg_dh, prod);
	}

}

}