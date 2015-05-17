/* -------------------------------------------------------------------------
//	File Name	:	LibCurlHttpGetRequest.cpp
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-20 9:42:43
//	Description	:	
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "LibCurlHttpGetRequest.h"

// -------------------------------------------------------------------------

namespace LibCurl
{
    CHttpGetRequest::CHttpGetRequest(const TransferSetting &transferSettings)
        : CHttpBase(transferSettings)
    {

    }

    CHttpGetRequest::CHttpGetRequest()
    {

    }

    CHttpGetRequest::~CHttpGetRequest()
    {

    }

    ErrorCode CHttpGetRequest::requestURL(const char* url, UInt32 timeoutSec)
    {
        X_ASSERT(url && *url);

        ErrorCode ret = EC_OK;
        {
            setURL(url);
            setTimeout(timeoutSec * 1000);

            ret = perform();
            ERROR_CHECK_BOOL(EC_OK == ret);
        }

Exit0:
        return ret;
    }

    void CHttpGetRequest::_curlSetDIY()
    {
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_CONNECTTIMEOUT, 10);
    }
}

//--------------------------------------------------------------------------
