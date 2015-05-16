#include "StdAfx.h"

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

//class CListWnd;

class CListEditWnd : public CWindowWnd
{
	//friend class CListWnd;
public:
	CListEditWnd();

	void Init(CListEditUI* pOwner);
	RECT CalPos();

	LPCTSTR GetWindowClassName() const;
	LPCTSTR GetSuperClassName() const;
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	CListEditUI* m_pOwner;
	HBRUSH m_hBkBrush;
	bool m_bInit;
};


CListEditWnd::CListEditWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false)
{
}

void CListEditWnd::Init(CListEditUI* pOwner)
{
	m_pOwner = pOwner;
	RECT rcPos = CalPos();
	UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;
	if( m_pOwner->IsPasswordMode() ) uStyle |= ES_PASSWORD;
	Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
	SetWindowFont(m_hWnd, m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->hFont, TRUE);
	Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());
	if( m_pOwner->IsPasswordMode() ) Edit_SetPasswordChar(m_hWnd, m_pOwner->GetPasswordChar());
	Edit_SetText(m_hWnd, m_pOwner->GetText());
	Edit_SetModify(m_hWnd, FALSE);
	SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
	Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
	Edit_SetReadOnly(m_hWnd, m_pOwner->IsReadOnly() == true);
	//Styls
	LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);
	styleValue |= pOwner->GetWindowStyls();
	::SetWindowLong(GetHWND(), GWL_STYLE, styleValue);
	::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	::SetFocus(m_hWnd);
	m_bInit = true;    
}

RECT CListEditWnd::CalPos()
{
	CRect rcPos = m_pOwner->GetPos();
	RECT rcInset = m_pOwner->GetTextPadding();
	rcPos.left += rcInset.left;
	rcPos.top += rcInset.top;
	rcPos.right -= rcInset.right;
	rcPos.bottom -= rcInset.bottom;
	LONG lEditHeight = m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->tm.tmHeight;
	if( lEditHeight < rcPos.GetHeight() ) {
		rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
		rcPos.bottom = rcPos.top + lEditHeight;
	}
	return rcPos;
}

LPCTSTR CListEditWnd::GetWindowClassName() const
{
	return _T("EditWnd");
}

LPCTSTR CListEditWnd::GetSuperClassName() const
{
	return WC_EDIT;
}

void CListEditWnd::OnFinalMessage(HWND /*hWnd*/)
{
	// Clear reference and die
	if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
	m_pOwner->m_pWindow = NULL;
	delete this;
}

LRESULT CListEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	if( uMsg == WM_KILLFOCUS/* && !m_pOwner->IsListShowing()*/)
	{
		/*lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		TEventUI event;
		event.Type = UIEVENT_KILLFOCUS;
		m_pOwner->DoEvent(event);*/	

		m_pOwner->KillFocus();
	}
	else if( uMsg == OCM_COMMAND ) {
		if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
		else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE ) {
			RECT rcClient;
			::GetClientRect(m_hWnd, &rcClient);
			::InvalidateRect(m_hWnd, &rcClient, FALSE);
		}
	}
	else if( uMsg == WM_KEYDOWN)
	{
		if ((TCHAR(wParam) >= VK_SPACE) && (TCHAR(wParam) <= VK_HELP))
		{
			bHandled = FALSE;
		}
        if ((TCHAR(wParam)) == VK_LEFT || (TCHAR(wParam)) == VK_RIGHT)
        {
            bHandled = FALSE;
        }
        else if (m_pOwner->IsListShowing())
		{
			TEventUI event;
			event.Type = UIEVENT_KEYDOWN;
			event.chKey = (TCHAR)wParam;
			event.wParam = wParam;
			event.lParam = lParam;
			m_pOwner->DoEvent(event);
			return 0;
		}
		else if (TCHAR(wParam) == VK_RETURN)
		{
			m_pOwner->GetManager()->SendNotify(m_pOwner, _T("return"));
		}
	}
	else if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC ) {
		if( m_pOwner->GetNativeEditBkColor() == 0xFFFFFFFF ) return NULL;
		::SetBkMode((HDC)wParam, TRANSPARENT);
		DWORD dwTextColor = m_pOwner->GetTextColor();
		::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
		if( m_hBkBrush == NULL ) {
			DWORD clrColor = m_pOwner->GetNativeEditBkColor();
			m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
		}
		return (LRESULT)m_hBkBrush;
	}
	else if( uMsg == WM_CLOSE)
	{
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
	else bHandled = FALSE;
	if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CListEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if( !m_bInit ) return 0;
	if( m_pOwner == NULL ) return 0;
	// Copy text back
	int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
	LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
	ASSERT(pstr);
	if( pstr == NULL ) return 0;
	::GetWindowText(m_hWnd, pstr, cchLen);
	m_pOwner->m_sText = pstr;
	m_pOwner->CheckInput();
	m_pOwner->GetManager()->SendNotify(m_pOwner, _T("textchanged"));
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

class CListWnd : public CWindowWnd
{
public:
    void Init(CListEditUI* pOwner);
    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void EnsureVisible(int iIndex);
    void Scroll(int dx, int dy);

#if(_WIN32_WINNT >= 0x0501)
	virtual UINT GetClassStyle() const;
#endif

public:
    CPaintManagerUI m_pm;
    CListEditUI* m_pOwner;
    CVerticalLayoutUI* m_pLayout;
    int m_iOldSel;
};


void CListWnd::Init(CListEditUI* pOwner)
{
    m_pOwner = pOwner;
    m_pLayout = NULL;
    m_iOldSel = m_pOwner->GetCurSel();

    // Position the popup window in absolute space
    SIZE szDrop = m_pOwner->GetDropBoxSize();
    RECT rcOwner = pOwner->GetPos();
    RECT rc = rcOwner;
    rc.top = rc.bottom;		// 父窗口left、bottom位置作为弹出窗口起点
    rc.bottom = rc.top + szDrop.cy;	// 计算弹出窗口高度
    if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;	// 计算弹出窗口宽度

    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
    int cyFixed = 0;
    for( int it = 0; it < pOwner->GetCount(); it++ ) {
        CControlUI* pControl = static_cast<CControlUI*>(pOwner->GetItemAt(it));
        if( !pControl->IsVisible() ) continue;
        SIZE sz = pControl->EstimateSize(szAvailable);
        cyFixed += sz.cy;
    }
    cyFixed += 4; // CVerticalLayoutUI 默认的Inset 调整
    rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);

    ::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);

    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    CRect rcWork = oMonitor.rcWork;
    if( rc.bottom > rcWork.bottom ) {
        rc.left = rcOwner.left;
        rc.right = rcOwner.right;
        if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;
        rc.top = rcOwner.top - MIN(cyFixed, szDrop.cy);
        rc.bottom = rcOwner.top;
        ::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
    }
    
    Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW, rc);
    // HACK: Don't deselect the parent's caption
    HWND hWndParent = m_hWnd;
    while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
    ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
    ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

LPCTSTR CListWnd::GetWindowClassName() const
{
    return _T("ComboWnd");
}

void CListWnd::OnFinalMessage(HWND hWnd)
{
    m_pOwner->m_pListWindow = NULL;
    m_pOwner->Invalidate();
    delete this;
}

LRESULT CListWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if( uMsg == WM_CREATE ) {
        m_pm.Init(m_hWnd, m_pOwner->GetManager()->GetResource());
        // The trick is to add the items to the new container. Their owner gets
        // reassigned by this operation - which is why it is important to reassign
        // the items back to the righfull owner/manager when the window closes.
        m_pLayout = new CVerticalLayoutUI;
        m_pm.UseParentResource(m_pOwner->GetManager());
        m_pLayout->SetManager(&m_pm, NULL, true);
        LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("VerticalLayout"));
        if( pDefaultAttributes ) {
            m_pLayout->ApplyAttributeList(pDefaultAttributes);
        }
        m_pLayout->SetInset(CRect(1, 1, 1, 1));
        m_pLayout->SetBkColor(0xFFFFFFFF);
        m_pLayout->SetBorderColor(0xFFC6C7D2);
        m_pLayout->SetBorderSize(1);
        m_pLayout->SetAutoDestroy(false);
        m_pLayout->EnableScrollBar();
        m_pLayout->ApplyAttributeList(m_pOwner->GetDropBoxAttributeList());
        for( int i = 0; i < m_pOwner->GetCount(); i++ ) {
            m_pLayout->Add(static_cast<CControlUI*>(m_pOwner->GetItemAt(i)));
        }
        m_pm.AttachDialog(m_pLayout);
		m_pOwner->SetFocusOnItem(0);
        
        return 0;
    }
    else if( uMsg == WM_CLOSE ) {
        m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
        //m_pOwner->SetPos(m_pOwner->GetPos());
        //m_pOwner->SetFocus();
    }
    else if( uMsg == WM_LBUTTONUP ) {
        POINT pt = { 0 };
        ::GetCursorPos(&pt);
        ::ScreenToClient(m_pm.GetPaintWindow(), &pt);
        CControlUI* pControl = m_pm.FindControl(pt);
        if (pControl && _tcscmp(pControl->GetClass(), _T("ScrollBarUI")) != 0)
        {
            m_pOwner->AcceptListInput();
            return 0;
        }
        else if (pControl)
        {
            TEventUI event;
            event.Type = UIEVENT_BUTTONDOWN;
            event.chKey = (TCHAR)wParam;
            event.wParam = wParam;
            event.lParam = lParam;
            m_pOwner->DoEvent(event); 
        }
    }
    else if( uMsg == WM_KEYDOWN ) {
        switch( wParam ) {
        case VK_ESCAPE:
            m_pOwner->SelectItem(m_iOldSel, true);
            EnsureVisible(m_iOldSel);
            // FALL THROUGH...
        case VK_RETURN:
			m_pOwner->AcceptListInput();
            return 0;
            //PostMessage(WM_KILLFOCUS);
            break;
        default:
            TEventUI event;
            event.Type = UIEVENT_KEYDOWN;
            event.chKey = (TCHAR)wParam;
            m_pOwner->DoEvent(event);
            EnsureVisible(m_pOwner->GetCurSel());
            return 0;
        }
    }
	/*else if( uMsg == WM_MOUSEWHEEL ) {
	int zDelta = (int) (short) HIWORD(wParam);
	TEventUI event = { 0 };
	event.Type = UIEVENT_SCROLLWHEEL;
	event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
	event.lParam = lParam;
	event.dwTimestamp = ::GetTickCount();
	m_pOwner->DoEvent(event);
	EnsureVisible(m_pOwner->GetCurSel());
	return 0;
	}*/
    else if( uMsg == WM_KILLFOCUS ) {
        //if( m_hWnd != (HWND) wParam ) PostMessage(WM_CLOSE);
    }

    LRESULT lRes = 0;
    if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CListWnd::EnsureVisible(int iIndex)
{
    if( m_pOwner->GetCurSel() < 0 ) return;
    m_pLayout->FindSelectable(m_pOwner->GetCurSel(), false);
    RECT rcItem = m_pLayout->GetItemAt(iIndex)->GetPos();
    RECT rcList = m_pLayout->GetPos();
    CScrollBarUI* pHorizontalScrollBar = m_pLayout->GetHorizontalScrollBar();
    if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();
    int iPos = m_pLayout->GetScrollPos().cy;
    if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
    int dx = 0;
    if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
    if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
    Scroll(0, dx);
}

void CListWnd::Scroll(int dx, int dy)
{
    if( dx == 0 && dy == 0 ) return;
    SIZE sz = m_pLayout->GetScrollPos();
    m_pLayout->SetScrollPos(CSize(sz.cx + dx, sz.cy + dy));
}

#if(_WIN32_WINNT >= 0x0501)
UINT CListWnd::GetClassStyle() const
{
	return __super::GetClassStyle() | CS_DROPSHADOW;
}
#endif

////////////////////////////////////////////////////////


CListEditUI::CListEditUI() : m_pWindow(NULL), m_pListWindow(NULL), m_uMaxChar(255), m_bReadOnly(false), 
	m_bPasswordMode(false), m_cPasswordChar(_T('*')), m_uButtonState(0), 
	m_dwEditbkColor(0xFFFFFFFF), m_iWindowStyls(0), m_iCurSel(-1), m_nMinLength(0)
{
	SetTextPadding(CRect(4, 3, 4, 3));
	SetBkColor(0xFFFFFFFF);

	m_szDropBox = CSize(0, 150);

	m_ListInfo.nColumns = 0;
	m_ListInfo.nFont = -1;
	m_ListInfo.uTextStyle = DT_VCENTER;
	m_ListInfo.dwTextColor = 0xFF000000;
	m_ListInfo.dwBkColor = 0;
	m_ListInfo.bAlternateBk = false;
	m_ListInfo.dwSelectedTextColor = 0xFF000000;
	m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
	m_ListInfo.dwHotTextColor = 0xFF000000;
	m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
	m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
	m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
	m_ListInfo.dwLineColor = 0;
	m_ListInfo.bShowHtml = false;
	m_ListInfo.bMultiExpandable = false;
	::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
	::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
}

CListEditUI::~CListEditUI()
{
    m_pManager->RemoveMessageFilter(this);
}

void CListEditUI::DoInit()
{
    m_pManager->AddMessageFilter(this);
	m_pContainer = new CContainerUI;
	m_pContainer->SetManager(m_pManager, NULL, true);

	CLabelUI::DoInit();
}

LRESULT CListEditUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if (uMsg == WM_MOUSEWHEEL)
    {
        if (m_pListWindow)
        {
            bool bDownward = GET_WHEEL_DELTA_WPARAM(wParam) == -120;
            SetFocusOnItem(m_pContainer->FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
            m_pListWindow->EnsureVisible(GetCurSel());
            bHandled = true;
        }
    }
    if (uMsg == WM_NCLBUTTONDOWN)
    {
        KillFocus();
    }
    return 0;
}

LPCTSTR CListEditUI::GetClass() const
{
	return _T("ListEditUI");
}

LPVOID CListEditUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("ListEdit")) == 0 ) return static_cast<CListEditUI*>(this);
	if( _tcscmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwnerUI*>(this);
	if( _tcscmp(pstrName, _T("IContainer")) == 0 ) return static_cast<IContainerUI*>(this);
	return CLabelUI::GetInterface(pstrName);
}

UINT CListEditUI::GetControlFlags() const
{
	if( !IsEnabled() ) return CControlUI::GetControlFlags();

	return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

void CListEditUI::DoEvent(TEventUI& event)
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->DoEvent(event);
		else CLabelUI::DoEvent(event);
		return;
	}

	if( event.Type == UIEVENT_SETCURSOR && IsEnabled() )
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
		return;
	}
	if( event.Type == UIEVENT_WINDOWSIZE )
	{
		if( m_pWindow != NULL ) m_pManager->SetFocusNeeded(this);
	}/*
	if( event.Type == UIEVENT_SCROLLWHEEL )
	{
		if( m_pWindow != NULL ) return;
	}*/
	if( event.Type == UIEVENT_SETFOCUS && IsEnabled() ) 
	{
		if( m_pWindow ) return;
		m_pWindow = new CListEditWnd();
		ASSERT(m_pWindow);
		m_pWindow->Init(this);
		Invalidate();
	}
	/*if( event.Type == UIEVENT_KILLFOCUS && IsEnabled() ) 
	{
		AcceptInput();
		CloseList();
		CloseInputEdit();
		Invalidate();
	}*/
	if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) 
	{
		if( IsEnabled() ) {
			GetManager()->ReleaseCapture();
			if( /*IsFocused() &&*/ m_pWindow == NULL )
			{
				m_pWindow = new CListEditWnd();
				ASSERT(m_pWindow);
				m_pWindow->Init(this);
                ::SetFocus(m_pWindow->GetHWND());

				if( PtInRect(&m_rcItem, event.ptMouse) )
				{
					int nSize = GetWindowTextLength(*m_pWindow);
					if( nSize == 0 )
						nSize = 1;

					Edit_SetSel(*m_pWindow, 0, nSize);
				}
			}
			else if( m_pWindow != NULL )
			{
#if 1
				int nSize = GetWindowTextLength(*m_pWindow);
				if( nSize == 0 )
					nSize = 1;

                Edit_SetSel(*m_pWindow, 0, nSize);
                ::SetFocus(m_pWindow->GetHWND());
#else
				POINT pt = event.ptMouse;
				pt.x -= m_rcItem.left + m_rcTextPadding.left;
				pt.y -= m_rcItem.top + m_rcTextPadding.top;
				::SendMessage(*m_pWindow, WM_LBUTTONDOWN, event.wParam, MAKELPARAM(pt.x, pt.y));
#endif
			}
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSEMOVE ) 
	{
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP ) 
	{
		return;
	}
	if( event.Type == UIEVENT_KEYDOWN )
	{
		switch( event.chKey ) {
		case VK_RETURN:
			{
				AcceptInput();

				if( m_pListWindow != NULL ) 
					m_pListWindow->Close();

				Invalidate();
			}
			return;
		case VK_F4:
			//Activate();
			return;
		case VK_UP:
            SetFocusOnItem(m_pContainer->FindSelectable(m_iCurSel - 1, false));
            if (m_pListWindow) m_pListWindow->EnsureVisible(GetCurSel());
			return;
		case VK_DOWN:
            SetFocusOnItem(m_pContainer->FindSelectable(m_iCurSel + 1, true));
            if (m_pListWindow) m_pListWindow->EnsureVisible(GetCurSel());
			return;
		case VK_PRIOR:
            SetFocusOnItem(m_pContainer->FindSelectable(m_iCurSel - 1, false));
            if (m_pListWindow) m_pListWindow->EnsureVisible(GetCurSel());
			return;
		case VK_NEXT:
            SetFocusOnItem(m_pContainer->FindSelectable(m_iCurSel + 1, true));
            if (m_pListWindow) m_pListWindow->EnsureVisible(GetCurSel());
			return;
		case VK_HOME:
            SetFocusOnItem(m_pContainer->FindSelectable(0, false));
            if (m_pListWindow) m_pListWindow->EnsureVisible(GetCurSel());
			return;
		case VK_END:
            SetFocusOnItem(m_pContainer->FindSelectable(GetCount() - 1, true));
            if (m_pListWindow) m_pListWindow->EnsureVisible(GetCurSel());
			return;
		}
	}
	if( event.Type == UIEVENT_SCROLLWHEEL )
	{
		bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
        SetFocusOnItem(m_pContainer->FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
        if (m_pListWindow) m_pListWindow->EnsureVisible(GetCurSel());
		return;
	}
	if( event.Type == UIEVENT_CONTEXTMENU )
	{
		return;
	}
	if( event.Type == UIEVENT_MOUSEENTER )
	{
		if( IsEnabled() ) {
			m_uButtonState |= UISTATE_HOT;
			Invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
		if( IsEnabled() ) {
			m_uButtonState &= ~UISTATE_HOT;
			Invalidate();
		}
		return;
	}
	CLabelUI::DoEvent(event);
}

void CListEditUI::SetEnabled(bool bEnable)
{
	CControlUI::SetEnabled(bEnable);
	if( !IsEnabled() ) {
		m_uButtonState = 0;
	}
}

void CListEditUI::SetText(LPCTSTR pstrText)
{
	m_sText = pstrText;
	if( m_pWindow != NULL ) Edit_SetText(*m_pWindow, m_sText);
	Invalidate();
}

void CListEditUI::SetMaxChar(UINT uMax)
{
	m_uMaxChar = uMax;
	if( m_pWindow != NULL ) Edit_LimitText(*m_pWindow, m_uMaxChar);
}

UINT CListEditUI::GetMaxChar()
{
	return m_uMaxChar;
}

void CListEditUI::SetReadOnly(bool bReadOnly)
{
	if( m_bReadOnly == bReadOnly ) return;

	m_bReadOnly = bReadOnly;
	if( m_pWindow != NULL ) Edit_SetReadOnly(*m_pWindow, m_bReadOnly);
	Invalidate();
}

bool CListEditUI::IsReadOnly() const
{
	return m_bReadOnly;
}

void CListEditUI::SetNumberOnly(bool bNumberOnly)
{
	if( bNumberOnly )
	{
		m_iWindowStyls |= ES_NUMBER;
	}
	else
	{
		m_iWindowStyls |= ~ES_NUMBER;
	}
}

bool CListEditUI::IsNumberOnly() const
{
	return m_iWindowStyls&ES_NUMBER ? true:false;
}

int CListEditUI::GetWindowStyls() const 
{
	return m_iWindowStyls;
}

void CListEditUI::SetPasswordMode(bool bPasswordMode)
{
	if( m_bPasswordMode == bPasswordMode ) return;
	m_bPasswordMode = bPasswordMode;
	Invalidate();
}

bool CListEditUI::IsPasswordMode() const
{
	return m_bPasswordMode;
}

void CListEditUI::SetPasswordChar(TCHAR cPasswordChar)
{
	if( m_cPasswordChar == cPasswordChar ) return;
	m_cPasswordChar = cPasswordChar;
	if( m_pWindow != NULL ) Edit_SetPasswordChar(*m_pWindow, m_cPasswordChar);
	Invalidate();
}

TCHAR CListEditUI::GetPasswordChar() const
{
	return m_cPasswordChar;
}

LPCTSTR CListEditUI::GetNormalImage()
{
	return m_sNormalImage;
}

void CListEditUI::SetNormalImage(LPCTSTR pStrImage)
{
	m_sNormalImage = pStrImage;
	Invalidate();
}

LPCTSTR CListEditUI::GetHotImage()
{
	return m_sHotImage;
}

void CListEditUI::SetHotImage(LPCTSTR pStrImage)
{
	m_sHotImage = pStrImage;
	Invalidate();
}

LPCTSTR CListEditUI::GetFocusedImage()
{
	return m_sFocusedImage;
}

void CListEditUI::SetFocusedImage(LPCTSTR pStrImage)
{
	m_sFocusedImage = pStrImage;
	Invalidate();
}

LPCTSTR CListEditUI::GetDisabledImage()
{
	return m_sDisabledImage;
}

void CListEditUI::SetDisabledImage(LPCTSTR pStrImage)
{
	m_sDisabledImage = pStrImage;
	Invalidate();
}

void CListEditUI::SetNativeEditBkColor(DWORD dwBkColor)
{
	m_dwEditbkColor = dwBkColor;
}

DWORD CListEditUI::GetNativeEditBkColor() const
{
	return m_dwEditbkColor;
}

void CListEditUI::SetPromptText(LPCTSTR pStrText)
{
	m_sPromptText = pStrText;
}

LPCTSTR CListEditUI::GetPromptText()const
{
	return m_sPromptText;
}

void CListEditUI::SetSel(long nStartChar, long nEndChar)
{
	if( m_pWindow != NULL ) Edit_SetSel(*m_pWindow, nStartChar,nEndChar);
}

void CListEditUI::SetSelAll()
{
	SetSel(0,-1);
}

void CListEditUI::SetReplaceSel(LPCTSTR lpszReplace)
{
	if( m_pWindow != NULL ) Edit_ReplaceSel(*m_pWindow, lpszReplace);
}

void CListEditUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	if( m_pWindow != NULL ) {
		RECT rcPos = m_pWindow->CalPos();
		::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, 
			rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);        
	}
}

void CListEditUI::SetVisible(bool bVisible)
{
	CControlUI::SetVisible(bVisible);
	if( !IsVisible() && m_pWindow != NULL ) m_pManager->SetFocus(NULL);
}

void CListEditUI::SetInternVisible(bool bVisible)
{
	if( !IsVisible() && m_pWindow != NULL ) m_pManager->SetFocus(NULL);
}

SIZE CListEditUI::EstimateSize(SIZE szAvailable)
{
	if( m_cxyFixed.cy == 0 ) return CSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 6);
	return CControlUI::EstimateSize(szAvailable);
}

void CListEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("readonly")) == 0 ) SetReadOnly(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("numberonly")) == 0 ) SetNumberOnly(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("password")) == 0 ) SetPasswordMode(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("maxchar")) == 0 ) SetMaxChar(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
	else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
	else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
	else if( _tcscmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
	else if( _tcscmp(pstrName, _T("nativebkcolor")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetNativeEditBkColor(clrColor);
	}
	else if( _tcscmp(pstrName, _T("prompttext")) == 0) SetPromptText(pstrValue);
	else CLabelUI::SetAttribute(pstrName, pstrValue);
}

void CListEditUI::PaintStatusImage(HDC hDC)
{
	if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~ UISTATE_FOCUSED;
	if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
	else m_uButtonState &= ~ UISTATE_DISABLED;

	if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
		if( !m_sDisabledImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
		if( !m_sFocusedImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		if( !m_sHotImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ) m_sHotImage.Empty();
			else return;
		}
	}

	if( !m_sNormalImage.IsEmpty() ) {
		if( !DrawImage(hDC, (LPCTSTR)m_sNormalImage) ) m_sNormalImage.Empty();
		else return;
	}
}

void CListEditUI::PaintText(HDC hDC)
{
	if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
	if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

	if( m_sText.IsEmpty() && (m_sPromptText.IsEmpty() || !IsEnabled()) ) return;

	CStdString sText = m_sText;
	if( m_bPasswordMode ) 
	{
		sText.Empty();
		LPCTSTR p = m_sText.GetData();
		while( *p != _T('\0') ) 
		{
			sText += m_cPasswordChar;
			p = ::CharNext(p);
		}
	}

	RECT rc = m_rcItem;
	rc.left += m_rcTextPadding.left;
	rc.right -= m_rcTextPadding.right;
	rc.top += m_rcTextPadding.top;
	rc.bottom -= m_rcTextPadding.bottom;


	if( IsEnabled() )
	{
		DWORD dwTextColor = m_dwTextColor;
		if(!IsFocused() && sText.IsEmpty())
		{
			sText = m_sPromptText;
			dwTextColor = m_dwDisabledTextColor;
		}
		CRenderEngine::DrawText(hDC, m_pManager, rc, sText, dwTextColor, \
			m_iFont, DT_SINGLELINE | m_uTextStyle);
	}
	else 
	{
		CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, \
			m_iFont, DT_SINGLELINE | m_uTextStyle);

	}

	/*RECT rcText = m_rcItem;
	rcText.left += m_rcTextPadding.left;
	rcText.right -= m_rcTextPadding.right;
	rcText.top += m_rcTextPadding.top;
	rcText.bottom -= m_rcTextPadding.bottom;

	if( m_iCurSel >= 0 ) {
	CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(m_iCurSel));
	IListItemUI* pElement = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
	if( pElement != NULL ) {
	pElement->DrawItemText(hDC, rcText);
	}
	else {
	RECT rcOldPos = pControl->GetPos();
	pControl->SetPos(rcText);
	pControl->DoPaint(hDC, rcText);
	pControl->SetPos(rcOldPos);
	}
	}*/
}

//////////////////////////////////////////////////////////////////////////
// IListOwnerUI
TListInfoUI* CListEditUI::GetListInfo()
{
	return &m_ListInfo;
}

int CListEditUI::GetCurSel() const
{
	return m_iCurSel;
}

int CListEditUI::GetSelCount() const
{
	if(m_iCurSel >= 0)
		return 1;
	return 0;
}

bool CListEditUI::SelectItem(int iIndex, bool bTakeFocus)
{
	if( iIndex == m_iCurSel ) return true;
	if( iIndex < 0 ) return false;
	if( GetCount() == 0 ) return false;
	if( iIndex >= GetCount() ) iIndex = GetCount() - 1;
	m_iCurSel = iIndex;

	return true;
}

bool CListEditUI::SetFocusOnItem(int iIndex, bool bTakeFocus)
{
	if( iIndex == m_iCurSel ) return true;
	int iOldSel = m_iCurSel;
	if( m_iCurSel >= 0 ) {
		CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(m_iCurSel));
		if( !pControl ) return false;
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) pListItem->Select(false);
		m_iCurSel = -1;
	}
	if( iIndex < 0 ) return false;
	if( GetCount() == 0 ) return false;
	if( iIndex >= GetCount() ) iIndex = GetCount() - 1;
	CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(iIndex));
	if( !pControl || !pControl->IsVisible() || !pControl->IsEnabled() ) return false;
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
	if( pListItem == NULL ) return false;
	m_iCurSel = iIndex;
	//if( m_pListWindow != NULL || bTakeFocus ) pControl->SetFocus();
	pListItem->Select(true);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// IListOwnerUI
CControlUI* CListEditUI::GetItemAt(int iIndex) const
{
	return m_pContainer->GetItemAt(iIndex);
}

int CListEditUI::GetItemIndex(CControlUI* pControl) const
{
	return m_pContainer->GetItemIndex(pControl);
}

bool CListEditUI::SetItemIndex(CControlUI* pControl, int iIndex)
{
	return m_pContainer->SetItemIndex(pControl, iIndex);
}

int CListEditUI::GetCount() const
{
	return m_pContainer->GetCount();
}

bool CListEditUI::Add(CControlUI* pControl)
{
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
	if( pListItem != NULL ) 
	{
		pListItem->SetOwner(this);
		pListItem->SetIndex(GetCount());
	}
	return m_pContainer->Add(pControl);
}

bool CListEditUI::AddAt(CControlUI* pControl, int iIndex)
{
	if (!m_pContainer->AddAt(pControl, iIndex)) return false;

	// The list items should know about us
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
	if( pListItem != NULL ) {
		pListItem->SetOwner(this);
		pListItem->SetIndex(iIndex);
	}

	for(int i = iIndex + 1; i < GetCount(); ++i) {
		CControlUI* p = GetItemAt(i);
		pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) {
			pListItem->SetIndex(i);
		}
	}
	if( m_iCurSel >= iIndex ) m_iCurSel += 1;
	return true;
}

bool CListEditUI::Remove(CControlUI* pControl)
{
	int iIndex = GetItemIndex(pControl);
	if (iIndex == -1) return false;

	if (!m_pContainer->RemoveAt(iIndex)) return false;

	for(int i = iIndex; i < GetCount(); ++i) {
		CControlUI* p = GetItemAt(i);
		IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) {
			pListItem->SetIndex(i);
		}
	}

	if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
		int iSel = m_iCurSel;
		m_iCurSel = -1;
		SelectItem(m_pContainer->FindSelectable(iSel, false));
	}
	else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
	return true;
}

bool CListEditUI::RemoveAt(int iIndex)
{
	if (!m_pContainer->RemoveAt(iIndex)) return false;

	for(int i = iIndex; i < GetCount(); ++i) {
		CControlUI* p = GetItemAt(i);
		IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) pListItem->SetIndex(i);
	}

	if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
		int iSel = m_iCurSel;
		m_iCurSel = -1;
		SelectItem(m_pContainer->FindSelectable(iSel, false));
	}
	else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
	return true;
}

void CListEditUI::RemoveAll()
{
	m_iCurSel = -1;
	m_pContainer->RemoveAll();
	CloseList();
}

//////////////////////////////////////////////////////////////////////////
// DropList
CStdString CListEditUI::GetDropBoxAttributeList()
{
	return m_sDropBoxAttributes;
}

void CListEditUI::SetDropBoxAttributeList(LPCTSTR pstrList)
{
	m_sDropBoxAttributes = pstrList;
}

SIZE CListEditUI::GetDropBoxSize() const
{
	return m_szDropBox;
}

void CListEditUI::SetDropBoxSize(SIZE szDropBox)
{
	m_szDropBox = szDropBox;
}

void CListEditUI::CloseList()
{
	if(m_pListWindow)
	{
		//m_pListWindow->PostMessage(WM_CLOSE);
		::DestroyWindow(m_pListWindow->GetHWND());

		m_setSearchResult.clear();

		if( m_pWindow != NULL )
			::SetFocus(m_pWindow->GetHWND());
	}
}

void CListEditUI::CloseInputEdit()
{
	if(m_pWindow)
		m_pWindow->PostMessage(WM_CLOSE);
}

bool CListEditUI::Activate()
{
	if( !CLabelUI::Activate() ) return false;
	if( m_pListWindow ) return true;
	m_pListWindow = new CListWnd();
	ASSERT(m_pListWindow);
	m_pListWindow->Init(this);
	if( m_pManager != NULL ) m_pManager->SendNotify(this, _T("dropdown"));
	Invalidate();
	return true;
}

//void CListEditUI::SetFocus()
//{
//	CLabelUI::SetFocus();
//
//	if( m_pWindow != NULL )
//	{
//		Edit_SetText(*m_pWindow, m_sText);
//		int nSize = GetWindowTextLength(*m_pWindow);
//		Edit_SetSel(*m_pWindow, nSize, nSize);
//		::SetFocus(m_pWindow->GetHWND());
//	}
//}

void CListEditUI::FindAll(LPCTSTR lpszString, std::set<CStdString>& vResult)
{
	std::set<CStdString>::iterator last = m_setStrings.end();		
	for (std::set<CStdString>::iterator it = m_setStrings.begin(); it != last; ++it)
	{
		//contains

		//startwith
		if( _tcsnicmp(*it, lpszString, _tcslen(lpszString)) == 0)
		{
			vResult.insert(*it);
		}
	}
}

void CListEditUI::AddSearchString(LPCTSTR lpszString)
{
	m_setStrings.insert(lpszString);	
}

void CListEditUI::Clear()
{
	m_setStrings.clear();
}

int CListEditUI::FindString(LPCTSTR lpszString)
{
	std::set<CStdString> setResult;
	std::set<CStdString> setNew;
	std::set<CStdString> setDeleted;
	FindAll(lpszString, setResult);

	if(!setResult.empty())
	{
		std::set_difference(
			m_setSearchResult.begin(), m_setSearchResult.end(),
			setResult.begin(), setResult.end(),
			std::inserter(setNew, setNew.end())
			);

		std::set_difference(	
			setResult.begin(), setResult.end(),
			m_setSearchResult.begin(), m_setSearchResult.end(),
			std::inserter(setDeleted, setDeleted.end())
			);

		if(!setNew.empty() || !setDeleted.empty())
		{
			RemoveAll();
			FillList(setResult);
			Activate();
			m_setSearchResult = setResult;
		}	
	}		
	else
	{
		m_setSearchResult.clear();
		CloseList();
	}

	return (int)setResult.size();
}

void CListEditUI::FillList(std::set<CStdString>& vResult)
{
	std::set<CStdString>::iterator last = vResult.end();	
	for (std::set<CStdString>::iterator it = vResult.begin(); it != last; ++it)
	{			
		CListLabelElementUI* pElement = new CListLabelElementUI;
		pElement->SetText(*it);
		Add(pElement);
	}	
}

void CListEditUI::SetMinLength(int nMinLen)
{
	m_nMinLength = nMinLen;
}

void CListEditUI::CheckInput()
{
	if(m_sText.GetLength() > m_nMinLength)
	{
		/*if(!m_bCursorMode)
		{
			FindString(strText);
		}*/

		CStdString strSearch;
		int nIndex = m_sText.ReverseFind(L';');
		if (nIndex > 0)
			strSearch = m_sText.Mid(nIndex + 1);
		else
			strSearch = m_sText;

		if (strSearch.IsEmpty())
		{
			CloseList();
		}
		else
		{
			FindString(strSearch);
			if(m_pWindow)
				::SetFocus(m_pWindow->GetHWND());
		}
	}
	else
	{
		CloseList();
	}
}

bool CListEditUI::IsListShowing()
{
	return (m_pListWindow != NULL);
}

void CListEditUI::AcceptInput()
{
	if( m_iCurSel >= 0 ) 
	{
		CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(m_iCurSel));
		if( pControl != NULL ) 
		{
			int nIndex = m_sText.ReverseFind(L';');
			if (nIndex > 0)
				m_sText = m_sText.Mid(0, nIndex + 1);
			else
				m_sText = L"";

			m_sText = m_sText + pControl->GetText() + L";";
		}
	}

	if( m_pWindow != NULL )
	{
		Edit_SetText(*m_pWindow, m_sText);
		int nSize = GetWindowTextLength(*m_pWindow);
		Edit_SetSel(*m_pWindow, nSize, nSize);
		::SetFocus(m_pWindow->GetHWND());
	}
}

void CListEditUI::KillFocus()
{
	HWND hWnd = ::GetFocus();
	if (m_pListWindow && hWnd == m_pListWindow->GetHWND())
		return;

	if( IsEnabled()) 
	{
		CloseList();
		CloseInputEdit();
		Invalidate();
	}
}

void CListEditUI::AcceptListInput()
{
    CloseList();
	AcceptInput();
	Invalidate();
}

void CListEditUI::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("textchanged")) == 0)
	{
		if(m_sText.GetLength() > m_nMinLength)
		{
			/*if(!m_bCursorMode)
			{
				FindString(strText);
			}*/
			FindString(m_sText);
		}
		else
		{
			CloseList();
		}
	}
	else if (_tcsicmp(msg.sType, _T("itemchecked")) == 0)
	{

	}
	else if (_tcsicmp(msg.sType, _T("itemunchecked")) == 0)
	{
	}	
}

} // namespace DuiLib
