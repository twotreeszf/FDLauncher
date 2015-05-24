/* -------------------------------------------------------------------------
//  File Name   :   FileHash.h
//  Author      :   Li Jiahan
//  Create Time :   2012-3-22 14:10:14
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __FILEHASH_H__
#define __FILEHASH_H__

#include "ErrorCheck.h"
#include "FileConfig/FileConfig.h"
#include "FastDelegate/FastDelegate.h"

//-------------------------------------------------------------------------
template <typename THASH>
class CFileHash
{
public:
	typedef fastdelegate::FastDelegate0<bool>				CancelDelegate;
	typedef fastdelegate::FastDelegate1<double>				HashFileProgressDelegate;

private:
    THASH						m_alg;
	CancelDelegate				m_cancel;
	HashFileProgressDelegate	m_progress;
	UInt64						m_hashedSize;

public:
	CFileHash()	: m_progress(NULL), m_cancel(NULL), m_hashedSize(0)
	{
	}

	~CFileHash()
	{
	}

    ErrorCode hashFile(const std::string& filePath, Byte *result)
    {
		ErrorCode ret = EC_OK;
        FILE *fp = NULL;
        {
            Int32 tmpRet = 0;
			fp = ::fopen_x(filePath, "rb");
			ERROR_CHECK_BOOLEX(NULL != fp, ret = EC_LOCAL_FS_FAIL);

            const UInt32 BUFFER_SIZE	= 1024;
            Byte buf[BUFFER_SIZE]		= { 0 };
            Int32 len			        = 0;

            do 
            {
                len    = ::fread(buf, 1, BUFFER_SIZE, fp);
                tmpRet = ::ferror(fp);
				ERROR_CHECK_BOOLEX(0 == tmpRet, ret = EC_LOCAL_FS_FAIL);
                m_alg.update(buf, len);

				m_hashedSize += len;

				if (!m_cancel.empty() && m_cancel())
                    break;
                if (!m_progress.empty())
					m_progress(m_hashedSize);

            } while (len > 0);

            m_alg.final(result);
        }

Exit0:
        if (fp)
            ::fclose(fp);
        m_alg.reset();

        return ret;
    }

	void setCancelCallback(CancelDelegate cancel)
	{
		m_cancel = cancel;
	}

	void setProgressCallback(HashFileProgressDelegate progress)
	{
		m_progress = progress;
	}
};

//--------------------------------------------------------------------------
#endif /* __FILEHASH_H__ */