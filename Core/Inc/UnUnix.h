/*============================================================================
	UnUnix.h: Unix system-specific declarations.

	Revision history:
		* Created by Mike Danylchuk
		* Modified for Unreal Engine standards compliance by Brandon Reinhart
============================================================================*/

// Pathnames.
class FString;
#define PATH(s) appUnixPath( s )
FString appUnixPath( const FString& Path );

// Networking.
unsigned long appGetLocalIP( void );

// String functions.
int stricmp( const char* s, const char* t );
int strnicmp( const char* s, const char* t, int n );
char* strupr( char* s );
int _wcsicmp( const wchar_t* s, const wchar_t* t );
wchar_t* _wcsupr( wchar_t* s);
int _wtoi (__const wchar_t *__nptr);
int _wcsnicmp ( const wchar_t* s, const wchar_t* t, int n );

// Globally Unique Identifiers.
void appGetGUID( void* GUID );

// Signal Handling
void HandleSignal( int Signal );
void HandleInterrupt( int Signal );


/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/

