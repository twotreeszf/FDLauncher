/* -------------------------------------------------------------------------
//	文件名	 ：	messagebox.h
//	创建者	 ：	周传杰
//	创建时间 ：2012-6-5 10:00:21
//	功能描述 ：	
//
// -----------------------------------------------------------------------*/

#ifndef __MESSAGEBOX_H__
#define __MESSAGEBOX_H__

#include "FormUI.h"
#include "Ctrl/TitleBar.h"

#define SET_VISIBLE_BTN(btnName, bVisible)	\
	 CButtonUI* pBtn##btnName = static_cast<CButtonUI*>(m_paintManager.FindControl(L#btnName));\
	if(pBtn##btnName) pBtn##btnName->SetVisible(bVisible);

#define SET_VISIBLE_ICON(iconName, bVisible)	\
	CControlUI* pIcon##iconName = static_cast<CControlUI*>(m_paintManager.FindControl(L#iconName));\
	if(pIcon##iconName) pIcon##iconName->SetVisible(bVisible);


class CMessageBox : public CFormUI
{
public:
	CMessageBox(LPCTSTR szCaption, UINT iFlags = MB_OK)
	{
		m_iFlags = iFlags;
		m_strCaption = szCaption;
	}

	virtual ~CMessageBox()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	// Event Handler
	DUI_BEGIN_NOTIFY_MAP(CMessageBox)
		DUI_NOTIFY_CONTROL_HANDLER(L"Btn_OK", L"click",	OnOKClick)
		DUI_NOTIFY_CONTROL_HANDLER(L"Btn_Cancle", L"click",	OnCancleClick)
		DUI_NOTIFY_CONTROL_HANDLER(L"Btn_Yes", L"click",	OnYesClick)
		DUI_NOTIFY_CONTROL_HANDLER(L"Btn_No", L"click",	OnNoClick)
		DUI_CHAIN_NOTIFY_MAP(CFormUI)
	DUI_END_NOTIFY_MAP()

	DUI_IMPLEMENT_FORM(_T("MessageBox"), _T("messagebox.xml"))

public:

	void OnNoClick(TNotifyUI& msg, BOOL& bHandled)
	{
        if(IsModal())
        {
            EndDialog(IDNO);
        }
        else
        {
            Close();
        }
	}
	
	void OnYesClick(TNotifyUI& msg, BOOL& bHandled)
	{
        if(IsModal())
        {
            EndDialog(IDYES);
        }
        else
        {
            Close();
        }
	}

	void OnOKClick(TNotifyUI& msg, BOOL& bHandled)
	{
        if(IsModal())
        {
            EndDialog(IDOK);
        }
        else
        {
            Close();
        }
	}

	void OnCancleClick(TNotifyUI& msg, BOOL& bHandled)
	{		
        if(IsModal())
        {
            EndDialog(IDCANCEL);
        }
        else
        {
            Close();
        }
	}
	
	// 设置主要内容
	void SetContentText(LPCTSTR szText)
	{
		m_strContent = szText;
	}

	// 设置提示文字
	void SetTipText(LPCTSTR szTipText)
	{
		m_strTipText = szTipText;
	}

	void SetOKBtnText(LPCTSTR szText)
	{
		m_strOKText = szText;
	}

	void SetCancelBtnText(LPCTSTR szText)
	{
		m_strCancelText = szText;
	}

	void AdjustWindow()
	{
		CHorizontalLayoutUI* pBtnGroup = static_cast<CHorizontalLayoutUI*>(m_paintManager.FindControl(L"Btn_Group"));

		CTitleBarUI* pTitleBar = static_cast<CTitleBarUI*>(m_paintManager.FindControl(L"titleBar"));
		if (pTitleBar)
			pTitleBar->SetCaption(m_strCaption);

		CTextUI* pTipText = static_cast<CTextUI*>(m_paintManager.FindControl(L"tipText"));
		if (pTipText)
		{
			if (!m_strTipText.IsEmpty())
				pTipText->SetText(m_strTipText);
			else
				pTipText->SetVisible(false);
		}

		CTextUI* pContent = static_cast<CTextUI*>(m_paintManager.FindControl(L"content"));
		if (pContent)
			pContent->SetText(m_strContent);

		int nHeight = pTitleBar->GetFixedHeight() + pBtnGroup->GetFixedHeight() + 40 + 30;

		int padding = 15 + 10 + 10;

		RECT rcDlg = { 0 };
		::GetWindowRect(m_hWnd, &rcDlg);

		SIZE aSize;
		aSize.cx = rcDlg.right - rcDlg.left;
		aSize = pContent->EstimateSize(aSize);

		if (pTipText->IsVisible())
		{
			SIZE tipSize;
			tipSize.cx = rcDlg.right - rcDlg.left;
			tipSize = pTipText->EstimateSize(tipSize);

			aSize.cx = aSize.cx > tipSize.cx ? aSize.cx : tipSize.cx;
			aSize.cy +=tipSize.cy;
			aSize.cy += 10 ;
		}

		aSize.cx += padding + 34 ;		// 34 是图标宽度

		ResizeClient(aSize.cx, nHeight + aSize.cy);



		if (m_strTipText.IsEmpty())
		{
			//ResizeClient(360,150);
		}
	}
protected:	
	virtual void OnInit()
	{
		DUI_BIND_CONTROL_VAR(L"Btn_OK",				m_pOKBtn,				CButtonUI);
		DUI_BIND_CONTROL_VAR(L"Btn_Cancle",			m_pCancelBtn,			CButtonUI);

		if (!m_strOKText.IsEmpty())
			m_pOKBtn->SetText(m_strOKText);

		if (!m_strCancelText.IsEmpty())
			m_pCancelBtn->SetText(m_strCancelText);

		ModifyStyleEx(WS_EX_APPWINDOW, 0);
		 //m_iFlags 
		UINT btnFlags = m_iFlags & 0x0F ;
		if (MB_OK == btnFlags)
		{
			SET_VISIBLE_BTN(Btn_OK, TRUE);
		}
		else if (MB_OKCANCEL == btnFlags)
		{
			SET_VISIBLE_BTN(Btn_OK, TRUE);
			SET_VISIBLE_BTN(Btn_Cancle, TRUE);
		}
		else if (MB_YESNOCANCEL == btnFlags)
		{
			SET_VISIBLE_BTN(Btn_Yes, TRUE);
			SET_VISIBLE_BTN(Btn_No, TRUE);
			SET_VISIBLE_BTN(Btn_Cancle, TRUE);
		}
		else if (MB_YESNO == btnFlags)
		{
			SET_VISIBLE_BTN(Btn_Yes, TRUE);
			SET_VISIBLE_BTN(Btn_No, TRUE);
		}
		
		// TODO 先不处理图标
		UINT iconFlags = m_iFlags & 0xF0 ;
		if (MB_ICONQUESTION == iconFlags)
		{
			SET_VISIBLE_ICON(questionIcon, true);
		}
		else if (MB_ICONINFORMATION == iconFlags)
		{
			SET_VISIBLE_ICON(infoIcon, true);
		}
		else if (MB_ICONERROR == iconFlags)
		{
			SET_VISIBLE_ICON(errorIcon, true);
		}
		else if (MB_USERICON == iconFlags)
		{
			SET_VISIBLE_ICON(correctIcon, true);
		}

		AdjustWindow();
		CenterWindow();
		SetWindowPos(/*HWND_TOPMOST*/NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	}
private:

	CString m_strCaption;
	CString m_strContent;
	CString m_strTipText;
	CString m_strOKText;
	CString m_strCancelText;
	CButtonUI* m_pOKBtn;
	CButtonUI* m_pCancelBtn;

	UINT	m_iFlags;
};


#endif	/*__messagebox_h__*/