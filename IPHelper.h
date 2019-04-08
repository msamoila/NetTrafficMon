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
#ifndef __IP_HELPER__H__
#define __IP_HELPER__H__

void MacAddr2Str( const BYTE* MacAddr, long cbSizeMac, LPTSTR szMac, long cbSizeStr );
BOOL GetIPFromIFIndex(DWORD InterfaceIndex, LPTSTR szIPAddr, long cbSize);

#endif // __IP_HELPER__H__
