#ifndef __UILISTEDIT_H__
#define __UILISTEDIT_H__

#include <vector>
#include <set>
#include <iterator>
#include <algorithm>

#pragma once
#pragma warning(push)
#pragma warning(disable:4251)
namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class CListEditWnd;
class CListWnd;

class UILIB_API CListEditUI : public CLabelUI, public IListOwnerUI, IContainerUI, IMessageFilterUI
{
	friend class CListEditWnd;
	friend class CListWnd;
public:
	CListEditUI();
    ~CListEditUI();
	virtual void DoInit();

    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
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

	// IListOwnerUI
	TListInfoUI* GetListInfo();
	int GetCurSel() const;
	int GetSelCount() const;
	bool SelectItem(int iIndex, bool bTakeFocus = false);
	CControlUI* GetItemAt(int iIndex) const;
	int GetItemIndex(CControlUI* pControl) const;
	bool SetItemIndex(CControlUI* pControl, int iIndex);
	int GetCount() const;
	bool Add(CControlUI* pControl);
	bool AddAt(CControlUI* pControl, int iIndex);
	bool Remove(CControlUI* pControl);
	bool RemoveAt(int iIndex);
	void RemoveAll();

	// DropList
	CStdString GetDropBoxAttributeList();
	void SetDropBoxAttributeList(LPCTSTR pstrList);
	SIZE GetDropBoxSize() const;
	void SetDropBoxSize(SIZE szDropBox);
	void CloseList();
	void CloseInputEdit();
	bool SetFocusOnItem(int iIndex, bool bTakeFocus = false);

	// Inherit from CLabelUI
	virtual bool Activate();
	//virtual void SetFocus();

	// Search
	void FindAll(LPCTSTR lpszString, std::set<CStdString>& vResult);
	void AddSearchString(LPCTSTR lpszString);
	void Clear();
	int FindString(LPCTSTR lpszString);
	void FillList(std::set<CStdString>& vResult);
	void SetMinLength(int nMinLen);
	void CheckInput();

	bool IsListShowing();
	void AcceptInput();
	void KillFocus();
    void AcceptListInput();

protected:	

	void Notify(TNotifyUI& msg);

protected:
	CListEditWnd* m_pWindow;
	CListWnd* m_pListWindow;

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

	// DropList
	TListInfoUI m_ListInfo;
	int m_iCurSel;
	SIZE m_szDropBox;
	CContainerUI* m_pContainer;
	CStdString m_sDropBoxAttributes;

	std::set<CStdString> m_setStrings;
	int	m_nMinLength; 
	std::set<CStdString> m_setSearchResult;
};

} // namespace DuiLib
#pragma warning(pop)
#endif // __UILISTEDIT_H__
