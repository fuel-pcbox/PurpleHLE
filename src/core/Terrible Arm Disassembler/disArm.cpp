
//#include "stdafx.h"
#include "dis.h"

using namespace std;

//defines for the stream:
#define COND cond(BITS(op, 28, 4))

#define REG_0 reg(BITS(op, 0, 4))
#define REG_8 reg(BITS(op, 8, 4))
#define REG_12 reg(BITS(op, 12, 4))
#define REG_16 reg(BITS(op, 16, 4))

#define SHAMT BITS(op, 7, 5)
#define IMM8 BITS(op, 0, 8)
#define IMM12 BITS(op, 0, 12)
#define IMMHL8 (BITS(op, 0, 4) | (BITS(op, 8, 4) << 4))
#define IMM24 BITS(op, 0, 24)

#define REALIMM24 (SIGNEX(IMM24, 24) * 4 + address + 8)

#define ROTIMMBY8 (BITS(op, 10, 2) * 8)
#define ROTIMMBY2 (BITS(op, 8, 4) * 2)

#define BIT(n) BITS(op, (n), 1)

#define AND << ", " <<

string reg(u32 reg)
{
	stringstream ret;
	if (reg == 15)
		ret << "PC";
	else if (reg == 14)
		ret << "LR";
	else if (reg == 13)
		ret << "SP";
	else if (reg == 12)
		ret << "IP";
	else if (reg >= 4)
		ret << "V" << (reg - 3);
	else
		ret << "A" << (reg + 1);
	return ret.str();
}

char *cond(u32 cond)
{
	switch (cond)
	{
	case 0: return "EQ";
	case 1: return "NE";
	case 2: return "HS";
	case 3: return "LO";
	case 4: return "MI";
	case 5: return "PL";
	case 6: return "VS";
	case 7: return "VC";
	case 8: return "HI";
	case 9: return "LS";
	case 10: return "GE";
	case 11: return "LT";
	case 12: return "GT";
	case 13: return "LE";
	case 14: return "";
	default: return "UNKNOWN";
	}
}

string reglistop(u32 regs)
{
	stringstream ret;
	ret << "{";
	bool comma = false;
	for (int i = 0; i < 16; i++)
	{
		if (regs & (1 << i))
		{
			if (comma)
				ret << ',';
			ret << reg(i);
			comma = true;
		}
	}
	ret << "}";
	return ret.str();
}

string immrot(u32 op)
{
	stringstream ret;
	ret << hex << (_rotr(IMM8, ROTIMMBY2));
	return ret.str();	
}

string shiftop(u32 op)
{
	stringstream ret;
	ret << hex;
	if (!BIT(4)) //imm
	{
		if (BITS(op, 5, 7) == 0)
		{
			ret << REG_0;
			return ret.str();
		}
		int shamt = SHAMT;
		switch (BITS(op, 5, 2))
		{
		case 0: ret << "LSL "; break;
		case 1: ret << "LSR "; shamt = shamt ? shamt : 32; break;
		case 2: ret << "ASR "; shamt = shamt ? shamt : 32; break;
		case 3: if (SHAMT) {ret << "ROR ";} else {ret << "RRX "; shamt = 1;} break;
		}
		ret << REG_0 AND shamt;
	}
	else
	{
		switch (BITS(op, 5, 2))
		{
		case 0: ret << "LSL "; break;
		case 1: ret << "LSR "; break;
		case 2: ret << "ASR "; break;
		case 3: ret << "ROR "; break;
		}
		ret << REG_0 AND REG_8;
	}
	return ret.str();
}

string addrmode(u32 op, string rm)
{
	stringstream ret;
	ret << hex;

	char sign;
	if (BIT(23))
		sign = '+';
	else 
		sign = '-';

	ret << "[" << REG_16;
	
	if ((BIT(21) && BIT(24)) || !BIT(24))
		ret << "="; //mine
	
	if (BIT(24))
		ret AND sign << rm << "]";
	else
		ret << "]" AND sign << rm;

	return ret.str();
}

string copop(u32 op)
{
	stringstream ret;
	ret << hex;
	int cpn = BITS(op, 8, 4);
	int crn = BITS(op, 16, 4);
	int crd = BITS(op, 12, 4);
	int crm = BITS(op, 0, 4);
	int op2 = BITS(op, 5, 3);
	int op1f = BITS(op, 20, 4);
	int op1 = BITS(op, 21, 3);

	if (BIT(4) == 0)
		ret << "CDP " << cpn AND op1f AND crd AND crn AND crm AND op2;
	else if (BIT(20) == 0)
		ret << "MCR " << cpn AND op1 AND REG_12 AND crn AND crm AND op2;
	else
		ret << "MRC " << cpn AND op1 AND REG_12 AND crn AND crm AND op2;

	return ret.str();
}

string memop(u32 op)
{
	stringstream ret;
	ret << hex;

	if (BIT(20))
		ret << "LDR";
	else
		ret << "STR";
	if (BIT(22))
		ret << "B";

	ret << COND << " " << REG_12 << ", ";

	stringstream rm;
	if (BIT(25))
		rm << shiftop(op);
	else
		rm << hex << IMM12;

	ret << addrmode(op, rm.str());

	return ret.str();
}

string msrop(u32 op)
{
	stringstream ret;
	ret << hex;

	string sr;
	if (BIT(22) == 0)
		sr = "CPSR";
	else
		sr = "SPSR";

	if (BIT(21) == 0) 
		ret << "MRS " << REG_12 AND sr;
	else
	{
		ret << "MSR " << sr << "_" << BITS(op, 16, 4) AND "";
		if (BIT(25) == 0)
			ret << REG_0;
		else
			ret << immrot(op);
	}
	return ret.str();
}

string memmiscop(u32 op)
{
	stringstream ret;
	ret << hex;

	switch (BITS(op, 5, 2) | (BIT(20)<<2))
	{
	case 1: ret << "STRH"; break;
	case 2: ret << "LDRD"; break;
	case 3: ret << "STRD"; break;
	case 5: ret << "LDRH"; break;
	case 6: ret << "LDRSB"; break;
	case 7: ret << "LDRSH"; break;
	}

	ret << COND << " " << REG_12 << ", ";

	stringstream rm;
	if (BIT(22))
		rm << hex << IMMHL8;
	else
		rm << REG_0;

	ret << addrmode(op, rm.str());

	return ret.str();
}

string mmop(u32 op)
{
	stringstream ret;
	ret << hex;

	if (BIT(20))
		ret << "LDM";
	else
		ret << "STM";

	switch (BITS(op, 23, 2))
	{
	case 0: ret << "DA"; break;
	case 1: ret << "IA"; break;
	case 2: ret << "DB"; break;
	case 3: ret << "IB"; break;
	}

	ret << COND << " " << REG_16;

	if (BIT(21))
		ret << "=";

	ret AND reglistop(op & 0xffff);

	return ret.str();
}

string bop(u32 op, u32 address)
{
	stringstream ret;
	ret << hex;
	ret << "B";

	bool blx = (BITS(op, 28, 4) == 15);

	if (blx || BIT(24))
		ret << "L";
	if (blx)
		ret << "X ";
	else
		ret << COND << " ";

	int imm24 = REALIMM24;
	if (blx)
		imm24 += (BIT(24)*2);
	ret << hex << imm24;
	return ret.str();
}

string brop(u32 op)
{
	stringstream ret;
	ret << hex;
	ret << "B";

	if (BIT(5))
		ret << "L";

	ret << "X" << COND << " " << REG_0;

	return ret.str();
}

string mulop(u32 op)
{
	stringstream ret;
	ret << hex;

	u32 mul = BITS(op, 21, 4);
	switch (mul)
	{
	case 0: ret << "MUL"; break;
	case 1: ret << "MLA"; break;
	case 4: ret << "UMULL"; break;
	case 5: ret << "UMLAL"; break;
	case 6: ret << "SMULL"; break;
	case 7: ret << "SMLAL"; break;
	default: ret << "UNKNOWN MUL"; break;
	}
	if (BIT(20))
		ret << "S";

	ret << COND << " ";
	if (mul == 0)
		ret << REG_16 AND REG_0 AND REG_8;
	else if (mul == 1)
		ret << REG_16 AND REG_0 AND REG_8 AND REG_12;
	else
		ret << REG_12 AND REG_16 AND REG_0 AND REG_8;

	return ret.str();
}

string extop(u32 op)
{
	stringstream ret;
	ret << hex;

	switch (BITS(op, 20, 3))
	{
	case 2: ret << "SXTB"; break;
	case 3: ret << "SXTH"; break;
	case 6: ret << "UXTB"; break;
	case 7: ret << "UXTH"; break;
	default: ret << "UNKNOWNNN"; break;
	}

	if (BITS(op, 16, 4) != 15)
		ret << "UNKWNON(A)";

	ret << COND << " ";
	
	if (op & 0xc000)
		ret << REG_12 AND shiftop( op & ~0x10 );
	else
		ret << REG_12 AND REG_0;

	return ret.str();
}

string dataop(u32 op)
{
	stringstream ret;
	ret << hex;
	bool no_d = false, no_n = false;
	switch (BITS(op, 21, 4))
	{
	case 0: ret << "AND"; break;
	case 1: ret << "EOR"; break;
	case 2: ret << "SUB"; break;
	case 3: ret << "RSB"; break;
	case 4: ret << "ADD"; break;
	case 5: ret << "ADC"; break;
	case 6: ret << "SBC"; break;
	case 7: ret << "RSC"; break;
	case 8: ret << "TST"; no_d = true; break;
	case 9: ret << "TEQ"; no_d = true; break;
	case 10: ret << "CMP"; no_d = true; break;
	case 11: ret << "CMN"; no_d = true; break;
	case 12: ret << "ORR"; break;
	case 13: ret << "MOV"; no_n = true; break;
	case 14: ret << "BIC"; break;
	case 15: ret << "MVN"; no_n = true; break;
	}
	ret << COND;
	if (BIT(20))
		ret << "S";
	ret << " ";
	if (!no_d)
		ret << REG_12 << ", ";
	if (!no_n)
		ret << REG_16 << ", ";
	if (BIT(25))
		ret << immrot(op);
	else
		ret << shiftop(op);
	return ret.str();
}

	
void ARMdisassemble(stringstream &ss, u32 address, u32 op)
{
	if (BITS(op, 25, 3) == 3 && BITS(op, 4, 1) == 1) //media
	{
		if (BITS(op, 23, 2) == 1 && BITS(op, 5, 3) == 3) //extend
			ss << extop(op);

		else
			ss << "UNKNOWN MEDIA";
	}
	
	else if (BITS(op, 24, 4) == 0 && BITS(op, 4, 4) == 9) //multiply
		ss << mulop(op);

	else if (BITS(op, 26, 2) == 0 && BITS(op, 23, 2) == 2 && BIT(20) == 0) //misc
	{
		if (BIT(25) == 0 && BITS(op, 21, 2) == 1 && BITS(op, 4, 4) != 0 && BITS(op, 4, 4) < 4) //branch
			ss << brop(op);

		else if (BIT(25) == 1 || BITS(op, 4, 4) == 0)
			ss << msrop(op);

		else if (BITS(op, 20, 8) == 0x12 && BITS(op, 4, 4) == 7)
			ss << "BKPT";

		else
			ss << "UNKNOWN MISC";
	}

	else if (BITS(op, 25, 3) == 0 && BIT(4) == 1 && BIT(7) == 1) //misc load/store
	{
		if (BITS(op, 5, 2) != 0 && BIT(22)) //misc memory imm
			ss << memmiscop(op);
		
		else if (BITS(op, 5, 2) != 0 && !BIT(22)) //misc memory reg
			ss << memmiscop(op);

		else
			ss << "UNKNOWN MISC L/S";
	}

	else if (BITS(op, 25, 3) == 5) //branch imm
		ss << bop(op, address);

	else if (BITS(op, 26, 2) == 0) //data
	{
		if (BIT(25) == 0 && BIT(4) == 0) 
			ss << dataop(op);
		
		else if (BIT(25) == 0 && BIT(4) == 1) 
			ss << dataop(op);
		
		else if (BIT(25) == 1) 
			ss << dataop(op);
	}

	else if (BITS(op, 26, 2) == 1) //load/store
	{
		if (BIT(25) == 1) 
			ss << memop(op);
		
		else if (BIT(25) == 0) 
			ss << memop(op);
	}

	else if (BITS(op, 25, 3) == 4) //multiple
		ss << mmop(op);

	else if (BITS(op, 24, 4) == 0xe) //coprocessor
	{
		if (BITS(op, 8, 4) == 0xf)
			ss << copop(op);
		else
			ss << "UNKNOWN COP " << hex << op;
	}

	else if (BITS(op, 24, 4) == 0xf) //swi
		ss << "SWI " << hex << BITS(op, 0, 24);

	else
		ss << "UNKNOWN OTHER " << hex << op;
}

wxString ArmDisasm(u32 code, u32 address, u8 thumb)
{
	stringstream ss;
	if (!thumb)
		ARMdisassemble(ss, address, code);
	else
		THUMBdisassemble(ss, address, code);
	return wxString::FromAscii(ss.str().c_str());
}