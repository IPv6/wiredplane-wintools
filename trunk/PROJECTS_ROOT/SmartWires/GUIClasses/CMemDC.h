#ifndef _MEMDC_H_
#define _MEMDC_H_


////////////////////////////////////////////////////////////////////////////////
// CMemDC - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-2002, Keith Rule
//
// You may freely use or modify this code provided this
// copyright is included in all derived versions.
//
// History -	10/03/97	Fixed scrolling bug.
//							Added print support. - KR
//
//				11/03/99	Fixed most common complaint. Added
//							background color fill. - KR
//
//				11/03/99	Added support for mapping modes other than
//							MM_TEXT as suggested by Lee Sang Hun. - KR
//
//				02/11/02	Added support for CScrollView as supplied
//							by Gary Kirkham. - KR
//
//				03/03/06	Added save/restore of a bunch of GDI stuff - AGB
//
// This class implements a memory device context which allows
// flicker free drawing.
////////////////////////////////////////////////////////////////////////////////


class CMemDC : public CDC
{
	public:

		// Constructor sets up the memory DC
		CMemDC(CDC* pDC, const CRect* pRect = NULL) : CDC()
		{
			// make sure the call provided a valid device context
			ASSERT(pDC != NULL);

			// save the original device context for later BitBlt()
			m_pDC = pDC;

			// check whether we're printing
			m_bMemDC = !pDC->IsPrinting();

			// get the output rectangle
			if (pRect == NULL)
				pDC->GetClipBox(&m_rect);
			else
				m_rect = *pRect;

			if (m_bMemDC)
			{
				// do the memory DC stuff if not printing

				// create a memory DC
				CreateCompatibleDC(pDC);

				// create new bitmap and save old bitmap
				pDC->LPtoDP(&m_rect);
				m_MemBitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
				m_pOldBitmap = SelectObject(&m_MemBitmap);
				SetMapMode(pDC->GetMapMode());
				SetWindowExt(pDC->GetWindowExt());
				SetViewportExt(pDC->GetViewportExt());
				pDC->DPtoLP(&m_rect);

				// save old background brush
				m_pOldBrush = GetCurrentBrush();

				// save old font
				m_pOldFont = GetCurrentFont();

				// save old background color
				m_crOldBkColor = GetBkColor();

				// save old text color
				m_crOldTextColor = GetTextColor();

				// save old pen
				m_pOldPen = GetCurrentPen();

				// save old background mode
				m_nOldBkMode = GetBkMode();

				// save old window origin and set new one
				m_ptOldOrigin = SetWindowOrg(m_rect.left, m_rect.top);
			}
			else
			{
				// make a copy of the relevant parts of the original DC for printing
				m_bPrinting = pDC->m_bPrinting;
				m_hDC		= pDC->m_hDC;
				m_hAttribDC = pDC->m_hAttribDC;
			}

			// fill background 
			FillSolidRect(m_rect, m_crOldBkColor);
		}

		// Destructor copies the contents of the memory DC to the original DC
		~CMemDC()
		{
			if (m_bMemDC)
			{
				// copy the offscreen bitmap onto the screen
				m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
							  this, m_rect.left, m_rect.top, SRCCOPY);

				// restore the original DC settings
				SelectObject(m_pOldBitmap);
				SelectObject(m_pOldBrush);
				SelectObject(m_pOldFont);
				SetBkColor(m_crOldBkColor);
				SetTextColor(m_crOldTextColor);
				SelectObject(m_pOldPen);
				SetBkMode(m_nOldBkMode);
				SetWindowOrg(m_ptOldOrigin);
			}
			else
			{
				// all we need to do is replace the DC with an illegal value,
				// this keeps us from accidently deleting the handles associated
				// with the CDC that was passed to the constructor
				m_hDC = m_hAttribDC = NULL;
			}
		}

		// Allow usage as a pointer
		CMemDC* operator->() { return this; }

		// Allow usage as a pointer
		operator CMemDC*() { return this; }

	private:
		BOOL		m_bMemDC;					// TRUE if not printing
		CBitmap		m_MemBitmap;				// offscreen bitmap
		CDC*		m_pDC;						// saves CDC passed in constructor
		CRect		m_rect;						// rectangle of drawing area

		// GDI stuff to be saved and restored
		CBitmap*	m_pOldBitmap;				// original bitmap
		CBrush*		m_pOldBrush;				// original brush
		CFont*		m_pOldFont;					// original font
		COLORREF	m_crOldBkColor;				// original background color
		COLORREF	m_crOldTextColor;			// original text color
		CPen*		m_pOldPen;					// original pen
		CPoint		m_ptOldOrigin;				// original origin
		int			m_nOldBkMode;				// original background mode
};

#endif											//#ifndef _MEMDC_H_
