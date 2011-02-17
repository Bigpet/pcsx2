/*
 *	Copyright (C) 2007-2009 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#pragma once

#include "GSScanlineEnvironment.h"
#include "GSFunctionMap.h"

using namespace Xbyak;

class GSDrawScanlineCodeGenerator : public GSCodeGenerator
{
	void operator = (const GSDrawScanlineCodeGenerator&);

	static const GSVector4i m_test[8];

	GSScanlineSelector m_sel;
	GSScanlineLocalData& m_local;

	void Generate();

	void Init();
	void Step();
	void TestZ(const Xmm& temp1, const Xmm& temp2);
	void SampleTexture();
	void Wrap(const Xmm& uv0);
	void Wrap(const Xmm& uv0, const Xmm& uv1);
	void AlphaTFX();
	void ReadMask();
	void TestAlpha();
	void ColorTFX();
	void Fog();
	void ReadFrame();
	void TestDestAlpha();
	void WriteMask();
	void WriteZBuf();
	void AlphaBlend();
	void WriteFrame();

	void ReadPixel(const Xmm& dst, const Reg32& addr);
	void WritePixel(const Xmm& src, const Reg32& addr, const Reg8& mask, bool fast, int psm, int fz);
	void WritePixel(const Xmm& src, const Reg32& addr, uint8 i, int psm);
	void ReadTexel(const Xmm& dst, const Xmm& addr, const Xmm& temp1, const Xmm& temp2);
	void ReadTexel(const Xmm& dst, const Xmm& addr, uint8 i);

	template<int shift> void modulate16(const Xmm& a, const Operand& f);
	template<int shift> void lerp16(const Xmm& a, const Xmm& b, const Xmm& f);
	void mix16(const Xmm& a, const Xmm& b, const Xmm& temp);
	void clamp16(const Xmm& a, const Xmm& temp);
	void alltrue();
	void blend(const Xmm& a, const Xmm& b, const Xmm& mask);
	void blendr(const Xmm& b, const Xmm& a, const Xmm& mask);
	void blend8(const Xmm& a, const Xmm& b);
	void blend8r(const Xmm& b, const Xmm& a);

public:
	GSDrawScanlineCodeGenerator(void* param, uint64 key, void* code, size_t maxsize);
};
