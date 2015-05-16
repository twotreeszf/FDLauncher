/* -------------------------------------------------------------------------
//	�ļ���	 ��	HeaderCtrl.h
//	������	 ��	�ܴ���
//	����ʱ�� ��2012-6-5 20:17:37
//	�������� �����д��ڵı�����	
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