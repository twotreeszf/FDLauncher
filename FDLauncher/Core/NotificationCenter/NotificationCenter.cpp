/* -------------------------------------------------------------------------
//  File Name   :   NotificationCenter.cpp
//  Author      :   Zhang Fan
//  Create Time :   2013-1-7 10:58:27
//  Description :   
//
// -----------------------------------------------------------------------*/

#include "StdAfx.h"
#include "NotificationCenter.h"

namespace 
{
	class CSingleLock
	{
	public:
		CCriticalSection *m_pcs;
		CSingleLock(CCriticalSection *pcs)
		{
			m_pcs = pcs;
			m_pcs->Enter();
		}
		~CSingleLock()
		{
			m_pcs->Leave();
		}
	};

}

//-------------------------------------------------------------------------

CNotificationCenter g_defaultCenter;

CNotificationCenter& CNotificationCenter::defaultCenter()
{
    return g_defaultCenter;
}

bool CNotificationCenter::init(HWND hwnd)
{
    ASSERT(!m_poster.IsWindow());
    return m_poster.Create(hwnd) != NULL;
}

bool CNotificationCenter::uninit()
{
    if(m_poster.IsWindow())
        m_poster.DestroyWindow();

    {
        CSingleLock lock(&m_cs);
        m_signalChannels.clear();
    }

    return true;
}

void CNotificationCenter::addObserver(const void* observer, NotificationDelegate delegate, const std::string& notificationName)
{
    ASSERT(observer);
    ASSERT(!delegate.empty());
    ASSERT(notificationName.length());

    CSignalChannel channel(notificationName, CClosure(observer, delegate));

    if (!_signalChannelExist(channel))
    {
        CSingleLock lock(&m_cs);

        m_signalChannels.push_back(channel);
    }
}

void CNotificationCenter::removeObserver(const void* observer)
{
    ASSERT(observer);

    CSingleLock lock(&m_cs);

    for (std::list<CSignalChannel>::iterator it = m_signalChannels.begin(); it != m_signalChannels.end(); NULL)
    {
        if (observer == it->m_closure.m_pThis)
            it = m_signalChannels.erase(it);
        else
            ++it;
    }
}

void CNotificationCenter::removeObserver(const void* observer, const std::string& notificationName)
{
    ASSERT(observer);
    ASSERT(notificationName.length());

    CSingleLock lock(&m_cs);

    for (std::list<CSignalChannel>::iterator it = m_signalChannels.begin(); it != m_signalChannels.end(); NULL)
    {
        if ((observer == it->m_closure.m_pThis) && (notificationName == it->m_notificationName))
            it = m_signalChannels.erase(it);
        else
            ++it;
    }
}

void CNotificationCenter::sendNotification(const CNotification* notificaton)
{
    ASSERT(notificaton);
    ASSERT(notificaton->m_name.length());

    std::vector<NotificationDelegate> delegates;
    {
        CSingleLock lock(&m_cs);

        for (std::list<CSignalChannel>::iterator it = m_signalChannels.begin(); it != m_signalChannels.end(); ++it)
            if ((notificaton->m_name == it->m_notificationName))
                delegates.push_back(it->m_closure.m_delegate);
    }
    
    std::for_each(delegates.begin(), delegates.end(), [=](NotificationDelegate& item) 
    { 
        item(notificaton);
    });

    delete notificaton;
}

void CNotificationCenter::postNotification(const CNotification* notificaton)
{
    m_poster.PostMessage(WM_UM_POST_NOTIFICATION, (WPARAM)this, (LPARAM)notificaton);
}

void CNotificationCenter::postNotification(const std::string& name)
{
    ASSERT(name.length());

    CNotification* pNotification = new CNotification(name);

    postNotification(pNotification);
}

void CNotificationCenter::postNotification(const std::string& name, const Json::Value& userInfo)
{
    ASSERT(name.length());
    ASSERT(userInfo);

    CNotification* pNotification = new CNotification(name, userInfo);

    postNotification(pNotification);
}

bool CNotificationCenter::_signalChannelExist(const CSignalChannel& channel)
{
    CSingleLock lock(&m_cs);

    for (std::list<CSignalChannel>::iterator it = m_signalChannels.begin(); it != m_signalChannels.end(); ++it)
        if (channel == *it)
            return true;

    return false;
}

//--------------------------------------------------------------------------