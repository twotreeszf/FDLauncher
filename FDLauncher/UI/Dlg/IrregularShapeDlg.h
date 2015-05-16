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

class CIrregularShapeDlg : public CAnimatePanelUI
{

private:
	RECT   m_rctTransparent;

public:
	CIrregularShapeDlg(RECT rctTransparent, DWORD dwAWHideFlag = AW_VER_NEGATIVE | AW_HIDE, DWORD dwAWShowFlag = AW_VER_POSITIVE);
	virtual ~CIrregularShapeDlg();

	//////////////////////////////////////////////////////////////////////////
	// message Handler	
	BEGIN_MSG_MAP(CIrregularShapeDlg)		
		MESSAGE_HANDLER(WM_PAINT,		OnPaint)
		MESSAGE_HANDLER(WM_SHOWWINDOW,	OnShowWindow)
		CHAIN_MSG_MAP(CAnimatePanelUI)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void DoPaint();

protected:
	// override
	virtual void OnInit();

private:
	void _DrawMessageTipBackground(COLORREF* pBackgroundBits, DWORD dwHeight, DWORD dwWidth);
	void _DrawBridgePanelBackground(COLORREF* pBackgroundBits, DWORD dwHeight, DWORD dwWidth);
};


#endif		// __IRREGULARSHAPEWND_H__
