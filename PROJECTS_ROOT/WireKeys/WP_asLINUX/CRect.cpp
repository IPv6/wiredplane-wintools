#include "stdafx.h"
#include "CRect.h"

// CSize
_AFXWIN_INLINE CSize::CSize()
	{ /* random filled */ }
_AFXWIN_INLINE CSize::CSize(int initCX, int initCY)
	{ cx = initCX; cy = initCY; }
_AFXWIN_INLINE CSize::CSize(SIZE initSize)
	{ *(SIZE*)this = initSize; }
_AFXWIN_INLINE CSize::CSize(POINT initPt)
	{ *(POINT*)this = initPt; }
_AFXWIN_INLINE CSize::CSize(DWORD dwSize)
	{
		cx = (short)LOWORD(dwSize);
		cy = (short)HIWORD(dwSize);
	}
_AFXWIN_INLINE BOOL CSize::operator==(SIZE size) const
	{ return (cx == size.cx && cy == size.cy); }
_AFXWIN_INLINE BOOL CSize::operator!=(SIZE size) const
	{ return (cx != size.cx || cy != size.cy); }
_AFXWIN_INLINE void CSize::operator+=(SIZE size)
	{ cx += size.cx; cy += size.cy; }
_AFXWIN_INLINE void CSize::operator-=(SIZE size)
	{ cx -= size.cx; cy -= size.cy; }
_AFXWIN_INLINE CSize CSize::operator+(SIZE size) const
	{ return CSize(cx + size.cx, cy + size.cy); }
_AFXWIN_INLINE CSize CSize::operator-(SIZE size) const
	{ return CSize(cx - size.cx, cy - size.cy); }
_AFXWIN_INLINE CSize CSize::operator-() const
	{ return CSize(-cx, -cy); }
_AFXWIN_INLINE CPoint CSize::operator+(POINT point) const
	{ return CPoint(cx + point.x, cy + point.y); }
_AFXWIN_INLINE CPoint CSize::operator-(POINT point) const
	{ return CPoint(cx - point.x, cy - point.y); }
_AFXWIN_INLINE CRect CSize::operator+(const RECT* lpRect) const
	{ return CRect(lpRect) + *this; }
_AFXWIN_INLINE CRect CSize::operator-(const RECT* lpRect) const
	{ return CRect(lpRect) - *this; }

// CPoint
_AFXWIN_INLINE CPoint::CPoint()
	{ /* random filled */ }
_AFXWIN_INLINE CPoint::CPoint(int initX, int initY)
	{ x = initX; y = initY; }
#if !defined(_AFX_CORE_IMPL) || !defined(_AFXDLL) || defined(_DEBUG)
_AFXWIN_INLINE CPoint::CPoint(POINT initPt)
	{ *(POINT*)this = initPt; }
#endif
_AFXWIN_INLINE CPoint::CPoint(SIZE initSize)
	{ *(SIZE*)this = initSize; }
_AFXWIN_INLINE CPoint::CPoint(DWORD dwPoint)
	{
		x = (short)LOWORD(dwPoint);
		y = (short)HIWORD(dwPoint);
	}
_AFXWIN_INLINE void CPoint::Offset(int xOffset, int yOffset)
	{ x += xOffset; y += yOffset; }
_AFXWIN_INLINE void CPoint::Offset(POINT point)
	{ x += point.x; y += point.y; }
_AFXWIN_INLINE void CPoint::Offset(SIZE size)
	{ x += size.cx; y += size.cy; }
_AFXWIN_INLINE BOOL CPoint::operator==(POINT point) const
	{ return (x == point.x && y == point.y); }
_AFXWIN_INLINE BOOL CPoint::operator!=(POINT point) const
	{ return (x != point.x || y != point.y); }
_AFXWIN_INLINE void CPoint::operator+=(SIZE size)
	{ x += size.cx; y += size.cy; }
_AFXWIN_INLINE void CPoint::operator-=(SIZE size)
	{ x -= size.cx; y -= size.cy; }
_AFXWIN_INLINE void CPoint::operator+=(POINT point)
	{ x += point.x; y += point.y; }
_AFXWIN_INLINE void CPoint::operator-=(POINT point)
	{ x -= point.x; y -= point.y; }
_AFXWIN_INLINE CPoint CPoint::operator+(SIZE size) const
	{ return CPoint(x + size.cx, y + size.cy); }
_AFXWIN_INLINE CPoint CPoint::operator-(SIZE size) const
	{ return CPoint(x - size.cx, y - size.cy); }
_AFXWIN_INLINE CPoint CPoint::operator-() const
	{ return CPoint(-x, -y); }
_AFXWIN_INLINE CPoint CPoint::operator+(POINT point) const
	{ return CPoint(x + point.x, y + point.y); }
_AFXWIN_INLINE CSize CPoint::operator-(POINT point) const
	{ return CSize(x - point.x, y - point.y); }
_AFXWIN_INLINE CRect CPoint::operator+(const RECT* lpRect) const
	{ return CRect(lpRect) + *this; }
_AFXWIN_INLINE CRect CPoint::operator-(const RECT* lpRect) const
	{ return CRect(lpRect) - *this; }

// CRect
_AFXWIN_INLINE CRect::CRect()
	{ /* random filled */ }
_AFXWIN_INLINE CRect::CRect(int l, int t, int r, int b)
	{ left = l; top = t; right = r; bottom = b; }
_AFXWIN_INLINE CRect::CRect(const RECT& srcRect)
	{ ::CopyRect(this, &srcRect); }
_AFXWIN_INLINE CRect::CRect(LPCRECT lpSrcRect)
	{ ::CopyRect(this, lpSrcRect); }
_AFXWIN_INLINE CRect::CRect(POINT point, SIZE size)
	{ right = (left = point.x) + size.cx; bottom = (top = point.y) + size.cy; }
_AFXWIN_INLINE CRect::CRect(POINT topLeft, POINT bottomRight)
	{ left = topLeft.x; top = topLeft.y;
		right = bottomRight.x; bottom = bottomRight.y; }
_AFXWIN_INLINE int CRect::Width() const
	{ return right - left; }
_AFXWIN_INLINE int CRect::Height() const
	{ return bottom - top; }
_AFXWIN_INLINE CSize CRect::Size() const
	{ return CSize(right - left, bottom - top); }
_AFXWIN_INLINE CPoint& CRect::TopLeft()
	{ return *((CPoint*)this); }
_AFXWIN_INLINE CPoint& CRect::BottomRight()
	{ return *((CPoint*)this+1); }
_AFXWIN_INLINE const CPoint& CRect::TopLeft() const
	{ return *((CPoint*)this); }
_AFXWIN_INLINE const CPoint& CRect::BottomRight() const
	{ return *((CPoint*)this+1); }
_AFXWIN_INLINE CPoint CRect::CenterPoint() const
	{ return CPoint((left+right)/2, (top+bottom)/2); }
_AFXWIN_INLINE void CRect::SwapLeftRight()
	{ SwapLeftRight(LPRECT(this)); }
_AFXWIN_INLINE void CRect::SwapLeftRight(LPRECT lpRect)
	{ LONG temp = lpRect->left; lpRect->left = lpRect->right; lpRect->right = temp; }
_AFXWIN_INLINE CRect::operator LPRECT()
	{ return this; }
_AFXWIN_INLINE CRect::operator LPCRECT() const
	{ return this; }
_AFXWIN_INLINE BOOL CRect::IsRectEmpty() const
	{ return ::IsRectEmpty(this); }
_AFXWIN_INLINE BOOL CRect::IsRectNull() const
	{ return (left == 0 && right == 0 && top == 0 && bottom == 0); }
_AFXWIN_INLINE BOOL CRect::PtInRect(POINT point) const
	{ return ::PtInRect(this, point); }
_AFXWIN_INLINE void CRect::SetRect(int x1, int y1, int x2, int y2)
	{ ::SetRect(this, x1, y1, x2, y2); }
_AFXWIN_INLINE void CRect::SetRect(POINT topLeft, POINT bottomRight)
	{ ::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y); }
_AFXWIN_INLINE void CRect::SetRectEmpty()
	{ ::SetRectEmpty(this); }
_AFXWIN_INLINE void CRect::CopyRect(LPCRECT lpSrcRect)
	{ ::CopyRect(this, lpSrcRect); }
_AFXWIN_INLINE BOOL CRect::EqualRect(LPCRECT lpRect) const
	{ return ::EqualRect(this, lpRect); }
_AFXWIN_INLINE void CRect::InflateRect(int x, int y)
	{ ::InflateRect(this, x, y); }
_AFXWIN_INLINE void CRect::InflateRect(SIZE size)
	{ ::InflateRect(this, size.cx, size.cy); }
_AFXWIN_INLINE void CRect::DeflateRect(int x, int y)
	{ ::InflateRect(this, -x, -y); }
_AFXWIN_INLINE void CRect::DeflateRect(SIZE size)
	{ ::InflateRect(this, -size.cx, -size.cy); }
_AFXWIN_INLINE void CRect::OffsetRect(int x, int y)
	{ ::OffsetRect(this, x, y); }
_AFXWIN_INLINE void CRect::OffsetRect(POINT point)
	{ ::OffsetRect(this, point.x, point.y); }
_AFXWIN_INLINE void CRect::OffsetRect(SIZE size)
	{ ::OffsetRect(this, size.cx, size.cy); }
_AFXWIN_INLINE BOOL CRect::IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2)
	{ return ::IntersectRect(this, lpRect1, lpRect2);}
_AFXWIN_INLINE BOOL CRect::UnionRect(LPCRECT lpRect1, LPCRECT lpRect2)
	{ return ::UnionRect(this, lpRect1, lpRect2); }
_AFXWIN_INLINE void CRect::operator=(const RECT& srcRect)
	{ ::CopyRect(this, &srcRect); }
_AFXWIN_INLINE BOOL CRect::operator==(const RECT& rect) const
	{ return ::EqualRect(this, &rect); }
_AFXWIN_INLINE BOOL CRect::operator!=(const RECT& rect) const
	{ return !::EqualRect(this, &rect); }
_AFXWIN_INLINE void CRect::operator+=(POINT point)
	{ ::OffsetRect(this, point.x, point.y); }
_AFXWIN_INLINE void CRect::operator+=(SIZE size)
	{ ::OffsetRect(this, size.cx, size.cy); }
_AFXWIN_INLINE void CRect::operator+=(LPCRECT lpRect)
	{ InflateRect(lpRect); }
_AFXWIN_INLINE void CRect::operator-=(POINT point)
	{ ::OffsetRect(this, -point.x, -point.y); }
_AFXWIN_INLINE void CRect::operator-=(SIZE size)
	{ ::OffsetRect(this, -size.cx, -size.cy); }
_AFXWIN_INLINE void CRect::operator-=(LPCRECT lpRect)
	{ DeflateRect(lpRect); }
_AFXWIN_INLINE void CRect::operator&=(const RECT& rect)
	{ ::IntersectRect(this, this, &rect); }
_AFXWIN_INLINE void CRect::operator|=(const RECT& rect)
	{ ::UnionRect(this, this, &rect); }
_AFXWIN_INLINE CRect CRect::operator+(POINT pt) const
	{ CRect rect(*this); ::OffsetRect(&rect, pt.x, pt.y); return rect; }
_AFXWIN_INLINE CRect CRect::operator-(POINT pt) const
	{ CRect rect(*this); ::OffsetRect(&rect, -pt.x, -pt.y); return rect; }
_AFXWIN_INLINE CRect CRect::operator+(SIZE size) const
	{ CRect rect(*this); ::OffsetRect(&rect, size.cx, size.cy); return rect; }
_AFXWIN_INLINE CRect CRect::operator-(SIZE size) const
	{ CRect rect(*this); ::OffsetRect(&rect, -size.cx, -size.cy); return rect; }
_AFXWIN_INLINE CRect CRect::operator+(LPCRECT lpRect) const
	{ CRect rect(this); rect.InflateRect(lpRect); return rect; }
_AFXWIN_INLINE CRect CRect::operator-(LPCRECT lpRect) const
	{ CRect rect(this); rect.DeflateRect(lpRect); return rect; }
_AFXWIN_INLINE CRect CRect::operator&(const RECT& rect2) const
	{ CRect rect; ::IntersectRect(&rect, this, &rect2);
		return rect; }
_AFXWIN_INLINE CRect CRect::operator|(const RECT& rect2) const
	{ CRect rect; ::UnionRect(&rect, this, &rect2);
		return rect; }
_AFXWIN_INLINE BOOL CRect::SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2)
	{ return ::SubtractRect(this, lpRectSrc1, lpRectSrc2); }
