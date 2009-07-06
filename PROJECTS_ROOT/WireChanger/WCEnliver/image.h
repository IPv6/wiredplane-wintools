// Copyright (c) 2002 Andreas Jönsson, www.AngelCode.com

#ifndef IMAGE_H
#define IMAGE_H

// We only need a low-resolution image as the overlay 
// is stretched to fill the screen by the hardware
void UpdateImage();
BOOL ClearBitmap(HBITMAP& hBitmap);
CSize GetBitmapSize(HBITMAP pBmp);
void InitImage(BOOL bFully=TRUE);
#endif