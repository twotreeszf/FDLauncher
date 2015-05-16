#pragma once
#include <UIlib.h>
#include <functional>
#include <atlwin.h>
#include <atlcrack.h>

//////////////////////////////////////////////////////////////////////////

typedef std::function<void()> InvokeDelegate;

class CShadowWnd;
class CFormUI 
    : public CWindowImpl<CFormUI, CWindow, CFrameWinTraits>
    , public CMessageFilter
    , public INotifyUI
{
protected:
	enum
	{
		WF_MODALLOOP     =  0x0008,  // currently in modal loop
		WF_CONTINUEMODAL =  0x0010  // modal loop should continue running
	};

public:
	DECLARE_WND_CLASS(L"UIFoundation");

	CFormUI(bool bSupportEsc = true);
	virtual ~CFormUI();	

    virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0);
	virtual void OnFinalMessage(HWND hWnd);
	virtual void Notify(TNotifyUI& msg){ }

	HWND GetHWND()const{ return m_hWnd; }	

	int	ShowModal(HWND hParent = ::GetActiveWindow(), LPCTSTR szCaption = NULL);
	virtual void EndDialog(int nResult);
	bool IsModal()const;	

	void ShowActive();

	// 强行关闭窗口
	void Close(); 

	CControlUI*	FindControl( LPCTSTR lpszName );

	static CFormUI* FindByName(LPCTSTR szName);
	static CFormUI* FindByClassName(LPCTSTR szClassName);	
	
    virtual CStdString GetGroupName();
    virtual void SetGroupName(const CStdString& strGroupName);

    virtual HWND GetChildHWnd(){ return NULL; };

    virtual bool IsMovable();
    virtual void SetMovable(bool bMovable);

	virtual LPCTSTR GetFormName()const = 0;
	virtual LPCTSTR GetWindowClassName() const { return _T("FormUI"); }		
	virtual LPCTSTR GetSkinFile() const { return _T("formui.xml"); }
	virtual DuiLib::IDialogBuilderCallback* GetDialogBuilderCallback(){ return NULL; }

	bool IsRequiredInvoke() const;
	void Invoke(InvokeDelegate fn);

    std::function<void()> OnDispose;
protected:
	//加载完xml文件后被调用
	virtual void OnInit();

	virtual int  OnCreate(LPCREATESTRUCT lpCreateStruct);	
	virtual void OnDestroy();	
	virtual void OnClose();
	//virtual void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnLButtonDown(UINT nFlags, WTL::CPoint point);
	virtual void OnLButtonUp(UINT nFlags, WTL::CPoint point);
	virtual void OnSize(UINT nType, WTL::CSize size);
	virtual void OnMove(WTL::CPoint ptPos);
	virtual LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam);
	virtual BOOL OnNcActivate(BOOL bActive);

	int  RunModalLoop();
	bool ContinueModal()const;
private:
	static UINT WM_MSG_INVOKE;
	void OnInvoke(WPARAM wParam, LPARAM lParam );
protected:
    friend class CFormUIManager;

	CPaintManagerUI    m_paintManager;

	bool			   m_bShadowEnabled;
	CShadowWnd*		   m_pShadowWnd;

	DWORD              m_nFlags;
	int                m_nResult;
	bool               m_bSupportEsc;
    bool               m_bMovable;
    CStdString         m_strGroupName;
};

//////////////////////////////////////////////////////////////////////////

class CFormUIManager
{	
private:
	typedef std::list<CFormUI*> form_container_type;
	typedef form_container_type::iterator form_iterator;
	typedef form_container_type::reverse_iterator form_reverse_iterator;

	form_container_type	m_lstForms;
private:
	CFormUIManager();
public:
	~CFormUIManager();
	static CFormUIManager& Instance();
	
	void Attach(CFormUI* pForm);
	void Detach(CFormUI* pForm);

	void CloseAll();
	void CloseAllDialog();
	void CloseAllForms();	
	void CloseAllWithout(CFormUI* pForm);
	
	CFormUI* GetFirstByName(LPCTSTR lpszName);
	CFormUI* GetFirstByClassName(LPCTSTR lpszClassName);

	void GetAllByName(LPCTSTR lpszName, CStdPtrArray& arrForms);
	void GetAllByClassName(LPCTSTR lpszName, CStdPtrArray& arrForms);

    CFormUI* GetFirstByGroupName(LPCTSTR lpszName);
    void GetAllByGroupName(LPCTSTR lpszName, CStdPtrArray& arrForms);

	CFormUI* GetTopDialog();
	CFormUI* GetLastVisibleForm();
	CFormUI* FindForm(std::tr1::function<bool(CFormUI*)> pred);
};

//////////////////////////////////////////////////////////////////////////

class CAnimatePanelUI : public CFormUI
{
public:
	CAnimatePanelUI(DWORD dwAWHideFlag = AW_HOR_POSITIVE | AW_HIDE, 
		DWORD dwAWShowFlag = AW_HOR_NEGATIVE, 
		bool bSupportEsc = true);

	virtual ~CAnimatePanelUI();

	virtual LPCTSTR GetWindowClassName() const { return _T("AnimatePanel"); }

public:
	BEGIN_MSG_MAP(CAnimatePanelUI)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		CHAIN_MSG_MAP(CFormUI)
	END_MSG_MAP()

	void OnShowWindow(BOOL bShow, UINT nStatus);
	void SetTriggerBtn(COptionUI* pTriggerBtn);

	BOOL GetAnimation();
	void SetAnimation(BOOL bAnimation);

protected:	
	virtual void OnClose();

protected:
	DWORD           m_dwAWHideFlag;
	DWORD           m_dwAWShowFlag;
	DWORD           m_dwTime;
	COptionUI*      m_pTriggerBtn;
	BOOL            m_bAnimation;
};

//////////////////////////////////////////////////////////////////////////

// 		LPCTSTR lpText,
// 		LPCTSTR lpCaption,

int ShowMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

// lpTipText 是副标题
int ShowMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpTipText, LPCTSTR lpCaption, UINT uType);

int ShowMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpTipText, LPCTSTR lpCaption, LPCTSTR lpOKBtnText, LPCTSTR lpCancelBtnText, UINT uType);




enum MessageType
{
	MESSAGETYPE_UP = 1,
	MESSAGETYPE_DOWN
};

#define DUI_IMPLEMENT_FORM(formName, skinFile)	\
	virtual LPCTSTR GetFormName()const	{ return (formName); }\
	virtual LPCTSTR GetSkinFile()const { return (skinFile); } 

#define DUI_BIND_CONTROL_VAR(sName, uiCtrl, type)	\
	(uiCtrl) = static_cast<type*>(m_paintManager.FindControl((sName)))  \


#define DUI_BEGIN_NOTIFY_MAP(theClass)		\
public:								\
	void Notify(TNotifyUI& msg){	\
		BOOL bHandled = TRUE;		\
		(bHandled);					\
		(msg);

#define DUI_NOTIFY_HANDLER(type, func)\
	if((msg).sType == type)			\
	{								\
		bHandled = TRUE;			\
		func(msg, bHandled);		\
		if(bHandled)					\
			return;					\
	}

#define DUI_NOTIFY_CONTROL_HANDLER(sCtrl, type, func)\
	if((msg).pSender && (msg).pSender->GetName() == sCtrl && (msg).sType == type)	\
	{															\
		bHandled = TRUE;										\
		func(msg, bHandled);									\
		if(bHandled)												\
			return;												\
	}

#define DUI_CHAIN_NOTIFY_MAP(theChainClass) \
{ \
	theChainClass::Notify(msg); \
}

#define DUI_END_NOTIFY_MAP()			\
}
