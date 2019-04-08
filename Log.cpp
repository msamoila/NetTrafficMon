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
//  Marius Samoila, 2011
//
#include "stdafx.h"
#include "Log.h"
#include "Ini.h"

extern CIni ini;


CLog::CLog(LPCTSTR szFileName, BOOL bIsCsv)
{
	m_hFile = INVALID_HANDLE_VALUE;
	ATLASSERT(szFileName);
	m_szFileName = szFileName;
	m_bIsCsv = bIsCsv;
}

BOOL CLog::Write(LPCTSTR szFormat, ...)
{
	BOOL bRet = FALSE;
	DWORD dwWritten = 0;
	long lMaxLen = _tcslen(szFormat) + 256;
	LPTSTR szText = (LPTSTR)malloc(lMaxLen*sizeof(TCHAR));
	if(szText == NULL)
		return FALSE;

	DWORD i, len;
	
	SYSTEMTIME st;
	GetLocalTime(&st);
	i = GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &st, _T("dd'/'MMM'/'yyyy'"), szText, lMaxLen);
	if(i--)
		len = i;

	if(m_bIsCsv)
	{
		len += _sntprintf(szText+len, lMaxLen-len, _T(","));
	}
	else
	{
		if(ini.m_bEnableLog == FALSE)
			goto $end;
		i = GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, &st, _T(" HH':'mm':'ss'"), szText+len, lMaxLen-len);
		if(i--)
		{
			len += i;
			len += _sntprintf(szText+len, lMaxLen-len, _T(".%03d "), st.wMilliseconds);
		}
	}

	va_list va;
	va_start( va, szFormat);

	len += _vsntprintf(szText+len, lMaxLen-len, szFormat, va);
	va_end( va );


	m_hFile = CreateFile(m_szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
	if(m_hFile == INVALID_HANDLE_VALUE)
		goto $end;

	SetFilePointer(m_hFile, 0, NULL, FILE_END);

	WriteFile(m_hFile, szText, len*sizeof(TCHAR), &dwWritten, NULL);

	if(dwWritten == len*sizeof(TCHAR))
		bRet = TRUE;

	CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;
$end:
	free(szText);
	return bRet;
}
