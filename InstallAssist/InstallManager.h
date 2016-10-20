#pragma once

using namespace std;

class CMainFrameWnd;
class CInstallManager
{
public:
	CInstallManager(CMainFrameWnd* pMainFrameWnd);
	~CInstallManager();

	void Begin();

	void End();

	static void InstallThread(
		CInstallManager* pObjInstallManager
	);

	static HANDLE ExecuteFile(
		LPCTSTR lpszExecuteFile, 
		LPCTSTR lpszCmdLineParamter = nullptr, 
		BOOL bWaitFinish = FALSE, 
		DWORD *pdwRetExitCode = nullptr,
		BOOL bShow = TRUE
	);

private:
	CMainFrameWnd*	m_pMainFrameWnd;
	thread*			m_pInstallThread;
	CIniFile*		m_pIniFile;
};

