#ifdef _WIN32
#include "Math.h"
#else
#include <cmath>
#endif

#include "Instruction.h"
#include <limits.h>

namespace Interpreter
{

//helpers for convert
double round(double d) 
{ 
	u32 round = BITS(cpu.fpmode, 22, 2);
	if (round == 0) //nearest
		return (d >= 0) ? (d + 0.5) : (d - 0.5);
	else if (round == 1) //up
		return d + 0.5;
	else if (round == 2) //down
		return d - 0.5;
	else //zero
		return d;
}

double clampU(double d) { if (d != d) return 0; 
						  else if (d >= UINT_MAX) return UINT_MAX;
						  else if (d <= 0) return 0;
						  else return d; }
double clampS(double d) { if (d != d) return 0; 
						  else if (d >= INT_MAX) return INT_MAX;
						  else if (d <= INT_MIN) return INT_MIN;
						  else return d; }

	//functions
double fmac(double d, double n, double m) { return d + (n * m); }
double fnmac(double d, double n, double m) { return d - (n * m); }
double fmsc(double d, double n, double m) { return -d + (n * m); }
double fnmsc(double d, double n, double m) { return -d - (n * m); }
typedef double (*vop3accfunc)(double, double, double);
const vop3accfunc vop3accfuncArr[] = {&fmac, &fnmac, &fmsc, &fnmsc};

double fmul(double n, double m) { return n * m; }
double fnmul(double n, double m) { return -(n * m); }
double fadd(double n, double m) { return n + m; }
double fsub(double n, double m) { return n - m; }
double fdiv(double n, double m) { return n / m; }
typedef double (*vop3func)(double, double);
const vop3func vop3funcArr[] = {&fmul, &fnmul, &fadd, &fsub, &fdiv};

double fcpy(double m) { return m; } //really logical
double fabs(double m) { return abs(m); } //really logical 
double fneg(double m) { return -m; } //really logical
double fsqrt(double m) { return sqrt(m); }
typedef double (*vop2func)(double);
const vop2func vop2funcArr[] = {&fcpy, &fabs, &fneg, &fsqrt};

#define VINC(val) ((val) = ((val) & 0x18) | (((val) & 0x7) + (cpu.vstride ? 2 : 1)))

INLINE void VFP_ACC3VOP(bool thumb, u32 cond, u32 opcode, u32 freg_n, u32 freg_d, u32 dbl, u32 freg_m)
{
	if (!COND(cond))
		return;

	Assert(opcode < 4);

	vop3accfunc func = vop3accfuncArr[opcode];

	if (cpu.vlen == 0 || freg_d < 8)
	{
		double f_d = FPR_F(freg_d, dbl);
		double f_n = FPR_F(freg_n, dbl);
		double f_m = FPR_F(freg_m, dbl);
		double res = func(f_d, f_n, f_m);
		setFPR_F(freg_d, res, dbl);
	}
	else if (freg_m < 8)
	{
		double f_m = FPR_F(freg_m, dbl);
		for (int i = 0; i <= cpu.vlen; i++, VINC(freg_d), VINC(freg_n))
		{
			double f_d = FPR_F(freg_d, dbl);
			double f_n = FPR_F(freg_n, dbl);
			double res = func(f_d, f_n, f_m);
			setFPR_F(freg_d, res, dbl);
		}
	}
	else
	{
		for (int i = 0; i <= cpu.vlen; i++, VINC(freg_d), VINC(freg_m), VINC(freg_n))
		{
			double f_d = FPR_F(freg_d, dbl);
			double f_n = FPR_F(freg_n, dbl);
			double f_m = FPR_F(freg_m, dbl);
			double res = func(f_d, f_n, f_m);
			setFPR_F(freg_d, res, dbl);
		}
	}
}

INLINE void VFP_3VOP(bool thumb, u32 cond, u32 opcode, u32 freg_n, u32 freg_d, u32 dbl, u32 freg_m)
{
	if (!COND(cond))
		return;

	Assert(opcode >= 4 && opcode < 9);

	vop3func func = vop3funcArr[opcode - 4];

	if (cpu.vlen == 0 || freg_d < 8)
	{
		double f_n = FPR_F(freg_n, dbl);
		double f_m = FPR_F(freg_m, dbl);
		double res = func(f_n, f_m);
		setFPR_F(freg_d, res, dbl);
	}
	else if (freg_m < 8)
	{
		double f_m = FPR_F(freg_m, dbl);
		for (int i = 0; i <= cpu.vlen; i++, VINC(freg_d), VINC(freg_n))
		{
			double f_n = FPR_F(freg_n, dbl);
			double res = func(f_n, f_m);
			setFPR_F(freg_d, res, dbl);
		}
	}
	else
	{
		for (int i = 0; i <= cpu.vlen; i++, VINC(freg_d), VINC(freg_m), VINC(freg_n))
		{
			double f_n = FPR_F(freg_n, dbl);
			double f_m = FPR_F(freg_m, dbl);
			double res = func(f_n, f_m);
			setFPR_F(freg_d, res, dbl);
		}
	}
}

INLINE void VFP_2VOP(bool thumb, u32 cond, u32 opcode, u32 freg_d, u32 dbl, u32 freg_m)
{
	if (!COND(cond))
		return;

	Assert(opcode < 4);
	vop2func func = vop2funcArr[opcode];

	if (cpu.vlen == 0 || freg_d < 8)
	{
		double f_m = FPR_F(freg_m, dbl);
		double res = func(f_m);
		setFPR_F(freg_d, res, dbl);
	}
	else if (freg_m < 8)
	{
		double f_m = FPR_F(freg_m, dbl);
		double res = func(f_m);
		for (int i = 0; i <= cpu.vlen; i++, VINC(freg_d))
			setFPR_F(freg_d, res, dbl);
	}
	else
	{
		for (int i = 0; i <= cpu.vlen; i++, VINC(freg_d), VINC(freg_m))
		{
			double f_m = FPR_F(freg_m, dbl);
			double res = func(f_m);
			setFPR_F(freg_d, res, dbl);
		}
	}
}

INLINE void VFP_2FOP(bool thumb, u32 cond, u32 opcode, u32 freg_d, u32 dbl, u32 freg_m)
{
	if (!COND(cond))
		return;

	switch (opcode)
	{
	case 8: //cmp
	case 9: //cmpe
		{
			double f_d = FPR_F(freg_d, dbl);
			double f_m = FPR_F(freg_m, dbl);
			cpu.fpnf = f_d < f_m;
			cpu.fpzf = f_d == f_m;
			cpu.fpcf = !(f_d < f_m);
			cpu.fpvf = (f_d != f_d) || (f_m != f_m);
			break;
		}

	case 10: //cmpz
	case 11: //cmpez
		{
			double f_d = FPR_F(freg_d, dbl);
			cpu.fpnf = f_d < 0.0;
			cpu.fpzf = f_d == 0.0;
			cpu.fpcf = !(f_d < 0.0);
			cpu.fpvf = (f_d != f_d);
			break;
		}

	case 15: //fcvtds/sd
		if (!dbl) //single to double
			setFPR_F(freg_d, FPR_F(freg_m, 0), 1);
		else //double to single
			setFPR_F(freg_d, FPR_F(freg_m, 1), 0);
		break;

	case 16: //unsigned to float
		setFPR_F(freg_d, (u32)FPR_U(freg_m, 0), dbl);
		break;
	case 17: //signed to float
		setFPR_F(freg_d, (s32)FPR_U(freg_m, 0), dbl);
		break;

	case 24: //float to unsigned 
		setFPR_U(freg_d, (u32)clampU(round(FPR_F(freg_m, dbl))), 0);
		break;
	case 25: //float to unsigned, round to zero
		setFPR_U(freg_d, (u32)clampU(FPR_F(freg_m, dbl)), 0);
		break;
	case 26: //float to signed
		setFPR_U(freg_d, (s32)clampS(round(FPR_F(freg_m, dbl))), 0);
		break;
	case 27: //float to signed, round to zero 
		setFPR_U(freg_d, (s32)clampS(FPR_F(freg_m, dbl)), 0);
		break;

	default:
		Fail(opcode);
	}
}

}