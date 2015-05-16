#ifndef __UIIMAGETURNER_H__
#define __UIIMAGETURNER_H__



namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CImageTurnerUI : public CControlUI
{
	friend class CImageWnd;
public:
	CImageTurnerUI();
	~CImageTurnerUI();

	void SetImage(LPCTSTR pStrImage);

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	virtual void DoInit();
	virtual void DoPaint(HDC hDC, const RECT& rcPaint);
	virtual void SetVisible(bool bVisible = true);
	virtual void SetInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数

	SIZE EstimateSize(SIZE szAvailable);
	void DoEvent(TEventUI& event);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

protected:
	CImageWnd*		m_pWindow;
	CStdString		m_sImage;

	// GDI+
	ULONG_PTR		m_gdiplusToken;
};

} // namespace DuiLib

#endif // __UIIMAGETURNER_H__
