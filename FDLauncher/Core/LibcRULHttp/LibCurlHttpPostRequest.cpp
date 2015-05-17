/* -------------------------------------------------------------------------
//	File Name	:	LibCurlHttpPost.cpp
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-20 11:39:50
//	Description	:	
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "LibCurlHttpPostRequest.h"

// -------------------------------------------------------------------------

namespace LibCurl
{
    CHttpPostRequest::CHttpPostRequest()
        : m_isBinaryMode(false)
    {

    }

    CHttpPostRequest::CHttpPostRequest(const TransferSetting &tranferSetting)
        : m_isBinaryMode(false)
        , CHttpBase(tranferSetting)
    {

    }

    CHttpPostRequest::~CHttpPostRequest()
    {

    }

    void CHttpPostRequest::setBinaryMode(bool isBinaryMode)
    {
        m_isBinaryMode = isBinaryMode;
    }

    void CHttpPostRequest::setPostData(std::string &postData)
    {
        X_ASSERT(!postData.empty());

        m_postData = postData;
    }

    ErrorCode CHttpPostRequest::requestURL(const char* url, UInt32 timeoutSec)
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

    void CHttpPostRequest::_curlSetDIY()
    {
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_CONNECTTIMEOUT, 10);
        ::curl_easy_setopt(m_pLibCURL, CURLOPT_POST, true);

        X_ASSERT(!m_postData.empty());

        if (!m_isBinaryMode)
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_POSTFIELDS, m_postData.c_str());
        else
        {
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_POSTFIELDS, m_postData.data()); 
            ::curl_easy_setopt(m_pLibCURL, CURLOPT_POSTFIELDSIZE, m_postData.size());
        }
    }
}

//--------------------------------------------------------------------------
