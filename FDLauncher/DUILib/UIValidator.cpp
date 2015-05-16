#include "StdAfx.h"
#include "UIValidator.h"
#include <regex>

namespace DuiLib{


//////////////////////////////////////////////////////////////////////////
LPCTSTR CRequiredValidatorUI::GetClass() const
{
	return _T("RequiredValidatorUI");
}

LPVOID CRequiredValidatorUI::GetInterface( LPCTSTR pstrName )
{
	if(_tcscmp(pstrName, _T("IValidatorElementUI")) == 0)
		return static_cast<IValidatorElementUI*>(this);
	if(_tcscmp(pstrName, _T("RequiredValidatorUI")) == 0)
		return static_cast<CRequiredValidatorUI*>(this);
	else 
		return CTextUI::GetInterface(pstrName);
}

bool CRequiredValidatorUI::Validate( CControlUI* pControl )
{
	return !pControl->GetText().IsEmpty();
}

//////////////////////////////////////////////////////////////////////////
CLengthValidatorUI::CLengthValidatorUI()
	: m_nMinLength(0), m_nMaxLength(INT_MAX), m_bUnicode(true)
{

}

LPCTSTR CLengthValidatorUI::GetClass() const
{
	return _T("LengthValidatorUI");
}

LPVOID CLengthValidatorUI::GetInterface( LPCTSTR pstrName )
{
	if(_tcscmp(pstrName, _T("IValidatorElementUI")) == 0)
		return static_cast<IValidatorElementUI*>(this);
	if(_tcscmp(pstrName, _T("LengthValidatorUI")) == 0)
		return static_cast<CLengthValidatorUI*>(this);
	else 
		return CTextUI::GetInterface(pstrName);
}

void CLengthValidatorUI::SetMinLength( int value )
{
	m_nMinLength = value;
}

int CLengthValidatorUI::GetMinLength() const
{
	return m_nMinLength;
}

void CLengthValidatorUI::SetMaxLength( int value )
{
	m_nMaxLength = value;
}

int CLengthValidatorUI::GetMaxLength() const
{
	return m_nMaxLength;
}

void CLengthValidatorUI::SetUnicode(bool value)
{
	m_bUnicode = value;
}

bool CLengthValidatorUI::GetUnicode()
{
	return m_bUnicode;
}

void CLengthValidatorUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	if(_tcscmp(pstrName, _T("minlen")) == 0)
	{		
		SetMinLength(_tstoi(pstrValue));
	}
	else if(_tcscmp(pstrName, _T("maxlen")) == 0)
	{
		SetMaxLength(_tstoi(pstrValue));
	}
	else if(_tcscmp(pstrName, _T("unicode")) == 0)
	{
		SetUnicode(_tcscmp(pstrValue, _T("true")) == 0);
	}
	else CTextUI::SetAttribute(pstrName, pstrValue);
}

bool CLengthValidatorUI::Validate( CControlUI* pControl )
{
	if (m_bUnicode)
	{
		int nLen = pControl->GetText().GetLength();
		return  nLen >= m_nMinLength && nLen <= m_nMaxLength;
	}
	else
	{
		LPCTSTR strText = pControl->GetText().GetData();
		int nLenUnicode = lstrlenW( strText );
		int nLen = WideCharToMultiByte( CP_ACP, 0, strText, nLenUnicode, NULL, 0, NULL, NULL ); 
		return  nLen >= m_nMinLength && nLen <= m_nMaxLength;
	}
}

//////////////////////////////////////////////////////////////////////////
CRegexValidatorUI::CRegexValidatorUI()
	:m_bIgnoreCase(true)
{

}

LPCTSTR CRegexValidatorUI::GetClass() const
{
	return _T("RegexValidatorUI");
}

LPVOID CRegexValidatorUI::GetInterface( LPCTSTR pstrName )
{
	if(_tcscmp(pstrName, _T("IValidatorElementUI")) == 0)
		return static_cast<IValidatorElementUI*>(this);
	if(_tcscmp(pstrName, _T("RegexValidatorUI")) == 0)
		return static_cast<CRegexValidatorUI*>(this);
	else 
		return CTextUI::GetInterface(pstrName);
}

void CRegexValidatorUI::SetExpression( LPCTSTR pstrExpr )
{
	m_sExpression = pstrExpr;
}

LPCTSTR CRegexValidatorUI::GetExpression() const
{
	return m_sExpression;
}

void CRegexValidatorUI::SetIgnoreCase( bool value )
{
	m_bIgnoreCase = value;
}

bool CRegexValidatorUI::GetIgnoreCase() const
{
	return m_bIgnoreCase;
}

void CRegexValidatorUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	if(_tcscmp(pstrName, _T("expression")) == 0)SetExpression(pstrValue);
	else if(_tcscmp(pstrName, _T("icase")) == 0){
		if(_tcscmp(pstrValue, _T("true")) == 0)SetIgnoreCase(true);
		else SetIgnoreCase(false);
	}
	else CTextUI::SetAttribute(pstrName, pstrValue);
}

bool CRegexValidatorUI::Validate( CControlUI* pControl )
{
	typedef std::tr1::basic_regex<TCHAR> _tregex;
	typedef std::basic_string<TCHAR>     _tstring;
	if(m_sExpression.IsEmpty())
		return true;

	_tregex::flag_type flag = _tregex::ECMAScript;
	if(m_bIgnoreCase) flag |= std::regex_constants::icase;
	_tregex reg(m_sExpression, flag);	
	return std::tr1::regex_match(_tstring(pControl->GetText()), reg);
}
//////////////////////////////////////////////////////////////////////////
CRangeValidatorUI::CRangeValidatorUI()
	:m_nMaxValue(INT_MAX), m_nMinValue(INT_MIN)
{

}

LPCTSTR CRangeValidatorUI::GetClass() const
{
	return _T("RangeValidatorUI");
}

LPVOID CRangeValidatorUI::GetInterface( LPCTSTR pstrName )
{
	if(_tcscmp(pstrName, _T("IValidatorElementUI")) == 0)
		return static_cast<IValidatorElementUI*>(this);
	if(_tcscmp(pstrName, _T("RangeValidatorUI")) == 0)
		return static_cast<CRangeValidatorUI*>(this);
	else 
		return CTextUI::GetInterface(pstrName);
}

void CRangeValidatorUI::SetMaxValue( int value )
{
	m_nMaxValue = value;
}

int CRangeValidatorUI::GetMaxValue() const
{
	return m_nMaxValue;
}

void CRangeValidatorUI::SetMinValue( int value )
{
	m_nMinValue = value;
}

int CRangeValidatorUI::GetMinValue()const
{
	return m_nMinValue;
}

void CRangeValidatorUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	if(_tcscmp(pstrName, _T("maxval")) == 0)SetMaxValue(_tstoi(pstrValue));
	else if(_tcscmp(pstrName, _T("minval")) == 0)SetMinValue(_tstoi(pstrValue));
	else CTextUI::SetAttribute(pstrName, pstrValue);
}

bool CRangeValidatorUI::Validate( CControlUI* pControl )
{
	CStdString sText = pControl->GetText();
	if(sText.IsEmpty())return false;

	double value = _tstof(sText);
	return value >= m_nMinValue && value < m_nMaxValue;
}

//////////////////////////////////////////////////////////////////////////

class CValidatorTipWnd : public CWindowWnd
{
public:
	CValidatorTipWnd();
	~CValidatorTipWnd();
	void Init(CValidatorUI* pOwner, CControlUI* pElement);

	LPCTSTR GetWindowClassName() const;	
	//UINT   GetClassStyle()const{ return /*CS_DROPSHADOW*/0x00020000; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnFinalMessage(HWND hWnd);

	LRESULT OnCreate( WPARAM wParam, LPARAM lParam );

	void SetWindow() ;

	void DoPaint( HDC hDcBackground, RECT rcWnd );

	//HRGN CreateRgn(const RECT& rcWnd,const SIZE& szArrow, const SIZE& szRound);
public:	
	CPaintManagerUI m_pm;
	CValidatorUI* m_pOwner;
	CVerticalLayoutUI* m_pLayout;
	CControlUI*   m_pElement;
};

//////////////////////////////////////////////////////////////////////////

void GetImageRect(const CStdString& sImage,LPCTSTR pstrName ,CRect& rcRect)
{
	int nPos = sImage.Find(pstrName);
	if(nPos >= 0)
	{		
		nPos = sImage.Find(_T("\'"), nPos + 1);
		if(nPos < 0)
		{
			return;
		}

		LPTSTR lpszValue = NULL;
		LPCTSTR lpszImage = sImage;
		rcRect.left   = _tcstol(lpszImage + nPos + 1, &lpszValue, 10);  ASSERT(lpszValue);    
		rcRect.top    = _tcstol(lpszValue + 1, &lpszValue, 10);    ASSERT(lpszValue);    
		rcRect.right  = _tcstol(lpszValue + 1, &lpszValue, 10);  ASSERT(lpszValue);    
		rcRect.bottom = _tcstol(lpszValue + 1, &lpszValue, 10); ASSERT(lpszValue); 		
	}
}

CValidatorTipWnd::CValidatorTipWnd()
{
	m_pOwner   = NULL;
	m_pLayout  = NULL;
	m_pElement = NULL;	
}

CValidatorTipWnd::~CValidatorTipWnd()
{
	
}

void CValidatorTipWnd::Init(CValidatorUI* pOwner, CControlUI* pElement)
{
	m_pOwner   = pOwner;
	m_pElement = pElement;

	CControlUI* pTarget = m_pOwner->GetTargetControl();

	RECT rcTarget = pTarget->GetPos();
	RECT rc = rcTarget;
	RECT rcInset = m_pOwner->GetInset();

	// 计算宽高	
	SIZE szAvailable = { MAX(rc.right - rc.left, pElement->GetFixedWidth()), 9999 };
	szAvailable = m_pElement->EstimateSize(szAvailable);

	szAvailable.cx = MAX(szAvailable.cx, pElement->GetFixedWidth());

	CRect rcArrow,rcCorner;
	CStdString sArrowImage = m_pOwner->GetArrowImage();
	CStdString sBkImage    = m_pOwner->GetBkImage();
	if(!sArrowImage.IsEmpty())
	{
		GetImageRect(sArrowImage, _T("source"), rcArrow);
	}

	if(!sBkImage.IsEmpty())
	{
		GetImageRect(sBkImage, _T("corner"), rcCorner);
	}


	rc.top    = rc.bottom;
	//rc.right  = rc.left + szAvailable.cx + rcInset.left + rcInset.right;
	rc.left   = rc.right -  (szAvailable.cx + rcInset.left + rcInset.right + m_pOwner->GetBorderSize() * 2);
	rc.bottom = rc.top + szAvailable.cy  + rcInset.top + rcInset.bottom + rcArrow.GetHeight() + m_pOwner->GetBorderSize() - rcCorner.top;

	if(rc.left < rcTarget.left)
	{
		rc.right = rcTarget.left + rc.right - rc.left;
		rc.left  = rcTarget.left;
	}

	::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);

	/*MONITORINFO oMonitor = {0};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CRect rcWork = oMonitor.rcWork;
	if( rc.bottom > rcWork.bottom ) {		
		rc.top    = rcTarget.top - (rc.bottom - rc.top);
		rc.bottom = rcTarget.top;
		::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
	}*/

	Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_LAYERED, rc);
	// HACK: Don't deselect the parent's caption
	HWND hWndParent = m_hWnd;
	while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
	::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	//::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);

}


LPCTSTR CValidatorTipWnd::GetWindowClassName()const
{
	return _T("ValidatorTipWnd");
}

LRESULT CValidatorTipWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( uMsg == WM_CREATE ) {
		return OnCreate(wParam, lParam);		
	}
	else if( uMsg == WM_DESTROY || uMsg == WM_CLOSE ) {
		//m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);		
		m_pElement->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
	}
	else if( uMsg == WM_LBUTTONDOWN ) {
		m_pOwner->m_pControl->SetFocus(); //这个会触发Expaire，导致销毁窗体
		//m_pOwner->Expaire();			
		return 0;
	}
	else if( uMsg == WM_SHOWWINDOW){
		if((BOOL)wParam){
			SetWindow();
		}
	}

	LRESULT lRes = 0;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

//HRGN CValidatorTipWnd::CreateRgn(const RECT& rcWnd,const SIZE& szArrow, const SIZE& szRound)
//{
//	HRGN hRgn1 = NULL;		
//	if(szRound.cx != 0 && szRound.cy != 0)
//	{
//		hRgn1 = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRound.cx, szRound.cy);
//	}
//	else
//	{
//		hRgn1 = ::CreateRectRgn(rcWnd.left, rcWnd.top , rcWnd.right, rcWnd.bottom);
//	}
//
//	POINT ptPoly[] = {
//		{rcWnd.left + (rcWnd.right - rcWnd.left) / 2, rcWnd.top - szArrow.cy },
//		{rcWnd.left + (rcWnd.right - rcWnd.left) / 2 - szArrow.cx, rcWnd.top },
//		{rcWnd.left + (rcWnd.right - rcWnd.left) / 2 + szArrow.cx, rcWnd.top }
//	};
//
//	HRGN hRgn2 = CreatePolygonRgn(ptPoly, 3, ALTERNATE);
//	::CombineRgn(hRgn1, hRgn1, hRgn2, RGN_OR);
//	return hRgn1;
//}

LRESULT CValidatorTipWnd::OnCreate( WPARAM wParam, LPARAM lParam )
{
	RECT rcWnd;
	GetClientRect(m_hWnd, &rcWnd);

	CRect rcArrow;
	CStdString sArrowImage = m_pOwner->GetArrowImage();
	if(!sArrowImage.IsEmpty())
	{
		GetImageRect(sArrowImage, _T("source"), rcArrow);
	}	

	m_pm.Init(m_hWnd, m_pOwner->GetManager()->GetResource());

	m_pLayout = new CVerticalLayoutUI;
	m_pm.UseParentResource(m_pOwner->GetManager());
	m_pLayout->SetManager(&m_pm, NULL, true);
	LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("VerticalLayout"));
	if( pDefaultAttributes ) {
		m_pLayout->ApplyAttributeList(pDefaultAttributes);
	}
	RECT rcInset = m_pOwner->GetInset();	
	
	m_pLayout->SetInset(rcInset);	
	m_pLayout->SetBkColor(m_pOwner->GetBkColor());
	m_pLayout->SetBorderColor(m_pOwner->GetBorderColor());
	m_pLayout->SetBorderSize(m_pOwner->GetBorderSize());
	m_pLayout->SetBorderRound(m_pOwner->GetBorderRound());
	m_pLayout->SetBkImage(m_pOwner->GetBkImage());
	m_pLayout->SetAutoDestroy(false);		
	//m_pLayout->ApplyAttributeList(m_pOwner->GetBalloonAttributeList());
	m_pElement->SetInternVisible(true);
	m_pLayout->Add(m_pElement);
	
	m_pm.AttachDialog(m_pLayout);

	RECT rcPos   = rcWnd;
	rcPos.top += rcArrow.GetHeight();
	m_pLayout->SetPos(rcPos);

	SetWindow();

	return 0;
}

void CValidatorTipWnd::SetWindow()
{
	if(m_pLayout == NULL)
		return;

	RECT rcWnd;
	GetClientRect(m_hWnd, &rcWnd);
	
	int cx = rcWnd.right - rcWnd.left;
	int cy = rcWnd.bottom - rcWnd.top;

	BITMAPINFO bmi;
	::ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = cx;
	bmi.bmiHeader.biHeight = cy;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = cx * cy * 4;
	
	LPBYTE pDest = NULL;
	HBITMAP hbmpBackground = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
	if( !hbmpBackground ) return;
	::ZeroMemory(pDest, bmi.bmiHeader.biSizeImage);
	
	HDC hDcPaint = ::GetDC(NULL);
	HDC hDcBackground = ::CreateCompatibleDC(hDcPaint);	
	HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hDcBackground, hbmpBackground);

	DoPaint(hDcBackground, rcWnd);	
	
	// 设置文本区域alpha值
	RECT rcPos = m_pLayout->GetPos();
	RECT rcInset = m_pLayout->GetInset();

	rcPos.left += rcInset.left;
	rcPos.top  += rcInset.top;
	rcPos.right -= rcInset.right;
	rcPos.bottom -= rcInset.bottom;
	for (int i = rcPos.top;i <= rcPos.bottom; i++)
	{
		unsigned int* pSrc = (unsigned int*)pDest + (cy - i) * cx; //32位直接乘长度
		for(int j = rcPos.left;j < rcPos.right;j++)
		{
			pSrc[j] |= 0xFF000000;
		}
	}

	//rcWnd = { 0 };
	::GetWindowRect(m_hWnd, &rcWnd);

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	POINT ptPos   = { rcWnd.left, rcWnd.top };
	SIZE sizeWnd  = { cx, cy };
	POINT ptSrc   = { 0, 0 };
	
	::UpdateLayeredWindow(m_hWnd, hDcPaint, &ptPos, &sizeWnd, hDcBackground, &ptSrc, 0, &bf, ULW_ALPHA);

	::SelectObject(hDcBackground, hOldBitmap);
	if( hDcBackground != NULL ) ::DeleteDC(hDcBackground);
	if( hbmpBackground != NULL ) ::DeleteObject(hbmpBackground);
	::ReleaseDC(NULL, hDcPaint);
}

void CValidatorTipWnd::DoPaint( HDC hDcBackground, RECT rcWnd )
{	
	m_pLayout->DoPaint(hDcBackground, rcWnd);
	
	// 画箭头
	CStdString sArrowImage = m_pOwner->GetArrowImage();
	if(!sArrowImage.IsEmpty())
	{		
		CRect rcCorner, rcArrow;
		GetImageRect(m_pOwner->GetBkImage(), _T("corner") ,rcCorner);
		GetImageRect(sArrowImage, _T("source"), rcArrow);

		RECT rcPaint = {0};		
		rcPaint.left = rcWnd.left + rcArrow.GetWidth();
		rcPaint.top  = rcWnd.top + rcCorner.top;
		rcPaint.bottom = rcPaint.top + rcArrow.GetHeight();
		rcPaint.right = rcPaint.left + rcArrow.GetWidth();
		CRenderEngine::DrawImageString(hDcBackground, &m_pm, rcPaint, rcWnd, sArrowImage);
	}
}



void CValidatorTipWnd::OnFinalMessage(HWND hWnd)
{
	if(m_pOwner)
	{
		m_pOwner->m_pWindow = NULL;
	}
	delete this;
}

//////////////////////////////////////////////////////////////////////////

CValidatorUI::CValidatorUI() : m_pWindow(NULL), m_pControl(NULL), m_bControlIsValid(true)
{		
	m_bImmediate = false;
	m_dwErrorBorderColor = 0xFFFF0000;
	m_nErrorBorderSize   = 1; 
	SetVisible(false);
}

CValidatorUI::~CValidatorUI()
{
	if(m_pManager)m_pManager->RemoveMessageFilter(this);
	if(m_pWindow)
	{		
		::DestroyWindow(m_pWindow->GetHWND());		
	}
}

void CValidatorUI::DoInit()
{	
	m_pManager->AddMessageFilter(this);
	// ASSERT(!m_sControl.IsEmpty());
	if(!m_sControl.IsEmpty())
	{
		m_pControl = m_pManager->FindSubControlByName(m_pManager->GetRoot(), m_sControl);

		ASSERT(m_pControl);
		if(m_pControl)
		{
			m_pControl->OnNotify += DuiLib::MakeDelegate(this, &CValidatorUI::OnControlNotify);
			m_dwCtrlBorderColor = m_pControl->GetBorderColor();
			m_nCtrlBorderSize   = m_pControl->GetBorderSize();
		}
	}	
	// ASSERT(m_pControl || m_sControl);
}

void CValidatorUI::SetManager( CPaintManagerUI* pManager, CControlUI* pParent, bool bInit /*= true*/ ) 
{
	if(m_pManager)m_pManager->RemoveMessageFilter(this);
	CContainerUI::SetManager(pManager, pParent, bInit);
}

void CValidatorUI::SetInternVisible(bool bVisible /*= true*/ )
{
	CContainerUI::SetInternVisible(bVisible);
	this->Expaire();
}

LRESULT CValidatorUI::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled )
{
	bHandled = FALSE;
	if(m_pWindow == NULL) {		
		return 0;
	}
	if(uMsg == WM_SIZE || uMsg == WM_MOVE || uMsg == WM_DESTROY || uMsg == WM_CLOSE)
	{
		this->Expaire();
	}
	return 0;
}

bool CValidatorUI::Validate( bool bShowTip /*= true*/ )
{
	m_bControlIsValid = true;

	if(m_pControl == NULL || !m_pControl->IsEnabled() || GetCount() == 0 || !IsEnabled())return true;

	int nIndex = 0;
	do 
	{
		CControlUI* pControl = GetItemAt(nIndex);
		if(pControl->IsEnabled())
		{
			IValidatorElementUI* pValidator = static_cast<IValidatorElementUI*>(pControl->GetInterface(L"IValidatorElementUI"));
			if(pValidator)
			{
				m_bControlIsValid = pValidator->Validate(m_pControl);
				if(!m_bControlIsValid && bShowTip)
				{						
					this->ShowTip(pControl);
					break;
				}	
			}
		}
	} while (++nIndex < GetCount());
	
	return m_bControlIsValid;
}

bool CValidatorUI::ShowTip( LPCTSTR pstrElementName )
{
	if(m_pWindow != NULL)
		this->Expaire();

	CControlUI* pControl = this->FindSubControl(pstrElementName);
	ASSERT(pControl);
	if(pControl)
	{
		ShowTip(pControl);
		return true;
	}
	else
	{
		return false;
	}
}

void CValidatorUI::ShowTip(CControlUI* pControl)
{
	if(m_pWindow != NULL)
	{
		::DestroyWindow(*m_pWindow);
	}

	if(m_pWindow == NULL)
	{
		DWORD borderColor = m_pControl->GetBorderColor();
		int   borderSize   = m_pControl->GetBorderSize();

		if( m_dwErrorBorderColor != 0 && m_nErrorBorderSize != 0 && 
			(m_dwErrorBorderColor != borderColor || m_nErrorBorderSize != borderSize))
		{	
			m_dwCtrlBorderColor = borderColor;
			m_nCtrlBorderSize   = borderSize;

			m_pControl->SetBorderColor(m_dwErrorBorderColor);
			m_pControl->SetBorderSize(m_nErrorBorderSize);
		}

		m_pWindow = new CValidatorTipWnd;
		m_pWindow->Init(this, pControl);
	}
}

void CValidatorUI::Expaire()
{
	//if(m_bTipDisposed)return;
	
	if(m_pWindow)
	{
		//m_bTipDisposed = true;
		if(m_pControl && m_dwErrorBorderColor != 0 && m_nErrorBorderSize != 0)
		{
			m_pControl->SetBorderColor(m_dwCtrlBorderColor);
			m_pControl->SetBorderSize(m_nCtrlBorderSize);
		}

		//::DestroyWindow(*m_pWindow);	
		m_pWindow->ShowWindow(SW_HIDE);
	}
	
}

bool CValidatorUI::IsValid() const
{
	return m_bControlIsValid;
}

void CValidatorUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{	
	if( _tcscmp(pstrName, _T("targetcontrol")) == 0)SetTargetControlName(pstrValue);	
	else if( _tcscmp(pstrName, _T("arrowimage")) == 0)SetArrowImage(pstrValue);	
	else if( _tcscmp(pstrName, _T("errorbodercolor")) == 0){
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetErrorBorderColor(clrColor);
	}
	else if( _tcscmp(pstrName, _T("errorbordersize")) == 0)SetErrorBorderSize(_tstoi(pstrValue));
	else CContainerUI::SetAttribute(pstrName, pstrValue);
}

LPCTSTR CValidatorUI::GetTargetControlName() const
{
	return m_sControl;
}

void CValidatorUI::SetTargetControlName( LPCTSTR pstrName )
{
	m_sControl = pstrName;
}

void    CValidatorUI::SetArrowImage(LPCTSTR pstrImage)
{
	m_sArrowImage = pstrImage;
}

LPCTSTR CValidatorUI::GetArrowImage()const
{
	return m_sArrowImage;
}

void  CValidatorUI::SetErrorBorderColor(DWORD dwColor)
{
	m_dwErrorBorderColor = dwColor;
}

DWORD CValidatorUI::GetErrorBorderColor()const
{
	return m_dwErrorBorderColor;
}

void CValidatorUI::SetErrorBorderSize(int nSize)
{
	m_nErrorBorderSize = nSize;
}
int  CValidatorUI::GetErrorBorderSize()const
{
	return m_nErrorBorderSize;
}

void CValidatorUI::SetImmediate(bool value)
{
	m_bImmediate = value;
}

bool CValidatorUI::IsImmediate()const
{
	return m_bImmediate;
}

CControlUI* CValidatorUI::GetTargetControl()
{
	return m_pControl;
}

void CValidatorUI::SetTargetControl(CControlUI* pControl)
{
	if(m_pControl)
	{
		m_pControl->OnNotify -= DuiLib::MakeDelegate(this, &CValidatorUI::OnControlNotify);		
	}

	m_pControl = pControl;
	if(m_pControl)
	{
		m_pControl->OnNotify += DuiLib::MakeDelegate(this, &CValidatorUI::OnControlNotify);
		m_dwCtrlBorderColor = m_pControl->GetBorderColor();
		m_nCtrlBorderSize   = m_pControl->GetBorderSize();
	}
}

bool CValidatorUI::OnControlNotify(void *pMsg)
{
	TNotifyUI* pNotify = (TNotifyUI*)pMsg;
	if(pNotify->sType == _T("killfocus"))
	{
		if(IsImmediate())
		{
			this->Validate(true);
		}		
	}	
	else if(pNotify->sType == _T("textchanged") || pNotify->sType == _T("setfocus") || pNotify->sType == _T("clearvalidator") )
	{
		this->Expaire();
	}	
	return true;
}



}