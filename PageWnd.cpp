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
#include "PageWnd.h"
#include "IPHelper.h"
#include "Ini.h"
#include "Log.h"

#define CFG_TOTAL_IN _T("Total_in")
#define CFG_TOTAL_OUT _T("Total_out")
#define CFG_START_DAYTIME _T("StartDay")
#define CFG_ACTIVE_COUNT_IN _T("ActiveCountIn")
#define CFG_ACTIVE_COUNT_OUT _T("ActiveCountOut")

extern BOOL MustReset(const SYSTEMTIME* stNow, DATE dateStartedAt);
extern CIni ini;
extern CLog log;
extern CLog logTraffic;

HBRUSH g_hAlertBrush = NULL;

LPCTSTR CPageWnd::UpdateIf(const MIB_IFROW* pIfRow)
{
	if(pIfRow == NULL)
	{
		m_bActive = FALSE;
		return NULL;
	}

	m_bActive = TRUE;

	SYSTEMTIME stNow;
	DATE now;
	GetLocalTime(&stNow);
	SystemTimeToVariantTime(&stNow, &now);

	if(m_vStartedAt.vt == VT_EMPTY)
	{
		m_vStartedAt.date = now;
		m_vStartedAt.vt = VT_DATE;
	}
	if(MustReset(&stNow, m_vStartedAt.date))
	{
		TCHAR szMac[64];
		MacAddr2Str(pIfRow->bPhysAddr, pIfRow->dwPhysAddrLen, szMac, 64);
		log.Write(_T("The counters for %s port\t'%s'\twere reset because the reset date was reached\r\n\tIN %10I64u bytes OUT %10I64u bytes\r\n"), szMac, m_szIPAddr, m_ldwInTotal, m_ldwOutTotal);
		if(ini.m_bEnableTrafficLog)
			logTraffic.Write(_T("%s,%s,%I64u,%I64u,%I64u\r\n"), szMac, m_szIPAddr, m_ldwInTotal, m_ldwOutTotal, m_ldwInTotal + m_ldwOutTotal);
		ResetCounters(now);
	}

	//do not compute:
	// - for the first time when m_dwDuration is zero
	// - when we lost connection and we have it back 
	//		since the pIfRow->dwInOctets and pIfRow->dwOutOctets were reset
	//		and the stored values in m_mibIfRow cannot be used for comparision
	if(m_dwDuration++ && pIfRow->dwInOctets >= m_mibIfRow.dwInOctets &&
						 pIfRow->dwOutOctets >= m_mibIfRow.dwOutOctets )
	{
		//IN
		m_dwInPerSec = pIfRow->dwInOctets - m_mibIfRow.dwInOctets;
		m_ldwInTotal += m_dwInPerSec;
		if (m_dwInPerSec > 0)
			m_dwActiveCountIn++;
		if (m_dwInPerSec > m_dwPeakInPerSec)
			m_dwPeakInPerSec = m_dwInPerSec;
		
		if(m_dwActiveCountIn)
			m_dwAverageInPerSec = (DWORD)(m_ldwInTotal / m_dwActiveCountIn);
		else
			m_dwAverageInPerSec = 0;


		//OUT
		m_dwOutPerSec = pIfRow->dwOutOctets - m_mibIfRow.dwOutOctets;
		m_ldwOutTotal += m_dwOutPerSec;
		if(m_dwOutPerSec > 0)
			m_dwActiveCountOut++;
		if(m_dwOutPerSec > m_dwPeakOutPerSec)
			m_dwPeakOutPerSec = m_dwOutPerSec;
		
		if(m_dwActiveCountOut)
			m_dwAverageOutPerSec = (DWORD)(m_ldwOutTotal / m_dwActiveCountOut);
		else
			m_dwAverageOutPerSec = 0;
	}


	if(m_mibIfRow.dwSpeed != pIfRow->dwSpeed)
		m_bSpeedDirty = TRUE;

	ATLASSERT(pIfRow->dwPhysAddrLen <= MAXLEN_PHYSADDR);
	if(memcmp(m_mibIfRow.bPhysAddr, pIfRow->bPhysAddr, pIfRow->dwPhysAddrLen))
		m_bMacDirty = TRUE;

	ATLASSERT(pIfRow->dwDescrLen <= MAXLEN_IFDESCR);
	if(strncmp((LPCSTR)m_mibIfRow.bDescr, (LPCSTR)pIfRow->bDescr, pIfRow->dwDescrLen))
		m_bDescrDirty = TRUE;

	memcpy(&m_mibIfRow, pIfRow, sizeof(MIB_IFROW));
	wcscpy(m_mibIfRow.wszName, pIfRow->wszName);
	
	if(m_bMacDirty)
		memcpy(m_mibIfRow.bPhysAddr, pIfRow->bPhysAddr, pIfRow->dwPhysAddrLen);
	
	if(m_bDescrDirty)
		memcpy(m_mibIfRow.bDescr, pIfRow->bDescr, pIfRow->dwDescrLen);

	TCHAR szNewIP[64];
	GetIPFromIFIndex(m_mibIfRow.dwIndex, szNewIP, 64);
	if(_tcscmp(szNewIP, m_szIPAddr))
	{
		_tcsncpy(m_szIPAddr, szNewIP, 64);
		return m_szIPAddr;
	}
	return NULL;
}

void CPageWnd::UpdateCtrl()
{
	if(m_bDescrDirty)
	{
		SetDlgItemText(IDC_ADAPT_DESCR, (LPCSTR)m_mibIfRow.bDescr);
		m_bDescrDirty = FALSE;
	}
	if(m_bMacDirty)
	{
		TCHAR szText[64];
		MacAddr2Str(m_mibIfRow.bPhysAddr, m_mibIfRow.dwPhysAddrLen, szText, 64);
		SetDlgItemText(IDC_MAC_ADDRESS, szText);
		m_bMacDirty = FALSE;
	}
	if(m_bSpeedDirty)
	{
		TCHAR szTxt[64];
		if(m_mibIfRow.dwSpeed > 10000000)
		{
			_stprintf(szTxt, _T("%d Mbps"), m_mibIfRow.dwSpeed / 1000000);
		}
		else if(m_mibIfRow.dwSpeed > 10000)
		{
			_stprintf(szTxt, _T("%d Kbps"), m_mibIfRow.dwSpeed / 1000);
		}
		else
			_stprintf(szTxt, _T("%d bps"), m_mibIfRow.dwSpeed);
		SetDlgItemText(IDC_SPEED, szTxt);
		m_bSpeedDirty = FALSE;
	}
	if(m_bActive)
	{
		LPCTSTR szStatus = NULL;

		switch(m_mibIfRow.dwOperStatus)
		{
		case MIB_IF_OPER_STATUS_NON_OPERATIONAL:
			szStatus = _T("LAN adapter has been disabled");
			break;
		case MIB_IF_OPER_STATUS_UNREACHABLE:
			szStatus = _T("WAN adapter is not connected");
			break;
		case MIB_IF_OPER_STATUS_DISCONNECTED:
			szStatus = _T("LAN network cable disconnected/WAN no carrier");
			break;
		case MIB_IF_OPER_STATUS_CONNECTING:
			szStatus = _T("WAN adapter that is in the process of connecting");
			break;
		case MIB_IF_OPER_STATUS_CONNECTED:
			szStatus = _T("WAN adapter that is connected to a remote peer");
			break;
		default:
			szStatus = _T("Connected");
		}
		SetDlgItemText(IDC_STATUS, szStatus);
	}
	else
		SetDlgItemText(IDC_STATUS, _T("NOT Connected"));

	SetDlgItemInt(IDC_DURATION, m_dwDuration, FALSE);

	SetDlgItemTextWithUnit(IDC_IN_TR_SEC, m_dwInPerSec, _T("B"));
	SetDlgItemTextWithUnit(IDC_IN_PEAK_SEC, m_dwPeakInPerSec, _T("B"));
	SetDlgItemTextWithUnit(IDC_IN_AVG_SEC, m_dwAverageInPerSec, _T("B"));
	SetDlgItemTextWithUnit(IDC_IN_TOTAL, m_ldwInTotal, _T("B"));
	
	SetDlgItemTextWithUnit(IDC_OUT_TR_SEC, m_dwOutPerSec, _T("B"));
	SetDlgItemTextWithUnit(IDC_OUT_PEAK_SEC, m_dwPeakOutPerSec, _T("B"));
	SetDlgItemTextWithUnit(IDC_OUT_AVG_SEC, m_dwAverageOutPerSec, _T("B"));
	SetDlgItemTextWithUnit(IDC_OUT_TOTAL, m_ldwOutTotal, _T("B"));

	SetDlgItemTextWithUnit(IDC_TOTAL, m_ldwInTotal + m_ldwOutTotal, _T("B"));

	const ULONGLONG ldwInLimit = ini.m_ldwInLimit*ini.m_nAlertPercent/100;
	const ULONGLONG ldwOutLimit = ini.m_ldwOutLimit*ini.m_nAlertPercent/100;

	switch(ini.m_eAlertType)
	{
	case AlertDistinct:
		if(ldwInLimit && m_ldwInTotal > ldwInLimit || ldwOutLimit && m_ldwOutTotal > ldwOutLimit)
		{
			m_bAlert = TRUE;
			log.Write(_T("Monitor port '%s' - reached the alert limit\r\n"), m_szIPAddr);
			SetDlgItemText(IDC_ALERT, m_ldwInTotal > ldwInLimit ? 
				_T("IN passed the limit") : 
				_T("OUT passed the limit") );
			break;
		}
	case AlertInOut:
		if(ldwInLimit + ldwOutLimit && m_ldwInTotal + m_ldwOutTotal > ldwInLimit + ldwOutLimit)
		{
			m_bAlert = TRUE;
			SetDlgItemText(IDC_ALERT, _T("IN+OUT passed the limit") );
			break;
		}
	case AlertDisabled:
		m_bAlert = FALSE;
		SetDlgItemText(IDC_ALERT, NULL);
		break;
	}
	if(m_bActive == FALSE && m_bActiveDirty)
	{
		m_bActiveDirty = FALSE;
		log.Write(_T("Monitor port '%s' - connection was lost\r\n"), m_szIPAddr);
	}
}

LRESULT CPageWnd::OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_bAlert == FALSE || (HWND)lParam != GetDlgItem(IDC_ALERT))
	{
		bHandled = FALSE;
		return 0;
	}
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LPARAM)g_hAlertBrush;
}


void CPageWnd::LoadStart(LPCTSTR szMac)
{
	CComVariant vDate;

	ini.LoadNum(szMac, CFG_TOTAL_IN,		&m_ldwInTotal);
	ini.LoadNum(szMac, CFG_TOTAL_OUT,		&m_ldwOutTotal);
	ini.LoadNum(szMac, CFG_ACTIVE_COUNT_IN, &m_dwActiveCountIn);
	ini.LoadNum(szMac, CFG_ACTIVE_COUNT_OUT, &m_dwActiveCountOut);

	ini.LoadDate(szMac, CFG_START_DAYTIME, &vDate);
	if(vDate.vt == VT_DATE)
		UpdateStartedDate(vDate.date);
}

LRESULT CPageWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TCHAR szMac[64];
	MacAddr2Str(m_mibIfRow.bPhysAddr, m_mibIfRow.dwPhysAddrLen, szMac, 64);
	log.Write(_T("Monitor %s port\t'%s'\tStop IN %10I64u bytes OUT %10I64u bytes\r\n"), szMac, m_szIPAddr, m_ldwInTotal, m_ldwOutTotal);
	SaveEnd();
	return 0;
}


void CPageWnd::SaveEnd()
{
	CComVariant vDate;
	TCHAR szMac[64];
	MacAddr2Str(m_mibIfRow.bPhysAddr, m_mibIfRow.dwPhysAddrLen, szMac, 64);

	ini.SaveNum(szMac, CFG_TOTAL_IN,		m_ldwInTotal);
	ini.SaveNum(szMac, CFG_TOTAL_OUT,		m_ldwOutTotal);
	ini.SaveNum(szMac, CFG_ACTIVE_COUNT_IN, m_dwActiveCountIn);
	ini.SaveNum(szMac, CFG_ACTIVE_COUNT_OUT, m_dwActiveCountOut);

	if(m_vStartedAt.vt == VT_DATE)
	{
		ini.SaveDate(szMac, CFG_START_DAYTIME, &m_vStartedAt);
	}
}

void CPageWnd::SetDlgItemTextWithUnit(UINT nID, ULONGLONG ldwValue, LPCTSTR szUnit)
{
	TCHAR szTxt[64];
	SetTextWithUnit(ldwValue, szUnit, szTxt, 64);
	SetDlgItemText(nID, szTxt);
}

long CPageWnd::GetTipText(LPTSTR szText, long cbSize) const
{
	long used_len;
	used_len = _sntprintf(szText, cbSize, _T("In "));
	used_len += SetTextWithUnit(m_ldwInTotal, _T("B"), szText+used_len, cbSize-used_len);

	used_len += _sntprintf(szText+used_len, cbSize-used_len, _T(", Out "));
	used_len += SetTextWithUnit(m_ldwOutTotal, _T("B"), szText+used_len, cbSize-used_len);

	used_len += _sntprintf(szText+used_len, cbSize-used_len, _T(", All "));
	used_len += SetTextWithUnit(m_ldwInTotal + m_ldwOutTotal, _T("B"), szText+used_len, cbSize-used_len);
	return used_len;
}
