/*============================================================================
	UnSocket.h: Common interface for WinSock and BSD sockets.

	Revision history:
		* Created by Mike Danylchuk
============================================================================*/

/*-----------------------------------------------------------------------------
	Definitions.
-----------------------------------------------------------------------------*/

#if __WINSOCK__
	typedef INT					SOCKLEN;
	#define GCC_OPT_INT_CAST
  #ifndef MSG_NOSIGNAL
	#define MSG_NOSIGNAL 		0
	#endif
	#ifndef INVALID_SOCKET
	#define INVALID_SOCKET		0
	#endif
	#ifndef SOCKET_ERROR
	#define SOCKET_ERROR		0
	#endif
	#define UDP_ERR_PORT_UNREACH WSAECONNRESET
	#define UDP_ERR_WOULD_BLOCK WSAEWOULDBLOCK
#endif

// Provide WinSock definitions for BSD sockets.
#if __BSD_SOCKETS__
	#if __BSD_SOCKETS_TRUE_BSD__
		//typedef int socklen_t;
	#endif

	typedef uintptr_t			SOCKET;
	typedef struct hostent		HOSTENT;
	typedef in_addr				IN_ADDR;
	typedef struct sockaddr		SOCKADDR;
	typedef struct sockaddr_in	SOCKADDR_IN;
	typedef struct linger		LINGER;
	typedef struct timeval		TIMEVAL;
	#ifndef UNICODE
	typedef TCHAR*				LPSTR;
	#endif
	typedef socklen_t			SOCKLEN;

	#define INVALID_SOCKET		-1
	#define SOCKET_ERROR		-1
	//#define WSAEWOULDBLOCK		EWOULDBLOCK
	#define WSAEWOULDBLOCK		EINPROGRESS
	#define WSAENOTSOCK			ENOTSOCK
	#define WSATRY_AGAIN		TRY_AGAIN
	#define WSAHOST_NOT_FOUND	HOST_NOT_FOUND
	#define WSANO_DATA			NO_ADDRESS
	#define LPSOCKADDR			sockaddr*

	#define closesocket			close
	#define ioctlsocket			ioctl
	#define WSAGetLastError()	errno

	#define GCC_OPT_INT_CAST	(DWORD*)

	#define UDP_ERR_PORT_UNREACH ECONNREFUSED
	#define UDP_ERR_WOULD_BLOCK EAGAIN

	#if __BSD_SOCKETS_TRUE_BSD__
		#define WSAGetLastHostError()	errno
	#else
		#define WSAGetLastHostError()	h_errno
	#endif
#endif

// IP address macros.
#if __WINSOCK__
	#define IP(sin_addr,n) sin_addr.S_un.S_un_b.s_b##n
#elif __BSD_SOCKETS__
	#define IP(sin_addr,n) ((BYTE*)&sin_addr.s_addr)[n-1]
#endif

/*----------------------------------------------------------------------------
	Functions.
----------------------------------------------------------------------------*/

UBOOL InitSockets( FString& Error );
UBOOL SetNonBlocking( INT Socket );
UBOOL SetSocketReuseAddr( INT Socket, UBOOL ReUse=1 );
UBOOL SetSocketLinger( INT Socket );
const TCHAR* SocketError( INT Code=-1 );
const TCHAR* SocketHostError( INT Code=-1 );
UBOOL IpMatches(const sockaddr_in& A, const sockaddr_in& B );
UBOOL IpAddressMatches(const sockaddr_in& A, const sockaddr_in& B ); //elmuerte: jack_porter security fix
UBOOL IpAddressMatches(const in_addr& A, const in_addr& B);
void IpGetBytes( in_addr Addr, BYTE& Ip1, BYTE& Ip2, BYTE& Ip3, BYTE& Ip4 );
void IpSetBytes( in_addr& Addr, BYTE Ip1, BYTE Ip2, BYTE Ip3, BYTE Ip4 );
void IpGetInt( in_addr Addr, DWORD& Ip );
void IpSetInt( in_addr& Addr, DWORD Ip );
FString IpString( in_addr Addr, INT Port=0 );
UBOOL IpIsLAN(const in_addr& Addr); // Marco: Check if address is within typical LAN network range.

inline UBOOL operator==(const sockaddr_in& A, const sockaddr_in& B)
{
#if __WINSOCK__
	return	A.sin_addr.S_un.S_addr	== B.sin_addr.S_un.S_addr
		&&	A.sin_port				== B.sin_port
		&&	A.sin_family			== B.sin_family;
#elif __BSD_SOCKETS__
	return	A.sin_addr.s_addr		== B.sin_addr.s_addr
		&&	A.sin_port				== B.sin_port
		&&	A.sin_family			== B.sin_family;
#endif
}
inline UBOOL operator==(const in_addr& A, const in_addr& B)
{
#if __WINSOCK__
	return	A.S_un.S_addr == B.S_un.S_addr;
#elif __BSD_SOCKETS__
	return	A.s_addr == B.s_addr;
#endif
}

inline DWORD GetTypeHash(const sockaddr_in& A)
{
#if __WINSOCK__
	return ((DWORD)A.sin_addr.S_un.S_addr) | ((DWORD)A.sin_port) | ((DWORD)A.sin_family);
#elif __BSD_SOCKETS__
	return ((DWORD)A.sin_addr.s_addr) | ((DWORD)A.sin_port) | ((DWORD)A.sin_family);
#endif
}
inline DWORD GetTypeHash(const in_addr& A)
{
#if __WINSOCK__
	return (DWORD)A.S_un.S_addr;
#elif __BSD_SOCKETS__
	return (DWORD)A.s_addr;
#endif
}

inline FArchive& operator<<(FArchive& Ar, sockaddr_in& addr)
{
	return Ar;
}
inline FArchive& operator<<(FArchive& Ar, in_addr& addr)
{
	return Ar;
}

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
