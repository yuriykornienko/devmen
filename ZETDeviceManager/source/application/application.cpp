#include "stdafx.h"
#include "application.h"
#include "../gui/main/main_dlg.h"
#include "resource\Phrases.h"

#ifdef _DEBUG
#   define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CZETDeviceManagerApp, CWinApp)
END_MESSAGE_MAP()

CZETDeviceManagerApp::CZETDeviceManagerApp()
{

}

CZETDeviceManagerApp theApp;

BOOL CZETDeviceManagerApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!CheckOtherInstances())
	{
		CString sCommandLine = GetCommandLine();

		COPYDATASTRUCT cds;
		cds.dwData = 0;
		cds.cbData = (sCommandLine.GetLength() + 1) * sizeof(wchar_t);
		cds.lpData = (PVOID)sCommandLine.GetString();
		if (cds.lpData != 0)
			memcpy_s(cds.lpData, cds.cbData, sCommandLine.GetString(), cds.cbData);

		HWND hwnd = OtherInstanceHwnd();
		::SendMessage(hwnd, WM_COPYDATA, (WPARAM)hwnd, (LPARAM)&cds);

		return FALSE;
	}

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));

	// Magic line from http://social.msdn.microsoft.com/Forums/en-US/vcgeneral/thread/c3feab0f-601b-4ca6-beb2-8d4d615438cc/
	// Removes "0xC015000F: The activation context being deactivated is not the most recently activated one."
	//afxAmbientActCtx = FALSE;


	AfxEnableControlContainer();

	SetRegistryKey(_T("ZET\\ZETLab"));
	SetAppID(_T("ZET.ZETLab"));

	try
	{
		CZETDeviceManagerDlg* dlg = new CZETDeviceManagerDlg;
		if (dlg != nullptr)
		{
			m_pMainWnd = dlg;
			dlg->DoModal();

			// Поскольку диалоговое окно закрыто, возвратите значение FALSE, чтобы можно было выйти из
			//  приложения вместо запуска генератора сообщений приложения.
			delete dlg;
			dlg = nullptr;
		}
	}
	catch (std::bad_alloc) { return FALSE; }
	return FALSE;
}
