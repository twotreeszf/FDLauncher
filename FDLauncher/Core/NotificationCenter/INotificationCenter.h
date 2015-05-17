
/* -------------------------------------------------------------------------
//  File Name   :   INotificationCenter.h
//  Author      :   Zhang Fan
//  Create Time :   2013-6-24 10:23:21
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __INOTIFICATIONCENTER_H__
#define __INOTIFICATIONCENTER_H__

#include "../Misc/FastDelegate/FastDelegate.h"
#include "../JsonCpp/json.h"


class CNotification
{
public:
	const std::string     m_name;
	const Json::Value    m_userInfo;

public:
	explicit CNotification(const std::string& name)
		: m_name(name)
		, m_userInfo(NULL)
	{

	}
	
	explicit CNotification(const std::string& name, const Json::Value& userInfo)
		: m_name(name)
		, m_userInfo(userInfo)
	{

	}
};
//-------------------------------------------------------------------------
typedef fastdelegate::FastDelegate1<const CNotification*, void> NotificationDelegate;

//-------------------------------------------------------------------------
struct INotificationCenter
{
    virtual void addObserver(const void* observer, NotificationDelegate delegate, const std::string& notificationName) = 0;
    virtual void removeObserver(const void* observer) = 0;
    virtual void removeObserver(const void* observer, const std::string& notificationName) = 0;
    virtual void sendNotification(const CNotification* notificaton) = 0;
    virtual void postNotification(const CNotification* notificaton) = 0;
    virtual void postNotification(const std::string& name) = 0;
    virtual void postNotification(const std::string& name, const Json::Value& userInfo) = 0;
};

//--------------------------------------------------------------------------
#endif /* __INOTIFICATIONCENTER_H__ */