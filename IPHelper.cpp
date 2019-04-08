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
// IPHelper.cpp
//	04-Oct-2005	MS	Modified GetIPFromIFIndex fix memory leak
#include "stdafx.h"
#include "IPHelper.h"


const LPCWSTR  NULL_IP = L"  0.  0.  0.  0";

//------conversion of well-known port numbers to service names----------------

struct SId2Description
{
    DWORD	key;
    LPTSTR	value[20];
};

struct STcpConnStatus
{
	CComBSTR	LocalIP;
    CComBSTR	LocalPort;
    CComBSTR	RemoteIP;
    CComBSTR	RemotePort;
    CComBSTR	Status;
};


const
    // only most "popular" services...
	SId2Description WellKnownPorts[] = {
    { 0, _T("LOOPBACK")	},
    { 7, _T("ECHO")		},   // Ping    
    { 9, _T("DISCRD")		},   // Discard
    { 13, _T("DAYTIM")		},   // DayTime
    { 17, _T("QOTD")		},   // Quote Of The Day
    { 19, _T("CHARGEN")	},   // CharGen
    { 20, _T("FTP ")		},   // File Transfer Protocol
    { 21, _T("FTPC")		},   // File Transfer Control Protocol
    { 23, _T("TELNET")		},   // TelNet
    { 25, _T("SMTP")		},   // Simple Mail Transfer Protocol
    { 37, _T("TIME")		},   // Time Protocol 
    { 43, _T("WHOIS")		},   // WHO IS service  
    { 53, _T("DNS ")		},   // Domain Name Service 
    { 67, _T("BOOTPS")		},   // BOOTP Server 
    { 68, _T("BOOTPC")		},   // BOOTP Client 
    { 69, _T("TFTP")		},   // Trivial FTP  
    { 70, _T("GOPHER")		},   // Gopher       
    { 79, _T("FING")		},   // Finger       
    { 80, _T("HTTP")		},   // HTTP         
    { 88, _T("KERB")		},   // Kerberos     
    { 109, _T("POP2")		},   // Post Office Protocol Version 2 
    { 110, _T("POP3")		},   // Post Office Protocol Version 3 
    { 119, _T("NNTP")		},   // Network News Transfer Protocol 
    { 123, _T("NTP ")		},   // Network Time protocol          
    { 135, _T("LOCSVC")	},   // Location Service
    { 137, _T("NBNAME")	},   // NETBIOS Name service
    { 138, _T("NBDGRAM")	},   // NETBIOS Datagram Service
    { 139, _T("NBSESS")	},   // NETBIOS Session Service
    { 161, _T("SNMP")		},    // Simple Netw. Management Protocol
	{0, NULL }
};


//-----------conversion of ICMP error codes to strings--------------------------
             /* taken from www.sockets.com/ms_icmp.c */

const long
  ICMP_ERROR_BASE = 11000;
  LPCWSTR IcmpErr[22] =
  {
   L"IP_BUFFER_TOO_SMALL", L"IP_DEST_NET_UNREACHABLE", L"IP_DEST_HOST_UNREACHABLE",
   L"IP_PROTOCOL_UNREACHABLE", L"IP_DEST_PORT_UNREACHABLE", L"IP_NO_RESOURCES",
   L"IP_BAD_OPTION", L"IP_HARDWARE_ERROR", L"IP_PACKET_TOO_BIG", L"IP_REQUEST_TIMED_OUT",
   L"IP_BAD_REQUEST", L"IP_BAD_ROUTE", L"IP_TTL_EXPIRED_TRANSIT",
   L"IP_TTL_EXPIRED_REASSEM", L"IP_PARAMETER_PROBLEM", L"IP_SOURCE_QUENCH",
   L"IP_OPTION_TOO_BIG", L"IP_BAD_DESTINATION", L"IP_ADDRESS_DELETED",
   L"IP_SPEC_MTU_CHANGE", L"IP_MTU_CHANGE", L"IP_UNLOAD"
  };


//----------conversion of various enumerated values to strings----------------//

  LPCWSTR ARPEntryType[] = { L"Other", L"Invalid",
    L"Dynamic", L"Static"
  };

  LPCWSTR TCPConnState[] =      // TCP connection states
  { L"closed", L"listening", L"syn_sent",
    L"syn_rcvd", L"established", L"fin_wait1",
    L"fin_wait2", L"close_wait", L"closing",
    L"last_ack", L"time_wait", L"delete_tcb"
  };

  LPCWSTR TCPToAlgo[] =  // TCP time out algorithms
  { L"Const.Timeout", L"MIL-STD-1778",
  L"Van Jacobson", L"Other" };

  LPCWSTR IPForwTypes[] =   // IP forwarding methods
  { L"other", L"invalid", L"local", L"remote" };

  LPCWSTR IPForwProtos[] =  // IP forwarding protocols
  { L"OTHER", L"LOCAL", L"NETMGMT", L"ICMP", L"EGP",
    L"GGP", L"HELO", L"RIP", L"IS_IS", L"ES_IS",
    L"CISCO", L"BBN", L"OSPF", L"BGP", L"BOOTP",
    L"AUTO_STAT", L"STATIC", L"NOT_DOD" };


static const SId2Description  NETBIOSTypes[] = 
{
	{ BROADCAST_NODETYPE,	 _T("Broadcast")},
	{ PEER_TO_PEER_NODETYPE, _T("Peer to peer")},
	{ MIXED_NODETYPE,		 _T("Mixed")},
	{ HYBRID_NODETYPE,		 _T("Hybrid")},
	{ 0, NULL }
};



//---------------exported stuff-----------------------------------------------

// data output to Tstrings for display purposes
void Get_AdaptersInfo( CSimpleArray<CComBSTR>& List );
void Get_NetworkParams( LPTSTR szTxt, long cbSize );
void Get_ARPTable( CSimpleArray<CComBSTR>& List );
void Get_TCPTable( CSimpleArray<CComBSTR>& List );
void Get_TCPStatistics( CSimpleArray<CComBSTR>& List );
void Get_UDPTable( CSimpleArray<CComBSTR>& List );
void Get_UDPStatistics( CSimpleArray<CComBSTR>& List );
void Get_IPAddrTable( CSimpleArray<CComBSTR>& List );
void Get_IPForwardTable( CSimpleArray<CComBSTR>& List );
void Get_IPStatistics( CSimpleArray<CComBSTR>& List );


// conversion utils
void IpAddr2Str( DWORD IPAddr, LPTSTR szIPAddr, long cbSize );
DWORD Str2IpAddr( LPCWSTR IPStr );
CComBSTR Port2Svc( DWORD Port );
CComBSTR ICMPErr2Str( DWORD ICMPErrCode );


//--------------General utilities-----------------------------------------------


// converts numerical MAC-address to ww-xx-yy-zz string 
void MacAddr2Str( const BYTE* MacAddr, long cbSizeMac, LPTSTR szMac, long cbSizeStr )
{
	long   i, j;
	if (cbSizeMac == 0)
	{
		_tcsncpy(szMac, _T("00-00-00-00-00-00"), cbSizeStr);
	}
	else 
	{
		for(i = j = 0; i<cbSizeMac; i++)
		{
			j+=_sntprintf(szMac+j, cbSizeStr-j, _T("%02X-"), MacAddr[i]);
		}
		szMac[j-1] = '\0';
	}
}

//------------------------------------------------------------------------------
//converts IP-address in network byte order DWORD to dotted decimal string
void IpAddr2Str( DWORD IPAddr, LPTSTR szIPAddr, long cbSize )
{
	long i, j;

	ATLASSERT(szIPAddr);
	for(i = j = 0; i<4; i++)
	{
		DWORD Num = (IPAddr >> 8*i) & 0xFF;
		j += _sntprintf(szIPAddr+j, cbSize-j, _T("%d."), Num);
	}
	szIPAddr[j-1] = '\0';
}

//------------------------------------------------------------------------------
// converts dotted decimal IP-address to network byte order DWORD}
DWORD Str2IpAddr( LPCWSTR IPStr )
{
	LPWSTR ptr, szTemp = _wcsdup( IPStr );
	DWORD	Num=0, tmp;
	long i;
	for(ptr = wcstok(szTemp, L"."), i=0; ptr; ptr = wcstok(NULL, L"."), i++)
	{
		tmp = _wtoi(ptr);
		tmp <<= 8*(3-i);
		Num |= tmp;
	}
	free(szTemp);
	return Num;
}

//------------------------------------------------------------------------------
//converts well-known port numbers to service ID 
CComBSTR Port2Svc( DWORD Port )
{
	int i;

	for(i=0; WellKnownPorts[i].value; i++)
	{
		if (Port = WellKnownPorts[i].key)
		    return CComBSTR( (LPCWSTR)WellKnownPorts[i].value );
    }
	WCHAR szTmp[8];
	_snwprintf(szTmp, 8, L"[%4d]", Port ); // in case port not found
	return CComBSTR(szTmp);
}

//-----------------------------------------------------------------------------
// general,  fixed network parameters
void Get_NetworkParams( LPTSTR szTxt, long cbSize )
{
	DWORD	ulOutBufLen = sizeof( FIXED_INFO ), i;
	DWORD	ret = ERROR_NOT_SUPPORTED;
	FIXED_INFO* FixedInfo;

	FixedInfo = (FIXED_INFO *) malloc( ulOutBufLen );
	ZeroMemory(FixedInfo, ulOutBufLen);

	ret = GetNetworkParams( FixedInfo, &ulOutBufLen );
	if( ret == ERROR_INSUFFICIENT_BUFFER ) 
	{
		FixedInfo = (FIXED_INFO*)realloc( FixedInfo, ulOutBufLen );
		ZeroMemory(FixedInfo, ulOutBufLen);
		ret = GetNetworkParams( FixedInfo, &ulOutBufLen );
	}
	
	if(ret == ERROR_SUCCESS)
	{
		for(i=0; NETBIOSTypes[i].value; i++)
			if(NETBIOSTypes[i].key == FixedInfo->NodeType)
				break;
		_sntprintf(szTxt, cbSize, 
			_T("HOSTNAME          : %s\r\n")
			_T("DOMAIN            : %s\r\n")
			_T("SCOPE             : %s\r\n")
			_T("NETBIOS NODE TYPE : %s\r\n")
			_T("ROUTING ENABLED   : %d\r\n")
			_T("PROXY   ENABLED   : %d\r\n")
			_T("DNS     ENABLED   : %d\r\n"),
			FixedInfo->HostName, FixedInfo->DomainName, FixedInfo->ScopeId,
			NETBIOSTypes[i].value, FixedInfo->EnableRouting,
			FixedInfo->EnableProxy, FixedInfo->EnableDns);
    }
	else
		Win32Message( ret, szTxt, cbSize );
	free(FixedInfo);
}

//------------------------------------------------------------------------------
CComBSTR ICMPErr2Str( DWORD ICMPErrCode )
{
	WCHAR szTmp[8];
	CComBSTR Result(L"UnknownError : ");
	Result += _itow(ICMPErrCode, szTmp, 10 );
	if(ICMPErrCode > ICMP_ERROR_BASE && ICMPErrCode < ICMP_ERROR_BASE+22)
		Result = IcmpErr[ ICMPErrCode - ICMP_ERROR_BASE - 1];
	return Result;
}

BOOL GetIPFromIFIndex(DWORD InterfaceIndex, LPTSTR szIPAddr, long cbSize)
{
	DWORD	ulOutBufLen = sizeof( MIB_IPADDRTABLE ), i;
	DWORD	ret = ERROR_NOT_SUPPORTED;
	MIB_IPADDRTABLE* pIpAddressTable;


	pIpAddressTable = (MIB_IPADDRTABLE *) malloc( ulOutBufLen );
	if(pIpAddressTable == NULL)
		return FALSE;
	ZeroMemory(pIpAddressTable, ulOutBufLen);

	ret = GetIpAddrTable( pIpAddressTable, &ulOutBufLen, FALSE );
	if( ret == ERROR_INSUFFICIENT_BUFFER)
	{
		pIpAddressTable = (MIB_IPADDRTABLE*)realloc(pIpAddressTable, ulOutBufLen );
		ZeroMemory(pIpAddressTable, ulOutBufLen);
		ret = GetIpAddrTable( pIpAddressTable, &ulOutBufLen, FALSE );
	}
	
	_sntprintf(szIPAddr, cbSize, _T("!not_found!"));
	if(ret == ERROR_SUCCESS)
	{
		for(i=0; i< pIpAddressTable->dwNumEntries; i++)  // look for matching index...
		{
			if(pIpAddressTable->table[i].dwIndex == InterfaceIndex)
			{
				IpAddr2Str(pIpAddressTable->table[i].dwAddr, szIPAddr, cbSize);
				
				free(pIpAddressTable);
				return TRUE;
			}
		}
	}
	else
	{
		Win32Message( ret, szIPAddr, cbSize );
	}
	free(pIpAddressTable);
	return FALSE;
}
