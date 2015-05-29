/* -------------------------------------------------------------------------
// �ļ���		:	common/commandlineoption.h
// ������		:	����
// ����ʱ��		:	2010/5/7 14:12:55
// ��������     :	
//
// $Id: $
// -----------------------------------------------------------------------*/
#ifndef __COMMON_COMMANDLINEOPTION_H__
#define __COMMON_COMMANDLINEOPTION_H__

#include <ShellAPI.h>
#include <vector>

//////////////////////////////////////////////////////////////////////////
//
class CCommandLineOption
{
public:
	typedef std::vector<CString> ParamsVector;

private:
	LPWSTR *m_argv;
	int m_nArgNum;
	CString m_strCmdLine;

public:
	CCommandLineOption()
	{
		m_argv = NULL;
	}
	~CCommandLineOption()
	{
		if (m_argv)
			::LocalFree(m_argv);
	}

	void Init(LPCWSTR szCommandLine)
	{
		m_argv = ::CommandLineToArgvW(szCommandLine, &m_nArgNum);
		m_strCmdLine = szCommandLine;
	}

	//////////////////////////////////////////////////////////////////////////
	// ����ת��
public:
	operator LPCWSTR() const
	{
		return (LPCWSTR)m_strCmdLine;
	}

	//////////////////////////////////////////////////////////////////////////
	// ��������
public:
	BOOL IsSwitchHasParams(LPCWSTR szSwitch, BOOL bInorgeCase = TRUE) // �жϲ��������Ƿ�Ϊ��
	{
		ParamsVector cmdlParamContent;
		GetSwitchParam(szSwitch, cmdlParamContent, bInorgeCase);

        if (cmdlParamContent.empty())
			return FALSE;
        
		return TRUE;
	}

	BOOL GetSwitchParam(LPCWSTR szSwitch, ParamsVector &cmdlParamContent, BOOL bInorgeCase = TRUE)
	{
		for (int i = 0; i < m_nArgNum; ++i)
		{
			if (_CompareString(m_argv[i], szSwitch, bInorgeCase) == 0)
			{
                for (int j = i + 1; j < m_nArgNum; ++j)
                {
					//�жϸ�ֵ�Ƿ��������в���
                    if (wcslen(m_argv[j]) > 0 && m_argv[j][0] == '-')
						return TRUE;
                    
					cmdlParamContent.push_back(m_argv[j]);
                }
			}
		}

		return cmdlParamContent.size() ? TRUE : FALSE;
	}

	BOOL HasSwitch(LPCWSTR szSwitch, BOOL bInorgeCase = TRUE)
	{
		for (int i = 0; i < m_nArgNum; ++i)
		{
			if (_CompareString(m_argv[i], szSwitch, bInorgeCase) == 0)
				return TRUE;
		}
		
		return FALSE;
	}

	BOOL FindSwitch(LPCWSTR szSwitch, CString &strSwitchContent, BOOL bInorgeCase = TRUE)
	{
		for (int i = 0; i < m_nArgNum; ++i)
		{
			if (_CompareString(m_argv[i], szSwitch, bInorgeCase) == 0)
			{
				ASSERT(i + 1 < m_nArgNum);
				if (i + 1 < m_nArgNum)
					strSwitchContent = m_argv[i + 1];
				return TRUE;
			}
		}

		return FALSE;
	}

	BOOL RemoveSwitch(LPCWSTR szSwitch, BOOL bInorgeCase = TRUE)
	{
		for (int i = 0; i < m_nArgNum; ++i)
		{
			if (_CompareString(m_argv[i], szSwitch, bInorgeCase) == 0)
			{
				m_argv[i] = L"";
				return TRUE;
			}
		}

		return FALSE;
	}

	void Clear()
	{
		m_strCmdLine.Empty();
		m_nArgNum = 0;
		if (m_argv)
		{
			::LocalFree(m_argv);
			m_argv = NULL;
		}
	}

private:
	inline int _CompareString(LPCWSTR sz1, LPCWSTR sz2, BOOL bInorge)
	{
		return bInorge ? _wcsicmp(sz1, sz2) : wcscmp(sz1, sz2);
	}
};

// -------------------------------------------------------------------------
// $Log: $

#endif /* __COMMON_COMMANDLINEOPTION_H__ */
