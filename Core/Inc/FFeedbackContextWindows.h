/*=============================================================================
	FFeedbackContextWindows.h: Unreal Windows user interface interaction.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	FFeedbackContextWindows.
-----------------------------------------------------------------------------*/

//
// Feedback context.
//
class FFeedbackContextWindows : public FFeedbackContext
{
public:
	// Variables.
	INT SlowTaskCount;
	UBOOL bCanceled;
	HWND hWndProgressBar, hWndProgressText, hWndProgressDlg, hWndCancelButton;

	// Constructor.
	FFeedbackContextWindows()
	: SlowTaskCount( 0 )
	, hWndProgressBar( 0 )
	, hWndProgressText( 0 )
	, hWndProgressDlg( 0 )
	, hWndCancelButton( 0 )
	{}
	void Serialize( const TCHAR* V, EName Event )
	{
		guard(FFeedbackContextWindows::Serialize);
		if( Event==NAME_UserPrompt && (GIsClient || GIsEditor) )
			::MessageBox( NULL, V, *LocalizeError(TEXT("Warning"),TEXT("Core")), MB_OK|MB_TASKMODAL );
		else
			debugf( NAME_Warning, TEXT("%ls"), V );
		unguard;
	}
	UBOOL YesNof( const TCHAR* Fmt, ... )
	{
		TCHAR TempStr[4096];
		GET_VARARGS( TempStr, ARRAY_COUNT(TempStr), Fmt );

		guard(FFeedbackContextWindows::YesNof);
		if( GIsClient || GIsEditor )
			return( ::MessageBox( NULL, TempStr, *LocalizeError(TEXT("Question"),TEXT("Core")), MB_YESNO|MB_TASKMODAL ) == IDYES);
		else
			return 0;
		unguard;
	}
	void BeginSlowTask( const TCHAR* Task, UBOOL StatusWindow, UBOOL Cancelable )
	{
		guard(FFeedbackContextWindows::BeginSlowTask);
#ifdef SDLBUILD
		//add SDLCode here.
		/*
		if( hWndProgressBar && hWndProgressText )
		{
			SendMessageX( (HWND)hWndProgressBar, PBM_SETRANGE, (WPARAM)0, MAKELPARAM(0, 100) );
			SendMessageLX( (HWND)hWndProgressText, WM_SETTEXT, (WPARAM)0, Task );
			UpdateWindow( (HWND)hWndProgressText );
		}
		*/
#elif QTBUILD
	//add QTCode here.
#else
		::ShowWindow( (HWND)hWndProgressDlg, SW_SHOW );
		if( hWndProgressBar && hWndProgressText )
		{
			SendMessageW( hWndProgressText, WM_SETTEXT, (WPARAM)0, (LPARAM)Task );
			SendMessageW( hWndProgressBar, PBM_SETRANGE, (WPARAM)0, MAKELPARAM(0, 100) );
			ShowWindow((HWND)hWndCancelButton, Cancelable ? SW_SHOW : SW_HIDE);

			UpdateWindow( hWndProgressDlg );
			UpdateWindow( hWndProgressText );
			UpdateWindow( hWndProgressBar );

			{	// flush all messages
				MSG mfm_msg;
				while(::PeekMessage(&mfm_msg, hWndProgressDlg, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&mfm_msg);
					DispatchMessage(&mfm_msg);
				}
			}
		}
#endif
		bCanceled = 0;
		GIsSlowTask = ++SlowTaskCount>0;
		unguard;
	}
	void EndSlowTask()
	{
		guard(FFeedbackContextWindows::EndSlowTask);
		check(SlowTaskCount>0);
		bCanceled = 0;
		GIsSlowTask = --SlowTaskCount>0;
#if 1 //U2Ed
		if( !GIsSlowTask )
			::ShowWindow( hWndProgressDlg, SW_HIDE );
#endif
		unguard;
	}
	UBOOL VARARGS StatusUpdatef( INT Numerator, INT Denominator, const TCHAR* Fmt, ... )
	{
		guard(FFeedbackContextWindows::StatusUpdatef);
		#ifdef SDLBUILD

		#elif QTBUILD

		#else
		va_list ArgPtr;
		va_start(ArgPtr, Fmt);
		FString Temp = FString::Printf(Fmt, ArgPtr);
		va_end(ArgPtr);

		if( GIsSlowTask && hWndProgressBar && hWndProgressText )
		{
			SendMessageW( hWndProgressText, WM_SETTEXT, (WPARAM)0, (LPARAM)*Temp );
			SendMessageW( hWndProgressBar, PBM_SETPOS, (WPARAM)(Denominator ? 100*Numerator/Denominator : 0), (LPARAM)0 );
			UpdateWindow( hWndProgressDlg );
			UpdateWindow( hWndProgressText );
			UpdateWindow( hWndProgressBar );

			{	// flush all messages
				MSG mfm_msg;
				while(::PeekMessage(&mfm_msg, hWndProgressDlg, 0, 0, PM_REMOVE)) {
					TranslateMessage(&mfm_msg);
					DispatchMessage(&mfm_msg);
				}
			}
		}
		#endif
		return !bCanceled;
		unguard;
	}
	void SetContext( FContextSupplier* InSupplier )
	{}
	void OnCancelProgress()
	{
		bCanceled = 1;
	}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
