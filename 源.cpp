
#include <stdio.h>
#include <tchar.h>
#include <winsock2.h> 
#include <ws2bth.h> 

#pragma comment(lib,"ws2_32.lib") 
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS


  int main(int argc, _TCHAR* argv[])
{
	// ��ʼ��winsock 
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		_tprintf(_T("WSAStartup failed with error code: %d\n"), WSAGetLastError());
		return 1;
	}

	// ��ʼ������ö�ٽṹ 
	DWORD dwWsaqsLen = sizeof(WSAQUERYSET);
	LPWSAQUERYSET lpWsaqs = (LPWSAQUERYSET)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwWsaqsLen);
	lpWsaqs->dwSize = sizeof(WSAQUERYSET);
	lpWsaqs->dwNameSpace = NS_BTH;

	// ��ʼö�������豸 
	DWORD dwWsaqsFlags = LUP_CONTAINERS | LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_ADDR;
	HANDLE hService;

	if (WSALookupServiceBegin(lpWsaqs, dwWsaqsFlags, &hService) != SOCKET_ERROR)
	{
		bool bFinished = false;
		while (!bFinished)
		{
			if (WSALookupServiceNext(hService, dwWsaqsFlags, &dwWsaqsLen, lpWsaqs) == NO_ERROR)
			{
				SOCKADDR_BTH *saBth = (SOCKADDR_BTH*)lpWsaqs->lpcsaBuffer->RemoteAddr.lpSockaddr;
				BTH_ADDR bthAddr = saBth->btAddr;

				_tprintf(_T("\n���������豸: \n"));
				_tprintf(_T("------------------------\n"));
				_tprintf(_T("NAP: 0x%04X  SAP: 0x%08X �豸����: %s\n"), GET_NAP(bthAddr), GET_SAP(bthAddr), lpWsaqs->lpszServiceInstanceName);
			}
			else
			{
				// ������ 
				switch (WSAGetLastError())
				{
					// ������������С���������� 
				case WSAEFAULT:
					HeapFree(GetProcessHeap(), 0, lpWsaqs);
					lpWsaqs = (LPWSAQUERYSET)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwWsaqsLen);
					break;

					// û�и��������豸�� 
				case WSA_E_NO_MORE:
					bFinished = true;
					break;

				default:
					//bFinished = true; 
					break;
				}
			}
		}
		WSALookupServiceEnd(hService);
	}

	_tprintf(_T("\n���������豸: \n"));

	// �ͷ���Դ 
	HeapFree(GetProcessHeap(), 0, lpWsaqs);
	WSACleanup();

	return 0;
}
