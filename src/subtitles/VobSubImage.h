/*
 * (C) 2003-2006 Gabest
 * (C) 2006-2014 see Authors.txt
 *
 * This file is part of MPC-HC.
 *
 * MPC-HC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * MPC-HC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <atlcoll.h>

struct COutline {
    CAtlArray<CPoint> pa;
    CAtlArray<int> da;
    void RemoveAll() {
        pa.RemoveAll();
        da.RemoveAll();
    }
    void Add(const CPoint p, const int d) {
        pa.Add(p);
        da.Add(d);
    }
};

class CVobSubImage
{
    friend class CVobSubFile;

private:
    CSize org;
    RGBQUAD* lpTemp1;
    RGBQUAD* lpTemp2;

    std::size_t nOffset[2U], nPlane;
    bool fCustomPal;
    bool fAligned;
    int tridx;
    RGBQUAD* orgpal /*[16]*/, * cuspal /*[4]*/;

    [[nodiscard]] bool Alloc(int w, int h);
    constexpr void Free() noexcept;

    [[nodiscard]] constexpr BYTE GetNibble(const BYTE* lpData) noexcept;
    void DrawPixels(CPoint p, int length, std::size_t colorId);
    void TrimSubImage();

public:
    std::size_t iLang, iIdx;
    bool fForced, fAnimated;
    int tCurrent;
    __int64 start, delay;
    CRect rect;
    struct SubPal {
        BYTE pal: 4, tr: 4;
    };
    SubPal pal[4U];
    RGBQUAD* lpPixels;

    constexpr explicit CVobSubImage() noexcept;
    constexpr virtual ~CVobSubImage() noexcept;

    void Invalidate() { iLang = iIdx = -1; }

    void GetPacketInfo(const BYTE* lpData, std::size_t packetSize, std::size_t dataSize, int t = INT_MAX);
    [[nodiscard]] bool Decode(BYTE* lpData, std::size_t packetSize, std::size_t dataSize, int t,
                bool fCustomPal,
                int tridx,
                RGBQUAD* orgpal /*[16]*/, RGBQUAD* cuspal /*[4]*/,
                bool fTrim);

private:
    CAutoPtrList<COutline>* GetOutlineList(CPoint& topleft);
    [[nodiscard]] int  GrabSegment(int start, const COutline& o, COutline& ret);
    void SplitOutline(const COutline& o, COutline& o1, COutline& o2);
    void AddSegment(COutline& o, CAtlArray<BYTE>& pathTypes, CAtlArray<CPoint>& pathPoints);

public:
    bool Polygonize(CAtlArray<BYTE>& pathTypes, CAtlArray<CPoint>& pathPoints, bool fSmooth, int scale);
    [[nodiscard]] bool Polygonize(CStringW& assstr, bool fSmooth = true, int scale = 3);

    void Scale2x();
};
