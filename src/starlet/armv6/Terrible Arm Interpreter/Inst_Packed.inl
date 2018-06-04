#include "Instruction.h"

namespace Interpreter
{

typedef enum {Sign=1, SignSat=2, SignHalf=3, Unsign=5, UnsignSat=6, UnsignHalf=7} ARITH_TYPE;

typedef enum {Add16=0, AddSub=1, SubAdd=2, Sub16=3, Add8=4, Sub8=7} PACK_TYPE;

const u32 max16 = 0x10000;
const u32 max8 = 0x100;

INLINE void PARALLEL(bool thumb, u32 cond, u32 arithFunc, u32 reg_n, u32 reg_d, u32 packFunc, u32 reg_m)
{
	if (!COND(cond))
		return;

	multival a, b, res;
	a.uv32 = GPR(reg_n);
	b.uv32 = GPR(reg_m);

	switch (arithFunc + (packFunc << 3))
	{
	case Unsign + (Add16 << 3):
		cpu.ge = 0;
		for (int i = 0; i < 2; i++) 
		{
			u32 result = a.uv16[i] + b.uv16[i];
			cpu.ge |= ((result >= max16) ? 3 : 0) << (i * 2);
			res.uv16[i] = (u16)result;
		}
		break;
		
	case Unsign + (Add8 << 3):
		cpu.ge = 0;
		for (int i = 0; i < 4; i++) 
		{
			u32 result = a.uv8[i] + b.uv8[i];
			cpu.ge |= ((result >= max8) ? 1 : 0) << i;
			res.uv8[i] = (u8)result;
		}
		break;
		
	case Unsign + (Sub16 << 3):
		cpu.ge = 0;
		for (int i = 0; i < 2; i++) 
		{
			u32 result = a.uv16[i] - b.uv16[i];
			cpu.ge |= ((result >= max16) ? 0 : 3) << (i * 2);
			res.uv16[i] = (u16)result;
		}
		break;
		
	case Unsign + (Sub8 << 3):
		cpu.ge = 0;
		for (int i = 0; i < 4; i++) 
		{
			u32 result = a.uv8[i] - b.uv8[i];
			cpu.ge |= ((result >= max8) ? 0 : 1) << i;
			res.uv8[i] = (u8)result;
		}
		break;		

	case Sign + (Add16 << 3):
		cpu.ge = 0;
		for (int i = 0; i < 2; i++) 
		{
			s32 result = a.sv16[i] + b.sv16[i];
			cpu.ge |= ((result >= 0) ? 3 : 0) << (i * 2);
			res.sv16[i] = (s16)result;
		}
		break;
		
	case Sign + (Add8 << 3):
		cpu.ge = 0;
		for (int i = 0; i < 4; i++) 
		{
			s32 result = a.sv8[i] + b.sv8[i];
			cpu.ge |= ((result >= 0) ? 1 : 0) << i;
			res.sv8[i] = (s8)result;
		}
		break;
		
	case Sign + (Sub16 << 3):
		cpu.ge = 0;
		for (int i = 0; i < 2; i++) 
		{
			s32 result = a.sv16[i] - b.sv16[i];
			cpu.ge |= ((result >= 0) ? 3 : 0) << (i * 2);
			res.sv16[i] = (s16)result;
		}
		break;
		
	case Sign + (Sub8 << 3):
		cpu.ge = 0;
		for (int i = 0; i < 4; i++) 
		{
			s32 result = a.sv8[i] - b.sv8[i];
			cpu.ge |= ((result >= 0) ? 1 : 0) << i;
			res.sv8[i] = (s8)result;
		}
		break;	
		
	NODEFAULT(arithFunc + (packFunc << 3));
	}

	setGPR(reg_d, res.uv32);

}

INLINE void PACK(bool thumb, u32 cond, u32 reg_n, u32 reg_d, u32 shamt, u32 op, u32 reg_m)
{
	if (!COND(cond))
		return;

	multival a, b, res;
	a.uv32 = GPR(reg_n);
	b.uv32 = GPR(reg_m);

	if (op == 0) //Bottom-Top, shift left
	{
		b.uv32 <<= shamt;
		res.uv16[0] = a.uv16[0];
		res.uv16[1] = b.uv16[1];
	}
	else //Top-Bottom, shift arith right
	{
		if (shamt == 0) //by 32
			b.sv32 >>= 31;
		else
			b.sv32 >>= shamt;
		res.uv16[0] = b.uv16[0];
		res.uv16[1] = a.uv16[1];
	}

	setGPR(reg_d, res.uv32);
}

}