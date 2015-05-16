#include "StdAfx.h"
#include "ImageEx.h"
///////////////////////////////////////////////////////////////////////////////////////
DECLARE_HANDLE(HZIP);	// An HZIP identifies a zip file that has been opened
typedef DWORD ZRESULT;
typedef struct
{ 
	int index;                 // index of this file within the zip
	char name[MAX_PATH];       // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime,ctime,mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRY;
typedef struct
{ 
	int index;                 // index of this file within the zip
	TCHAR name[MAX_PATH];      // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime,ctime,mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRYW;
#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern HZIP OpenZipU(void *z,unsigned int len,DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);
#ifdef _UNICODE
#define ZIPENTRY ZIPENTRYW
#define GetZipItem GetZipItemW
#define FindZipItem FindZipItemW
#else
#define GetZipItem GetZipItemA
#define FindZipItem FindZipItemA
#endif
extern ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY *ze);
extern ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW *ze);
extern ZRESULT FindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
extern ZRESULT FindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
extern ZRESULT UnzipItem(HZIP hz, int index, void *dst, unsigned int len, DWORD flags);

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

class ImageEx;
class CImageTurnerUI;

class CImageWnd : public CWindowWnd
{
public:
	CImageWnd();
	HWND Init(CImageTurnerUI* pOwner, LPCTSTR pStrImage);

	LPCTSTR GetWindowClassName() const;
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void CreateImage(LPCTSTR pStrImage);

protected:
	CImageTurnerUI*		m_pOwner;
	ImageEx*			m_image;
	bool				m_bStart;
	CRect				m_rcWndPos;
};

CImageWnd::CImageWnd() : m_pOwner(NULL), m_image(NULL), m_bStart(false)
{
}

HWND CImageWnd::Init(CImageTurnerUI* pOwner, LPCTSTR pStrImage)
{
	m_pOwner = pOwner;
	CreateImage(pStrImage);
	CRect m_rcWndPos = m_pOwner->GetPos();
	CRect rcPadding = m_pOwner->GetPadding();
	m_rcWndPos.left = m_rcWndPos.left + rcPadding.left;
	m_rcWndPos.top = m_rcWndPos.top + rcPadding.top;
	m_rcWndPos.right = min(m_rcWndPos.left + m_image->GetWidth(), m_rcWndPos.right - rcPadding.right);
	m_rcWndPos.bottom = min(m_rcWndPos.top + m_image->GetHeight(), m_rcWndPos.bottom - rcPadding.bottom);
	UINT uStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	Create(m_pOwner->GetManager()->GetPaintWindow(), _T("UIImage"), uStyle, 0L, m_rcWndPos, NULL);
	::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);

	return m_hWnd;
}

LPCTSTR CImageWnd::GetWindowClassName() const
{
	return _T("ImageWnd");
}

void CImageWnd::OnFinalMessage(HWND hWnd)
{
	m_pOwner->m_pWindow = NULL;
	delete m_image;
	delete this;
}

LRESULT CImageWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes=0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
	case WM_PAINT:			lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_ENABLE:			lRes = OnEnable(uMsg, wParam, lParam, bHandled); break;
	default:
		bHandled = FALSE;
	}
	if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CImageWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	::BeginPaint(m_hWnd, &ps);
	if (!m_bStart && m_image != NULL)
	{
		m_bStart = true;
		m_image->InitAnimation(m_hWnd);
	}
	::EndPaint(m_hWnd, &ps);
	return 1;
}

LRESULT CImageWnd::OnEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_image != NULL)
		m_image->SetPause(bool(wParam));
	return 1;
}

void CImageWnd::CreateImage(LPCTSTR pStrImage)
{
	if (m_image == NULL)
	{
		CResourceUI* pResource = m_pOwner->GetManager()->GetResource();
		CStdString sFile = pResource->GetResourcePath();
		if( pResource->GetResourceZip().IsEmpty() ) {
			sFile += pStrImage;
			m_image = new ImageEx(sFile);
		}
		else {
			sFile += pResource->GetResourceZip();
			HZIP hz = NULL;
			if( pResource->IsCachedResourceZip() ) hz = (HZIP)pResource->GetResourceZipHandle();
			else hz = OpenZip((void*)sFile.GetData(), 0, 2);
			if( hz == NULL ) return;
			ZIPENTRY ze; 
			int i; 
			if( FindZipItem(hz, pStrImage, true, &i, &ze) != 0 ) return;
			DWORD dwSize = ze.unc_size;
			if( dwSize == 0 ) return;
			BYTE* pData = new BYTE[ dwSize ];
			int res = UnzipItem(hz, i, pData, dwSize, 3);
			if( res != 0x00000000 && res != 0x00000600) {
				delete[] pData;
				if( !pResource->IsCachedResourceZip() ) CloseZip(hz);
				return;
			}
			if( !pResource->IsCachedResourceZip() ) CloseZip(hz);

			if(pData)
			{
				m_image = new ImageEx(pData, dwSize);
				delete [] pData;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////
//
//
CImageTurnerUI::CImageTurnerUI() : m_pWindow(NULL)
{
	// GDI+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

CImageTurnerUI::~CImageTurnerUI()
{
	// GDI+
	GdiplusShutdown(m_gdiplusToken);
}

void CImageTurnerUI::SetImage(LPCTSTR pStrImage)
{
	m_sImage = pStrImage;
}

LPCTSTR CImageTurnerUI::GetClass() const
{
	return _T("ImageTurnerUI");
}

LPVOID CImageTurnerUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("ImageTurner")) == 0 ) return static_cast<CImageTurnerUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void CImageTurnerUI::DoInit()
{
	
}

void CImageTurnerUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	CControlUI::DoPaint(hDC, rcPaint);

	if (m_pWindow == NULL)
	{
		m_pWindow = new CImageWnd();
		ASSERT(m_pWindow);
		m_pWindow->Init(this, m_sImage);
	}
}

void CImageTurnerUI::SetVisible(bool bVisible)
{
	CControlUI::SetVisible(bVisible);
	if(m_pWindow != NULL && ::IsWindow(m_pWindow->GetHWND())) 
		::ShowWindow(m_pWindow->GetHWND(), IsVisible() ? SW_SHOW : SW_HIDE);
}

void CImageTurnerUI::SetInternVisible(bool bVisible)
{
	CControlUI::SetInternVisible(bVisible);
	if(m_pWindow != NULL && ::IsWindow(m_pWindow->GetHWND()))
		::ShowWindow(m_pWindow->GetHWND(), IsVisible() ? SW_SHOW : SW_HIDE);
}

void CImageTurnerUI::DoEvent(TEventUI& event)
{
	CControlUI::DoEvent(event);
}

SIZE CImageTurnerUI::EstimateSize(SIZE szAvailable)
{
	return CControlUI::EstimateSize(szAvailable);
}

void CImageTurnerUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("image")) == 0 ) SetImage(pstrValue);
	else CControlUI::SetAttribute(pstrName, pstrValue);
}

} // namespace DuiLib