#pragma once

#include <atltypes.h>
#include <cmath>

#define  SIGN_OF(x) ((x)>0?1:(x)=0?0:-1)
#define  DISTANCE(x1,y1,x2,y2)	(std::abs(x1-x2)+std::abs(y1-y2))

class CRect2: public CRect
{
public:
	inline static int Compare(LPCRECT lpRect1, LPCRECT lpRect2)
	{
		int dLeft{lpRect1->left - lpRect2->left};
		if(dLeft!=0)
		{
			return dLeft;
		}
		else
		{
			int dTop{lpRect1->top - lpRect2->top};
			return dTop;
		}
	}
	[[nodiscard]] inline static bool IsIntersect(const LPCRECT lpRect1, const LPCRECT lpRect2)
	{
		CRect r;
		return ::IntersectRect(r, lpRect1, lpRect2) == TRUE;
	}
	[[nodiscard]] constexpr inline static int Area(const LPCRECT lpRect)
	noexcept
	{
		return (lpRect->right-lpRect->left)*(lpRect->bottom-lpRect->top);
	}
};

class CPoint2: public CPoint
{
public:
	inline static int Distance(POINT p1, POINT p2)
	{
		return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y);
	}
	[[nodiscard]] inline static int Distance(const int x1, const int y1, const int x2, const int y2)
	noexcept
	{
		return std::abs(x1-x2) + std::abs(y1-y2);
	}
};
