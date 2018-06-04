#include "Instruction.h"
#include <float.h>
#pragma fenv_access (on)

namespace Interpreter
{

INLINE u64 VFP_MEM_LOAD(u32 addr, u32 dbl)
{
	MakeSure((addr & 3) == 0);
	if (dbl == 0) //float
		return ::Ram<u32>(addr);
	else          //double
	{
		u64 low = ::Ram<u32>(addr);  //endianess-dependant
		u64 high = ::Ram<u32>(addr + 4);
		return (high << 32) | low;
	}
}

INLINE void VFP_MEM_STORE(u32 addr, u64 val, u32 dbl)
{
	MakeSure((addr & 3) == 0);
	if (dbl == 0) //float
		::Ram<u32>(addr) = (u32)val;
	else          //double
	{
		::Ram<u32>(addr) = (u32)val;  //endianess-dependant
		::Ram<u32>(addr + 4) = (u32)(val >> 32);
	}
}

INLINE void VFP_MEM(bool thumb, u32 cond, u32 up, u32 load, u32 reg_n, u32 freg_d, u32 dbl, u32 offset)
{
	if (!COND(cond))
		return;
	
	u32 addr = GPR(reg_n, 2, thumb);

	if (up)
		addr += offset*4;
	else
		addr -= offset*4;

	if (!load) //store
		VFP_MEM_STORE( addr, FPR_U(freg_d, dbl), dbl );
	else
		setFPR_U(freg_d, VFP_MEM_LOAD( addr, dbl ), dbl);

}

INLINE void VFP_MEM_MULTIPLE(bool thumb, u32 cond, u32 writeback, u32 up, u32 load, u32 reg_n, u32 freg_d,
							 u32 dbl, u32 offset)
{
	if (!COND(cond))
		return;

	u32 addr;
	if (writeback)
		addr = GPR(reg_n);
	else
		addr = GPR(reg_n, 2, thumb);

	int inc = dbl ? 2 : 1;
	if (!up)
		addr -= 4 * offset;

	for (u32 i = 0; i < offset; i += inc)
	{
		if (dbl && (offset - i) == 1)
			break;

		if (!load) //store
			VFP_MEM_STORE( addr + 4*i, FPR_U(freg_d + i, dbl), dbl );
		else
			setFPR_U(freg_d + i, VFP_MEM_LOAD( addr + 4*i, dbl ), dbl);
	}

	if (writeback)
	{
		if (up)
			setGPR(reg_n, addr + 4*offset);
		else
			setGPR(reg_n, addr);
	}
}

INLINE void VFP_DMOVE(bool thumb, u32 cond, u32 togpr, u32 reg_high, u32 reg_low, u32 dbl, u32 freg_m)
{
	if (!COND(cond))
		return;

	if (!togpr)
	{
		setFPR_U(freg_m, GPR(reg_low), 0);
		setFPR_U(freg_m + 1, GPR(reg_high), 0);
	}
	else
	{
		setGPR(reg_low, (u32)FPR_U(freg_m, 0));
		setGPR(reg_high, (u32)FPR_U(freg_m + 1, 0));
	}

}

INLINE void VFP_MOVE(bool thumb, u32 cond, u32 opcode, u32 togpr, u32 freg_n, u32 reg_d, u32 dbl)
{
	if (!COND(cond))
		return;

	if (opcode != 7 || !togpr)
		Assert(reg_d != 15);


	if (opcode == 0 && !togpr)
		setFPR_U(freg_n, GPR(reg_d), 0); //single or low half of double

	else if (opcode == 0 && togpr)
		setGPR(reg_d, (u32)FPR_U(freg_n, 0)); //single or low half of double

	else if (opcode == 1 && dbl && !togpr)
		setFPR_U(freg_n + 1, GPR(reg_d), 0); //upper half of double
	
	else if (opcode == 1 && dbl && togpr)
		setGPR(reg_d, (u32)FPR_U(freg_n + 1, 0)); //upper half of double

	else if (opcode == 7 && freg_n == 2 && !dbl && !togpr) //FPSCR
	{
		u32 value = GPR(reg_d);
		cpu.fpnf = BITS(value, 31, 1);
		cpu.fpzf = BITS(value, 30, 1);
		cpu.fpcf = BITS(value, 29, 1);
		cpu.fpvf = BITS(value, 28, 1);
		cpu.vlen = BITS(value, 16, 3);
		cpu.vstride = BITS(value, 20, 2);
		cpu.fpmode = value & 0x0fc8ffff; 

		u32 ftz = BITS(cpu.fpmode, 24, 1);
		u32 round = BITS(cpu.fpmode, 22, 2);

		_control87(ftz ? _DN_FLUSH : _DN_SAVE, _MCW_DN);
		_control87((round == 0) ? _RC_NEAR : (round == 1) ? _RC_UP : (round == 2) ? _RC_DOWN : _RC_CHOP, _MCW_RC);
	}

	else if (opcode == 7 && freg_n == 2 && !dbl && togpr && reg_d != 15) //FPSCR
	{
		setGPR(reg_d, (cpu.fpnf << 31) | (cpu.fpzf << 30) | (cpu.fpcf << 29) | (cpu.fpvf << 28) | 
			(cpu.vlen << 16) | (cpu.vstride << 20) | cpu.fpmode);
	}

	else if (opcode == 7 && freg_n == 2 && !dbl && togpr && reg_d == 15) //FMSTAT
	{
		cpu.nf = cpu.fpnf;
		cpu.zf = cpu.fpzf;
		cpu.cf = cpu.fpcf;
		cpu.vf = cpu.fpvf;
	}

	else 
		Fail(opcode);
}

}