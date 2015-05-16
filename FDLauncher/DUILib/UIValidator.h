/********************************************************************
	created:	2012/06/29	
	filename: 	UIValidator.h	
	author:		tabu
	email:      mgl_jl@hotmail.com
	purpose:	控件数据验证
*********************************************************************/
#ifndef __UIVALIDATOR_H__
#define __UIVALIDATOR_H__
#pragma once


namespace DuiLib{


//////////////////////////////////////////////////////////////////////////
// 验证接口 ValidatorUI依赖此接口
class UILIB_API IValidatorElementUI
{
public:
	virtual ~IValidatorElementUI(){}
	virtual bool Validate(CControlUI* pControl) = 0;	
};

// 内容非空验证
class UILIB_API CRequiredValidatorUI : public CTextUI,public IValidatorElementUI
{
public:
	LPCTSTR GetClass()const; 
	LPVOID  GetInterface(LPCTSTR pstrName);

	bool Validate(CControlUI* pControl);
};

// 内容长度验证
class UILIB_API CLengthValidatorUI : public CTextUI, public IValidatorElementUI
{
public:
	CLengthValidatorUI();

	LPCTSTR GetClass()const; 
	LPVOID  GetInterface(LPCTSTR pstrName);

	bool Validate(CControlUI* pControl);

	void SetMinLength(int value);
	int  GetMinLength()const;

	void SetMaxLength(int value);
	int  GetMaxLength()const;

	void SetUnicode(bool value);
	bool GetUnicode();

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
private:
	int m_nMinLength;
	int m_nMaxLength;	
	bool m_bUnicode;
};

// 验证表达式 
class UILIB_API CRegexValidatorUI: public CTextUI, public IValidatorElementUI
{
public:
	CRegexValidatorUI();

	LPCTSTR GetClass()const; 
	LPVOID  GetInterface(LPCTSTR pstrName);

	void SetExpression(LPCTSTR pstrExpr);
	LPCTSTR GetExpression()const;

	void SetIgnoreCase(bool value);
	bool GetIgnoreCase()const;

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	bool Validate(CControlUI* pControl);
private:
	CStdString m_sExpression;
	bool	   m_bIgnoreCase;
};

// 验证表达式：value >= minvalue && value < maxvalue
class UILIB_API CRangeValidatorUI : public CTextUI, public IValidatorElementUI
{
public:
	CRangeValidatorUI();

	LPCTSTR GetClass()const; 
	LPVOID  GetInterface(LPCTSTR pstrName);

	void SetMaxValue(int value);
	int  GetMaxValue()const;

	void SetMinValue(int value);
	int  GetMinValue()const;

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	
	bool Validate(CControlUI* pControl);
private:
	int m_nMaxValue;
	int m_nMinValue;
};

//////////////////////////////////////////////////////////////////////////
class CValidatorTipWnd;
class UILIB_API CValidatorUI : public CContainerUI, public IMessageFilterUI
{
public:
	CValidatorUI();
	virtual ~CValidatorUI();

	// 重写
	void DoInit();
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit  = true);
	void SetInternVisible(bool bVisible = true );
	
	LPCTSTR GetTargetControlName()const;
	void    SetTargetControlName(LPCTSTR pstrName);
	CControlUI* GetTargetControl();
	void		SetTargetControl(CControlUI* pControl);

	void    SetArrowImage(LPCTSTR pstrImage);
	LPCTSTR GetArrowImage()const;

	void  SetErrorBorderColor(DWORD dwColor);
	DWORD GetErrorBorderColor()const;

	void SetErrorBorderSize(int nSize);
	int  GetErrorBorderSize()const;
	
	void SetImmediate(bool value);
	bool IsImmediate()const;
	
	bool Validate(bool bShowTip = true);
	bool IsValid()const;

	bool ShowTip(LPCTSTR pstrElementName);	
	void Expaire();

	LRESULT MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled );
private:
	bool OnControlNotify(void *pMsg); 
	void ShowTip(CControlUI* pControl);
protected:		
	CStdString	m_sControl;		
	CStdString	m_sArrowImage;
	bool		m_bImmediate; 	

	DWORD m_dwErrorBorderColor;
	int   m_nErrorBorderSize;

	DWORD m_dwCtrlBorderColor;	
	int   m_nCtrlBorderSize;
	

	CValidatorTipWnd* m_pWindow;
	CControlUI*       m_pControl;	
	bool			  m_bControlIsValid;
	friend class CValidatorTipWnd;		 
};


} //end of namespace

#endif