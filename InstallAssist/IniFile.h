#pragma once
#include <vector>
#include <string>

#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

class CIniFile
{
public:
	CIniFile() : m_strFile(_T("")) {}
	CIniFile(const tstring& strFile) : m_strFile(strFile) 
	{
		SetFileAttributes( m_strFile.c_str(), FILE_ATTRIBUTE_NORMAL );
	}
	~CIniFile() {}

	void SetFileName(const tstring& strFile) 
	{ 
		m_strFile = strFile; 
		SetFileAttributes( m_strFile.c_str(), FILE_ATTRIBUTE_NORMAL );
	}

	const tstring& GetFileName() const 
	{
		return m_strFile;
	}

	void ReadSection(std::vector<tstring>& setArray)
	{
		TCHAR szBuf[1024*64];
		ZeroMemory(szBuf, sizeof(szBuf));
		TCHAR* p = szBuf;
		int nLen = 0;

		if (GetPrivateProfileString(NULL, NULL, _T(""), 
			szBuf, sizeof(szBuf), 
			m_strFile.c_str()) > 0)
		{
			while (*p != '\0')
			{
				setArray.push_back(p);
				nLen = (int)_tcslen(p) + 1;
				p += nLen;
			}  
		}
	}
	
	void ReadSectionEntry(LPCTSTR pSection, std::vector<tstring>& setArray)
	{
	    TCHAR szBuf[1024*64] = {0};
		TCHAR* p = szBuf;
	    int nLen = 0;
	    if (GetPrivateProfileString(pSection, NULL, _T(""), 
	        szBuf, sizeof(szBuf), 
	        m_strFile.c_str()) > 0)
	    {
	        while (*p != '\0')
	        {
	            setArray.push_back(p);
	            nLen = (int)_tcslen(p) + 1;
	            p += nLen;
	        }  
	    }
	}
	void ReadSectionString(LPCTSTR lpSection, std::vector<tstring>& setArray)
	{
		std::vector<tstring> ayKey;
		tstring strItem;
		ReadSectionEntry(lpSection, ayKey);
		for (size_t i=0; i<ayKey.size(); ++i)
		{
			ReadString(lpSection, ayKey[i].c_str(), strItem);
			if (!strItem.empty())
				setArray.push_back(strItem);
		}
	}
	void ReadString(LPCTSTR pSection, LPCTSTR pEntry, tstring& strItem)
	{
		TCHAR szReturn[1024*4];
		memset(szReturn, 0, sizeof(szReturn));
		strItem.empty();
		if (GetPrivateProfileString(pSection, pEntry, _T(""), 
									szReturn, sizeof(szReturn), 
									m_strFile.c_str()) > 0)
			strItem = szReturn;
	}
	
	void ReadInt(LPCTSTR pSection, LPCTSTR pEntry, int& nValue)
	{
		TCHAR szReturn[32];
		memset(szReturn, 0, sizeof(szReturn));
		if (GetPrivateProfileString(pSection, pEntry, _T(""), 
									szReturn, sizeof(szReturn), 
									m_strFile.c_str()) > 0)
			nValue = _tstoi(szReturn);
		else
			nValue = 0;
	}
		
	void ReadInt(LPCTSTR pSection, LPCTSTR pEntry, short& nValue)
	{
		int nIntValue = 0;
		ReadInt(pSection, pEntry, nIntValue);
		nValue = (short)nIntValue;
	}

	void WriteString(LPCTSTR pSection, LPCTSTR pEntry, LPCTSTR pItem)
	{
		WritePrivateProfileString(pSection, pEntry, pItem, m_strFile.c_str());
	}
	
	void WriteInt(LPCTSTR pSection, LPCTSTR pEntry, int nValue)
	{
	    TCHAR szValue[32] = {0};
		_stprintf_s(szValue, sizeof(szValue), _T("%d"), nValue);
		WriteString(pSection, pEntry, szValue);
	}
	
	void EraseSection(LPCTSTR pSection)
	{
		WritePrivateProfileStruct(pSection, NULL, NULL, 0, m_strFile.c_str());
	}

protected:
	tstring m_strFile;
};
