/*=============================================================================
	FFeedbackContextAnsi.h: Unreal Ansi user interface interaction.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	FFeedbackContextAnsi.
-----------------------------------------------------------------------------*/


#ifdef SDL2BUILD
const SDL_MessageBoxButtonData buttons[] = {
    { /* .flags, .buttonid, .text */        0, 0, "yes" },
    { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "no" },
};
#endif

// Color codes for log window.
constexpr WORD LOGCOLOR_Normal = 15;
constexpr WORD LOGCOLOR_Header = 11;
constexpr WORD LOGCOLOR_SubHeader = 14;
constexpr WORD LOGCOLOR_Warning = 13;
constexpr WORD LOGCOLOR_Error = 12;
constexpr WORD LOGCOLOR_ScriptLog = 7;
constexpr WORD LOGCOLOR_ScriptStack = 6;
constexpr WORD LOGCOLOR_Init = 14;
constexpr WORD LOGCOLOR_Chat = 10;

//
// Feedback context.
//
class FFeedbackContextAnsi : public FFeedbackContext
{
public:
	// Variables.
	INT SlowTaskCount;
	INT WarningCount;
	FContextSupplier* Context;
	FOutputDevice* AuxOut;
	INT LogPrefix;
	UBOOL bPrintedProgress;
	WORD LastTextColor;

	// Local functions.
	void LocalPrint( const TCHAR* Str )
	{
#if UNICODE
		wprintf(TEXT("%ls"),Str);
#else
		printf(TEXT("%ls"),Str);
#endif
	}

	inline void SetTextColor(const WORD Mode)
	{
		guardSlow(FFeedbackContextAnsi::SetTextColor);
#ifdef WIN32
		if (LastTextColor != Mode)
		{
			LastTextColor = Mode;
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, Mode);
		}
#endif
		unguardSlow;
	}

	// Constructor.
	FFeedbackContextAnsi()
		: SlowTaskCount(0)
		, WarningCount(0)
		, Context(NULL)
		, AuxOut(NULL)
		, LogPrefix(-1)
		, bPrintedProgress(FALSE)
		, LastTextColor(0)
	{}
	void Serialize( const TCHAR* V, EName Event )
	{
		guard(FFeedbackContextAnsi::Serialize);
		TCHAR Temp[4096]=TEXT(""); //elmuerte: buffer overflow fix, changed from 1024
		if( Event==NAME_Title )
		{
#ifdef WIN32
			SetConsoleTitle(V);
#else
            wprintf(*FString::Printf(TEXT("\033]0; %ls \007"),V));
#endif
			return;
		}
		else if( Event==NAME_Heading )
		{
			SetTextColor(LOGCOLOR_Header);
			INT StrLen = appStrlen(V);
			INT PreCount = Max(32 - (StrLen >> 1), 0);
			INT i;
			TCHAR* T = Temp;
			for (i = 0; i < PreCount; ++i)
				*(T++) = '-';
			for (i = 0; i < StrLen; ++i)
				*(T++) = V[i];
			PreCount = 64 - (PreCount + StrLen);
			for (i = 0; i < PreCount; ++i)
				*(T++) = '-';
			*T = 0;
			V = Temp;
		}
		else if( Event==NAME_SubHeading )
		{
			SetTextColor(LOGCOLOR_SubHeader);
			appSprintf( Temp, TEXT("%ls..."), (TCHAR*)V );
			V = Temp;
		}
		else if( Event==NAME_Error || Event==NAME_Warning || Event==NAME_ExecWarning || Event==NAME_ScriptWarning )
		{
			SetTextColor((Event == NAME_Warning) ? LOGCOLOR_Warning : LOGCOLOR_Error);
			if( Context )
			{
				appSprintf( Temp, TEXT("%ls : %ls, %ls"), *Context->GetContext(), *FName(Event), (TCHAR*)V );
				V = Temp;
			}
			WarningCount++;
		}
		else if( Event==NAME_Progress )
		{
			SetTextColor(LOGCOLOR_Normal);
			if(bPrintedProgress)
				ClearLine();
			appSprintf( Temp, TEXT("%ls"), V );
			V = Temp;
			LocalPrint( V );
			bPrintedProgress = TRUE;
			fflush( stdout );
			return;
		}
		else if (Event == NAME_ScriptLog)
			SetTextColor(LOGCOLOR_ScriptLog);
		else if (Event == NAME_ScriptStack)
			SetTextColor(LOGCOLOR_ScriptStack);
		else if (Event == NAME_Init)
			SetTextColor(LOGCOLOR_Init);
		else if (Event == NAME_DevNet)
			SetTextColor(LOGCOLOR_Header);
		else if (Event == NAME_NetComeGo)
			SetTextColor(LOGCOLOR_Chat);
		else SetTextColor(LOGCOLOR_Normal);

		//elmuerte: log prefix, if set appent the event names
		if (LogPrefix == -1) LogPrefix = INT( ParseParam( appCmdLine(), TEXT("LOGPREFIX") ));
		if (LogPrefix == 1)
		{
			appSprintf( Temp, TEXT("%ls: %ls"), FName::SafeString(Event), V );
    		V = Temp;
		} //elmuerte: log prefix -- end
		if (bPrintedProgress)
			ClearLine();
		LocalPrint( V );
		LocalPrint( TEXT("\n") );
		if( GLog != this )
			GLog->Serialize( V, Event );
		if( AuxOut )
			AuxOut->Serialize( V, Event );
		fflush( stdout );
		unguard;
	}
	UBOOL YesNof( const TCHAR* Fmt, ... )
	{
		SetTextColor(LOGCOLOR_Normal);
		TCHAR TempStr[4096];
		GET_VARARGS( TempStr, ARRAY_COUNT(TempStr), Fmt );
		//debugf(TEXT("TempStr %ls, Fmt %ls"),TempStr);
		guard(FFeedbackContextAnsi::YesNof);
		#ifdef SDL2BUILD
            const SDL_MessageBoxData messageboxdata =
            {
            SDL_MESSAGEBOX_INFORMATION,
            NULL,
            appToAnsi(*LocalizeError(TEXT("Question"),TEXT("Core"))), /* .title */
            appToAnsi(TempStr), /* .message */
            SDL_arraysize(buttons), /* .numbuttons */
            buttons, /* .buttons */
            NULL /* .colorScheme */
            };

            INT buttonid;

            if( GIsClient || GIsEditor )
                SDL_ShowMessageBox(&messageboxdata, &buttonid);
            else
            {
				if (bPrintedProgress)
				{
					bPrintedProgress = FALSE;
					ClearLine();
				}
                LocalPrint( TempStr );
                LocalPrint( TEXT(" (Y/N): ") );
                INT Ch = getchar();
                return (Ch=='Y' || Ch=='y');
            }

            return !buttonid;
        #else
            if( (GIsClient || GIsEditor) && !ParseParam(appCmdLine(),TEXT("Silent")) )//!!
            {
				if (bPrintedProgress)
					ClearLine();
                LocalPrint( TempStr );
				SetTextColor(LOGCOLOR_SubHeader);
                LocalPrint( TEXT(" (Y/N): ") );
                INT Ch = getchar();
				getchar(); // Marco: Grab to skip CR/LF char.
                return (Ch=='Y' || Ch=='y');
            }
            else return 1;
		#endif
		unguard;
	}
	void ClearLine()
	{
		if (bPrintedProgress)
		{
			LocalPrint(TEXT("\r                                                                               "));
			LocalPrint(TEXT("\r"));
			bPrintedProgress = FALSE;
		}
	}
	void BeginSlowTask( const TCHAR* Task, UBOOL StatusWindow, UBOOL Cancelable )
	{
		guard(FFeedbackContextAnsi::BeginSlowTask);
		GIsSlowTask = ++SlowTaskCount>0;
		unguard;
	}
	void EndSlowTask()
	{
		guard(FFeedbackContextAnsi::EndSlowTask);
		check(SlowTaskCount>0);
		GIsSlowTask = --SlowTaskCount>0;
		unguard;
	}
	UBOOL VARARGS StatusUpdatef( INT Numerator, INT Denominator, const TCHAR* Fmt, ... )
	{
		guard(FFeedbackContextAnsi::StatusUpdatef);
		TCHAR TempStr[4096];
		GET_VARARGS( TempStr, ARRAY_COUNT(TempStr), Fmt );
		if( GIsSlowTask )
		{
			//!!
		}
		return 1;
		unguard;
	}
	void SetContext( FContextSupplier* InSupplier )
	{
		guard(FFeedbackContextAnsi::SetContext);
		Context = InSupplier;
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
