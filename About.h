//=======================================================
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
//	File Name: About.h
//	Description:
//					The About dialog
//
//
//	HISTORY
//
//	Created on 3/15/00
//=======================================================

#ifndef _ABOUT_H__
#define _ABOUT_H__


#include "resource.h"
#include "NetTrafficMonVer.h"

class CHyperlink : public CWindowImpl<CHyperlink>
{
	HFONT			m_hFont;
	HCURSOR			m_hCursor;
	BOOL			m_bFirst;
	BOOL			m_bVisited;
public:

DECLARE_WND_SUPERCLASS(NULL, _T("Static"))

BEGIN_MSG_MAP(CHyperlink)
	MESSAGE_HANDLER(WM_LBUTTONDOWN,		OnLButtonDown)
	MESSAGE_HANDLER(WM_SETCURSOR,		OnSetCursor)
	MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtlColorStatic)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()

	CHyperlink()
	{
		m_hFont = NULL;
		m_hCursor = ::LoadCursor(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDC_CURSOR_HAND));
		m_bFirst = TRUE;
		m_bVisited = FALSE;
	}
	~CHyperlink()
	{
		if(m_hFont)
			DeleteObject(m_hFont);
	}
	void CreateFont()
	{
		if(m_bFirst == FALSE)
			return;
		m_bFirst = FALSE;
		ATLASSERT(m_hFont == NULL);
		HFONT hFont = GetFont();
		LOGFONT lf;
		::GetObject(hFont, sizeof(LOGFONT), &lf);
		lf.lfUnderline = TRUE;
		m_hFont = ::CreateFontIndirect(&lf);
	}
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		TCHAR szTxt[128];
		LPCTSTR szMailTo = _T("mailto:");
		long i = _tcslen(szMailTo);
		GetWindowText(szTxt+i, 121);
		if(_tcschr(szTxt+i, '@'))
		{
			_tcsncpy(szTxt, szMailTo, i);
			i = 0;
		}
		long code;
		code = (long)::ShellExecute(0, _T("open"), szTxt+i, 0, 0, SW_SHOWNORMAL);
		if(code <= 32)
		{
			_sntprintf(szTxt, 128, _T("Fail to execute associated application.\nCode: %#x"), code);
			MessageBox(szTxt, APP_NAME, MB_OK|MB_ICONSTOP);
		}
		else
			m_bVisited = TRUE;
		return 0;
	}
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	LRESULT OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CreateFont();
		HDC hdc = (HDC)wParam;
		::SelectObject(hdc, m_hFont);
		::SetTextColor(hdc, m_bVisited ? RGB(128, 0, 0) : RGB(0, 0, 255));
		::SetBkMode(hdc, TRANSPARENT);
		// return hollow brush to preserve parent background color
		return (LRESULT)::GetStockObject(HOLLOW_BRUSH);
	}
};


class CAboutDlg : public CDialogImpl<CAboutDlg>
{
	CHyperlink m_staticEMail, m_staticWeb;
public:

	enum { IDD = IDD_ABOUTBOX };

BEGIN_MSG_MAP(CAboutDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnClose)
	COMMAND_ID_HANDLER(IDCANCEL, OnClose)
	REFLECT_NOTIFICATIONS()
END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_staticEMail.SubclassWindow(GetDlgItem(IDC_AUTHOR));
		m_staticEMail.SetWindowText(NET_TRAFFIC_MON_COMPANY_EMAIL);
		m_staticWeb.SubclassWindow(GetDlgItem(IDC_WEB));
		m_staticWeb.SetWindowText(NET_TRAFFIC_MON_COMPANY_WEB);
		
		TCHAR szTxt[256];
		_sntprintf(szTxt, 256, _T("Network Traffic monitor %d.%d.%d.%d"), NET_TRAFFIC_MON_VER);
		SetDlgItemText(IDC_APP_NAME_VER, szTxt);

		return 1;  // Let the system set the focus
	}
	LRESULT OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		return EndDialog(wID);
	}

};

#endif // _ABOUT_H__
