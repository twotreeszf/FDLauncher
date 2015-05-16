/* -------------------------------------------------------------------------
//	File Name	:	ErrorCheck.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-19 16:49:55
//	Description	:   error check and code path control
//
// -----------------------------------------------------------------------*/

#ifndef __ERRORCHECK_H__
#define __ERRORCHECK_H__

#include "CrossPlatformConfig.h"

#define LOG_ERROR     void(0)

//---------------------------------------------------------------------------
#if defined(_DEBUG) && !defined(DISABLE_ASSERT)

    #if defined (_WIN32) || defined(_WIN64)
        #include <crtdbg.h>
        #define XLIVE_ASSERT(exp)               \
        do                                      \
        {                                       \
            _ASSERT(exp);                       \
            if (!(exp))                         \
            {                                   \
                LOG_ERROR(#exp);                \
            }                                   \
        } while (0)
        
    #elif defined (__APPLE__)
        #include <CoreFoundation/CoreFoundation.h>
        #define XLIVE_ASSERT(exp)\
        do                                      \
        {                                       \
            if (!(exp))                         \
            {                                   \
                CFUserNotificationDisplayAlert(10, kCFUserNotificationNoteAlertLevel, NULL, NULL, NULL, CFSTR(#exp), NULL, NULL, NULL, NULL, NULL);\
                asm("int $3");                  \
                LOG_ERROR(#exp);                \
            }                                   \
        } while (0)
            
    #else
        #include <assert.h>
        #define XLIVE_ASSERT(exp)               \
        do                                      \
        {                                       \
            assert(exp);                        \
            if (!(exp))                         \
            {                                   \
                LOG_ERROR(#exp);                \
            }                                   \
        } while (0)
        
    #endif
#else
    #define XLIVE_ASSERT(exp)                   \
    do                                          \
    {                                           \
        if (!(exp))                             \
        {                                       \
            LOG_ERROR(#exp);                    \
        }                                       \
    } while (0)
    
#endif

// -------------------------------------------------------------------------

#define CHECK_BOOL(exp)														\
    do {																	\
        if (!(exp))															\
        {																	\
            goto Exit0;														\
        }																	\
    } while(0)

#define ERROR_CHECK_BOOL(exp)												\
    do {																	\
    if (!(exp))															    \
        {																	\
        XLIVE_ASSERT(!"ERROR_CHECK_BOOL:" #exp);							\
        goto Exit0;														    \
        }																	\
    } while(0)

#define CHECK_BOOLEX(exp, exp1)												\
    do {																	\
    if (!(exp))															    \
        {																	\
        exp1;															    \
		LOG_INFO("CHECK_BOOLEX:" #exp);										\
        goto Exit0;														    \
        }																	\
    } while(0)

#define ERROR_CHECK_BOOLEX(exp, exp1)										\
    do {																	\
    if (!(exp))			    												\
        {																	\
        XLIVE_ASSERT(!"ERROR_CHECK_BOOLEX" #exp);							\
        exp1;															    \
        goto Exit0;														    \
        }																	\
    } while(0)

#define QUIT()          \
    do                  \
    {                   \
    goto Exit0;         \
    } while (0)

//--------------------------------------------------------------------------
#endif /* __ERRORCHECK_H__ */