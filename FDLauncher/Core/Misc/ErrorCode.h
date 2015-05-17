/* -------------------------------------------------------------------------
//	File Name	:	LibXLiveError.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-10 17:29:52
//	Description	:	
//
// -----------------------------------------------------------------------*/

#ifndef __LibXLiveError_H__
#define __LibXLiveError_H__

#include "CrossPlatformConfig.h"

// -------------------------------------------------------------------------

// Category Mask
const UInt32  EC_LOGIC_MASK		=  0x00000000U;
const UInt32  EC_CURL_MASK		=  0x00010000U;
const UInt32  EC_HTTP_MASK		=  0x00020000U;
const UInt32  EC_SQLITE_MASK	=  0x00080000U;

// Error No
enum ErrorCode
{
    // basic
	EC_OK                          = EC_LOGIC_MASK,
	EC_CURL_INTERERROR             ,
	EC_HTTP_NORETURN               ,
	EC_SERVER_UNKNOWN_ERROR        ,

	// logic
	EC_CANCEL                      ,
    EC_FAIL                     ,
	EC_LOCAL_FS_FAIL
};

#define IS_CURL_ERROR(err)                      (err & XLIVE_CURL_MASK)
#define IS_HTTP_ERROR(err)                      (err & XLIVE_HTTP_MASK)
#define IS_NET_ERROR(err)                       ((err & XLIVE_CURL_MASK) || (err & XLIVE_HTTP_MASK))
#define IS_SQLITE_ERROR(err)                    (err & XLIVE_SQLITE_MASK)

#define ERRORNO_TO_CURL(err)                    (err & (~XLIVE_CURL_MASK))
#define ERRORNO_TO_HTTP(err)                    (err & (~XLIVE_HTTP_MASK))
#define ERRORNO_TO_SQLITE(err)                  (err & (~XLIVE_SQLITE_MASK))

//--------------------------------------------------------------------------
#endif /* __LibXLiveError_H__ */



