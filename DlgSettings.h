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
// DlgSettings.h: interface for the CDlgSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGSETTINGS_H__75EEC99A_5CCD_4677_B3E0_074CB6249895__INCLUDED_)
#define AFX_DLGSETTINGS_H__75EEC99A_5CCD_4677_B3E0_074CB6249895__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

class CDlgSettings : public CDialogImpl<CDlgSettings>  
{
public:
	enum { IDD = IDD_SETTINGS };
	
	CDlgSettings();
	~CDlgSettings();

	BOOL m_bResetCounters;
protected:

BEGIN_MSG_MAP(CAboutDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOk)
	COMMAND_ID_HANDLER(IDCANCEL, OnClose)
	COMMAND_HANDLER(IDC_FROM_CUSTOM, BN_CLICKED, OnClickedFrom_custom)
	COMMAND_HANDLER(IDC_FROM_MONTH, BN_CLICKED, OnClickedFrom_month)
	COMMAND_HANDLER(IDC_FROM_NEVER, BN_CLICKED, OnClickedFrom_never)
	COMMAND_HANDLER(IDC_FROM_TODAY, BN_CLICKED, OnClickedFrom_today)
	COMMAND_HANDLER(IDC_DISTINCT, BN_CLICKED, OnClickedDistinct)
	COMMAND_HANDLER(IDC_NO_ALERT, BN_CLICKED, OnClickedNo_alert)
	COMMAND_HANDLER(IDC_TOTAL_LIMIT, BN_CLICKED, OnClickedTotal_limit)
	COMMAND_HANDLER(IDC_FROM_NOW, BN_CLICKED, OnClickedFrom_now)
	COMMAND_HANDLER(IDC_RESET_COUNTERS, BN_CLICKED, OnClickedReset_counters)
	COMMAND_HANDLER(IDC_EN_LOG, BN_CLICKED, OnClickedEn_log)
	COMMAND_HANDLER(IDC_LOG_PATH_SEL, BN_CLICKED, OnClickedLog_path_sel)
	COMMAND_HANDLER(IDC_EN_TRAFFIC_LOG, BN_CLICKED, OnClickedEn_Traffic_log)
	COMMAND_HANDLER(IDC_TRAFFIC_LOG_PATH_SEL, BN_CLICKED, OnClickedLog_Traffic_path_sel)
END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		return EndDialog(wID);
	}
	LRESULT OnOk(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedFrom_custom(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		UpdateData(ResetOn, AlertNotValid);
		return 0;
	}
	LRESULT OnClickedFrom_month(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		UpdateData(ResetMonthly, AlertNotValid);
		return 0;
	}
	LRESULT OnClickedFrom_never(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		UpdateData(ResetDisabled, AlertNotValid);
		return 0;
	}
	LRESULT OnClickedFrom_today(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		UpdateData(ResetDaily, AlertNotValid);
		return 0;
	}

	LRESULT OnClickedDistinct(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		UpdateData(ResetNotValid, AlertDistinct);
		return 0;
	}
	LRESULT OnClickedNo_alert(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		UpdateData(ResetNotValid, AlertDisabled);
		return 0;
	}
	LRESULT OnClickedTotal_limit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		UpdateData(ResetNotValid, AlertInOut);
		return 0;
	}

	void UpdateData(EResetType eType, EAlertType eAlert);
	LRESULT OnClickedFrom_now(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		SYSTEMTIME stNow;
		GetLocalTime(&stNow);
		stNow.wHour = stNow.wMinute = stNow.wSecond = stNow.wMilliseconds = 0;
		SendDlgItemMessage(IDC_DATE_FROM, DTM_SETSYSTEMTIME, 0, (LPARAM)&stNow);

		SendDlgItemMessage(IDC_DAYS, CB_SETCURSEL, stNow.wDay-1);
		SetDlgItemInt(IDC_DAYS, stNow.wDay);
		return 0;
	}
	LRESULT OnClickedReset_counters(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		m_bResetCounters = !m_bResetCounters;
		::ShowWindow(GetDlgItem(IDC_RESET_TEXT), m_bResetCounters ? SW_NORMAL : SW_HIDE);
		SetDlgItemText(IDC_RESET_COUNTERS, m_bResetCounters ? _T("Do not &Reset Counters") : _T("&Reset counters"));
		return 0;
	}
	LRESULT OnClickedEn_log(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedLog_path_sel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedEn_Traffic_log(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedLog_Traffic_path_sel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};

#endif // !defined(AFX_DLGSETTINGS_H__75EEC99A_5CCD_4677_B3E0_074CB6249895__INCLUDED_)
