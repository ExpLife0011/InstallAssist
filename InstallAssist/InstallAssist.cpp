// InstallAssist.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "InstallAssist.h"
#include "MainFrameWnd.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	// 设置关联实例
	CPaintManagerUI::SetInstance(hInstance);
	// 设置皮肤路径
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));

	// 初始化 COM 库
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		return -1;
	}

	CMainFrameWnd* pFrame = new CMainFrameWnd();

	pFrame->Create(NULL, NULL, UI_WNDSTYLE_DIALOG, 0);
	pFrame->CenterWindow();
	pFrame->ShowWindow();

	CPaintManagerUI::MessageLoop();

	if (nullptr != pFrame)
	{
		delete pFrame;
	}

	::CoUninitialize();

    return 0;
}
