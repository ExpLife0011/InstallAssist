#include "stdafx.h"
#include "MainFrameWnd.h"
#include "InstallManager.h"

#include <Shlwapi.h>

#define INIFILE_NAME	_T("Settings.ini")
#define SOFTWARE_NAME	_T("name")
#define SOFTWARE_DESC	_T("desc")
#define SOFTWARE_IMAGE	_T("image")
#define SOFTWARE_PATH	_T("path")
#define SOFTWARE_ARGS	_T("args")

CInstallManager::CInstallManager(CMainFrameWnd* pMainFrameWnd)
	: m_pMainFrameWnd(pMainFrameWnd)
	, m_pInstallThread(nullptr)
	, m_pIniFile(nullptr)
{
}


CInstallManager::~CInstallManager()
{
}

void CInstallManager::Begin()
{
	m_pInstallThread = new thread(InstallThread, this);
}

void CInstallManager::End()
{
	if (nullptr != m_pInstallThread)
	{
		m_pInstallThread->join();
	}
}

void CInstallManager::InstallThread(CInstallManager* pObjInstallManager)
{
	CMainFrameWnd*	pMainFrameWnd = pObjInstallManager->m_pMainFrameWnd;
	CIniFile*		pIniFile = pObjInstallManager->m_pIniFile;

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
	if (nullptr == pIniFile)
	{
		return;
	}

	pIniFile->ReadSection(setArray);
	if (!setArray.size())
	{
		return;
	}

	TCHAR szExecuteFilePath[MAX_PATH];
	for (size_t i = 0; i < setArray.size(); i++)
	{
		tstring	strName;
		tstring strDesc;
		tstring strImage;
		tstring strPath;
		tstring strArgs;
		CDuiString strMessage;

		pIniFile->ReadString(setArray[i].c_str(), SOFTWARE_NAME, strName);
		pIniFile->ReadString(setArray[i].c_str(), SOFTWARE_DESC, strDesc);
		pIniFile->ReadString(setArray[i].c_str(), SOFTWARE_IMAGE, strImage);
		pIniFile->ReadString(setArray[i].c_str(), SOFTWARE_PATH, strPath);
		pIniFile->ReadString(setArray[i].c_str(), SOFTWARE_ARGS, strArgs);

		if (!strName.size() || !strDesc.size() || 
			!strImage.size() ||!strPath.size())
		{
			continue;
		}

		strMessage.Format(_T("正在安装：%s"), strName.c_str());
		pMainFrameWnd->SetCurrentInstallText(strMessage);

		strMessage.Format(_T("软件描述：%s"), strDesc.c_str());
		pMainFrameWnd->SetDescriptionText(strMessage);
		
		strMessage.Format(_T("安装进度：%d/%d"), i + 1, setArray.size());
		pMainFrameWnd->SetProgressText(strMessage);

		pMainFrameWnd->SetImage(strImage.c_str());

		pMainFrameWnd->SetProgressValue(static_cast<int>(((i + 1.0f) / setArray.size()) * 100));

		ZeroMemory(szExecuteFilePath, MAX_PATH);
		CopyMemory(szExecuteFilePath, szCurrentPath, MAX_PATH);
		PathAppend(szExecuteFilePath, _T("\\"));
		PathAppend(szExecuteFilePath, strPath.c_str());

		strMessage.Format(_T("%s %s"), szExecuteFilePath, strArgs.c_str());
		OutputDebugString(strMessage);

		ExecuteFile(_T("cmd.exe"), _T("/c echo Hello InstallAssist&pause"), TRUE);
	}

	pMainFrameWnd->SetProgressValue(100);
}

HANDLE CInstallManager::ExecuteFile(
	LPCTSTR lpszExecuteFile, 
	LPCTSTR lpszCmdLineParamter /*= nullptr*/, 
	BOOL bWaitFinish /*= FALSE*/, 
	DWORD *pdwRetExitCode /*= nullptr*/, 
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

