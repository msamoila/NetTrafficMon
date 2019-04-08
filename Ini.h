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
#ifndef __INI__H__
#define __INI__H__

class CIni
{
public:
	SYSTEMTIME	m_stReset;
	EResetType	m_eResetType;
	EAlertType	m_eAlertType;
	ULONGLONG	m_ldwInLimit;
	ULONGLONG	m_ldwOutLimit;
	BYTE		m_nAlertPercent;
	TCHAR		m_szLogFile[_MAX_PATH];
	BOOL		m_bEnableLog;
	TCHAR		m_szTrafficLogFile[_MAX_PATH];
	BOOL		m_bEnableTrafficLog;
	BOOL		m_bIgnoreStandardLoopBack;

	CIni();

	BOOL Load();
	BOOL Save();

	BOOL LoadNum(LPCTSTR szMac, LPCTSTR szEntry, ULONGLONG* pnVal);
	BOOL LoadNum(LPCTSTR szMac, LPCTSTR szEntry, ULONG* pnVal);
	BOOL SaveNum(LPCTSTR szMac, LPCTSTR szEntry, ULONGLONG nVal);
	BOOL SaveNum(LPCTSTR szMac, LPCTSTR szEntry, ULONG nVal);
	BOOL LoadDate(LPCTSTR szMac, LPCTSTR szEntry, VARIANT* pvDate);
	BOOL SaveDate(LPCTSTR szMac, LPCTSTR szEntry, const VARIANT* pvDate);
};


#endif // __INI__H__