#include "StdAfx.h"

#include <cstdlib>
#include <ctime>

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

const int CYCLE_ANGLE               = 360;
const LONG DEFAULT_PADDING          = 3L;
const LONG HORIZ_SPACING            = 28L;
const LONG VERT_SPACING             = 10L;				//between items
const LONG REF_CYCLE_WIDTH          = 16L;
const LONG REF_CYCLE_HEIGHT         = REF_CYCLE_WIDTH;	//item's name height
const LONG REF_TEXT_HEIGHT          = REF_CYCLE_HEIGHT;
const int REF_CYCLE_TEXT_OFFSET     = 4L;				//between chart and items
const DWORD REF_TEXT_COLOR          = 0xFF000000;

    CPieChartUI::CPieChartUI()
    {
        ::ZeroMemory(&m_rcPieChart, sizeof(RECT));
        ::ZeroMemory(&m_rcLegendChart, sizeof(RECT));

        srand(static_cast<unsigned int>(time(NULL)));
        m_fStartAngle = (float)(rand() % CYCLE_ANGLE);
    }

    CPieChartUI::~CPieChartUI()
    {
        std::map<CStdString, PieBlock*>::iterator it = m_pieBlockMap.begin();
        for(; m_pieBlockMap.end() != it; ++it)
        {
            delete it->second;
        }
        m_pieBlockMap.clear();
        m_displayVector.clear();
    }

    LPCTSTR CPieChartUI::GetClass() const
    {
        return _T("PieChartUI");
    }

    LPVOID CPieChartUI::GetInterface(LPCTSTR pstrName)
    {
        if( _tcscmp(pstrName, _T("PieChart")) == 0 ) return static_cast<CPieChartUI*>(this);
        return CControlUI::GetInterface(pstrName);
    }

    void CPieChartUI::SetPos(RECT rc)
    {
        CControlUI::SetPos(rc);

        m_rcPieChart.left   = m_rcItem.left + DEFAULT_PADDING;
        m_rcPieChart.right  = m_rcItem.right - DEFAULT_PADDING;
        m_rcPieChart.top    = m_rcItem.top + DEFAULT_PADDING;
        m_rcPieChart.bottom = m_rcItem.bottom - DEFAULT_PADDING;
        m_rcLegendChart = m_rcPieChart;
        LONG lWidth     = m_rcPieChart.right - m_rcPieChart.left;
        LONG lHeight    = m_rcPieChart.bottom - m_rcPieChart.top;
        if(lWidth > lHeight)
        {
            m_rcPieChart.left       = m_rcPieChart.left + m_iLeftOffset;//(lWidth -  3 * lHeight / 2) / 2;
            m_rcPieChart.right      = m_rcPieChart.left + lHeight; 
            m_rcLegendChart.left    = m_rcPieChart.right;
            m_rcLegendChart.right   = m_rcItem.right - DEFAULT_PADDING;
        }
        else
        {
            m_rcPieChart.top        = m_rcPieChart.top + (lHeight - 3 * lWidth / 2) / 2;
            m_rcPieChart.bottom     = m_rcPieChart.top + lWidth;
            m_rcLegendChart.top     = m_rcPieChart.bottom;
            m_rcLegendChart.bottom  = m_rcItem.bottom - DEFAULT_PADDING;
        }
    }

    void CPieChartUI::DoEvent(TEventUI& event)
    {
        if( event.Type == UIEVENT_MOUSEENTER )
        {
            return;
        }
        if( event.Type == UIEVENT_MOUSELEAVE )
        {
            return;
        }
        CControlUI::DoEvent(event);
    }

    void CPieChartUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        ASSERT(pstrName);
        ASSERT(pstrValue);
        if(_tcscmp(pstrName, _T("blocks")) == 0)
        {
            CStdString szKey;
            CStdString szValue;
            bool isValue        = false;
            bool isString       = false;
            PieBlock* pBlock    = NULL;

            // Json style
            // Value don't contain the seperators(exception string with blank space) below
            for(LPCTSTR pCursor = pstrValue; *pCursor != _T('\0'); ++pCursor)
            {
                if(*pCursor == _T('{'))
                {
                    pBlock = new PieBlock();
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
                        else if(_tcscmp(szKey, _T("description")) == 0) pBlock->m_szDescription = szValue;
                        else if(_tcscmp(szKey, _T("percent")) == 0) pBlock->m_fPercent = static_cast<float>(_tstof(szValue)) / 100.0f;
                        else if(_tcscmp(szKey, _T("fillcolor")) == 0) pBlock->m_dwFillColor = _ColorValue(szValue);
                        else if(_tcscmp(szKey, _T("drawborder")) == 0) pBlock->m_bDrawBorder = (szValue == _T("true"));
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
                    else if(_tcscmp(szKey, _T("description")) == 0) pBlock->m_szDescription = szValue;
                    else if(_tcscmp(szKey, _T("percent")) == 0) pBlock->m_fPercent = static_cast<float>(_tstof(szValue)) / 100.0f;
                    else if(_tcscmp(szKey, _T("fillcolor")) == 0) pBlock->m_dwFillColor = _ColorValue(szValue);
                    else if(_tcscmp(szKey, _T("drawborder")) == 0) pBlock->m_bDrawBorder = (szValue == _T("true"));
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

            std::vector<PieBlock*>::iterator it = m_displayVector.begin();
            for(; m_displayVector.end() != it; ++it)
            {
                std::map<CStdString, PieBlock*>::iterator mapIt = m_pieBlockMap.find((*it)->m_szName);
                if(m_pieBlockMap.end() != mapIt)
                {
                    delete mapIt->second;
                }
                m_pieBlockMap[(*it)->m_szName] = *it;
            }
        }
        else
        {
			if( _tcscmp(pstrName, _T("textPadding")) == 0 ) {
				LPTSTR pstr = NULL;
				m_rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
				m_rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
				m_rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
				m_rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
				return;
			}
			else if( _tcscmp(pstrName, _T("leftoffset")) == 0 ) {
				m_iLeftOffset = _ttoi(pstrValue);
				return;
			}
            CControlUI::SetAttribute(pstrName, pstrValue);
        }
    }

    void CPieChartUI::PaintStatusImage(HDC hDC)
    {
        DWORD dwCount = 0L;
        std::vector<PieBlock*>::iterator it = m_displayVector.begin();
        for(; m_displayVector.end() != it; ++it)
        {
            if((*it)->m_fPercent > 0.0f) ++dwCount;
        }

        RECT rcRefCycle;
        RECT rcRefText;
        LONG lSpace         = REF_CYCLE_HEIGHT + VERT_SPACING;
        LONG lHeight        = (dwCount * REF_CYCLE_HEIGHT + (dwCount - 1) * VERT_SPACING);
        rcRefCycle.left     = m_rcLegendChart.left + m_rcTextPadding.left;//HORIZ_SPACING;
        rcRefCycle.top      = m_rcLegendChart.top + ((m_rcLegendChart.bottom - m_rcLegendChart.top) - lHeight) / 2;
        rcRefCycle.right    = rcRefCycle.left + REF_CYCLE_WIDTH;
        rcRefCycle.bottom   = rcRefCycle.top + REF_CYCLE_HEIGHT;
        rcRefText.left      = rcRefCycle.right + REF_CYCLE_TEXT_OFFSET;
        rcRefText.top       = rcRefCycle.top + (REF_CYCLE_HEIGHT - REF_TEXT_HEIGHT) / 2;
        rcRefText.right     = m_rcLegendChart.right - m_rcTextPadding.left;//HORIZ_SPACING;
        rcRefText.bottom    = rcRefText.top + REF_TEXT_HEIGHT;

        float fStartAngle   = m_fStartAngle;
        float fSweepAngle   = 0.0f;
        PieBlock* pPieBlock = NULL;

		rcRefCycle.top      += m_rcTextPadding.top;
		rcRefCycle.bottom   += m_rcTextPadding.top;
		rcRefText.top       += m_rcTextPadding.top;
		rcRefText.bottom    += m_rcTextPadding.top;

        for(it = m_displayVector.begin(); m_displayVector.end() != it; ++it)
        {
            pPieBlock   = *it;
            if(pPieBlock->m_fPercent > 0.0f)
            {
                fSweepAngle = CYCLE_ANGLE * pPieBlock->m_fPercent;
                CRenderEngine::DrawFanShape(hDC, m_rcPieChart, fStartAngle, fSweepAngle, pPieBlock->m_dwFillColor, 
                    pPieBlock->m_bDrawBorder && dwCount > 1, pPieBlock->m_dwBorderColor, pPieBlock->m_fBorderWidth);
                CRenderEngine::FillEllipse(hDC, rcRefCycle, pPieBlock->m_dwFillColor);
                CRenderEngine::DrawText(hDC, m_pManager, rcRefText, pPieBlock->m_szDescription, REF_TEXT_COLOR, -1, DT_SINGLELINE);

                fStartAngle         += fSweepAngle;
                rcRefCycle.top      += lSpace;
                rcRefCycle.bottom   += lSpace;
                rcRefText.top       += lSpace;
                rcRefText.bottom    += lSpace;
            }
        }
    }

    void CPieChartUI::Update(LPCTSTR pstrName, float fPercent, LPCTSTR pstrDescription)
    {
        std::map<CStdString, PieBlock*>::iterator it = m_pieBlockMap.find(pstrName);
        if(m_pieBlockMap.end() != it)
        {
            it->second->m_fPercent      = fPercent;
            it->second->m_szDescription = pstrDescription;
        }
    }

    // "#FFABCDEF" style
    DWORD CPieChartUI::_ColorValue(LPCTSTR szColor)
    {
        while( *szColor > _T('\0') && *szColor <= _T(' ') ) szColor = ::CharNext(szColor);
        if( *szColor == _T('#')) szColor = ::CharNext(szColor);

        LPTSTR szStr    = NULL;
        DWORD dwColor   = _tcstoul(szColor, &szStr, 16);

        return dwColor;
    }

} // namespace DuiLib
