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

	static DWORD WINAPI InstallThread(LPVOID lpParameter);

	static HANDLE ExecuteFile(
		LPCTSTR lpszExecuteFile, 
		LPCTSTR lpszCmdLineParamter = NULL, 
		BOOL bWaitFinish = FALSE, 
		DWORD *pdwRetExitCode = NULL,
		BOOL bShow = TRUE
	);

private:
	CMainFrameWnd*	m_pMainFrameWnd;
	HANDLE			m_hInstallThread;
	CIniFile*		m_pIniFile;
};

