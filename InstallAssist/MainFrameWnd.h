#pragma once

class CInstallManager;
class CMainFrameWnd : public WindowImplBase
{
public:
	CMainFrameWnd();
	~CMainFrameWnd();

	// …Ë÷√Õº∆¨øÿº˛Õº∆¨
	void SetImage(LPCTSTR pStrImage);
	void SetCurrentInstallText(LPCTSTR pStrText);
	void SetDescriptionText(LPCTSTR pStrText);
	void SetProgressText(LPCTSTR pStrText);
	void SetProgressValue(int nValue);

	static DWORD WINAPI WorkerThread(LPVOID lpParameter);

protected:
	virtual CDuiString	GetSkinFolder();
	virtual CDuiString	GetSkinFile();
	virtual LPCTSTR		GetWindowClassName(void) const;

	virtual void		Notify(TNotifyUI& msg);
	virtual void		InitWindow();

	virtual LRESULT		MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/);
	virtual LRESULT		OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	CWndShadow*			m_pWndShadow;
	CInstallManager*	m_pInstallManager;

	COptionUI*			m_pImage;
	CTextUI*			m_pCurrentInstall;
	CTextUI*			m_pDescription;
	CTextUI*			m_pProgress;
	CProgressUI*		m_pProgressUI;

	HANDLE				m_hWorkerThread;
};

