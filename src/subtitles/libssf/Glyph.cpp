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
#include "Glyph.h"
#include "Split.h"

#define deg2rad(d) static_cast<float>(M_PI/180*(d))

namespace ssf
{
	Glyph::Glyph()
	{
		c = 0;
		font = nullptr;
		ascent = descent = width = spacing = fill = 0;
		tl.x = tl.y = tls.x = tls.y = 0;
	}

	float Glyph::GetBackgroundSize() const
	{
		return style.background.size * (scale.cx + scale.cy) / 2;
	}

	float Glyph::GetShadowDepth() const
	{
		return style.shadow.depth * (scale.cx + scale.cy) / 2;
	}

	CRect Glyph::GetClipRect() const
	{
		CRect r = bbox + tl;

		int size = static_cast<int>(GetBackgroundSize() + 0.5);
		int depth = static_cast<int>(GetShadowDepth() + 0.5);

		r.InflateRect(size, size);
		r.InflateRect(depth, depth);

		r.left >>= 6;
		r.top >>= 6;
		r.right = (r.right + 32) >> 6;
		r.bottom = (r.bottom + 32) >> 6;

		return r;
	}

	void Glyph::CreateBkg()
	{
		path_bkg.RemoveAll();

		if(style.background.type == L"enlarge" && style.background.size > 0)
		{
			path_bkg.Enlarge(path, GetBackgroundSize());
		}
		else if(style.background.type == L"box" && style.background.size >= 0)
		{
			if(c != ssf::Text::LSEP)
			{
				int s = static_cast<int>(GetBackgroundSize() + 0.5);
				int x0 = (!vertical ? -spacing/2 : ascent - row_ascent);
				int y0 = (!vertical ? ascent - row_ascent : -spacing/2);
				int x1 = x0 + (!vertical ? width + spacing : row_ascent + row_descent);
				int y1 = y0 + (!vertical ? row_ascent + row_descent : width + spacing);
				path_bkg.types.SetCount(4U);
				path_bkg.types[0U] = PT_MOVETO;
				path_bkg.types[1U] = PT_LINETO;
				path_bkg.types[2U] = PT_LINETO;
				path_bkg.types[3U] = PT_LINETO;
				path_bkg.points.SetCount(4U);
				path_bkg.points[0U] = CPoint(x0-s, y0-s);
				path_bkg.points[1U] = CPoint(x1+s, y0-s);
				path_bkg.points[2U] = CPoint(x1+s, y1+s);
				path_bkg.points[3U] = CPoint(x0-s, y1+s);
			}
		}
	}

	void Glyph::CreateSplineCoeffs(const CRect& spdrc)
	{
		spline.RemoveAll();

		if(style.placement.path.IsEmpty())
		{
			return;
		}

		std::size_t i{0U}, j{style.placement.path.GetCount()};

		CAtlArray<Point> pts;
		pts.SetCount(j + 2U);

		Point p;

		while(i < j)
		{
			p.x = style.placement.path[i].x * scale.cx + spdrc.left * 64;
			p.y = style.placement.path[i].y * scale.cy + spdrc.top * 64;
			pts[++i] = p;
		}

		if(pts.GetCount() >= 4U)
		{
			if(pts[1U].x == pts[j].x && pts[1U].y == pts[j].y)
			{
				pts.SetAt(0, pts[j-1U]);
				pts.SetAt(j+1U, pts[2U]);
			}
			else
			{
				p.x = pts[1U].x*2 - pts[2U].x;
				p.y = pts[1U].y*2 - pts[2U].y;
				pts.SetAt(0, p);

				p.x = pts[j].x*2 - pts[j-1U].x;
				p.y = pts[j].y*2 - pts[j-1U].y;
				pts.SetAt(j+1U, p);
			}

			spline.SetCount(pts.GetCount()-3U);

			for(std::size_t splineIndex{0U}, lastSpline{pts.GetCount()-4U}; splineIndex <= lastSpline; ++splineIndex)
			{
				static const float _1div6 = 1.0f / 6;

				SplineCoeffs sc;

				sc.cx[3U] = _1div6*(-  pts[splineIndex+0U].x + 3*pts[splineIndex+1U].x - 3*pts[splineIndex+2U].x + pts[splineIndex+3U].x);
				sc.cx[2U] = _1div6*( 3*pts[splineIndex+0U].x - 6*pts[splineIndex+1U].x + 3*pts[splineIndex+2U].x);
				sc.cx[1U] = _1div6*(-3*pts[splineIndex+0U].x	            + 3*pts[splineIndex+2U].x);
				sc.cx[0U] = _1div6*(   pts[splineIndex+0U].x + 4*pts[splineIndex+1U].x + 1*pts[splineIndex+2U].x);

				sc.cy[3U] = _1div6*(-  pts[splineIndex+0U].y + 3*pts[splineIndex+1U].y - 3*pts[splineIndex+2U].y + pts[splineIndex+3U].y);
				sc.cy[2U] = _1div6*( 3*pts[splineIndex+0U].y - 6*pts[splineIndex+1U].y + 3*pts[splineIndex+2U].y);
				sc.cy[1U] = _1div6*(-3*pts[splineIndex+0U].y                + 3*pts[splineIndex+2U].y);
				sc.cy[0U] = _1div6*(   pts[splineIndex+0U].y + 4*pts[splineIndex+1U].y + 1*pts[splineIndex+2U].y);

				spline.SetAt(splineIndex, sc);
			}
		}
	}

	void Glyph::Transform(GlyphPath& glyphPath, CPoint org, const CRect& subrect)
	{
		// TODO: add sse code path

		float sx = style.font.scale.cx;
		float sy = style.font.scale.cy;

		bool brotate = style.placement.angle.x || style.placement.angle.y || style.placement.angle.z;
		bool bspline = !spline.IsEmpty();
		bool bscale = brotate || bspline || sx != 1 || sy != 1;

		float caz = static_cast<float>(std::cos(deg2rad(style.placement.angle.z)));
		float saz = static_cast<float>(std::sin(deg2rad(style.placement.angle.z)));
		float cax = static_cast<float>(std::cos(deg2rad(style.placement.angle.x)));
		float sax = static_cast<float>(std::sin(deg2rad(style.placement.angle.x)));
		float cay = static_cast<float>(std::cos(deg2rad(style.placement.angle.y)));
		float say = static_cast<float>(std::sin(deg2rad(style.placement.angle.y)));

		for(std::size_t i{0U}, j{glyphPath.types.GetCount()}; i < j; ++i)
		{
			CPoint p = glyphPath.points[i];

			if(bscale)
			{
				float x, y, z, xx, yy, zz;

				x = sx * (p.x - org.x);
				y = sy * (p.y - org.y);
				z = 0;

				if(bspline)
				{
					float pos = vertical ? y + org.y + tl.y - subrect.top : x + org.x + tl.x - subrect.left;
					float size = static_cast<float>(vertical ? subrect.Size().cy : subrect.Size().cx);
					float dist = vertical ? x : y;

					const SplineCoeffs* sc;
					float t;

					if(pos >= size)
					{
						sc = &spline[spline.GetCount() - 1U];
						t = 1;
					}
					else
					{
						float u = size / spline.GetCount();
						sc = &spline[max(static_cast<int>(pos / u), 0)];
						t = static_cast<float>(std::fmod(pos, u) / u);
					}

					float nx = sc->cx[1U] + 2*t*sc->cx[2U] + 3*t*t*sc->cx[3U];
					float ny = sc->cy[1U] + 2*t*sc->cy[2U] + 3*t*t*sc->cy[3U];
					float nl = static_cast<float>(1.0f / std::sqrt(nx*nx + ny*ny));

					nx *= nl;
					ny *= nl;

					x = sc->cx[0U] + t*(sc->cx[1U] + t*(sc->cx[2U] + t*sc->cx[3U])) - ny * dist - org.x - tl.x;
					y = sc->cy[0U] + t*(sc->cy[1U] + t*(sc->cy[2U] + t*sc->cy[3U])) + nx * dist - org.y - tl.y;
				}

				if(brotate)
				{
					xx = x*caz + y*saz;
					yy = -(x*saz - y*caz);
					zz = z;

					x = xx;
					y = yy*cax + zz*sax;
					z = yy*sax - zz*cax;

					xx = x*cay + z*say;
					yy = y;
					zz = x*say - z*cay;

					zz = 1.0f / (max(zz, -19000) + 20000);

					x = (xx * 20000) * zz;
					y = (yy * 20000) * zz;
				}

				p.x = static_cast<int>(x + org.x + 0.5);
				p.y = static_cast<int>(y + org.y + 0.5);

				glyphPath.points[i] = p;
			}

			if(p.x < bbox.left) { bbox.left = p.x; }
			if(p.x > bbox.right) { bbox.right = p.x; }
			if(p.y < bbox.top) { bbox.top = p.y; }
			if(p.y > bbox.bottom) { bbox.bottom = p.y; }
		}
	}

	void Glyph::Transform(CPoint org, const CRect& subrect)
	{
		if(!style.placement.org.auto_x) { org.x = static_cast<LONG>(style.placement.org.x * scale.cx); }
		if(!style.placement.org.auto_y) { org.y = static_cast<LONG>(style.placement.org.y * scale.cy); }

		org -= tl;

		bbox.SetRect(INT_MAX, INT_MAX, INT_MIN, INT_MIN);

		Transform(path_bkg, org, subrect);
		Transform(path, org, subrect);

		bbox |= CRect(0, 0, 0, 0);
	}

	void Glyph::Rasterize()
	{
		if(!path_bkg.IsEmpty())
		{
			ras_bkg.ScanConvert(path_bkg, bbox);
			ras_bkg.Rasterize(tl.x, tl.y);
		}

		ras.ScanConvert(path, bbox);

		if(style.background.type == L"outline" && style.background.size > 0)
		{
			ras.CreateWidenedRegion(static_cast<int>(GetBackgroundSize() + 0.5));
		}

		//

		Rasterizer* r = path_bkg.IsEmpty() ? &ras : &ras_bkg;
		int plane = path_bkg.IsEmpty() ? (style.font.color.a < 255 ? 2 : 1) : 0;
		
		ras.Rasterize(tl.x, tl.y);
		r->Blur(style.background.blur, plane);

		if(style.shadow.depth > 0)
		{
			ras_shadow.Reuse(*r);

			float depth = GetShadowDepth();

			tls.x = tl.x + static_cast<int>(depth * std::cos(deg2rad(style.shadow.angle)) + 0.5);
			tls.y = tl.y + static_cast<int>(depth * -std::sin(deg2rad(style.shadow.angle)) + 0.5);

			ras_shadow.Rasterize(tls.x, tls.y);
			ras_shadow.Blur(style.shadow.blur, plane ? 1 : 0);
		}
	}

}