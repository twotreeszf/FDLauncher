/* -------------------------------------------------------------------------
//	File Name	:	LibCurlHttpBase.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-19 17:10:44
//	Description	:
//
// -----------------------------------------------------------------------*/

#ifndef __LIBCURLHTTPBASE_H__
#define __LIBCURLHTTPBASE_H__

#include "../Misc/CrossPlatformConfig.h"
#include "../Misc/ErrorCode.h"

#include "curl/curl.h"
#include "../Misc/FastDelegate/FastDelegate.h"

#include <string>
#include <vector>

// -------------------------------------------------------------------------

namespace LibCurl
{
    typedef fastdelegate::FastDelegate0<bool> CancelDelegate;

	struct ProxyMode
	{
		enum type
		{
			NoProxy = 0,
			UsingBrowserProxy,
			ProxyHTTP,
			ProxySock4,
			ProxySock5
		};
	};

	struct ProxyInfo
	{
		std::string    m_ip;
		UInt32         m_port;
		bool           m_havepwd;
		std::string    m_user;
		std::string    m_passwd;

		ProxyInfo();
		void reset();
	};

	struct TransferSetting
	{
		UInt32                      m_uploadBandWidth;
		UInt32                      m_downloadBandWidth;
		ProxyMode::type    m_proxyMode;
		ProxyInfo              m_proxyInfo;

		TransferSetting();
		void reset();
	};

    class CHttpBase
    {
    protected:
        CURL*                       m_pLibCURL;
        std::string                 m_url;
        TransferSetting        m_transferSetting;
        
        curl_slist*                 m_pSendHeader;
        Int32                       m_timeOut;

        std::string                 m_recvData;
        std::string                 m_recvHeader;
        UInt32                      m_httpCode;
        std::vector<char>           m_errorMessage;
        

        CancelDelegate              m_cancelDelegate;

        // progress info
        double                      m_uploadTotal;
        double                      m_uploadNow;
        double                      m_downloadTotal;
        double                      m_downloadNow;
        double                      m_uploadSpeed;
        double                      m_downloadSpeed;

    public:
        static ErrorCode initGlobal();
        static void  uninitGlobal();

    public:
        CHttpBase();
        explicit CHttpBase(const TransferSetting& transferSetting);
        virtual ~CHttpBase();
        
        void setUploadBandWidth(UInt32 bandWidth);
        void setDownLoadBandWidth(UInt32 bandWidth);
        void setTimeout(Int32 timeout);
        void setURL(const char*url);
        void setSendHeader(const char* headers);
        void setProxymode(ProxyMode::type mode);
        void setProxyInfo(ProxyInfo &proxySetting);
        virtual void setCancelDelegate(CancelDelegate cancel);

        ErrorCode perform();

        std::string getErrorMessage();
        UInt32 getHttpCode();
        const std::string& getRecvData();
        const std::string& getRecvHeader();

    protected:
        static size_t   defaultWriteStreamFunc(void* ptr, size_t msize, size_t n, std::string* stream);
        static Int32    progressCallback(void* userData, double dlTotal, double dlNow, double ulTotal, double ulNow);
        static const Int32 CURL_HTTP_DEFAULT_TIMEOUT = (20 * 60 * 1000);

        void _curlSetUploadSpeed(UInt32 limit);                                   // CURLOPT_MAX_SEND_SPEED_LARGE
        void _curlSetDownloadSpeed(UInt32 limit);                                 // CURLOPT_MAX_RECV_SPEED_LARGE
        
    private:
        virtual ErrorCode _initCurl();
        virtual void    _cleanUpCurl();
        virtual bool    _isUsingHttps(); 
        
        void _curlSetUrlAddress();                                    // CURLOPT_URL
        void _curlSetTimeOutMS();                                     // CURLOPT_TIMEOUT_MS
        void _curlSetSendHeader();                                    // CURLOPT_HTTPHEADER
        void _curlSetProgressFunction();                              // CURLOPT_NOPROGRESS
        void _curlSetProxy();                                         // CURLOPT_PROXYTYPE...
        void _curlSetErrorBuffer();                                   // CURLOPT_ERRORBUFFER

        virtual void _curlSetRecvDataFunction();                      // CURLOPT_WRITEFUNCTION,  CURLOPT_WRITEDATA
        virtual void _curlSetRecvHeaderFunction();                    // CURLOPT_HEADERFUNCTION, CURLOPT_HEADERDATA
        virtual void _curlSetProtocols();                             // CURLOPT_PROTOCOLS, CURLOPT_SSL_VERIFYPEER
        virtual void _curlSetDIY();                                   // do it for yourself :-)
    };
}

//--------------------------------------------------------------------------
#endif /* __LIBCURLHTTPBASE_H__ */