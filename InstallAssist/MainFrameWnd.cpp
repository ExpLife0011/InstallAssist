#include "stdafx.h"
#include "InstallManager.h"
#include "MainFrameWnd.h"


CMainFrameWnd::CMainFrameWnd()
	: m_pWndShadow(NULL)
	, m_pImage(NULL)
	, m_pCurrentInstall(NULL)
	, m_pDescription(NULL)
	, m_pProgress(NULL)
	, m_pInstallManager(NULL)
	, m_pProgressUI(NULL)
	, m_hWorkerThread(NULL)
{
}

CMainFrameWnd::~CMainFrameWnd()
{
	if (NULL != m_pWndShadow)
	{
		delete m_pWndShadow;
		m_pWndShadow = NULL;
	}

	if (NULL != m_pInstallManager)
	{
		delete m_pInstallManager;
		m_pInstallManager = NULL;
	}
	
	if (NULL != m_hWorkerThread)
	{
		CloseHandle(m_hWorkerThread);
		m_hWorkerThread = NULL;
	}
}

CDuiString CMainFrameWnd::GetSkinFolder()
{
	return m_PaintManager.GetInstancePath();
}

CDuiString CMainFrameWnd::GetSkinFile()
{
	return _T("InstallAssist.xml");
}

LPCTSTR CMainFrameWnd::GetWindowClassName(void) const
{
	return _T("InstallAssist");
}

void CMainFrameWnd::Notify(TNotifyUI& msg)
{
	__super::Notify(msg);

	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btnHello"))
		{
			::MessageBox(NULL, _T("我是按钮"), _T("点击了按钮"), NULL);
		}
		if (msg.pSender->GetName() == _T("closebtn"))
		{
			Close();
			PostQuitMessage(0);
		}
	}
}

void CMainFrameWnd::InitWindow()
{
	DWORD dwThread;

	m_pImage = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("image")));
	m_pCurrentInstall = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("current")));
	m_pDescription = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("discription")));
	m_pProgress = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("progress")));
	m_pProgressUI = static_cast<CProgressUI*>(m_PaintManager.FindControl(_T("down_progress_front")));

	m_hWorkerThread = CreateThread(NULL, 0, WorkerThread, this, 0, &dwThread);
}

LRESULT CMainFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/)
{
	if (uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
			return FALSE;
		default:
			break;
		}
	}

	return FALSE;
}

LRESULT CMainFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);

	CWndShadow::Initialize(m_PaintManager.GetInstance());
	m_pWndShadow = new CWndShadow;
	m_pWndShadow->Create(m_hWnd);

#if 0
	RECT rcCorner = { 5,5,5,5 };
	RECT rcHoleOffset = { 0,0,0,0 };
	m_pWndShadow->SetImage(_T("bg_main_shadow.png"), rcCorner, rcHoleOffset);
#else
	m_pWndShadow->SetDarkness(80);
	m_pWndShadow->SetSize(4);
	m_pWndShadow->SetPosition(0, 0);
#endif
	
	return 0;
}

void CMainFrameWnd::SetImage(LPCTSTR pStrImage)
{
	if (NULL != m_pImage)
	{
		m_pImage->SetForeImage(pStrImage);
	}
}

void CMainFrameWnd::SetCurrentInstallText(LPCTSTR pStrText)
{
	if (NULL != m_pCurrentInstall)
	{
		m_pCurrentInstall->SetText(pStrText);
	}
}

void CMainFrameWnd::SetDescriptionText(LPCTSTR pStrText)
{
	if (NULL != m_pDescription)
	{
		m_pDescription->SetText(pStrText);
	}
}

void CMainFrameWnd::SetProgressText(LPCTSTR pStrText)
{
	if (NULL != m_pProgress)
	{
		m_pProgress->SetText(pStrText);
	}
}

void CMainFrameWnd::SetProgressValue(int nValue)
{
	m_pProgressUI->SetValue(nValue);
}

DWORD WINAPI CMainFrameWnd::WorkerThread(LPVOID lpParameter)
{
	CMainFrameWnd* pMainFrameWnd = reinterpret_cast<CMainFrameWnd*>(lpParameter);

	pMainFrameWnd->m_pInstallManager = new CInstallManager(pMainFrameWnd);

	pMainFrameWnd->m_pInstallManager->Begin();
	pMainFrameWnd->m_pInstallManager->End();

	if (NULL != pMainFrameWnd->m_pInstallManager)
	{
		delete pMainFrameWnd->m_pInstallManager;
		pMainFrameWnd->m_pInstallManager = NULL;
	}

	pMainFrameWnd->Close();
	PostQuitMessage(0);
	exit(0);
}
