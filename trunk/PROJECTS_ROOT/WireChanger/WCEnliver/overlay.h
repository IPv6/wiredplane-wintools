// Copyright (c) 2002 Andreas Jönsson, www.AngelCode.com

#ifndef OVERLAY_H
#define OVERLAY_H

// Choose a color that are unlikely to be used by any window,
// also choose a color that can be represented in 16 bit mode 
// as well as 32 bit.
#define COLORKEY   RGB(8,0,16)

int  InitOverlay();
void UninitOverlay();
int  UpdateOverlay();

#endif