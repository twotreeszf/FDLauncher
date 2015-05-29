#include "stdafx.h"
#include "FormUI.h"
#include <atldlgs.h>
#include "../resource.h"
#include "Ctrl/ShadowWnd.h"

const int CONST_SHADOWN_SIZE = 5;

struct InvokeWrapper
{
	InvokeDelegate m_fn;

	InvokeWrapper(InvokeDelegate fn) : m_fn(fn)
	{
		
	}

	void operator()()
	{
		m_fn();
	}
};
//////////////////////////////////////////////////////////////////////////
UINT CFormUI::WM_MSG_INVOKE = RegisterWindowMessage(_T("Kuaipan.FormInvoke"));
CFormUI::CFormUI(bool bSupportEsc)
    : m_bShadowEnabled(true)
    , m_pShadowWnd(NULL)
    , m_nFlags(0)
    , m_nResult(IDCANCEL)
    , m_bSupportEsc(bSupportEsc)
    , m_bMovable(true)
{
	CFormUIManager::Instance().Attach(this);	
}

CFormUI::~CFormUI()
{
	CFormUIManager::Instance().Detach(this);

    // 保证移除msg filter
    if(CMessageLoop* pMsgLoop = _Module.GetMessageLoop())
    {
        pMsgLoop->RemoveMessageFilter(this);
    }
    if (OnDispose) OnDispose();
}

BOOL CFormUI::PreTranslateMessage(MSG* pMsg)
{
    if(m_bSupportEsc && pMsg->message == WM_KEYDOWN && (TCHAR)pMsg->wParam == VK_ESCAPE)
    {
        PostMessage(WM_CLOSE);

        return TRUE;
    }

    return FALSE;
}

BOOL CFormUI::ProcessWindowMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID /*= 0*/ )
{	
	switch(uMsg)
	{
		case WM_CREATE:      SetMsgHandled(TRUE);   lResult = OnCreate((LPCREATESTRUCT)lParam); break;
		//case WM_SHOWWINDOW:	 SetMsgHandled(TRUE);   lResult = 0; OnShowWindow((BOOL)wParam, (int)lParam);
		case WM_DESTROY:     SetMsgHandled(TRUE);   lResult = 0; OnDestroy(); break;
		case WM_CLOSE:		 SetMsgHandled(TRUE);	lResult = 0; OnClose(); break;		
		case WM_LBUTTONDOWN: SetMsgHandled(TRUE);	lResult = 0; OnLButtonDown((UINT)wParam, WTL::CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))); break;
		case WM_LBUTTONUP:	 SetMsgHandled(TRUE);	lResult = 0; OnLButtonUp((UINT)wParam, WTL::CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))); break;			
		case WM_SIZE:		 SetMsgHandled(TRUE);	lResult = 0; OnSize((UINT)wParam, WTL::CSize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))); break;	
		case WM_MOVE:        SetMsgHandled(TRUE);   lResult = 0; OnMove(WTL::CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))); break;				
		case WM_NCACTIVATE:	 SetMsgHandled(TRUE);	lResult = OnNcActivate((BOOL)wParam); break;
		case WM_NCCALCSIZE:	 SetMsgHandled(TRUE);	lResult = OnNcCalcSize((BOOL)wParam, lParam); break;
		
		case WM_USER: //对话框延迟销毁
			{ 
				SetMsgHandled(TRUE);
				if(wParam == WM_DESTROY)
				{
					DestroyWindow();
				}
			}
			break;
		default: SetMsgHandled(FALSE);break;
	}
	if(uMsg == WM_MSG_INVOKE)
	{
		SetMsgHandled(TRUE);   
		lResult = 0; 
		OnInvoke(wParam, lParam);
	}

	if(!IsMsgHandled())
	{
		if(m_paintManager.MessageHandler(uMsg, wParam, lParam, lResult))
		{
			SetMsgHandled(TRUE);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;	
}

void CFormUI::OnFinalMessage( HWND hWnd )
{
	delete this;
}

void CFormUI::ShowActive()
{
	if(!IsZoomed())
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE);
	}	
	ShowWindow(SW_SHOW);
	BringWindowToTop();
}

int CFormUI::ShowModal(HWND hParent /*= ::GetActiveWindow()*/, LPCTSTR szCaption/* = NULL*/)
{	
	if(m_hWnd == NULL)
	{
		Create(hParent, NULL, szCaption, WS_SYSMENU | WS_POPUP | WS_VISIBLE | DS_CENTER | DS_MODALFRAME, WS_EX_APPWINDOW);
	}

	if(IsWindow())
	{
		return RunModalLoop();
	}
	else
	{
		return -1;
	}

	/*if (m_thunk.Init(NULL,NULL) == FALSE)
	{
	SetLastError(ERROR_OUTOFMEMORY);
	return -1;
	}

	_AtlWinModule.AddCreateWndData(&m_thunk.cd, (CDialogImplBaseT<CWindow>*)this);

	CMemDlgTemplate dlgTemplate;
	dlgTemplate.Create(false, NULL, 0, 0, 100, 200, WS_SYSMENU | WS_POPUP | WS_VISIBLE | DS_CENTER | DS_MODALFRAME, WS_EX_APPWINDOW);
	return ::DialogBoxIndirectParam(_AtlBaseModule.GetResourceInstance(), dlgTemplate.GetTemplatePtr(), hParent, (DLGPROC)StartDialogProc, dwInitParam);	*/
}

bool CFormUI::IsModal()const
{
	return (m_nFlags & WF_MODALLOOP) == WF_MODALLOOP;
}

void CFormUI::Close()
{
	if(IsModal())
	{
		EndDialog(IDCANCEL);
	}
	else
	{
		//DestroyWindow();
		PostMessage(WM_CLOSE);
	}
}

void CFormUI::EndDialog( int nResult )
{	
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(IsModal());

	// this result will be returned from ShowModal
	m_nResult = nResult;

	// make sure a message goes through to exit the modal loop
	if (m_nFlags & WF_CONTINUEMODAL)
	{
		m_nFlags &= ~WF_CONTINUEMODAL;
		PostMessage(WM_NULL);			
	}
	
	//ASSERT(::EndDialog(m_hWnd, nResult));
}

int CFormUI::RunModalLoop()
{
	ASSERT(::IsWindow(m_hWnd));	
	ASSERT(!(m_nFlags & WF_MODALLOOP)); // window must not already be in modal state
	m_nFlags  |= (WF_MODALLOOP|WF_CONTINUEMODAL);
	m_nResult  = IDCANCEL;

	ShowWindow(SW_SHOWNORMAL);

    HWND hWndParent = GetParent();    
	BOOL bEnabledParent = FALSE;
	if(hWndParent != GetDesktopWindow() && ::IsWindowEnabled(hWndParent))
	{
		::EnableWindow(hWndParent, FALSE);		
		bEnabledParent = TRUE;
	}
    MSG msg = { 0 };
    while( ::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0) ) {

        if( !_Module.GetMessageLoop()->PreTranslateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
        }

		if( msg.message == WM_QUIT ) 
			return -1;

		if (!ContinueModal())
			break;
    }

	m_nFlags &= ~(WF_MODALLOOP|WF_CONTINUEMODAL);

	PostMessage(WM_USER, WM_DESTROY, 0); // 延迟销毁
	if(bEnabledParent)
	{
		::EnableWindow(hWndParent, TRUE);
		//::SetFocus(hWndParent);    
	}

	if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
		::SetActiveWindow(hWndParent);
	if(m_pShadowWnd)
		::ShowWindow(m_pShadowWnd->GetHWND(), SW_HIDE);

	ShowWindow(SW_HIDE);
    
    return m_nResult;
}

bool CFormUI::ContinueModal() const
{
	return (m_nFlags & WF_CONTINUEMODAL) == WF_CONTINUEMODAL;
}

CControlUI*	CFormUI::FindControl( LPCTSTR lpszName )
{
	return m_paintManager.FindControl(lpszName);
}

//////////////////////////////////////////////////////////////////////////
// msg handler
int CFormUI::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	ModifyStyle(WS_CAPTION, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	HICON hIcon = ::LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	m_paintManager.Init(m_hWnd, _Module.GetResource());
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_Module.GetResource(), this->GetSkinFile(), (UINT)0, NULL, &m_paintManager);
	ASSERT(pRoot && "Failed to parse XML");
	if(pRoot == NULL)
		return -1;	
	m_paintManager.AttachDialog(pRoot);
	m_paintManager.AddNotifier(this);	

	if(m_bShadowEnabled)
	{
		if( m_pShadowWnd == NULL ) 
			m_pShadowWnd = new CShadowWnd;

		m_pShadowWnd->Create(::GetParent(m_hWnd), _T(""), WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS, WS_EX_LAYERED | WS_EX_TOOLWINDOW,
			lpCreateStruct->x, 
			lpCreateStruct->y, 
			lpCreateStruct->cx, 
			lpCreateStruct->cy, NULL);
		::EnableWindow(*m_pShadowWnd, FALSE);
		::SetWindowPos(*m_pShadowWnd, m_hWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

    _Module.GetMessageLoop()->AddMessageFilter(this);

	this->OnInit();

	return 0;
}

void CFormUI::OnInit()
{

}

void CFormUI::OnDestroy()
{
    //if (IsModal())
    //    return;
	if(m_pShadowWnd)
	{
        ::DestroyWindow(*m_pShadowWnd);
        delete m_pShadowWnd;
        m_pShadowWnd = NULL;
	}

	// 销毁OnCreate中设置的图标资源
	HICON hIcon = SetIcon(NULL, TRUE);
	SetIcon(NULL, FALSE);
	if (hIcon)
	{
		::DestroyIcon(hIcon);
	}

    _Module.GetMessageLoop()->RemoveMessageFilter(this);

	SetMsgHandled(FALSE);
}

void CFormUI::OnClose()
{
	if(IsModal())
	{
		EndDialog(IDCANCEL);
	}
	else
	{
		if(m_pShadowWnd)
		{
			::ShowWindow(m_pShadowWnd->GetHWND(), SW_HIDE);
		}
		DestroyWindow();
	}	
}

void CFormUI::OnLButtonDown( UINT nFlags, WTL::CPoint pt )
{	
	RECT rcClient;
	GetClientRect(&rcClient);

	RECT rcCaption = m_paintManager.GetCaptionRect();
	if( m_bMovable && pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_paintManager.FindControl(pt));
		if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
			_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 && 
			_tcscmp(pControl->GetClass(), _T("ProgressUI")) != 0)
		{
			// bug在事件中删除当前交点控件后会无法拖动
			if(GetCapture() == m_hWnd)
			{
				ReleaseCapture();
			}
			PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, (LPARAM)(pt.y << 16 | pt.x));	
			return;				
		}			
	}
	SetMsgHandled(FALSE);	
}

void CFormUI::OnLButtonUp( UINT nFlags, WTL::CPoint pt )
{		
	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_paintManager.GetCaptionRect();
	if( m_bMovable && pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_paintManager.FindControl(pt));
		if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
			_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
			_tcscmp(pControl->GetClass(), _T("ProgressUI")) != 0)
		{
			PostMessage(WM_NCLBUTTONUP,HTCAPTION, (LPARAM)(pt.y << 16 | pt.x));
			//bHandled = TRUE;	// 这里为True会导致鼠标状态为手形不消失
		}
	}
	SetMsgHandled(FALSE);
}

void CFormUI::OnSize( UINT nType, WTL::CSize size )
{
	SIZE szRoundCorner = m_paintManager.GetRoundCorner();
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		DuiLib::CRect rcWnd;
		::GetWindowRect(m_hWnd, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(m_hWnd, hRgn, TRUE);
		::DeleteObject(hRgn);
	}

	if( m_pShadowWnd != NULL ) 
	{
		RECT rcWnd = { 0 };
		::GetWindowRect(m_hWnd, &rcWnd );
		::SetWindowPos(*m_pShadowWnd, m_hWnd, rcWnd.left - CONST_SHADOWN_SIZE, rcWnd.top - CONST_SHADOWN_SIZE, rcWnd.right -rcWnd.left + 2*CONST_SHADOWN_SIZE,
			rcWnd.bottom - rcWnd.top + 2*CONST_SHADOWN_SIZE, SWP_NOACTIVATE | SWP_NOZORDER);
	}
	SetMsgHandled(FALSE);
}

void CFormUI::OnMove( WTL::CPoint ptPos )
{
	if( m_pShadowWnd != NULL ) {				
		RECT rcWnd = { 0 };
		::GetWindowRect(m_hWnd, &rcWnd );
		::SetWindowPos(*m_pShadowWnd, m_hWnd, rcWnd.left - CONST_SHADOWN_SIZE, rcWnd.top - CONST_SHADOWN_SIZE, rcWnd.right -rcWnd.left + 2*CONST_SHADOWN_SIZE,
			rcWnd.bottom - rcWnd.top + 2*CONST_SHADOWN_SIZE, SWP_NOACTIVATE);
	}
	SetMsgHandled(FALSE);	
}

LRESULT CFormUI::OnNcCalcSize( BOOL bCalcValidRects, LPARAM lParam )
{
	return 0;
}

BOOL CFormUI::OnNcActivate( BOOL bActive )
{
	if( ::IsIconic(*this) ) 
		SetMsgHandled(FALSE);

	if(m_pShadowWnd)
	{
		m_pShadowWnd->ParentActive(bActive);
		if(bActive)
		{
			::SetWindowPos(m_pShadowWnd->GetHWND(), m_hWnd, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
		}
	}
	
	return !bActive; //(wParam == 0) ? TRUE : FALSE;
}

CFormUI* CFormUI::FindByName(LPCTSTR szName)
{
	return CFormUIManager::Instance().GetFirstByName(szName);
}

CFormUI* CFormUI::FindByClassName(LPCTSTR szClassName)
{
	return CFormUIManager::Instance().GetFirstByClassName(szClassName);
}

CStdString CFormUI::GetGroupName()
{
    return m_strGroupName;
}

void CFormUI::SetGroupName(const CStdString& strGroupName)
{
    m_strGroupName = strGroupName;
}

bool CFormUI::IsMovable()
{
    return m_bMovable;
}

void CFormUI::SetMovable(bool bMovable)
{
    m_bMovable = bMovable;
}

bool CFormUI::IsRequiredInvoke() const
{
	return ::GetCurrentThreadId() != _Module.m_dwMainThreadID;
}

void CFormUI::Invoke(InvokeDelegate fn)
{
	InvokeWrapper* pWrapper = new InvokeWrapper(fn);
	PostMessage(WM_MSG_INVOKE, (WPARAM)pWrapper, 0);
}

void CFormUI::OnInvoke(WPARAM wParam, LPARAM lParam )
{
	InvokeWrapper* pWrapper = (InvokeWrapper*)wParam;
	try
	{
		pWrapper->operator()();
		delete pWrapper;
	}
	catch(...)
	{
		delete pWrapper;
		throw;
	}
}
//////////////////////////////////////////////////////////////////////////
CFormUIManager::CFormUIManager()
{
	
}

CFormUIManager::~CFormUIManager()
{
	form_iterator it = m_lstForms.begin();
	for (;it != m_lstForms.end();++it)
	{
		ASSERT(!::IsWindow((*it)->GetHWND()));
		//delete *it;
	}
	m_lstForms.clear();
}

CFormUIManager& CFormUIManager::Instance()
{
	static CFormUIManager __obj;
	return __obj;
}

void CFormUIManager::Attach( CFormUI* pForm )
{
#ifdef _DEBUG
	form_iterator it = std::find(m_lstForms.begin(), m_lstForms.end(), pForm);
	ASSERT(it == m_lstForms.end());
#endif // _DEBUG

	m_lstForms.push_back(pForm);
}

void CFormUIManager::Detach( CFormUI* pForm )
{
	form_iterator it = std::find(m_lstForms.begin(), m_lstForms.end(), pForm);
	if(it != m_lstForms.end())
	{
		m_lstForms.erase(it);
	}
}

void CFormUIManager::CloseAll()
{
	CloseAllDialog();
	CloseAllForms();	
}

void CFormUIManager::CloseAllDialog()
{
	std::list<CFormUI*> lstForms = m_lstForms;
	form_reverse_iterator it = lstForms.rbegin();
	for (;it != lstForms.rend();++it)
	{		
		if(::IsWindow( (*it)->GetHWND()) && (*it)->IsModal())
		{
			(*it)->EndDialog(IDCANCEL);
		}
	}
}

void CFormUIManager::CloseAllForms()
{
	std::list<CFormUI*> lstForms = m_lstForms;
	form_reverse_iterator it = lstForms.rbegin();
	for (;it != lstForms.rend();++it)
	{
		if(::IsWindow((*it)->GetHWND()) && !(*it)->IsModal())
		{
			(*it)->Close();
		}
	}
}

void CFormUIManager::CloseAllWithout( CFormUI* pForm )
{
	std::list<CFormUI*> lstForms = m_lstForms;
	form_reverse_iterator it = lstForms.rbegin();
    CFormUI* lastDlg = NULL;
    std::vector<CFormUI*> forms;
	for (;it != lstForms.rend();++it)
	{		
		if(*it != pForm && ::IsWindow( (*it)->GetHWND()))
		{
			if((*it)->IsModal())
			{
                lastDlg = *it;
				(*it)->EndDialog(IDCANCEL);
			}
			else
			{
				//(*it)->Close();
                forms.push_back(*it);
			}			
		}
	}

    if (lastDlg == NULL)
    {
        std::for_each(forms.begin(), forms.end(), [](CFormUI* f){f->Close(); });        
    }
    else
    {
        lastDlg->OnDispose = [=](){
            std::for_each(forms.begin(), forms.end(), [](CFormUI* f){
                if (CFormUIManager::Instance().FindForm([=](CFormUI* form){return form == f; }) != NULL)
                {
                    f->Close();
                }
            });
        };
    }
}

CFormUI* CFormUIManager::GetFirstByName(LPCTSTR lpszName)
{
	form_iterator it = m_lstForms.begin();
	for (;it != m_lstForms.end();++it)
	{
		if(_tcscmp((*it)->GetFormName(), lpszName) == 0)
		{
			return (*it);
		}
	}
	return NULL;
}

CFormUI* CFormUIManager::GetFirstByClassName( LPCTSTR lpszName )
{
	form_iterator it = m_lstForms.begin();
	for (;it != m_lstForms.end();++it)
	{
		if(_tcscmp((*it)->GetWindowClassName(), lpszName) == 0)
		{
			return (*it);
		}
	}
	return NULL;
}

void CFormUIManager::GetAllByName(LPCTSTR lpszName, CStdPtrArray& arrForms)
{
    form_iterator it = m_lstForms.begin();
    for (;it != m_lstForms.end();++it)
    {
        if(_tcscmp((*it)->GetFormName(), lpszName) == 0)
        {
            arrForms.Add(*it);
        }
    }
}

void CFormUIManager::GetAllByClassName( LPCTSTR lpszName, CStdPtrArray& arrForms )
{
	form_iterator it = m_lstForms.begin();
	for (;it != m_lstForms.end();++it)
	{
		if(_tcscmp((*it)->GetWindowClassName(), lpszName) == 0)
		{
			arrForms.Add(*it);
		}
	}	
}

CFormUI* CFormUIManager::GetFirstByGroupName(LPCTSTR lpszName)
{
    form_iterator it = m_lstForms.begin();
    for (;it != m_lstForms.end();++it)
    {
        if(_tcscmp((LPCTSTR)(*it)->GetGroupName(), lpszName) == 0)
        {
            return (*it);
        }
    }
    return NULL;
}

void CFormUIManager::GetAllByGroupName(LPCTSTR lpszName, CStdPtrArray& arrForms)
{
    form_iterator it = m_lstForms.begin();
    for (;it != m_lstForms.end();++it)
    {
        if(_tcscmp((LPCTSTR)(*it)->GetGroupName(), lpszName) == 0)
        {
            arrForms.Add(*it);
        }
    }
}

CFormUI* CFormUIManager::GetTopDialog()
{
	form_reverse_iterator it = m_lstForms.rbegin();
	for (;it != m_lstForms.rend();++it)
	{
		if((*it)->IsModal())
		{
			return *it;
		}
	}	

	return NULL;
}

CFormUI* CFormUIManager::GetLastVisibleForm()
{
	form_reverse_iterator it = m_lstForms.rbegin();
	for (;it != m_lstForms.rend();++it)
	{
		CFormUI* pForm = *it;
		if(pForm->IsWindow() && pForm->IsWindowVisible() && !(pForm->GetStyle() & WS_CHILD))
		{
			return *it;			
		}
	}	

	return NULL;
}

CFormUI* CFormUIManager::FindForm(std::tr1::function<bool(CFormUI*)> pred)
{
	form_reverse_iterator it = m_lstForms.rbegin();
	for (;it != m_lstForms.rend();++it)
	{
		if(pred(*it))
		{
			return *it;			
		}
	}	

	return NULL;
}
