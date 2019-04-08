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
#ifndef __LOG__H__
#define __LOG__H__

class CLog
{
	HANDLE m_hFile;
	LPCTSTR m_szFileName;
	BOOL	m_bIsCsv;

public:
	CLog(LPCTSTR szFileName, BOOL bIsCsv);
	BOOL Write(LPCTSTR szFormat, ...);
};

#endif // __LOG__H__