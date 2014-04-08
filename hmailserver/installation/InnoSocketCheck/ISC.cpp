// ISC.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ISC.h"

#include <winsock2.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

bool CheckPort(int iPort)
{
   SOCKET sSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL,0, 0 );

   // --- Bind the socket to a port.
   SOCKADDR_IN sa;
   memset( &sa, 0, sizeof( sa ) );
   sa.sin_family = AF_INET;
   u_short us = iPort;
   sa.sin_port = htons( us );
   sa.sin_addr.s_addr = htonl( INADDR_ANY );

   bool bResult = true;
   if ( bind( sSock, (sockaddr*)&sa, sizeof(sa) ) )
      bResult = false;

   return bResult;

}


ISC_API int CheckPorts(void)
{
   WORD wVersionRequested;
   WSADATA wsaData;

   wVersionRequested = MAKEWORD( 1, 1 );
   WSAStartup( wVersionRequested, &wsaData );

   if (!CheckPort(25))
      return -1;

   if (!CheckPort(110))
      return -1;
   
   if (!CheckPort(143))
      return -1;

   WSACleanup();

   return 0;
}

