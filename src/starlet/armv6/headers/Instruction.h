#pragma once
#include "types.h"
#include "CpuState.h"

namespace Interpreter
{

#define IMPL_SPECIFIC_PC_OFFSET 2

INLINE u32 GPR(u32 reg); //PC is not allowed
INLINE void setGPR(u32 reg, u32 val); //PC is not allowed
INLINE u32 GPR(u32 reg, int inst_offset, bool thumb); //PC is allowed with inst offset
INLINE u32 PCVAL(int inst_offset, bool thumb);
INLINE u8 COND(u32 cond);
INLINE void setPC(u32 target); 

INLINE u64 FPR_U(u32 reg, u32 dbl);
INLINE void setFPR_U(u32 reg, u64 val, u32 dbl);
INLINE double FPR_F(u32 reg, u32 dbl); //bad but i don't care
INLINE void setFPR_F(u32 reg, double val, u32 dbl); //bad but i don't care


INLINE u32 MEM_LOAD(bool misc, u32 type, u32 addr);
INLINE void MEM_STORE(bool misc, u32 type, u32 addr, u32 val);

INLINE u32 SHIFT_IMM(bool thumb, bool allowPC, bool c_out, u32 shift_imm, u32 shift, u32 reg_m);

INLINE u32 SHIFT_REG(bool thumb, bool c_out, u32 reg_s, u32 shift, u32 reg_m);

INLINE void DATA(bool thumb, bool allowPC, u32 func, u32 flags, u32 reg_n, u32 reg_d, u32 shval);

INLINE void MEM(bool thumb, u32 preindex, u32 add, bool misc, u32 type, u32 wbit, u32 load, 
		 u32 reg_n, u32 reg_d, u32 offset); 

INLINE void DATA_IMM(bool thumb, u32 cond, u32 func, u32 flags, u32 reg_n, u32 reg_d, u32 rot_imm, u32 imm);

INLINE void DATA_SHIFT_IMM(bool thumb, u32 cond, u32 func, u32 flags, u32 reg_n, u32 reg_d, u32 shift_imm, u32 shift, u32 reg_m);

INLINE void DATA_SHIFT_REG(bool thumb, u32 cond, u32 func, u32 flags, u32 reg_n, u32 reg_d, u32 reg_s, u32 shift, u32 reg_m);

INLINE void MEM_IMM(bool thumb, u32 cond, u32 preindex, u32 add, u32 byte, u32 wbit, u32 load,
			 u32 reg_n, u32 reg_d, u32 imm);

INLINE void MEM_SHIFT(bool thumb, u32 cond, u32 preindex, u32 add, u32 byte, u32 wbit, u32 load,
			   u32 reg_n, u32 reg_d, u32 shift_imm, u32 shift, u32 reg_m);

INLINE void MEM_LDREX(bool thumb, u32 cond, u32 reg_n, u32 reg_d);

INLINE void MEM_STREX(bool thumb, u32 cond, u32 reg_n, u32 reg_strex, u32 reg_m);

INLINE void MEM_MISC_IMM(bool thumb, u32 cond, u32 preindex, u32 add, u32 wbit, u32 load,
				  u32 reg_n, u32 reg_d, u32 type, u32 imm8);

INLINE void MEM_MISC_REG(bool thumb, u32 cond, u32 preindex, u32 add, u32 wbit, u32 load,
				  u32 reg_n, u32 reg_d, u32 type, u32 reg_m);

INLINE void MEM_MULTIPLE(bool thumb, u32 cond, u32 preindex, u32 add, u32 special, u32 writeback, 
				  u32 load, u32 reg_n, u32 reglist);

INLINE void MEM_SWP(bool thumb, u32 cond, u32 byte, u32 reg_n, u32 reg_d, u32 reg_m);

INLINE void BRANCH_IMM(bool thumb, u32 cond, u32 link, u32 imm24);

INLINE void BRANCH_LX_ARM(bool thumb, u32 hbit, u32 imm24);

INLINE void BRANCH_LX_THUMB(bool thumb, u32 imm24);

INLINE void BRANCH_L_THUMB(bool thumb, u32 imm24);

INLINE void BRANCH_CHANGE_REG(bool thumb, u32 cond, u32 link, u32 reg_m);

INLINE void MULTIPLY_WORD(bool thumb, u32 cond, u32 func, u32 flags, u32 reg_dh, u32 reg_dl, 
				   u32 reg_s, u32 reg_m);

INLINE void MULTIPLY_SHORT(bool thumb, u32 cond, u32 func, u32 reg_dh, u32 reg_dl, 
				   u32 reg_s, u32 x, u32 y, u32 reg_m);

INLINE void EXTEND(bool thumb, u32 cond, u32 func, u32 reg_n, u32 reg_d, u32 rot_imm, u32 reg_m);

INLINE void REV(bool thumb, u32 cond, u32 sh_op, u32 reg_d, u32 h_op, u32 reg_m);

INLINE void CLZ(bool thumb, u32 cond, u32 reg_d, u32 reg_m);

INLINE void PARALLEL(bool thumb, u32 cond, u32 arithFunc, u32 reg_n, u32 reg_d, u32 packFunc, u32 reg_m);

INLINE void PACK(bool thumb, u32 cond, u32 reg_n, u32 reg_d, u32 shamt, u32 op, u32 reg_m);

INLINE void MEM_THUMB_PC(bool thumb, u32 reg_d, u32 imm8);

INLINE void DATA_THUMB_PC(bool thumb, u32 reg_d, u32 imm8);

INLINE void TO_FLAGS_IMM(bool thumb, u32 cond, u32 freg, u32 mask, u32 rot_imm, u32 imm);

INLINE void TO_FLAGS_REG(bool thumb, u32 cond, u32 freg, u32 mask, u32 reg_m);

INLINE void FROM_FLAGS(bool thumb, u32 cond, u32 freg, u32 reg_d);

INLINE void TO_FLAGS(bool thumb, u32 freg, u32 mask, u32 value);

INLINE void SWI(bool thumb, u32 cond, u32 swi);

INLINE void BKPT(bool thumb, u32 cond, u32 bkpt);

INLINE void FROM_COP15(bool thumb, u32 cond, u32 opc1, u32 crn, u32 reg_d, u32 opc2, u32 crm);

INLINE void VFP_MEM(bool thumb, u32 cond, u32 up, u32 load, u32 reg_n, u32 freg_d, u32 dbl, u32 offset);

INLINE void VFP_MEM_MULTIPLE(bool thumb, u32 cond, u32 writeback, u32 up, u32 load, u32 reg_n, u32 freg_d,
							 u32 dbl, u32 offset);

INLINE void VFP_DMOVE(bool thumb, u32 cond, u32 togpr, u32 reg_n, u32 reg_d, u32 dbl, u32 freg_m);

INLINE void VFP_MOVE(bool thumb, u32 cond, u32 type, u32 togpr, u32 freg_n, u32 reg_d, u32 dbl);

INLINE void VFP_ACC3VOP(bool thumb, u32 cond, u32 opcode, u32 freg_n, u32 freg_d, u32 dbl, u32 freg_m);

INLINE void VFP_3VOP(bool thumb, u32 cond, u32 opcode, u32 freg_n, u32 freg_d, u32 dbl, u32 freg_m);

INLINE void VFP_2VOP(bool thumb, u32 cond, u32 opcode, u32 freg_d, u32 dbl, u32 freg_m);

INLINE void VFP_2FOP(bool thumb, u32 cond, u32 opcode, u32 freg_d, u32 dbl, u32 freg_m);

INLINE void DecodeArm(u32 op);
INLINE void DecodeThumb(u16 op);
void ExecuteOne();

template <typename T>
INLINE T MemRead(u32 addr);
template <typename T>
INLINE void MemWrite(u32 addr, T value);

}

union multival
{
	u32 uv32;
	s32 sv32;
	u16 uv16[2];
	u8 uv8[4];
	s16 sv16[2];
	s8 sv8[4];
};

#include "Inst_Bits.inl"
#include "Inst_Branch.inl"
#include "Inst_Data.inl"
#include "Inst_Flags.inl"
#include "Inst_Mem.inl"
#include "Inst_Misc.inl"
#include "Inst_Multiple.inl"
#include "Inst_Multiply.inl"
#include "Inst_Packed.inl"
#include "Inst_Shift.inl"
#include "Inst_Vfp.inl"
#include "Inst_VfpMem.inl"

#include "Decode.inl" //included last (or use more #undefs there)