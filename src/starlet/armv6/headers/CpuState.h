#pragma once
#include "types.h"

namespace Interpreter
{

struct CpuState
{
	union
	{
		u32 gpr[15];
		struct
		{
			u32 a1, a2, a3, a4, v1, v2, v3, v4;
			u32 v5, v6, v7, v8, ip, sp, lr;
		};
	};
	u32 nia;
	u8 cf, zf, vf, nf;
	u8 qf, ge;
	u8 thumb;

	union
	{
		u32 fpr32[32];
		float fprs[32];
		u64 fpr64[16];
		double fprd[16];
	};
	u8 fpcf, fpzf, fpvf, fpnf;
	u8 vlen, vstride;
	u32 fpmode; //most of fpscr

	bool exclusive; //not very good
	u32 threadData;

	CpuState() { memset(this, 0, sizeof(CpuState)); }

};

extern CpuState cpu;

void ExecuteOne();


}