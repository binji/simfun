#ifndef __COLOR_H__
#define __COLOR_H__

struct Color
{
	typedef unsigned char u8;
	u8 r,g,b,a;
	
	Color(): r(0), g(0), b(0), a(0) {}
	Color(u8 _r, u8 _g, u8 _b, u8 _a): r(_r), g(_g), b(_b), a(_a) {}
	static Color randomRange(const Color &a, const Color &b);
};

#endif