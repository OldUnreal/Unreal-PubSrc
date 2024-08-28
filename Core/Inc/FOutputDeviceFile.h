/*=============================================================================
	FOutputDeviceFile.h: ANSI file output device.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "FFileManagerUnicode.h"

//
// ANSI file output device.
//

class FOutputDeviceFile : public FOutputDevice
{
public:
	FOutputDeviceFile()
	: LogAr( NULL )
	, Opened( FALSE )
	, Dead( FALSE )
	, UseTimestampLog( FALSE )
	{
#if FORCE_UTF8_LOG
		Encoding = ENCODING_UTF8_BOM;
#elif FORCE_ANSI_LOG
		Encoding = ENCODING_ANSI;
#else
		Encoding = ENCODING_UTF16LE_BOM;
#endif
	}
	~FOutputDeviceFile()
	{
		if( LogAr )
		{
//			FScopeThread<FThreadLockRecursive> LogScope(LogMutex);
//			Logf(NAME_Log, TEXT("Log file closed, %ls"), *appTimestamp());
			delete LogAr;
			LogAr = NULL;

			if (GIsCriticalError && !UseTimestampLog && GSys && GSys->AutoBackupCrashLogs)
			{
				FString NewLogName = Filename.GetFilenameOnly() + TEXT("_") + GetTimeStamp() + TEXT(".log");
				GFileManager->Move(*NewLogName, *Filename);
			}
		}
	}
	void Serialize( const TCHAR* Data, enum EName Event )
	{
		FScopeThread<FThreadLockRecursive> LogScope(LogMutex);
		if( !GIsCriticalError )
		{
			if (!ULogHandler::GrabLogLine(Event, Data))
				SerializeInternal(Data, Event);
		}
		else
		{
			try
			{
				// Ignore errors to prevent infinite-recursive exception reporting.
				SerializeInternal(Data, Event);
			}
			catch( ... )
			{
			}
		}
	}
	void Flush()
	{
		if (LogAr)
			LogAr->Flush();
	}

	FArchiveUnicodeWriterHelper* LogAr;
	FString Filename;
	FileEncoding Encoding;
private:
	FThreadLockRecursive LogMutex; // needs to be recursive because we can log from exception contexts
	UBOOL Opened, Dead, UseTimestampLog;

	inline const TCHAR* GetTimeStamp() const
	{
		static TCHAR ResultStr[1024];
		INT Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec;
		appSystemTime(Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec);
		appSnprintf(ResultStr, 1024, TEXT("[%02d-%02d-%02d_%02d-%02d-%02d]"), Year, Month, Day, Hour, Min, Sec);
		return ResultStr;
	}
	inline const TCHAR* GetTimeStampShort() const
	{
		static TCHAR ResultStr[1024];
		INT Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec;
		appSystemTime(Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec);
		appSnprintf(ResultStr, 1024, TEXT("[%d:%02d:%02d-%03d] "), Hour, Min, Sec, MSec);
		return ResultStr;
	}
	inline void SerializeInternal(const TCHAR* Data, enum EName Event)
	{
		if (!FName::SafeSuppressed(Event))
		{
			if (!LogAr && !Dead)
			{
				// Make log filename.
				if (Filename.Len() == 0)
				{
					Filename = appBaseDir();
					FString NLogName;
					if (!Parse(appCmdLine(), TEXT("LOG="), NLogName))
					{
						NLogName = appPackage();
					}
					Filename *= NLogName;

					UseTimestampLog = ParseParam(appCmdLine(), TEXT("TIMESTAMPLOG"));
					if (UseTimestampLog)
						Filename += GetTimeStamp();
				}
				// Shambler: Fix for abusing log files to overwrite stuff
				if (appStricmp(Filename.GetFileExtension(), TEXT("log")))
					Filename += TEXT(".log");

				if (GSys &&
					(ParseParam(appCmdLine(), TEXT("forceflush")) ||
					 ParseParam(appCmdLine(), TEXT("forcelogflush"))))
					GSys->NoLogBuffering = 1;

				// Open log file.
				LogAr = new FArchiveUnicodeWriterHelper(*Filename, FILEWRITE_AllowRead | FILEWRITE_Unbuffered | (Opened ? FILEWRITE_Append : 0));
				if (LogAr && LogAr->IsOpen())
				{
					Opened = TRUE;
					LogAr->SetEncoding(Encoding, true);
					TCHAR LineStr[512];
					appSnprintf(LineStr, 512, TEXT("Log file open, %ls"), *appTimestamp());
					SerializeInternal(LineStr, NAME_Log);
				}
				else Dead = TRUE;
			}
			if (LogAr && Event != NAME_Title)
			{
			    if (UseTimestampLog)
                    LogAr->WriteString(GetTimeStampShort());
				LogAr->WriteString(FName::SafeString(Event));
				LogAr->WriteString(TEXT(": "));
				LogAr->WriteString(Data);
				LogAr->WriteString(LINE_TERMINATOR);

				if (Event == NAME_Exit || Event == NAME_Critical || (GSys && GSys->NoLogBuffering))
					Flush();
			}
			if (GLogHook)
				GLogHook->Serialize(Data, Event);
		}
	}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
