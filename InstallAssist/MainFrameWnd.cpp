#include "stdafx.h"
#include "InstallManager.h"
#include "MainFrameWnd.h"


CMainFrameWnd::CMainFrameWnd()
	: m_pWndShadow(nullptr)
	, m_pImage(nullptr)
	, m_pCurrentInstall(nullptr)
	, m_pDescription(nullptr)
	, m_pProgress(nullptr)
	, m_pInstallManager(nullptr)
	, m_pProgressUI(nullptr)
{
}

CMainFrameWnd::~CMainFrameWnd()
{
	if (nullptr != m_pWndShadow)
	{
		delete m_pWndShadow;
		m_pWndShadow = nullptr;
	}

	if (nullptr != m_pInstallManager)
	{
		delete m_pInstallManager;
		m_pInstallManager = nullptr;
	}
}

CDuiString CMainFrameWnd::GetSkinFolder()
{
	return _T("Skin");
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
			m_pInstallManager->End();
			PostQuitMessage(0);
		}
	}
}

void CMainFrameWnd::InitWindow()
{
	m_pImage = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("image")));
	m_pCurrentInstall = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("current")));
	m_pDescription = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("discription")));
	m_pProgress = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("progress")));
	m_pProgressUI = static_cast<CProgressUI*>(m_PaintManager.FindControl(_T("down_progress_front")));

	m_pInstallManager = new CInstallManager(this);
	m_pInstallManager->Begin();
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
	if (nullptr != m_pImage)
	{
		m_pImage->SetForeImage(pStrImage);
	}
}

void CMainFrameWnd::SetCurrentInstallText(LPCTSTR pStrText)
{
	if (nullptr != m_pCurrentInstall)
	{
		m_pCurrentInstall->SetText(pStrText);
	}
}

void CMainFrameWnd::SetDescriptionText(LPCTSTR pStrText)
{
	if (nullptr != m_pDescription)
	{
		m_pDescription->SetText(pStrText);
	}
}

void CMainFrameWnd::SetProgressText(LPCTSTR pStrText)
{
	if (nullptr != m_pProgress)
	{
		m_pProgress->SetText(pStrText);
	}
}

void CMainFrameWnd::SetProgressValue(int nValue)
{
	m_pProgressUI->SetValue(nValue);
}
