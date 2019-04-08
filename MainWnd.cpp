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
//
// DlgOptions.cpp: implementation of the CMainWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainWnd.h"
#include "IPHelper.h"
#include "DlgSettings.h"
#include "Ini.h"
#include "Log.h"

#define TASK_BAR_ID 1
#define NET_TR_MONITOR_TITLE _T("Net Traffic Monitor")

static LPARAM compare_MAC(CPageWnd* pPage, LPARAM lParam);
static LPARAM update_ctrl(CPageWnd* pPage, LPARAM lParam);
static LPARAM update_if_null(CPageWnd* pPage, LPARAM lParam);
static LPARAM update_start_date(CPageWnd* pPage, LPARAM lParam);
static LPARAM update_reset_counters(CPageWnd* pPage, LPARAM lParam);

extern HBRUSH g_hAlertBrush;

CIni ini;
CLog log(ini.m_szLogFile, FALSE), logTraffic(ini.m_szTrafficLogFile, TRUE);

short maxDaysInMonth[] = 
{
	0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

BOOL MustReset(const SYSTEMTIME* stNow, DATE dateStartedAt)
{

	SYSTEMTIME stLastReset;
	memcpy(&stLastReset, stNow, sizeof(SYSTEMTIME));
	switch(ini.m_eResetType)
	{
	case ResetDisabled:
		return FALSE;	//reset disabled
	case ResetMonthly:
		stLastReset.wDay = ini.m_stReset.wDay;
		//this is the day in this month when we need reset
		if(stLastReset.wDay > maxDaysInMonth[ stLastReset.wMonth ])
			stLastReset.wDay = maxDaysInMonth[ stLastReset.wMonth ];
		//break here is missing on purpose to reset time as well
	case ResetDaily:
		//last reset it is the midnight what this day begun
		//if we started to count before midnight then we need to reset
		stLastReset.wHour = stLastReset.wMinute = stLastReset.wSecond = stLastReset.wMilliseconds = 0;
		break;
	case ResetOn:
		memcpy(&stLastReset, &ini.m_stReset, sizeof(SYSTEMTIME));
		break;
	}

	DATE dtLastReset, dtNow;
	SystemTimeToVariantTime(const_cast<SYSTEMTIME*>(stNow), &dtNow);
	SystemTimeToVariantTime(&stLastReset, &dtLastReset);
	//we reset if the reset date is in the past AND the date when we started to count is even older than this reset
	return dtNow > dtLastReset && dateStartedAt < dtLastReset;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainWnd::CMainWnd()
{
	m_bNoTimer = FALSE;
	m_bShowToolbar = TRUE;
	m_hIconMenu = NULL;
	m_bFinished = FALSE;
	m_hIconAlert = NULL;
}

CMainWnd::~CMainWnd()
{
	ATLASSERT(m_bFinished);
}

LRESULT CMainWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//  Check for leap year and set the number of days in the month
	SYSTEMTIME stNow;
	GetLocalTime(&stNow);
	BOOL bLeapYear = ((stNow.wYear & 3) == 0) &&
      ((stNow.wYear % 100) != 0 || (stNow.wYear % 400) == 0);
	if(bLeapYear)
		maxDaysInMonth[2] = 29;

	RECT rc = { 0 };
	if(!m_wndTabCtrl.Create(WC_TABCONTROL, m_hWnd, rc, NULL,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|TCS_FOCUSONBUTTONDOWN,
		0, IDC_TAB_CTRL))
		return -1;
	m_wndTabCtrl.SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT), FALSE);

	//create the tool bar
	if( !m_wndToolbar.Create(m_hWnd, IDC_TOOL_BAR) ||
		!m_wndToolbar.LoadToolbar(IDR_TB_TR_MON))
	{
		ATLTRACE(_T("Fail to create the toolbar\r\n"));
		return -1;
	}

	ini.Load();

	m_hIconMenu = LoadMenu(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_ICON_MENU));
	m_hIconAlert = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_NET_TRAF_MON_RED));

	NOTIFYICONDATA nd = { 0 };
	nd.cbSize = sizeof(NOTIFYICONDATA);
	nd.hWnd = m_hWnd;
	nd.uID = TASK_BAR_ID;
	nd.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nd.hIcon = (HICON)GetClassLong(m_hWnd, GCL_HICON);
	nd.uCallbackMessage = WM_STATUS_ICON;
	_tcscpy(nd.szTip, NET_TR_MONITOR_TITLE);
	Shell_NotifyIcon(NIM_ADD, &nd);

	g_hAlertBrush = CreateSolidBrush( RGB( 0xFF, 0, 0) );

	OnTimerUpdateIPData(0, 0, 0, bHandled);

	SetTimer(ID_TIMER, 1000);

	return 0;
}

LRESULT CMainWnd::OnStatusIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( lParam == WM_CONTEXTMENU || lParam == WM_RBUTTONDOWN || 
		lParam == WM_LBUTTONDOWN || lParam == NIN_KEYSELECT || lParam == NIN_SELECT)
	{
		POINT pt;
		GetCursorPos(&pt);
		TrackPopupMenu(GetSubMenu(m_hIconMenu, 0), TPM_RIGHTALIGN|TPM_BOTTOMALIGN, pt.x, pt.y,
			0, m_hWnd, NULL);
	}
	return 0;
}

LRESULT CMainWnd::OnEndSession(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_bFinished)
		return 0;
	m_bFinished = TRUE;

	KillTimer(ID_TIMER);

	//close all tab windows which should trigger the information saving
	//in .INI and .LOG for each page
	m_wndTabCtrl.SendMessage(WM_CLOSE);

	ini.Save();

	if(g_hAlertBrush)
	{
		DeleteObject(g_hAlertBrush);
		g_hAlertBrush = NULL;
	}

	NOTIFYICONDATA nd = { 0 };
	nd.cbSize = sizeof(NOTIFYICONDATA);
	nd.hWnd = m_hWnd;
	nd.uID = TASK_BAR_ID;
	Shell_NotifyIcon(NIM_DELETE, &nd);
	return 0;
}

LRESULT CMainWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(IsWindowVisible())
	{	//just hide, closing should come from icon menu only
		ShowWindow(SW_HIDE);
		return 0;
	}

	OnEndSession(0, 0, 0, bHandled);
	bHandled = FALSE;
	return 0;
}


LRESULT CMainWnd::OnSettings(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CDlgSettings dlg;
	if(dlg.DoModal() == IDOK && dlg.m_bResetCounters)
	{
		SYSTEMTIME stNow;
		DATE now;
		GetLocalTime(&stNow);
		SystemTimeToVariantTime(&stNow, &now);
		EnumTrafficPages(update_reset_counters, (LPARAM)&now);
		log.Write(_T("The counters were reset at user request\r\n"));
	}

	return 0;
}


LRESULT CMainWnd::OnTimerUpdateIPData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD len = sizeof(MIB_IFTABLE);
	DWORD i, ret;
	TCHAR szText[64];
	if(m_bNoTimer)
		return 0;
	m_bNoTimer = TRUE;
	MIB_IFTABLE* pMibIfTable = (MIB_IFTABLE*)malloc(len);

	//get information about interface table
	ret = GetIfTable(pMibIfTable, &len, FALSE);
	if(ret == ERROR_INSUFFICIENT_BUFFER)
	{
		pMibIfTable = (MIB_IFTABLE*)realloc(pMibIfTable, len);
		ret = GetIfTable(pMibIfTable, &len, FALSE);
	}
	if(ret != ERROR_SUCCESS)
	{
		m_bNoTimer = FALSE;
		free(pMibIfTable);
		Win32Message(ret, szText, 20);
		return 0;
	}
	CPageWnd* pWnd = NULL;
	EnumTrafficPages(update_if_null, 0);//mark all existing as inactive

	for(i=0; i<pMibIfTable->dwNumEntries; i++)
	{
		//we try to find the IP address. If there is no IP then we skip
		if(!GetIPFromIFIndex(pMibIfTable->table[i].dwIndex, szText, 20))
			continue;
		if(ini.m_bIgnoreStandardLoopBack && _tcscmp(szText, "127.0.0.1") == 0)
			continue;

		pWnd = (CPageWnd*)EnumTrafficPages(compare_MAC, (LPARAM)&pMibIfTable->table[i]);
		if(pWnd == NULL)
		{	//new interface, create a new page for it
			TCITEM tcItem;
			TCHAR szMac[64];
			tcItem.mask = TCIF_TEXT|TCIF_PARAM;
			//the caption is the IP address
			tcItem.pszText = szText;
			
			pWnd = new CPageWnd(szText);
			tcItem.lParam = (LPARAM)pWnd;
			if(!pWnd->Create(m_wndTabCtrl.m_hWnd, (LPARAM)this))
			{
				Win32Message(GetLastError(), szText, 64);
				delete pWnd;
				pWnd = NULL;
				continue;
			}

			MacAddr2Str(pMibIfTable->table[i].bPhysAddr, pMibIfTable->table[i].dwPhysAddrLen, szMac, 64);
			log.Write(_T("Monitor %s port '%s'\tStart\r\n"), szMac, szText);
			// Remove caption from the dialog box because no caption is allowed
			// for pages in the tab control.
			pWnd->ModifyStyle(WS_CAPTION, 0);
			long idx = m_wndTabCtrl.SendMessage(TCM_INSERTITEM, i, (LPARAM)&tcItem);
			// IMPORTANT: Why sending a WM_NCACTIVATE message to the dialog box?
			// When writing this sample, we found a problem which is related to
			// the edit control refuses to give up the input focus when mouse is 
			// being clicked on some other edit control.  This problem only 
			// happens when title bar is found from a dialog resource.  And we
			// need the title bar because it is the text in the tab control.  
			// Sending a WM_NCACTIVATE message seems to fix the problem.
			pWnd->SendMessage(WM_NCACTIVATE, TRUE);
			if(idx == 0)
			{
				m_wndTabCtrl.SendMessage(TCM_SETCURSEL, idx, 0);
				BOOL bHandled;
				NMHDR nmHdr;
				nmHdr.idFrom = IDC_TAB_CTRL;
				nmHdr.hwndFrom = m_wndTabCtrl;
				nmHdr.code = TCN_SELCHANGING;
				OnSelChanging(IDC_TAB_CTRL, &nmHdr, bHandled);
				nmHdr.code = TCN_SELCHANGE;
				OnSelChange(IDC_TAB_CTRL, &nmHdr, bHandled);
			}

			pWnd->LoadStart(szMac);
		}
		
		LPCTSTR szNewIP = pWnd->UpdateIf(&pMibIfTable->table[i]);
		if(szNewIP)
		{
			long i, lCount;
			TCITEM tcItem;
			tcItem.mask = TCIF_PARAM;
			lCount = m_wndTabCtrl.SendMessage(TCM_GETITEMCOUNT);
			LPARAM ret = 0;
			for(i=0; i<lCount; i++)
			{
				if(!m_wndTabCtrl.SendMessage(TCM_GETITEM, i, (LPARAM)&tcItem))
					continue;
				CPageWnd* pWnd1 = (CPageWnd*)tcItem.lParam;
				ATLASSERT(pWnd1);
				if(pWnd1 == pWnd)
				{
					tcItem.mask = TCIF_TEXT;
					tcItem.pszText = const_cast<LPTSTR>(szNewIP);
					m_wndTabCtrl.SendMessage(TCM_SETITEM, i, (LPARAM)&tcItem);
					break;
				}
			}
		}
	}

	EnumTrafficPages(update_ctrl, 0);

	NOTIFYICONDATA nd = { 0 };
	nd.cbSize = sizeof(NOTIFYICONDATA);
	nd.hWnd = m_hWnd;
	nd.uID = TASK_BAR_ID;
	nd.uFlags = NIF_ICON | NIF_TIP;
	pWnd = GetActivePage();
	if(pWnd)
	{
		i = _sntprintf(nd.szTip, 64, _T("NetTrMon: "));
		pWnd->GetTipText(nd.szTip+i, 64-i);
		nd.hIcon = pWnd->IsAlert() ? m_hIconAlert : (HICON)GetClassLong(m_hWnd, GCL_HICON);
	}
	else
	{
		 _sntprintf(nd.szTip, 64, NET_TR_MONITOR_TITLE);
		nd.hIcon = (HICON)GetClassLong(m_hWnd, GCL_HICON);
	}
	
	Shell_NotifyIcon(NIM_MODIFY , &nd);
	if(IsIconic())
		SetWindowText(nd.szTip);
	else
		SetWindowText(APP_NAME);

	free(pMibIfTable);
	m_bNoTimer = FALSE;
	return 0;
}

LPARAM CMainWnd::EnumTrafficPages(LPARAM (*enum_proc)(CPageWnd* pPage, LPARAM lParam), LPARAM lParam)
{
	long i, lCount;
	TCITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	lCount = m_wndTabCtrl.SendMessage(TCM_GETITEMCOUNT);
	LPARAM ret = 0;
	for(i=0; i<lCount; i++)
	{
		if(!m_wndTabCtrl.SendMessage(TCM_GETITEM, i, (LPARAM)&tcItem))
			continue;
		CPageWnd* pWnd = (CPageWnd*)tcItem.lParam;
		ATLASSERT(pWnd);
		ret = enum_proc(pWnd, lParam);
		if(ret)
			break;
	}
	return ret;
}

static LPARAM compare_MAC(CPageWnd* pPage, LPARAM lParam)
{
	ATLASSERT(pPage);
	const MIB_IFROW* pIf = (MIB_IFROW*)lParam;
	if(pPage->IsEqualMAC(pIf->bPhysAddr, pIf->dwPhysAddrLen))
		return (LPARAM)pPage;
	return 0;
}

static LPARAM update_ctrl(CPageWnd* pPage, LPARAM lParam)
{
	ATLASSERT(pPage);
	pPage->UpdateCtrl();
	return 0;//continue enum for all pages
}

static LPARAM update_if_null(CPageWnd* pPage, LPARAM lParam)
{
	ATLASSERT(pPage);
	pPage->UpdateIf(NULL);
	return 0;//continue enum for all pages
}

static LPARAM update_start_date(CPageWnd* pPage, LPARAM lParam)
{
	ATLASSERT(pPage);
	pPage->UpdateIf(NULL);
	return 0;//continue enum for all pages
}

static LPARAM update_reset_counters(CPageWnd* pPage, LPARAM lParam)
{
	ATLASSERT(pPage);
	pPage->ResetCounters(*(DATE*)lParam);
	return 0;//continue enum for all pages
}

