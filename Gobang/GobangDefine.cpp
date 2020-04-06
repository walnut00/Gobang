#include "stdafx.h"
#include "GobangDefine.h"

bool GetLocalIP(DWORD * pnIP, CString * pstrIP)
{ 
	WORD wVersionRequested;
	WSADATA wsaData;
	DWORD nIP;
	int err;

	wVersionRequested = MAKEWORD( 1, 1);	 
	err = WSAStartup( wVersionRequested, &wsaData );
	if(err != 0)
	{
		return false;
	}

	char szName[128]; 
	memset(szName, 0, 128);
	if(0 != gethostname(szName, 128))
	{
		return false;
	}

	hostent *pHost = gethostbyname(szName); 
	if(NULL == pHost)//failed 
	{
		return false;
	}

	nIP = ntohl(**(DWORD**)(pHost-> h_addr_list));
	if(NULL != pnIP) *pnIP = nIP;
	if(NULL != pstrIP) pstrIP->Format("%d.%d.%d.%d", (0xFF000000&nIP)>>24,(0xFF0000&nIP)>>16,(0xFF00&nIP)>>8,0xFF&nIP);

	return true;
} 

CString TransferIP(const DWORD & dwIPAddress)
{
	CString strReturn = "";
	strReturn.Format("%d.%d.%d.%d", (0xFF000000&dwIPAddress)>>24,(0xFF0000&dwIPAddress)>>16,(0xFF00&dwIPAddress)>>8,0xFF&dwIPAddress);
	return strReturn;
}


DWORD TransferIP(const char * strIPAddress)
{
	char * pCh = NULL;
	char *pNextChar = NULL;
	BYTE nField[4] ;
	int i = 0;
	memset(nField, 0, 4);
	pCh = strtok_s((LPTSTR)strIPAddress, ".", &pNextChar);
	while ((pCh != NULL))
	{
			nField[i++] = atoi(pCh);
			pCh = strtok_s( NULL, ".", &pNextChar);
	}

	return	(nField[0]<<24) + (nField[1]<<16) +(nField[2]<<8) + nField[3];
}