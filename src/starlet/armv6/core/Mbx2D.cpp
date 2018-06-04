#include "GL/glew.h"
#include "GL/gl.h"

#include "Mbx2D.h"
#include "Surface.h"
#include "FrameBufferPort.h"

// Helper functions

u32 nextpow2(u32 x)
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
    x |= x >> 8;
	x |= x >> 16;
    x++;
	return x;
}

void glVertex2dAdj(double x, double y)
{
	glVertex2d(x/160.0 - 1.0, 1.0 - y/240.0);
}

void MbxFunc4(Surface* outSurf, void* data)
{
	struct Params
	{ 
		u16 outPitch; u8 unk11, unk12; u32 unkz1; //outPitch not supported yet
		u16 inPitch; u8 unk21, unk22; u32 unkz2;
		struct { u32 inY1:12; u32 in4X1:12; u32 unk3:8; };
		u32 unk4; u16 rop4;
		u16 unk5; 
		struct { u8 b; u8 g; u8 r; u8 a; } col;
		u16 outY1, outX1, outY2, outX2; //dimensions inside outSurf
		u32 unk7[6]; 
	} p = *(Params *)data;

	Warn("-----------------------------");
	Warn("A outP:%d %d %d %d", p.outPitch, p.unk11, p.unk12, p.unkz1);
	Warn("B inP:%d %d %d %d", p.inPitch, p.unk21, p.unk22, p.unkz2);
	Warn("C in: Y1:%d X1*4:%d ?:%x", p.inY1, p.in4X1, p.unk3);
	Warn("D %x %x %x", p.unk4, p.rop4, p.unk5);
	Warn("E %x %x %x %x", p.col.b, p.col.g, p.col.r, p.col.a);
	Warn("F out: Y1:%d X1:%d Y2:%d X2:%d", p.outY1, p.outX1, p.outY2, p.outX2);
	Warn("G %x %x %x %x %x %x", p.unk7[0], p.unk7[1], p.unk7[2], p.unk7[3], p.unk7[4], p.unk7[5]);
	Warn("-----------------------------");
	Warn("");

	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);

		glColor4b(p.col.r, p.col.g, p.col.b, p.col.a);

		glVertex2dAdj (p.outX1, p.outY1);
		glVertex2dAdj (p.outX1, p.outY2);
		glVertex2dAdj (p.outX2, p.outY2);
		glVertex2dAdj (p.outX2, p.outY1);

	glEnd();*/
}

void MbxFunc5(Surface* inSurf, Surface* outSurf, void* data)
{
	struct Params
	{ 
		u16 outPitch; u8 unk11, unk12; u32 unkz1; //outPitch not supported yet
		u16 inPitch; u8 unk21, unk22; u32 unkz2;
		struct { u32 inY1:12; u32 in4X1:12; u32 unk3:8; };
		u32 unk4; u16 rop4;
		u16 unk5; u32 unk6; 
		u16 outY1, outX1, outY2, outX2; //dimensions inside outSurf
		u32 unk7; 
	} p = *(Params *)data;

	Warn("-----------------------------");
	Warn("A outP:%d %d %d %d", p.outPitch, p.unk11, p.unk12, p.unkz1);
	Warn("B inP:%d %d %d %d", p.inPitch, p.unk21, p.unk22, p.unkz2);
	Warn("C in: Y1:%d X1*4:%d ?:%x", p.inY1, p.in4X1, p.unk3);
	Warn("D %x %x %x %x", p.unk4, p.rop4, p.unk5, p.unk6);
	Warn("E out: Y1:%d X1:%d Y2:%d X2:%d", p.outY1, p.outX1, p.outY2, p.outX2);
	Warn("F %x", p.unk7);
	Warn("-----------------------------");
	Warn("");

	u32* inCopy = new u32[inSurf->AllocSize() / 4];
	FromRamCpy(inCopy, inSurf->MemBase() + inSurf->MemOffset(), inSurf->AllocSize());

	double anyW = p.outX2 - p.outX1;
	double anyH = p.outY2 - p.outY1;

	double inAllW = p.inPitch / 4;

	Assert(p.in4X1 % 4 == 0);
	double inX1 = p.in4X1 / 4;
	double inY1 = p.inY1;

	double inX2 = inX1 + anyW;
	double inY2 = inY1 + anyH;

	GLuint image;
	glGenTextures(1, &image);
	glBindTexture(GL_TEXTURE_2D, image);
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, inAllW, inY2, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, inCopy);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);

		glTexCoord2d (inX1/inAllW, inY1/inY2);
		glVertex2dAdj (p.outX1, p.outY1);

		glTexCoord2d (inX1/inAllW, 1.0);
		glVertex2dAdj (p.outX1, p.outY2);

		glTexCoord2d (inX2/inAllW, 1.0);
		glVertex2dAdj (p.outX2, p.outY2);

		glTexCoord2d (inX2/inAllW, inY1/inY2);
		glVertex2dAdj (p.outX2, p.outY1);

	glEnd();

	glDeleteTextures(1, &image);

	delete[] inCopy;
}

void MbxFunc7(Surface* outSurf, void* data)
{
	struct Params
	{ 
		float xStart, yStart, xEnd, yEnd;
		u32 unk18k_0, unkwid_0;
		u32 unk6_1, unkwid_1;
		u16 xStart16, xEnd16, yStart16, yEnd16; //of WHAT?! similar to the floats but off
		struct { float x, y; } coord[4];
		u32 bits1;
		struct { float x, y; } odd[4];
		u32 ugh0;

	} p = *(Params *)data;

	Warn("-----------------------------");
	Warn("A %g %g %g %g", p.xStart, p.yStart, p.xEnd, p.yEnd);
	Warn("B %d %d %d %d", p.unk18k_0, p.unkwid_0, p.unk6_1, p.unkwid_1);
	Warn("C %d %d %d %d", p.xStart16, p.xEnd16, p.yStart16, p.yEnd16); //round to 8?
	Warn("D %g %g %g %g %g %g %g %g", 
		p.coord[0].x, p.coord[0].y, p.coord[1].x, p.coord[1].y, 
		p.coord[2].x, p.coord[2].y, p.coord[3].x, p.coord[3].y);
	Warn("E %x", p.bits1);
	Warn("F %g %g %g %g %g %g %g %g", 
		p.odd[0].x, p.odd[0].y, p.odd[1].x, p.odd[1].y, 
		p.odd[2].x, p.odd[2].y, p.odd[3].x, p.odd[3].y);
	Warn("G %x", p.ugh0);
	Warn("-----------------------------");
	Warn("");

	/*
	u32 inW, inH;
	inW = p.width;
	inH = (inSurf->AllocSize() / 4) / inW;

	glScissor(p.xStart, p.yStart, p.xEnd - p.xStart, p.yEnd - p.yStart);
	glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);

	glBegin(GL_QUADS);
		glTexCoord2d (0.0, 0.0);
		glVertex2d (p.coord[1].x/160.0 - 1.0, -p.coord[1].y/240.0 + 1.0);
		glTexCoord2d (0.0, 1.0);
		glVertex2d (p.coord[0].x/160.0 - 1.0, -p.coord[0].y/240.0 + 1.0);
		glTexCoord2d (1.0, 1.0);
		glVertex2d (p.coord[2].x/160.0 - 1.0, -p.coord[2].y/240.0 + 1.0);
		glTexCoord2d (1.0, 0.0);
		glVertex2d (p.coord[3].x/160.0 - 1.0, -p.coord[3].y/240.0 + 1.0);
	glEnd();*/
}

void MbxFunc8(Surface* inSurf, Surface* outSurf, void* data)
{
	struct Params
	{ 
		float outX1, outY1, outX2, outY2;
		u32 unk18k_1, unkwid_1; //unkwid_1 may be outPitch
		u32 unk6, unkwid_2; //unkwid_2 may be outPitch
		u16 out16X1, out16X2, out16Y1, out16Y2; //like outX/Y1/2 but aligned to 16 bytes (down for 1, up for 2)
		u32 unkblend1, unkblend2; //blend?
		u32 unk18k_2, inPitch; 
		struct { float x, y; } out[4], in[4];
		u32 bits1, bits2;
		float ones[4]; //???
	} p = *(Params *)data;

	Warn("-----------------------------");
	Warn("A outDim: X1:%g Y1:%g X2:%g Y2:%g", p.outX1, p.outY1, p.outX2, p.outY2);
	Warn("B %d %d %d %d", p.unk18k_1, p.unkwid_1, p.unk6, p.unkwid_2);
	Warn("C out??: %d %d %d %d", p.out16X1, p.out16X2, p.out16Y1, p.out16Y2);
	Warn("D %d %d %d inP:%d", p.unkblend1, p.unkblend2, p.unk18k_2, p.inPitch);
	Warn("E %g %g %g %g %g %g %g %g", 
		p.out[0].x, p.out[0].y, p.out[1].x, p.out[1].y, 
		p.out[2].x, p.out[2].y, p.out[3].x, p.out[3].y);
	Warn("F %g %g %g %g %g %g %g %g", 
		p.in[0].x, p.in[0].y, p.in[1].x, p.in[1].y, 
		p.in[2].x, p.in[2].y, p.in[3].x, p.in[3].y);
	Warn("G %x %x", p.bits1, p.bits2);
	Warn("H %g %g %g %g", p.ones[0], p.ones[1], p.ones[2], p.ones[3]);
	Warn("-----------------------------");
	Warn("");

	u32* inCopy = new u32[inSurf->AllocSize() / 4];
	FromRamCpy(inCopy, inSurf->MemBase() + inSurf->MemOffset(), inSurf->AllocSize());

	u32 inW = p.inPitch;
	u32 inH = (inSurf->AllocSize() / 4 + inW - 1) / inW;
	u32 inHsafe = (inSurf->AllocSize() / 4) / inW;

	u32 inW2 = nextpow2(inW);
	u32 inH2 = nextpow2(inH);

	double inWrel = (double)inW2 / (double)inW;
	double inHrel = (double)inH2 / (double)inH;

	GLuint image;
	glGenTextures(1, &image);
	glBindTexture(GL_TEXTURE_2D, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, inW, inHsafe, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, inCopy);

	glScissor(p.outX1, p.outY1, p.outX2 - p.outX1, p.outY2 - p.outY1); //this is the scissor?

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);

		glTexCoord2d (p.in[1].x*inWrel, p.in[1].y*inHrel);
		glVertex2dAdj (p.out[1].x, p.out[1].y);

		glTexCoord2d (p.in[0].x*inWrel, p.in[0].y*inHrel);
		glVertex2dAdj (p.out[0].x, p.out[0].y);

		glTexCoord2d (p.in[2].x*inWrel, p.in[2].y*inHrel);
		glVertex2dAdj (p.out[2].x, p.out[2].y);

		glTexCoord2d (p.in[3].x*inWrel, p.in[3].y*inHrel);
		glVertex2dAdj (p.out[3].x, p.out[3].y);

	glEnd();

	glDeleteTextures(1, &image);

	delete[] inCopy;
}

void MbxFunc9(Surface* inSurf, Surface* outSurf, void* data)
{
	struct Params
	{ 
		float outX1, outY1, outX2, outY2;
		u32 unk18k_1, unkwid_1; //unkwid_1 may be outPitch
		u32 unk6, unkwid_2; //unkwid_2 may be outPitch
		u16 out16X1, out16X2, out16Y1, out16Y2; //like outX/Y1/2 but aligned to 16 bytes (down for 1, up for 2)
		u32 unkblend1, unkblend2; //blend?
		u32 unk18k_2, inPitch; 
		struct { float x, y; } out[4], in[4];
		u32 bits1, bits2;
		float ones[4]; //???
		struct { float x, y; } outWeird[4]; //out / 1024 ???
		u32 ugh1, ugh2; //what

	} p = *(Params *)data;

	Warn("-----------------------------");
	Warn("A outDim: X1:%g Y1:%g X2:%g Y2:%g", p.outX1, p.outY1, p.outX2, p.outY2);
	Warn("B %d %d %d %d", p.unk18k_1, p.unkwid_1, p.unk6, p.unkwid_2);
	Warn("C out??: %d %d %d %d", p.out16X1, p.out16X2, p.out16Y1, p.out16Y2);
	Warn("D %d %d %d inP:%d", p.unkblend1, p.unkblend2, p.unk18k_2, p.inPitch);
	Warn("E %g %g %g %g %g %g %g %g", 
		p.out[0].x, p.out[0].y, p.out[1].x, p.out[1].y, 
		p.out[2].x, p.out[2].y, p.out[3].x, p.out[3].y);
	Warn("F %g %g %g %g %g %g %g %g", 
		p.in[0].x, p.in[0].y, p.in[1].x, p.in[1].y, 
		p.in[2].x, p.in[2].y, p.in[3].x, p.in[3].y);
	Warn("G %x %x", p.bits1, p.bits2);
	Warn("H %g %g %g %g", p.ones[0], p.ones[1], p.ones[2], p.ones[3]);
	Warn("I %g %g %g %g %g %g %g %g", 
		p.outWeird[0].x, p.outWeird[0].y, p.outWeird[1].x, p.outWeird[1].y,
		p.outWeird[2].x, p.outWeird[2].y, p.outWeird[3].x, p.outWeird[3].y);
	Warn("J %x %x", p.ugh1, p.ugh2);
	Warn("-----------------------------");
	Warn("");

	u32* inCopy = new u32[inSurf->AllocSize() / 4];
	FromRamCpy(inCopy, inSurf->MemBase() + inSurf->MemOffset(), inSurf->AllocSize());

	u32 inW = p.inPitch;
	u32 inH = (inSurf->AllocSize() / 4 + inW - 1) / inW;
	u32 inHsafe = (inSurf->AllocSize() / 4) / inW;

	u32 inW2 = nextpow2(inW);
	u32 inH2 = nextpow2(inH);

	double inWrel = (double)inW2 / (double)inW;
	double inHrel = (double)inH2 / (double)inH;

	GLuint image;
	glGenTextures(1, &image);
	glBindTexture(GL_TEXTURE_2D, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, inW, inHsafe, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, inCopy);

	glScissor(p.outX1, p.outY1, p.outX2 - p.outX1, p.outY2 - p.outY1); //this is the scissor?

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);

		glTexCoord2d (p.in[1].x*inWrel, p.in[1].y*inHrel);
		glVertex2dAdj (p.out[1].x, p.out[1].y);

		glTexCoord2d (p.in[0].x*inWrel, p.in[0].y*inHrel);
		glVertex2dAdj (p.out[0].x, p.out[0].y);

		glTexCoord2d (p.in[2].x*inWrel, p.in[2].y*inHrel);
		glVertex2dAdj (p.out[2].x, p.out[2].y);

		glTexCoord2d (p.in[3].x*inWrel, p.in[3].y*inHrel);
		glVertex2dAdj (p.out[3].x, p.out[3].y);

	glEnd();

	glDeleteTextures(1, &image);

	delete[] inCopy;
}

void MbxCommandStream(u32* start, u32* end)
{
	while (start < end)
	{
		u32 totalSize = start[0];
		u32 func = start[1];
		u32 extraSize = start[2];
		u32 inputAmt = start[3];
		u32 outputAmt = start[4];
		u32* input = &start[5];
		u32* output = &start[5 + inputAmt];
		void* data = &start[5 + inputAmt + outputAmt];

		switch (func)
		{
		case 1:
			Assert(totalSize == 0x14 && extraSize == 0x0 && inputAmt == 1 && outputAmt == 0);
			{
				Surface* in = SurfaceRootPort::GetSurface(input[0]);
				Warn("mbx1 %d", input[0]);
			}
			break;

		case 3:
			Assert(totalSize == 0x1c && extraSize == 0xc && inputAmt == 0 && outputAmt == 0);
			{
				Warn("mbx3");
			}
			break;

		case 4:
			Assert(totalSize == 0x54 && extraSize == 0x40 && inputAmt == 0 && outputAmt == 1);
			{
				Surface* out = SurfaceRootPort::GetSurface(output[0]);

				Warn("mbx4 %d", output[0]);
				MbxFunc4(out, data);
			}
			break;

		case 5:
			Assert(totalSize == 0x44 && extraSize == 0x2c && inputAmt == 1 && outputAmt == 1);
			{
				Surface* in = SurfaceRootPort::GetSurface(input[0]);
				Surface* out = SurfaceRootPort::GetSurface(output[0]);
				
				Warn("mbx5 %d %d", output[0], input[0]);
				MbxFunc5(in, out, data);
			}
			break;

		case 7:
			Assert(totalSize == 0x84 && extraSize == 0x70 && inputAmt == 0 && outputAmt == 1);
			{
				Surface* out = SurfaceRootPort::GetSurface(output[0]);
				
				Warn("mbx7 %d", output[0]);
				MbxFunc7(out, data);
			}
			break;

		case 8:
			Assert(totalSize == 0xa8 && extraSize == 0x90 && inputAmt == 1 && outputAmt == 1);
			{
				Surface* in = SurfaceRootPort::GetSurface(input[0]);
				Surface* out = SurfaceRootPort::GetSurface(output[0]);
				
				Warn("mbx8 %d %d", output[0], input[0]);
				MbxFunc8(in, out, data);
			}
			break;

		case 9:
			Assert(totalSize == 0xd0 && extraSize == 0xb8 && inputAmt == 1 && outputAmt == 1);
			{
				Surface* in = SurfaceRootPort::GetSurface(input[0]);
				Surface* out = SurfaceRootPort::GetSurface(output[0]);
				
				Warn("mbx9 %d %d", output[0], input[0]);
				MbxFunc9(in, out, data);
			}
			break;

		default:
			Warn("Unk Mbx command in stream %d", func);
		}
		start += (totalSize / 4) + 1;
	}

	glFlush();

	stop = true;
}