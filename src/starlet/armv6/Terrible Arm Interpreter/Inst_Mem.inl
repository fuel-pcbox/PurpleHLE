#include "Instruction.h"

namespace Interpreter
{

INLINE void MEM_IMM(bool thumb, u32 cond, u32 preindex, u32 up, u32 byte, u32 wbit, u32 load,
						  u32 reg_n, u32 reg_d, u32 imm)
{
	if (cond == 15 && preindex && !wbit && load && byte) //pld
		return;
	if (!COND(cond))
		return;

	MEM(thumb, preindex, up, false, byte, wbit, load, reg_n, reg_d, imm);
}

INLINE void MEM_SHIFT(bool thumb, u32 cond, u32 preindex, u32 up, u32 byte, u32 wbit, u32 load,
						    u32 reg_n, u32 reg_d, u32 shift_imm, u32 shift, u32 reg_m)
{
	if (cond == 15 && preindex && !wbit && load && byte) //pld
		return;
	if (!COND(cond))
		return;

	u32 shval = SHIFT_IMM(thumb, false, false, shift_imm, shift, reg_m); 

	MEM(thumb, preindex, up, false, byte, wbit, load, reg_n, reg_d, shval);
}

INLINE void MEM_MISC_IMM(bool thumb, u32 cond, u32 preindex, u32 up, u32 wbit, u32 load,
				  u32 reg_n, u32 reg_d, u32 type, u32 imm8)
{
	if (!COND(cond))
		return;

	MEM(thumb, preindex, up, true, type, wbit, load, reg_n, reg_d, imm8);
}

INLINE void MEM_MISC_REG(bool thumb, u32 cond, u32 preindex, u32 up, u32 wbit, u32 load,
				  u32 reg_n, u32 reg_d, u32 type, u32 reg_m)
{
	if (!COND(cond))
		return;

	MEM(thumb, preindex, up, true, type, wbit, load, reg_n, reg_d, GPR(reg_m));
}

INLINE void MEM_SWP(bool thumb, u32 cond, u32 byte, u32 reg_n, u32 reg_d, u32 reg_m)
{
	if (!COND(cond))
		return;

	u32 addr = GPR(reg_n);
	if (!byte)
		MakeSure((addr & 3) == 0);

	u32 loadVal = MEM_LOAD(false, byte, addr);
	MEM_STORE(false, byte, addr, GPR(reg_m));
	setGPR(reg_d, loadVal);
}

INLINE void MEM_LDREX(bool thumb, u32 cond, u32 reg_n, u32 reg_d)
{
	if (!COND(cond))
		return;

	u32 addr = GPR(reg_n); 
	MakeSure((addr & 3) == 0);

	setGPR(reg_d, MEM_LOAD(false, 0, addr));

	cpu.exclusive = true;
}

INLINE void MEM_STREX(bool thumb, u32 cond, u32 reg_n, u32 reg_strex, u32 reg_m)
{
	if (!COND(cond))
		return;

	u32 addr = GPR(reg_n);
	MakeSure((addr & 3) == 0);

	MEM_STORE(false, 0, addr, GPR(reg_m));
	setGPR(reg_strex, cpu.exclusive ? 0 : 1);
}

INLINE void MEM_THUMB_PC(bool thumb, u32 reg_d, u32 imm)
{
	u32 addr = (PCVAL(2, thumb) & ~0x3) + (imm * 4);

	u32 memval = MEM_LOAD( false, 0, addr ); //word load

	setGPR(reg_d, memval);
}

INLINE void MEM(bool thumb, u32 preindex, u32 up, bool misc, u32 type, u32 wbit, u32 load, 
					  u32 reg_n, u32 reg_d, u32 offset) 
{
	bool writeback = (preindex && wbit) || !preindex;
	bool word = !misc && type == 0;

	if (!preindex)
		Assert( !wbit ); //privileged
	if (writeback && reg_d == reg_n ) //invalid
		writeback = 0;
	if (!word)
		Assert( reg_d != 15 ); //invalid

	u32 addr;
	if (writeback)
		addr = GPR(reg_n);
	else
		addr = GPR(reg_n, 2, thumb);

	u32 memval = 0;
	if (!load) //store
		memval = GPR(reg_d, IMPL_SPECIFIC_PC_OFFSET, thumb);

	if (preindex)
	{
		if (up)
			addr += offset;
		else
			addr -= offset;
	}

	if (!load) //store
	{
		MEM_STORE( misc, type, addr, memval ); 
	}
	else
	{
		memval = MEM_LOAD( misc, type, addr ); 
		if (reg_d != 15)
			setGPR(reg_d, memval);
		else
			setPC(memval);
	}

	if (writeback)
	{
		if (preindex)
			setGPR(reg_n, addr);
		else if (up)
			setGPR(reg_n, addr + offset);
		else
			setGPR(reg_n, addr - offset);
	}
}

INLINE u32 MEM_LOAD(bool misc, u32 type, u32 addr)
{
	if (!misc)
	{
		if (type == 0) //WORD
			return ::Ram<u32>(addr);
		else if (type == 1) //BYTE
			return ::Ram<u8>(addr);
		else
			Assert(0);
	}
	else
	{
		if (type == 1) //HWORD
			return ::Ram<u16>(addr);
		else if (type == 2) //SBYTE
			return (s32)(s8)::Ram<u8>(addr);
		else if (type == 3) //SHWORD
			return (s32)(s16)::Ram<u16>(addr);
		else 
			Assert(0);
	}
	return 0;
}

INLINE void MEM_STORE(bool misc, u32 type, u32 addr, u32 val)
{
	if (!misc)
	{
		if (type == 0) //WORD
			::Ram<u32>(addr) = val;
		else if (type == 1) //BYTE
			::Ram<u8>(addr) = val;
		else
			Assert(0);
	}
	else
	{
		if (type == 1) //HWORD
			::Ram<u16>(addr) = val;
		else 
			Assert(0);
	}
}

}