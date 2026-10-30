/*
 *	Copyright (C) 2003-2006 Gabest
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

#include "stdafx.h"
#include "GlyphPath.h"

namespace ssf
{
    GlyphPath::GlyphPath(const GlyphPath& path)
    {
        *this = path;
    }

    void GlyphPath::operator=(const GlyphPath& path)
    {
        types.Copy(path.types);
        points.Copy(path.points);
    }

    bool GlyphPath::IsEmpty()
    {
        return types.IsEmpty() || points.IsEmpty();
    }

    void GlyphPath::RemoveAll()
    {
        types.RemoveAll();
        points.RemoveAll();
    }

    void GlyphPath::MovePoints(const CPoint& o)
    {
        std::size_t n{ points.GetCount() };
        POINT* p = points.GetData();

        unsigned int i{ 0U };

        if (!((DWORD_PTR)p & 7))
        {
            for (; i < n && ((DWORD_PTR)&p[i] & 15); ++i)
            {
                p[i].x += o.x;
                p[i].y += o.y;
            }

            __m128i oo = _mm_set_epi32(o.y, o.x, o.y, o.x);

            for (unsigned int j{ i + ((n - i) & ~7) }; i < j; i += 8)
            {
                __m128i r0 = _mm_load_si128((__m128i*)&p[i + 0U]);
                __m128i r1 = _mm_load_si128((__m128i*)&p[i + 2U]);
                __m128i r2 = _mm_load_si128((__m128i*)&p[i + 4U]);
                __m128i r3 = _mm_load_si128((__m128i*)&p[i + 6U]);
                _mm_store_si128((__m128i*)&p[i + 0U], _mm_add_epi32(r0, oo));
                _mm_store_si128((__m128i*)&p[i + 2U], _mm_add_epi32(r1, oo));
                _mm_store_si128((__m128i*)&p[i + 4U], _mm_add_epi32(r2, oo));
                _mm_store_si128((__m128i*)&p[i + 6U], _mm_add_epi32(r3, oo));
            }
        }

        for (; i < n; ++i)
        {
            p[i].x += o.x;
            p[i].y += o.y;
        }
    }

    void GlyphPath::Enlarge(const GlyphPath& src, float size)
    {
        types.SetCount(src.types.GetCount());
        points.SetCount(src.points.GetCount());

        std::memcpy(types.GetData(), src.types.GetData(), types.GetCount());

        std::size_t start{ 0U }, end{ 0U };

        for (std::size_t i{ 0U }, j{ src.types.GetCount() }; i <= j; ++i)
        {
            if (i > 0U && (i == j || (src.types[i] & ~PT_CLOSEFIGURE) == PT_MOVETO))
            {
                end = i - 1U;

                bool cw = true; // TODO: determine orientation (ttf is always cw and we are sill before Transform)
                float rotate = static_cast<float>(cw ? -M_PI_2 : M_PI_2);

                CPoint prev = src.points[end];
                CPoint cur = src.points[start];

                for (std::size_t k{ start }; k <= end; ++k)
                {
                    CPoint next = k < end ? src.points[k + 1U] : src.points[start];

                    for (int l = static_cast<int>(k - 1U); prev == cur; --l)
                    {
                        if (l < static_cast<int>(start))
                        {
                            l = static_cast<int>(end);
                        }
                        prev = src.points[l];
                    }

                    for (int l = static_cast<int>(k + 1U); next == cur; ++l)
                    {
                        if (l > static_cast<int>(end))
                        {
                            l = static_cast<int>(start);
                        }
                        next = src.points[l];
                    }

                    CPoint in = cur - prev;
                    CPoint out = next - cur;

                    float angle_in = static_cast<float>(std::atan2(static_cast<float>(in.y), static_cast<float>(in.x)));
                    float angle_out = static_cast<float>(std::atan2(static_cast<float>(out.y), static_cast<float>(out.x)));
                    float angle_diff = angle_out - angle_in;
                    if (angle_diff < 0)
                    {
                        angle_diff += static_cast<float>(M_PI) * 2;
                    }
                    if (angle_diff > static_cast<float>(M_PI))
                    {
                        angle_diff -= static_cast<float>(M_PI) * 2;
                    }
                    float scale = static_cast<float>(std::cos(angle_diff / 2));

                    CPoint p;

                    if (angle_diff < 0)
                    {
                        if (angle_diff > -static_cast<float>(M_PI) / 8)
                        {
                            if (scale < 1)
                            {
                                scale = 1;
                            }
                        }
                        else
                        {
                            if (scale < 0.50f)
                            {
                                scale = 0.50f;
                            }
                        }
                    }
                    else
                    {
                        if (angle_diff < static_cast<float>(M_PI) / 8)
                        {
                            if (scale < 0.75f)
                            {
                                scale = 0.75f;
                            }
                        }
                        else
                        {
                            if (scale < 0.25f)
                            {
                                scale = 0.25f;
                            }
                        }
                    }

                    if (scale < 0.1f)
                    {
                        scale = 0.1f;
                    }

                    float angle = angle_in + angle_diff / 2 - rotate;
                    float radius = -size / scale; // FIXME

                    p.x = static_cast<int>(radius * std::cos(angle) + 0.5);
                    p.y = static_cast<int>(radius * std::sin(angle) + 0.5);

                    points[k] = cur + p;

                    prev = cur;
                    cur = next;
                }

                start = end + 1U;
            }
        }
    }
}