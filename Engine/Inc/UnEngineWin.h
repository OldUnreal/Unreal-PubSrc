/*=============================================================================
	UnEngineWin.h: Unreal engine windows-specific code.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Tim Sweeney.
=============================================================================*/
#pragma once

/*-----------------------------------------------------------------------------
	Splash screen.
-----------------------------------------------------------------------------*/

//
// Splash screen, implemented with old-style Windows code so that it
// can be opened super-fast before initialization.
//
BOOL CALLBACK SplashDialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg==WM_DESTROY )
		PostQuitMessage(0);
	return 0;
}
static HWND    hWndSplash = NULL;
static HBITMAP hBitmap    = NULL;
static INT     BitmapX    = 0;
static INT     BitmapY    = 0;
static DWORD   ThreadId   = 0;
static HANDLE  hThread    = 0;

// stijn: Does DPI scaling for an image that we want to use for STM_SETIMAGE
void ScaleImage(HBITMAP& Bitmap, INT& ImageWidth, INT& ImageHeight, HWND InHwnd)
{
	HDC hdc = GetDC(InHwnd);
	INT LogX = GetDeviceCaps(hdc, LOGPIXELSX);
	INT LogY = GetDeviceCaps(hdc, LOGPIXELSY);

	if (LogX != 96 || LogY != 96)
	{
		ImageWidth = MulDiv(ImageWidth, LogX, 96);
		ImageHeight = MulDiv(ImageHeight, LogY, 96);
		Bitmap = reinterpret_cast<HBITMAP>(CopyImage(Bitmap, IMAGE_BITMAP, ImageWidth, ImageHeight, LR_COPYDELETEORG));
	}

	ReleaseDC(InHwnd, hdc);
}

DWORD WINAPI ThreadProc( VOID* Parm )
{
	hWndSplash = CreateDialogW(hInstance,MAKEINTRESOURCEW(IDDIALOG_Splash), NULL, reinterpret_cast<DLGPROC>(SplashDialogProc));
	if( hWndSplash )
	{
		HWND hWndLogo = GetDlgItem(hWndSplash,IDC_Logo);
		if( hWndLogo )
		{
			ScaleImage(hBitmap, BitmapX, BitmapY, hWndLogo);
			SetWindowPos(hWndSplash,HWND_TOPMOST,(GetSystemMetrics(SM_CXSCREEN)-BitmapX)/2,(GetSystemMetrics(SM_CYSCREEN)-BitmapY)/2,BitmapX,BitmapY,SWP_SHOWWINDOW);
			SetWindowPos(hWndSplash,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			SendMessageW( hWndLogo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap );
			UpdateWindow( hWndSplash );
			MSG Msg;
			while(GetMessageW(&Msg,NULL,0,0))
				DispatchMessageW(&Msg);
		}
	}
	return 0;
}
void InitSplash( const TCHAR* Filename )
{
	FWindowsBitmap Bitmap(1);
	if( Filename )
	{
		if (Bitmap.LoadFile(Filename))
		{
			hBitmap = Bitmap.GetBitmapHandle();
			BitmapX = Bitmap.SizeX;
			BitmapY = Bitmap.SizeY;
		}
		else GWarn->Logf(TEXT("Splash screen bitmap '%ls' not found!"), Filename);
	}
	hThread = CreateThread(NULL, 0, &ThreadProc, NULL, 0, &ThreadId);
}
void ExitSplash()
{
	if( ThreadId )
		PostThreadMessageW(ThreadId,WM_QUIT,0,0);
}

/*-----------------------------------------------------------------------------
	System Directories.
-----------------------------------------------------------------------------*/

TCHAR SysDir[256]=TEXT(""), WinDir[256]=TEXT(""), ThisFile[256]=TEXT("");
void InitSysDirs()
{
	GetSystemDirectory( SysDir, ARRAY_COUNT(SysDir) );
	GetWindowsDirectory( WinDir, ARRAY_COUNT(WinDir) );
	GetModuleFileName( NULL, ThisFile, ARRAY_COUNT(ThisFile) );
	if( appStrlen(ThisFile) > 4 && !appStricmp( &ThisFile[appStrlen(ThisFile) - 4], TEXT(".ICD") ) )
		appStrcpy( &ThisFile[appStrlen(ThisFile) - 4], TEXT(".EXE") ); // stijn: mem safety ok
}

/*-----------------------------------------------------------------------------
	Config wizard.
-----------------------------------------------------------------------------*/

class WConfigWizard : public WWizardDialog
{
	DECLARE_WINDOWCLASS(WConfigWizard,WWizardDialog,Startup)
	WLabel LogoStatic;
	FWindowsBitmap LogoBitmap;
	UBOOL Cancel;
	FString Title;
	WConfigWizard()
	: LogoStatic(this,IDC_Logo)
	, Cancel(0)
	{
		InitSysDirs();
	}
	void OnInitDialog()
	{
		guard(WStartupWizard::OnInitDialog);
		WWizardDialog::OnInitDialog();
		SendMessageW( *this, WM_SETICON, ICON_BIG, (WPARAM)LoadIconW(hInstance,MAKEINTRESOURCEW(IDICON_Mainframe)) );
		LogoBitmap.LoadFile( TEXT("..\\Help\\SetupLogo.bmp") );
		SendMessageW( LogoStatic, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)LogoBitmap.GetBitmapHandle() );
		SetText( *Title );
		SetForegroundWindow( hWnd );
		unguard;
	}
};

class WConfigPageFirstTime : public WWizardPage
{
	DECLARE_WINDOWCLASS(WConfigPageFirstTime,WWizardPage,Startup)
	WConfigWizard* Owner;
	WConfigPageFirstTime( WConfigWizard* InOwner )
	: WWizardPage( TEXT("ConfigPageFirstTime"), IDDIALOG_ConfigPageFirstTime, InOwner )
	, Owner(InOwner)
	{}
	const FString GetNextText()
	{
		return *LocalizeGeneral(TEXT("Run"),TEXT("Startup"));
	}
	WWizardPage* GetNext()
	{
		Owner->EndDialog(1);
		return NULL;
	}
};

class WConfigPageSafeOptions : public WWizardPage
{
	DECLARE_WINDOWCLASS(WConfigPageSafeOptions,WWizardPage,Startup)
	WConfigWizard* Owner;
	WButton NoSoundButton, No3DSoundButton, No3DVideoButton, WindowButton, ResButton, ResetConfigButton, NoProcessorButton, NoJoyButton;
	WConfigPageSafeOptions( WConfigWizard* InOwner )
	: WWizardPage		( TEXT("ConfigPageSafeOptions"), IDDIALOG_ConfigPageSafeOptions, InOwner )
	, Owner				(InOwner)
	, NoSoundButton		(this,IDC_NoSound)
	, No3DSoundButton	(this,IDC_No3DSound)
	, No3DVideoButton	(this,IDC_No3dVideo)
	, WindowButton		(this,IDC_Window)
	, ResButton			(this,IDC_Res)
	, ResetConfigButton	(this,IDC_ResetConfig)
	, NoProcessorButton	(this,IDC_NoProcessor)
	, NoJoyButton		(this,IDC_NoJoy)
	{}
	void OnInitDialog()
	{
		WWizardPage::OnInitDialog();
		SendMessageW( NoSoundButton,     BM_SETCHECK, 1, 0 );
		SendMessageW( No3DSoundButton,   BM_SETCHECK, 1, 0 );
		SendMessageW( No3DVideoButton,   BM_SETCHECK, 1, 0 );
		SendMessageW( WindowButton,      BM_SETCHECK, 1, 0 );
		SendMessageW( ResButton,         BM_SETCHECK, 1, 0 );
		SendMessageW( ResetConfigButton, BM_SETCHECK, 0, 0 );
		SendMessageW( NoProcessorButton, BM_SETCHECK, 1, 0 );
		SendMessageW( NoJoyButton,       BM_SETCHECK, 1, 0 );
	}
	const FString GetNextText()
	{
		return *LocalizeGeneral(TEXT("Run"),TEXT("Startup"));
	}
	WWizardPage* GetNext()
	{
		FString CmdLine;
		if( SendMessageW(NoSoundButton,BM_GETCHECK,0,0)==BST_CHECKED )
			CmdLine+=TEXT(" -nosound");
		if( SendMessageW(No3DSoundButton,BM_GETCHECK,0,0)==BST_CHECKED )
			CmdLine+=TEXT(" -no3dsound");
		if( SendMessageW(No3DSoundButton,BM_GETCHECK,0,0)==BST_CHECKED )
			CmdLine+=TEXT(" -nohard");
		if( SendMessageW(No3DSoundButton,BM_GETCHECK,0,0)==BST_CHECKED )
			CmdLine+=TEXT(" -nohard -noddraw");
		if( SendMessageW(No3DSoundButton,BM_GETCHECK,0,0)==BST_CHECKED )
			CmdLine+=TEXT(" -defaultres");
		if( SendMessageW(NoProcessorButton,BM_GETCHECK,0,0)==BST_CHECKED )
			CmdLine+=TEXT(" -nommx -nokni -nok6");
		if( SendMessageW(NoJoyButton,BM_GETCHECK,0,0)==BST_CHECKED )
			CmdLine+=TEXT(" -nojoy");
		if( SendMessageW(ResetConfigButton,BM_GETCHECK,0,0)==BST_CHECKED )
			GFileManager->Delete( *(FString(appPackage())+TEXT(".ini")) );
		ShellExecuteW(NULL, TEXT("open"), ThisFile, *CmdLine, appBaseDir(), SW_SHOWNORMAL );
		Owner->EndDialog(0);
		return NULL;
	}
};

class WConfigPageDetail : public WWizardPage
{
	DECLARE_WINDOWCLASS(WConfigPageDetail,WWizardPage,Startup)
	WConfigWizard* Owner;
	WEdit DetailEdit;
	WConfigPageDetail( WConfigWizard* InOwner )
	: WWizardPage( TEXT("ConfigPageDetail"), IDDIALOG_ConfigPageDetail, InOwner )
	, Owner(InOwner)
	, DetailEdit(this,IDC_DetailEdit)
	{}
	void OnInitDialog()
	{
		WWizardPage::OnInitDialog();
		FString Info;

		INT DescFlags=0;
		FString Driver = GConfig->GetStr(TEXT("Engine.Engine"),TEXT("GameRenderDevice"));
		GConfig->GetInt(*Driver,TEXT("DescFlags"),DescFlags);

		// Frame rate dependent LOD.
		if( Driver==TEXT("SoftDrv.SoftwareRenderDevice") || 280.0*1000.0*1000.0*GSecondsPerCycle>1.f )
		{
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("MinDesiredFrameRate"), TEXT("20") );
		}
		else if( Driver==TEXT("D3DDrv.D3DRenderDevice") )
		{
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("MinDesiredFrameRate"), TEXT("28") );
		}
		else if( Driver==TEXT("D3D8Drv.D3D8RenderDevice") )
		{
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("MinDesiredFrameRate"), TEXT("28") );
		}
		else if( Driver==TEXT("OpenGLDrv.OpenGLRenderDevice") )
		{
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("MinDesiredFrameRate"), TEXT("28") );
		}
		else if( Driver==TEXT("XOpenGLDrv.XOpenGLRenderDevice") )
		{
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("MinDesiredFrameRate"), TEXT("28") );
		}

		// Sound quality.
		if( !GIsMMX || GPhysicalMemory <= 32*1024*1024 )
		{
			Info = Info + LocalizeGeneral(TEXT("SoundLow"),TEXT("Startup")) + TEXT("\r\n");
			GConfig->SetString( TEXT("Galaxy.GalaxyAudioSubsystem"), TEXT("UseReverb"),       TEXT("False") );
			GConfig->SetString( TEXT("Galaxy.GalaxyAudioSubsystem"), TEXT("OutputRate"),      TEXT("11025Hz") );
			GConfig->SetString( TEXT("Galaxy.GalaxyAudioSubsystem"), TEXT("UseSpatial"),      TEXT("False") );
			GConfig->SetString( TEXT("Galaxy.GalaxyAudioSubsystem"), TEXT("UseFilter"),       TEXT("False") );
			GConfig->SetString( TEXT("Galaxy.GalaxyAudioSubsystem"), TEXT("EffectsChannels"), TEXT("8") );
			GConfig->SetBool( TEXT("Galaxy.GalaxyAudioSubsystem"), TEXT("LowSoundQuality"), 1 );
		}
		else
		{
			Info = Info + LocalizeGeneral(TEXT("SoundHigh"),TEXT("Startup")) + TEXT("\r\n");
		}

		// Skins.
		if( (GPhysicalMemory < 96*1024*1024) || (DescFlags&RDDESCF_LowDetailSkins) )
		{
			Info = Info + LocalizeGeneral(TEXT("SkinsLow"),TEXT("Startup")) + TEXT("\r\n");
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("SkinDetail"), TEXT("Medium") );
		}
		else
		{
			Info = Info + LocalizeGeneral(TEXT("SkinsHigh"),TEXT("Startup")) + TEXT("\r\n");
		}

		// World.
		if( (GPhysicalMemory < 64*1024*1024) || (DescFlags&RDDESCF_LowDetailWorld) )
		{
			Info = Info + LocalizeGeneral(TEXT("WorldLow"),TEXT("Startup")) + TEXT("\r\n");
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("TextureDetail"), TEXT("Medium") );
		}
		else
		{
			Info = Info + LocalizeGeneral(TEXT("WorldHigh"),TEXT("Startup")) + TEXT("\r\n");
		}

		// Resolution.
		if( (!GIsMMX) && Driver==TEXT("SoftDrv.SoftwareRenderDevice") )
		{
			Info = Info + LocalizeGeneral(TEXT("ResLow"),TEXT("Startup")) + TEXT("\r\n");
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("WindowedViewportX"),  TEXT("320") );
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("WindowedViewportY"),  TEXT("240") );
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("WindowedColorBits"),  TEXT("16") );
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("FullscreenViewportX"), TEXT("320") );
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("FullscreenViewportY"), TEXT("240") );
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("FullscreenColorBits"), TEXT("16") );
		}
		else if( Driver==TEXT("SoftDrv.SoftwareRenderDevice") || (DescFlags&RDDESCF_LowDetailWorld) )
		{
			Info = Info + LocalizeGeneral(TEXT("ResLow"),TEXT("Startup")) + TEXT("\r\n");
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("WindowedViewportX"),  TEXT("512") );
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("WindowedViewportY"),  TEXT("384") );
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("WindowedColorBits"),  TEXT("16") );
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("FullscreenViewportX"), TEXT("512") );
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("FullscreenViewportY"), TEXT("384") );
			GConfig->SetString( TEXT("WinDrv.WindowsClient"), TEXT("FullscreenColorBits"), TEXT("16") );
		}
		else
		{
			Info = Info + LocalizeGeneral(TEXT("ResHigh"),TEXT("Startup")) + TEXT("\r\n");
		}
		DetailEdit.SetText(*Info);
	}
	WWizardPage* GetNext()
	{
		return new WConfigPageFirstTime(Owner);
	}
};

class WConfigPageDriver : public WWizardPage
{
	DECLARE_WINDOWCLASS(WConfigPageDriver,WWizardPage,Startup)
	WConfigWizard* Owner;
	WUrlButton WebButton;
	WLabel Card;
	WConfigPageDriver( WConfigWizard* InOwner )
	: WWizardPage( TEXT("ConfigPageDriver"), IDDIALOG_ConfigPageDriver, InOwner )
	, Owner(InOwner)
	, WebButton(this,*LocalizeGeneral(TEXT("Direct3DWebPage"),TEXT("Startup")),IDC_WebButton)
	, Card(this,IDC_Card)
	{}
	void OnInitDialog()
	{
		WWizardPage::OnInitDialog();
		FString CardName=GConfig->GetStr(TEXT("D3DDrv.D3DRenderDevice"),TEXT("Description"));
		/*
		if( CardName==TEXT("") )
		FString CardName=GConfig->GetStr(TEXT("D3D8Drv.D3D8RenderDevice"),TEXT("Description")); // add maybe later for newer drivers, but not really needed (yet), the d3d card check does fine for the other renderers as well.
		if( CardName==TEXT("") )
		FString CardName=GConfig->GetStr(TEXT("OpenGLDrv.OpenGLRenderDevice"),TEXT("Description"));
		*/
		if( CardName!=TEXT("") )
			Card.SetText(*CardName);
	}
	WWizardPage* GetNext()
	{
		return new WConfigPageDetail(Owner);
	}
};

class WConfigPageSound : public WWizardPage
{
	DECLARE_WINDOWCLASS(WConfigPageSound,WWizardPage,Startup)
	WConfigWizard* Owner;
	WListBox SoundList;
	WButton ShowCompatible;
	WLabel SoundNote;
	INT First;
	TArray<FRegistryObjectInfo> Classes;
	WConfigPageSound( WConfigWizard* InOwner )
	: WWizardPage( TEXT("ConfigPageSound"), IDDIALOG_ConfigPageSound, InOwner )
	, Owner(InOwner)
	, SoundList(this,IDC_SoundList)
	, ShowCompatible(this,IDC_Compatible,FDelegate(this,(TDelegate)&WConfigPageSound::RefreshList))
	, SoundNote(this,IDC_SoundNote)
	, First(0)
	{}
	void RefreshList()
	{
		SoundList.Empty();
		FString Default;
		Classes.Empty();
		UObject::GetRegistryObjects( Classes, UClass::StaticClass(), UAudioSubsystem::StaticClass(), 0 );
		for( TArray<FRegistryObjectInfo>::TIterator It(Classes); It; ++It )
		{
			FString Path=It->Object, Left, Right, Temp;
			if( Path.Divide(TEXT("."),Left,Right) )
			{
				SoundList.AddString( *(Temp=Localize(*Right,TEXT("ClassCaption"),*Left)) );

				if (!Default.Len())
					Default=Temp;
			}
		}
		if( Default!=TEXT("") )
			SoundList.SetCurrent(SoundList.FindStringChecked(*Default),1);
		CurrentChange();
	}
	void CurrentChange()
	{
		debugf(TEXT("CurrentDriver %ls"),*CurrentDriver());
		SoundNote.SetText(*Localize(TEXT("Descriptions"),*CurrentDriver(),TEXT("Startup"),NULL,1));
	}
	void OnPaint()
	{
		if( !First++ )
		{
			UpdateWindow( *this );
			GConfig->Flush( 1 );
			RefreshList();
		}
	}
	void OnCurrent()
	{
		guard(WFilerPageInstallProgress::OnCurrent);
		unguard;
	}
	void OnInitDialog()
	{
		WWizardPage::OnInitDialog();
		SendMessageW(ShowCompatible,BM_SETCHECK,BST_CHECKED,0);
		SoundList.SelectionChangeDelegate = FDelegate(this,(TDelegate)& WConfigPageSound::CurrentChange);
		SoundList.DoubleClickDelegate = FDelegate(Owner,(TDelegate)& WWizardDialog::OnNext);
		SoundList.AddString(*LocalizeGeneral(TEXT("Detecting"),TEXT("Startup")));
	}
	FString CurrentDriver()
	{
		if( SoundList.GetCurrent()>=0 )
		{
			FString Name = SoundList.GetString(SoundList.GetCurrent());
			for( TArray<FRegistryObjectInfo>::TIterator It(Classes); It; ++It )
			{
				FString Path=It->Object, Left, Right, Temp;
				if (Path.Divide(TEXT("."), Left, Right))
					if( Name==Localize(*Right,TEXT("ClassCaption"),*Left) )
						return Path;
			}
		}
		return TEXT("");
	}
	WWizardPage* GetNext()
	{
		if( CurrentDriver()!=TEXT("") )
			GConfig->SetString(TEXT("Engine.Engine"),TEXT("AudioDevice"),*CurrentDriver());
		if( CurrentDriver()==TEXT("ALAudio.ALAudioSubsystem") )
			return new WConfigPageDriver(Owner);
		if( CurrentDriver()==TEXT("Galaxy.GalaxyAudioSubsystem") )
			return new WConfigPageDriver(Owner);
		if( CurrentDriver()==TEXT("FMODAudioDrv.FMODAudioDevice") )
			return new WConfigPageDriver(Owner);
		if (CurrentDriver() == TEXT("SwFMOD.SwFMOD"))
			return new WConfigPageDriver(Owner);
		else
			return new WConfigPageDetail(Owner);
	}
};

class WConfigPageRenderer : public WWizardPage
{
	DECLARE_WINDOWCLASS(WConfigPageRenderer,WWizardPage,Startup)
	WConfigWizard* Owner;
	WListBox RenderList;
	WButton ShowCompatible, ShowAll;
	WLabel RenderNote;
	INT First;
	TArray<FRegistryObjectInfo> Classes;
	WConfigPageRenderer( WConfigWizard* InOwner )
	: WWizardPage( TEXT("ConfigPageRenderer"), IDDIALOG_ConfigPageRenderer, InOwner )
	, Owner(InOwner)
	, RenderList(this,IDC_RenderList)
	, ShowCompatible(this,IDC_Compatible,FDelegate(this,(TDelegate)&WConfigPageRenderer::RefreshList))
	, ShowAll(this,IDC_All,FDelegate(this,(TDelegate)&WConfigPageRenderer::RefreshList))
	, RenderNote(this,IDC_RenderNote)
	, First(0)
	{}
	void RefreshList()
	{
		RenderList.Empty();
		INT All=(SendMessageW(ShowAll,BM_GETCHECK,0,0)==BST_CHECKED), BestPriority=0;
		FString Default;
		Classes.Empty();
		FString CurrentSetting;
		GConfig->GetString(TEXT("Engine.Engine"), TEXT("GameRenderDevice"), CurrentSetting);
		UObject::GetRegistryObjects( Classes, UClass::StaticClass(), URenderDevice::StaticClass(), 0 );
		for( TArray<FRegistryObjectInfo>::TIterator It(Classes); It; ++It )
		{
			FString Path=It->Object, Left, Right, Temp;
			if (Path.Divide(TEXT("."), Left, Right))
			{
				INT DoShow=All, Priority=0;
				INT DescFlags=0;
				GConfig->GetInt(*Path,TEXT("DescFlags"),DescFlags);
				if
				(	It->Autodetect!=TEXT("")
				&& (GFileManager->FileSize(*FString::Printf(TEXT("%ls\\%ls"), SysDir, *It->Autodetect))>=0
				||  GFileManager->FileSize(*FString::Printf(TEXT("%ls\\%ls"), WinDir, *It->Autodetect))>=0) )
					DoShow = Priority = 3;
				else if( DescFlags & RDDESCF_Certified )
					DoShow = Priority = 2;
				else if( Path==TEXT("SoftDrv.SoftwareRenderDevice") )
					DoShow = Priority = 1;
				if( DoShow )
				{
					if (Path == CurrentSetting)
						Priority = 4;
					RenderList.AddString( *(Temp=Localize(*Right,TEXT("ClassCaption"),*Left)) );
					if( Priority>=BestPriority )
						{Default=Temp; BestPriority=Priority;}
				}
			}
		}
		if( Default!=TEXT("") )
			RenderList.SetCurrent(RenderList.FindStringChecked(*Default),1);
		CurrentChange();
	}
	void CurrentChange()
	{
		RenderNote.SetText(*Localize(TEXT("Descriptions"),*CurrentDriver(),TEXT("Startup"),NULL,1));
	}
	void OnPaint()
	{
		if( !First++ )
		{
			UpdateWindow( *this );
			GConfig->Flush( 1 );
			if( !ParseParam(appCmdLine(),TEXT("nodetect")) )
			{
				GFileManager->Delete(TEXT("Detected.ini"));
				ShellExecuteW( NULL, TEXT("open"), ThisFile, TEXT("testrendev=D3DDrv.D3DRenderDevice log=Detected.log"), appBaseDir(), SW_SHOWNORMAL );
				for( INT MSec=80000; MSec>0 && GFileManager->FileSize(TEXT("Detected.ini"))<0; MSec-=100 )
					Sleep(100);
			}
			RefreshList();
		}
	}
	void OnCurrent()
	{
		guard(WFilerPageInstallProgress::OnCurrent);
		unguard;
	}
	void OnInitDialog()
	{
		WWizardPage::OnInitDialog();
		SendMessageW(ShowCompatible,BM_SETCHECK,BST_CHECKED,0);
		RenderList.SelectionChangeDelegate = FDelegate(this,(TDelegate)&WConfigPageRenderer::CurrentChange);
		RenderList.DoubleClickDelegate = FDelegate(Owner,(TDelegate)&WWizardDialog::OnNext);
		RenderList.AddString( *LocalizeGeneral(TEXT("Detecting"),TEXT("Startup")) );
	}
	FString CurrentDriver()
	{
		if( RenderList.GetCurrent()>=0 )
		{
			FString Name = RenderList.GetString(RenderList.GetCurrent());
			for( TArray<FRegistryObjectInfo>::TIterator It(Classes); It; ++It )
			{
				FString Path=It->Object, Left, Right, Temp;
				if (Path.Divide(TEXT("."), Left, Right))
					if( Name==Localize(*Right,TEXT("ClassCaption"),*Left) )
						return Path;
			}
		}
		return TEXT("");
	}
	WWizardPage* GetNext()
	{
		if( CurrentDriver()!=TEXT("") )
			GConfig->SetString(TEXT("Engine.Engine"),TEXT("GameRenderDevice"),*CurrentDriver());
		return new WConfigPageSound(Owner);
	}
};

class WConfigPageSafeMode : public WWizardPage
{
	DECLARE_WINDOWCLASS(WConfigPageSafeMode,WWizardPage,Startup)
	WConfigWizard* Owner;
	WCoolButton RunButton, VideoButton, SoundButton, SafeModeButton, WebButton;
	WConfigPageSafeMode( WConfigWizard* InOwner )
	: WWizardPage    ( TEXT("ConfigPageSafeMode"), IDDIALOG_ConfigPageSafeMode, InOwner )
	, RunButton      ( this, IDC_Run,      FDelegate(this,(TDelegate)&WConfigPageSafeMode::OnRun) )
	, VideoButton    ( this, IDC_Video,    FDelegate(this,(TDelegate)&WConfigPageSafeMode::OnVideo) )
	, SoundButton    ( this, IDC_Audio,    FDelegate(this,(TDelegate)&WConfigPageSafeMode::OnSound) )
	, SafeModeButton ( this, IDC_SafeMode, FDelegate(this,(TDelegate)&WConfigPageSafeMode::OnSafeMode) )
	, WebButton      ( this, IDC_Web,      FDelegate(this,(TDelegate)&WConfigPageSafeMode::OnWeb) )
	, Owner          (InOwner)
	{}
	void OnRun()
	{
		Owner->EndDialog(1);
	}
	void OnVideo()
	{
		Owner->Advance( new WConfigPageRenderer(Owner) );
	}
	void OnSound()
	{
		Owner->Advance( new WConfigPageSound(Owner) );
	}
	void OnSafeMode()
	{
		Owner->Advance( new WConfigPageSafeOptions(Owner) );
	}
	void OnWeb()
	{
		ShellExecuteW( *this, TEXT("open"), *LocalizeGeneral(TEXT("WebPage"),TEXT("Startup")), TEXT(""), appBaseDir(), SW_SHOWNORMAL );
		Owner->EndDialog(0);
	}
	const FString GetNextText()
	{
		return TEXT("");
	}
};
/*-----------------------------------------------------------------------------
	Exec hook.
-----------------------------------------------------------------------------*/

// FExecHook.
class FExecHook : public FExec, public FNotifyHook
{
private:
	WConfigProperties* Preferences;
	void NotifyDestroy( void* Src )
	{
		if( Src==Preferences )
			Preferences = NULL;
	}
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar )
	{
		guard(FExecHook::Exec);
		if( ParseCommand(&Cmd,TEXT("ShowLog")) )
		{
			if( GLogWindow )
			{
				GLogWindow->Show(1);
				SetFocus( *GLogWindow );
				GLogWindow->Display.ScrollCaret();
			}
			return 1;
		}
		else if( ParseCommand(&Cmd,TEXT("TakeFocus")) )
		{
			TObjectIterator<UEngine> EngineIt;
			if
			(	EngineIt
			&&	EngineIt->Client
			&&	EngineIt->Client->Viewports.Num() )
				SetForegroundWindow( (HWND)EngineIt->Client->Viewports(0)->GetWindow() );
			return 1;
		}
		else if( ParseCommand(&Cmd,TEXT("EditActor")) )
		{
			// Shambler: Prevent use of EditActor online
			if (GAntiTCC || !UBitmap::__Client || !UBitmap::__Client->Viewports.Num())
			{
				Ar.Logf(TEXT("You cannot use the EditActor command while playing online"));
				return 1;
			}
			if (!UBitmap::__Client->Viewports(0)->Actor) // Not in a level atm?
				return 1;

			// Shambler: Consider any offline use of EditActor a use of temporary console commands
			GTCCUsed = 1;

			UClass* Class;
			FName ActorName;
			const FVector& CamPos = UBitmap::__Client->Viewports(0)->Actor->CalcCameraLocation;
			ULevel* CurrentLevel = UBitmap::__Client->Viewports(0)->Actor->XLevel;

			AActor* Found = NULL;

			if( ParseObject<UClass>( Cmd, TEXT("Class="), Class, ANY_PACKAGE ) )
			{
				FLOAT MinDist = 0.f, Dist = 0.f;
				AActor* A;
				for (INT i = (CurrentLevel->Actors.Num() - 1); i >= 0; --i)
				{
					A = CurrentLevel->Actors(i);
					if (A && !A->IsPendingKill() && A->IsA(Class))
					{
						Dist = (A->Location - CamPos).SizeSquared();
						if (!Found || MinDist > Dist)
						{
							Found = A;
							MinDist = Dist;
						}
					}
				}
			}
			else if( Parse( Cmd, TEXT("Name="), ActorName ) )
			{
				// look for actor by name
				AActor* A;
				for (INT i = (CurrentLevel->Actors.Num() - 1); i >= 0; --i)
				{
					A = CurrentLevel->Actors(i);
					if (A && !A->IsPendingKill() && A->GetFName()== ActorName)
					{
						Found = A;
						break;
					}
				}
			}

			if( Found )
			{
				WObjectProperties* P = new WObjectProperties( TEXT("EditActor"), 0, TEXT(""), NULL, 1 );
				P->OpenWindow( (HWND)UBitmap::__Client->Viewports(0)->GetWindow() );
				P->Root.SetObjects( (UObject**)&Found, 1 );
				P->Show(1);
			}
			else Ar.Logf( TEXT("Bad or missing class or name (EditActor Class=ABC/Name=ABC)") );
			return 1;
		}
		else if( ParseCommand(&Cmd,TEXT("EditObject")) )
		{
			if (GAntiTCC || !UBitmap::__Client || !UBitmap::__Client->Viewports.Num())
			{
				Ar.Logf(TEXT("You cannot use the EditObject command while playing online"));
				return 1;
			}

			// Smirftsch: Consider any offline use of EditObject a use of temporary console commands
			GTCCUsed = 1;

			UClass* Class;
			FName Name;
			if( (ParseObject<UClass>( Cmd, TEXT("Class="), Class, ANY_PACKAGE ) || Parse( Cmd, TEXT("Name="), Name ) ) )
			{
				UObject* Found   = NULL;
				for( FObjectIterator It; It; ++It )
				{
					if (It->IsPendingKill())
						continue;

					// !! give priority to name  // !! give priority to name
					if (!Found && It->GetClass() == Class)
						Found = *It;

					if (It->GetFName() == Name)
						Found = *It; // !! give priority to name
				}
				if( Found )
				{
					WObjectProperties* P = new WObjectProperties( TEXT("EditObject"), 0, TEXT(""), NULL, 1 );
					P->OpenWindow( (HWND)UBitmap::__Client->Viewports(0)->GetWindow() );
					P->Root.SetObjects( (UObject**)&Found, 1 );
					P->Show(1);
				}
				else Ar.Logf( TEXT("Object not found") );
			}
			else Ar.Logf( TEXT("Missing class or name (EditObject Class=ABC Name=ABC)") );
			return 1;
		}

		else if( ParseCommand(&Cmd,TEXT("HideLog")) )
		{
			if( GLogWindow )
				GLogWindow->Show(0);
			return 1;
		}
		else if( ParseCommand(&Cmd,TEXT("Preferences")) && !GIsClient )
		{
			if( !Preferences )
			{
				Preferences = new WConfigProperties(TEXT("Preferences"), *LocalizeGeneral(TEXT("AdvancedOptionsTitle"), TEXT("Window"), TEXT("int")), *LocalizeGeneral(TEXT("AdvancedOptionsTitle"), TEXT("Window")));
				Preferences->SetNotifyHook( this );
				Preferences->OpenWindow( GLogWindow ? GLogWindow->hWnd : NULL );
				Preferences->ForceRefresh();
			}
			Preferences->Show(1);
			SetFocus( *Preferences );
			return 1;
		}
		else return 0;
		unguard;
	}
public:
	FExecHook()
	: Preferences( NULL )
	{}
};

/*-----------------------------------------------------------------------------
	Startup and shutdown.
-----------------------------------------------------------------------------*/

//
// Initialize.
//
#ifndef _EDITOR_
static UEngine* InitEngine()
{
	guard(InitEngine);
	FTime LoadTime = appSeconds();

	// Set exec hook.
	static FExecHook GLocalHook;
	GExec = &GLocalHook;

	// Create mutex so installer knows we're running.
	CreateMutexW( NULL, 0, TEXT("UnrealIsRunning"));
	UBOOL AlreadyRunning = (GetLastError()==ERROR_ALREADY_EXISTS);

	// First-run menu.
	INT FirstRun=0;
	GConfig->GetInt( TEXT("FirstRun"), TEXT("FirstRun"), FirstRun );
	if( ParseParam(appCmdLine(),TEXT("FirstRun")) )
		FirstRun=0;
#if 0 // Marco: No longer in use since 227j
	if( FirstRun<ENGINE_VERSION )
	{
		// Migrate savegames.
		TArray<FString> Saves = GFileManager->FindFiles( TEXT("..\\Save\\*.usa"), 1, 0 );
		for( TArray<FString>::TIterator It(Saves); It; ++It )
		{
			INT Pos = appAtoi(**It+4);
			FString Section = TEXT("UnrealShare.UnrealSlotMenu");
			FString Key     = FString::Printf(TEXT("SlotNames[%i]"),Pos);
			if( appStricmp(*GConfig->GetStr(*Section,*Key,TEXT("user")),TEXT(""))==0 )
				GConfig->SetString(*Section,*Key,TEXT("Saved game"),TEXT("user"));
		}
	}
#endif

	// Commandline (for mplayer/heat)
	FString Command;
	if( Parse(appCmdLine(),TEXT("consolecommand="), Command) )
	{
		debugf(TEXT("Executing console command %ls"),*Command);
		GExec->Exec( *Command, *GLog );
		return NULL;
	}

	// Test render device.
	FString Device;
	if( Parse(appCmdLine(),TEXT("testrendev="),Device) )
	{
		debugf(TEXT("Detecting %ls"),*Device);
		try
		{
			UClass* Cls = LoadClass<URenderDevice>( NULL, *Device, NULL, 0, NULL );
			GConfig->SetInt(*Device,TEXT("DescFlags"),RDDESCF_Incompatible);
			GConfig->Flush(0);
			if( Cls )
			{
				URenderDevice* RenDev = ConstructObject<URenderDevice>(Cls);
				if( RenDev )
				{
					if( RenDev->Init(NULL,0,0,0,0) )
					{
						debugf(TEXT("Successfully detected %ls"),*Device);
					}
					else
					{
						delete RenDev;
						RenDev = NULL;
					}
				}
			}
		} catch( ... ) {}
		FArchive* Ar = GFileManager->CreateFileWriter(TEXT("Detected.ini"),0);
		if( Ar )
			delete Ar;
		return NULL;
	}

	UBOOL bWasRunning = FALSE;
	UBOOL bCrashRecover = FALSE;

	// Config UI.
	guard(ConfigUI);
	if( !GIsEditor && GIsClient )
	{
		WConfigWizard D;
		WWizardPage* Page = NULL;
		if( ParseParam(appCmdLine(),TEXT("safe")) || appStrfind(appCmdLine(),TEXT("readini")) )
			{Page = new WConfigPageSafeMode(&D); D.Title=LocalizeGeneral(TEXT("SafeMode"),TEXT("Startup"));}
		else if( FirstRun<ENGINE_VERSION )
			{Page = new WConfigPageRenderer(&D); D.Title=LocalizeGeneral(TEXT("FirstTime"),TEXT("Startup"));}
		else if( ParseParam(appCmdLine(),TEXT("changevideo")) )
			{Page = new WConfigPageRenderer(&D); D.Title=LocalizeGeneral(TEXT("Video"),TEXT("Startup"));}
		else if( ParseParam(appCmdLine(),TEXT("changesound")) )
			{Page = new WConfigPageSound(&D); D.Title=LocalizeGeneral(TEXT("Audio"),TEXT("Startup"));}
		else if( !AlreadyRunning && GFileManager->FileSize(GRunningFile)>=0 )
		{
			FString Temp;
			if(GSys->CrashAutoRestart && GConfig->GetString(TEXT("URL"), TEXT("Map"), Temp, GRunningFile))
				bCrashRecover = TRUE;
			else
			{
				Page = new WConfigPageSafeMode(&D);
				D.Title = LocalizeGeneral(TEXT("RecoveryMode"), TEXT("Startup"));
			}
			bWasRunning = TRUE;
		}
		if( Page )
		{
			ExitSplash();
			D.Advance( Page );
			if( !D.DoModal() )
				return NULL;
			if (bWasRunning)
			{
				FString LastMap;
				if (GConfig->GetString(TEXT("URL"), TEXT("Map"), LastMap, GRunningFile) && GWarn->YesNof(TEXT("Continue playing from where you left off at before crash?\nMap: %ls"), *LastMap))
					bCrashRecover = TRUE;
			}
			InitSplash(NULL);
		}
	}
	unguard;

	if (!GIsEditor && GIsClient && !bWasRunning)
	{
		// Create is-running semaphore file.
		FArchive* Ar = GFileManager->CreateFileWriter(GRunningFile, 0);
		if (Ar)
			delete Ar;
	}

	// Update first-run.
	if (FirstRun < ENGINE_VERSION)
		GConfig->SetInt(TEXT("FirstRun"), TEXT("FirstRun"), ENGINE_VERSION);

#if 0 // Marco: why?
	// Cd check.
	FString CdPath;
	GConfig->GetString( TEXT("Engine.Engine"), TEXT("CdPath"), CdPath );
	if
	(	CdPath!=TEXT("")
	&&	GFileManager->FileSize(TEXT("..\\Textures\\Palettes.utx"))<=0 )//oldver
	{
		FString Check = CdPath * TEXT("Textures\\Palettes.utx");
		while( !GIsEditor && GFileManager->FileSize(*Check)<=0 )
		{
			if( MessageBox
			(
				NULL,
				*LocalizeGeneral(TEXT("InsertCdText"),TEXT("Window")),
				*LocalizeGeneral(TEXT("InsertCdTitle"),TEXT("Window")),
				MB_TASKMODAL|MB_OKCANCEL
			)==IDCANCEL )
			{
				GIsCriticalError = 1;
				ExitProcess( 0 );
			}
		}
	}
#endif

#if ENGINE_VERSION<230
	// Display the damn story to appease the German censors.
	UBOOL CanModifyGore=1;
	GConfig->GetBool( TEXT("UnrealI.UnrealGameOptionsMenu"), TEXT("bCanModifyGore"), CanModifyGore );
	if( !CanModifyGore && !GIsEditor )
	{
		FString S;
		if (appLoadFileToString(S, TEXT("Story.txt")))
		{
			WTextScrollerDialog Dlg(TEXT("The Story"), *S);
			Dlg.DoModal();
		}
	}
#endif

	// Create the global engine object.
	UClass* EngineClass;
	if( !GIsEditor )
	{
		// Create game engine.
		EngineClass = UObject::StaticLoadClass( UGameEngine::StaticClass(), NULL, TEXT("ini:Engine.Engine.GameEngine"), NULL, LOAD_NoFail, NULL );
	}
	else
	{
		// Editor.
		EngineClass = UObject::StaticLoadClass( UEngine::StaticClass(), NULL, TEXT("ini:Engine.Engine.EditorEngine"), NULL, LOAD_NoFail, NULL );
	}
	UEngine* Engine = ConstructObject<UEngine>( EngineClass );
	Engine->bBootCrashRecovery = bCrashRecover;
	//appDebugMessagef(TEXT("Engine->GetFullName()); %ls"),Engine->GetFullName());
	Engine->Init();
	debugf( TEXT("Startup time: %f seconds"), appSeconds()-LoadTime );

	return Engine;
	unguard;
}

//
// Unreal's main message loop.  All windows in Unreal receive messages
// somewhere below this function on the stack.
//

static inline void DispatchMessages()
{
	// Handle all incoming messages.
	guard(MessagePump);
	MSG Msg;
	POINT mouse;
	while (PeekMessageW(&Msg, NULL, 0, 0, PM_REMOVE))
	{
		if (Msg.message == WM_QUIT)
			GIsRequestingExit = 1;

		if (GIsEditor)
		{
			HWND Curr = GetActiveWindow();
			if (Curr == NULL)
				Curr = Msg.hwnd;
			TranslateAccelerator(
				Curr,  // handle to receiving window
				hAccel,    // handle to active accelerator table
				&Msg);     // message data
			if (WC_DIALOG == MAKEINTATOM(GetClassLong(Curr, GCW_ATOM)) && IsDialogMessage(Curr, &Msg))
				continue;
		}

		guard(TranslateDispatchMessage);
		if (Msg.message == WM_MOUSEWHEEL && GetCursorPos(&mouse))
		{ // hack for send scroll message to window below cursor, not which has focus
			HWND Win = WindowFromPoint(mouse);
			if (Win)
			{
				DWORD Pid = 0;
				GetWindowThreadProcessId(Win, &Pid);
				if (Pid == GetCurrentProcessId())
					Msg.hwnd = Win;
			}
		}

		GMessageTime = Msg.time;

		TranslateMessage(&Msg);
		DispatchMessageW(&Msg);
		unguardf((TEXT("%08X %i"), (PTRINT)Msg.hwnd, Msg.message));

		// stijn: I added this because modern versions of windows no longer propagate exceptions thrown in Window procs.
		// If you want an exception thrown in a window proc to propagate all the way to the main loop, then you need to
		// eat the exception in the window proc, and rethrow it here.
		// See https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms633573(v=vs.85) for details
		if (GIsCriticalError)
			throw 1;
	}
	unguard;
}

static inline void AdjustThreadPriority(DWORD xThreadId, HANDLE xhThread)
{
	// If editor thread doesn't have the focus, don't suck up too much CPU time.
	if (GIsEditor)
	{
		guard(ThrottleEditor);
		static UBOOL HadFocus = 1;
		UBOOL HasFocus = (GetWindowThreadProcessId(GetForegroundWindow(), NULL) == xThreadId);
		if (HadFocus && !HasFocus)
		{
			// Drop our priority to speed up whatever is in the foreground.
			SetThreadPriority(xhThread, THREAD_PRIORITY_BELOW_NORMAL);
		}
		else if (HasFocus && !HadFocus)
		{
			// Boost our priority back to normal.
			SetThreadPriority(xhThread, THREAD_PRIORITY_NORMAL);
		}
		if (!HasFocus)
		{
			// Surrender the rest of this timeslice.
			Sleep(0);
		}
		HadFocus = HasFocus;
		unguard;
	}
}

static void MainLoop( UEngine* Engine )
{
	guard(MainLoop);
	check(Engine);

	// Enter main loop.
	guard(EnterMainLoop);
	if( GLogWindow )
		GLogWindow->SetExec( Engine );
	unguard;

	// Loop while running.
	GIsRunning = 1;
	DWORD xThreadId = GetCurrentThreadId();
	HANDLE xhThread = GetCurrentThread();
	FTime OldTime = appSeconds();
	FTime SecondStartTime = OldTime;
	INT TickCount = 0;

	#if HUNG_SAFE_BUILD
	FHungChecker ErrorChecker;
	#endif

	STAT(QWORD LastCycles = appCycles());

	while( GIsRunning && !GIsRequestingExit )
	{
		// Update the world.
		guard(UpdateWorld);
		#if HUNG_SAFE_BUILD
		ErrorChecker.Refresh();
		#endif
		#if _WITH_DEBUG_CPU_COUNT_
		FCPUFunctionClock::Reset();
		#endif
		FTime NewTime   = appSeconds();
		FLOAT DeltaTime = NewTime - OldTime;
		Engine->Tick( DeltaTime );
		if( GWindowManager )
			GWindowManager->Tick( DeltaTime );
		OldTime = NewTime;
		TickCount++;
		if( OldTime > SecondStartTime + 1 )
		{
			Engine->CurrentTickRate = (FLOAT)TickCount / (OldTime - SecondStartTime);
			SecondStartTime = OldTime;
			TickCount = 0;
		}
		unguard;

		// Enforce optional maximum tick rate.
		guard(EnforceTickRate);
		FLOAT MaxTickRate = Engine->GetMaxTickRate();
		if( MaxTickRate>0.0 )
		{
			FLOAT Delta = (1.0f / MaxTickRate) - (appSeconds() - OldTime);
			// stijn: jezus guys wtf were you smoking. undersleeps are fine. Oversleeps are the devil!
			//while (Delta > 0.f) 
			//{
				appSleep(Max(0.f, Delta));
			//	Delta = (1.0f / MaxTickRate) - (appSeconds() - OldTime);
			//}
		}
		unguard;

		DispatchMessages();

		// If editor thread doesn't have the focus, don't suck up too much CPU time.
		if( GIsEditor )
		{
			guard(ThrottleEditor);
			static UBOOL HadFocus=1;
			UBOOL HasFocus = (GetWindowThreadProcessId(GetForegroundWindow(),NULL) == xThreadId );
			if( HadFocus && !HasFocus )
			{
				// Drop our priority to speed up whatever is in the foreground.
				SetThreadPriority( xhThread, THREAD_PRIORITY_BELOW_NORMAL );
			}
			else if( HasFocus && !HadFocus )
			{
				// Boost our priority back to normal.
				SetThreadPriority( xhThread, THREAD_PRIORITY_NORMAL );
			}
			if( !HasFocus )
			{
				// Surrender the rest of this timeslice.
				Sleep(0);
			}
			HadFocus = HasFocus;
			unguard;
		}
		//else UObject::TickGarbageCollector();

#if STATS
		guard(TickStats);
		QWORD NewCycles = appCycles();
		GStatFPS.FPS.Time = NewCycles - LastCycles;
		++GStatFPS.FrameCount.Count;
		LastCycles = NewCycles;
		FStatGroup::Tick();
		unguard;
#endif
	}
	GIsRunning = 0;

	// Exit main loop.
	guard(ExitMainLoop);
	if( GLogWindow )
		GLogWindow->SetExec( NULL );
	GExec = NULL;

	if (Engine->PhysicsEngine)
	{
		Engine->PhysicsEngine->ConditionalDestroy();
		delete Engine->PhysicsEngine;
		Engine->PhysicsEngine = NULL;
	}
	if (GIsCriticalError)
	{
		FThreadingBase::ShutdownThreads();
		appThrowf(TEXT("Error on main thread")); // Error from a sub-thread.
	}
	unguard;

	unguardf((TEXT("(ClientCam Pos=(%g,%g,%g) Rot=(%i,%i,%i))"), _ClientCameraLocation.X, _ClientCameraLocation.Y, _ClientCameraLocation.Z, _ClientCameraRotation.Pitch, _ClientCameraRotation.Yaw, _ClientCameraRotation.Roll));
}
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
