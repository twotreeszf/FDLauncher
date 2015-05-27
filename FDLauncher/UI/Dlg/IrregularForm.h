/* -------------------------------------------------------------------------
// 文件名		:	IrregularShapeDlg.h
// 创建人		:	李树松
// 创建时间	:	2012-12-17
// 功能描述	:	不规则形状窗口基类
//
// -----------------------------------------------------------------------*/
#ifndef __IRREGULARSHAPEWND_H__
#define __IRREGULARSHAPEWND_H__

#include "..\FormUI.h"

class CIrregularForm : public CFormUI
{
public:
	CIrregularForm(bool bSupportEsc = true);
	virtual ~CIrregularForm();

	//////////////////////////////////////////////////////////////////////////
	// message Handler	
	BEGIN_MSG_MAP(CIrregularForm)		
		MESSAGE_HANDLER(WM_PAINT,		OnPaint)
		MESSAGE_HANDLER(WM_SHOWWINDOW,	OnShowWindow)
		CHAIN_MSG_MAP(CFormUI)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void DoPaint();

protected:
	// override
	virtual void OnInit();
};


#endif		// __IRREGULARSHAPEWND_H__
