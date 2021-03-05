
#include <stdio.h>
#include <tchar.h>
#include <winsock2.h> 
#include <ws2bth.h> 

#pragma comment(lib,"ws2_32.lib") 
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS


  int main(int argc, _TCHAR* argv[])
{
	// 初始化winsock 
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		_tprintf(_T("WSAStartup failed with error code: %d\n"), WSAGetLastError());
		return 1;
	}

	// 初始化蓝牙枚举结构 
	DWORD dwWsaqsLen = sizeof(WSAQUERYSET);
	LPWSAQUERYSET lpWsaqs = (LPWSAQUERYSET)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwWsaqsLen);
	lpWsaqs->dwSize = sizeof(WSAQUERYSET);
	lpWsaqs->dwNameSpace = NS_BTH;

	// 开始枚举蓝牙设备 
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

				_tprintf(_T("\n发现蓝牙设备: \n"));
				_tprintf(_T("------------------------\n"));
				_tprintf(_T("NAP: 0x%04X  SAP: 0x%08X 设备名称: %s\n"), GET_NAP(bthAddr), GET_SAP(bthAddr), lpWsaqs->lpszServiceInstanceName);
			}
			else
			{
				// 错误处理 
				switch (WSAGetLastError())
				{
					// 所给缓冲区过小，重新申请 
				case WSAEFAULT:
					HeapFree(GetProcessHeap(), 0, lpWsaqs);
					lpWsaqs = (LPWSAQUERYSET)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwWsaqsLen);
					break;

					// 没有更多蓝牙设备了 
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

	_tprintf(_T("\n发现蓝牙设备: \n"));

	// 释放资源 
	HeapFree(GetProcessHeap(), 0, lpWsaqs);
	WSACleanup();

	return 0;
}
