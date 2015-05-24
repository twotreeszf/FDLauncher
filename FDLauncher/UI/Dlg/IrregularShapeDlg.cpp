/* -------------------------------------------------------------------------
// 文件名		:	IrregularShapeDlg.cpp
// 创建人		:	李树松
// 创建时间	:	2012-12-17
// 功能描述	:	不规则形状窗口基类
//
// -----------------------------------------------------------------------*/
#include "stdafx.h"
#include "IrregularShapeDlg.h"
#include "../Ctrl/ShadowWnd.h"

CIrregularShapeDlg::CIrregularShapeDlg(RECT rctTransparent, DWORD dwAWHideFlag, DWORD dwAWShowFlag) 
	: CAnimatePanelUI(dwAWHideFlag, dwAWShowFlag),
	m_rctTransparent(rctTransparent)
{

}

CIrregularShapeDlg::~CIrregularShapeDlg()
{

}

void CIrregularShapeDlg::OnInit()
{
	ModifyStyleEx(0, WS_EX_LAYERED);
}

LRESULT CIrregularShapeDlg::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DoPaint();
	return 0;
}

LRESULT CIrregularShapeDlg::OnShowWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	DoPaint();
	return 0;
}

void CIrregularShapeDlg::DoPaint()
{
	RECT rcClient = { 0 };
	::GetClientRect(m_hWnd, &rcClient);
	DWORD dwWidth = rcClient.right - rcClient.left;
	DWORD dwHeight = rcClient.bottom - rcClient.top;

	HDC hDcPaint = ::GetDC(m_hWnd);
	HDC hDcBackground = ::CreateCompatibleDC(hDcPaint);
	COLORREF* pBackgroundBits;
	HBITMAP hbmpBackground = CreateMyBitmap(hDcPaint, dwWidth, dwHeight, &pBackgroundBits);
	::ZeroMemory(pBackgroundBits, dwWidth * dwHeight * 4);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDcBackground, hbmpBackground);

	m_paintManager.GetRoot()->SetPos(rcClient);
	m_paintManager.GetRoot()->DoPaint(hDcBackground, rcClient);

	//CRenderEngine::DrawImageString(hDcBackground, &m_pm, rcClient, rcClient, _T("file='guide\\1.png' "), NULL);			
	
	for (int i = m_rctTransparent.top; i <= dwHeight - m_rctTransparent.bottom; i++)
	{
		unsigned int* pSrc = (unsigned int*)pBackgroundBits + (dwHeight - i) * dwWidth; //32位直接乘长度
		for(int j = m_rctTransparent.left;j < dwWidth - m_rctTransparent.right;j++)
		{
			pSrc[j] |= 0xFF000000;
		}
	}

	RECT rcWnd = { 0 };
	::GetWindowRect(m_hWnd, &rcWnd);

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	POINT ptPos   = { rcWnd.left, rcWnd.top };
	SIZE sizeWnd  = { dwWidth, dwHeight };
	POINT ptSrc   = { 0, 0 };
	UpdateLayeredWindow(m_hWnd, hDcPaint, &ptPos, &sizeWnd, hDcBackground, &ptSrc, 0, &bf, ULW_ALPHA);

	::SelectObject(hDcBackground, hOldBitmap);
	if( hDcBackground != NULL ) ::DeleteDC(hDcBackground);
	if( hbmpBackground != NULL ) ::DeleteObject(hbmpBackground);
	::ReleaseDC(m_hWnd, hDcPaint);
}

void CIrregularShapeDlg::_DrawMessageTipBackground(COLORREF* pBackgroundBits, DWORD dwHeight, DWORD dwWidth)
{
	ASSERT(pBackgroundBits);
	for (int i = 1;i <= dwHeight - 10; i++)
	{
		unsigned int* pSrc = (unsigned int*)pBackgroundBits + (dwHeight - i) * dwWidth; //32位直接乘长度
		for(int j = 2;j < dwWidth - 2;j++)
		{
			pSrc[j] |= 0xFF000000;
		}
	}
}

