/* -------------------------------------------------------------------------
//  File Name   :   NotificationCenter.h
//  Author      :   Zhang Fan
//  Create Time :   2013-1-7 10:58:13
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __NOTIFICATIONCENTER_H__
#define __NOTIFICATIONCENTER_H__

#include <atlsync.h>
#include <atlwin.h>
#include "INotificationCenter.h"

//-------------------------------------------------------------------------

class CNotificationCenter
    : public INotificationCenter
{
    class CClosure
    {
    public:
        const void*               m_pThis;
        NotificationDelegate      m_delegate;

    public:
        CClosure(const void* pthis, NotificationDelegate delegate)
            : m_pThis(pthis)
            , m_delegate(delegate)
        {

        }

        friend bool operator != (const CClosure& L, const CClosure& R)
        {
            return L.m_delegate != R.m_delegate;
        }

        friend bool operator == (const CClosure& L, const CClosure& R)
        {
            return L.m_delegate == R.m_delegate;
        }
    };

    class CSignalChannel
    {
    public:
         const std::string  m_notificationName;
         CClosure           m_closure;

    public:
        CSignalChannel(const std::string& notificationName, CClosure closure)
            : m_notificationName(notificationName)
            , m_closure(closure)
        {

        }

    public:
        bool friend operator != (const CSignalChannel& L, const CSignalChannel& R)
        {
            return (L.m_notificationName != R.m_notificationName) || (L.m_closure != R.m_closure);
        }

        bool friend operator == (const CSignalChannel& L, const CSignalChannel& R)
        {
            return (L.m_notificationName == R.m_notificationName) && (L.m_closure == R.m_closure);
        }
    };

    const static UINT WM_UM_POST_NOTIFICATION = WM_USER + 1;

    class CNotificationPoster: public CWindowImpl<CNotificationPoster, CWindow>
    {
    public:
        BEGIN_MSG_MAP(CNotificationPoster)
            MESSAGE_HANDLER(WM_UM_POST_NOTIFICATION, onPostNotification)
        END_MSG_MAP()

        LRESULT onPostNotification(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
        {
            ASSERT(wParam);
            ASSERT(lParam);

            CNotificationCenter*    pCenter         = (CNotificationCenter*)wParam;
            CNotification*          pNotification   = (CNotification*)lParam;

            pCenter->sendNotification(pNotification);

            return 0;
        }
    };

//---------------------------------------------------------------------------

private:
    CNotificationPoster         m_poster;
    std::list<CSignalChannel>   m_signalChannels;
    ATL::CCriticalSection       m_cs;

public:
    static CNotificationCenter& defaultCenter();

public:
    bool init(HWND hwnd);
    bool uninit();

public:
    void addObserver(const void* observer, NotificationDelegate delegate, const std::string& notificationName);
    void removeObserver(const void* observer);
    void removeObserver(const void* observer, const std::string& notificationName);

public:
    void sendNotification(const CNotification* notificaton);

    void postNotification(const CNotification* notificaton);
    void postNotification(const std::string& name);
    void postNotification(const std::string& name, const Json::Value& userInfo);

private:
    inline bool _signalChannelExist(const CSignalChannel& channel);
};

//--------------------------------------------------------------------------
#endif /* __NOTIFICATIONCENTER_H__ */