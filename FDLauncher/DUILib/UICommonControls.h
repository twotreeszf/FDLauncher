#ifndef __UICOMMONCONTROLS_H__
#define __UICOMMONCONTROLS_H__

#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CLabelUI : public CControlUI
{
public:
    CLabelUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetTextStyle(UINT uStyle);
	UINT GetTextStyle() const;
    void SetTextColor(DWORD dwTextColor);
	DWORD GetTextColor() const;
    void SetDisabledTextColor(DWORD dwTextColor);
	DWORD GetDisabledTextColor() const;
    void SetFont(int index);
	int GetFont() const;
    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);
    bool IsMultiline();
    void SetMultiline(bool bMultiline = false);

    SIZE EstimateSize(SIZE szAvailable);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintText(HDC hDC);

protected:
    DWORD m_dwTextColor;
    DWORD m_dwDisabledTextColor;
    int m_iFont;
    UINT m_uTextStyle;
    RECT m_rcTextPadding;
    bool m_bShowHtml;
    bool m_bMultiline;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CButtonUI : public CLabelUI
{
public:
    CButtonUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    bool Activate();
    void SetEnabled(bool bEnable = true);
    void DoEvent(TEventUI& event);

    LPCTSTR GetNormalImage();
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage();
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage();
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage();
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage();
    void SetDisabledImage(LPCTSTR pStrImage);

    void SetHotTextColor(DWORD dwColor);
	DWORD GetHotTextColor() const;
    void SetPushedTextColor(DWORD dwColor);
	DWORD GetPushedTextColor() const;
    void SetFocusedTextColor(DWORD dwColor);
	DWORD GetFocusedTextColor() const;

	void SetHotFont(int index);
	int GetHotFont() const;

    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

protected:
    UINT m_uButtonState;

	DWORD m_dwHotTextColor;
	DWORD m_dwPushedTextColor;
	DWORD m_dwFocusedTextColor;

	int	  m_iHotFont;

    CStdString m_sNormalImage;
    CStdString m_sHotImage;
    CStdString m_sPushedImage;
    CStdString m_sFocusedImage;
    CStdString m_sDisabledImage;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API COptionUI : public CButtonUI
{
public:
    COptionUI();
    ~COptionUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);

    bool Activate();
    void SetEnabled(bool bEnable = true);

    LPCTSTR GetSelectedImage();
    void SetSelectedImage(LPCTSTR pStrImage);
	
	void SetSelectedTextColor(DWORD dwTextColor);
	DWORD GetSelectedTextColor();
		
	LPCTSTR GetForeImage();
	void SetForeImage(LPCTSTR pStrImage);

    LPCTSTR GetGroup() const;
    void SetGroup(LPCTSTR pStrGroupName = NULL);
    bool IsSelected() const;
    void Selected(bool bSelected, bool SendMessage = true);

    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintStatusImage(HDC hDC);
	void PaintText(HDC hDC);

protected:
    bool m_bSelected;
    CStdString m_sGroupName;

	DWORD m_dwSelectedTextColor;

    CStdString m_sSelectedImage;
	CStdString m_sForeImage;
};

/////////////////////////////////////////////////////////////////////////////////////
//
class UILIB_API  CIconButtonUI : public CButtonUI
{
public:
	CIconButtonUI();
	~CIconButtonUI();
	LPCTSTR GetNormalIconImage();
	void SetNormalIconImage(LPCTSTR pStrImage);
	LPCTSTR GetHotIconImage();
	void SetHotIconImage(LPCTSTR pStrImage);
	LPCTSTR GetDisableIconImage();
	void SetDisableIconImage(LPCTSTR pStrImage);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void PaintText(HDC hDC);

protected:
	CStdString m_sNormalIconImage;
	CStdString m_sHotIconImage;
	CStdString m_sDisableIconImage;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CTextUI : public CLabelUI
{
public:
    CTextUI();
    ~CTextUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    CStdString* GetLinkContent(int iIndex);

    void DoEvent(TEventUI& event);
    SIZE EstimateSize(SIZE szAvailable);

    void PaintText(HDC hDC);

	void SetMaxLink(int nCount);
    void SetHoverUnderline(bool bHoverUnderline);
protected:
    enum { MAX_LINK = 10 };
    int m_nLinks;
	int m_nMaxLinks;
    LPRECT m_rcLinks;
    CStdString* m_sLinks;
    bool m_bHoverUnderline;
    int m_nHoverLink;
};

/////////////////////////////////////////////////////////////////////////////////////
//
class UILIB_API CProgressBarUI : public CControlUI
{
public:
	CProgressBarUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void PaintStatusImage(HDC hDC);

	int GetMinValue() const;
	void SetMinValue(int nMin);
	int GetMaxValue() const;
	void SetMaxValue(int nMax);
	int GetValue() const;
	void SetValue(int nValue);

protected:
	DWORD m_barColor1;
	DWORD m_barColor2;
	int m_nMax;
	int m_nMin;
	int m_nValue;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CProgressUI : public CLabelUI
{
public:
    CProgressUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    bool IsHorizontal();
    void SetHorizontal(bool bHorizontal = true);
	bool IsStretchForeImage();
	void SetStretchForeImage(bool bStretchForeImage = true);
    int GetMinValue() const;
    void SetMinValue(int nMin);
    int GetMaxValue() const;
    void SetMaxValue(int nMax);
    int GetValue() const;
    void SetValue(int nValue);
    LPCTSTR GetForeImage() const;
    void SetForeImage(LPCTSTR pStrImage);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void PaintStatusImage(HDC hDC);

protected:
    bool m_bHorizontal;
	bool m_bStretchForeImage;
    int m_nMax;
    int m_nMin;
    int m_nValue;

    CStdString m_sForeImage;
    CStdString m_sForeImageModify;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CProgressImageUI : public CProgressUI
{
public:
    CProgressImageUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    void SetPartForeImage(LPCTSTR pStrImage);
    void SetFullForeImage(LPCTSTR pStrImage);
    void SetPartValue(int nPart);
    void SetAddValue(int nDefult);
    void SetValue(int nValue);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void PaintStatusImage(HDC hDC);
protected:
    int m_nPart;
    int m_nAdd;

    CStdString m_sPartForeImage;
    CStdString m_sFullForeImage;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CSliderUI : public CProgressUI
{
public:
    CSliderUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetEnabled(bool bEnable = true);

    int GetChangeStep();
    void SetChangeStep(int step);
    void SetThumbSize(SIZE szXY);
    RECT GetThumbRect() const;
    LPCTSTR GetThumbImage() const;
    void SetThumbImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbHotImage() const;
    void SetThumbHotImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbPushedImage() const;
    void SetThumbPushedImage(LPCTSTR pStrImage);

    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void PaintStatusImage(HDC hDC);

protected:
    SIZE m_szThumb;
    UINT m_uButtonState;
    int m_nStep;

    CStdString m_sThumbImage;
    CStdString m_sThumbHotImage;
    CStdString m_sThumbPushedImage;

    CStdString m_sImageModify;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class CEditWnd;

class UILIB_API CEditUI : public CLabelUI
{
    friend class CEditWnd;
public:
    CEditUI();
	~CEditUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    void SetEnabled(bool bEnable = true);
    void SetText(LPCTSTR pstrText);
    void SetMaxChar(UINT uMax);
    UINT GetMaxChar();
    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;
    void SetPasswordMode(bool bPasswordMode);
    bool IsPasswordMode() const;
    void SetPasswordChar(TCHAR cPasswordChar);
    TCHAR GetPasswordChar() const;
	void SetNumberOnly(bool bNumberOnly);
	bool IsNumberOnly() const;
	int GetWindowStyls() const;

    LPCTSTR GetNormalImage();
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage();
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage();
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage();
    void SetDisabledImage(LPCTSTR pStrImage);
    void SetNativeEditBkColor(DWORD dwBkColor);
    DWORD GetNativeEditBkColor() const;

	void SetPromptText(LPCTSTR pStrText);
	LPCTSTR GetPromptText()const;

    void PaintSel();
    void SetSel(long nStartChar, long nEndChar);
    void SetSelAll();
    void SetReplaceSel(LPCTSTR lpszReplace);

    void SetPos(RECT rc);
    void SetVisible(bool bVisible = true);
    void SetInternVisible(bool bVisible = true);
    SIZE EstimateSize(SIZE szAvailable);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintStatusImage(HDC hDC);
    void PaintText(HDC hDC);

protected:
    CEditWnd* m_pWindow;

    UINT m_uMaxChar;
    bool m_bReadOnly;
    bool m_bPasswordMode;
    TCHAR m_cPasswordChar;
    UINT m_uButtonState;
    CStdString m_sNormalImage;
    CStdString m_sHotImage;
    CStdString m_sFocusedImage;
    CStdString m_sDisabledImage;
	CStdString m_sPromptText;
    DWORD m_dwEditbkColor;
	int m_iWindowStyls;
    long m_nStartChar;
    long m_nEndChar;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CScrollBarUI : public CControlUI
{
public:
    CScrollBarUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    CContainerUI* GetOwner() const;
    void SetOwner(CContainerUI* pOwner);

    void SetVisible(bool bVisible = true);
    void SetEnabled(bool bEnable = true);
    void SetFocus();

    bool IsHorizontal();
    void SetHorizontal(bool bHorizontal = true);
    int GetScrollRange() const;
    void SetScrollRange(int nRange);
    int GetScrollPos() const;
    void SetScrollPos(int nPos);
    int GetLineSize() const;
    void SetLineSize(int nSize);

    bool GetShowButton1();
    void SetShowButton1(bool bShow);
    LPCTSTR GetButton1NormalImage();
    void SetButton1NormalImage(LPCTSTR pStrImage);
    LPCTSTR GetButton1HotImage();
    void SetButton1HotImage(LPCTSTR pStrImage);
    LPCTSTR GetButton1PushedImage();
    void SetButton1PushedImage(LPCTSTR pStrImage);
    LPCTSTR GetButton1DisabledImage();
    void SetButton1DisabledImage(LPCTSTR pStrImage);

    bool GetShowButton2();
    void SetShowButton2(bool bShow);
    LPCTSTR GetButton2NormalImage();
    void SetButton2NormalImage(LPCTSTR pStrImage);
    LPCTSTR GetButton2HotImage();
    void SetButton2HotImage(LPCTSTR pStrImage);
    LPCTSTR GetButton2PushedImage();
    void SetButton2PushedImage(LPCTSTR pStrImage);
    LPCTSTR GetButton2DisabledImage();
    void SetButton2DisabledImage(LPCTSTR pStrImage);

	int GetThumbMinSize()const;
	void SetThumbMinSize(int nSize);

    LPCTSTR GetThumbNormalImage();
    void SetThumbNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbHotImage();
    void SetThumbHotImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbPushedImage();
    void SetThumbPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbDisabledImage();
    void SetThumbDisabledImage(LPCTSTR pStrImage);

    LPCTSTR GetRailNormalImage();
    void SetRailNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetRailHotImage();
    void SetRailHotImage(LPCTSTR pStrImage);
    LPCTSTR GetRailPushedImage();
    void SetRailPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetRailDisabledImage();
    void SetRailDisabledImage(LPCTSTR pStrImage);

    LPCTSTR GetBkNormalImage();
    void SetBkNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetBkHotImage();
    void SetBkHotImage(LPCTSTR pStrImage);
    LPCTSTR GetBkPushedImage();
    void SetBkPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetBkDisabledImage();
    void SetBkDisabledImage(LPCTSTR pStrImage);
    UINT GetThumbState() { return m_uThumbState; };

    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void DoPaint(HDC hDC, const RECT& rcPaint);

    void PaintBk(HDC hDC);
    void PaintButton1(HDC hDC);
    void PaintButton2(HDC hDC);
    void PaintThumb(HDC hDC);
    void PaintRail(HDC hDC);

protected:

    enum { 
        DEFAULT_SCROLLBAR_SIZE = 16,
        DEFAULT_TIMERID = 10,
    };

    bool m_bHorizontal;
    int m_nRange;
    int m_nScrollPos;
    int m_nLineSize;
    CContainerUI* m_pOwner;
    POINT ptLastMouse;
    int m_nLastScrollPos;
    int m_nLastScrollOffset;
    int m_nScrollRepeatDelay;

    CStdString m_sBkNormalImage;
    CStdString m_sBkHotImage;
    CStdString m_sBkPushedImage;
    CStdString m_sBkDisabledImage;

    bool m_bShowButton1;
    RECT m_rcButton1;
    UINT m_uButton1State;
    CStdString m_sButton1NormalImage;
    CStdString m_sButton1HotImage;
    CStdString m_sButton1PushedImage;
    CStdString m_sButton1DisabledImage;

    bool m_bShowButton2;
    RECT m_rcButton2;
    UINT m_uButton2State;
    CStdString m_sButton2NormalImage;
    CStdString m_sButton2HotImage;
    CStdString m_sButton2PushedImage;
    CStdString m_sButton2DisabledImage;

    RECT m_rcThumb;
    UINT m_uThumbState;
	int  m_nThumbMinSize;
    CStdString m_sThumbNormalImage;
    CStdString m_sThumbHotImage;
    CStdString m_sThumbPushedImage;
    CStdString m_sThumbDisabledImage;

    CStdString m_sRailNormalImage;
    CStdString m_sRailHotImage;
    CStdString m_sRailPushedImage;
    CStdString m_sRailDisabledImage;

    CStdString m_sImageModify;
};

/// ����ͨ�ĵ�ѡ��ť�ؼ���ֻ���ǡ������ֽ��
/// ������COptionUI��ֻ��ÿ��ֻ��һ����ť���ѣ�����Ϊ�գ������ļ�Ĭ�����Ծ�����
/// <CheckBox name="CheckBox" value="height='20' align='left' textpadding='24,0,0,0' normalimage='file='sys_check_btn.png' source='0,0,20,20' dest='0,0,20,20'' selectedimage='file='sys_check_btn.png' source='20,0,40,20' dest='0,0,20,20'' disabledimage='file='sys_check_btn.png' source='40,0,60,20' dest='0,0,20,20''"/>

class UILIB_API CCheckBoxUI : public COptionUI
{
public:
	LPCTSTR GetClass() const;

	void SetCheck(bool bCheck);
	bool GetCheck() const;
};


/// ��չ�����б��
/// ����arrowimage����,һ��ͼƬƽ���ֳ�5��,Normal/Hot/Pushed/Focused/Disabled(������source����)
/// <Default name="ComboBox" value="arrowimage=&quot;file='sys_combo_btn.png' source='0,0,16,16'&quot; "/>
class UILIB_API CComboBoxUI : public CComboUI
{
public:
	CComboBoxUI();
	LPCTSTR GetClass() const;

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void PaintText(HDC hDC);
	void PaintStatusImage(HDC hDC);

protected:
	CStdString m_sArrowImage;
	int        m_nArrowWidth;
};


class CDateTimeWnd;

/// ʱ��ѡ��ؼ�
class UILIB_API CDateTimeUI : public CLabelUI
{
	friend class CDateTimeWnd;
public:
	CDateTimeUI();
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	SYSTEMTIME& GetTime();
	void SetTime(SYSTEMTIME* pst);

	void SetReadOnly(bool bReadOnly);
	bool IsReadOnly() const;

	void UpdateText();

	void DoEvent(TEventUI& event);

protected:
	SYSTEMTIME m_sysTime;
	int        m_nDTUpdateFlag;
	bool       m_bReadOnly;

	CDateTimeWnd* m_pWindow;
};

class UILIB_API CFileIconUI : public CControlUI
{
public:
    CFileIconUI();
    ~CFileIconUI();

    HICON GetIcon();
    void SetIcon(HICON hIcon);

    LPCTSTR GetNormalImage();
    void SetNormalImage(LPCTSTR pStrImage);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintBkImage(HDC hDC);

private:
    HICON	   m_hIcon;
    CStdString m_sNormalImage;
};

} // namespace DuiLib

#endif // __UICOMMONCONTROLS_H__

