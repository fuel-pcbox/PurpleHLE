#pragma once

#include "Assert.h"
#include "intrin.h"
#include "stdio.h"
#include "stdlib.h"
#include <stdarg.h>

#include <string>

typedef unsigned __int8 u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

typedef signed __int8 s8;
typedef signed __int16 s16;
typedef signed __int32 s32;
typedef signed __int64 s64;

typedef u32 macptr;

#define CMASK(count) ((~(u32)0) >> (32 - (count)))

#define MASK(first, count) (CMASK(count) << (first))

#define BITS(data, first, count) (((data) >> (first)) & CMASK(count))

#define BIT(data, first) (BITS(data, first, 1))

#define SIGNEX(data, amount) ((s32)((data) << (32 - (amount))) >> (32 - (amount)))

#define ROUNDUP(value, up) ((((value) + (up) - 1) / (up)) * (up))

#define ROUNDUP_MASK(value, mask) (((value) + (mask)) &~ (mask)) //not sure about this at all!

#define LOW32(data) ((u32)(data & 0xffffffff))
#define HIGH32(data) ((u32)(data >> 32))
#define MAKE64(lo, hi) ((u64)(lo) | ((u64)(hi) << 32))

#define _S(str) (std::string(str))

#define INLINE __forceinline

#define NODEFAULT(x) default: Fail(x)

#define MACRO(mac) do { mac; } while(0)

#define STRICT_ALIGN __pragma(pack(push, 1))
#define POP_ALIGN __pragma(pack(pop))

class Logger
{
public:
	Logger() { fopen_s(&f, "log.txt", "w"); }
	~Logger() { fclose(f); }

	void Error(std::string func, int line, int note) 
		{ fprintf(f, "ERROR in %s, file line %d. note: 0x%x\n", func.c_str(), line, note); fflush(f); }
	
	void Warn(std::string format, ...) 
	{ 
		va_list args;
		va_start(args, format);
		vfprintf(f, (format + "\n").c_str(), args); fflush(f); 
		va_end(args);
	}

protected:
	FILE *f;
};

class MyError {};

extern Logger* g_log;

#define Fail(x) MACRO( g_log->Error(std::string(__FUNCTION__), __LINE__, x); throw MyError(); )
#define Warn1(x) g_log->Error(std::string(__FUNCTION__), __LINE__, x)
#define Warn(...) g_log->Warn(__VA_ARGS__)
#define WarnFail(...) MACRO( g_log->Warn(__VA_ARGS__); throw MyError(); )

#define MakeSure(x) MACRO( if (!(x)) Warn1(-1) )
#define Assert(x) MACRO( if (!(x)) Fail(-1) )

template <typename D, typename S>
INLINE D As(S value)
{
	return *(D*)&value;
}

extern bool stop;