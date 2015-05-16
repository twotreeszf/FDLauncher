#ifndef __UIPIECHART_H__
#define __UIPIECHART_H__

#pragma once

#include <map>

namespace DuiLib {
    /////////////////////////////////////////////////////////////////////////////////////
    //

    struct PieBlock
    {
        CStdString  m_szName;
        CStdString  m_szDescription;
        float       m_fPercent;
        DWORD       m_dwFillColor;
        bool        m_bDrawBorder;
        DWORD       m_dwBorderColor;
        float       m_fBorderWidth;
        
        PieBlock()
            : m_fPercent(0.0f)
            , m_dwFillColor(0xFFFFFFFF)
            , m_bDrawBorder(false)
            , m_dwBorderColor(0x00000000)
            , m_fBorderWidth(1.0f)
        {

        }
    };

    class UILIB_API CPieChartUI: public CControlUI
    {
    public:
        CPieChartUI();
        ~CPieChartUI();
        virtual LPCTSTR GetClass() const;
        virtual LPVOID GetInterface(LPCTSTR pstrName);
        virtual void SetPos(RECT rc);
        virtual void DoEvent(TEventUI& event);
        virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
        virtual void PaintStatusImage(HDC hDC);

    public:
        void Update(LPCTSTR pstrName, float fPercent, LPCTSTR pstrDescription);

    private:
        inline DWORD _ColorValue(LPCTSTR szColor);

    protected:
        std::map<CStdString, PieBlock*> m_pieBlockMap;
        std::vector<PieBlock*>          m_displayVector;
        RECT    m_rcPieChart;
        RECT    m_rcLegendChart;
		RECT	m_rcTextPadding;
        float   m_fStartAngle;
		int		m_iLeftOffset;
    };

} // namespace DuiLib

#endif // __UIPIECHART_H__
