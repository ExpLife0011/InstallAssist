#include "stdafx.h"
#include "MainFrameWnd.h"
#include "InstallManager.h"

#include <Shlwapi.h>
#include <io.h>
#include <Shlobj.h>

#define INIFILE_NAME	_T("Settings.ini")
#define SOFTWARE_NAME	_T("name")
#define SOFTWARE_DESC	_T("desc")
#define SOFTWARE_IMAGE	_T("image")
#define SOFTWARE_PATH	_T("path")
#define SOFTWARE_ARGS	_T("args")
#define SOFTWARE_HIDDEN	_T("hidden")
#define SOFTWARE_WAIT	_T("wait")

CInstallManager::CInstallManager(CMainFrameWnd* pMainFrameWnd)
	: m_pMainFrameWnd(pMainFrameWnd)
	, m_pIniFile(NULL)
	, m_hInstallThread(NULL)
{
}


CInstallManager::~CInstallManager()
{
	if (NULL != m_hInstallThread)
	{
		CloseHandle(m_hInstallThread);
		m_hInstallThread = NULL;
	}
}

void CInstallManager::Begin()
{
	DWORD dwThreadID = 0;
	m_hInstallThread = CreateThread(NULL, 0, InstallThread, this, 0, &dwThreadID);
}

void CInstallManager::End()
{
	if (NULL != m_hInstallThread)
	{
		DWORD dwWait = WaitForSingleObject(m_hInstallThread, INFINITE);
		if (dwWait == WAIT_OBJECT_0)
		{
			CloseHandle(m_hInstallThread);
			m_hInstallThread = NULL;
		}
	}
}

DWORD WINAPI CInstallManager::InstallThread(LPVOID lpParameter)
{
	CInstallManager*	pObjInstallManager = reinterpret_cast<CInstallManager*>(lpParameter);
	CMainFrameWnd*		pMainFrameWnd = pObjInstallManager->m_pMainFrameWnd;
	CIniFile*			pIniFile = pObjInstallManager->m_pIniFile;

	vector<tstring> setArray;

	// 获取程序当前目录
	TCHAR szCurrentPath[MAX_PATH] = { 0 };
	TCHAR szIniFilePath[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
	PathRemoveFileSpec(szCurrentPath);
	CopyMemory(szIniFilePath, szCurrentPath, MAX_PATH);
	PathAppend(szIniFilePath, INIFILE_NAME);

	// 初始化 ini 对象
	pIniFile = new CIniFile(szIniFilePath);
	if (NULL == pIniFile)
	{
		return -1;
	}

	pIniFile->ReadSection(setArray);
	if (!setArray.size())
	{
		return -1;
	}

	TCHAR szExecuteFilePath[MAX_PATH];
	for (size_t i = 0; i < setArray.size(); i++)
	{
		tstring		strName;
		tstring		strDesc;
		tstring		strImage;
		tstring		strPath;
		tstring		strArgs;
		INT			nHidden = 0;
		INT			nWait = 1;
		DWORD		dwResultCode = 1;
		CDuiString	strMessage;

		LONG		lBeginTime = 0;
		LONG		lEndTime = 0;

		pIniFile->ReadString(setArray[i].c_str(), SOFTWARE_NAME, strName);
		pIniFile->ReadString(setArray[i].c_str(), SOFTWARE_DESC, strDesc);
		pIniFile->ReadString(setArray[i].c_str(), SOFTWARE_IMAGE, strImage);
		pIniFile->ReadString(setArray[i].c_str(), SOFTWARE_PATH, strPath);
		pIniFile->ReadString(setArray[i].c_str(), SOFTWARE_ARGS, strArgs);
		pIniFile->ReadInt(setArray[i].c_str(), SOFTWARE_HIDDEN, nHidden);
		pIniFile->ReadInt(setArray[i].c_str(), SOFTWARE_WAIT, nWait);

		if (!strName.size() || !strDesc.size() ||
			!strImage.size() || !strPath.size())
		{
			continue;
		}

		lBeginTime = GetTickCount();

		// title
		strMessage.Format(_T("正在安装：%s"), strName.c_str());
		pMainFrameWnd->SetCurrentInstallText(strMessage);

		// description
		strMessage.Format(_T("软件描述：%s"), strDesc.c_str());
		pMainFrameWnd->SetDescriptionText(strMessage);

		// progress
		strMessage.Format(_T("安装进度：%d/%d"), i + 1, setArray.size());
		pMainFrameWnd->SetProgressText(strMessage);

		// image
		pMainFrameWnd->SetImage(strImage.c_str());

		// prgress per
		pMainFrameWnd->SetProgressValue(static_cast<int>((static_cast<float>(i) / setArray.size()) * 100));

		ZeroMemory(szExecuteFilePath, MAX_PATH);
		if (strPath.at(0) == _T('\\'))
		{
			CopyMemory(szExecuteFilePath, szCurrentPath, MAX_PATH);
		}
		PathAppend(szExecuteFilePath, strPath.c_str());

		strMessage.Format(_T("ExecuteFile: %s %s"), szExecuteFilePath, strArgs.c_str());
		OutputDebugString(strMessage);

		ExecuteFile(szExecuteFilePath,
			strArgs.c_str(),
			static_cast<BOOL>(nWait),
			&dwResultCode,
			static_cast<BOOL>(!nHidden));

		lEndTime = GetTickCount();

		if (lEndTime - lBeginTime < 1000)
		{
			Sleep(1000 - (lEndTime - lBeginTime));
		}
	}

	pMainFrameWnd->SetProgressValue(100);

	return 0;
}

HANDLE CInstallManager::ExecuteFile(
	LPCTSTR lpszExecuteFile, 
	LPCTSTR lpszCmdLineParamter /*= NULL*/, 
	BOOL bWaitFinish /*= FALSE*/, 
	DWORD *pdwRetExitCode /*= NULL*/, 
	BOOL bShow /*= TRUE*/)
{
	/* 执行程序之前，把程序所在的目录设置为当前运行目录 */
	CDuiString csPathTmp = lpszExecuteFile;
	int nIndex = csPathTmp.ReverseFind(_T('\\'));
	CDuiString csPath = csPathTmp.Left(nIndex);
	if (!csPath.IsEmpty())
	{
		SetCurrentDirectory(csPath);
	}

	SHELLEXECUTEINFO   sei;
	ZeroMemory(&sei, sizeof(sei));
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.hwnd = NULL;
	sei.fMask = SEE_MASK_FLAG_DDEWAIT | SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = _T("open");
	sei.lpFile = lpszExecuteFile;
	sei.lpParameters = lpszCmdLineParamter;
	sei.nShow = bShow ? SW_SHOWNORMAL : SW_HIDE;

	if (!ShellExecuteEx(&sei))
	{
		return NULL;
	}

	if (bWaitFinish)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
		if (pdwRetExitCode)
		{
			GetExitCodeProcess(sei.hProcess, pdwRetExitCode);
		}
		CloseHandle(sei.hProcess);
	}

	return sei.hProcess;
}

