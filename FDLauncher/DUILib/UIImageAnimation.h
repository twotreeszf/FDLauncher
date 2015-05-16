/* -------------------------------------------------------------------------
//
//	文件名		：	ImageAnimation.h
//	创建者		：	夏路遥
//	创建时间	：	2013-8-30 16:15:19
//	功能描述	：	
//
// -----------------------------------------------------------------------*/

#ifndef __ImageAnimation_h__
#define __ImageAnimation_h__

// -------------------------------------------------------------------------
#define AutoPicMoveNxtTimerId 100
#define PicMoveNxtTimerId 101
#define PicMovePreTimerId 102

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	struct ImageBlock
	{
		CStdString  m_szName;
		CStdString  m_szImage;
		DWORD       m_dwBorderColor;
		float		m_fBorderWidth;
		ImageBlock(): 
		m_dwBorderColor(0xFFFFFFFF),
			m_fBorderWidth(0.0)
		{

		}
	};
	class UILIB_API CImageAnimationUI: public CControlUI
	{
	public:
		CImageAnimationUI();
		~CImageAnimationUI();
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual void SetPos(RECT rc);
		virtual void DoEvent(TEventUI& event);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void PaintBkImage(HDC hDC);
		virtual void PaintStatusImage(HDC hDC);

	public:
		void MoveUpdate(int fLeftOffset, int iCurrentImage, bool bOutside = false);
		int GetChildrenHeight();
		int GetChildrenWidth();
		int GetOffset();
		void SetImageChangeTime(int timeCnt){ m_iDefTimerCnt = timeCnt;};
		void SetAutoImageChangeEnable(bool bEnable, int iAutoTimeCnt = 0);
		void showNextImage();
		void showPreImage();
		void OnImageMoveNxt();
		void OnImageMovePre();
		void SetTargetImage(int iTargetImage);
		void SetRelativeBtn(std::vector<COptionUI*> btnList);
		void SetBkImageMovable(bool bBkimageMovable){m_bBkimageMovable = bBkimageMovable;};
	private:
		inline DWORD _ColorValue(LPCTSTR szColor);
		void PaintImage( HDC hDC, ImageBlock* item, const int iLeftOffset );
		CStdString	m_sMovingBkImage;

	protected:
		std::vector<ImageBlock*>		m_displayVector;
		std::vector<COptionUI*>			m_btnVector;
		int								m_iLeftOffset;
		int								m_iDefaultLeftOffset;
		int								m_iCurrentImage;
		int								m_iCurrentImageOffset;
		int								m_iTargetImage;

		int								m_iChildrenHeight;
		int								m_iChildrenWidth;
		int								m_iChildrenOffest;

		bool							m_bEnableAuto;
		int								m_iAutoTimeCnt;
		bool							m_bBkimageMovable;
		int								m_iBkimageMove;

		int								m_iTimerCnt;
		int								m_iDefTimerCnt;
		RECT							m_rectImage;

	};
}

//--------------------------------------------------------------------------
#endif /* __ImageAnimation_h__ */
