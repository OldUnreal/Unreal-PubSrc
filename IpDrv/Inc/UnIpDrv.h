/*=============================================================================
	IpDrvPrivate.h: Unreal TCP/IP driver.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Tim Sweeney.
=============================================================================*/

#if _MSC_VER
	#pragma warning( disable : 4201 )
#endif

// Socket API.
#if _MSC_VER
	#define __WINSOCK__ 1
	#define SOCKET_API TEXT("WinSock")
#else
	#define __BSD_SOCKETS__ 1
	#if ((MACOSX) || (defined __FreeBSD__) || (MACOSXPPC))
		#define __BSD_SOCKETS_TRUE_BSD__ 1
	#endif
	#define SOCKET_API TEXT("Sockets")
#endif

// WinSock includes.
#if __WINSOCK__
	#include <windows.h>
	#include <winsock.h>
	#include <conio.h>
	#define GETHOSTBYNAME_LOCK()
	#define GETHOSTBYNAME_UNLOCK()
#endif

// BSD socket includes.
#if __BSD_SOCKETS__
	#include <stdio.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <sys/uio.h>
	#include <sys/ioctl.h>
	#include <sys/time.h>
	#include <errno.h>
	#include <pthread.h>
	#include <fcntl.h>

	// Handle glibc < 2.1.3.
	#ifndef MSG_NOSIGNAL
	#define MSG_NOSIGNAL 0x4000
	#endif

	extern pthread_mutex_t gethostbyname_mutex;
	#define GETHOSTBYNAME_LOCK() pthread_mutex_lock(&gethostbyname_mutex)
	#define GETHOSTBYNAME_UNLOCK() pthread_mutex_unlock(&gethostbyname_mutex)
#endif

#include "Engine.h"
#include "UnNet.h"
#include "UnSocket.h"

/*-----------------------------------------------------------------------------
	Definitions.
-----------------------------------------------------------------------------*/

#define MAXRECVDATASIZE 4096

// An IP address.
struct FIpAddr
{
	DWORD Addr;
	DWORD Port;
};

// SocketData.
struct FSocketData
{
	SOCKADDR_IN Addr;
	INT Port;
	SOCKET Socket;
};

struct LogUDPClients
{
	sockaddr_in ClientAdress;
	FTime LogTime;
};

// OperationStats
struct FOperationStats
{
	INT MessagesServiced;
	INT BytesReceived;
	INT BytesSent;
};

// Globals.
extern UBOOL GInitialized;

/*-----------------------------------------------------------------------------
	Host resolution thread.
-----------------------------------------------------------------------------*/

#define MAX_RESOLVE_THREADS 8 // Maximum number of cothreads to resolve URL's.

//
// Class for creating a background thread to resolve a host.
//
class FResolveInfo : public FThreadingBase
{
	static INT NumResolving;

public:
	// Variables.
	in_addr		Addr;
	TCHAR		HostName[256];
	ANSICHAR	AnsiHostName[256];
	TCHAR		Error[256];
	UBOOL		bPaused;

	// Functions.
	FResolveInfo(const TCHAR* InHostName);

	~FResolveInfo();

	UBOOL Resolved()
	{
		return !IsRunning() && !bPaused;
	}
	const TCHAR* GetError()
	{
		return *Error ? Error : NULL;
	}
	const in_addr GetAddr()
	{
		return Addr;
	}
	const TCHAR* GetHostName()
	{
		return HostName;
	}
	void Main();

	inline void PollStart()
	{
		guard(FResolveInfo::PollStart);
		if (bPaused && NumResolving < MAX_RESOLVE_THREADS)
		{
			++NumResolving;
			bPaused = FALSE;
			RunThread();
		}
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	Bind to next available port.
-----------------------------------------------------------------------------*/

/*
#ifdef __linux__
#include <stdlib.h>  // (need rand() for bindnextport().  --ryan.)
#endif
*/

inline int trybindport(SOCKET s, struct sockaddr_in* addr)
{
	if (!bind(s, (sockaddr*)addr, sizeof(sockaddr_in)))
	{
		if (ntohs(addr->sin_port) != 0)
			return ntohs(addr->sin_port);
		else
		{
			// 0 means allocate a port for us, so find out what that port was
			struct sockaddr_in boundaddr;
#if _MSC_VER
			INT size = sizeof(boundaddr);
#else
			socklen_t size = sizeof(boundaddr);
#endif
			getsockname(s, (sockaddr*)(&boundaddr), &size);
			return ntohs(boundaddr.sin_port);
		}
	}
	return 0;
}

//
// Bind to next available port.
//
inline int bindnextport(SOCKET s, struct sockaddr_in* addr, int portcount, int portinc)
{
	guard(bindnextport);
	// Long explanation follows:
	//
	// Using port 0 tells the OS to allocate an unused port. But, this will
	// allocate the same port as the socket we just closed on Linux's
	// TCP/IP stack. If you get a stray packet from the last socket, the game
	// gets VERY confused. This is most obvious by hitting a passworded server
	// with the wrong password. We connect, fail to gain access, user enters a
	// password and we closesocket() and immediately hit the server again with
	// a newly bound port...and gets some extra unexpected chatter that hangs
	// the client.
	//
	// The workaround is to pick our own random ports if port zero is
	// requested. If we can't find a valid port above 1024 (below 1024 is
	// reserved for the superuser) after a couple tries, we block for three
	// seconds and then continue as normal...the blocking sucks, but
	// tends to fix things, too. Oh well.
	//
	// The MacOS X and Win32 TCP/IP stacks do not exhibit this problem.
	//
	//    --ryan.

	// Marco: Seams some windows systems also exhibit this issue???
	if (addr->sin_port == 0)
	{
		// try 4 random ports...
		for (int i = 0; i < 4; i++)
		{
			addr->sin_port = htons((appRand() & 0x7FFF) + 1024);
			int rc = trybindport(s, addr);
			if (rc)
				return rc; // got one.
		}
		addr->sin_port = 0;
		int rc = trybindport(s, addr);
		if (rc)
			return rc;
	}
	else
	{
		for (int i = 0; i < portcount; i++)
		{
			int rc = trybindport(s, addr);
			if (rc)
				return rc;
			addr->sin_port = htons(ntohs(addr->sin_port) + portinc);
		}
	}
	return 0;
	unguard;
}

inline int getlocalhostaddr( FOutputDevice& Out, in_addr &HostAddr )
{
	guard(getlocalhostaddr);
	int CanBindAll = 0;
	IpSetInt( HostAddr, INADDR_ANY );
	FString Home, HostName;
	ANSICHAR AnsiHostName[256]="localhost";
	if( gethostname( AnsiHostName, 256 ) )
		Out.Logf( TEXT("%ls: gethostname failed (%ls)"), SOCKET_API, SocketHostError() );
	HostName = appFromAnsi(AnsiHostName);
	if( Parse(appCmdLine(),TEXT("MULTIHOME="),Home) )
	{
		const TCHAR *A, *B, *C, *D;
		A=*Home;
		if
		(	(A=*Home)!=NULL
		&&	(B=appStrchr(A,'.'))!=NULL
		&&	(C=appStrchr(B+1,'.'))!=NULL
		&&	(D=appStrchr(C+1,'.'))!=NULL )
		{
			IpSetBytes( HostAddr, appAtoi(A), appAtoi(B+1), appAtoi(C+1), \
				appAtoi(D+1) );
		}
		else Out.Logf( TEXT("Invalid multihome IP address %ls"), *Home );
	}
	else
	{
		GETHOSTBYNAME_LOCK();
		HOSTENT* HostEnt = gethostbyname( AnsiHostName );
		if( HostEnt==NULL )
		{
            		Out.Logf( TEXT("gethostbyname resolving %ls failed (%ls)"),*HostName, SocketHostError() );
		}
		else if( HostEnt->h_addrtype!=PF_INET )
		{
			Out.Logf( TEXT("gethostbyname: non-Internet address (%ls)"), \
				SocketHostError() );
		}
		else
		{
			HostAddr = *(in_addr*)( *HostEnt->h_addr_list );
			if( !ParseParam(appCmdLine(),TEXT("PRIMARYNET")) )
				CanBindAll = 1;

			static UBOOL First=0;
			if( !First )
			{
				First = 1;
				debugf( NAME_Init, TEXT("%ls: I am %ls (%ls)"), SOCKET_API, *HostName, *IpString( HostAddr ) );
			}
		}
		GETHOSTBYNAME_UNLOCK();
	}
	return CanBindAll;
	unguard;
}

//
// Get local IP to bind to
//
inline in_addr getlocalbindaddr( FOutputDevice& Out )
{
	guard(getlocalbindaddr);

	in_addr BindAddr;

	// If we can bind to all addresses, return 0.0.0.0
	if( getlocalhostaddr( Out, BindAddr ) )
		IpSetInt( BindAddr, INADDR_ANY );

	return BindAddr;
	unguard;
}
/*-----------------------------------------------------------------------------
	Public includes.
-----------------------------------------------------------------------------*/

#include "IpDrvClasses.h"

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
