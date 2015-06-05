/* -------------------------------------------------------------------------
// 文件名		:	IrregularShapeDlg.cpp
// 创建人		:	李树松
// 创建时间	:	2012-12-17
// 功能描述	:	不规则形状窗口基类
//
// -----------------------------------------------------------------------*/
#include "stdafx.h"
#include "IrregularForm.h"
#include "../Ctrl/ShadowWnd.h"

CIrregularForm::CIrregularForm(bool bSupportEsc)
	: CFormUI(bSupportEsc)
{

}

CIrregularForm::~CIrregularForm()
{

}

void CIrregularForm::OnInit()
{
	ModifyStyleEx(WS_THICKFRAME, WS_EX_LAYERED);
}

LRESULT CIrregularForm::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DoPaint();
	return 0;
}

LRESULT CIrregularForm::OnShowWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	DoPaint();
	return 0;
}

void CIrregularForm::DoPaint()
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