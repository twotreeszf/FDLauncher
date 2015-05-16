/* -------------------------------------------------------------------------
//	文件名	 ：	HeaderCtrl.h
//	创建者	 ：	周传杰
//	创建时间 ：2012-6-5 20:17:37
//	功能描述 ：所有窗口的标题栏	
//
// -----------------------------------------------------------------------*/

#ifndef __HEADERCTRL_H__
#define __HEADERCTRL_H__

class CTitleBarUI: public CHorizontalLayoutUI
{
public:
	CTitleBarUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	//UINT GetControlFlags() const;

    virtual void DoInit();

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void SetPos(RECT rc);

	void SetCaption(LPCTSTR szCaption);

	CControlUI* GetCloseBtnControl()
	{
		return m_btnClose;
	}

private:
	bool OnClickClose(void* event);
	bool OnClickMin(void* event);
    void _SetCloseBtnPos();
private:

	CHorizontalLayoutUI* m_pLeftLayout;
	CHorizontalLayoutUI* m_pSysBtnLayout;

	CLabelUI*	m_lblCaption;
	CButtonUI*  m_btnClose;
	CButtonUI*  m_btnMin;

	CContainerUI* m_conIcon;

	CString m_strCaption;
	BOOL	m_bShowMax;
};

#endif	/*__HeaderCtrl_h__*/