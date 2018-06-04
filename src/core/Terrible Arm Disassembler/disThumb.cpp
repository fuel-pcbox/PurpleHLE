
//#include "stdafx.h"
#include "dis.h"

using namespace std;

//defines for the stream:

#define BIT0 BITS(code, 0, 1)
#define BIT1 BITS(code, 1, 1)
#define BIT2 BITS(code, 2, 1)

#define REG_0 reg(BITS(code, 0, 3))
#define REG_3 reg(BITS(code, 3, 3))
#define REG_6 reg(BITS(code, 6, 3))
#define REG_8 reg(BITS(code, 8, 3))

#define REGX_0 reg((BITS(code, 0, 3) | (BITS(code, 7, 1) << 3)))
#define REGX_3 reg(BITS(code, 3, 4))

#define IMM3_6 BITS(code, 6, 3)
#define IMM5_6 BITS(code, 6, 5)
#define IMM7 BITS(code, 0, 7)
#define IMM8 BITS(code, 0, 8)
#define IMM11 BITS(code, 0, 11)

#define SIMM8 ((s32)(s8)IMM8)
#define SIMM11 (SIGNEX(IMM11, 11))

#define SHIFTIMM (IMM5_6 ? IMM5_6 : 32)
#define BRANCHIMM8 (address + 4 + SIMM8*2)
#define BRANCHIMM11 (address + 4 + SIMM11*2)

#define AND << ", " <<

string reg(u32 reg);

string reglist(u32 regs)
{
	stringstream ret;
	ret << "{";
	bool comma = false;
	for (int i = 0; i < 8; i++)
	{
		if (regs & (1 << i))
		{
			if (comma)
				ret << ',';
			ret << reg(i);
			comma = true;
		}
	}
	return ret.str();
}
	
void THUMBdisassemble(stringstream &ss, u32 address, u32 op32)
{
	u16 code = (u16)op32;
	ss << hex;

	if ((code & 0xf800) == 0x0000)
		ss << "LSL " << REG_0 AND REG_3 AND IMM5_6;

	else if ((code & 0xf800) == 0x0800)
		ss << "LSR " << REG_0 AND REG_3 AND SHIFTIMM;

	else if ((code & 0xf800) == 0x1000)
		ss << "ASR " << REG_0 AND REG_3 AND SHIFTIMM;

	else if ((code & 0xfe00) == 0x1800)
		ss << "ADD " << REG_0 AND REG_3 AND REG_6;

	else if ((code & 0xfe00) == 0x1a00)
		ss << "SUB " << REG_0 AND REG_3 AND REG_6;

	else if ((code & 0xfe00) == 0x1c00)
		ss << "ADD " << REG_0 AND REG_3 AND IMM3_6;

	else if ((code & 0xfe00) == 0x1e00)
		ss << "SUB " << REG_0 AND REG_3 AND IMM3_6;
	
	else if ((code & 0xf800) == 0x2000)
		ss << "MOV " << REG_8 AND IMM8;
	
	else if ((code & 0xf800) == 0x2800)
		ss << "CMP " << REG_8 AND IMM8;

	else if ((code & 0xf800) == 0x3000)
		ss << "ADD " << REG_8 AND IMM8;

	else if ((code & 0xf800) == 0x3800)
		ss << "SUB " << REG_8 AND IMM8;

	else if ((code & 0xffc0) == 0x4000)
		ss << "AND " << REG_0 AND REG_3;

	else if ((code & 0xffc0) == 0x4040)
		ss << "EOR " << REG_0 AND REG_3;

	else if ((code & 0xffc0) == 0x4080)
		ss << "LSL " << REG_0 AND REG_3;

	else if ((code & 0xffc0) == 0x40c0)
		ss << "LSR " << REG_0 AND REG_3;

	else if ((code & 0xffc0) == 0x4100)
		ss << "ASR " << REG_0 AND REG_3;

	else if ((code & 0xffc0) == 0x4140) 
		ss << "ADC " << REG_0 AND REG_3;

	else if ((code & 0xffc0) == 0x4180) 
		ss << "SBC " << REG_0 AND REG_3;

	else if ((code & 0xffc0) == 0x41c0) 
		ss << "ROR " << REG_0 AND REG_3;
	
	else if ((code & 0xffc0) == 0x4200)
		ss << "TST " << REG_0 AND REG_3;
	
	else if ((code & 0xffc0) == 0x4240)
		ss << "NEG " << REG_0 AND REG_3;
	
	else if ((code & 0xffc0) == 0x4280)
		ss << "CMP " << REG_0 AND REG_3;
	
	else if ((code & 0xffc0) == 0x42c0)
		ss << "CMN " << REG_0 AND REG_3;
	
	else if ((code & 0xffc0) == 0x4300)
		ss << "ORR " << REG_0 AND REG_3;
	
	else if ((code & 0xffc0) == 0x4340)
		ss << "MUL " << REG_0 AND REG_3;

	else if ((code & 0xffc0) == 0x4380)
		ss << "BIC " << REG_0 AND REG_3;
	
	else if ((code & 0xffc0) == 0x43c0)
		ss << "MVN " << REG_0 AND REG_3;
	
	else if ((code & 0xff00) == 0x4400)
		ss << "ADD " << REGX_0 AND REGX_3;
	
	else if ((code & 0xff00) == 0x4500)
		ss << "CMP " << REGX_0 AND REGX_3;
	
	else if ((code & 0xff00) == 0x4600)
		ss << "CPY " << REGX_0 AND REGX_3;
	
	else if ((code & 0xff87) == 0x4700)
		ss << "BX " << REGX_3;

	else if ((code & 0xff87) == 0x4780)
		ss << "BLX " << REGX_3;

	else if ((code & 0xf800) == 0x4800)
		ss << "LDR " << REG_8 AND "[" << "PC" AND IMM8*4 << "]";

	else if ((code & 0xfe00) == 0x5000)
		ss << "STR " << REG_0 AND "[" << REG_3 AND REG_6 << "]";

	else if ((code & 0xfe00) == 0x5200)
		ss << "STRH " << REG_0 AND "[" << REG_3 AND REG_6 << "]";

	else if ((code & 0xfe00) == 0x5400)
		ss << "STRB " << REG_0 AND "[" << REG_3 AND REG_6 << "]";

	else if ((code & 0xfe00) == 0x5600)
		ss << "LDRSB " << REG_0 AND "[" << REG_3 AND REG_6 << "]";

	else if ((code & 0xfe00) == 0x5800)
		ss << "LDR " << REG_0 AND "[" << REG_3 AND REG_6 << "]";

	else if ((code & 0xfe00) == 0x5a00)
		ss << "LDRH " << REG_0 AND "[" << REG_3 AND REG_6 << "]";

	else if ((code & 0xfe00) == 0x5c00)
		ss << "LDRB " << REG_0 AND "[" << REG_3 AND REG_6 << "]";
	
	else if ((code & 0xfe00) == 0x5e00)
		ss << "LDRSH " << REG_0 AND "[" << REG_3 AND REG_6 << "]";

	else if ((code & 0xf800) == 0x6000)
		ss << "STR " << REG_0 AND "[" << REG_3 AND IMM5_6*4 << "]";

	else if ((code & 0xf800) == 0x6800)
		ss << "LDR " << REG_0 AND "[" << REG_3 AND IMM5_6*4 << "]";

	else if ((code & 0xf800) == 0x7000)
		ss << "STRB " << REG_0 AND "[" << REG_3 AND IMM5_6 << "]";

	else if ((code & 0xf800) == 0x7800)
		ss << "LDRB " << REG_0 AND "[" << REG_3 AND IMM5_6 << "]";

	else if ((code & 0xf800) == 0x8000)
		ss << "STRH " << REG_0 AND "[" << REG_3 AND IMM5_6*2 << "]";

	else if ((code & 0xf800) == 0x8800)
		ss << "LDRH " << REG_0 AND "[" << REG_3 AND IMM5_6*2 << "]";

	else if ((code & 0xf800) == 0x9000)
		ss << "STR " << REG_8 AND "[" << "SP" AND IMM8*4 << "]";

	else if ((code & 0xf800) == 0x9800)
		ss << "LDR " << REG_8 AND "[" << "SP" AND IMM8*4 << "]";

	else if ((code & 0xf800) == 0xa000)
		ss << "ADD " << REG_8 AND "PC" AND IMM8*4;

	else if ((code & 0xf800) == 0xa800)
		ss << "ADD " << REG_8 AND "SP" AND IMM8*4;

	else if ((code & 0xff80) == 0xb000)
		ss << "ADD " << "SP" AND IMM7*4;

	else if ((code & 0xff80) == 0xb080)
		ss << "SUB " << "SP" AND IMM7*4;

	else if ((code & 0xffc0) == 0xb200)
		ss << "SXTH " << REG_0 AND REG_3;

	else if ((code & 0xffc0) == 0xb240)
		ss << "SXTB " << REG_0 AND REG_3;

	else if ((code & 0xffc0) == 0xb280)
		ss << "UXTH " << REG_0 AND REG_3;

	else if ((code & 0xffc0) == 0xb2c0)
		ss << "UXTB " << REG_0 AND REG_3;

	else if ((code & 0xff00) == 0xb400)
		ss << "PUSH " << reglist(IMM8) << "}";

	else if ((code & 0xff00) == 0xb500)
		ss << "PUSH " << reglist(IMM8) << ",LR}";

	else if ((code & 0xffff) == 0xb650)
		ss << "SETEND LE";

	else if ((code & 0xffff) == 0xb658)
		ss << "SETEND BE";

	else if ((code & 0xfff8) == 0xb660)
		ss << "CPSIE " << (BIT2 ? 'a' : '\0') <<  (BIT1 ? 'i' : '\0') << (BIT0 ? 'f' : '\0');

	else if ((code & 0xfff8) == 0xb670)
		ss << "CPSID " << (BIT2 ? 'a' : '\0') <<  (BIT1 ? 'i' : '\0') << (BIT0 ? 'f' : '\0');

	else if ((code & 0xffc0) == 0xba00)
		ss << "REV " << REG_0 AND REG_3;

	else if ((code & 0xffc0) == 0xba40)
		ss << "REV16 " << REG_0 AND REG_3;

	else if ((code & 0xffc0) == 0xbac0)
		ss << "REVSH " << REG_0 AND REG_3;

	else if ((code & 0xff00) == 0xbc00)
		ss << "POP " << reglist(IMM8) << "}";

	else if ((code & 0xff00) == 0xbd00)
		ss << "POP " << reglist(IMM8) << ",PC}";

	else if ((code & 0xff00) == 0xbe00)
		ss << "BKPT " << IMM8;

	else if ((code & 0xf800) == 0xc000)
		ss << "STMIA " << REG_8 << "!" AND reglist(IMM8) << "}";

	else if ((code & 0xf800) == 0xc800)
		ss << "LDMIA " << REG_8 << "!" AND reglist(IMM8) << "}";

	else if ((code & 0xff00) == 0xd000)
		ss << "BEQ " << BRANCHIMM8;

	else if ((code & 0xff00) == 0xd100)
		ss << "BNE " << BRANCHIMM8;
	
	else if ((code & 0xff00) == 0xd200)
		ss << "BHS " << BRANCHIMM8;

	else if ((code & 0xff00) == 0xd300)
		ss << "BLO " << BRANCHIMM8;

	else if ((code & 0xff00) == 0xd400)
		ss << "BMI " << BRANCHIMM8;

	else if ((code & 0xff00) == 0xd500)
		ss << "BPL " << BRANCHIMM8;
	
	else if ((code & 0xff00) == 0xd600)
		ss << "BVS" << BRANCHIMM8;

	else if ((code & 0xff00) == 0xd700)
		ss << "BVC " << BRANCHIMM8;

	else if ((code & 0xff00) == 0xd800)
		ss << "BHI " << BRANCHIMM8;

	else if ((code & 0xff00) == 0xd900)
		ss << "BLS " << BRANCHIMM8;
	
	else if ((code & 0xff00) == 0xda00)
		ss << "BGE " << BRANCHIMM8;

	else if ((code & 0xff00) == 0xdb00)
		ss << "BLT " << BRANCHIMM8;

	else if ((code & 0xff00) == 0xdc00)
		ss << "BGT " << BRANCHIMM8;

	else if ((code & 0xff00) == 0xdd00)
		ss << "BLE " << BRANCHIMM8;

	else if ((code & 0xff00) == 0xdf00)
		ss << "SWI " << IMM8;
	
	else if ((code & 0xf800) == 0xe000)
		ss << "B " << BRANCHIMM11;
	
	else if ((code & 0xf800) == 0xe800)
		ss << "";
	
	else if ((code & 0xf800) == 0xf000)
	{
		u32 realaddr = address + 4 + (SIMM11 << 12) + (BITS(op32 >> 16, 0, 11) << 1);
		if (((op32 >> 16) & 0xf800) == 0xe800)
			ss << "BLX " << (realaddr & ~0x2);
		else if (((op32 >> 16) & 0xf800) == 0xf800)
			ss << "BL " << realaddr;
		else
			ss << "UNKNOWN";
	}
	
	else if ((code & 0xf800) == 0xf800)
		ss << "";

	else
		ss << "UNKNOWN";
}
