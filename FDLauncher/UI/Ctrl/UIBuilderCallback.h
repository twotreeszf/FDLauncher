#ifndef __KBUILDERCALLBACK_H__
#define __KBUILDERCALLBACK_H__

#include "UIDlgBuilder.h"

class UIBuilderCallback : public IDialogBuilderCallback
{
public:
	UIBuilderCallback();
	CControlUI* CreateControl(LPCTSTR pstrClass);

};

#endif	/*__KBuilderCallback_h__*/