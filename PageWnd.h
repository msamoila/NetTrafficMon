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
#ifndef __PAGE_WND_H_
#define __PAGE_WND_H_

#include "resource.h"

class CMainWnd;

class CPageWnd : public CDialogImpl<CPageWnd>
{
protected:
	CMainWnd*	m_pParent;
	MIB_IFROW	m_mibIfRow;
	BOOL		m_bActive;
	DWORD		m_dwDuration;
	CComVariant	m_vStartedAt;

	DWORD		m_dwActiveCountIn;
	DWORD		m_dwActiveCountOut;
	ULONGLONG	m_ldwInTotal;
	ULONGLONG	m_ldwOutTotal;
	DWORD		m_dwInPerSec;
	DWORD		m_dwOutPerSec;
	DWORD		m_dwPeakInPerSec;
	DWORD		m_dwPeakOutPerSec;
	DWORD		m_dwAverageInPerSec;
	DWORD		m_dwAverageOutPerSec;
	TCHAR		m_szIPAddr[64];
	BOOL		m_bActiveDirty;
public:

	CPageWnd(LPCTSTR szIPAddr)
	{
		m_pParent = NULL;
		m_bDescrDirty = TRUE;
		m_bMacDirty = TRUE;
		m_bSpeedDirty = TRUE;
		m_bActive = FALSE;
		m_dwDuration = 0;
		m_bAlert = FALSE;
		m_bActiveDirty = TRUE;

		m_dwActiveCountIn = m_dwActiveCountOut = 0;
		m_ldwInTotal = m_ldwOutTotal = 0;
		m_dwInPerSec = m_dwOutPerSec = 0;
		m_dwPeakInPerSec = m_dwPeakOutPerSec = 0;

		ZeroMemory(&m_mibIfRow, sizeof(MIB_IFROW));
		_tcsncpy(m_szIPAddr, szIPAddr, 64);
	}
public:
	enum { IDD = IDD_IF_PAGE };
protected:

BEGIN_MSG_MAP(CPageWnd)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
END_MSG_MAP()

	
	void OnActivate() {} ;
	void OnDeactivate() {} ;
	~CPageWnd() {} ;

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_pParent = (CMainWnd*)lParam;
		ATLASSERT(m_pParent);
		return TRUE;
	}
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
//		HDWP hDef = ::BeginDeferWindowPos(2);
//		::DeferWindowPos(hDef, GetDlgItem(IDC_LOG), NULL, 0, 0,
//			LOWORD(lParam)-15, HIWORD(lParam) - 140, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
//		::EndDeferWindowPos(hDef);
		return 0;
	}
	LRESULT OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BOOL IsEqualMAC(const BYTE* pPhysAddr, const DWORD cbLen) const
	{
		if(cbLen != m_mibIfRow.dwPhysAddrLen)
			return FALSE;
		return !memcmp(m_mibIfRow.bPhysAddr, pPhysAddr, cbLen); 
	}
	//returns the IP address string if IP address has been changed, or NULL otherwise
	LPCTSTR UpdateIf(const MIB_IFROW* pIfRow);
	void UpdateCtrl();
	void LoadStart(LPCTSTR szMac);
	void SaveEnd();
	void ResetCounters(DATE dtNewDate)
	{
		UpdateStartedDate(dtNewDate);
		m_dwDuration = 0;
		m_dwActiveCountIn = m_dwActiveCountOut = 0;
		m_ldwInTotal = m_ldwOutTotal = 0;
		m_dwInPerSec = m_dwOutPerSec = 0;
		m_dwPeakInPerSec = m_dwPeakOutPerSec = 0;
	}
	void UpdateStartedDate(DATE dtNewDate)
	{
		SYSTEMTIME st;
		m_vStartedAt.date = dtNewDate;
		m_vStartedAt.vt = VT_DATE;
		if(m_hWnd)
		{
			VariantTimeToSystemTime(m_vStartedAt.date, &st);
			TCHAR buff[256];
			DWORD i = GetDateFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, buff, 256);
			if(i--)
			{
				i += _sntprintf(buff+i, 256-i, _T("\r\n"));
				GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, buff+i, 256-i);
			}
			SetDlgItemText(IDC_DATE_FROM, buff);
		}
	}
	void SetDlgItemTextWithUnit(UINT nID, ULONGLONG ldwValue, LPCTSTR szUnit);
	long GetTipText(LPTSTR szText, long cbSize) const;
	BOOL IsAlert() const { return m_bAlert; }
private:
	BOOL m_bDescrDirty, m_bMacDirty, m_bSpeedDirty;
	BOOL m_bAlert;
};


#endif // __PAGE_WND_H_