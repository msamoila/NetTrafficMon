// This file is part of the msamoila distribution (https://github.com/msamoila).
// Copyright (c) 2011 Marius Samoila.
// 
// This program is free software: you can redistribute it and/or modify  
// it under the terms of the GNU General Public License as published by  
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but 
// WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License 
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// NetTrafficMon.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MainWnd.h"
#include "resource.h"

CComModule _Module;



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;

	HWND hWndPrevious = FindWindow(_T("NetTrafficMonAtl_class"), NULL);
	if(hWndPrevious)
	{
		ShowWindow(hWndPrevious, SW_NORMAL);
		SetForegroundWindow(hWndPrevious);
		return -1;
	}

	INITCOMMONCONTROLSEX cc;
	cc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	cc.dwICC = ICC_BAR_CLASSES | ICC_DATE_CLASSES  | ICC_TAB_CLASSES | ICC_UPDOWN_CLASS  ;
	InitCommonControlsEx(&cc);

	_Module.Init(NULL, hInstance);

	CMainWnd wndMain;
	if(strstr(lpCmdLine, "/s"))
		nCmdShow = SW_HIDE;

	WNDCLASSEX* pwcex;
	RECT rc = { 0, 0, 317, 157 };
	long baseunits = GetDialogBaseUnits();
	rc.right = MulDiv( rc.right, LOWORD(baseunits), 4 );
	rc.bottom = MulDiv( rc.bottom, HIWORD(baseunits), 8 ) + 30;//includes the toolbar
	pwcex = &CMainWnd::GetWndClassInfo().m_wc;

	pwcex->hIcon		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NET_TRAF_MON));
	pwcex->hIconSm		= pwcex->hIcon;

	if(!wndMain.Create(NULL, rc, APP_NAME,
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN, WS_EX_APPWINDOW|WS_EX_CLIENTEDGE, 0U, NULL))
		return FALSE;

	wndMain.ShowWindow(nCmdShow);
	wndMain.UpdateWindow();

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if(wndMain.PreTranslateMessage(&msg))
			continue;
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	_Module.Term();
	return 0;
}


void Win32Message(DWORD dwCode, LPTSTR szTxt, long cbSize)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL );// Process any inserts in lpMsgBuf.

	ATLTRACE((LPTSTR)lpMsgBuf);
	_sntprintf(szTxt, cbSize, _T("%#x '%s'"), dwCode, (LPTSTR)lpMsgBuf); 
	LocalFree( lpMsgBuf );
}

long SetTextWithUnit(ULONGLONG ldwValue, LPCTSTR szUnit, LPTSTR szTxt, long cbSize)
{
	if(ldwValue > 1000000000)
	{
		return _sntprintf(szTxt, cbSize, _T("%.2f G%s"), (DWORD)(ldwValue / 1000000) / 1000. , szUnit);
	}
	else if(ldwValue > 20000000)
	{
		return _sntprintf(szTxt, cbSize, _T("%d M%s"), (DWORD)(ldwValue / 1000000), szUnit);
	}
	else if(ldwValue > 2000000)
	{
		return _sntprintf(szTxt, cbSize, _T("%.2f M%s"), (DWORD)(ldwValue / 1000) / 1000., szUnit);
	}
	else if(ldwValue > 1000000)
	{
		return _sntprintf(szTxt, cbSize, _T("%.3f M%s"), (DWORD)(ldwValue / 1000) / 1000., szUnit);
	}
	else if(ldwValue > 1000)
	{
		return _sntprintf(szTxt, cbSize, _T("%d K%s"), (DWORD)(ldwValue / 1000), szUnit);
	}
	else
		return _sntprintf(szTxt, cbSize, _T("%d %s"), (DWORD)ldwValue, szUnit);
}