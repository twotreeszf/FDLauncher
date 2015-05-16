/* -------------------------------------------------------------------------
//	文件名	 ：	TitleBar.cpp
//	创建者　　：周传杰
//	创建时间 ：2012-6-6 9:28:27
//	功能描述 ：窗口标题栏
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "TitleBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CControlUI* CALLBACK __FindControlFromName(CControlUI* pThis, LPVOID pData)
{
	LPCTSTR pstrName = static_cast<LPCTSTR>(pData);
	const CStdString& sName = pThis->GetName();
	if( sName.IsEmpty() ) return NULL;
	return (_tcsicmp(sName, pstrName) == 0) ? pThis : NULL;
}

CTitleBarUI::CTitleBarUI():
m_bShowMax(TRUE)
{
	CDialogBuilder builder;
	CContainerUI* pContainer = static_cast<CContainerUI*>(builder.Create(_Module.GetResource(), _T("titlebar.xml"), (UINT)0, NULL));

	if (pContainer)
	{
  		m_pLeftLayout = static_cast<CHorizontalLayoutUI*>(pContainer->FindControl(__FindControlFromName, LPVOID(L"winIconLayout"), UIFIND_ALL));
		m_pSysBtnLayout = static_cast<CHorizontalLayoutUI*>(pContainer->FindControl(__FindControlFromName, LPVOID(L"sysBtnLayout"), UIFIND_ALL));
 		m_lblCaption = static_cast<CLabelUI*>(pContainer->FindControl(__FindControlFromName, LPVOID(L"winTitle"), UIFIND_ALL));
		m_btnClose = static_cast<CButtonUI*>(pContainer->FindControl(__FindControlFromName, LPVOID(L"sys_closebtn"), UIFIND_ALL));
		m_btnMin = static_cast<CButtonUI*>(pContainer->FindControl(__FindControlFromName, LPVOID(L"sys_minbtn"), UIFIND_ALL));
		m_conIcon = static_cast<CContainerUI*>(pContainer->FindControl(__FindControlFromName, LPVOID(L"winIcon"), UIFIND_ALL));
		
		CHorizontalLayoutUI::Add(pContainer);
	}
}

LPCTSTR CTitleBarUI::GetClass() const
{
	return L"TitleBarUI";
}

LPVOID CTitleBarUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("TitleBar")) == 0 ) return static_cast<CTitleBarUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}

void CTitleBarUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("caption")) == 0 ) m_lblCaption->SetText(pstrValue);
	else if (_tcscmp(pstrName, _T("showicon")) == 0)
		m_conIcon->SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
	else if (_tcscmp(pstrName, _T("showmin")) == 0)
		m_btnMin->SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
	else if (_tcscmp(pstrName, _T("showmax")) == 0)
		m_bShowMax = _tcscmp(pstrValue, _T("true")) == 0;
	else if (_tcscmp(pstrName, _T("showclose")) == 0)
		m_btnClose->SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
	else if (_tcscmp(pstrName, _T("icon")) == 0)
        m_conIcon->SetBkImage(pstrValue);
    else if( _tcscmp(pstrName, _T("font")) == 0 ) 
        m_lblCaption->SetFont(_ttoi(pstrValue));
	else 
		CHorizontalLayoutUI::SetAttribute(pstrName, pstrValue);
}

void CTitleBarUI::SetPos(RECT rc)
{
	int subWidth = 0;
	for (int i = 0; i < m_pSysBtnLayout->GetCount() ; ++i)
	{
		if (m_pSysBtnLayout->GetItemAt(i)->IsVisible())
		{
			subWidth += m_pSysBtnLayout->GetItemAt(i)->GetFixedWidth(); 
		}
	}
	m_pSysBtnLayout->SetFixedWidth(subWidth);

    CHorizontalLayoutUI::SetPos(rc);
    _SetCloseBtnPos();
}

void CTitleBarUI::SetCaption(LPCTSTR szCaption)
{
	if (m_lblCaption)
		m_lblCaption->SetText(szCaption);
}


void CTitleBarUI::DoInit()
{
	m_btnClose->OnNotify += MakeDelegate(this, &CTitleBarUI::OnClickClose);
	m_btnMin->OnNotify += MakeDelegate(this, &CTitleBarUI::OnClickMin);
    m_btnClose->SetFocus();
}

bool CTitleBarUI::OnClickClose(void* event)
{
	TNotifyUI* pNotify = (TNotifyUI*)event;
	if(pNotify->sType == _T("click"))
	{
		::PostMessage(m_pManager->GetPaintWindow(), WM_CLOSE, 0, 0);
		return true;
	}
	return false;
}

bool CTitleBarUI::OnClickMin(void* event)
{
	TNotifyUI* pNotify = (TNotifyUI*)event;
	if(pNotify->sType == _T("click"))
	{
		::PostMessage(m_pManager->GetPaintWindow(), WM_SYSCOMMAND, SC_MINIMIZE, 0);
		return true;
	}
	return false;

}

void CTitleBarUI::_SetCloseBtnPos()
{
    RECT rcTitle = GetPos();
    RECT rc = m_btnClose->GetPos();
    if(rc.top != ((rcTitle.bottom + rcTitle.top)/2 - 11))
        rc.top = (rcTitle.bottom + rcTitle.top)/2 - 11;
    rc.bottom = rc.top + 20;
    m_btnClose->SetPos(rc);
}

