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
// MainWnd.h: interface for the CMainWnd class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MAIN_WND_NET_TR_MON__H__
#define __MAIN_WND_NET_TR_MON__H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PageWnd.h"
#include "about.h"
#include "toolbar.h"

#define IDC_TAB_CTRL 1
#define IDC_TOOL_BAR 2
#define ID_TIMER 1

#define WM_STATUS_ICON (WM_USER+1)


class CMainWnd : public CWindowImpl<CMainWnd>  
{

public:
	CMainWnd();
	~CMainWnd();

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		CPageWnd* pWnd = GetActivePage();
		if(pWnd == NULL)
			return FALSE;

		if(pWnd->IsDialogMessage(pMsg))
			return TRUE;

		return FALSE;
	}

	CWindow	m_wndTabCtrl;
	CToolbar m_wndToolbar;

	BOOL m_bShowToolbar;
	HICON	m_hIconAlert;

	DECLARE_WND_CLASS_EX(_T("NetTrafficMonAtl_class"), CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, (HBRUSH)NULL)

protected:

BEGIN_MSG_MAP(CMainWnd)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_TIMER, OnTimerUpdateIPData)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_ENDSESSION, OnEndSession)
	MESSAGE_HANDLER(WM_CLOSE, OnClose)
	MESSAGE_HANDLER(WM_STATUS_ICON, OnStatusIcon)
	COMMAND_ID_HANDLER(ID_ABOUT, OnAbout)
	COMMAND_ID_HANDLER(ID_SETTINGS, OnSettings)
	COMMAND_ID_HANDLER(ID_ICONMENU_SHOW, OnIconMenuShow)
	COMMAND_ID_HANDLER(ID_ICONMENU_CLOSE, OnIconMenuClose)
	NOTIFY_HANDLER(IDC_TAB_CTRL, TCN_SELCHANGING, OnSelChanging)
	NOTIFY_HANDLER(IDC_TAB_CTRL, TCN_SELCHANGE, OnSelChange)
END_MSG_MAP()
	
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnStatusIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		RECT rcToolbar;
		m_wndToolbar.GetWindowRect(&rcToolbar);
		long cyToolbar;
		if(m_bShowToolbar)
		{
			cyToolbar = rcToolbar.bottom-rcToolbar.top;
			m_wndToolbar.SetWindowPos(NULL, 0, 0, LOWORD(lParam), cyToolbar,
				  SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
		else
			cyToolbar = 0;

		m_wndTabCtrl.SetWindowPos(NULL, 0, cyToolbar, LOWORD(lParam), HIWORD(lParam)-cyToolbar, SWP_NOZORDER|SWP_NOACTIVATE);
		BOOL b;
		//this is just to call SetWindowPos for the active page
		//the idCtrl is set to 0 to make the difference between WM_SIZE and TCN_SELCHANGE
		OnSelChange(0, NULL, b);	
		return 0;
	}
	LRESULT OnEndSession(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		CAboutDlg dlg;
		dlg.DoModal(m_hWnd);
		return 0;
	}
	LRESULT OnSettings(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnIconMenuShow(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		ShowWindow(IsWindowVisible() ? SW_HIDE : SW_NORMAL);
		return 0;
	}
	LRESULT OnIconMenuClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		ShowWindow(SW_HIDE);
		if(MessageBox(_T("Do you really want to close Net Traffic Monitor ?"), APP_NAME, MB_YESNO|MB_ICONQUESTION|MB_TOPMOST) == IDYES)
			PostMessage(WM_CLOSE);
		return 0;
	}
	LRESULT OnSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		// get the index of the page that is newly
		// selected.  This page will be visible soon.
		long lActivePage = m_wndTabCtrl.SendMessage(TCM_GETCURSEL);
		if(lActivePage < 0)
			return 0;

		TCITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		
		if(!m_wndTabCtrl.SendMessage(TCM_GETITEM, lActivePage, (LPARAM)&tcItem))
			return 0;

		RECT rc;
		m_wndTabCtrl.GetClientRect(&rc);
		m_wndTabCtrl.SendMessage(TCM_ADJUSTRECT, FALSE, (LPARAM)&rc);


		CPageWnd* pWnd = (CPageWnd*)tcItem.lParam;
		ATLASSERT(pWnd);
		pWnd->SetWindowPos(NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER|SWP_SHOWWINDOW);
		if(idCtrl)
		{
			pWnd->SetFocus();
			pWnd->OnActivate();
		}
		return 0;
	}
	LRESULT OnSelChanging(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		// Call GetCurSel() here will return the index of the page that
		// will be hidden soon.  This is NOT the newly selected page when 
		// OnTabSelChange() is called.
		int lActivePage = m_wndTabCtrl.SendMessage(TCM_GETCURSEL);
		if(lActivePage < 0)
			return 0;

		TCITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		
		if(!m_wndTabCtrl.SendMessage(TCM_GETITEM, lActivePage, (LPARAM)&tcItem))
			return 0;

		CPageWnd* pWnd = (CPageWnd*)tcItem.lParam;
		ATLASSERT(pWnd);
		pWnd->ShowWindow(SW_HIDE);
		pWnd->OnDeactivate();
		return FALSE;	// return FALSE to allow seletion to change
	}
	LRESULT OnTimerUpdateIPData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void OnFinalMessage(HWND hWnd)
	{
		PostQuitMessage(0);
	}
private:
	LPARAM EnumTrafficPages(LPARAM (*enum_proc)(CPageWnd* pPage, LPARAM lParam), LPARAM lParam);

	CPageWnd* GetActivePage()
	{
		long lActivePage = m_wndTabCtrl.SendMessage(TCM_GETCURSEL);
		if(lActivePage < 0)
			return NULL;

		TCITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		
		if(!m_wndTabCtrl.SendMessage(TCM_GETITEM, lActivePage, (LPARAM)&tcItem))
			return NULL;

		CPageWnd* pWnd = (CPageWnd*)tcItem.lParam;
		ATLASSERT(pWnd);
		return pWnd;
	}

	BOOL	m_bNoTimer;
	HMENU	m_hIconMenu;
	BOOL	m_bFinished;
};

#endif // __MAIN_WND_NET_TR_MON__H__
