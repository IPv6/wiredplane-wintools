#ifndef __CUSTOMMENUSH__
#define __CUSTOMMENUSH__

#ifndef USE_MFC7_HTMLVIEW_FEATURES
#include "Custsite.h"
#endif

enum WebContextMenuMode
{
	kDefaultMenuSupport = 0,
	kNoContextMenu,
	kTextSelectionOnly,
	kAllowAllButViewSource,
	kCustomMenuSupport,
	kWebContextMenuModeLimit
};

HRESULT CustomShowContextMenu(UINT mode, DWORD dwID, POINT *pptPosition,
					IUnknown *pCommandTarget, IDispatch *pDispatchObjectHit);

#endif
