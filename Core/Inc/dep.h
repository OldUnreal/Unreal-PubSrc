/*-----------------------------------------------------------------------------
	Disables DEP for the process if system policy is OptOut.
	If load library fails it is safe to assume that OS doesnt support DEP,
	so everything is probably fine.
	Returns false if game should exit (After user confirmation).
-----------------------------------------------------------------------------*/
#if INCLUDE_DEP_POLICY_CHANGE
typedef BOOL (WINAPI *GetProcessDEPPolicyFunc)( HANDLE hProcess, LPDWORD lpFlags, PBOOL lpPermanent );
typedef BOOL (WINAPI *SetProcessDEPPolicyFunc)( DWORD dwFlags );

UBOOL LaunchChangeDataExecutionPreventionPolicy()
{
	// Load Kernel32.dll.
	HINSTANCE hInstance = LoadLibraryW(TEXT("Kernel32.dll"));
	if ( !hInstance )
	{
		debugf( NAME_Warning, TEXT("Failed to load library Kernel32.dll (%#x)."), GetLastError() );
		return 1;
	}

	// Load GetProcessDEPPolicy().
	GetProcessDEPPolicyFunc GetProcessDEPPolicy = (GetProcessDEPPolicyFunc)GetProcAddress( hInstance, "GetProcessDEPPolicy" );
	if ( !GetProcessDEPPolicy )
	{
		debugf( NAME_Warning, TEXT("Failed to load GetProcessDEPPolicy() out of library Kernel32.dll (%#x)."), GetLastError() );
		return 1;
	}

	// Load SetProcessDEPPolicy().
	SetProcessDEPPolicyFunc SetProcessDEPPolicy = (SetProcessDEPPolicyFunc)GetProcAddress( hInstance, "SetProcessDEPPolicy" );
	if ( !SetProcessDEPPolicy )
	{
		debugf( NAME_Warning, TEXT("Failed to load SetProcessDEPPolicy() out of library Kernel32.dll (%#x)."), GetLastError() );
		return 1;
	}

	DWORD Flags=0;
	BOOL  Permanent=0;
	if ( !GetProcessDEPPolicy(GetCurrentProcess(),&Flags,&Permanent) )
	{
		debugf( NAME_Warning, TEXT("Failed to read process Data Execution Prevention policy (%#x)."), GetLastError() );
		return 1;
	}

	debugf( NAME_Init, TEXT("DEP: Policy is %i:%i"), Flags, Permanent );

	// Notice user if things are bad.
	/*
	if ( Flags && Permanent )
		return !GWarn->YesNof( TEXT("Data Execution Prevention is enabled for this process and cannot be disabled. If you experience UFireTexture::ConstantTick<-UTexture::Tick crashes this might be the cause. Would you like to exit the game now?") );
	*/
	if (Flags && Permanent)
		GWarn->Logf(TEXT("Data Execution Prevention is enabled for this process and cannot be disabled."));


	// Nothing to do.
	if ( Flags==0 )
		return 1;

	// Try disabling DEP for this process in case system DEP policy is OptOut.
	if ( !SetProcessDEPPolicy(0) )
		debugf( NAME_Warning, TEXT("Failed to set process Data Execution Prevention policy (%#x)."), GetLastError() );

	if ( !GetProcessDEPPolicy(GetCurrentProcess(),&Flags,&Permanent) )
	{
		debugf( NAME_Warning, TEXT("Failed to read process Data Execution Prevention policy (%#x)."), GetLastError() );
		return 1;
	}

	debugf( NAME_Init, TEXT("DEP: Policy changed to %i:%i"), Flags, Permanent);

	/*
	// Notice user if things are bad.
	if ( Flags )
		if ( GWarn->YesNof( TEXT("Data Execution Prevention is enabled for this process and disabling failed. The game is likely to crash due to known incompatibilities. Would you like to exit the game now?") ) )
			return 0;
	*/
	return 1;
}
#endif