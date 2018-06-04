#include "Instruction.h"

namespace Interpreter
{

#define REG_0 BITS(op, 0, 3)
#define REG_3 BITS(op, 3, 3)
#define REG_6 BITS(op, 6, 3)
#define REG_8 BITS(op, 8, 3)

#define REG_SP 13

#define REGX_0 (BITS(op, 0, 3) | (BITS(op, 7, 1) << 3))
#define REGX_3 BITS(op, 3, 4)

#define IMM3_6 BITS(op, 6, 3)
#define IMM5_6 BITS(op, 6, 5)
#define IMM7 BITS(op, 0, 7)
#define IMM8 BITS(op, 0, 8)
#define IMM11 BITS(op, 0, 11)

#define ARM_AND 0
#define ARM_EOR 1
#define ARM_SUB 2
#define ARM_RSB 3
#define ARM_ADD 4
#define ARM_ADC 5
#define ARM_SBC 6
#define ARM_TST 8
#define ARM_CMP 0xa
#define ARM_CMN 0xb
#define ARM_ORR 0xc
#define ARM_MOV 0xd
#define ARM_BIC 0xe
#define ARM_MVN 0xf

#define ARM_LSL 0
#define ARM_LSR 1
#define ARM_ASR 2
#define ARM_ROR 3

#define ARMMEM_H 1
#define ARMMEM_SB 2
#define ARMMEM_SH 3

#define ARM_SXTB 2
#define ARM_SXTH 3
#define ARM_UXTB 6
#define ARM_UXTH 7

#define NOCOND 0xe

void DecodeThumb(u16 op)
{
	if ((op & 0xf800) == 0x0000) //lsl imm
		DATA_SHIFT_IMM(true, NOCOND, ARM_MOV, 1, 0, REG_0, IMM5_6, ARM_LSL, REG_3);

	else if ((op & 0xf800) == 0x0800) //lsr imm
		DATA_SHIFT_IMM(true, NOCOND, ARM_MOV, 1, 0, REG_0, IMM5_6, ARM_LSR, REG_3);

	else if ((op & 0xf800) == 0x1000) //asr imm
		DATA_SHIFT_IMM(true, NOCOND, ARM_MOV, 1, 0, REG_0, IMM5_6, ARM_ASR, REG_3);

	else if ((op & 0xfe00) == 0x1800) //add
		DATA_SHIFT_IMM(true, NOCOND, ARM_ADD, 1, REG_3, REG_0, 0, 0, REG_6);

	else if ((op & 0xfe00) == 0x1a00) //sub
		DATA_SHIFT_IMM(true, NOCOND, ARM_SUB, 1, REG_3, REG_0, 0, 0, REG_6);

	else if ((op & 0xfe00) == 0x1c00) //add imm small
		DATA_IMM(true, NOCOND, ARM_ADD, 1, REG_3, REG_0, 0, IMM3_6);

	else if ((op & 0xfe00) == 0x1e00) //sub imm small
		DATA_IMM(true, NOCOND, ARM_SUB, 1, REG_3, REG_0, 0, IMM3_6);
	
	else if ((op & 0xf800) == 0x2000) //mov imm
		DATA_IMM(true, NOCOND, ARM_MOV, 1, 0, REG_8, 0, IMM8);
	
	else if ((op & 0xf800) == 0x2800) //cmp imm
		DATA_IMM(true, NOCOND, ARM_CMP, 1, REG_8, 0, 0, IMM8);

	else if ((op & 0xf800) == 0x3000) //add imm
		DATA_IMM(true, NOCOND, ARM_ADD, 1, REG_8, REG_8, 0, IMM8);

	else if ((op & 0xf800) == 0x3800) //sub imm
		DATA_IMM(true, NOCOND, ARM_SUB, 1, REG_8, REG_8, 0, IMM8);

	else if ((op & 0xffc0) == 0x4000) //and
		DATA_SHIFT_IMM(true, NOCOND, ARM_AND, 1, REG_0, REG_0, 0, 0, REG_3);

	else if ((op & 0xffc0) == 0x4040) //eor
		DATA_SHIFT_IMM(true, NOCOND, ARM_EOR, 1, REG_0, REG_0, 0, 0, REG_3);

	else if ((op & 0xffc0) == 0x4080) //lsl reg
		DATA_SHIFT_REG(true, NOCOND, ARM_MOV, 1, 0, REG_0, REG_3, ARM_LSL, REG_0);

	else if ((op & 0xffc0) == 0x40c0) //lsr reg
		DATA_SHIFT_REG(true, NOCOND, ARM_MOV, 1, 0, REG_0, REG_3, ARM_LSR, REG_0);

	else if ((op & 0xffc0) == 0x4100) //asr reg
		DATA_SHIFT_REG(true, NOCOND, ARM_MOV, 1, 0, REG_0, REG_3, ARM_ASR, REG_0);

	else if ((op & 0xffc0) == 0x4140) //adc
		DATA_SHIFT_IMM(true, NOCOND, ARM_ADC, 1, REG_0, REG_0, 0, 0, REG_3);

	else if ((op & 0xffc0) == 0x4180) //sbc
		DATA_SHIFT_IMM(true, NOCOND, ARM_SBC, 1, REG_0, REG_0, 0, 0, REG_3);

	else if ((op & 0xffc0) == 0x41c0) //ror
		DATA_SHIFT_REG(true, NOCOND, ARM_MOV, 1, 0, REG_0, REG_3, ARM_ROR, REG_0);
	
	else if ((op & 0xffc0) == 0x4200) //tst
		DATA_SHIFT_IMM(true, NOCOND, ARM_TST, 1, REG_0, 0, 0, 0, REG_3);
	
	else if ((op & 0xffc0) == 0x4240) //neg
		DATA_IMM(true, NOCOND, ARM_RSB, 1, REG_3, REG_0, 0, 0);
	
	else if ((op & 0xffc0) == 0x4280) //cmp
		DATA_SHIFT_IMM(true, NOCOND, ARM_CMP, 1, REG_0, 0, 0, 0, REG_3);
	
	else if ((op & 0xffc0) == 0x42c0) //cmn
		DATA_SHIFT_IMM(true, NOCOND, ARM_CMN, 1, REG_0, 0, 0, 0, REG_3);
	
	else if ((op & 0xffc0) == 0x4300) //orr
		DATA_SHIFT_IMM(true, NOCOND, ARM_ORR, 1, REG_0, REG_0, 0, 0, REG_3);
	
	else if ((op & 0xffc0) == 0x4340) //mul
		MULTIPLY_WORD(true, NOCOND, 0, 1, REG_0, 0, REG_0, REG_3);

	else if ((op & 0xffc0) == 0x4380) //bic
		DATA_SHIFT_IMM(true, NOCOND, ARM_BIC, 1, REG_0, REG_0, 0, 0, REG_3);
	
	else if ((op & 0xffc0) == 0x43c0) //mvn
		DATA_SHIFT_IMM(true, NOCOND, ARM_MVN, 1, 0, REG_0, 0, 0, REG_3);
	
	else if ((op & 0xff00) == 0x4400) //add highs
		DATA_SHIFT_IMM(true, NOCOND, ARM_ADD, 0, REGX_0, REGX_0, 0, 0, REGX_3);
	
	else if ((op & 0xff00) == 0x4500) //cmp highs
		DATA_SHIFT_IMM(true, NOCOND, ARM_CMP, 1, REGX_0, 0, 0, 0, REGX_3);
	
	else if ((op & 0xff00) == 0x4600) //mov highs
		DATA_SHIFT_IMM(true, NOCOND, ARM_MOV, 0, 0, REGX_0, 0, 0, REGX_3);
	
	else if ((op & 0xff87) == 0x4700) //bx reg
		BRANCH_CHANGE_REG(true, NOCOND, 0, REGX_3);

	else if ((op & 0xff87) == 0x4780) //blx reg
		BRANCH_CHANGE_REG(true, NOCOND, 1, REGX_3);

	else if ((op & 0xf800) == 0x4800) //ldr pc
		MEM_THUMB_PC(true, REG_8, IMM8);

	else if ((op & 0xfe00) == 0x5000) //str reg
		MEM_SHIFT(true, NOCOND, 1, 1, 0, 0, 0, REG_3, REG_0, 0, 0, REG_6);

	else if ((op & 0xfe00) == 0x5200) //strh reg
		MEM_MISC_REG(true, NOCOND, 1, 1, 0, 0, REG_3, REG_0, ARMMEM_H, REG_6);

	else if ((op & 0xfe00) == 0x5400) //strb reg
		MEM_SHIFT(true, NOCOND, 1, 1, 1, 0, 0, REG_3, REG_0, 0, 0, REG_6);

	else if ((op & 0xfe00) == 0x5600) //ldrsb reg
		MEM_MISC_REG(true, NOCOND, 1, 1, 0, 1, REG_3, REG_0, ARMMEM_SB, REG_6);

	else if ((op & 0xfe00) == 0x5800) //ldr reg
		MEM_SHIFT(true, NOCOND, 1, 1, 0, 0, 1, REG_3, REG_0, 0, 0, REG_6);

	else if ((op & 0xfe00) == 0x5a00) //ldrh reg
		MEM_MISC_REG(true, NOCOND, 1, 1, 0, 1, REG_3, REG_0, ARMMEM_H, REG_6);

	else if ((op & 0xfe00) == 0x5c00) //ldrb reg
		MEM_SHIFT(true, NOCOND, 1, 1, 1, 0, 1, REG_3, REG_0, 0, 0, REG_6);
	
	else if ((op & 0xfe00) == 0x5e00) //ldrsh reg
		MEM_MISC_REG(true, NOCOND, 1, 1, 0, 1, REG_3, REG_0, ARMMEM_SH, REG_6);

	else if ((op & 0xf800) == 0x6000) //str imm
		MEM_IMM(true, NOCOND, 1, 1, 0, 0, 0, REG_3, REG_0, IMM5_6*4);

	else if ((op & 0xf800) == 0x6800) //ldr imm
		MEM_IMM(true, NOCOND, 1, 1, 0, 0, 1, REG_3, REG_0, IMM5_6*4);

	else if ((op & 0xf800) == 0x7000) //strb imm
		MEM_IMM(true, NOCOND, 1, 1, 1, 0, 0, REG_3, REG_0, IMM5_6);

	else if ((op & 0xf800) == 0x7800) //ldrb imm
		MEM_IMM(true, NOCOND, 1, 1, 1, 0, 1, REG_3, REG_0, IMM5_6);

	else if ((op & 0xf800) == 0x8000) //strh imm
		MEM_MISC_IMM(true, NOCOND, 1, 1, 0, 0, REG_3, REG_0, ARMMEM_H, IMM5_6*2);

	else if ((op & 0xf800) == 0x8800) //ldrh imm
		MEM_MISC_IMM(true, NOCOND, 1, 1, 0, 1, REG_3, REG_0, ARMMEM_H, IMM5_6*2);

	else if ((op & 0xf800) == 0x9000) //str sp
		MEM_IMM(true, NOCOND, 1, 1, 0, 0, 0, REG_SP, REG_8, IMM8*4);

	else if ((op & 0xf800) == 0x9800) //ldr sp
		MEM_IMM(true, NOCOND, 1, 1, 0, 0, 1, REG_SP, REG_8, IMM8*4);

	else if ((op & 0xf800) == 0xa000) //add from pc
		DATA_THUMB_PC(true, REG_8, IMM8);

	else if ((op & 0xf800) == 0xa800) //add from sp
		DATA_IMM(true, NOCOND, ARM_ADD, 0, REG_SP, REG_8, 15, IMM8);

	else if ((op & 0xff80) == 0xb000) //add sp
		DATA_IMM(true, NOCOND, ARM_ADD, 0, REG_SP, REG_SP, 15, IMM7);

	else if ((op & 0xff80) == 0xb080) //sub sp
		DATA_IMM(true, NOCOND, ARM_SUB, 0, REG_SP, REG_SP, 15, IMM7);

	else if ((op & 0xffc0) == 0xb200) //sxth
		EXTEND(true, NOCOND, ARM_SXTH, 15, REG_0, 0, REG_3);
	
	else if ((op & 0xffc0) == 0xb240) //sxtb
		EXTEND(true, NOCOND, ARM_SXTB, 15, REG_0, 0, REG_3);
	
	else if ((op & 0xffc0) == 0xb280) //uxth
		EXTEND(true, NOCOND, ARM_UXTH, 15, REG_0, 0, REG_3);
	
	else if ((op & 0xffc0) == 0xb2c0) //uxtb
		EXTEND(true, NOCOND, ARM_UXTB, 15, REG_0, 0, REG_3);

	else if ((op & 0xff00) == 0xb400) //push 
		MEM_MULTIPLE(true, NOCOND, 1, 0, 0, 1, 0, REG_SP, IMM8);

	else if ((op & 0xff00) == 0xb500) //push lr
		MEM_MULTIPLE(true, NOCOND, 1, 0, 0, 1, 0, REG_SP, IMM8 | (1 << 14));

	else if ((op & 0xffc0) == 0xba00) //rev
		REV(true, NOCOND, 0, REG_0, 0, REG_3);
	
	else if ((op & 0xffc0) == 0xba40) //rev16
		REV(true, NOCOND, 0, REG_0, 1, REG_3);
	
	else if ((op & 0xffc0) == 0xbac0) //revsh
		REV(true, NOCOND, 1, REG_0, 1, REG_3);

	else if ((op & 0xff00) == 0xbc00) //pop
		MEM_MULTIPLE(true, NOCOND, 0, 1, 0, 1, 1, REG_SP, IMM8);

	else if ((op & 0xff00) == 0xbd00) //pop pc
		MEM_MULTIPLE(true, NOCOND, 0, 1, 0, 1, 1, REG_SP, IMM8 | (1 << 15));

	else if ((op & 0xff00) == 0xbe00) //bkpt
		BKPT(true, NOCOND, IMM8);

	else if ((op & 0xf800) == 0xc000) //stmia
		MEM_MULTIPLE(true, NOCOND, 0, 1, 0, 1, 0, REG_8, IMM8);

	else if ((op & 0xf800) == 0xc800 && (((1 << REG_8) & IMM8) == 0)) //ldmia
		MEM_MULTIPLE(true, NOCOND, 0, 1, 0, 1, 1, REG_8, IMM8);
	
	else if ((op & 0xf800) == 0xc800 && (((1 << REG_8) & IMM8) != 0)) //ldmia no wb
		MEM_MULTIPLE(true, NOCOND, 0, 1, 0, 0, 1, REG_8, IMM8);

	else if ((op & 0xf000) == 0xd000 && BITS(op, 8, 4) < 14) //b cond
		BRANCH_IMM(true, BITS(op, 8, 4), 0, SIGNEX(IMM8, 8));

	else if ((op & 0xff00) == 0xdf00) //swi
		SWI(true, NOCOND, IMM8);

	else if ((op & 0xf800) == 0xe000) //b uncond
		BRANCH_IMM(true, NOCOND, 0, SIGNEX(IMM11, 11));
	
	else if ((op & 0xf800) == 0xf000) //long bl
	{
		u16 opEX = ::Ram<u16>(cpu.nia); //nia will not be advanced here

		u32 eximm = (SIGNEX(IMM11, 11) << 11) | BITS(opEX, 0, 11);
		if ((opEX & 0xf800) == 0xf800) //BL
			BRANCH_L_THUMB(true, eximm);
		else if ((opEX & 0xf800) == 0xe800) //BLX
			BRANCH_LX_THUMB(true, eximm);
		else 
			Fail(op);
	}

	else 
		Fail(op);
}

#undef REG_0
#undef REG_3
#undef REG_6
#undef REG_8
#undef REGX_0
#undef REGX_3
#undef REG_SP
#undef IMM3_6
#undef IMM5_6
#undef IMM7
#undef IMM8
#undef IMM11

#define COND BITS(op, 28, 4)

#define REG_0 BITS(op, 0, 4)
#define REG_8 BITS(op, 8, 4)
#define REG_12 BITS(op, 12, 4)
#define REG_16 BITS(op, 16, 4)

#define FREG_0 ((BITS(op, 0, 4) << 1) | BIT(op, 5))
#define FREG_12 ((BITS(op, 12, 4) << 1) | BIT(op, 22))
#define FREG_16 ((BITS(op, 16, 4) << 1) | BIT(op, 7))
#define DBL (BIT(op, 8))
#define PQRS ((BIT(op, 23) << 3) | (BITS(op, 20, 2) << 1) | (BIT(op, 6)))

#define SHAMT BITS(op, 7, 5)
#define IMM8 BITS(op, 0, 8)
#define IMM12 BITS(op, 0, 12)
#define IMM24 BITS(op, 0, 24)
#define IMM8LH ((BITS(op, 8, 4) << 4) | BITS(op, 0, 4))

#define ROTIMM2 BITS(op, 10, 2)
#define ROTIMM4 BITS(op, 8, 4)

void DecodeArm(u32 op)
{	
	if (BITS(op, 24, 4) == 0 && BITS(op, 4, 4) == 9) //multiply
		MULTIPLY_WORD(false, COND, BITS(op, 21, 3), BIT(op, 20), REG_16, REG_12, REG_8, REG_0);

	else if (BITS(op, 23, 5) == 2 && BIT(op, 20) == 0 && (!BIT(op, 4) || !BIT(op, 7))) //misc
	{
		if (BIT(op, 21) == 1 && BITS(op, 4, 4) == 0) //move to flags reg
			TO_FLAGS_REG(false, COND, BIT(op, 22), BITS(op, 16, 4), REG_0);
		
		else if (BITS(op, 4, 4) == 1 && BITS(op, 20, 3) == 6) //clz
			CLZ(false, COND, REG_12, REG_0);
		
		else if (BIT(op, 21) == 1 && (BITS(op, 4, 4) == 1 || BITS(op, 4, 4) == 3)) //branch change
			BRANCH_CHANGE_REG(false, COND, BIT(op, 5), REG_0);
		
		else if (BIT(op, 21) == 0 && BITS(op, 0, 12) == 0) //move from flags
			FROM_FLAGS(false, COND, BIT(op, 22), REG_12);

		else if (BIT(op, 7) && !BIT(op, 4)) //short multiplies
			MULTIPLY_SHORT(false, COND, BITS(op, 21, 2), REG_16, REG_12, REG_8, BIT(op, 5), BIT(op, 6), REG_0);

		else if (BITS(op, 20, 3) == 2 && BITS(op, 4, 4) == 7)
			BKPT(false, COND, BITS(op, 0, 4) | (BITS(op, 8, 12) << 4));

		else
			Fail(op);
	}

	else if (BITS(op, 25, 3) == 0 && BIT(op, 4) == 1 && BIT(op, 7) == 1) //misc load/store
	{
		if (BITS(op, 5, 2) != 0 && BIT(op, 22)) //misc memory imm
			MEM_MISC_IMM(false, COND, BIT(op, 24), BIT(op, 23), BIT(op, 21), BIT(op, 20), 
						REG_16, REG_12, BITS(op, 5, 2), IMM8LH);
		
		else if (BITS(op, 5, 2) != 0 && !BIT(op, 22)) //misc memory reg
			MEM_MISC_REG(false, COND, BIT(op, 24), BIT(op, 23), BIT(op, 21), BIT(op, 20), 
						REG_16, REG_12, BITS(op, 5, 2), REG_0);

		else if (BITS(op, 5, 2) == 0 && BITS(op, 20, 5) == 0x19) //load exclusive
			MEM_LDREX(false, COND, REG_16, REG_12);
		
		else if (BITS(op, 5, 2) == 0 && BITS(op, 20, 5) == 0x18) //store exclusive
			MEM_STREX(false, COND, REG_16, REG_12, REG_0);

		else if (BITS(op, 5, 2) == 0 && BITS(op, 23, 2) == 2)
			MEM_SWP(false, COND, BIT(op, 22), REG_16, REG_12, REG_0);

		else
			Fail(op);
	}

	else if (BITS(op, 25, 3) == 5) //branch imm
	{
		if (COND == 15)
			BRANCH_LX_ARM(false, BIT(op, 24), IMM24); //branch exchange link immediate

		else
			BRANCH_IMM(false, COND, BIT(op, 24), IMM24); //branch (link) immediate
	}

	else if (BITS(op, 25, 3) == 3 && BIT(op, 4) == 1) //media
	{
		if (BITS(op, 23, 2) == 1 && BITS(op, 4, 4) == 7) //extend
			EXTEND(false, COND, BITS(op, 20, 3), REG_16, REG_12, ROTIMM2, REG_0);

		else if (BITS(op, 23, 2) == 1 && BITS(op, 20, 2) == 3 && BITS(op, 5, 2) == 1)
			REV(false, COND, BIT(op, 22), REG_12, BIT(op, 7), REG_0);

		else if (BITS(op, 23, 2) == 0) 
			PARALLEL(false, COND, BITS(op, 20, 3), REG_16, REG_12, BITS(op, 5, 3), REG_0);

		else if (BITS(op, 20, 5) == 8 && BITS(op, 4, 2) == 1)
			PACK(false, COND, REG_16, REG_12, SHAMT, BIT(op, 6), REG_0);

		else
			Fail(op);
	}

	else if (BITS(op, 23, 5) == 6 && BITS(op, 20, 2) == 2) //move to flags imm
		TO_FLAGS_IMM(false, COND, BIT(op, 22), BITS(op, 16, 4), ROTIMM4, IMM8);

	else if (BITS(op, 26, 2) == 0) //data
	{
		if (BIT(op, 25) == 0 && BIT(op, 4) == 0) 
			DATA_SHIFT_IMM(false, COND, BITS(op, 21, 4), BIT(op, 20), REG_16, REG_12, SHAMT, 
							BITS(op, 5, 2), REG_0);
		
		else if (BIT(op, 25) == 0 && BIT(op, 4) == 1) 
			DATA_SHIFT_REG(false, COND, BITS(op, 21, 4), BIT(op, 20), REG_16, REG_12, REG_8, 
							BITS(op, 5, 2), REG_0);
		
		else if (BIT(op, 25) == 1) 
			DATA_IMM(false, COND, BITS(op, 21, 4), BIT(op, 20), REG_16, REG_12, ROTIMM4, IMM8);

		else
			Fail(op);
	}

	else if (BITS(op, 26, 2) == 1) //load/store
	{
		if (BIT(op, 25) == 1) 
			MEM_SHIFT(false, COND, BIT(op, 24), BIT(op, 23), BIT(op, 22), BIT(op, 21), BIT(op, 20), 
					REG_16, REG_12, SHAMT, BITS(op, 5, 2), REG_0);
		
		else if (BIT(op, 25) == 0) 
			MEM_IMM(false, COND, BIT(op, 24), BIT(op, 23), BIT(op, 22), BIT(op, 21), BIT(op, 20), 
					REG_16, REG_12, IMM12);

		else
			Fail(op);
	}

	else if (BITS(op, 25, 3) == 4) //multiple
		MEM_MULTIPLE(false, COND, BIT(op, 24), BIT(op, 23), BIT(op, 22), BIT(op, 21), BIT(op, 20), REG_16, BITS(op, 0, 16));

	else if (BITS(op, 24, 4) == 15) //swi
		SWI(false, COND, BITS(op, 0, 24));

	else if (BITS(op, 25, 3) == 6 && BITS(op, 9, 3) == 5) //vfp load/store/dmove
	{
		if (BIT(op, 24) == 1 && BIT(op, 21) == 0)
			VFP_MEM(false, COND, BIT(op, 23), BIT(op, 20), REG_16, FREG_12, DBL, IMM8);

		else if (BIT(op, 24) == 0 && BIT(op, 23) == 0)
			VFP_DMOVE(false, COND, BIT(op, 20), REG_16, REG_12, DBL, FREG_0);

		else
			VFP_MEM_MULTIPLE(false, COND, BIT(op, 21), BIT(op, 23), BIT(op, 20), REG_16, FREG_12, DBL, IMM8);
	}

	else if (BITS(op, 25, 3) == 7 && BITS(op, 9, 3) == 5) //vfp move/op
	{
		if (BIT(op, 4))
			VFP_MOVE(false, COND, BITS(op, 21, 3), BIT(op, 20), FREG_16, REG_12, DBL);

		else if (PQRS == 15)
		{
			if (FREG_16 >= 8)
				VFP_2FOP(false, COND, FREG_16, FREG_12, DBL, FREG_0);

			else
				VFP_2VOP(false, COND, FREG_16, FREG_12, DBL, FREG_0);
		}
		else if (PQRS >= 4)
			VFP_3VOP(false, COND, PQRS, FREG_16, FREG_12, DBL, FREG_0);

		else
			VFP_ACC3VOP(false, COND, PQRS, FREG_16, FREG_12, DBL, FREG_0);
	}

	else if (BITS(op, 24, 4) == 14 && BITS(op, 8, 4) == 15) //cop15 move/op
	{
		if (BIT(op, 4) && BIT(op, 20))
			FROM_COP15(false, COND, BITS(op, 21, 3), REG_16, REG_12, BITS(op, 5, 3), REG_0);
		
		else
			Fail(op);
	}

	else
		Fail(op);
}

void ExecuteOne()
{
	if (cpu.thumb)
	{
		u16 op = ::Ram<u16>( cpu.nia );
		cpu.nia += 2;
		DecodeThumb( op );
	}
	else
	{
		u32 op = ::Ram<u32>( cpu.nia );
		cpu.nia += 4;
		DecodeArm( op );
	}
}

}



