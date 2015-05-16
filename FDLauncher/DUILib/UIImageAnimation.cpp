/* -------------------------------------------------------------------------
//
//	文件名		：	ImageAnimation.cpp
//	创建者		：	夏路遥
//	创建时间	：	2013-8-30 16:15:30
//	功能描述	：	
//
// -----------------------------------------------------------------------*/

#include "StdAfx.h"
#include "UIImageAnimation.h"

// -------------------------------------------------------------------------
namespace DuiLib
{
	CImageAnimationUI::CImageAnimationUI()
	{
		m_iCurrentImage = 0;
		m_iChildrenWidth = 0;
		m_iLeftOffset = 0;
		m_iBkimageMove = 0;
		m_iTargetImage = 0;
		m_bEnableAuto = false;
	}

	CImageAnimationUI::~CImageAnimationUI()
	{
		m_displayVector.clear();
	}

	LPCTSTR CImageAnimationUI::GetClass() const
	{
		return _T("ImageAnimationUI");
	}
	LPVOID CImageAnimationUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("ImageAnimation")) == 0 ) return static_cast<CImageAnimationUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	void CImageAnimationUI::SetPos( RECT rc )
	{
		m_rectImage = rc;
		CControlUI::SetPos(rc);
	}

	void CImageAnimationUI::DoEvent( TEventUI& event )
	{
		if( event.Type == UIEVENT_TIMER && event.wParam == PicMoveNxtTimerId)
		{
			OnImageMoveNxt();
		}
		else if( event.Type == UIEVENT_TIMER && event.wParam == PicMovePreTimerId)
		{
			OnImageMovePre();
		}
		else if(event.Type == UIEVENT_TIMER && event.wParam == AutoPicMoveNxtTimerId)
		{
			showNextImage();
		}
		CControlUI::DoEvent(event);
	}

	void CImageAnimationUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		ASSERT(pstrName);
		ASSERT(pstrValue);
		if(_tcscmp(pstrName, _T("blocks")) == 0)
		{
			CStdString szKey;
			CStdString szValue;
			bool isValue        = false;
			bool isString       = false;
			ImageBlock* pBlock    = NULL;
			for(LPCTSTR pCursor = pstrValue; *pCursor != _T('\0'); ++pCursor)
			{
				if(*pCursor == _T('{'))
				{
					pBlock = new ImageBlock();
				}
				else if(*pCursor == _T('\''))
				{
					if(isValue) isString = !isString;
				}
				else if(*pCursor == _T('='))
				{
					isValue = true;
				}
				else if(*pCursor == _T(','))
				{
					if(pBlock != NULL)
					{
						if(_tcscmp(szKey, _T("name")) == 0) pBlock->m_szName = szValue;
						else if(_tcscmp(szKey, _T("image")) == 0) pBlock->m_szImage = szValue;
						else if(_tcscmp(szKey, _T("bordercolor")) == 0) pBlock->m_dwBorderColor = _ColorValue(szValue);
						else if(_tcscmp(szKey, _T("borderwidth")) == 0) pBlock->m_fBorderWidth = static_cast<float>(_tstof(szValue));

						isValue = false;
						szKey.Empty();
						szValue.Empty();
					}
				}
				else if(*pCursor == _T('}'))
				{
					if(_tcscmp(szKey, _T("name")) == 0) pBlock->m_szName = szValue;
					else if(_tcscmp(szKey, _T("image")) == 0) pBlock->m_szImage = szValue;
					else if(_tcscmp(szKey, _T("bordercolor")) == 0) pBlock->m_dwBorderColor = _ColorValue(szValue);
					else if(_tcscmp(szKey, _T("borderwidth")) == 0) pBlock->m_fBorderWidth = static_cast<float>(_tstof(szValue));

					isValue = false;
					szKey.Empty();
					szValue.Empty();

					m_displayVector.push_back(pBlock);
					pBlock = NULL;
				}
				else if(*pCursor == _T(' '))
				{
					if(isString)
					{
						szValue += *pCursor;
					}
					else 
					{
						szKey.Empty();
						szValue.Empty();
					}
				}
				else
				{
					if(isValue) szValue += *pCursor;
					else szKey += *pCursor;
				}
			}
		}
		else if (_tcscmp(pstrName, _T("childrenheight")) == 0)
		{
			m_iChildrenHeight = _ttoi(pstrValue);
		}
		else if (_tcscmp(pstrName, _T("childrenwidth")) == 0)
		{
			m_iChildrenWidth = _ttoi(pstrValue);
		}
		else if (_tcscmp(pstrName, _T("childrenoffset")) == 0)
		{
			m_iChildrenOffest = _ttoi(pstrValue);
		}
		else if (_tcscmp(pstrName, _T("defaultleftOffset")) == 0)
		{
			m_iDefaultLeftOffset = _ttoi(pstrValue);
			m_iCurrentImageOffset = m_iDefaultLeftOffset;
			m_iLeftOffset = m_iDefaultLeftOffset;
		}
		else if(_tcscmp(pstrName, _T("imagemove")) == 0)
		{
			m_iBkimageMove = _ttoi(pstrValue);
		}
		else
		{
			CControlUI::SetAttribute(pstrName, pstrValue);
		}
	}
	void CImageAnimationUI::PaintBkImage(HDC hDC)
	{
		if( m_sBkImage.IsEmpty() ) return;
		if(!m_bBkimageMovable)
		{
			if( !DrawImage(hDC, (LPCTSTR)m_sBkImage) ) m_sBkImage.Empty();
		}
		else
		{
			CStdString newBkImage, tempStr;
			int l,r,t,b;
			l = m_iCurrentImage * m_iBkimageMove - m_iLeftOffset*m_iBkimageMove/(m_rectImage.right - m_rectImage.left);
			r = l + m_rectImage.right - m_rectImage.left;
			t = 0;
			b = m_rectImage.bottom - m_rectImage.top;
			newBkImage.Format(_T(" source='%d,%d,%d,%d'"), l, t, r, b);
			tempStr = m_sBkImage + newBkImage;
			if( !DrawImage(hDC, (LPCTSTR)tempStr) ) m_sBkImage.Empty();
		}
	}
	void CImageAnimationUI::PaintStatusImage( HDC hDC )
	{
		if( m_displayVector.empty()) return;

		int leftImage = (m_iCurrentImage + m_displayVector.size() -1) % m_displayVector.size();
		int rightImage = (m_iCurrentImage + 1) % m_displayVector.size();
		int iCurrentImage = m_iCurrentImage;
		PaintImage(hDC, m_displayVector[leftImage], m_iLeftOffset - m_iChildrenWidth - m_iChildrenOffest*2);
		PaintImage(hDC, m_displayVector[iCurrentImage], m_iLeftOffset);
		PaintImage(hDC, m_displayVector[rightImage], m_iLeftOffset + m_iChildrenWidth + m_iChildrenOffest*2);
	}

	void CImageAnimationUI::PaintImage( HDC hDC, ImageBlock* item, const int iLeftOffset )
	{
		if(iLeftOffset + m_iChildrenWidth < 0) return;
		if(iLeftOffset > m_cxyFixed.cx) return;
		RECT rect;
		rect.left = m_rectImage.left + iLeftOffset;
		rect.right = rect.left + m_iChildrenWidth;
		rect.top = m_rectImage.top;
		rect.bottom = rect.top + m_iChildrenHeight;
		RECT rcSource;
		rcSource.left = 0;
		rcSource.right = rect.right - rect.left;
		rcSource.top = 0;
		rcSource.bottom = rect.bottom - rect.top;
		if(rect.left < m_rectImage.left)
		{
			rcSource.left = m_rectImage.left - rect.left;
		}
		if(rect.right > m_rectImage.right)
		{
			rcSource.right = m_rectImage.right - rect.left;
		}

		if(rect.top < m_rectImage.top)
		{
			rcSource.top = m_rectImage.top - rect.top;
		}
		if(rect.bottom > m_rectImage.bottom)
		{
			rcSource.bottom = m_rectImage.bottom - rect.top;
		}
		CStdString ImageName, ImageParam;
		ImageName.Format(_T("file='%ws'"), item->m_szImage);
		ImageParam.Format(_T(" source='%d, %d, %d, %d'"), (int)rcSource.left, (int)rcSource.top, (int)rcSource.right, (int)rcSource.bottom);
		ImageName.Append(ImageParam);
		rect.left = max(rect.left, m_rectImage.left);
		rect.right = min(rect.right, m_rectImage.right);
		rect.top = max(rect.top, m_rectImage.top);
		rect.bottom = min(rect.bottom, m_rectImage.bottom);
		CRenderEngine::DrawImageString(hDC, m_pManager, rect, rect, ImageName);
	}

	void CImageAnimationUI::MoveUpdate( int fLeftOffset, int iCurrentImage, bool bOutside)
	{
		if(!bOutside || (m_iLeftOffset != fLeftOffset && m_iCurrentImage != iCurrentImage && bOutside))
		{
			m_iLeftOffset = fLeftOffset;
			m_iCurrentImage = iCurrentImage;
			Invalidate();
		}
	}

	void CImageAnimationUI::showNextImage()
	{
		m_pManager->SetTimer(this, PicMoveNxtTimerId, 20);
		m_pManager->KillTimer(this, PicMovePreTimerId);
		m_iTimerCnt = m_iDefTimerCnt/20;
		m_iTargetImage = ( m_iTargetImage + 1 ) % m_displayVector.size();
		if(m_bEnableAuto && m_iTargetImage == m_displayVector.size() - 1) m_pManager->KillTimer(this, AutoPicMoveNxtTimerId);
	}

	void CImageAnimationUI::showPreImage()
	{
		m_pManager->SetTimer(this, PicMovePreTimerId, 20);
		m_pManager->KillTimer(this, PicMoveNxtTimerId);
		m_iTimerCnt = m_iDefTimerCnt/20;
		m_iTargetImage = ( m_iTargetImage - 1 + m_displayVector.size()) % m_displayVector.size();
	}

	void CImageAnimationUI::OnImageMoveNxt()
	{
		int iImageTotalCnt = m_displayVector.size();
		m_iTimerCnt--;
		if(m_iTimerCnt == 0)
		{
			MoveUpdate(m_iDefaultLeftOffset, m_iCurrentImage);
			m_pManager->KillTimer(this, PicMoveNxtTimerId);
			return;
		}
		int cnt = (m_iTargetImage - m_iCurrentImage + iImageTotalCnt) % iImageTotalCnt;
		int totalDis = cnt*(m_iChildrenOffest*2 + m_iChildrenWidth) + m_iCurrentImageOffset - m_iDefaultLeftOffset;
		m_iCurrentImageOffset = m_iCurrentImageOffset - totalDis/m_iTimerCnt;
		while(m_iCurrentImageOffset + m_iChildrenWidth/2 <= 0 )
		{
			m_iCurrentImage = (m_iCurrentImage + 1) % iImageTotalCnt;
			if(m_btnVector[m_iCurrentImage])
				m_btnVector[m_iCurrentImage]->Selected(true);
			m_iCurrentImageOffset = m_iCurrentImageOffset +  m_iChildrenWidth + m_iChildrenOffest*2;
		}
		MoveUpdate(m_iCurrentImageOffset, m_iCurrentImage);
		return;
	}

	void CImageAnimationUI::OnImageMovePre()
	{
		int iImageTotalCnt = m_displayVector.size();
		m_iTimerCnt--;
		if(m_iTimerCnt == 0)
		{
			MoveUpdate(m_iDefaultLeftOffset, m_iCurrentImage);
			m_pManager->KillTimer(this, PicMovePreTimerId);
			return;
		}
		int cnt = (m_iCurrentImage - m_iTargetImage + iImageTotalCnt) % iImageTotalCnt;
		int totalDis = cnt*(m_iChildrenOffest*2 + m_iChildrenWidth) - m_iCurrentImageOffset + m_iDefaultLeftOffset;
		m_iCurrentImageOffset = m_iCurrentImageOffset + totalDis/m_iTimerCnt;
		while(m_iCurrentImageOffset >= m_rectImage.right - m_rectImage.left - m_iChildrenWidth/2)
		{
			m_iCurrentImage = (m_iCurrentImage + iImageTotalCnt - 1) % iImageTotalCnt;
			if(m_btnVector[m_iCurrentImage])
				m_btnVector[m_iCurrentImage]->Selected(true);
			m_iCurrentImageOffset = m_iCurrentImageOffset -  m_iChildrenWidth - m_iChildrenOffest*2;
		}
		MoveUpdate(m_iCurrentImageOffset, m_iCurrentImage);
		return;
	}

	void CImageAnimationUI::SetTargetImage( int iTargetImage )
	{
		m_iTargetImage = iTargetImage;
		int iImageCnt = m_displayVector.size();
		//if((m_iTargetImage - m_iCurrentImage + iImageCnt)% iImageCnt <= (m_iCurrentImage - m_iTargetImage + iImageCnt)% iImageCnt)
		if(m_iTargetImage >= m_iCurrentImage)
		{
			m_pManager->SetTimer(this, PicMoveNxtTimerId, 50);
			m_pManager->KillTimer(this, PicMovePreTimerId);
			if(m_iTargetImage != m_iCurrentImage)
				m_iTimerCnt = m_iDefTimerCnt/50;
		}
		else
		{
			m_pManager->SetTimer(this, PicMovePreTimerId, 50);
			m_pManager->KillTimer(this, PicMoveNxtTimerId);
			if(m_iTargetImage != m_iCurrentImage)
				m_iTimerCnt = m_iDefTimerCnt/50;
		}
		if(m_bEnableAuto) m_pManager->KillTimer(this, AutoPicMoveNxtTimerId);
	}

	void CImageAnimationUI::SetAutoImageChangeEnable(bool bEnable, int iAutoTimeCnt )
	{
		if(m_bEnableAuto == bEnable) return;
		m_bEnableAuto = bEnable;
		m_iAutoTimeCnt = iAutoTimeCnt;
		if(bEnable)
		{
			m_pManager->SetTimer(this, AutoPicMoveNxtTimerId, iAutoTimeCnt);
		}
		else
		{
			m_pManager->KillTimer(this, AutoPicMoveNxtTimerId);
		}
	}
	int CImageAnimationUI::GetChildrenHeight()
	{
		return m_iChildrenHeight;
	}

	int CImageAnimationUI::GetChildrenWidth()
	{
		return m_iChildrenWidth;
	}

	int CImageAnimationUI::GetOffset()
	{
		return m_iChildrenOffest;
	}



	DWORD CImageAnimationUI::_ColorValue(LPCTSTR szColor)
	{
		while( *szColor > _T('\0') && *szColor <= _T(' ') ) szColor = ::CharNext(szColor);
		if( *szColor == _T('#')) szColor = ::CharNext(szColor);

		LPTSTR szStr    = NULL;
		DWORD dwColor   = _tcstoul(szColor, &szStr, 16);

		return dwColor;
	}

	void CImageAnimationUI::SetRelativeBtn(std::vector<COptionUI*> btnList)
	{
		m_btnVector = btnList;
	}

}


//--------------------------------------------------------------------------
