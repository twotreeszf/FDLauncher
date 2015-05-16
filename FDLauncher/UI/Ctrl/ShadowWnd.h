/* -------------------------------------------------------------------------
//	created:	2012-6-20 10:30:18	
//	filename:	ShadowWnd.cpp
//	author:		zhouchuanjie@kingsoft.com
//	purpose:	
//
// -----------------------------------------------------------------------*/

#ifndef __SHADOWWND_H__
#define __SHADOWWND_H__

inline HBITMAP CreateMyBitmap(HDC hDC, int cx, int cy, COLORREF** pBits)
{
	LPBITMAPINFO lpbiSrc = NULL;
	lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiSrc == NULL) return NULL;

	lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbiSrc->bmiHeader.biWidth = cx;
	lpbiSrc->bmiHeader.biHeight = cy;
	lpbiSrc->bmiHeader.biPlanes = 1;
	lpbiSrc->bmiHeader.biBitCount = 32;
	lpbiSrc->bmiHeader.biCompression = BI_RGB;
	lpbiSrc->bmiHeader.biSizeImage = cx * cy;
	lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
	lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
	lpbiSrc->bmiHeader.biClrUsed = 0;
	lpbiSrc->bmiHeader.biClrImportant = 0;

	HBITMAP hBitmap = CreateDIBSection (hDC, lpbiSrc, DIB_RGB_COLORS, (void **)pBits, NULL, NULL);
	delete [] lpbiSrc;
	return hBitmap;
}

class CShadowWnd : public CWindowWnd
{
private:
	bool m_bNeedUpdate;
	bool m_bParentActive;
	CPaintManagerUI m_pm;

public:
	CShadowWnd() 
	{ 
		m_bParentActive = true; m_bNeedUpdate = true;
	}

	LPCTSTR GetWindowClassName() const { return _T("UIShadow"); };
	UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME; };
	void OnFinalMessage(HWND /*hWnd*/) { /*delete this*/; };
	void ParentActive(bool bActive) { m_bParentActive = bActive; m_bNeedUpdate  = true; }

	void RePaint() 
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
		HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hDcBackground, hbmpBackground);
		
		if(m_bParentActive)
		{
			CRenderEngine::DrawImageString(hDcBackground, &m_pm, rcClient, rcClient, _T("file='common\\bk_shadow.png' corner='8,8,8,8'"), NULL);			
		}
		else
		{
			CRenderEngine::DrawImageString(hDcBackground, &m_pm, rcClient, rcClient, _T("file='common\\bk_shadow.png' fade='180' corner='8,8,8,8'"), NULL);
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

		m_bNeedUpdate = false;
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if( uMsg == WM_CREATE ) 
		{
			m_pm.Init(m_hWnd, _Module.GetResource());
			SetTimer(m_hWnd, 9000, 10, NULL);
		}
		else if( uMsg == WM_SIZE ) m_bNeedUpdate = true;
		else if( uMsg == WM_CLOSE ) KillTimer(m_hWnd, 9000);
		else if( uMsg == WM_TIMER ) {
			if( LOWORD(wParam) == 9000 && m_bNeedUpdate == true) {
				if( !::IsIconic(m_hWnd) ) RePaint();
			}
		}

		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
};


#endif	/*__ShadowWnd_h__*/