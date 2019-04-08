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
#include "stdafx.h"
#include "Ini.h"


#define CFG_SECTION		_T("Settings")
#define CFG_RESET_DATE	_T("ResetDate")
#define CFG_RESET_TYPE	_T("ResetType")
#define CFG_ALERT_TYPE	_T("AlertType")
#define CFG_IN_LIMIT	_T("LimitIN")
#define CFG_OUT_LIMIT	_T("LimitOUT")
#define CFG_ALERT_PERCENT _T("AlertPercent")
#define CFG_LOG_ENABLE	_T("EnableLog")
#define CFG_LOG_PATHNAME _T("LogPathName")
#define CFG_TRAFFIC_LOG_ENABLE	_T("EnableTrafficLog")
#define CFG_TRAFFIC_LOG_PATHNAME _T("TrafficLogPathName")
#define CFG_IGNORE_LOOPBACK _T("IgnoreLoopBack")

static TCHAR	g_szIniFile[_MAX_PATH] = { 0 };

CIni::CIni()
{
	ZeroMemory(&m_stReset, sizeof(SYSTEMTIME));
	m_eResetType = ResetDisabled;
	m_eAlertType = AlertDisabled;
	m_ldwInLimit = 0;
	m_ldwOutLimit = 0;
	m_nAlertPercent = 100;
	m_szLogFile[0] = m_szTrafficLogFile[0] ='\0';
	m_bEnableLog = m_bEnableTrafficLog = FALSE;
	m_bIgnoreStandardLoopBack = TRUE;
}

BOOL CIni::Load()
{
	LPTSTR ptr;
	GetModuleFileName(NULL, g_szIniFile, _MAX_PATH);
	ptr = _tcsrchr(g_szIniFile, '.');
	if(ptr)
		_tcscpy(ptr, _T(".INI"));
	_tcscpy(m_szLogFile, g_szIniFile);
	ptr = _tcsrchr(m_szLogFile, '.');
	if(ptr)
		_tcscpy(ptr, _T(".LOG"));
	_tcscpy(m_szTrafficLogFile, g_szIniFile);
	ptr = _tcsrchr(m_szTrafficLogFile, '.');
	if(ptr)
		_tcscpy(ptr, _T(".CSV"));

	TCHAR szValue[64];
	GetPrivateProfileString(CFG_SECTION, CFG_RESET_DATE, _T(""), szValue, 64, g_szIniFile);
	if(szValue[0])
	{
		CComVariant vDate = szValue;
		HRESULT hr = vDate.ChangeType(VT_DATE);
		if(SUCCEEDED(hr))
			VariantTimeToSystemTime(vDate.date, &m_stReset);
		else
			m_stReset.wDay = _ttoi(szValue);
	}
	m_eResetType = (EResetType)GetPrivateProfileInt(CFG_SECTION, CFG_RESET_TYPE, ResetDisabled, g_szIniFile);
	m_eAlertType = (EAlertType)GetPrivateProfileInt(CFG_SECTION, CFG_ALERT_TYPE, AlertDisabled, g_szIniFile);
	m_ldwInLimit = UInt32x32To64( GetPrivateProfileInt(CFG_SECTION, CFG_IN_LIMIT, 0, g_szIniFile), 1000000000);
	m_ldwOutLimit = UInt32x32To64( GetPrivateProfileInt(CFG_SECTION, CFG_OUT_LIMIT, 0, g_szIniFile), 1000000000);
	m_nAlertPercent = GetPrivateProfileInt(CFG_SECTION, CFG_ALERT_PERCENT, 100, g_szIniFile);

	m_bEnableLog = GetPrivateProfileInt(CFG_SECTION, CFG_LOG_ENABLE, FALSE, g_szIniFile);
	GetPrivateProfileString(CFG_SECTION, CFG_LOG_PATHNAME, m_szLogFile, m_szLogFile, _MAX_PATH, g_szIniFile);

	m_bEnableTrafficLog = GetPrivateProfileInt(CFG_SECTION, CFG_TRAFFIC_LOG_ENABLE, FALSE, g_szIniFile);
	GetPrivateProfileString(CFG_SECTION, CFG_TRAFFIC_LOG_PATHNAME, m_szTrafficLogFile, m_szTrafficLogFile, _MAX_PATH, g_szIniFile);

	m_bIgnoreStandardLoopBack = GetPrivateProfileInt(CFG_SECTION, CFG_IGNORE_LOOPBACK, m_bIgnoreStandardLoopBack, g_szIniFile);
	return TRUE;
}
BOOL CIni::Save()
{
	TCHAR szValue[64];
	if(m_stReset.wYear)
	{
		USES_CONVERSION;
		CComVariant vDate;
		SystemTimeToVariantTime(&m_stReset, &vDate.date);
		vDate.vt = VT_DATE;
		HRESULT hr = vDate.ChangeType(VT_BSTR);
		WritePrivateProfileString(CFG_SECTION, CFG_RESET_DATE, OLE2T(vDate.bstrVal), g_szIniFile);
	}
	else
	{
		_itot(m_stReset.wDay, szValue, 10);
		WritePrivateProfileString(CFG_SECTION, CFG_RESET_DATE, szValue, g_szIniFile);
	}

	_itot(m_eResetType, szValue, 10);
	WritePrivateProfileString(CFG_SECTION, CFG_RESET_TYPE, szValue, g_szIniFile);
	
	_itot(m_eAlertType, szValue, 10);
	WritePrivateProfileString(CFG_SECTION, CFG_ALERT_TYPE, szValue, g_szIniFile);

	_itot((int)(m_ldwInLimit / 1000000000), szValue, 10);
	WritePrivateProfileString(CFG_SECTION, CFG_IN_LIMIT, szValue, g_szIniFile);

	_itot((int)(m_ldwOutLimit / 1000000000), szValue, 10);
	WritePrivateProfileString(CFG_SECTION, CFG_OUT_LIMIT, szValue, g_szIniFile);

	_itot(m_nAlertPercent, szValue, 10);
	WritePrivateProfileString(CFG_SECTION, CFG_ALERT_PERCENT, szValue, g_szIniFile);

	_itot(m_bEnableLog, szValue, 10);
	WritePrivateProfileString(CFG_SECTION, CFG_LOG_ENABLE, szValue, g_szIniFile);

	WritePrivateProfileString(CFG_SECTION, CFG_LOG_PATHNAME, m_szLogFile, g_szIniFile);

	_itot(m_bEnableTrafficLog, szValue, 10);
	WritePrivateProfileString(CFG_SECTION, CFG_TRAFFIC_LOG_ENABLE, szValue, g_szIniFile);

	WritePrivateProfileString(CFG_SECTION, CFG_TRAFFIC_LOG_PATHNAME, m_szTrafficLogFile, g_szIniFile);

	_itot(m_bIgnoreStandardLoopBack, szValue, 10);
	WritePrivateProfileString(CFG_SECTION, CFG_IGNORE_LOOPBACK, szValue, g_szIniFile);
	
	return TRUE;
}

BOOL CIni::LoadNum(LPCTSTR szMac, LPCTSTR szEntry, ULONGLONG* pnVal)
{
	TCHAR szValue[64];
	ATLASSERT(szEntry);
	ATLASSERT(pnVal);
	GetPrivateProfileString(szMac, szEntry, _T("0"), szValue, 64, g_szIniFile);
	*pnVal = _atoi64(szValue);
	return TRUE;
}

BOOL CIni::LoadNum(LPCTSTR szMac, LPCTSTR szEntry, ULONG* pnVal)
{
	TCHAR szValue[64];
	ATLASSERT(szEntry);
	ATLASSERT(pnVal);
	GetPrivateProfileString(szMac, szEntry, _T("0"), szValue, 64, g_szIniFile);
	*pnVal = _ttol(szValue);
	return TRUE;
}

BOOL CIni::SaveNum(LPCTSTR szMac, LPCTSTR szEntry, ULONGLONG nVal)
{
	TCHAR szValue[64];
	_ui64toa(nVal, szValue, 10);
	WritePrivateProfileString(szMac, szEntry, szValue, g_szIniFile);
	return TRUE;
}

BOOL CIni::SaveNum(LPCTSTR szMac, LPCTSTR szEntry, ULONG nVal)
{
	TCHAR szValue[64];
	_ultot(nVal, szValue, 10);
	WritePrivateProfileString(szMac, szEntry, szValue, g_szIniFile);
	return TRUE;
}

BOOL CIni::LoadDate(LPCTSTR szMac, LPCTSTR szEntry, VARIANT* pvDate)
{
	TCHAR szValue[64];
	CComVariant vDate;
	ATLASSERT(pvDate);
	GetPrivateProfileString(szMac, szEntry, _T(""), szValue, 64, g_szIniFile);
	if(szValue[0])
	{
		vDate = szValue;
		HRESULT hr = vDate.ChangeType(VT_DATE);
		if(FAILED(hr))
			vDate.Clear();
	}
	if(vDate.vt != VT_DATE)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		SystemTimeToVariantTime(&st, &vDate.date);
		vDate.vt = VT_DATE;
	}
	VariantCopy(pvDate, &vDate);
	return TRUE;
}

BOOL CIni::SaveDate(LPCTSTR szMac, LPCTSTR szEntry, const VARIANT* pvDate)
{
	USES_CONVERSION;
	ATLASSERT(pvDate);
	CComVariant vDate(*pvDate);
	HRESULT hr = vDate.ChangeType(VT_BSTR);
	WritePrivateProfileString(szMac, szEntry, OLE2T(vDate.bstrVal), g_szIniFile);
	return TRUE;
}
