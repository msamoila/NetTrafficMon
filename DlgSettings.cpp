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
// DlgSettings.cpp: implementation of the CDlgSettings class.
//
//	04-Oct-2005	MS	Modified OnClickedLog_path_sel - refresh the edit box
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DlgSettings.h"
#include "Ini.h"

extern CIni ini;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlgSettings::CDlgSettings()
{
	m_bResetCounters = FALSE;
}

CDlgSettings::~CDlgSettings()
{

}


LRESULT CDlgSettings::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int i;
	TCHAR szTxt[32];
	for(i=1; i<=31;SendDlgItemMessage(IDC_DAYS, CB_ADDSTRING, 0, (LPARAM)_itot(i++, szTxt, 10)));

	SendDlgItemMessage(IDC_SPIN1, UDM_SETRANGE, 0, MAKELONG(1000, 0));
	SendDlgItemMessage(IDC_SPIN2, UDM_SETRANGE, 0, MAKELONG(1000, 0));
	SendDlgItemMessage(IDC_SPIN3, UDM_SETRANGE, 0, MAKELONG(100, 0));

	SetDlgItemInt(IDC_LIMIT_IN, (DWORD)(ini.m_ldwInLimit/1000000000), FALSE);
	SetDlgItemInt(IDC_LIMIT_OUT, (DWORD)(ini.m_ldwOutLimit/1000000000), FALSE);
	SetDlgItemInt(IDC_ALERT_PERCENT, ini.m_nAlertPercent, FALSE);

	CheckDlgButton(IDC_IGNORE_LOOPBACK, ini.m_bIgnoreStandardLoopBack ? BST_CHECKED : BST_UNCHECKED);
	
	UpdateData(ini.m_eResetType, ini.m_eAlertType);
	return 1;  // Let the system set the focus
}

void CDlgSettings::UpdateData(EResetType eType, EAlertType eAlert)
{
	switch(eType)
	{
	case ResetDaily:
		CheckDlgButton(IDC_FROM_TODAY, BST_CHECKED);
		::ShowWindow(GetDlgItem(IDC_DATE_FROM), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_DAYS), SW_HIDE);
		::EnableWindow(GetDlgItem(IDC_TXT_1), TRUE);
		::EnableWindow(GetDlgItem(IDC_FROM_NOW), FALSE);
		SetDlgItemText(IDC_TXT_1, _T("Reset will be done every day at midnight"));
		break;
	case ResetMonthly:
		CheckDlgButton(IDC_FROM_MONTH, BST_CHECKED);
		::ShowWindow(GetDlgItem(IDC_DATE_FROM), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_DAYS), SW_NORMAL);
		::EnableWindow(GetDlgItem(IDC_TXT_1), TRUE);
		::EnableWindow(GetDlgItem(IDC_FROM_NOW), TRUE);
		SetDlgItemText(IDC_TXT_1, _T("Set the DAY of reset:"));
		SendDlgItemMessage(IDC_DAYS, CB_SETCURSEL, ini.m_stReset.wDay-1);
		SetDlgItemInt(IDC_DAYS, ini.m_stReset.wDay);
		break;
	case ResetDisabled:
		CheckDlgButton(IDC_FROM_NEVER, BST_CHECKED);
		::ShowWindow(GetDlgItem(IDC_DATE_FROM), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_DAYS), SW_HIDE);
		::EnableWindow(GetDlgItem(IDC_TXT_1), FALSE);
		::EnableWindow(GetDlgItem(IDC_FROM_NOW), FALSE);
		SetDlgItemText(IDC_TXT_1, NULL);
		break;
	case ResetOn:
		CheckDlgButton(IDC_FROM_CUSTOM, BST_CHECKED); 
		::ShowWindow(GetDlgItem(IDC_DATE_FROM), SW_NORMAL);
		::ShowWindow(GetDlgItem(IDC_DAYS), SW_HIDE);
		::EnableWindow(GetDlgItem(IDC_TXT_1), TRUE);
		::EnableWindow(GetDlgItem(IDC_FROM_NOW), TRUE);
		SetDlgItemText(IDC_TXT_1, _T("Set the DAY, MONTH and YEAR of reset:"));
		SendDlgItemMessage(IDC_DATE_FROM, DTM_SETSYSTEMTIME, 0, (LPARAM)&ini.m_stReset);
		break;
	case ResetNotValid:
		break;	// do nothing
	}

	switch(eAlert)
	{
	case AlertDistinct:
		CheckDlgButton(IDC_DISTINCT, BST_CHECKED);
		break;
	case AlertInOut:
		CheckDlgButton(IDC_TOTAL_LIMIT, BST_CHECKED);
		break;
	case AlertDisabled:
		CheckDlgButton(IDC_NO_ALERT, BST_CHECKED);
		break;
	case AlertNotValid:
		break;
	}
	const UINT ids[] = { IDC_TXT_2, IDC_TXT_3, IDC_TXT_5,
		IDC_LIMIT_IN, IDC_LIMIT_OUT, IDC_ALERT_PERCENT, 0};

	long i;
	if(eAlert == AlertDistinct || eAlert == AlertInOut)
	{
		for(i=0; ids[i]; ::EnableWindow(GetDlgItem(ids[i++]), TRUE));
	}
	else if(eAlert == AlertDisabled)
	{
		for(i=0; ids[i]; ::EnableWindow(GetDlgItem(ids[i++]), FALSE));
	}


	CheckDlgButton(IDC_EN_LOG, ini.m_bEnableLog ? BST_CHECKED : BST_UNCHECKED);
	::EnableWindow(GetDlgItem(IDC_TXT_4), ini.m_bEnableLog);
	::EnableWindow(GetDlgItem(IDC_LOG), ini.m_bEnableLog);
	::EnableWindow(GetDlgItem(IDC_LOG_PATH_SEL), ini.m_bEnableLog);
	SetDlgItemText(IDC_LOG, ini.m_szLogFile);

	CheckDlgButton(IDC_EN_TRAFFIC_LOG, ini.m_bEnableTrafficLog ? BST_CHECKED : BST_UNCHECKED);
	::EnableWindow(GetDlgItem(IDC_TXT_6), ini.m_bEnableTrafficLog);
	::EnableWindow(GetDlgItem(IDC_TRAFFIC_LOG), ini.m_bEnableTrafficLog);
	::EnableWindow(GetDlgItem(IDC_TRAFFIC_LOG_PATH_SEL), ini.m_bEnableTrafficLog);
	SetDlgItemText(IDC_TRAFFIC_LOG, ini.m_szTrafficLogFile);
}

LRESULT CDlgSettings::OnClickedEn_log(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ini.m_bEnableLog = !ini.m_bEnableLog;
	UpdateData(ResetNotValid, AlertNotValid);
	return 0;
}

LRESULT CDlgSettings::OnClickedLog_path_sel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	static OPENFILENAME ofn = {0};
	static LPCTSTR szFilter = _T("Log files (*.LOG)\0*.LOG\0All files (*.*)\0*.*\0"), defExt = _T("LOG");
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= m_hWnd;
	ofn.hInstance		= _Module.GetModuleInstance();
	ofn.lpstrFilter		= szFilter;
	ofn.lpstrFile		= ini.m_szLogFile;
	ofn.nMaxFile		= _MAX_PATH;
	ofn.Flags			= OFN_CREATEPROMPT | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrDefExt		= defExt;
	if(GetOpenFileName(&ofn))
		SetDlgItemText(IDC_LOG, ini.m_szLogFile);
	return 0;
}

LRESULT CDlgSettings::OnClickedEn_Traffic_log(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ini.m_bEnableTrafficLog = !ini.m_bEnableTrafficLog;
	UpdateData(ResetNotValid, AlertNotValid);
	return 0;
}

LRESULT CDlgSettings::OnClickedLog_Traffic_path_sel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	static OPENFILENAME ofn = {0};
	static LPCTSTR szFilter = _T("Comma separated values (*.CSV)\0*.CSV\0All files (*.*)\0*.*\0"), defExt = _T("CSV");
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= m_hWnd;
	ofn.hInstance		= _Module.GetModuleInstance();
	ofn.lpstrFilter		= szFilter;
	ofn.lpstrFile		= ini.m_szTrafficLogFile;
	ofn.nMaxFile		= _MAX_PATH;
	ofn.Flags			= OFN_CREATEPROMPT | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrDefExt		= defExt;
	if(GetOpenFileName(&ofn))
		SetDlgItemText(IDC_TRAFFIC_LOG, ini.m_szTrafficLogFile);
	return 0;
}

LRESULT CDlgSettings::OnOk(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(IsDlgButtonChecked(IDC_FROM_NEVER) == BST_CHECKED)
	{
		ini.m_eResetType = ResetDisabled;
		ZeroMemory(&ini.m_stReset, sizeof(SYSTEMTIME));
	}
	else if(IsDlgButtonChecked(IDC_FROM_CUSTOM) == BST_CHECKED)
	{
		ini.m_eResetType = ResetOn;
		SendDlgItemMessage(IDC_DATE_FROM, DTM_GETSYSTEMTIME, 0, (LPARAM)&ini.m_stReset);
		ini.m_stReset.wHour = ini.m_stReset.wMinute = ini.m_stReset.wSecond = ini.m_stReset.wMilliseconds = 0;
	}
	else if(IsDlgButtonChecked(IDC_FROM_MONTH) == BST_CHECKED)
	{
		ini.m_eResetType = ResetMonthly;
		ini.m_stReset.wDay = GetDlgItemInt(IDC_DAYS, NULL, FALSE);
		ini.m_stReset.wYear = ini.m_stReset.wHour = ini.m_stReset.wMinute = ini.m_stReset.wSecond = ini.m_stReset.wMilliseconds = 0;
	}
	else if(IsDlgButtonChecked(IDC_FROM_TODAY) == BST_CHECKED)
		ini.m_eResetType = ResetDaily;

	if(IsDlgButtonChecked(IDC_DISTINCT) == BST_CHECKED)
		ini.m_eAlertType = AlertDistinct;
	else if(IsDlgButtonChecked(IDC_TOTAL_LIMIT) == BST_CHECKED)
		ini.m_eAlertType = AlertInOut;
	else if(IsDlgButtonChecked(IDC_NO_ALERT) == BST_CHECKED)
		ini.m_eAlertType = AlertDisabled;

	ini.m_ldwInLimit = UInt32x32To64( GetDlgItemInt(IDC_LIMIT_IN, NULL, FALSE), 1000000000);
	ini.m_ldwOutLimit = UInt32x32To64( GetDlgItemInt(IDC_LIMIT_OUT, NULL, FALSE), 1000000000);

	ini.m_nAlertPercent = GetDlgItemInt(IDC_ALERT_PERCENT, NULL, FALSE);

	ini.m_bIgnoreStandardLoopBack = IsDlgButtonChecked(IDC_IGNORE_LOOPBACK) == BST_CHECKED;
	return EndDialog(wID);
}
