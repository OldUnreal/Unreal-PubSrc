/*=============================================================================
	Window.h: GUI window management code.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if defined(_MSC_VER) && _MSC_VER==1200
#define VK_VOLUME_MUTE                  0xAD
#define VK_VOLUME_DOWN                  0xAE
#define VK_VOLUME_UP                    0xAF
#define WM_INPUT                        0x00FF
#define WM_MOUSEWHEEL                   0x020A
#define WM_DWMCOMPOSITIONCHANGED        0x031E
#define WM_DWMNCRENDERINGCHANGED        0x031F
#define WM_DWMCOLORIZATIONCOLORCHANGED  0x0320
#define WM_DWMWINDOWMAXIMIZEDCHANGE     0x0321
#endif
#include <Windowsx.h>

inline const TCHAR* GetWindowMessageString(UINT Message)
{
	guard(GetWindowMessageString);

#define HANDLE_CASE_(type) case type: return TEXT(#type)
	switch (Message)
	{
		HANDLE_CASE_(WM_NULL);
		HANDLE_CASE_(WM_CREATE);
		HANDLE_CASE_(WM_NCCREATE);
		HANDLE_CASE_(WM_INITDIALOG);
		HANDLE_CASE_(WM_GETMINMAXINFO);
		HANDLE_CASE_(WM_CHAR);
		HANDLE_CASE_(WM_KEYDOWN);
		HANDLE_CASE_(WM_LBUTTONDOWN);
		HANDLE_CASE_(WM_RBUTTONDOWN);
		HANDLE_CASE_(WM_ACTIVATEAPP);
		HANDLE_CASE_(WM_NCACTIVATE);
		HANDLE_CASE_(WM_DESTROY);
		HANDLE_CASE_(WM_DRAWITEM);
		HANDLE_CASE_(WM_MEASUREITEM);
		HANDLE_CASE_(WM_CLOSE);
		HANDLE_CASE_(WM_PAINT);
		HANDLE_CASE_(WM_TIMER);
		HANDLE_CASE_(WM_ENTERIDLE);
		HANDLE_CASE_(WM_SETFOCUS);
		HANDLE_CASE_(WM_ACTIVATE);
		HANDLE_CASE_(WM_KILLFOCUS);
		HANDLE_CASE_(WM_SIZE);
		HANDLE_CASE_(WM_PASTE);
		HANDLE_CASE_(WM_SHOWWINDOW);
		HANDLE_CASE_(WM_COPYDATA);
		HANDLE_CASE_(WM_CAPTURECHANGED);
		HANDLE_CASE_(WM_MDIACTIVATE);
		HANDLE_CASE_(WM_MOUSEMOVE);
		HANDLE_CASE_(WM_LBUTTONUP);
		HANDLE_CASE_(WM_RBUTTONUP);
		HANDLE_CASE_(WM_CUT);
		HANDLE_CASE_(WM_COPY);
		HANDLE_CASE_(WM_UNDO);
		HANDLE_CASE_(WM_SETCURSOR);
		HANDLE_CASE_(WM_COMMAND);
		HANDLE_CASE_(WM_HSCROLL);
		HANDLE_CASE_(WM_VSCROLL);
		HANDLE_CASE_(WM_INPUT);
		HANDLE_CASE_(WM_MOUSEWHEEL);
		HANDLE_CASE_(WM_HELP);
		HANDLE_CASE_(WM_MOVE);
		HANDLE_CASE_(EM_GETPASSWORDCHAR);
		HANDLE_CASE_(WM_TIMECHANGE);
		HANDLE_CASE_(WM_SETFONT);
		HANDLE_CASE_(WM_NCCALCSIZE);
		HANDLE_CASE_(WM_SETTEXT);
		HANDLE_CASE_(WM_PARENTNOTIFY);
		HANDLE_CASE_(WM_WINDOWPOSCHANGING);
		HANDLE_CASE_(WM_WINDOWPOSCHANGED);
		HANDLE_CASE_(WM_GETICON);
		HANDLE_CASE_(WM_NCPAINT);
		HANDLE_CASE_(WM_ERASEBKGND);
		HANDLE_CASE_(WM_IME_SETCONTEXT);
		HANDLE_CASE_(WM_IME_NOTIFY);
		HANDLE_CASE_(WM_DISPLAYCHANGE);
		HANDLE_CASE_(WM_SETREDRAW);
		HANDLE_CASE_(WM_STYLECHANGING);
		HANDLE_CASE_(WM_STYLECHANGED);
		HANDLE_CASE_(WM_SYSKEYDOWN);
		HANDLE_CASE_(WM_CANCELMODE);
		HANDLE_CASE_(WM_SYSCHAR);
		HANDLE_CASE_(WM_SYSCOMMAND);
		HANDLE_CASE_(WM_ENTERMENULOOP);
		HANDLE_CASE_(WM_EXITMENULOOP);
		HANDLE_CASE_(WM_INITMENU);
		HANDLE_CASE_(WM_MENUCHAR);
		HANDLE_CASE_(LB_ADDSTRING);
		HANDLE_CASE_(LB_GETCOUNT);
		HANDLE_CASE_(LB_GETITEMHEIGHT);
		HANDLE_CASE_(LB_GETITEMDATA);
		HANDLE_CASE_(VK_VOLUME_MUTE);
		HANDLE_CASE_(VK_VOLUME_DOWN);
		HANDLE_CASE_(VK_VOLUME_UP);
		HANDLE_CASE_(VK_NUMLOCK);
		HANDLE_CASE_(VK_SCROLL);
		HANDLE_CASE_(WM_DWMCOMPOSITIONCHANGED);
		HANDLE_CASE_(WM_DWMNCRENDERINGCHANGED);
		HANDLE_CASE_(WM_DWMCOLORIZATIONCOLORCHANGED);
		HANDLE_CASE_(WM_DWMWINDOWMAXIMIZEDCHANGE);
		HANDLE_CASE_(WM_NCDESTROY);
		HANDLE_CASE_(WM_KEYUP);
		HANDLE_CASE_(WM_NCHITTEST);
		HANDLE_CASE_(WM_SYSKEYUP);
		HANDLE_CASE_(WM_MENUSELECT);
		HANDLE_CASE_(LB_GETITEMRECT);
		HANDLE_CASE_(LB_FINDSTRING);
		HANDLE_CASE_(LB_RESETCONTENT);
		HANDLE_CASE_(VK_LSHIFT);
		HANDLE_CASE_(VK_RSHIFT);
		HANDLE_CASE_(VK_LCONTROL);
		HANDLE_CASE_(VK_RCONTROL);
		HANDLE_CASE_(VK_LMENU);
		HANDLE_CASE_(VK_RMENU);
		HANDLE_CASE_(VK_INSERT);
		HANDLE_CASE_(LB_ITEMFROMPOINT);
		HANDLE_CASE_(WM_CTLCOLORLISTBOX);
		HANDLE_CASE_(LB_GETCARETINDEX);
		default:
		{
			static TCHAR CustomMsg[32];
			appSprintf(CustomMsg,TEXT("Unknown(0x%x)"), Message);
			return CustomMsg;
		}
	}
#undef HANDLE_CASE_
	unguard;
}

#pragma once
#include "..\Src\Res\WindowRes.h"
#include "UnMsg.h"
#include <richedit.h>
#include <Windows.h>
#include <Windowsx.h>

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	Defines.
-----------------------------------------------------------------------------*/

#ifndef WINDOW_API
#define WINDOW_API DLL_IMPORT
#endif

#define WIN_OBJ 0

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

// Classes.
class WWindow;
class WControl;
class WWizardDialog;
class WWizardPage;
class WDragInterceptor;

// Global functions.
WINDOW_API void InitWindowing();
WINDOW_API HBITMAP LoadFileToBitmap( const TCHAR* Filename, INT& SizeX, INT& SizeY );

// Global variables.
extern WINDOW_API HBRUSH hBrushWhite;
extern WINDOW_API HBRUSH hBrushOffWhite;
extern WINDOW_API HBRUSH hBrushHeadline;
extern WINDOW_API HBRUSH hBrushBlack;
extern WINDOW_API HBRUSH hBrushStipple;
extern WINDOW_API HBRUSH hBrushCurrent;
extern WINDOW_API HBRUSH hBrushDark;
extern WINDOW_API HBRUSH hBrushGrey;
extern WINDOW_API HFONT  hFontText;
extern WINDOW_API HFONT  hFontUrl;
extern WINDOW_API HFONT  hFontHeadline;
extern WINDOW_API class  WLog* GLogWindow;
extern WINDOW_API HINSTANCE hInstanceWindow;
extern WINDOW_API UBOOL GNotify;
extern WINDOW_API UINT WindowMessageOpen;
extern WINDOW_API UINT WindowMessageMouseWheel;
extern WINDOW_API NOTIFYICONDATA NID;
extern WINDOW_API HACCEL hAccel;      // handle to accelerator table
extern WINDOW_API NOTIFYICONDATAA NIDA;

/*-----------------------------------------------------------------------------
	Window class definition macros.
-----------------------------------------------------------------------------*/

inline void MakeWindowClassName( TCHAR* Result, const TCHAR* Base )
{
	guard(MakeWindowClassName);
	appSprintf( Result, TEXT("%lsUnreal%ls"), appPackage(), Base );
	unguard;
}

#if WIN_OBJ
	#define DECLARE_WINDOWCLASS(cls,parentcls,pkg) \
	public: \
		void GetWindowClassName( TCHAR* Result ) {MakeWindowClassName(Result,TEXT(#cls));} \
		void Destroy() \
		{ \
			Super::Destroy(); \
			MaybeDestroy(); \
		} \
		virtual const TCHAR* GetPackageName() { return TEXT(#pkg); }
#else
	#define DECLARE_WINDOWCLASS(cls,parentcls,pkg) \
	public: \
		void GetWindowClassName( TCHAR* Result ) {MakeWindowClassName(Result,TEXT(#cls));} \
		~cls() {MaybeDestroy();} \
		virtual const TCHAR* GetPackageName() { return TEXT(#pkg); }
#endif

#define DECLARE_WINDOWSUBCLASS(cls,parentcls,pkg) \
	DECLARE_WINDOWCLASS(cls,parentcls,pkg) \
	static WNDPROC SuperProc;

#define IMPLEMENT_WINDOWCLASS(cls,clsf) \
	{TCHAR Temp[256]; MakeWindowClassName(Temp,TEXT(#cls)); cls::RegisterWindowClass( Temp, clsf );}

#define IMPLEMENT_WINDOWSUBCLASS(cls,wincls) \
	{TCHAR Temp[256]; MakeWindowClassName(Temp,TEXT(#cls)); cls::SuperProc = cls::RegisterWindowClass( Temp, wincls );}

#define FIRST_AUTO_CONTROL 8192

/*-----------------------------------------------------------------------------
	FRect.
-----------------------------------------------------------------------------*/

struct FPoint
{
	INT X, Y;
	FPoint()
	{}
	FPoint( INT InX, INT InY )
	:	X( InX )
	,	Y( InY )
	{}
	static FPoint ZeroValue()
	{
		return FPoint(0,0);
	}
	static FPoint NoneValue()
	{
		return FPoint(INDEX_NONE,INDEX_NONE);
	}
	operator POINT*() const
	{
		return (POINT*)this;
	}
	const INT& operator()( INT i ) const
	{
		return (&X)[i];
	}
	INT& operator()( INT i )
	{
		return (&X)[i];
	}
	static INT Num()
	{
		return 2;
	}
	UBOOL operator==( const FPoint& Other ) const
	{
		return X==Other.X && Y==Other.Y;
	}
	UBOOL operator!=( const FPoint& Other ) const
	{
		return X!=Other.X || Y!=Other.Y;
	}
	FPoint& operator+=( const FPoint& Other )
	{
		X += Other.X;
		Y += Other.Y;
		return *this;
	}
	FPoint& operator-=( const FPoint& Other )
	{
		X -= Other.X;
		Y -= Other.Y;
		return *this;
	}
	FPoint operator+( const FPoint& Other ) const
	{
		return FPoint(*this) += Other;
	}
	FPoint operator-( const FPoint& Other ) const
	{
		return FPoint(*this) -= Other;
	}
};

struct FRect
{
	FPoint Min, Max;
	FRect()
	{}
	FRect( INT X0, INT Y0, INT X1, INT Y1 )
	:	Min( X0, Y0 )
	,	Max( X1, Y1 )
	{}
	FRect( FPoint InMin, FPoint InMax )
	:	Min( InMin )
	,	Max( InMax )
	{}
	FRect( RECT R )
	:	Min( R.left, R.top )
	,	Max( R.right, R.bottom )
	{}
	operator RECT*() const
	{
		return (RECT*)this;
	}
	const FPoint& operator()( INT i ) const
	{
		return (&Min)[i];
	}
	FPoint& operator()( INT i )
	{
		return (&Min)[i];
	}
	static INT Num()
	{
		return 2;
	}
	UBOOL operator==( const FRect& Other ) const
	{
		return Min==Other.Min && Max==Other.Max;
	}
	UBOOL operator!=( const FRect& Other ) const
	{
		return Min!=Other.Min || Max!=Other.Max;
	}
	FRect Right( INT Width )
	{
		return FRect( ::Max(Min.X,Max.X-Width), Min.Y, Max.X, Max.Y );
	}
	FRect Bottom( INT Height )
	{
		return FRect( Min.X, ::Max(Min.Y,Max.Y-Height), Max.X, Max.Y );
	}
	FPoint Size()
	{
		return FPoint( Max.X-Min.X, Max.Y-Min.Y );
	}
	INT Width()
	{
		return Max.X-Min.X;
	}
	INT Height()
	{
		return Max.Y-Min.Y;
	}
	FRect& operator+=( const FPoint& P )
	{
		Min += P;
		Max += P;
		return *this;
	}
	FRect& operator-=( const FPoint& P )
	{
		Min -= P;
		Max -= P;
		return *this;
	}
	FRect operator+( const FPoint& P ) const
	{
		return FRect( Min+P, Max+P );
	}
	FRect operator-( const FPoint& P ) const
	{
		return FRect( Min-P, Max-P );
	}
	FRect operator+( const FRect& R ) const
	{
		return FRect( Min+R.Min, Max+R.Max );
	}
	FRect operator-( const FRect& R ) const
	{
		return FRect( Min-R.Min, Max-R.Max );
	}
	FRect Inner( FPoint P ) const
	{
		return FRect( Min+P, Max-P );
	}
	UBOOL Contains( FPoint P ) const
	{
		return P.X>=Min.X && P.X<Max.X && P.Y>=Min.Y && P.Y<Max.Y;
	}
	FRect& DPIScaled(INT DPIX, INT DPIY)
	{
		Min.X = MulDiv(Min.X, DPIX, 96);
		Min.Y = MulDiv(Min.Y, DPIY, 96);
		Max.X = MulDiv(Max.X, DPIX, 96);
		Max.Y = MulDiv(Max.Y, DPIY, 96);
		return *this;
	}
};

/*-----------------------------------------------------------------------------
	FControlSnoop.
-----------------------------------------------------------------------------*/

// For forwarding interaction with a control to an object.
class WINDOW_API FControlSnoop
{
public:
	// FControlSnoop interface.
	virtual void SnoopChar( WWindow* Src, INT Char ) {}
	virtual void SnoopKeyDown( WWindow* Src, INT Char ) {}
	virtual void SnoopLeftMouseDown( WWindow* Src, FPoint P ) {}
	virtual void SnoopRightMouseDown( WWindow* Src, FPoint P ) {}
};

/*-----------------------------------------------------------------------------
	FCommandTarget.
-----------------------------------------------------------------------------*/

//
// Interface for accepting commands.
//
class WINDOW_API FCommandTarget
{
public:
	virtual void Unused() {}
};

//
// Delegate function pointers.
//
typedef void(FCommandTarget::*TDelegate)();
typedef void(FCommandTarget::*TDelegateInt)(INT);

//
// Simple bindings to an object and a member function of that object.
//
struct WINDOW_API FDelegate
{
	FCommandTarget* TargetObject;
	void (FCommandTarget::*TargetInvoke)();
	FDelegate( FCommandTarget* InTargetObject=NULL, TDelegate InTargetInvoke=NULL )
	: TargetObject( InTargetObject )
	, TargetInvoke( InTargetInvoke )
	{}
	virtual void operator()() { if( TargetObject ) (TargetObject->*TargetInvoke)(); }
};
struct WINDOW_API FDelegateInt
{
	FCommandTarget* TargetObject;
	void (FCommandTarget::*TargetInvoke)(int);
	FDelegateInt( FCommandTarget* InTargetObject=NULL, TDelegateInt InTargetInvoke=NULL )
	: TargetObject( InTargetObject )
	, TargetInvoke( InTargetInvoke )
	{}
	virtual void operator()( int I ) { if( TargetObject ) (TargetObject->*TargetInvoke)(I); }
};

// Text formatting.
static inline FString LineFormat(const TCHAR* In)
{
	return FString(In).Replace(TEXT("\\n"), TEXT("\n"));
}

static inline FString LineFormat(FString In)
{
	return In.Replace(TEXT("\\n"), TEXT("\n"));
}

/*-----------------------------------------------------------------------------
	Menu helper functions.
-----------------------------------------------------------------------------*/

//
// Load a menu and localize its text.
//
inline void LocalizeSubMenu( HMENU hMenu, const FString Name, const TCHAR* Package )
{
	guard(LocalizeSubMenu);
	for( INT i=GetMenuItemCount(hMenu)-1; i>=0; i-- )
	{
		MENUITEMINFO Info;
		TCHAR Buffer[1024];
		appMemzero( &Info, sizeof(Info) );
		Info.cbSize     = sizeof(Info);
		Info.fMask      = MIIM_TYPE | MIIM_SUBMENU;
		Info.cch        = ARRAY_COUNT(Buffer);
		Info.dwTypeData = Buffer;
		GetMenuItemInfo( hMenu, i, 1, &Info );
		const TCHAR* String = (const TCHAR*)Info.dwTypeData;
		if( String && String[0]=='I' && String[1]=='D' && String[2]=='_' )
		{
			FString Str = Localize(*Name, String, Package);
			appStrcpy(Buffer, *Str);
			SetMenuItemInfo( hMenu, i, 1, &Info );
		}
		if( Info.hSubMenu )
			LocalizeSubMenu( Info.hSubMenu, *Name, Package );
	}
	unguard;
}
inline WINDOW_API HMENU LoadLocalizedMenu( HINSTANCE hInst, INT Id, const FString Name, const TCHAR* Package=GPackage )
{
	guard(LoadLocalizedMenu);
	HMENU hMenu = LoadMenuW(hInst, MAKEINTRESOURCEW(Id) );
	if( !hMenu )
		appErrorf( TEXT("Failed loading menu: %ls %ls"), Package, *Name );
	LocalizeSubMenu( hMenu, *Name, Package );
	return hMenu;
	unguard;
}

//
// Toggle a menu item and return 0 if it's now off, 1 if it's now on.
//
inline WINDOW_API UBOOL ToggleMenuItem( HMENU hMenu, UBOOL Item )
{
	guard(ToggleMenuItem);
	if( GetMenuState(hMenu,Item,MF_BYCOMMAND)&MF_CHECKED )
	{
		// Now unchecked.
		CheckMenuItem( hMenu, Item, MF_UNCHECKED );
		return 0;
	}
	else
	{
		// Now checked.
		CheckMenuItem( hMenu, Item, MF_CHECKED );
		return 1;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	FWindowsBitmap.
-----------------------------------------------------------------------------*/

// A bitmap.
class FWindowsBitmap
{
public:
	INT SizeX, SizeY, Keep;
	FWindowsBitmap( UBOOL InKeep=0 )
	: SizeX( 0 )
	, SizeY( 0 )
	, Keep( InKeep )
	{}
	~FWindowsBitmap()
	{
		if( hBitmap && !Keep )
			DeleteObject( hBitmap );
	}
	UBOOL LoadFile( const TCHAR* Filename )
	{
		if( hBitmap )
			DeleteObject( hBitmap );
		hBitmap = LoadFileToBitmap( Filename, SizeX, SizeY );
		return hBitmap!=NULL;
	}
	HBITMAP GetBitmapHandle()
	{
		return hBitmap;
	}
private:
	HBITMAP hBitmap{NULL};
	void operator=( FWindowsBitmap& ) {}
};

/*-----------------------------------------------------------------------------
	WWindow.
-----------------------------------------------------------------------------*/

#if WIN_OBJ
	#define W_DECLARE_ABSTRACT_CLASS(a,b,c) DECLARE_ABSTRACT_CLASS(a,b,c)
	#define W_DECLARE_CLASS(a,b,c) DECLARE_CLASS(a,b,c)
	#define W_IMPLEMENT_CLASS(a) IMPLEMENT_CLASS(a)
#else
	#define W_DECLARE_ABSTRACT_CLASS(a,b,c) public:
	#define W_DECLARE_CLASS(a,b,c) public:
	#define W_IMPLEMENT_CLASS(a)
#endif

// An operating system window.
class WINDOW_API WWindow :
#if WIN_OBJ
public UObject,
#endif
public FCommandTarget
{
	W_DECLARE_ABSTRACT_CLASS(WWindow,UObject,CLASS_Transient);

	// Variables.
	HWND					hWnd;
	FName					PersistentName;
	WORD					ControlId, TopControlId;
	BITFIELD				Destroyed:1;
	BITFIELD				MdiChild:1;
	WWindow*				OwnerWindow;
	FNotifyHook*			NotifyHook;
	FControlSnoop*			Snoop;
	TArray<class WControl*>	Controls;
	BOOL m_bShow;
	WPARAM LastwParam;
	LPARAM LastlParam;

	// Static.
	static INT              ModalCount;
	static INT              DPIX;
	static INT              DPIY;
	static TArray<WWindow*> _Windows;
	static TArray<WWindow*> _DeleteWindows;
	// Use this procedure for modeless dialogs.
	static INT_PTR CALLBACK StaticDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		guard(WWindow::StaticDlgProc);
		INT i;
		for( i=0; i<_Windows.Num(); i++ )
			if( _Windows(i)->hWnd==hwndDlg )
				break;
		if( i==_Windows.Num() && uMsg==WM_INITDIALOG )
		{
			WWindow* WindowCreate = (WWindow*)lParam;
			check(WindowCreate);
			check(!WindowCreate->hWnd);
			WindowCreate->hWnd = hwndDlg;
			for( i=0; i<_Windows.Num(); i++ )
				if( _Windows(i)==WindowCreate )
					break;
			check(i<_Windows.Num());
		}
		if( i!=_Windows.Num() && !GIsCriticalError )
		{
			_Windows(i)->WndProc( uMsg, wParam, lParam );
		}

		// Give up cycles.
		//
		::Sleep(0);

		return 0;
		unguard;
	}
	static LRESULT APIENTRY StaticWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
	{
#if !defined(_DEBUG) && DO_GUARD
		try
		{
#endif
			guard(WWindow::StaticProc);

			INT i;
			for (i = 0; i < _Windows.Num(); i++)
				if (_Windows(i)->hWnd == hWnd)
					break;
			if (i == _Windows.Num() && (Message == WM_NCCREATE || Message == WM_INITDIALOG))
			{
				WWindow* WindowCreate
					= Message != WM_NCCREATE
					? (WWindow*)lParam
					: (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_MDICHILD)
					? (WWindow*)((MDICREATESTRUCT*)((CREATESTRUCT*)lParam)->lpCreateParams)->lParam
					: (WWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
				check(WindowCreate);
				check(!WindowCreate->hWnd);
				WindowCreate->hWnd = hWnd;
				for (i = 0; i < _Windows.Num(); i++)
					if (_Windows(i) == WindowCreate)
						break;
				check(i < _Windows.Num());
			}
			if (i == _Windows.Num() || GIsCriticalError)
			{
				// Gets through before WM_NCCREATE: WM_GETMINMAXINFO
				return DefWindowProcW(hWnd, Message, wParam, lParam);
			}
			else
			{
				return _Windows(i)->WndProc(Message, wParam, lParam);
			}
			unguardf((TEXT("(Message=%ls)"), GetWindowMessageString(Message)));
#if !defined(_DEBUG) && DO_GUARD
		}
		catch (TCHAR*)
		{
		}
		catch (...)
		{
			if (GIsEditor) // Editor doesn't seam to always handle error on some windows.
			{
				// Crashed.
				GIsCriticalError = TRUE;
				GIsRequestingExit = TRUE;
			}
			appUnwindf(TEXT("Critical error in window message!"));
			throw;
		}
		return DefWindowProcW(hWnd, Message, wParam, lParam);
#endif
	}
	static WNDPROC RegisterWindowClass( const TCHAR* Name, DWORD Style )
	{
		guard(WWindow::RegisterWindowClass);
		WNDCLASSEXW Cls;
		appMemzero( &Cls, sizeof(Cls) );
		Cls.cbSize			= sizeof(Cls);
		Cls.style			= Style;
		Cls.lpfnWndProc		= StaticWndProc;
		Cls.hInstance		= hInstanceWindow;
		Cls.hIcon			= LoadIconW(hInstanceWindow,MAKEINTRESOURCEW((GIsEditor?IDICON_Editor:IDICON_Mainframe)));
		Cls.lpszClassName	= Name;
		Cls.hIconSm			= LoadIconW(hInstanceWindow,MAKEINTRESOURCEW((GIsEditor?IDICON_Editor:IDICON_Mainframe)));
		verify(RegisterClassExW( &Cls ));
		return NULL;
		unguard;
	}

	// Structors.
	WWindow( FName InPersistentName=NAME_None, WWindow* InOwnerWindow=NULL )
#if WIN_OBJ
	:	UObject				( EC_InPlaceConstructor, WWindow::StaticClass(), UObject::GetTransientPackage(), NAME_None, 0 )
	,	hWnd				( NULL )
#else
	:	hWnd				( NULL )
#endif
	,	PersistentName		( InPersistentName )
	,	ControlId			( 0 )
	,	TopControlId		( FIRST_AUTO_CONTROL )
	,	Destroyed			( 0 )
	,   MdiChild            ( 0 )
	,	OwnerWindow			( InOwnerWindow )
	,	NotifyHook			( 0 )
	,   Snoop               ( NULL )
	, m_bShow(0)
	, LastwParam(0)
	, LastlParam(0)
	{}
#if WIN_OBJ
	void Destroy()
	{
		guard(WWindow::Destroy);
		Super::Destroy();
		MaybeDestroy();
		m_bShow = FALSE;
		WWindow::_DeleteWindows.RemoveItem( this );
		unguard;
	}
#else
	virtual ~WWindow() noexcept(false)
	{
		guard(WWindow:;~WWindow);
		MaybeDestroy();
		WWindow::_DeleteWindows.RemoveItem( this );
		unguard;
	}
#endif

	// Accessors.
	FRect GetClientRect() const
	{
		RECT R;
		::GetClientRect( hWnd, &R );
		return FRect( R );
	}
	void MoveWindow( FRect R, UBOOL bRepaint )
	{
		::MoveWindow( hWnd, R.Min.X, R.Min.Y, R.Width(), R.Height(), bRepaint );
		//::SetWindowPos( hWnd, HWND_TOP, R.Min.X, R.Min.Y, R.Width(), R.Height(), SWP_ASYNCWINDOWPOS );
	}
	void MoveWindow( int Left, int Top, int Width, int Height, UBOOL bRepaint )
	{
		::MoveWindow( hWnd, Left, Top, Width, Height, bRepaint );
		//::SetWindowPos( hWnd, HWND_TOP, Left, Top, Width, Height, SWP_ASYNCWINDOWPOS ); //maybe for non blocking window behavior some day.
	}
	FRect GetWindowRect() const
	{
		RECT R;
		::GetWindowRect( hWnd, &R );
		return OwnerWindow ? OwnerWindow->ScreenToClient(R) : FRect(R);
	}
	FPoint ClientToScreen( const FPoint& InP )
	{
		POINT P;
		P.x = InP.X;
		P.y = InP.Y;
		::ClientToScreen( hWnd, &P );
		return FPoint( P.x, P.y );
	}
	FPoint ScreenToClient( const FPoint& InP )
	{
		POINT P;
		P.x = InP.X;
		P.y = InP.Y;
		::ScreenToClient( hWnd, &P );
		return FPoint( P.x, P.y );
	}
	FRect ClientToScreen( const FRect& InR )
	{
		return FRect( ClientToScreen(InR.Min), ClientToScreen(InR.Max) );
	}
	FRect ScreenToClient( const FRect& InR )
	{
		return FRect( ScreenToClient(InR.Min), ScreenToClient(InR.Max) );
	}
	FPoint GetCursorPos()
	{
		FPoint Mouse;
		::GetCursorPos( Mouse );
		return ScreenToClient( Mouse );
	}
	void Show( UBOOL Show )
	{
		guard(WWindow::Show);
		m_bShow = Show;
		ShowWindow( hWnd, Show ? ::IsIconic(hWnd) ? SW_RESTORE : SW_SHOW : SW_HIDE );
		unguard;
	}

	// WWindow interface.
	virtual void Serialize( FArchive& Ar )
	{
		guard(WWindow::Serialize);
		//!!UObject interface.
		//!!Super::Serialize( Ar );
		Ar << PersistentName;
		unguard;
	}
	virtual const TCHAR* GetPackageName()
	{
		return TEXT("Window");
	}
	virtual void DoDestroy()
	{
		guard(WWindow::DoDestroy);
		if( NotifyHook )
			NotifyHook->NotifyDestroy( this );
		if( hWnd )
			DestroyWindow( *this );
		_Windows.RemoveItem( this );
		unguard;
	}
	virtual void GetWindowClassName( TCHAR* Result )=0;
	virtual LRESULT WndProc( UINT Message, WPARAM wParam, LPARAM lParam )
	{
		guard(WWindow::WndProc);
		try
		{
			LastwParam = wParam;
			LastlParam = lParam;

			// Message snoop.
			if( Snoop )
			{
				//guard(Snoop);
				if (Message == WM_CHAR)
				{
					Snoop->SnoopChar(this, wParam);
					if (wParam == 13)
						return 0;
				}
				else if( Message==WM_KEYDOWN )
					Snoop->SnoopKeyDown( this, wParam );
				else if( Message==WM_LBUTTONDOWN )
					Snoop->SnoopLeftMouseDown( this, FPoint(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)) );
				else if( Message==WM_RBUTTONDOWN )
					Snoop->SnoopRightMouseDown( this, FPoint(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)) );
				//unguard;
			}

			// Special multi-window activation handling.
			if( !MdiChild && !ModalCount )
			{
				//guard(Special);
				static UBOOL AppActive=0;
				if( Message==WM_ACTIVATEAPP )
				{
					AppActive = wParam;
					SendMessageW( hWnd, WM_NCACTIVATE, wParam, 0 );
				}
				else if( Message==WM_NCACTIVATE && AppActive && !wParam )
				{
					return 1;
				}
				//unguard;
			}

			// Message processing.
			if( Message==WM_DESTROY )
			{
				//guard(WM_DESTROY);
				OnDestroy();
				return 0;
				//unguard;
			}
			else if( Message==WM_DRAWITEM )
			{
				//guard(WM_DRAWITEM);
				DRAWITEMSTRUCT* Info = (DRAWITEMSTRUCT*)lParam;
				for( INT i=0; i<Controls.Num(); i++ )
					if( ((WWindow*)Controls(i))->hWnd==Info->hwndItem )
						{((WWindow*)Controls(i))->OnDrawItem(Info); break;}
				return 1;
				//unguard;
			}
			else if( Message==WM_MEASUREITEM )
			{
				//guard(WM_MEASUREITEM);
				MEASUREITEMSTRUCT* Info = (MEASUREITEMSTRUCT*)lParam;
				for( INT i=0; i<Controls.Num(); i++ )
					if( ((WWindow*)Controls(i))->ControlId==Info->CtlID )
						{((WWindow*)Controls(i))->OnMeasureItem(Info); break;}
				return 1;
				//unguard;
			}
			else if( Message==WM_CLOSE )
			{
				//guard(WM_CLOSE);
				OnClose();
				//unguard;
			}
			else if( Message==WM_CHAR )
			{
				//guard(WM_CHAR);
				OnChar( wParam );
				//unguard;
			}
			else if( Message==WM_KEYDOWN )
			{
				//guard(WM_KEYDOWN);
				OnKeyDown( wParam );
				//unguard;
			}
			else if( Message==WM_PAINT )
			{
				//guard(WM_PAINT);
				OnPaint();
				//unguard;
			}
			else if( Message==WM_CREATE )
			{
				//guard(WM_CREATE);
				OnCreate();
				//unguard;
			}
			else if( Message==WM_TIMER )
			{
				//guard(WM_TIMER);
				OnTimer();
				//unguard;
			}
			else if( Message==WM_INITDIALOG )
			{
				//guard(WM_INITDIALOG);
				OnInitDialog();
				//unguard;
			}
			else if( Message==WM_ENTERIDLE )
			{
				//guard(WM_ENTERIDLE);
				OnEnterIdle();
				//unguard;
			}
			else if( Message==WM_SETFOCUS )
			{
				//guard(WM_SETFOCUS);
				OnSetFocus( (HWND)wParam );
				//unguard;
			}
			else if( Message==WM_ACTIVATE )
			{
				//guard(WM_ACTIVATE);
				OnActivate( LOWORD(wParam)!=0 );
				//unguard;
			}
			else if( Message==WM_KILLFOCUS )
			{
				//guard(WM_KILLFOCUS);
				OnKillFocus( (HWND)wParam );
				//unguard;
			}
			else if( Message==WM_SIZE )
			{
				//guard(WM_SIZE);
				OnSize( wParam, LOWORD(lParam), HIWORD(lParam) );
				//unguard;
			}
			else if( Message==WM_PASTE )
			{
				//guard(WM_PASTE);
				OnPaste();
				//unguard;
			}
			else if( Message==WM_SHOWWINDOW )
			{
				//guard(WM_SHOWWINDOW);
				OnShowWindow( wParam );
				//unguard;
			}
			else if( Message==WM_COPYDATA )
			{
				//guard(WM_COPYDATA);
				OnCopyData( (HWND)wParam, (COPYDATASTRUCT*)lParam );
				//unguard;
			}
			else if( Message==WM_CAPTURECHANGED )
			{
				//guard(WM_CAPTURECHANGED);
				OnReleaseCapture();
				//unguard;
			}
			else if( Message==WM_MDIACTIVATE )
			{
				//guard(WM_MDIACTIVATE);
				OnMdiActivate( (HWND)lParam==hWnd );
				//unguard;
			}
			else if( Message==WM_MOUSEMOVE )
			{
				//guard(WM_MOUSEMOVE);
				OnMouseMove( wParam, FPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)) );
				//unguard;
			}
			else if( Message==WM_LBUTTONDOWN )
			{
				//guard(WM_LBUTTONDOWN);
				OnLeftButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				//unguard;
			}
			else if( Message==WM_RBUTTONDOWN )
			{
				//guard(WM_RBUTTONDOWN);
				OnRightButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				//unguard;
			}
			else if( Message==WM_LBUTTONUP )
			{
				//guard(WM_LBUTTONUP);
				OnLeftButtonUp();
				//unguard;
			}
			else if( Message==WM_RBUTTONUP )
			{
				//guard(WM_RBUTTONUP);
				OnRightButtonUp();
				//unguard;
			}
			else if( Message==WM_CUT )
			{
				//guard(WM_CUT);
				OnCut();
				//unguard;
			}
			else if( Message==WM_COPY )
			{
				//guard(WM_COPY);
				OnCopy();
				//unguard;
			}
			else if( Message==WM_UNDO )
			{
				//guard(WM_UNDO);
				OnUndo();
				//unguard;
			}
			else if( Message==WM_SETCURSOR )
			{
				//guard(WM_SETCURSOR);
				if( OnSetCursor() )
					return 1;
				//unguard;
			}
			else if( Message==WM_NOTIFY )
			{
				//guard(WM_NOTIFY);
				for( INT i=0; i<Controls.Num(); i++ )
					if(wParam==((WWindow*)Controls(i))->ControlId
							&& ((WWindow*)Controls(i))->InterceptControlCommand(Message,wParam,lParam) )
						return 1;
				OnCommand( wParam );
				//unguard;
			}
			else if( Message==WM_VSCROLL )
			{
				//guard(WM_VSCROLL);
				OnVScroll( wParam, lParam );
				//unguard;
			}
			else if (Message == WM_MOUSEWHEEL)
			{
				//guard(WM_MOUSEMOVE);
				SWORD zDelta = HIWORD(wParam);

				if (GIsEditor)
				{
					if (zDelta < 0)
						OnVScroll(MAKEWPARAM(SB_LINEDOWN, 0), WM_MOUSEWHEEL);// Hack to make scrollbar working in 2DShapeEditor.
					else if (zDelta > 0)
						OnVScroll(MAKEWPARAM(SB_LINEUP, 0), WM_MOUSEWHEEL);
				}
				//unguard;
 			}
			else if( Message==WM_KEYUP)
			{
				//guard(WM_KEYU);
				OnKeyUp( wParam, lParam );
				//unguard;
			}
			else if( Message==WM_COMMAND || Message==WM_HSCROLL )
			{
				//guard(WM_HSCROLL);
				for( INT i=0; i<Controls.Num(); i++ )
					if((HWND)lParam==((WWindow*)Controls(i))->hWnd
							&& ((WWindow*)Controls(i))->InterceptControlCommand(Message,wParam,lParam) )
						return 1;
				OnCommand(LOWORD(wParam)); //!!! cast needed to make accelerators work.

				// stijn: ugly hack here. Docking and undocking destroys the window.
				// Normally, we call the default proc after dispatching a message, but
				// in this case this is not safe since CallDefaultProc is a virtual func.
				// The CallDefaultProc dispatch will therefore dereference the this pointer
				// which becomes invalid after docking

				if (wParam == 40333) // IDMN_MB_DOCK - declared in UnrealEd resources
					return 1;
				
				//unguard;
			}
			else if( Message==WM_SYSCOMMAND )
			{
				//guard(WM_SYSCOMMAND);
				if (wParam == SC_CLOSE && OwnerWindow)
					ForceParentFocus();
				if( OnSysCommand( wParam ) )
					return 1;
				//unguard;
			}
			return CallDefaultProc( Message, wParam, lParam );
		}
		catch( const TCHAR* )
		{
			// This exception prevents the message from being routed to the default proc.
			return 0;
		}
		unguard;// f((TEXT("(Message=%ls,PersistentName=%ls)"), *PersistentName, GetWindowMessageString(Message)));
	}
	virtual LRESULT CallDefaultProc( UINT Message, WPARAM wParam, LPARAM lParam )
	{
		if( MdiChild )
			return DefMDIChildProcW( hWnd, Message, wParam, lParam );
		else
			return DefWindowProcW( hWnd, Message, wParam, lParam );
	}
	virtual UBOOL InterceptControlCommand( UINT Message, WPARAM wParam, LPARAM lParam )
	{
		return 0;
	}
	virtual FString GetText()
	{
		guard(WWindow::GetText);
		//check(hWnd);
		INT Length = GetLength();
		if (Length <= 0)
			return TEXT("");
		FString Out(TEXT(""), Length);
		SendMessage(*this, WM_GETTEXT, Length + 1, (LPARAM)&Out[0]);
		return Out;
		unguard;
	}
	virtual void SetText( const TCHAR* Text )
	{
		guard(WWindow::SetText);
		//check(hWnd);
		SendMessageW( *this, WM_SETTEXT, 0, (LPARAM)Text );
		unguard;
	}
	virtual INT GetLength()
	{
		guard(WWindow::GetLength);
		//check(hWnd);
		INT Length = SendMessageW(*this, WM_GETTEXTLENGTH, 0, 0);
		if (Length == LB_ERR)
			Length = 0;
		return Length;
		unguard;
	}
	void SetNotifyHook( FNotifyHook* InNotifyHook )
	{
		guard(WWindow::SetNotifyHook);
		NotifyHook = InNotifyHook;
		unguard;
	}

	virtual void ForceParentFocus(UBOOL bFirst = TRUE)
	{
		if (OwnerWindow)
			OwnerWindow->ForceParentFocus(FALSE);
		else if (!bFirst)
			SetFocus(*this);
		else if (_Windows.Num() && _Windows(0) && _Windows(0) != this)
			SetFocus(*_Windows(0));
	}

	// WWindow notifications.
	virtual void OnCopyData( HWND hWndSender, COPYDATASTRUCT* CD )
	{}
	virtual void OnSetFocus( HWND hWndLosingFocus )
	{}
	virtual void OnKillFocus( HWND hWndGaininFocus )
	{}
	virtual void OnSize( DWORD Flags, INT NewX, INT NewY )
	{}
	virtual void OnCommand( INT Command )
	{}
	virtual int OnSysCommand( INT Command )
	{
		return 0;
	}
	virtual void OnActivate( UBOOL Active )
	{}
	virtual void OnChar( TCHAR Ch )
	{}
	virtual void OnKeyDown( TCHAR Ch )
	{}
	virtual void OnCut()
	{}
	virtual void OnCopy()
	{}
	virtual void OnPaste()
	{}
	virtual void OnShowWindow( UBOOL bShow )
	{}
	virtual void OnUndo()
	{}
	virtual void OnVScroll( WPARAM wParam, LPARAM lParam )
	{}
	virtual void OnKeyUp( WPARAM wParam, LPARAM lParam )
	{}
	virtual void OnPaint()
	{}
	virtual void OnCreate()
	{}
	virtual void OnDrawItem( DRAWITEMSTRUCT* Info )
	{}
	virtual void OnMeasureItem( MEASUREITEMSTRUCT* Info )
	{}
	virtual void OnInitDialog()
	{}
	virtual void OnEnterIdle()
	{}
	virtual void OnMouseEnter()
	{}
	virtual void OnMouseLeave()
	{}
	virtual void OnMouseHover()
	{}
	virtual void OnTimer()
	{}
	virtual void OnReleaseCapture()
	{}
	virtual void OnMdiActivate( UBOOL Active )
	{}
	virtual void OnMouseMove( DWORD Flags, FPoint Location )
	{}
	virtual void OnLeftButtonDown(INT X, INT Y)
	{}
	virtual void OnRightButtonDown(INT X, INT Y)
	{}
	virtual void OnLeftButtonUp()
	{}
	virtual void OnRightButtonUp()
	{}
	virtual void OnFinishSplitterDrag( WDragInterceptor* Drag, UBOOL Success )
	{}
	virtual INT OnSetCursor()
	{
		guard(WWindow::OnSetCursor);
		return 0;
		unguard;
	}
	virtual void OnClose()
	{
		guard(WWindow::OnClose);
		if( MdiChild )
			SendMessage( OwnerWindow->hWnd, WM_MDIDESTROY, (WPARAM)hWnd, 0 );
		else
			DestroyWindow( *this );
		unguard;
	}
	virtual void OnDestroy()
	{
		guard(WWindow::OnDestroy);
		check(hWnd);
		if( PersistentName!=NAME_None && PersistentName.IsValid())
		{
			FRect R = GetWindowRect();
			if( !IsZoomed(hWnd) )
				GConfig->SetString( TEXT("WindowPositions"), *PersistentName, *FString::Printf( TEXT("(X=%i,Y=%i,XL=%i,YL=%i)"), R.Min.X, R.Min.Y, R.Width(), R.Height() ) );
		}
		_Windows.RemoveItem( this );
		hWnd = NULL;
		unguard;
	}

	// stijn: mark for deletion, but don't do the effective destruction right now because we might be in this WWindow's WndProc
	void DelayedDestroy()
	{
		_DeleteWindows.AddUniqueItem(this);
	}

	// WWindow functions.
	void SaveWindowPos()
	{
		guard(WWindow::SaveWindowPos);
		unguard;
	}
	void MaybeDestroy()
	{
		guard(WWindow::MaybeDestroy);
		if( !Destroyed )
		{
			Destroyed=1;
			DoDestroy();
		}
		unguard;
	}
	void _CloseWindow()
	{
		guard(WWindow::_CloseWindow);
		check(hWnd);
		DestroyWindow( *this );
		unguard;
	}
	operator HWND() const
	{
		return hWnd;
	}
	void SetFont( HFONT hFont )
	{
		guard(WWindow::SetFont);
		SendMessageW( *this, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(0,0) );
		unguard;
	}
	void PerformCreateWindowEx( DWORD dwExStyle, LPCTSTR lpWindowName, DWORD dwStyle, INT x, INT y, INT nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInst )
	{
		guard(PerformCreateWindowEx);
		check(hWnd==NULL);

		// Retrieve remembered position.
		FString Pos;
		if
		(	PersistentName!=NAME_None
		&&	GConfig->GetString( TEXT("WindowPositions"), *PersistentName, Pos ) )
		{
			// Get saved position.
			Parse( *Pos, TEXT("X="), x );
			Parse( *Pos, TEXT("Y="), y );
			if( dwStyle & WS_SIZEBOX )
			{
				Parse( *Pos, TEXT("XL="), nWidth );
				Parse( *Pos, TEXT("YL="), nHeight );

				nWidth = Max<INT>(nWidth, 100);
				nHeight = Max<INT>(nHeight, 80);
			}

			// Count identical windows already opened.
			INT Count=0;
			for( INT i=0; i<_Windows.Num(); i++ )
			{
				Count += _Windows(i)->PersistentName==PersistentName;
			}
			if( Count )
			{
				// Move away.
				x += Count*16;
				y += Count*16;
			}

			// Clip size to screen.
			FPoint Desktop = GetDesktopResolution();
			x = Min(x, Desktop.X - nWidth);
			y = Min(y, Desktop.Y - 48);
			if( x<0 )
			{
				if( dwStyle & WS_SIZEBOX )
					nWidth += x;
				x=0;
			}
			if( y<0 )
			{
				if( dwStyle & WS_SIZEBOX )
					nHeight += y;
				y=0;
			}
		}

		// Create window.
		_Windows.AddItem( this );
		TCHAR ClassName[256];
		GetWindowClassName( ClassName );
		//hinstance must match window class hinstance!!
		HWND hWndCreated = CreateWindowEx(dwExStyle,ClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstanceWindow,this);
		if( !hWndCreated )
			appErrorf( TEXT("CreateWindowEx failed: %ls"), appGetSystemErrorMessage() );
		check(hWndCreated);
		check(hWndCreated==hWnd);
		unguard;
	}
	void ScaleImageAndReplace(HBITMAP& hbm)
	{
		// stijn: do DPI scaling
		if (DPIX != 96 || DPIY != 96)
		{
			// fetch image dimensions
			BITMAP bm;
			GetObjectW(hbm, sizeof(bm), &bm);

			// scale up
			INT ScaledWidth = MulDiv(bm.bmWidth, DPIX, 96);
			INT ScaledHeight = MulDiv(bm.bmHeight, DPIY, 96);

			// copy and replace
			hbm = reinterpret_cast<HBITMAP>(CopyImage(hbm, IMAGE_BITMAP, ScaledWidth, ScaledHeight, LR_COPYDELETEORG));
		}
	}
	void SetRedraw( UBOOL Redraw )
	{
		guard(WWindow::SetRedraw);
		if (!m_bShow)
			return;
		SendMessageW(*this, WM_SETREDRAW, Redraw, 0);
		if (Redraw)
			RedrawWindow(*this, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
		unguard;
	}
	void ForceRepaint(UBOOL bClear = FALSE)
	{
		guardSlow(WWindow::ForceRepaint);
		InvalidateRect(hWnd, NULL, bClear);
		unguardSlow;
	}

	// Used in the editor ... used to draw window edges in custom colors.
	virtual void MyDrawEdge( HDC hdc, LPRECT qrc, UBOOL bRaised )
	{
		guard(WWindow::MyDrawEdge);

		HPEN penOld, penRaised = CreatePen( PS_SOLID, 1, RGB(159,159,159) ),
			penSunken = CreatePen( PS_SOLID, 1, RGB(106,106,106) );
		HDC	hDC = GetDC( hWnd );

		RECT rc = *qrc;
		rc.right -= 1;
		rc.bottom -= 1;

		penOld = (HPEN)SelectObject( hDC, (bRaised ? penRaised : penSunken ) );
		::MoveToEx( hDC, rc.left, rc.top, NULL );
		::LineTo( hDC, rc.right, rc.top );
		::MoveToEx( hDC, rc.left, rc.top, NULL );
		::LineTo( hDC, rc.left, rc.bottom);
		SelectObject( hDC, penOld );

		penOld = (HPEN)SelectObject( hDC, (bRaised ? penSunken : penRaised ) );
		::MoveToEx( hDC, rc.right, rc.bottom, NULL );
		::LineTo( hDC, rc.right, rc.top );
		::MoveToEx( hDC, rc.right, rc.bottom, NULL );
		::LineTo( hDC, rc.left, rc.bottom );
		SelectObject( hDC, penOld );

		DeleteObject( penRaised );
		DeleteObject( penSunken );
		::ReleaseDC( hWnd, hDC );

		unguard;
	}

	static FPoint GetDesktopResolution()
	{
		guardSlow(WWindow::GetDesktopResolution);
		return FPoint(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
		unguardSlow;
	}
};

/*-----------------------------------------------------------------------------
	WControl.
-----------------------------------------------------------------------------*/

class WINDOW_API WCustomWindowBase : public WWindow
{
	DECLARE_WINDOWCLASS(WCustomWindowBase, WWindow, Window);

	HBRUSH hbrBackground;

	WCustomWindowBase()
		: WWindow(), hbrBackground(nullptr)
	{}
	WCustomWindowBase(const FColor& BgColor)
		: WWindow(), hbrBackground(CreateSolidBrush(RGB(BgColor.R, BgColor.G, BgColor.B)))
	{}
	WCustomWindowBase(INT)
		: WWindow(), hbrBackground(CreateSolidBrush(RGB(240, 240, 240)))
	{}

	void DoDestroy()
	{
		guardSlow(WCustomWindowBase::DoDestroy);
		if (hbrBackground)
			DeleteObject(hbrBackground);
		WWindow::DoDestroy();
		unguardSlow;
	}
	void OnPaint()
	{
		guardSlow(WCustomWindowBase::OnPaint);
		if (hbrBackground)
		{
			FRect R = GetClientRect();
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			FillRect(ps.hdc, R, hbrBackground);
			EndPaint(hWnd, &ps);
		}
		unguardSlow;
	}
};

/*-----------------------------------------------------------------------------
	WControl.
-----------------------------------------------------------------------------*/

// A control which exists inside an owner window.
class WINDOW_API WControl : public WWindow
{
	W_DECLARE_ABSTRACT_CLASS(WControl,WWindow,CLASS_Transient);

	// Variables.
	WNDPROC WindowDefWndProc{};

	// Structors.
	WControl()
	{}
	WControl( WWindow* InOwnerWindow, INT InId, WNDPROC InSuperProc )
	: WWindow( NAME_None, InOwnerWindow )
	{
		check(OwnerWindow);
		WindowDefWndProc = InSuperProc;
		ControlId = InId ? InId : InOwnerWindow->TopControlId++;
		OwnerWindow->Controls.AddItem( this );
	}
#if WIN_OBJ
	void Destroy()
	{
		Super::Destroy();
		check(OwnerWindow);
		OwnerWindow->Controls.RemoveItem( this );
	}
#else
	~WControl() noexcept(false)
	{
		check(OwnerWindow);
		OwnerWindow->Controls.RemoveItem( this );
	}
#endif

	// WWindow interface.
	virtual LRESULT CallDefaultProc( UINT Message, WPARAM wParam, LPARAM lParam )
	{
		if (WindowDefWndProc)
			return CallWindowProcW( WindowDefWndProc, hWnd, Message, wParam, lParam );
		return 0;
	}

	static WNDPROC RegisterWindowClass( const TCHAR* Name, const TCHAR* WinBaseClass )
	{
		guard(WControl::RegisterWindowClass);
		WNDPROC SuperProc=NULL;
		WNDCLASSEXW Cls;
		appMemzero( &Cls, sizeof(Cls) );
		Cls.cbSize        = sizeof(Cls);
		verify( GetClassInfoExW( NULL, WinBaseClass, &Cls ) );
		SuperProc         = Cls.lpfnWndProc;
		Cls.lpfnWndProc   = WWindow::StaticWndProc;
		Cls.lpszClassName = Name;
		Cls.hInstance     = hInstanceWindow;
		check(Cls.lpszMenuName==NULL);
		verify(RegisterClassExW( &Cls ));
		return SuperProc;
		unguard;
	}

	virtual void SetEnable(UBOOL bEnabled)
	{
		::EnableWindow(hWnd, bEnabled);
	}
};

/*-----------------------------------------------------------------------------
	WTabControl.
-----------------------------------------------------------------------------*/

class WINDOW_API WTabControl : public WControl
{
	W_DECLARE_CLASS(WTabControl,WControl,CLASS_Transient)
	DECLARE_WINDOWSUBCLASS(WTabControl,WControl,Window)

	FDelegate SelectionChangeDelegate;

	// Constructor.
	WTabControl()
	{}
	WTabControl( WWindow* InOwner, INT InId=0, WNDPROC InSuperProc=NULL )
	: WControl( InOwner, InId, InSuperProc?InSuperProc:SuperProc )
	{}

	// WWindow interface.
	void OpenWindow( UBOOL Visible, DWORD dwExtraStyle = 0 )
	{
		guard(WTabControl::OpenWindow);
		PerformCreateWindowEx
		(
			0,
            NULL,
            WS_CHILD | (Visible?WS_VISIBLE:0) | dwExtraStyle,
            0, 0,
			0, 0,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);

		SendMessageW( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		unguard;
	}
	void AddTab( FString InText, int InlParam )
	{
		guard(WTabControl::AddTab);

		FString Text = InText;

		TCITEM tci;

		tci.mask = TCIF_TEXT | TCIF_PARAM;
		tci.pszText = &Text[0];
		tci.lParam = InlParam;

		SendMessageW( *this, TCM_INSERTITEM, 0, (LPARAM)(const LPTCITEM)&tci );

		unguard;
	}
	void Empty()
	{
		guard(WTabControl::Empty);
		SendMessageW( *this, TCM_DELETEALLITEMS, 0, 0 );
		unguard;
	}
	LRESULT GetCount()
	{
		guard(WTabControl::GetCount);
		return SendMessageW( *this, TCM_GETITEMCOUNT, 0, 0 );
		unguard;
	}
	LRESULT SetCurrent( int Index )
	{
		guard(WTabControl::SetCurrent);
		return SendMessageW( *this, TCM_SETCURSEL, Index, 0 );
		unguard;
	}
	LRESULT GetCurrent()
	{
		guard(WTabControl::GetCurrent);
		return SendMessageW( *this, TCM_GETCURSEL, 0, 0 );
		unguard;
	}
	int GetIndexFromlParam( int InlParam )
	{
		guard(WTabControl::GetIndexFromlParam);

		TCITEM tci;
		tci.mask = TCIF_PARAM;

		for( int x = 0 ; x < GetCount() ; x++ )
		{
			SendMessageW( *this, TCM_GETITEM, x, (LPARAM)(LPTCITEM)&tci );
			if( tci.lParam == InlParam )
				return x;
		}

		return -1;
		unguard;
	}
	FString GetString( int Index )
	{
		guard(WTabControl::GetString);

		TCHAR Buffer[256] = TEXT("");

		TCITEM tci;
		tci.mask = TCIF_TEXT;
		tci.pszText = Buffer;

		SendMessageW( *this, TCM_GETITEM, Index, (LPARAM)(LPTCITEM)&tci );

		return FString( Buffer );
		unguard;
	}
	LPARAM GetlParam( int Index )
	{
		guard(WTabControl::GetlParam);

		TCITEM tci;
		tci.mask = TCIF_PARAM;

		SendMessageW( *this, TCM_GETITEM, Index, (LPARAM)(LPTCITEM)&tci );

		return tci.lParam;
		unguard;
	}
	UBOOL InterceptControlCommand( UINT Message, WPARAM wParam, LPARAM lParam )
	{
		guard(WTabControl::InterceptControlCommand);

		LastwParam = wParam;
		LastlParam = lParam;

		NMHDR* pnmhdr = (LPNMHDR)lParam;

		if( pnmhdr->code == TCN_SELCHANGE)
			SelectionChangeDelegate();

		return 0;
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WLabel.
-----------------------------------------------------------------------------*/

// A non-interactive label control.
class WINDOW_API WLabel : public WControl
{
	W_DECLARE_CLASS(WLabel,WControl,CLASS_Transient)
	DECLARE_WINDOWSUBCLASS(WLabel,WControl,Window)

	// Constructor.
	WLabel()
	{}
	WLabel( WWindow* InOwner, INT InId=0, WNDPROC InSuperProc=NULL )
	: WControl( InOwner, InId, InSuperProc?InSuperProc:SuperProc )
	{}

	// WWindow interface.
	void OpenWindow(UBOOL Visible, UBOOL ClientEdge = 1, DWORD dwExtraStyle = 0, INT x = 0, INT y = 0, INT xs = 0, INT ys = 0)
	{
		guard(WLabel::OpenWindow);
		PerformCreateWindowEx
		(
			(ClientEdge?WS_EX_CLIENTEDGE:0),
            NULL,
            WS_CHILD | (Visible?WS_VISIBLE:0) | dwExtraStyle,
            x, y,
			xs, ys,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);

		SendMessageW( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WCustomLabel.
-----------------------------------------------------------------------------*/

class WINDOW_API WCustomLabel : public WControl
{
	W_DECLARE_CLASS(WCustomLabel,WControl,CLASS_Transient)
	DECLARE_WINDOWSUBCLASS(WCustomLabel,WControl,Window)

public:
	COLORREF TextColor;

	// Constructor.
	WCustomLabel()
	{}
	WCustomLabel( WWindow* InOwner, INT InId=0, WNDPROC InSuperProc=NULL )
	: WControl( InOwner, InId, InSuperProc?InSuperProc:SuperProc ), TextColor(RGB(0, 0, 0))
	{}

	void DoDestroy()
	{
		guard(WCustomLabel::DoDestroy);
		WControl::DoDestroy();
		unguard;
	}

	// WWindow interface.
	void OpenWindow( UBOOL Visible, UBOOL ClientEdge = 1, DWORD dwExtraStyle = 0, INT x = 0, INT y = 0, INT xs = 0, INT ys = 0)
	{
		guard(WCustomLabel::OpenWindow);
		PerformCreateWindowEx
		(
			(ClientEdge?WS_EX_CLIENTEDGE:0),
            NULL,
            WS_CHILD | (Visible?WS_VISIBLE:0) | dwExtraStyle,
            x, y,
			xs, ys,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);

		SendMessageW( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		unguard;
	}
	void OnPaint()
	{
	}

	inline void SetTextColor(const FColor& C)
	{
		TextColor = RGB(C.R, C.G, C.B);
	}
};

/*-----------------------------------------------------------------------------
	WButton.
-----------------------------------------------------------------------------*/

// A button.
class WINDOW_API WButton : public WControl
{
	W_DECLARE_CLASS(WButton,WControl,CLASS_Transient);
	DECLARE_WINDOWSUBCLASS(WButton,WControl,Window)

	// Delegates.
	FDelegate ClickDelegate;
	FDelegate DoubleClickDelegate;
	FDelegate PushDelegate;
	FDelegate UnPushDelegate;
	FDelegate SetFocusDelegate;
	FDelegate KillFocusDelegate;
	HBITMAP hbm{};
	UBOOL bChecked{}, bOwnerDraw{};

	// Constructor.
	WButton()
	{}
	WButton( WWindow* InOwner, INT InId=0, FDelegate InClicked=FDelegate(), WNDPROC InSuperProc=NULL )
	: WControl( InOwner, InId, InSuperProc?InSuperProc:SuperProc )
	, ClickDelegate( InClicked )
	{
		bChecked = 0;
	}

	// WWindow interface.
	void OpenWindow( UBOOL Visible, INT X, INT Y, INT XL, INT YL, const TCHAR* Text, UBOOL bBitmap = 0, DWORD dwExtraStyle = 0 )
	{
		guard(WButton::OpenWindow);
		PerformCreateWindowEx
		(
			0,
            NULL,
            WS_CHILD | BS_PUSHBUTTON | (bBitmap?BS_BITMAP:0) | dwExtraStyle,
            X, Y,
			XL, YL,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);

		SendMessageW( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		SetText( Text );
		if( Visible )
			ShowWindow( *this, SW_SHOWNOACTIVATE );
		bOwnerDraw = (dwExtraStyle & BS_OWNERDRAW);
		unguard;
	}
	void SetVisibleText(const TCHAR* Text)
	{
		guard(WButton::SetText);
		check(hWnd);
		if (*Text)
		{
			SetText(Text);
			Show(1);
		}
		else Show(0);
		unguard;
	}
	void SetBitmap( HBITMAP Inhbm )
	{
		guard(WButton::SetText);
		SendMessageW( hWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)Inhbm);
		hbm = Inhbm;
		unguard;
	}
	virtual void Clicked(void)
	{
		guard(WButton::Clicked);
		SendMessageW( OwnerWindow->hWnd, WM_COMMAND, WM_PB_PUSH, ControlId );
		unguard;
	}
	void OnDrawItem( DRAWITEMSTRUCT* Item )
	{
		guard(WButton::OnDrawItem);
		RECT R=Item->rcItem;
		UBOOL bPressed = (Item->itemState&ODS_SELECTED)!=0 || bChecked;

		HBRUSH brush = CreateSolidBrush( RGB(128,128,128) );
		FillRect( Item->hDC, &Item->rcItem, brush );

		MyDrawEdge( Item->hDC, &R, !bPressed );

		HDC hdcMem;
		HBITMAP hbmOld;

		hdcMem = CreateCompatibleDC(Item->hDC);
		hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
		BITMAP bitmap;
		GetObjectA( hbm, sizeof(BITMAP), (LPSTR)&bitmap );

		int Adjust = (bPressed) ? 2 : 0;

		BitBlt(Item->hDC,
			(R.right / 2) - (bitmap.bmWidth / 2) + Adjust, (R.bottom / 2) - (bitmap.bmHeight / 2) + Adjust,
			bitmap.bmWidth, bitmap.bmHeight,
			hdcMem,
			0, 0,
			SRCCOPY);

		// If this button is checked, draw a light border around in the inside to make it obvious.
		if( bPressed )
		{
			HPEN penOld, pen = CreatePen( PS_SOLID, 1, RGB(145,210,198) );

			RECT rc = R;
			rc.left++;	rc.top++;	rc.right--;		rc.bottom--;

			penOld = (HPEN)SelectObject( Item->hDC, pen );
			::MoveToEx( Item->hDC, rc.left, rc.top, NULL );
			::LineTo( Item->hDC, rc.right, rc.top );
			::LineTo( Item->hDC, rc.right, rc.bottom );
			::LineTo( Item->hDC, rc.left, rc.bottom );
			::LineTo( Item->hDC, rc.left, rc.top );
			SelectObject( Item->hDC, penOld );

			DeleteObject( pen );
		}

		// Clean up.
		//
		DeleteObject(brush);
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);

		unguard;
	}

	// WControl interface.
	UBOOL InterceptControlCommand( UINT Message, WPARAM wParam, LPARAM lParam )
	{
		guard(WButton::InterceptControlCommand);
		if     ( HIWORD(wParam)==BN_CLICKED   )
		{
			Clicked();
			ClickDelegate();
			// This notification returns 0 instead of 1 because the editor sometimes wants
			// to know about clicks without using a delegate (i.e. dynamically created buttons)
			return 0;
		}
		else if( HIWORD(wParam)==BN_DBLCLK    ) {DoubleClickDelegate(); return 1;}
		else if( HIWORD(wParam)==BN_PUSHED    ) {PushDelegate();        return 1;}
		else if( HIWORD(wParam)==BN_UNPUSHED  ) {UnPushDelegate();      return 1;}
		else if( HIWORD(wParam)==BN_SETFOCUS  ) {SetFocusDelegate();    return 1;}
		else if( HIWORD(wParam)==BN_KILLFOCUS ) {UnPushDelegate();      return 1;}
		else return 0;
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WToolTip
-----------------------------------------------------------------------------*/

// Tooltip window - easy way to create tooltips for standard controls.
class WToolTip : public WWindow
{
	DECLARE_WINDOWCLASS(WToolTip,WWindow,Window)

	HBITMAP hbmOn, hbmOff;
	RECT ClientPos, BmpOffPos, BmpOnPos;
	FString ToolTip;
	int ID;
	HWND hWnd;
	UBOOL bOn, bHasBeenSetup;

	// Structors.
	WToolTip()
	{}

	WToolTip( WWindow* InOwnerWindow )
	:	WWindow( TEXT("ToolTipButton"), InOwnerWindow )
	{
		hbmOn = hbmOff = NULL;
		hWnd = NULL;
		bOn = bHasBeenSetup = 0;
	}

	// WWindow interface.
	void OpenWindow()
	{
		guard(WToolTip::OpenWindow);
		hWnd = CreateWindow(TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP | WS_POPUP,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			OwnerWindow->hWnd, (HMENU)NULL, hInstance, NULL);
		check(hWnd);
		SendMessageW( hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		unguard;
	}
	void AddTool( HWND InHwnd, FString InToolTip, int InId, RECT* InRect = NULL )
	{
		guard(WToolTip::AddTool);

		check(::IsWindow( hWnd ));

		TOOLINFOW ti = {0};
		FString TmpToolTip = InToolTip;
		ti.cbSize = TTTOOLINFO_V1_SIZE;
		ti.uFlags = TTF_SUBCLASS;
		ti.hwnd = InHwnd;
		ti.hinst = hInstance;
		ti.uId = InId;
		ti.lpszText = &TmpToolTip[0];
		if (InRect)
			ti.rect = *InRect;
		else
			::GetClientRect(InHwnd, &ti.rect);

		verify(SendMessageW(hWnd, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)& ti));
		unguard;
	}
};
/*-----------------------------------------------------------------------------
	WToolTipPopup
-----------------------------------------------------------------------------*/

// Tooltip window - easy way to create tooltips for standard controls.
class WToolTipPopup
{
public:
	HWND hWnd;
	WWindow* ParentWindow;
	TOOLINFO ti;

protected:
	FTime VisTime;
	UINT UpdateFrame;
	FString PendingMsg;
	POINT CurPos;
	FLOAT PreDelay, PostDelay;
	INT PenX, PenY;
	BITFIELD bToolTipVis : 1;
	BITFIELD bPendingVis : 1;

public:
	// Structors.
	WToolTipPopup( WWindow* Parent=NULL )
		: hWnd(NULL)
		, ParentWindow(Parent)
		, bToolTipVis(0)
		, UpdateFrame(0)
	{
		guard(WToolTipPopup::WToolTipPopup);
		appMemzero(&ti,sizeof(TOOLINFO));
		unguard;
	}

	~WToolTipPopup() noexcept(false)
	{
		guard(WToolTipPopup::~WToolTipPopup);
		if( hWnd )
		{
			DestroyWindow(hWnd);
			hWnd = NULL;
		}
		unguard;
	}

	void OpenTooltip( const TCHAR* ToolTipStr, INT X, INT Y, FLOAT DisplayTime, FLOAT Delay=0.f )
	{
		guard(WToolTipPopup::OpenTooltip);

		if (Delay <= 0.f)
		{
			bPendingVis = 0;
			ShowTool(ToolTipStr, X, Y, DisplayTime);
		}
		else
		{
			if (bToolTipVis)
				HideToolTip();
			bPendingVis = 1;
			PendingMsg = ToolTipStr;
			PenX = X;
			PenY = Y;
			PreDelay = Delay;
			PostDelay = DisplayTime;
			CurPos.x = -1000;
		}

		unguard;
	}
	void HideToolTip()
	{
		guard(WToolTipPopup::HideToolTip);
		bPendingVis = 0;
		if( bToolTipVis )
		{
			bToolTipVis = 0;
			SendMessageW(hWnd, TTM_TRACKACTIVATE, 0, (LPARAM) (LPTOOLINFO)&ti);
		}
		unguard;
	}
	void Tick()
	{
		if ((bToolTipVis || bPendingVis) && ParentWindow && !ParentWindow->GetClientRect().Contains(ParentWindow->GetCursorPos()))
		{
			// Left window screen, stop pending.
			HideToolTip();
			return;
		}
		if (bToolTipVis && VisTime < appSeconds())
			HideToolTip();
		else if (bPendingVis)
		{
			POINT P;
			::GetCursorPos(&P);
			if (Abs(P.x - CurPos.x) < 5 && Abs(P.y - CurPos.y) < 5)
			{
				if (VisTime < appSeconds())
				{
					bPendingVis = 0;
					ShowTool(*PendingMsg, PenX, PenY, PostDelay);
					PendingMsg.Empty();
				}
			}
			else
			{
				CurPos = P;
				VisTime = appSeconds() + PreDelay;
			}
		}
	}

protected:
	void ShowTool(const TCHAR* ToolTipStr, INT X, INT Y, FLOAT DisplayTime)
	{
		if (!hWnd)
		{
			hWnd = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
				WS_POPUP | (ParentWindow ? 0 : TTS_ALWAYSTIP),
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				ParentWindow ? ParentWindow->hWnd : NULL, NULL, NULL, NULL);

			// INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE
#if _WIN32_WINNT  > 0x0501
			ti.cbSize = TTTOOLINFO_V1_SIZE;
#else
			ti.cbSize = sizeof(TOOLINFO);
#endif
			ti.uFlags = TTF_TRANSPARENT | TTF_SUBCLASS;
			ti.hwnd = ParentWindow ? ParentWindow->hWnd : NULL;
			ti.hinst = NULL;
			ti.uId = (UINT_PTR)hWnd;
			ti.lpszText = (PTSTR)ToolTipStr;

			SendMessageW(hWnd, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
			SendMessageW(hWnd, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(X, Y));
			SendMessageW(hWnd, TTM_SETMAXTIPWIDTH, 0, 250);
		}
		else
		{
			// Update the text.
			ti.lpszText = (PTSTR)ToolTipStr;
			SendMessageW(hWnd, TTM_SETTOOLINFO, 0, (LPARAM)&ti);

			// Position the tooltip. The coordinates are adjusted so that the tooltip does not overlap the mouse pointer.
			SendMessageW(hWnd, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(X, Y));
		}
		if (!bToolTipVis)
		{
			bToolTipVis = 1;
			SendMessageW(hWnd, TTM_TRACKACTIVATE, 1, (LPARAM)&ti);
		}
		VisTime = appSeconds() + DisplayTime;
	}
};
/*-----------------------------------------------------------------------------
	WPictureButton.
-----------------------------------------------------------------------------*/
class WPictureButton : public WWindow
{
	DECLARE_WINDOWCLASS(WPictureButton,WWindow,Window)

	HBITMAP hbmOn, hbmOff;
	RECT ClientPos, BmpOffPos, BmpOnPos;
	FString ToolTip;
	int ID;
	HWND hWndTT;
	UBOOL bOn, bHasBeenSetup;

	// Structors.
	WPictureButton( WWindow* InOwnerWindow )
		: WWindow(TEXT("PictureButton"), InOwnerWindow)
		, hbmOn(NULL), hbmOff(NULL)
		, ID(0)
		, hWndTT(NULL)
		, bOn(FALSE), bHasBeenSetup(FALSE)
	{
	}

	// WWindow interface.
	void OpenWindow()
	{
		guard(WPictureButton::OpenWindow);
		MdiChild = 0;

		// HAVE to call "SetUp" before opening the window
		check(bHasBeenSetup);

		PerformCreateWindowEx
		(
			0,
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			ClientPos.left,
			ClientPos.top,
			ClientPos.right,
			ClientPos.bottom,
			OwnerWindow ? OwnerWindow->hWnd : NULL,
			NULL,
			hInstance
		);
		SendMessageW( hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );

		hWndTT = CreateWindowA(TOOLTIPS_CLASSA, NULL, TTS_ALWAYSTIP,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, (HMENU) NULL, hInstance, NULL);
		check(hWndTT);

		unguard;
	}
	void OnSize( DWORD Flags, INT NewX, INT NewY )
	{
		guard(WPictureButton::OnSize);
		WWindow::OnSize(Flags, NewX, NewY);

		RECT rect;
		::GetClientRect( hWnd, &rect );

		if( ToolTip.Len() )
		{
			TOOLINFO ti;
			FString TmpToolTip = ToolTip;
			ti.cbSize = TTTOOLINFO_V1_SIZE;
			ti.uFlags = TTF_SUBCLASS;
			ti.hwnd = hWnd;
			ti.hinst = hInstance;
			ti.uId = 900;
			ti.lpszText = &TmpToolTip[0];
			ti.rect = rect;

			SendMessageW(hWndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO)&ti);
		}

		unguard;
	}
	void OnDestroy()
	{
		guard(WPictureButton::OnDestroy);
		WWindow::OnDestroy();
		DestroyWindow( hWndTT );
		unguard;
	}
	void OnCreate()
	{
		guard(WPictureButton::OnCreate);
		WWindow::OnCreate();
		unguard;
	}
	void OnPaint()
	{
		guard(WPictureButton::OnPaint);
		PAINTSTRUCT PS;
		HDC hDC = BeginPaint( *this, &PS );
		HBRUSH brushBack = CreateSolidBrush( RGB(255,255,255) );

		RECT rc;
		::GetClientRect( hWnd, &rc );

		FillRect( hDC, &rc, brushBack );

		HDC hdcMem;
		HBITMAP hbmOld;

		hdcMem = CreateCompatibleDC(hDC);
		hbmOld = (HBITMAP)SelectObject(hdcMem, (bOn ? hbmOn : hbmOff));

		RECT BmpPos = (bOn ? BmpOnPos : BmpOffPos);

		BitBlt(hDC,
			(rc.right - BmpPos.right) / 2, (rc.bottom - BmpPos.bottom) / 2,
			BmpPos.right, BmpPos.bottom,
			hdcMem,
			BmpPos.left, BmpPos.top,
			SRCCOPY);

		// Clean up.
		//
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
		DeleteObject( brushBack );

		EndPaint( *this, &PS );
		unguard;
	}
	void OnLeftButtonDown(INT X, INT Y)
	{
		guard(WPictureButton::OnLeftButtonDown);
		SendMessageW( OwnerWindow->hWnd, WM_COMMAND, WM_PB_PUSH, ID );
		unguard;
	}
	void SetUp( const TCHAR* InToolTip, int InID,
		int InClientLeft, int InClientTop, int InClientRight, int InClientBottom,
		HBITMAP InHbmOff, int InBmpOffLeft, int InBmpOffTop, int InBmpOffRight, int InBmpOffBottom,
		HBITMAP InHbmOn, int InBmpOnLeft, int InBmpOnTop, int InBmpOnRight, int InBmpOnBottom )
	{
		guard(WPictureButton::SetUp);

		bHasBeenSetup = TRUE;

		ToolTip = InToolTip;
		ID = InID;
		ClientPos.left = InClientLeft;
		ClientPos.top = InClientTop;
		ClientPos.right = InClientRight;
		ClientPos.bottom = InClientBottom;
		hbmOff = InHbmOff;
		BmpOffPos.left = InBmpOffLeft;
		BmpOffPos.top = InBmpOffTop;
		BmpOffPos.right = InBmpOffRight;
		BmpOffPos.bottom = InBmpOffBottom;
		hbmOn = InHbmOn;
		BmpOnPos.left = InBmpOnLeft;
		BmpOnPos.top = InBmpOnTop;
		BmpOnPos.right = InBmpOnRight;
		BmpOnPos.bottom = InBmpOnBottom;

		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WCheckBox.
-----------------------------------------------------------------------------*/

// A checkbox.
class WINDOW_API WCheckBox : public WButton
{
	W_DECLARE_CLASS(WCheckBox,WButton,CLASS_Transient);
	DECLARE_WINDOWSUBCLASS(WCheckBox,WButton,Window)

	UBOOL bAutocheck{};

	// Constructor.
	WCheckBox()
	{}
	WCheckBox( WWindow* InOwner, INT InId=0, FDelegate InClicked=FDelegate() )
	: WButton( InOwner, InId, InClicked )
	{}

	// WWindow interface.
	void OpenWindow( UBOOL Visible, INT X, INT Y, INT XL, INT YL, const TCHAR* Text, UBOOL InbAutocheck = 1, UBOOL bBitmap = 0, DWORD dwExtraStyle = 0 )
	{
		guard(WCheckBox::OpenWindow);
		bAutocheck = InbAutocheck;
		PerformCreateWindowEx
		(
			0,
            NULL,
			WS_CHILD | BS_CHECKBOX | (bAutocheck?BS_AUTOCHECKBOX:0) | (bBitmap?BS_BITMAP:0) | dwExtraStyle,
            X, Y,
			XL, YL,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);
		SetText( Text );
		SendMessageW( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		if( Visible )
			ShowWindow( *this, SW_SHOWNOACTIVATE );
		bOwnerDraw = (dwExtraStyle & BS_OWNERDRAW);

		unguard;
	}
	void OnCreate()
	{
		guard(WCheckBox::OnCreate);
		WButton::OnCreate();
		bOwnerDraw = 0;
		unguard;
	}
	virtual void OnRightButtonDown(INT X, INT Y)
	{
		guard(WCheckBox::OnRightButtonDown);
		// This is just a quick hack so the parent window can know when the user right clicks on
		// one of the buttons in the UnrealEd button bar.
		SendMessageW( ::GetParent( hWnd ), WM_COMMAND, WM_BB_RCLICK, ControlId );
		unguard;
	}
	BOOL IsChecked( void )
	{
		guard(WCheckBox::IsChecked);

		// Owner draw buttons require us to keep track of the checked status ourselves.
		if( bOwnerDraw )
			return bChecked;
		else
			return ( SendMessageW( *this, BM_GETCHECK, 0, 0 ) == BST_CHECKED );
		unguard;
	}
	void SetCheck( int iCheck )
	{
		guard(WCheckBox::SetCheck);
		SendMessageW( *this, BM_SETCHECK, (WPARAM)iCheck, 0 );
		bChecked = (iCheck == BST_CHECKED);
		unguard;
	}
	virtual void Clicked()
	{
		guard(WCheckBox::Clicked);
		if( bAutocheck )
			bChecked = !bChecked;
		else
			bChecked = FALSE;
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WVScrollBar.
-----------------------------------------------------------------------------*/

// A vertical scrollbar.
class WINDOW_API WVScrollBar : public WControl
{
	W_DECLARE_CLASS(WVScrollBar,WControl,CLASS_Transient);
	DECLARE_WINDOWSUBCLASS(WVScrollBar,WControl,Window)

	// Constructor.
	WVScrollBar()
	{}
	WVScrollBar( WWindow* InOwner, INT InId=0, WNDPROC InSuperProc=NULL  )
	: WControl( InOwner, InId, InSuperProc?InSuperProc:SuperProc )
	{}

	// WWindow interface.
	void OpenWindow( UBOOL Visible, INT X, INT Y, INT XL, INT YL )
	{
		guard(WVScrollBar::OpenWindow);
		PerformCreateWindowEx
		(
			0,
            NULL,
            WS_CHILD | SBS_RIGHTALIGN | SBS_VERT,
            X, Y,
			XL, YL,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);
		SendMessageW( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		if( Visible )
			ShowWindow( *this, SW_SHOWNOACTIVATE );
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WTreeView.
-----------------------------------------------------------------------------*/

// A tree control
class WINDOW_API WTreeView : public WControl
{
	W_DECLARE_CLASS(WTreeView,WControl,CLASS_Transient);
	DECLARE_WINDOWSUBCLASS(WTreeView,WControl,Window)

	// Delegates.
	FDelegate ItemExpandingDelegate;
	FDelegate SelChangedDelegate;
	FDelegate DblClkDelegate;
	FDelegate OnToolTipDelegate;
	UBOOL bShowToolTip;

	// Constructor.
	WTreeView()
	{}
	WTreeView( WWindow* InOwner, INT InId=0, WNDPROC InSuperProc=NULL  )
	: WControl( InOwner, InId, InSuperProc?InSuperProc:SuperProc )
	{}

	// WWindow interface.
	void OpenWindow( UBOOL Visible, INT X, INT Y, INT XL, INT YL, UBOOL bShowToolTips=FALSE )
	{
		guard(WTreeView::OpenWindow);
		PerformCreateWindowEx
		(
			WS_EX_CLIENTEDGE,
            NULL,
            WS_CHILD | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_DISABLEDRAGDROP | (bShowToolTips ? TVS_INFOTIP : 0),
            X, Y,
			XL, YL,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);
		SendMessageW( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		if( Visible )
			ShowWindow( *this, SW_SHOWNOACTIVATE );
		unguard;
	}
	void Empty( void )
	{
		guard(WTreeView::Empty);
		SendMessageW( *this, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT );
		unguard;
	}
	HTREEITEM AddItem( const TCHAR* pName, HTREEITEM hti, BOOL bHasChildren )
	{
		guard(WTreeView::AddItem);

		TV_INSERTSTRUCT	InsertInfo;

		InsertInfo.hParent = hti;
		InsertInfo.hInsertAfter = TVI_SORT;

		InsertInfo.item.mask = TVIF_TEXT;
		InsertInfo.item.pszText = const_cast<TCHAR*>(pName); // stijn: for historic reasons, the constness needs to be stripped off, even though Windows will not modify the value

		if( bHasChildren )
		{
			InsertInfo.item.mask |= TVIF_CHILDREN;
			InsertInfo.item.cChildren = 1;
		}

		return (HTREEITEM)SendMessageW( *this, TVM_INSERTITEM, 0, (LPARAM) &InsertInfo );

		unguard;
	}
	inline LPNMTVGETINFOTIP GetToolTipParm()
	{
		return (LPNMTVGETINFOTIP)LastlParam;
	}
	inline LPNMTREEVIEW GetTreeViewParm()
	{
		return (LPNMTREEVIEW)LastlParam;
	}
	const TCHAR* GetItemValue(HTREEITEM Item)
	{
		guard(WTreeView::GetItemValue);
		TCHAR chText[1024] = TEXT("\0");
		TVITEM tvi;

		appMemzero(&tvi, sizeof(tvi));
		tvi.hItem = Item;
		tvi.mask = TVIF_TEXT;
		tvi.pszText = chText;
		tvi.cchTextMax = sizeof(chText);

		if (SendMessageW(hWnd, TVM_GETITEM, 0, (LPARAM)&tvi))
		{
			TCHAR* Result = appStaticString1024();
			appStrncpy(Result, tvi.pszText, 1024);
			return Result;
		}
		return NULL;
		unguard;
	}
	HTREEITEM FindItem(HWND hWnd, HTREEITEM hItemParent, LPCWSTR FindName)
	{
		guard(WTreeView::FindItem);
		TVITEM Tvi;
		TCHAR ItemName[512];

		for (Tvi.hItem = TreeView_GetChild(hWnd, hItemParent); Tvi.hItem; Tvi.hItem = TreeView_GetNextSibling(hWnd, Tvi.hItem))
		{
			Tvi.mask = TVIF_TEXT | TVIF_CHILDREN;
			Tvi.pszText = ItemName;
			Tvi.cchTextMax = _countof(ItemName);
			if (TreeView_GetItem(hWnd, &Tvi))
			{
				if (appStricmp(Tvi.pszText, FindName) == 0)
					return Tvi.hItem;
				if (Tvi.cChildren)
				{
					HTREEITEM SearchChild = FindItem(hWnd, Tvi.hItem, FindName);
					if (SearchChild)
						return SearchChild;
				}
			}
		}
		return 0;
		unguard;
	}
	virtual void OnRightButtonDown(INT X, INT Y)
	{
		guard(WTreeView::OnRightButtonDown);
		PostMessageW( OwnerWindow->hWnd, WM_COMMAND, WM_TREEVIEW_RIGHT_CLICK, 0 );
		unguard;
	}
	UBOOL InterceptControlCommand( UINT Message, WPARAM wParam, LPARAM lParam )
	{
		guard(WTreeView::InterceptControlCommand);

		LastwParam = wParam;
		LastlParam = lParam;

		NMTREEVIEW* pntv = (LPNMTREEVIEW)lParam;

		switch (pntv->hdr.code)
		{
		case TVN_GETINFOTIP:
			bShowToolTip = FALSE;
			OnToolTipDelegate();
			return bShowToolTip;
		case TVN_SELCHANGEDA:
		case TVN_SELCHANGEDW:
			SelChangedDelegate();
			break;
		case TVN_ITEMEXPANDINGA:
		case TVN_ITEMEXPANDINGW:
			ItemExpandingDelegate();
			break;
		case NM_DBLCLK:
			DblClkDelegate();
			return TRUE;
		}

		return FALSE;
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WCoolButton.
-----------------------------------------------------------------------------*/

// Frame showing styles.
enum EFrameFlags
{
	CBFF_ShowOver	= 0x01,
	CBFF_ShowAway	= 0x02,
	CBFF_DimAway    = 0x04,
	CBFF_UrlStyle	= 0x08,
	CBFF_NoCenter   = 0x10
};

// A coolbar-style button.
class WINDOW_API WCoolButton : public WButton
{
	W_DECLARE_CLASS(WCoolButton,WButton,CLASS_Transient);
	DECLARE_WINDOWSUBCLASS(WCoolButton,WButton,Window)

	// Variables.
	static WCoolButton* GlobalCoolButton;
	HICON hIcon{};
	DWORD FrameFlags{};

	// Constructor.
	WCoolButton()
	{}
	WCoolButton( WWindow* InOwner, INT InId=0, FDelegate InClicked=FDelegate(), DWORD InFlags=CBFF_ShowOver|CBFF_DimAway )
	: WButton( InOwner, InId, InClicked )
	, hIcon( NULL )
	, FrameFlags( InFlags )
	{}

	// WWindow interface.
	void OpenWindow( UBOOL Visible, INT X, INT Y, INT XL, INT YL, const TCHAR* Text )
	{
		guard(WButton::OpenWindow);
#ifndef JAPANESE
		PerformCreateWindowEx
		(
			0,
            NULL,
            WS_CHILD | BS_OWNERDRAW,
            X, Y,
			XL, YL,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);
#else
		PerformCreateWindowEx
		(
			0,
            TEXT("BUTTON"),
            WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
            X, Y,
			XL, YL,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);
#endif
		SetText( Text );

		SendMessageW( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		if (Visible)
		{
			ShowWindow(*this, SW_SHOWNOACTIVATE);
			m_bShow = TRUE;
		}
		unguard;
	}
	void OnDestroy()
	{
		if( GlobalCoolButton==this )
			GlobalCoolButton=NULL;
		WButton::OnDestroy();
	}
#ifndef JAPANESE
	void OnCreate()
	{
		WButton::OnCreate();
		SetClassLongX( *this, GCL_STYLE, GetClassLongX(*this,GCL_STYLE) & ~CS_DBLCLKS);
	}

	// WCoolButton interface.
	void UpdateHighlight( UBOOL TurnOff )
	{
		guard(WCoolButton::UpdateHighlight);
		FPoint P;
		FRect R;
		::GetCursorPos((POINT*)&P);
		::GetWindowRect(*this,(RECT*)&R);
		UBOOL ShouldHighlight = (R.Contains(P) || GetCapture()==hWnd) && !TurnOff;
		if( (GlobalCoolButton==this) != (ShouldHighlight) )
		{
			if( GlobalCoolButton==this )
			{
				GlobalCoolButton = NULL;
				KillTimer( hWnd, 0 );
			}
			else
			{
				if( GlobalCoolButton )
					GlobalCoolButton->UpdateHighlight( 1 );
				GlobalCoolButton = this;
				SetTimer( hWnd, 0, 50, NULL );
			}
			InvalidateRect( *this, NULL, 1 );
			UpdateWindow( *this );
		}
		unguard;
	}
	void OnTimer()
	{
		guard(WCoolButton::OnTimer);
		UpdateHighlight(0);
		unguard;
	}
	INT OnSetCursor()
	{
		guard(WUrlButton::OnSetCursor);
		WButton::OnSetCursor();
		UpdateHighlight(0);
		if( FrameFlags & CBFF_UrlStyle )
			SetCursor(LoadCursorIdX(hInstanceWindow,IDC_Hand));
		return 1;
		unguard;
	}

	// WWindow interface.
	void OnDrawItem( DRAWITEMSTRUCT* Item )
	{
		guard(WCoolButton::OnDrawItem);
		RECT R=Item->rcItem;
		UBOOL Pressed = (Item->itemState&ODS_SELECTED)!=0;
		FillRect( Item->hDC, &Item->rcItem, (HBRUSH)(COLOR_BTNFACE+1));
		if( GlobalCoolButton==this )
		{
			if( FrameFlags & CBFF_ShowOver )
				DrawEdge( Item->hDC, &Item->rcItem, Pressed?EDGE_SUNKEN:EDGE_RAISED, BF_RECT );
		}
		else
		{
			if( FrameFlags & CBFF_DimAway )
				DrawEdge( Item->hDC, &Item->rcItem, Pressed?BDR_SUNKENINNER:BDR_RAISEDINNER, BF_RECT );
			else if( FrameFlags & CBFF_ShowAway )
				DrawEdge( Item->hDC, &Item->rcItem, Pressed?EDGE_SUNKEN:EDGE_RAISED, BF_RECT );
		}
		R.left += Pressed;
		R.right += Pressed;
		R.top += Pressed;
		R.bottom += Pressed;
		if( hIcon )
		{
			ICONINFO II;
			GetIconInfo( hIcon, &II );
			DrawIcon( Item->hDC, R.left, R.top + (R.bottom-R.top)/2-II.yHotspot, hIcon );
			R.left += II.xHotspot * 2;
		}
		FString Text = GetText();
		DWORD TextFlags
		=	DT_END_ELLIPSIS
		|	DT_VCENTER
		|	DT_SINGLELINE
		|	((FrameFlags & CBFF_NoCenter) ? 0 : DT_CENTER);
		HFONT OldFont;
		if( FrameFlags & CBFF_UrlStyle )
		{
			R.left += 8;
			SetTextColor( Item->hDC, RGB(0,0,255) );
			OldFont = SelectObject( Item->hDC, hFontUrl );
		}
		else
			OldFont = SelectObject( Item->hDC, hFontText );
		SetBkMode( Item->hDC, TRANSPARENT );
		DrawTextX( Item->hDC, *Text, Text.Len(), &R, TextFlags );
		SelectObject( Item->hDC, OldFont );
		unguard;
	}
#endif
};

/*-----------------------------------------------------------------------------
	WUrlButton.
-----------------------------------------------------------------------------*/

// A URL button.
class WINDOW_API WUrlButton : public WCoolButton
{
	W_DECLARE_CLASS(WUrlButton,WCoolButton,CLASS_Transient);
	DECLARE_WINDOWSUBCLASS(WUrlButton,WCoolButton,Window)

	// Variables.
	FString URL;

	// Constructor.
	WUrlButton()
	{}
	WUrlButton( WWindow* InOwner, const TCHAR* InURL, INT InId=0 )
	: WCoolButton( InOwner, InId, FDelegate(this,(TDelegate)&WUrlButton::OnClick) )
	, URL( InURL )
	{
		FrameFlags = CBFF_ShowOver | CBFF_UrlStyle | CBFF_NoCenter;
	}

	// WUrlButton interface.
	void OnClick()
	{
		guard(WUrlButton::OnClick);
		ShellExecuteW( GetActiveWindow(), TEXT("open"), *URL, TEXT(""), TEXT(""), SW_SHOWNORMAL );
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WComboBox.
-----------------------------------------------------------------------------*/

// A combo box control.
class WINDOW_API WComboBox : public WControl
{
	W_DECLARE_CLASS(WComboBox,WControl,CLASS_Transient);
	DECLARE_WINDOWSUBCLASS(WComboBox,WControl,Window)

	// Delegates.
	FDelegate DoubleClickDelegate;
	FDelegate DropDownDelegate;
	FDelegate CloseComboDelegate;
	FDelegate EditChangeDelegate;
	FDelegate EditUpdateDelegate;
	FDelegate SetFocusDelegate;
	FDelegate KillFocusDelegate;
	FDelegate SelectionChangeDelegate;
	FDelegate SelectionEndOkDelegate;
	FDelegate SelectionEndCancelDelegate;

	// Constructor.
	WComboBox()
	{}
	WComboBox( WWindow* InOwner, INT InId=0, WNDPROC InSuperProc=NULL )
	: WControl( InOwner, InId, InSuperProc?InSuperProc:SuperProc )
	{}

	// WWindow interface.
	void OpenWindow(UBOOL Visible, UBOOL Sort = FALSE, UINT _iListType = CBS_DROPDOWNLIST, INT x = 0, INT y = 0, INT xs = 0, INT ys = 0)
	{
		guard(WComboBox::OpenWindow);

		if (xs == 0)
			xs = MulDiv(64, DPIX, 96);
		if (ys == 0)
			ys = MulDiv(384, DPIY, 96);
		PerformCreateWindowEx
		(
			0,
            NULL,
            WS_CHILD | WS_VSCROLL | _iListType | (Sort?CBS_SORT:0) | (Visible?WS_VISIBLE:0),
            x, y,
			xs, ys,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);

		if (Visible)
			m_bShow = TRUE;
		SendMessageW( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		unguard;
	}
	virtual LRESULT WndProc( UINT Message, WPARAM wParam, LPARAM lParam )
	{
		if( Message==WM_KEYDOWN && (wParam==VK_UP || wParam==VK_DOWN) )
		{
			// Suppress arrow keys.
			if( Snoop )
				Snoop->SnoopKeyDown( this, wParam );
			return 1;
		}
		else return WControl::WndProc( Message, wParam, lParam );
	}

	// WControl interface.
	UBOOL InterceptControlCommand( UINT Message, WPARAM wParam, LPARAM lParam)
	{
		guard(WComboBox::InterceptControlCommand);
		if     ( HIWORD(wParam)==CBN_DBLCLK         ) {DoubleClickDelegate();        return 1;}
		else if( HIWORD(wParam)==CBN_DROPDOWN       )
		{
			INT ItemHeight      = SendMessage( *this, CB_GETITEMHEIGHT, 0, 0 );
			INT SelectionHeight = SendMessage( *this, CB_GETITEMHEIGHT, static_cast<WPARAM>(-1), 0 );
			FRect R = GetWindowRect();
			MoveWindow(R.Min.X, R.Min.Y, R.Max.X-R.Min.X, Min((INT)(GetCount()+1)*ItemHeight+SelectionHeight, MulDiv(384, DPIY, 96)), 0 );
			DropDownDelegate();
			return 1;
		}
		else if( HIWORD(wParam)==CBN_CLOSEUP        ) {CloseComboDelegate();         return 1;}
		else if( HIWORD(wParam)==CBN_EDITCHANGE     ) {EditChangeDelegate();         return 1;}
		else if( HIWORD(wParam)==CBN_EDITUPDATE     ) {EditUpdateDelegate();         return 1;}
		else if( HIWORD(wParam)==CBN_SETFOCUS       ) {SetFocusDelegate();           return 1;}
		else if( HIWORD(wParam)==CBN_KILLFOCUS      ) {KillFocusDelegate();          return 1;}
		else if( HIWORD(wParam)==CBN_SELCHANGE      ) {SelectionChangeDelegate();    return 1;}
		else if( HIWORD(wParam)==CBN_SELENDOK       ) {SelectionEndOkDelegate();     return 1;}
		else if( HIWORD(wParam)==CBN_SELENDCANCEL   ) {SelectionEndCancelDelegate(); return 1;}
		else return 0;
		unguard;
	}

	// WComboBox interface.
	virtual void AddString( const TCHAR* Str )
	{
		guard(WComboBox::AddString);
		SendMessageW( *this, CB_ADDSTRING, 0, (LPARAM)Str );
		unguard;
	}
	virtual void RemoveString(const TCHAR* Str)
	{
		guard(WComboBox::AddString);
		SendMessageW(*this, CB_DELETESTRING, 0, (LPARAM)Str);
		unguard;
	}
	virtual FString GetString( INT Index )
	{
		guard(WComboBox::GetString);
		INT Length = SendMessageW( *this, CB_GETLBTEXTLEN, Index, 0 );
		if( Length==CB_ERR )
			return TEXT("");

		TCHAR* Text = (TCHAR*)malloc((Length+1)*sizeof(TCHAR));
		SendMessage( *this, CB_GETLBTEXT, Index, (LPARAM)Text );
		FString Out = Text;
		free(Text);
		return Out;
		unguard;
	}
	virtual LRESULT GetCount()
	{
		guard(WComboBox::GetCount);
		return SendMessageW( *this, CB_GETCOUNT, 0, 0 );
		unguard;
	}
	virtual void SetCurrent( INT Index )
	{
		guard(WComboBox::SetCurrent);
		SendMessageW( *this, CB_SETCURSEL, Index, 0 );
		unguard;
	}
	virtual LRESULT GetCurrent()
	{
		guard(WComboBox::GetCurrent);
		return SendMessageW( *this, CB_GETCURSEL, 0, 0 );
		unguard;
	}
	virtual INT FindString( const TCHAR* String )
	{
		guard(WComboBox::FindString);
		INT Index = SendMessageW( *this, CB_FINDSTRING, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(String) );
		return Index!=CB_ERR ? Index : -1;
		unguard;
	}
	virtual INT FindStringExact( const TCHAR* String )
	{
		guard(WComboBox::FindString);
		INT Index = SendMessageW( *this, CB_FINDSTRINGEXACT, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(String) );
		return Index!=CB_ERR ? Index : -1;
		unguard;
	}
	void Empty()
	{
		guard(WComboBox::Empty);
		SendMessageW( *this, CB_RESETCONTENT, 0, 0 );
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WEdit.
-----------------------------------------------------------------------------*/

// A single-line or multiline edit control.
class WINDOW_API WEdit : public WControl
{
	W_DECLARE_CLASS(WEdit,WControl,CLASS_Transient);
	DECLARE_WINDOWSUBCLASS(WEdit,WControl,Window)

	// Variables.
	FDelegate ChangeDelegate;

	// Constructor.
	WEdit()
	{}
	WEdit( WWindow* InOwner, INT InId=0, WNDPROC InSuperProc=NULL )
	: WControl( InOwner, InId, InSuperProc?InSuperProc:SuperProc )
	{}

	// WWindow interface.
	void OpenWindow(UBOOL Visible, UBOOL Multiline, UBOOL ReadOnly, UBOOL HorizScroll = FALSE, UBOOL NoHideSel = FALSE, INT x = 0, INT y = 0, INT xs = 0, INT ys = 0)
	{
		guard(WEdit::OpenWindow);
		PerformCreateWindowEx
		(
			WS_EX_CLIENTEDGE,
            NULL,
            WS_CHILD | (HorizScroll?WS_HSCROLL:0) | (Visible?WS_VISIBLE:0) | ES_LEFT | (Multiline?(ES_MULTILINE|WS_VSCROLL):0) | ES_AUTOVSCROLL | ES_AUTOHSCROLL | (ReadOnly?ES_READONLY:0) | (NoHideSel?ES_NOHIDESEL:0),
            x, y,
			xs, ys,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);

		if (Visible)
			m_bShow = TRUE;
		SendMessageW( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		unguard;
	}
	UBOOL InterceptControlCommand( UINT Message, WPARAM wParam, LPARAM lParam)
	{
		guard(WEdit::InterceptControlCommand);
		if( HIWORD(wParam)==EN_CHANGE )
		{
			ChangeDelegate();
			return 1;
		}
		else return 0;
		unguard;
	}

	// WEdit interface.
	UBOOL GetReadOnly()
	{
		guard(WEdit::GetReadOnly);
		check(hWnd);
		return (GetWindowLongPtrW( *this, GWL_STYLE )&ES_READONLY)!=0;
		unguard;
	}
	void SetReadOnly( UBOOL ReadOnly )
	{
		guard(WEdit::SetReadOnly);
		check(hWnd);
		SendMessageW( *this, EM_SETREADONLY, ReadOnly, 0 );
		unguard;
	}
	LRESULT GetLineCount()
	{
		guard(WEdit::GetLineCount);
		check(hWnd);
		return SendMessageW( *this, EM_GETLINECOUNT, 0, 0 );
		unguard;
	}
	LRESULT GetLineIndex( INT Line )
	{
		guard(WEdit::GetLineIndex);
		check(hWnd);
		return SendMessageW( *this, EM_LINEINDEX, Line, 0 );
		unguard;
	}
	void GetSelection( INT& Start, INT& End )
	{
		guard(WEdit::GetSelection);
		check(hWnd);
		SendMessageW( *this, EM_GETSEL, (WPARAM)&Start, (LPARAM)&End );
		unguard;
	}
	void SetSelection( INT Start, INT End )
	{
		guard(WEdit::SetSelection);
		check(hWnd);
		SendMessageW( *this, EM_SETSEL, Start, End );
		unguard;
	}
	void SetSelectedText( const TCHAR* Text )
	{
		guard(WEdit::SetSelectedText);
		check(hWnd);
		SendMessageW( *this, EM_REPLACESEL, 1, reinterpret_cast<LPARAM>(Text) );
		unguard;
	}
	LRESULT GetModify() //UBOOL?
	{
		guard(WEdit::GetModify);
		return SendMessageW( *this, EM_GETMODIFY, 0, 0 )!=0;
		unguard;
	}
	void SetModify( UBOOL Modified )
	{
		guard(WEdit::SetModify);
		SendMessageW( *this, EM_SETMODIFY, Modified, 0 );
		unguard;
	}
	void ScrollCaret()
	{
		guard(WEdit::ScrollCaret);
		SendMessageW( *this, EM_SCROLLCARET, 0, 0 );
		unguard;
	}
	virtual FString GetText()
	{
		guard(WEdit::GetText);

		FString Str;
		Str = WWindow::GetText();

#if 0 // Marco: Disabled due to it being handeled by Int/Byte/FloatProperty themselves.
		// If the string begins with an "=" sign, we will evaluate it as a mathematical expresssion
		if( Str.Left(1) == TEXT("=") )
		{
			Str = Str.Mid(1);	// Strip off the "=" sign

			float Result;
			if( !Eval( Str, &Result ) )
				Result = 0;

			Str = FString::Printf(TEXT("%f"), Result );
		}
#endif

		return Str;

		unguard;
	}
	// Evaluate a numerical expression.
	// Returns 1 if ok, 0 if error.
	// Sets Result, or 0.0 if error.
	//
	// Operators and precedence: 1:+- 2:/% 3:* 4:^ 5:&|
	// Unary: -
	// Types: Numbers (0-9.), Hex ($0-$f)
	// Grouping: ( )
	//
	virtual UBOOL Eval( FString Str, float* pResult )
	{
		guard(WEdit::Eval);

		float Result;

		// Check for a matching number of brackets right up front.
		int Brackets = 0;
		for( int x = 0 ; x < Str.Len() ; x++ )
		{
			if( Str.Mid(x,1) == TEXT("(") )
				Brackets++;
			else
				if( Str.Mid(x,1) == TEXT(")") )
					Brackets--;
		}
		if( Brackets != 0 )
		{
			appMsgf(TEXT("Mismatched brackets."));
			Result = 0;
		}
		else
			if( !SubEval( &Str, &Result, 0 ) )
			{
				appMsgf(TEXT("Error in expression."));
				Result = 0;
			}

		*pResult = Result;

		return 1;
		unguard;
	}
	virtual UBOOL SubEval( FString* pStr, float* pResult, int Prec )
	{
		guard(WEdit::SubEval);

		FString c;
		float V, W, N;

		V = W = N = 0.0f;

		c = GrabChar(pStr);

		if((c >= TEXT("0") && c <= TEXT("9")) || c == TEXT(".")) // Number
		{
			V = 0;
			while(c >= TEXT("0") && c <= TEXT("9"))
			{
				V = V * 10 + Val(c);
				c = GrabChar(pStr);
			}
			if(c == TEXT("."))
			{
				N = 0.1f;
				c = GrabChar(pStr);
				while(c >= TEXT("0") && c <= TEXT("9"))
				{
					V = V + N * Val(c);
					N = N / 10.0f;
					c = GrabChar(pStr);
				}
			}
		}
		else if( c == TEXT("(")) // Opening parenthesis
		{
			if( !SubEval(pStr, &V, 0) )
				return 0;
			c = GrabChar(pStr);
		}
		else if( c == TEXT("-") ) // Negation
		{
			if( !SubEval(pStr, &V, 1000) )
				return 0;
			V = -V;
			c = GrabChar(pStr);
		}
		else if( c == TEXT("+")) // Positive
		{
			if( !SubEval(pStr, &V, 1000) )
				return 0;
			c = GrabChar(pStr);
		}
		else if( c == TEXT("@") ) // Square root
		{
			if( !SubEval(pStr, &V, 1000) )
				return 0;
			if( V < 0 )
			{
				appMsgf(TEXT("Can't take square root of negative number"));
				return 0;
			}
			else
				V = appSqrt(V);
			c = GrabChar(pStr);
		}
		else // Error
		{
			appMsgf(TEXT("No value recognized"));
			return 0;
		}
PrecLoop:
		if( c == TEXT("") )
		{
			*pResult = V;
			return 1;
		}
		else if( c == TEXT(")") )
		{
			*pStr = TEXT(")") + *pStr;
			*pResult = V;
			return 1;
		}
		else if( c == TEXT("+") )
		{
			if( Prec > 1 )
			{
				*pResult = V;
				*pStr = c + *pStr;
				return 1;
			}
			else
				if( SubEval(pStr, &W, 2) )
				{
					V = V + W;
					c = GrabChar(pStr);
					goto PrecLoop;
				}
		}
		else if( c == TEXT("-") )
		{
			if( Prec > 1 )
			{
				*pResult = V;
				*pStr = c + *pStr;
				return 1;
			}
			else
				if( SubEval(pStr, &W, 2) )
				{
					V = V - W;
					c = GrabChar(pStr);
					goto PrecLoop;
				}
		}
		else if( c == TEXT("/") )
		{
			if( Prec > 2 )
			{
				*pResult = V;
				*pStr = c + *pStr;
				return 1;
			}
			else
				if( SubEval(pStr, &W, 3) )
				{
					if( W == 0 )
					{
						appMsgf(TEXT("Division by zero isn't allowed"));
						return 0;
					}
					else
					{
						V = V / W;
						c = GrabChar(pStr);
						goto PrecLoop;
					}
				}
		}
		else if( c == TEXT("%") )
		{
			if( Prec > 2 )
			{
				*pResult = V;
				*pStr = c + *pStr;
				return 1;
			}
			else
				if( SubEval(pStr, &W, 3) )
				{
					if( W == 0 )
					{
						appMsgf(TEXT("Modulo zero isn't allowed"));
						return 0;
					}
					else
					{
						V = (int)V % (int)W;
						c = GrabChar(pStr);
						goto PrecLoop;
					}
				}
		}
		else if( c == TEXT("*") )
		{
			if( Prec > 3 )
			{
				*pResult = V;
				*pStr = c + *pStr;
				return 1;
			}
			else
				if( SubEval(pStr, &W, 4) )
				{
					V = V * W;
					c = GrabChar(pStr);
					goto PrecLoop;
				}
		}
		else
		{
			appMsgf(TEXT("Unrecognized Operator"));
		}

		*pResult = V;
		return 1;
		unguard;
	}
	FString GrabChar( FString* pStr )
	{
		guard(WEdit::GrabChar);

		FString GrabChar;
		if( pStr->Len() )
			do {
				GrabChar = pStr->Left(1);
				*pStr = pStr->Mid(1);
			} while( GrabChar == TEXT(" ") );
		else
			GrabChar = TEXT("");

		return GrabChar;
		unguard;
	}
	// Converts a string to it's numeric equivalent, ignoring whitespace.
	// "123  45" - becomes 12,345
	FLOAT Val( FString Value )
	{
		FLOAT RetValue = 0;

		for( int x = 0 ; x < Value.Len() ; x++ )
		{
			FString Char = Value.Mid(x, 1);

			if( Char >= TEXT("0") && Char <= TEXT("9") )
			{
				RetValue *= 10;
				RetValue += appAtoi( *Char );
			}
			else
				if( Char != TEXT(" ") )
					break;
		}

		return RetValue;
	}
};

/*-----------------------------------------------------------------------------
	WRichEdit.
-----------------------------------------------------------------------------*/

static char *GStreamPos;	// Pointer to the current position we are reading text from
static int GiStreamSz, GiStreamPos;
static DWORD CALLBACK RichEdit_StreamIn(DWORD_PTR dwCookie, LPBYTE pbBuffer, LONG cb, LONG *pcb)
{
	if( GiStreamSz - GiStreamPos < cb )
	{
		// Callback wants to read more data than what is actually available...
		::strncpy( (char*)pbBuffer, GStreamPos, GiStreamSz - GiStreamPos );
		*pcb = GiStreamSz - GiStreamPos;
	}
	else
	{
		::strncpy( (char*)pbBuffer, GStreamPos, cb );
		GStreamPos += cb;
		GiStreamPos += cb;
		*pcb = cb;
	}

	return 0;
}

static DWORD CALLBACK RichEdit_StreamOut(DWORD_PTR dwCookie, LPBYTE pbBuffer, LONG cb, LONG *pcb)
{
	::strncat( GStreamPos, (char*)pbBuffer, cb );
	*pcb = cb;

	return 0;
}

// A single-line or multiline edit control.
class WINDOW_API WRichEdit : public WControl
{
	W_DECLARE_CLASS(WRichEdit,WControl,CLASS_Transient);
	DECLARE_WINDOWSUBCLASS(WRichEdit,WControl,Window)

	// Variables.
	FDelegate ChangeDelegate;

	// Constructor.
	WRichEdit()
	{}
	WRichEdit( WWindow* InOwner, INT InId=0, WNDPROC InSuperProc=NULL )
	: WControl( InOwner, InId, InSuperProc?InSuperProc:SuperProc )
	{}

	void StreamTextIn( char* _StreamSrc, int _iSz )
	{
		guard(WRichEdit::StreamTextIn);

		if(_iSz > 25000)		GWarn->BeginSlowTask( TEXT("Loading text..."), 1, 0 );
		GStreamPos = _StreamSrc;
		GiStreamSz = _iSz;
		GiStreamPos = 0;

		EDITSTREAM es;
		es.dwCookie = 0;
		es.dwError = 0;
		es.pfnCallback = RichEdit_StreamIn;
		SendMessageW(hWnd, EM_STREAMIN, SF_RTF, (LPARAM) &es);
		if(_iSz > 25000)		GWarn->EndSlowTask();

		unguard;
	}

	void StreamTextOut( char* _StreamDst, int _iSz )
	{
		guard(WRichEdit::StreamTextIn);

		if( _iSz > 25000 )	GWarn->BeginSlowTask( TEXT("Saving text..."), 1, 0 );
		GStreamPos = _StreamDst;
		GiStreamSz = _iSz;
		GiStreamPos = 0;

		EDITSTREAM es;
		es.dwCookie = 0;
		es.dwError = 0;
		es.pfnCallback = RichEdit_StreamOut;
		SendMessageW(hWnd, EM_STREAMOUT, SF_TEXT, (LPARAM) &es);
		if( _iSz > 25000 )	GWarn->EndSlowTask();

		unguard;
	}

	// WWindow interface.
	void OpenWindow( UBOOL Visible, UBOOL ReadOnly )
	{
		guard(WRichEdit::OpenWindow);
		PerformCreateWindowEx
		(
			WS_EX_CLIENTEDGE,
			NULL,
			WS_CHILD | (Visible?WS_VISIBLE:0) | ES_MULTILINE | ES_NOHIDESEL | ES_SUNKEN | ES_SAVESEL | WS_HSCROLL | WS_VSCROLL,
			0, 0,
			0, 0,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);
		SendMessageW( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		// stijn: This needs to be set explicitly folder older versions of Windows as they use a 32K character limit otherwise
		SetTextLimit(0x7FFFFFFF);
		unguard;
	}
	UBOOL InterceptControlCommand( UINT Message, WPARAM wParam, LPARAM lParam)
	{
		guard(WRichEdit::InterceptControlCommand);
		if( HIWORD(wParam)==EN_CHANGE )
		{
			ChangeDelegate();
			return 1;
		}
		else return 0;
		unguard;
	}

	// WRichEdit interface.
	void SetTextLimit( int _Limit )
	{
		guard(WRichEdit::SetTextLimit);
		SendMessageW( *this, EM_EXLIMITTEXT, 0, _Limit );
		unguard;
	}
	void SetReadOnly( UBOOL ReadOnly )
	{
		guard(WRichEdit::SetReadOnly);
		check(hWnd);
		SendMessageW( *this, EM_SETREADONLY, ReadOnly, 0 );
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WTerminal.
-----------------------------------------------------------------------------*/

// Base class of terminal edit windows.
class WINDOW_API WTerminalBase : public WWindow
{
	W_DECLARE_ABSTRACT_CLASS(WTerminalBase,WWindow,CLASS_Transient);
	DECLARE_WINDOWCLASS(WTerminalBase,WWindow,Window)

	// Constructor.
	WTerminalBase()
	{}
	WTerminalBase( FName InPersistentName, WWindow* InOwnerWindow )
	: WWindow( InPersistentName, InOwnerWindow )
	{}

	// WTerminalBase interface.
	virtual void HitEnter() _VF_BASE;

	virtual void SwitchFocus(TCHAR Ch) _VF_BASE;
};

// A terminal edit window.
class WEditTerminal : public WEdit
{
public:
	// Variables.
	WTerminalBase* OwnerTerminal{};

	// Constructor.
	WEditTerminal( WTerminalBase* InOwner=NULL )
	: WEdit( InOwner )
	, OwnerTerminal( InOwner )
	{}

	~WEditTerminal() noexcept(false)
	{
		MaybeDestroy();
	}

	// WWindow interface.
	void OnChar( TCHAR Ch )
	{
		if (Ch != 3)
		{
			OwnerTerminal->SwitchFocus(Ch);
			throw TEXT("NoRoute");
		}
	}
	void OnKeyDown(TCHAR Ch)
	{
	}
	void OnRightButtonDown(INT X, INT Y)
	{
		throw TEXT("NoRoute");
	}
	void OnPaste()
	{
		throw TEXT("NoRoute");
	}
	void OnUndo()
	{
		throw TEXT("NoRoute");
	}
};

class WTerminalCombo : public WComboBox
{
public:
	// Variables.
	WTerminalBase* OwnerTerminal;

	// Constructor.
	WTerminalCombo(WTerminalBase* InOwner = NULL)
		: WComboBox(InOwner)
		, OwnerTerminal(InOwner)
	{}

	~WTerminalCombo() noexcept(false)
	{
		MaybeDestroy();
	}

	// WWindow interface.
	LRESULT WndProc(UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_KEYDOWN)
		{
			// Suppress arrow keys.
			if (wParam == VK_UP || wParam == VK_DOWN)
				return 1;
			if (wParam == VK_RETURN)
			{
				OwnerTerminal->HitEnter();
				return 1;
			}
		}
		if (Message == CB_GETDROPPEDSTATE)
			return 1;
		return WControl::WndProc(Message, wParam, lParam);
	}
};

// A terminal window.
class WINDOW_API WTerminal : public WTerminalBase, public FOutputDevice
{
	W_DECLARE_CLASS(WTerminal,WTerminalBase,CLASS_Transient);
	DECLARE_WINDOWCLASS(WTerminal,WTerminalBase,Window)

	// Variables.
	WEditTerminal Display;
	WTerminalCombo EditBox;
	FExec* Exec{};
	INT MaxLines{}, SlackLines{};
	UBOOL Shown{};
	static FThreadLock TerminalMutex;

	// Structors.
	WTerminal()
	{}
	WTerminal( FName InPersistentName, WWindow* InOwnerWindow )
	:	WTerminalBase	( InPersistentName, InOwnerWindow )
	,	Display			( this )
	,	EditBox			( this )
	,	Exec			( NULL )
	,	MaxLines		( 256 )
	,	SlackLines		( 64 )
	,	Shown			( 0 )
	{
	}

	// FOutputDevice interface.
	void Serialize(const TCHAR* Data, EName MsgType);

	// WWindow interface.
	void OnShowWindow( UBOOL bShow )
	{
		guard(WTerminal::OnShowWindow);
		Shown = bShow;
		unguard;
	}
	void OnCreate();
	void OpenWindow(UBOOL bMdi = 0, UBOOL AppWindow = 0);
	void OnSetFocus( HWND hWndLoser )
	{
		guard(WTerminal::OnSetFocus);
		WWindow::OnSetFocus(hWndLoser);
		SetFocus(EditBox);
		unguard;
	}
	void OnSize(DWORD Flags, INT NewX, INT NewY);

	// WTerminal interface.
	void HitEnter();

	void SetExec( FExec* InExec )
	{
		Exec = InExec;
	}

	void SwitchFocus(TCHAR Ch)
	{
		guard(WTerminal::SwitchFocus);
		SetFocus(EditBox);
		PostMessage(EditBox, WM_CHAR, Ch, 0L);
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WLog.
-----------------------------------------------------------------------------*/

// A log window.
class WINDOW_API WLog : public WTerminal
{
	W_DECLARE_CLASS(WLog,WTerminal,CLASS_Transient);
	DECLARE_WINDOWCLASS(WLog,WTerminal,Window)

	// Variables.
	UINT NidMessage{};
	FArchive*& LogAr;
	FString LogFilename;

	// Functions.
	WLog()
	:LogAr(LogAr)
	{}
	WLog( const TCHAR* InLogFilename, FArchive*& InLogAr, FName InPersistentName, WWindow* InOwnerWindow=NULL )
	: WTerminal( InPersistentName, InOwnerWindow )
	, NidMessage( RegisterWindowMessageW( TEXT("UnrealNidMessage")) )
	, LogAr( InLogAr )
	, LogFilename( InLogFilename )
	{}
	void OnCreate();
	void SetText(const TCHAR* Text);
	void OnShowWindow(UBOOL bShow);
	void OpenWindow(UBOOL bShow, UBOOL bMdi);
	void OnDestroy();
	void OnCopyData(HWND hWndSender, COPYDATASTRUCT* CD);
	void OnClose();
	void OnCommand(INT Command);
	LRESULT WndProc(UINT Message, WPARAM wParam, LPARAM lParam);
};

/*-----------------------------------------------------------------------------
	WDialog.
-----------------------------------------------------------------------------*/

// A dialog window, always based on a Visual C++ dialog template.
class WINDOW_API WDialog : public WWindow
{
	W_DECLARE_ABSTRACT_CLASS(WDialog,WWindow,CLASS_Transient);

	BOOL bModal{};

	// Constructors.
	WDialog()
	{}
	WDialog( FName InPersistentName, INT InDialogId, WWindow* InOwnerWindow=NULL )
	: WWindow( InPersistentName, InOwnerWindow )
	{
		ControlId = InDialogId;
	}

	// WDialog interface.
	LRESULT CallDefaultProc( UINT Message, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	}
	virtual INT DoModal( HINSTANCE hInst=hInstanceWindow )
	{
		guard(WDialog::DoModal);
		bModal = TRUE;
		check(hWnd==NULL);
		_Windows.AddItem( this );
		ModalCount++;
		INT Result = DialogBoxParamW(hInst/*!!*/,MAKEINTRESOURCEW(ControlId),OwnerWindow?OwnerWindow->hWnd:NULL,(DLGPROC)StaticWndProc,(LPARAM)this);
		ModalCount--;
		return Result;
		unguard;
	}
	void OpenChildWindow( INT InControlId, UBOOL Visible )
	{
		guard(WDialog::OpenChildWindow);
		check(!hWnd);
		_Windows.AddItem( this );
		HWND hWndParent = InControlId ? GetDlgItem(OwnerWindow->hWnd,InControlId) : OwnerWindow ? OwnerWindow->hWnd : NULL;
		HWND hWndCreated = CreateDialogParamW(hInstanceWindow/*!!*/,MAKEINTRESOURCEW(ControlId),hWndParent,(DLGPROC)StaticWndProc,(LPARAM)this);
		check(hWndCreated);
		check(hWndCreated==hWnd);
		Show( Visible );
		unguard;
	}
	static BOOL CALLBACK LocalizeTextEnumProc( HWND hInWmd, LPARAM lParam )
	{
		guard(WDialog::LocalizeTextEnumProc);
		FString String;
		TCHAR** Temp = (TCHAR**)lParam;
		TCHAR Ch[1024]=TEXT("");
		SendMessage( hInWmd, WM_GETTEXT, ARRAY_COUNT(Ch), (LPARAM)Ch );
		String = Ch;
		if( FString(String).Left(4)==TEXT("IDC_") )
			SendMessageW( hInWmd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(*LineFormat(Localize(Temp[0],*String,Temp[1]))) );
		else if( String==TEXT("IDOK") )
			SendMessageW( hInWmd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(*LineFormat(LocalizeGeneral("OkButton",TEXT("Window")))) );
		else if( String==TEXT("IDCANCEL") )
			SendMessageW( hInWmd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(*LineFormat(LocalizeGeneral("CancelButton",TEXT("Window")))) );
		SendMessageW( hInWmd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(1,0) );
		return 1;
		unguard;
	}
	virtual void LocalizeText( const TCHAR* Section, const TCHAR* Package=GPackage )
	{
		guard(WDialog::LocalizeText);
		const TCHAR* Temp[3];
		Temp[0] = Section;
		Temp[1] = Package;
		Temp[2] = (TCHAR*)this;
		EnumChildWindows( *this, LocalizeTextEnumProc, (LPARAM)Temp );
		LocalizeTextEnumProc( hWnd, (LPARAM)Temp );
		unguard;
	}
	virtual void OnInitDialog()
	{
		guard(WDialog::OnInitDialog);
		WWindow::OnInitDialog();
		SendMessageW( hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(1,0) );
		INT i;
		for( i=0; i<Controls.Num(); i++ )
		{
			// Bind all child controls.
			WControl* Control = Controls(i);
			check(!Control->hWnd);
			Control->hWnd = GetDlgItem( *this, Control->ControlId );
			check(Control->hWnd);
			_Windows.AddItem(Control);
			Control->WindowDefWndProc = (WNDPROC)GetWindowLongPtr(Control->hWnd, GWLP_WNDPROC);
			SetWindowLongPtr( Control->hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WWindow::StaticWndProc) );
			SetWindowPos(Control->hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED); // Regarding MSDN: If you have changed certain window data using SetWindowLong, you must call SetWindowPos for the changes to take effect. Use the following combination for uFlags: SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED.
			//warning: Don't set GWL_HINSTANCE, it screws up subclassed edit controls in Win95.
		}
		for( i=0; i<Controls.Num(); i++ )
		{
			// Send create to all controls.
			Controls(i)->OnCreate();
		}
		LocalizeText( *FString::Printf(TEXT("IDDIALOG_%ls"), *PersistentName), GetPackageName() );
		unguard;
	}
	void OnClose()
	{
		guard(WDialog::OnClose);
		// FIXME: This ForceParentFocus/OnClose call makes the application stop responding if you close a modal dialog
		if (!bModal)
		{
			ForceParentFocus();
			WWindow::OnClose();
		}
		unguard;
	}
	void Show(UBOOL Show)
	{
		guard(WDialog::Show);
		if(!Show)
			ForceParentFocus();
		WWindow::Show(Show);
		unguard;
	}
	void EndDialog( INT Result )
	{
		if (!bModal)
			ForceParentFocus();
		::EndDialog( hWnd, Result );
	}
	void EndDialogTrue()
	{
		EndDialog( 1 );
	}
	void EndDialogFalse()
	{
		EndDialog( 0 );
	}
	virtual void OnCommand(INT Command)
	{
	}
};

/*-----------------------------------------------------------------------------
	WPasswordDialog.
-----------------------------------------------------------------------------*/

// A password dialog box.
class WINDOW_API WPasswordDialog : public WDialog
{
	W_DECLARE_CLASS(WPasswordDialog,WDialog,CLASS_Transient);
	DECLARE_WINDOWCLASS(WPasswordDialog,WDialog,Window)

	// Controls.
	WCoolButton OkButton;
	WCoolButton CancelButton;
	WEdit Name;
	WEdit Password;
	WLabel Prompt;

	// Output.
	FString ResultName;
	FString ResultPassword;

	// Constructor.
	WPasswordDialog()
	: WDialog	  ( TEXT("PasswordDialog"), IDDIALOG_Password )
	, OkButton    ( this, IDOK,     FDelegate(this,(TDelegate)&WDialog::EndDialogTrue) )
	, CancelButton( this, IDCANCEL, FDelegate(this,(TDelegate)&WDialog::EndDialogFalse) )
	, Name		  ( this, IDEDIT_Name )
	, Password	  ( this, IDEDIT_Password )
	, Prompt      ( this, IDLABEL_Prompt )
	{}

	// WDialog interface.
	void OnInitDialog()
	{
		guard(WPasswordDialog::OnInitDialog);
		WDialog::OnInitDialog();
		SetText( *LocalizeQuery(TEXT("PassDlg"),TEXT("Core")) );
		Prompt.SetText( *LocalizeQuery(TEXT("PassPrompt"),TEXT("Core")) );
		Name.SetText( TEXT("") );
		Password.SetText( TEXT("") );
		SetFocus( Name );
		unguard;
	}
	void OnDestroy()
	{
		guard(WPasswordDialog::OnDestroy);
		ResultName     = Name.GetText();
		ResultPassword = Password.GetText();
		WDialog::OnDestroy();
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WTextScrollerDialog.
-----------------------------------------------------------------------------*/

// A text scroller dialog box.
class WINDOW_API WTextScrollerDialog : public WDialog
{
	W_DECLARE_CLASS(WTextScrollerDialog,WDialog,CLASS_Transient);
	DECLARE_WINDOWCLASS(WTextScrollerDialog,WDialog,Window)

	// Controls.
	WEdit TextEdit;
	WCoolButton OkButton;
	FString Caption, Message;

	// Constructor.
	WTextScrollerDialog()
	{}
	WTextScrollerDialog( const TCHAR* InCaption, const TCHAR* InMessage )
	: WDialog	( TEXT("TextScrollerDialog"), IDDIALOG_TextScroller )
	, TextEdit  ( this, IDEDIT_TextEdit )
	, OkButton  ( this, IDOK, FDelegate(this,(TDelegate)&WDialog::EndDialogTrue) )
	, Caption   ( InCaption )
	, Message   ( InMessage )
	{}

	// WDialog interface.
	void OnInitDialog()
	{
		guard(WTextScrollerDialog::OnInitDialog);
		WDialog::OnInitDialog();
		HDC hDC = GetDC(hWnd);
		ReleaseDC(hWnd,hDC);
		TextEdit.SetFont( hFontText );
		SetText( *Caption );
		TextEdit.SetText( *Message );
		SetFocus( OkButton );
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WTrackBar.
-----------------------------------------------------------------------------*/

// A non-interactive label control.
class WINDOW_API WTrackBar : public WControl
{
	W_DECLARE_CLASS(WTrackBar,WControl,CLASS_Transient)
	DECLARE_WINDOWSUBCLASS(WTrackBar,WControl,Window)

	// Delegates.
	FDelegate ThumbTrackDelegate;
	FDelegate ThumbPositionDelegate;

	// Constructor.
	WTrackBar()
	{}
	WTrackBar( WWindow* InOwner, INT InId=0, WNDPROC InSuperProc=NULL )
	: WControl( InOwner, InId, InSuperProc?InSuperProc:SuperProc )
	{}

	// WWindow interface.
	void OpenWindow( UBOOL Visible )
	{
		guard(WTrackBar::OpenWindow);
		PerformCreateWindowEx
		(
			WS_EX_CLIENTEDGE,
            NULL,
            WS_CHILD | TBS_HORZ | TBS_AUTOTICKS | TBS_BOTTOM | (Visible?WS_VISIBLE:0),
            0, 0,
			0, 0,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);
		if (Visible)
			m_bShow = TRUE;
		unguard;
	}

	// WControl interface.
	UBOOL InterceptControlCommand( UINT Message, WPARAM wParam, LPARAM lParam)
	{
		guard(WTrackBar::InterceptControlCommand);
		if     ( Message==WM_HSCROLL && LOWORD(wParam)==TB_THUMBTRACK ) {ThumbTrackDelegate();    return 1;}
		else if( Message==WM_HSCROLL                                  ) {ThumbPositionDelegate(); return 1;}
		else return 0;
		unguard;
	}

	// WTrackBar interface.
	void SetTicFreq( INT TicFreq )
	{
		guard(WTrackBar::SetTicFreq);
		SendMessageW( *this, TBM_SETTICFREQ, TicFreq, 0 );
		unguard;
	}
	void SetRange( INT Min, INT Max )
	{
		guard(WTrackBar::SetRange);
		SendMessageW( *this, TBM_SETRANGE, 1, MAKELONG(Min,Max) );
		unguard;
	}
	void SetPos( INT Pos )
	{
		guard(WTrackBar::SetPos);
		SendMessageW( *this, TBM_SETPOS, 1, Pos );
		unguard;
	}
	LRESULT GetPos()
	{
		guard(WTrackBar::GetPos);
		return SendMessageW( *this, TBM_GETPOS, 0, 0 );
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WTrackBar.
-----------------------------------------------------------------------------*/

// A non-interactive label control.
class WINDOW_API WProgressBar : public WControl
{
	W_DECLARE_CLASS(WProgressBar,WControl,CLASS_Transient);
	DECLARE_WINDOWSUBCLASS(WProgressBar,WControl,Window)

	// Variables.
	INT Percent{};

	// Constructor.
	WProgressBar()
	{}
	WProgressBar( WWindow* InOwner, INT InId=0, WNDPROC InSuperProc=NULL )
	: WControl( InOwner, InId, InSuperProc?InSuperProc:SuperProc )
	, Percent( 0 )
	{}

	// WWindow interface.
	void OpenWindow( UBOOL Visible )
	{
		guard(WProgressBar::OpenWindow);
		PerformCreateWindowEx
		(
			WS_EX_CLIENTEDGE,
            NULL,
            WS_CHILD | (Visible?WS_VISIBLE:0),
            0, 0,
			0, 0,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);
		SendMessageW( *this, PBM_SETRANGE, 0, 100 );
		unguard;
	}

	// WProgressBar interface.
	void SetProgress( INT InCurrent, INT InMax )
	{
		INT InPercent = (SQWORD)InCurrent*100/Max(InMax,1);
		if( InPercent!=Percent )
			SendMessageW( *this, PBM_SETPOS, InPercent, 0 );
		Percent = InPercent;
	}
};

/*-----------------------------------------------------------------------------
	WListBox.
-----------------------------------------------------------------------------*/

// A list box.
class WINDOW_API WListBox : public WControl
{
	W_DECLARE_CLASS(WListBox,WControl,CLASS_Transient);
	DECLARE_WINDOWSUBCLASS(WListBox,WControl,Window)

	// Delegates.
	FDelegate DoubleClickDelegate;
	FDelegate SelectionChangeDelegate;
	FDelegate SelectionCancelDelegate;
	FDelegate SetFocusDelegate;
	FDelegate KillFocusDelegate;
	FDelegate RightClickDelegate;

	BOOL m_bMultiSel{};

	// Constructor.
	WListBox()
	{}
	WListBox( WWindow* InOwner, INT InId=0, WNDPROC InSuperProc=NULL )
	: WControl( InOwner, InId, InSuperProc?InSuperProc:SuperProc )
	{
		check(OwnerWindow);
		m_bMultiSel = FALSE;
	}

	// WWindow interface.
	void OpenWindow( UBOOL Visible, UBOOL Integral, UBOOL MultiSel, UBOOL OwnerDrawVariable, UBOOL Sort = 0, DWORD dwExtraStyle = 0  )
	{
		guard(WListBox::OpenWindow);
		m_bMultiSel = MultiSel;
		// stijn: UE1 originally used the LBS_OWNERDRAWVARIABLE style for the OwnerDrawVariable==1 case.
		// Unfortunately, the LBS_OWNERDRAWVARIABLE style has been totally bugged since at least Windows 2000.
		// When using the mouse wheel to scroll in an LBS_OWNERDRAWVARIABLE listbox, the scrolling animation appears to happen in the opposite direction.
		// This is extremely confusing and annoying so I changed this to LBS_OWNERDRAWFIXED.
		// The only real side effect of this change is that we now have to provide a correct height for the list box items by responding to the WM_MEASUREITEM Windows message.
		PerformCreateWindowEx
		(
			WS_EX_CLIENTEDGE,
            NULL,
            WS_CHILD | WS_BORDER | WS_VSCROLL | WS_CLIPCHILDREN | LBS_NOTIFY | (Visible?WS_VISIBLE:0) | (Integral?0:LBS_NOINTEGRALHEIGHT)
			| (MultiSel?(LBS_EXTENDEDSEL|LBS_MULTIPLESEL):0) | (OwnerDrawVariable?LBS_OWNERDRAWFIXED:0) | (Sort?LBS_SORT:0) | dwExtraStyle,
            0, 0,
			0, 0,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);

		SendMessageW( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		unguard;
	}

	// WControl interface.
	UBOOL InterceptControlCommand( UINT Message, WPARAM wParam, LPARAM lParam)
	{
		guard(WListBox::InterceptControlCommand);
		if     ( HIWORD(wParam)==LBN_DBLCLK   ) {DoubleClickDelegate();     return 1;}
		else if( HIWORD(wParam)==LBN_SELCHANGE) {SelectionChangeDelegate(); return 1;}
		else if( HIWORD(wParam)==LBN_SELCANCEL) {SelectionCancelDelegate(); return 1;}
		else if( HIWORD(wParam)==LBN_SETFOCUS)  {SetFocusDelegate();        return 1;}
		else if( HIWORD(wParam)==LBN_KILLFOCUS) {KillFocusDelegate();       return 1;}
		else return 0;
		unguard;
	}

	// WListBox interface.
	FString GetString( INT Index )
	{
		guard(WListBox::GetString);
		INT Length = SendMessageW(*this,LB_GETTEXTLEN,Index,0);
		if (Length == LB_ERR)
			return TEXT("");

		TCHAR* Ch = (TCHAR*)malloc((Length+1)*sizeof(TCHAR));
		SendMessage( *this, LB_GETTEXT, Index, (LPARAM)Ch );
		Ch[Length] = 0;
		FString Out = Ch;
		free(Ch);
		return Out;
		unguard;
	}
	LRESULT GetItemData( INT Index )
	{
		guard(WListBox::GetItemData);
		return SendMessageW( *this, LB_GETITEMDATA, Index, 0 );
		unguard;
	}
	void SetItemData( INT Index, LPARAM Value )
	{
		guard(WListBox::SetItemData);
		SendMessageW( *this, LB_SETITEMDATA, Index, Value );
		unguard;
	}
	LRESULT GetCurrent()
	{
		guard(WListBox::GetCurrent);
		return SendMessageW( *this, LB_GETCARETINDEX, 0, 0 );
		unguard;
	}
	void ClearSel()
	{
		guard(WListBox::ClearSel);
		SendMessageW( *this, LB_SETSEL, FALSE, -1 );
		unguard;
	}
	void SetCurrent( INT Index, UBOOL bScrollIntoView )
	{
		guard(WListBox::SetCurrent);
		if( m_bMultiSel )
		{
			ClearSel();
			SendMessageW( *this, LB_SETSEL, TRUE, Index );
			SendMessageW( *this, LB_SETCARETINDEX, Index, bScrollIntoView );
		}
		else
		{
			SendMessageW( *this, LB_SETCURSEL, Index, 0 );
			SendMessageW( *this, LB_SETCARETINDEX, Index, bScrollIntoView );
		}
		unguard;
	}
	LRESULT GetTop()
	{
		guard(WListBox::GetTop);
		return SendMessageW( *this, LB_GETTOPINDEX, 0, 0 );
		unguard;
	}
	void SetTop( INT Index )
	{
		guard(WListBox::SetTop);
		SendMessageW( *this, LB_SETTOPINDEX, Index, 0 );
		unguard;
	}
	void DeleteString( INT Index )
	{
		guard(WListBox::DeleteItem);
		SendMessageW( *this, LB_DELETESTRING, Index, 0 );
		unguard;
	}
	LRESULT GetCount()
	{
		guard(WListBox::GetCount);
		return SendMessageW( *this, LB_GETCOUNT, 0, 0 );
		unguard;
	}
	LRESULT GetItemHeight( INT Index )
	{
		guard(WListBox::GetItemHeight);
		return SendMessageW( *this, LB_GETITEMHEIGHT, Index, 0 );
		unguard;
	}
	void SetItemHeight(INT Height)
	{
		SendMessageW(*this, LB_SETITEMHEIGHT, 0, Height);
	}
	INT ItemFromPoint( FPoint P )
	{
		guard(WListBox::ItemFromPoint);
		DWORD Result=SendMessageW( *this, LB_ITEMFROMPOINT, 0, MAKELPARAM(P.X,P.Y) );
		return HIWORD(Result) ? -1 : LOWORD(Result);
		unguard;
	}
	FRect GetItemRect( INT Index )
	{
		guard(WListBox::GetItemRect);
		RECT R; R.left=R.right=R.top=R.bottom=0;
		SendMessageW( *this, LB_GETITEMRECT, Index, (LPARAM)&R );
		return R;
		unguard;
	}
	void Empty()
	{
		guard(WListBox::Empty);
		SendMessageW( *this, LB_RESETCONTENT, 0, 0 );
		unguard;
	}
	LRESULT GetSelected( INT Index ) //UBOOL?
	{
		guard(WListBox::GetSelected);
		return SendMessageW( *this, LB_GETSEL, Index, 0 );
		unguard;
	}
	LRESULT GetSelectedItems( INT Count, INT* Buffer )
	{
		guard(WListBox::GetSelectedItems);
		return SendMessageW( *this, LB_GETSELITEMS, (WPARAM)Count, (LPARAM)Buffer );
		unguard;
	}
	LRESULT GetSelectedCount()
	{
		guard(WListBox::GetSelectedCount);
		return SendMessageW( *this, LB_GETSELCOUNT, 0, 0 );
		unguard;
	}

	// Accessing as strings.
	LRESULT AddString( const TCHAR* C )
	{
		guard(WListBox::AddString);
		return SendMessageW( *this, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(C) );
		unguard;
	}
	void InsertString( INT Index, const TCHAR* C )
	{
		guard(WListBox::InsertString);
		SendMessageW( *this, LB_INSERTSTRING, Index, reinterpret_cast<LPARAM>(C) );
		unguard;
	}
	LRESULT FindStringExact( const TCHAR* C )
	{
		guard(WListBox::FindStringExact);
		return SendMessageW( *this, LB_FINDSTRINGEXACT, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(C));
		unguard;
	}
	LRESULT FindString( const TCHAR* C )
	{
		guard(WListBox::FindString);
		return SendMessageW( *this, LB_FINDSTRING, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(C));
		unguard;
	}
	LRESULT FindExactString( const TCHAR* C )
	{
		guard(WListBox::FindString);
		return SendMessageW( *this, LB_FINDSTRINGEXACT, -1, (LPARAM)C );
		unguard;
	}
	INT FindStringChecked( const TCHAR* C )
	{
		guard(WListBox::FindStringChecked);
		INT Result = SendMessageW( *this, LB_FINDSTRING, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(C));
		check(Result!=LB_ERR);
		return Result;
		unguard;
	}
	void InsertStringAfter( const TCHAR* Existing, const TCHAR* New )
	{
		guard(WListBox::InsertStringAfter);
		InsertString( FindStringChecked(Existing)+1, New );
		unguard;
	}

	// Accessing as pointers.
	LRESULT AddItem( const void* C )
	{
		guard(WListBox::AddItem);
		return SendMessageW( *this, LB_ADDSTRING, 0, (LPARAM)C );
		unguard;
	}
	void InsertItem( INT Index, const void* C )
	{
		guard(WListBox::AddInsert);
		SendMessageW( *this, LB_INSERTSTRING, Index, (LPARAM)C );
		unguard;
	}
	LRESULT FindItem( const void* C )
	{
		guard(WListBox::FindItem);
		return SendMessageW( *this, LB_FINDSTRING, static_cast<WPARAM>(-1), (LPARAM)C );
		unguard;
	}
	INT FindItemChecked( const void* C )
	{
		guard(WListBox::FindItemChecked);
		INT Result = SendMessageW( *this, LB_FINDSTRING, static_cast<WPARAM>(-1), (LPARAM)C );
		check(Result!=LB_ERR);
		if(Result == LB_ERR)
			appErrorf(TEXT("Find %ls"), C);
		return Result;
		unguard;
	}
	void InsertItemAfter( const void* Existing, const void* New )
	{
		guard(WListBox::InsertStringAfter);
		InsertItem( FindItemChecked(Existing)+1, New );
		unguard;
	}
	virtual void OnRightButtonDown(INT X, INT Y)
	{
		guard(WListBox::OnRightButtonDown);
		INT Index = ItemFromPoint(GetCursorPos());
		if (Index != INDEX_NONE)
			SetCurrent(Index, FALSE);
		RightClickDelegate();
		unguard;
	}
	void AutoResizeColumns()
	{
		guard(WListBox::AutoResizeColumns);
		INT ItemCount = GetCount();
		INT MaxColumnWidth = 0;
		HDC dc = GetDC(*this);
		for (INT i = 0; i < ItemCount; ++i)
		{
			FString Text = GetString(i);
			if (Text.Len() > 0)
			{
				SIZE Size;
				GetTextExtentPoint32W(dc, *Text, Text.Len(), &Size);
				if (Size.cx > MaxColumnWidth)
					MaxColumnWidth = Size.cx;
			}
		}
		if (MaxColumnWidth > 0)
			SendMessageW(*this, LB_SETCOLUMNWIDTH, (WPARAM)MaxColumnWidth, 0);
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WCheckListBox.
-----------------------------------------------------------------------------*/

// A list box where each item has a checkbox beside it.
class WINDOW_API WCheckListBox : public WListBox
{
	W_DECLARE_CLASS(WCheckListBox,WListBox,CLASS_Transient);
	DECLARE_WINDOWSUBCLASS(WCheckListBox,WListBox,Window)

	HBITMAP hbmOff{}, hbmOn{};
	int bOn{};

	// Constructor.
	WCheckListBox()
	{}
	WCheckListBox( WWindow* InOwner, INT InId=0, WNDPROC InSuperProc=NULL )
		: WListBox( InOwner, InId )
	{
		check(OwnerWindow);
		hbmOff = (HBITMAP)LoadImageA( hInstance, MAKEINTRESOURCEA(IDBM_CHECKBOX_OFF), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );	check(hbmOff);
		hbmOn = (HBITMAP)LoadImageA( hInstance, MAKEINTRESOURCEA(IDBM_CHECKBOX_ON), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );	check(hbmOn);
		ScaleImageAndReplace(hbmOn);
		ScaleImageAndReplace(hbmOff);
		bOn = 0;
	}

	// WWindow interface.
	void OpenWindow( UBOOL Visible, UBOOL Integral, UBOOL MultiSel, UBOOL Sort = FALSE, DWORD dwExtraStyle = 0 )
	{
		guard(WCheckListBox::OpenWindow);
		m_bMultiSel = MultiSel;
		PerformCreateWindowEx
		(
			WS_EX_CLIENTEDGE,
            NULL,
            WS_CHILD | WS_BORDER | WS_VSCROLL | WS_CLIPCHILDREN | LBS_NOTIFY | (Visible?WS_VISIBLE:0) | (Integral?0:LBS_NOINTEGRALHEIGHT)
				| (MultiSel?(LBS_EXTENDEDSEL|LBS_MULTIPLESEL):0) | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | (Sort?LBS_SORT:0) | dwExtraStyle,
            0, 0,
			0, 0,
            *OwnerWindow,
            (HMENU)ControlId,
            hInstance
		);

		BITMAP bm;
		GetObjectW(hbmOn, sizeof(bm), &bm);
		SetItemHeight(bm.bmHeight + 6);

		SendMessageW( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		unguard;
	}
	void OnDestroy()
	{
		guard(WCheckListBox::OnDestroy);
		WListBox::OnDestroy();
		DeleteObject(hbmOff);
		DeleteObject(hbmOn);
		unguard;
	}
	void OnDrawItem( DRAWITEMSTRUCT* Item )
	{
		guard(WCheckListBox::OnDrawItem);

		if(((LONG)(Item->itemID) >= 0)
			&& (Item->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
		{
			BOOL fDisabled = !IsWindowEnabled(hWnd);

			COLORREF newTextColor = fDisabled ? RGB(0x80, 0x80, 0x80) : GetSysColor(COLOR_WINDOWTEXT);  // light gray
			COLORREF oldTextColor = SetTextColor(Item->hDC, newTextColor);

			COLORREF newBkColor = GetSysColor(COLOR_WINDOW);
			COLORREF oldBkColor = SetBkColor(Item->hDC, newBkColor);

			if (newTextColor == newBkColor)
				newTextColor = RGB(0xC0, 0xC0, 0xC0);   // dark gray

			if (!fDisabled && ((Item->itemState & ODS_SELECTED) != 0))
			{
				SetTextColor(Item->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
				SetBkColor(Item->hDC, GetSysColor(COLOR_HIGHLIGHT));
			}

			FString strText = GetString(Item->itemID);
			ExtTextOutA(Item->hDC, Item->rcItem.left + MulDiv(18, DPIX, 96),
				Item->rcItem.top + MulDiv(2, DPIY, 96),
				ETO_OPAQUE, &(Item->rcItem), appToAnsi( *strText ), strText.Len(), NULL);

			SetTextColor(Item->hDC, oldTextColor);
			SetBkColor(Item->hDC, oldBkColor);

			// BITMAP
			//
			HDC hdcMem = CreateCompatibleDC(Item->hDC);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, ( GetItemData( Item->itemID ) ? hbmOn : hbmOff));

			BitBlt(Item->hDC,
				Item->rcItem.left + MulDiv(2, DPIX, 96), Item->rcItem.top + MulDiv(3, DPIY, 96),
				MulDiv(13, DPIX, 96), MulDiv(13, DPIY, 96),
				hdcMem,
				0, 0,
				SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);
		}

		if((Item->itemAction & ODA_FOCUS) != 0)
			DrawFocusRect(Item->hDC, &(Item->rcItem));
		unguard;
	}
	void OnLeftButtonDown(INT X, INT Y)
	{
		guard(WCheckListBox::OnLeftButtonDown);
		WListBox::OnLeftButtonDown(X, Y);

		if( X <= 16 )
		{
			FPoint pt(X, Y);
			int Item = ItemFromPoint( pt );
			int Data = (int)GetItemData( Item );
			Data = Data ? 0 : 1;
			SetItemData( Item, Data );
		}

		check(OwnerWindow);
		PostMessageW( OwnerWindow->hWnd, WM_COMMAND, WM_WCLB_UPDATE_VISIBILITY, 0L );
		InvalidateRect( hWnd, NULL, FALSE );
		unguard;
	}
};

// A list box where each item has 2x checkboxes beside it.
class WINDOW_API WDblCheckListBox : public WCheckListBox
{
	W_DECLARE_CLASS(WCheckLisWDblCheckListBoxtBox, WCheckListBox, CLASS_Transient);
	DECLARE_WINDOWSUBCLASS(WDblCheckListBox, WCheckListBox, Window)

	// Constructor.
	WDblCheckListBox(WWindow* InOwner, INT InId = 0, WNDPROC InSuperProc = NULL)
		: WCheckListBox(InOwner, InId, InSuperProc)
	{}

	void OnDrawItem(DRAWITEMSTRUCT* Item)
	{
		guard(WDblCheckListBox::OnDrawItem);

		if (((LONG)(Item->itemID) >= 0)
			&& (Item->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
		{
			BOOL fDisabled = !IsWindowEnabled(hWnd);

			COLORREF newTextColor = fDisabled ? RGB(0x80, 0x80, 0x80) : GetSysColor(COLOR_WINDOWTEXT);  // light gray
			COLORREF oldTextColor = SetTextColor(Item->hDC, newTextColor);

			COLORREF newBkColor = GetSysColor(COLOR_WINDOW);
			COLORREF oldBkColor = SetBkColor(Item->hDC, newBkColor);

			if (newTextColor == newBkColor)
				newTextColor = RGB(0xC0, 0xC0, 0xC0);   // dark gray

			if (!fDisabled && ((Item->itemState & ODS_SELECTED) != 0))
			{
				SetTextColor(Item->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
				SetBkColor(Item->hDC, GetSysColor(COLOR_HIGHLIGHT));
			}

			FString strText = GetString(Item->itemID);
			ExtTextOutA(Item->hDC, Item->rcItem.left + 34,
				Item->rcItem.top + 2,
				ETO_OPAQUE, &(Item->rcItem), appToAnsi(*strText), strText.Len(), NULL);

			SetTextColor(Item->hDC, oldTextColor);
			SetBkColor(Item->hDC, oldBkColor);

			// BITMAP
			//
			LRESULT SelData = GetItemData(Item->itemID);
			HDC hdcMem = CreateCompatibleDC(Item->hDC);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, ((SelData & 1) ? hbmOn : hbmOff));

			BitBlt(Item->hDC,
				Item->rcItem.left + 2, Item->rcItem.top + 3,
				13, 13,
				hdcMem,
				0, 0,
				SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);

			hdcMem = CreateCompatibleDC(Item->hDC);
			hbmOld = (HBITMAP)SelectObject(hdcMem, ((SelData & 2) ? hbmOn : hbmOff));

			BitBlt(Item->hDC,
				Item->rcItem.left + 16, Item->rcItem.top + 3,
				13, 13,
				hdcMem,
				0, 0,
				SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);
		}

		if ((Item->itemAction & ODA_FOCUS) != 0)
			DrawFocusRect(Item->hDC, &(Item->rcItem));
		unguard;
	}
	void OnLeftButtonDown(INT X, INT Y)
	{
		guard(WDblCheckListBox::OnLeftButtonDown);
		WListBox::OnLeftButtonDown(X, Y);

		FPoint pt;
		::GetCursorPos((POINT*)pt);
		::ScreenToClient(hWnd, (POINT*)pt);

		if (pt.X <= 16)
		{
			INT Item = ItemFromPoint(pt);
			LRESULT Data = GetItemData(Item);
			if (Data & 1)
				Data &= ~1;
			else Data |= 1;
			SetItemData(Item, Data);
		}
		else if (pt.X <= 29)
		{
			int Item = ItemFromPoint(pt);
			LRESULT Data = GetItemData(Item);
			if (Data & 2)
				Data &= ~2;
			else Data |= 2;
			SetItemData( Item, Data );
		}

		check(OwnerWindow);
		PostMessageW( OwnerWindow->hWnd, WM_COMMAND, WM_WCLB_UPDATE_VISIBILITY, 0L );
		InvalidateRect( hWnd, NULL, FALSE );
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	FTreeItemBase.
-----------------------------------------------------------------------------*/

class WINDOW_API FTreeItemBase : public FCommandTarget, public FControlSnoop
{
public:
	virtual INT GetHeight() _VF_BASE_RET(0);

	virtual void Draw( HDC hDC ) _VF_BASE;
	virtual void SetSelected( UBOOL NewSelected ) _VF_BASE;
};

/*-----------------------------------------------------------------------------
	WItemBox.
-----------------------------------------------------------------------------*/

// A list box contaning list items.
class WINDOW_API WItemBox : public WListBox
{
	W_DECLARE_CLASS(WItemBox,WListBox,CLASS_Transient);
	DECLARE_WINDOWCLASS(WItemBox,WListBox,Window)

	// Constructors.
	WItemBox()
	{}
	WItemBox( WWindow* InOwner, INT InId=0)
	: WListBox( InOwner, InId )
	{
		check(OwnerWindow);
	}

	// WWindow interface.
	virtual LRESULT WndProc( UINT Message, WPARAM wParam, LPARAM lParam )
	{
		guard(WItemBox::WndProc);
		// Notifications.
		if (Message == WM_ERASEBKGND)
		{
			// Don't erase background.
			return 1;
		}
		else
			return WControl::WndProc(Message, wParam, lParam);
		unguardf((TEXT("(Message=%ls,PersistentName=%ls)"), *PersistentName, GetWindowMessageString(Message)));
	}
	void OnDrawItem( DRAWITEMSTRUCT* Info )
	{
		if( Info->itemData )
		{
			((FTreeItemBase*)Info->itemData)->SetSelected( (Info->itemState & ODS_SELECTED)!=0 );
			((FTreeItemBase*)Info->itemData)->Draw( Info->hDC );
		}
	}
	void OnMeasureItem( MEASUREITEMSTRUCT* Info )
	{
		if( Info->itemData )
			Info->itemHeight = ((FTreeItemBase*)Info->itemData)->GetHeight();
		else
		{
			// stijn: return a default height
			Info->itemHeight = MulDiv(16, DPIY, 96);
		}
	}
	void OnVScroll(WPARAM wParam, LPARAM lParam)
	{
		InvalidateRect(hWnd, NULL, FALSE);
	}
	void OnSize(DWORD Flags, INT NewX, INT NewY)
	{
		InvalidateRect(hWnd, NULL, FALSE);
	}
};

/*-----------------------------------------------------------------------------
	WListView.
-----------------------------------------------------------------------------

// A list view.
class WINDOW_API WListView : public WControl
{
	W_DECLARE_CLASS(WListView,WControl,CLASS_Transient)
	DECLARE_WINDOWSUBCLASS(WListView,WControl,Window)

	FDelegate DblClkDelegate;
	FDelegate SelChangedDelegate;

	// Constructor.
	WListView()
	{}
	WListView( WWindow* InOwner, INT InId=0, WNDPROC InSuperProc=NULL )
	: WControl( InOwner, InId, InSuperProc?InSuperProc:SuperProc )
	{}


	void OpenWindow( UBOOL Visible, DWORD dwExtraStyle = 0 )
	{
		guard(WListView::OpenWindow);
		PerformCreateWindowEx
		(
			WS_EX_CLIENTEDGE,
			NULL,
			WS_CHILD | (Visible?WS_VISIBLE:0) | dwExtraStyle,
			0, 0,
			0, 0,
			*OwnerWindow,
			(HMENU)ControlId,
			hInstance
		);

		SendMessageX( *this, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(0,0) );
		unguard;
	}

	void Empty()
	{
		guard(WListView::Empty);
		SendMessageX(hWnd, LVM_DELETEALLITEMS, 0, 0L);
		unguard;
	}

	UBOOL InterceptControlCommand( UINT Message, WPARAM wParam, LPARAM lParam )
	{
		guard(WListView::InterceptControlCommand);

		LastwParam = wParam;
		LastlParam = lParam;

		NMTREEVIEW* pntv = (LPNMTREEVIEW)lParam;

		if( pntv->hdr.code==NM_DBLCLK )
		{
			DblClkDelegate();
			return 1;
		}
		if( pntv->hdr.code==LVN_ITEMCHANGED )
			SelChangedDelegate();

		return 0;
		unguard;
	}


	LRESULT GetCurrent()
	{
		guard(WListView::GetCurrent);
		return SendMessageX( hWnd, LVM_GETNEXTITEM, -1, LVNI_ALL | LVNI_SELECTED | LVNI_FOCUSED );
		unguard;
	}
};
*/
/*-----------------------------------------------------------------------------
	WPropertiesBase.
-----------------------------------------------------------------------------*/

class WINDOW_API WPropertiesBase : public WWindow, public FControlSnoop
{
	W_DECLARE_ABSTRACT_CLASS(WPropertiesBase,WWindow,CLASS_Transient);

	// Variables.
	UBOOL ShowTreeLines{};
	WItemBox List;
	class FTreeItem* FocusItem{};

	// Structors.
	WPropertiesBase()
	{}
	WPropertiesBase( FName InPersistentName, WWindow* InOwnerWindow )
	:	WWindow			( InPersistentName, InOwnerWindow )
	,	List			( this )
	,	FocusItem		( NULL )
	,	ShowTreeLines	( 1 )
	{
		List.Snoop = this;
	}

	// WPropertiesBase interface.
	virtual void ResizeList() _VF_BASE;
	virtual void SetItemFocus( UBOOL FocusCurrent ) _VF_BASE;
	virtual void ForceRefresh() _VF_BASE;
	virtual void BeginSplitterDrag() _VF_BASE;
	virtual FTreeItem* GetRoot() _VF_BASE_RET(NULL);
	virtual INT GetDividerWidth() _VF_BASE_RET(0);
	virtual class FTreeItem* GetListItem( INT i )
	{
		guard(WPropertiesBase::GetListItem);
		FTreeItem* Result = (FTreeItem*)List.GetItemData(i);
		check(Result);
		return Result;
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WDragInterceptor.
-----------------------------------------------------------------------------*/

// Splitter drag handler.
class WINDOW_API WDragInterceptor : public WWindow
{
	W_DECLARE_CLASS(WDragInterceptor,WWindow,CLASS_Transient);
	DECLARE_WINDOWCLASS(WDragInterceptor,WWindow,Window)

	// Variables.
	FPoint		OldMouseLocation{};
	FPoint		DragIndices{};
	FPoint		DragPos{};
	FPoint		DragStart{};
	FPoint		DrawWidth{};
	FRect		DragClamp{};
	UBOOL		Success{};

	// Constructor.
	WDragInterceptor()
	{}
	WDragInterceptor( WWindow* InOwner, FPoint InDragIndices, FRect InDragClamp, FPoint InDrawWidth )
	:	WWindow			( NAME_None, InOwner )
	,	DragIndices		( InDragIndices )
	,	DragPos			( FPoint::ZeroValue() )
	,	DragClamp		( InDragClamp )
	,	DrawWidth		( InDrawWidth )
	,	Success			( 1 )
	{}

	// Functions.
	virtual void OpenWindow()
	{
		guard(WDragInterceptor::OpenWindow);
		PerformCreateWindowEx( 0, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, *OwnerWindow, NULL, hInstance );
		OldMouseLocation = OwnerWindow->GetCursorPos();
		DragStart = DragPos;
		SetCapture( *this );
		SetFocus( *this );
		ClipCursor( ClientToScreen(DragClamp-DragPos+OwnerWindow->GetCursorPos()) );
		ToggleDraw( NULL );
		unguard;
	}
	virtual void ToggleDraw( HDC hInDC )
	{
		guard(WDragInterceptor::ToggleDraw);

		HDC hDC = hInDC ? hInDC : GetDC(*OwnerWindow);
		HBRUSH OldBrush = (HBRUSH)SelectObject( hDC, hBrushStipple );
		if( DragIndices.X!=INDEX_NONE )
			PatBlt( hDC, DragPos.X, 0, DrawWidth.X, OwnerWindow->GetClientRect().Height(), PATINVERT );
		if( DragIndices.Y!=INDEX_NONE )
			PatBlt( hDC, 0, DragPos.Y, OwnerWindow->GetClientRect().Width(), DrawWidth.Y, PATINVERT );
		if( !hInDC )
			ReleaseDC( hWnd, hDC );
		SelectObject( hDC, OldBrush );

		unguard;
	}
	void OnKeyDown( TCHAR Ch )
	{
		if( Ch==VK_ESCAPE )
		{
			Success = 0;
			ReleaseCapture();
		}
	}
	void OnMouseMove( DWORD Flags, FPoint MouseLocation )
	{
		guard(WDragInterceptor::OnMouseMove);
		ToggleDraw( NULL );
		for( INT i=0; i<FPoint::Num(); i++ )
			if( DragIndices(i)!=INDEX_NONE )
				DragPos(i) = Clamp( DragPos(i) + MouseLocation(i) - OldMouseLocation(i), DragClamp.Min(i), DragClamp.Max(i) );
		ToggleDraw( NULL );
		OldMouseLocation = MouseLocation;
		unguard;
	}
	void OnReleaseCapture()
	{
		guard(WDragInterceptor::OnReleaseCapture);
		ClipCursor( NULL );
		ToggleDraw( NULL );
		OwnerWindow->OnFinishSplitterDrag( this, Success );
		DestroyWindow( *this );
		unguard;
	}
	void OnLeftButtonUp()
	{
		guard(WDragInterceptor::OnLeftButtonUp);
		ReleaseCapture();
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	FTreeItem.
-----------------------------------------------------------------------------*/

// QSort comparator.
static INT Compare( const class FTreeItem* T1, const class FTreeItem* T2 );

// Base class of list items.
class WINDOW_API FTreeItem : public FTreeItemBase
{
public:
	// Variables.
	class WPropertiesBase*	OwnerProperties{};
	FTreeItem*				Parent{};
	UBOOL					Expandable{};
	UBOOL					Expanded{};
	UBOOL					Sorted{};
	UBOOL					Selected{};
	UBOOL					IsAButton{};
	UBOOL					IsFreeComboEdit{};
	INT						ButtonWidth{};
	TArray<WCoolButton*>	Buttons;
	TArray<FTreeItem*>		Children;
	static TArray<FTreeItem*> DeleteItems;

	// Structors.
	FTreeItem()
	{}
	FTreeItem( WPropertiesBase* InOwnerProperties, FTreeItem* InParent, UBOOL InExpandable )
	:	OwnerProperties	( InOwnerProperties )
	,	Parent			( InParent )
	,	Expandable		( InExpandable )
	,	Expanded		( 0 )
	,	Sorted			( 1 )
	,	ButtonWidth		( 0 )
	,	Selected		( 0 )
	,	IsAButton		( 0 )
	,	Buttons			()
	,	Children		()
	{}
	virtual ~FTreeItem() noexcept(false)
	{
		guard(FTreeItem::~FTreeItem);
		for (INT i = 0; i < Buttons.Num(); i++)
		{
			Buttons(i)->Show(0);
			Buttons(i)->DelayedDestroy();
		}
		Buttons.Empty();
		DeleteItems.RemoveItem(this);
		EmptyChildren();
		unguard;
	}

	// FTreeItem interface.
	virtual HBRUSH GetBackgroundBrush( UBOOL Selected )
	{
		guard(FTreeItem::GetBackgroundBrush);
		return Selected ? hBrushCurrent : hBrushOffWhite;
		unguard;
	}
	virtual COLORREF GetTextColor( UBOOL Selected )
	{
		guard(FTreeItem::GetTextColor);
		return Selected ? RGB(255,255,255) : RGB(0,0,0);
		unguard;
	}
	virtual void Serialize( FArchive& Ar )
	{
		guard(FTreeItem::Serialize);
		//!!Super::Serialize( Ar );
		for( INT i=0; i<Children.Num(); i++ )
			Children(i)->Serialize( Ar );
		unguard;
	}
	virtual INT OnSetCursor()
	{
		guard(FTreeItem::OnSetCursor);
		return 0;
		unguard;
	}
	void EmptyChildren()
	{
		guard(FTreeItem::EmptyChildren);
		for (INT i = 0; i < Children.Num(); i++)
			Children(i)->DelayedDestroy();
		Children.Empty();
		unguard;
	}
	virtual void DelayedDestroy()
	{
		DeleteItems.AddUniqueItem(this);
	}
	virtual FRect GetRect()
	{
		guard(FTreeItem::GetRect);
		return OwnerProperties->List.GetItemRect(OwnerProperties->List.FindItemChecked(this));
		unguard;
	}
	virtual void Redraw()
	{
		guard(FTreeItem::Redraw);
		InvalidateRect( OwnerProperties->List, GetRect(), 0 );
		UpdateWindow( OwnerProperties->List );
		if( Parent!=OwnerProperties->GetRoot() )
			Parent->Redraw();
		unguard;
	}
	virtual void OnItemSetFocus()
	{
		guard(FTreeItem::OnItemSetFocus);
		if( Parent && Parent!=OwnerProperties->GetRoot() )
			Parent->OnItemSetFocus();
		unguard;
	}
	virtual void OnItemKillFocus( UBOOL Abort )
	{
		guard(FTreeItem::OnItemKillFocus);
		if (!IsAButton)
		{
			for (INT i = 0; i < Buttons.Num(); i++)
			{
				Buttons(i)->Show(0);
				Buttons(i)->DelayedDestroy();
			}
			Buttons.Empty();
		}
		ButtonWidth = 0;
		Redraw();
		if( Parent && Parent!=OwnerProperties->GetRoot() )
			Parent->OnItemKillFocus( Abort );
		unguard;
	}
	virtual void AddButton( const TCHAR* Text, FDelegate Action )
	{
		guard(FTreeItem::AddButton);
		INT Width = 0;
		WCoolButton* Button = new WCoolButton(&OwnerProperties->List,0,Action);
		Buttons.AddItem( Button );
		if (IsAButton)
			Button->OpenWindow(1, 0, 0, 0, 0, Text);
		else
		{
			FRect Rect = GetRect();
			Rect.Max.X -= ButtonWidth;
			SIZE Size;
			HDC hDC = GetDC(*OwnerProperties);
			GetTextExtentPoint32W(hDC, Text, appStrlen(Text), &Size);
			ReleaseDC(*OwnerProperties, hDC);
			Width = Size.cx + 2;

			Button->OpenWindow(1, Rect.Max.X - Width, Rect.Min.Y, Width, Rect.Max.Y - Rect.Min.Y, Text);
		}
		ButtonWidth += Width;
		unguard;
	}
	virtual void OnItemLeftMouseDown( FPoint P )
	{
		guard(FTreeItem::OnItemLeftMouseDown);
		if( Expandable && GetExpanderRect().Inner(FPoint(-1,-1)).Contains(P) )
			ToggleExpansion();
		unguard;
	}
	virtual void OnItemRightMouseDown( FPoint P )
	{
		guard(FTreeItem::OnItemRightMouseDown);
		unguard;
	}
	INT GetIndent()
	{
		guard(FTreeItem::GetIndent);
		INT Result=0;
		for( FTreeItem* Test=Parent; Test!=OwnerProperties->GetRoot(); Test=Test->Parent )
			Result++;
		return Result;
		unguard;
	}
	INT GetUnitIndentPixels()
	{
		return MulDiv(OwnerProperties->ShowTreeLines ? 12 : 8, WWindow::DPIX, 96);
	}
	virtual INT GetIndentPixels( UBOOL Text )
	{
		guard(FTreeItem::GetIndentPixels);
		return GetUnitIndentPixels()*GetIndent() + MulDiv((Text?OwnerProperties->ShowTreeLines?20:16:0), WWindow::DPIX, 96);
		unguard;
	}
	virtual FRect GetExpanderRect()
	{
		guard(FTreeItem::GetExpanderRect);
		return FRect(
			GetIndentPixels(0) + MulDiv(4, WWindow::DPIX, 96),
			MulDiv(4, WWindow::DPIY, 96),
			GetIndentPixels(0) + MulDiv(13, WWindow::DPIX, 96),
			MulDiv(13, WWindow::DPIY, 96) );
		unguard;
	}
	virtual UBOOL GetSelected()
	{
		guard(FTreeItem::GetSelected);
		return Selected;
		unguard;
	}
	void SetSelected( UBOOL InSelected )
	{
		guard(FTreeItem::SetSelected);
		Selected = InSelected;
		unguard;
	}
	virtual void DrawTreeLines( HDC hDC, FRect Rect )
	{
		guard(FTreeItem::DrawTreeLines);
		SetBkMode( hDC, TRANSPARENT );
		SetTextColor( hDC, RGB(0,0,0) );
		if( OwnerProperties->ShowTreeLines )
		{
			FTreeItem* Prev = this;
			for( INT i=GetIndent(); i>=0; i-- )
			{
				check(Prev->Parent);
				UBOOL FromAbove = (Prev->Parent->Children.Last()!=Prev || Prev->Parent->Children.Last()==this) && (i!=0 || Prev->Parent->Children(0)!=this);
				UBOOL ToBelow   = (Prev->Parent->Children.Last()!=Prev);
				FPoint P( Rect.Min.X + GetUnitIndentPixels()*i, Rect.Min.Y );
				if( FromAbove || ToBelow )
					FillRect( hDC,
						FRect(
							P+FPoint(MulDiv(8, WWindow::DPIX, 96),MulDiv(FromAbove?0:8, WWindow::DPIY, 96)),
							P+FPoint(MulDiv(9, WWindow::DPIX, 96),MulDiv(ToBelow?16:8, WWindow::DPIY, 96))),
						hBrushStipple );
				if( i==GetIndent() )
					FillRect( hDC,
						FRect(
							P+FPoint(MulDiv(8, WWindow::DPIX, 96),MulDiv(8, WWindow::DPIY, 96)),
							P+FPoint(MulDiv(20, WWindow::DPIX, 96),MulDiv(9,WWindow::DPIY, 96))),
						hBrushStipple );
				Prev = Prev->Parent;
			}
			check(Prev->Parent==NULL);
		}
		if( Expandable )
		{
			FRect R = GetExpanderRect() + GetRect().Min;
			FillRect( hDC, R, hBrushGrey );
			FillRect( hDC, R.Inner(FPoint(MulDiv(1, WWindow::DPIX, 96), MulDiv(1, WWindow::DPIY, 96))), GetBackgroundBrush(0) );
			FillRect( hDC, R.Inner(FPoint(MulDiv(2, WWindow::DPIX, 96), MulDiv(4, WWindow::DPIY, 96))), hBrushBlack );
			if( !Expanded )
				FillRect( hDC, R.Inner(FPoint(MulDiv(4, WWindow::DPIX, 96), MulDiv(2, WWindow::DPIY, 96))), hBrushBlack );
		}
		unguard;
	}
	virtual void Collapse()
	{
		guard(FTreeItem::Collapse);
		OwnerProperties->SetItemFocus( 0 );
		INT Index = OwnerProperties->List.FindItemChecked( this );
		INT Count = OwnerProperties->List.GetCount();
		while( Index+1<Count )
		{
			FTreeItem* NextItem = OwnerProperties->GetListItem(Index+1);
			FTreeItem* Check;
			for( Check=NextItem->Parent; Check && Check!=this; Check=Check->Parent );
			if( !Check )
				break;
			NextItem->Expanded = 0;
			OwnerProperties->List.DeleteString( Index+1 );
			Count--;
		}
		Expanded=0;
		OwnerProperties->ResizeList();
		unguard;
	}
	virtual void Expand()
	{
		guard(FTreeItem::Expand);
		if( Sorted )
		{
			Sort( &Children(0), Children.Num() );
		}
		if( this==OwnerProperties->GetRoot() )
		{
			for( INT i=0; i<Children.Num(); i++ )
				OwnerProperties->List.AddItem( Children(i) );
		}
		else
		{
			for( INT i=Children.Num()-1; i>=0; i-- )
				OwnerProperties->List.InsertItemAfter( this, Children(i) );
		}
		OwnerProperties->SetItemFocus( 0 );
		OwnerProperties->ResizeList();
		Expanded=1;
		unguard;
	}
	virtual void ToggleExpansion()
	{
		guard(FTreeItem::ToggleExpansion);
		if( Expandable )
		{
			OwnerProperties->List.SetRedraw( 0 );
			if( Expanded )
				Collapse();
			else
				Expand();
			OwnerProperties->List.SetRedraw( 1 );
			UpdateWindow( OwnerProperties->List );
		}
		OwnerProperties->SetItemFocus( 1 );
		unguard;
	}
	virtual void OnItemDoubleClick()
	{
		guard(FTreeItem::OnItemDoubleClick);
		ToggleExpansion();
		unguard;
	}
	virtual BYTE* GetReadAddress(UProperty* InProperty, FTreeItem* Top, INT Index = INDEX_NONE)
	{
		guard(FTreeItem::GetReadAddress);
		return Parent ? Parent->GetReadAddress(InProperty, Top, Index) : NULL;
		unguard;
	}
	virtual FArray* GetArrayAddress(INT Index = INDEX_NONE)
	{
		guard(FTreeList::GetArrayAddress);
		return NULL;
		unguard;
	}
	virtual UProperty* FindProperty(const TCHAR* PropName) const
	{
		guardSlow(FTreeList::FindProperty);
		return Parent ? Parent->FindProperty(PropName) : nullptr;
		unguardSlow;
	}
	virtual void SetProperty(UProperty* Property, FTreeItem* Offset, const TCHAR* Value, INT ExtraFlags = 0)
	{
		guard(FTreeList::SetProperty);
		if (Parent)
			Parent->SetProperty(Property, Offset, Value, ExtraFlags);
		unguard;
	}
	virtual void NotePropertyChange(UProperty* Property, FTreeItem* Offset)
	{
		guard(FTreeList::NotePropertyChange);
		if (Parent)
			Parent->NotePropertyChange(Property, Offset);
		unguard;
	}
	virtual void SaveObjectConfig()
	{
		guard(FTreeList::SaveObjectConfig);
		if (Parent) Parent->SaveObjectConfig();
		unguard;
	}
	virtual UClass* GetBaseClass()
	{
		guardSlow(FTreeList::GetBaseClass);
		return Parent ? Parent->GetBaseClass() : nullptr;
		unguardSlow;
	}
	virtual UBOOL AcceptFlags( DWORD InFlags )
	{
		guard(FTreeList::AcceptFlags);
		return Parent ? Parent->AcceptFlags( InFlags ) : 0;
		unguard;
	}
	virtual UProperty* GetParentProperty()
	{
		return Parent ? Parent->GetParentProperty() : NULL;
	}
	virtual UBOOL CopyFullProperties(FString* Output)
	{
		return Parent ? Parent->CopyFullProperties(Output) : FALSE;
	}
	virtual QWORD GetId() const=0;
	virtual FString GetCaption() const=0;
	virtual FString GetDescription()
	{
		return FString();
	}
	virtual void OnItemHelp() {}
	virtual void SetFocusToItem() {}
	virtual void SetValue( const TCHAR* Value ) {}
	virtual void OnArrayEmpty() {}
	virtual INT GetItemCount() const
	{
		return Parent ? Parent->GetItemCount() : 1;
	}

	// FControlSnoop interface.
	void SnoopChar( WWindow* Src, INT Char )
	{
		guard(FTreeItem::SnoopChar);
		if( Char==13 && Expandable )
			ToggleExpansion();
		unguard;
	}
	void SnoopKeyDown( WWindow* Src, INT Char )
	{
		guard(FTreeItem::SnoopKeyDown);
		if( Char==VK_UP || Char==VK_DOWN )
			PostMessageW( OwnerProperties->List, WM_KEYDOWN, Char, 0 );
		if( Char==9 )
			PostMessageW( OwnerProperties->List, WM_KEYDOWN, (GetKeyState(16)&0x8000)?VK_UP:VK_DOWN, 0 );
		unguard;
	}
	virtual UObject* GetParentOuter()
	{
		guard(FTreeItem::GetParentOuter);
		return Parent ? Parent->GetParentOuter() : NULL;
		unguard;
	}
	virtual INT GetArrayIndex() const
	{
		guardSlow(FTreeItem::GetArrayIndex);
		return Parent ? Parent->GetArrayIndex() : INDEX_NONE;
		unguardSlow;
	}
	virtual void NotifyEditProperty(UProperty* P, UProperty* SubProperty, INT ArrayIndex)
	{
		guardSlow(FTreeItem::NotifyEditProperty);
		if (Parent)
			Parent->NotifyEditProperty(P, SubProperty, ArrayIndex);
		unguardSlow;
	}
	virtual void NotifyEditObject(UObject* Obj) const
	{
		guardSlow(FTreeItem::NotifyEditObject);
		if (Parent)
			Parent->NotifyEditObject(Obj);
		unguardSlow;
	}

	virtual void NotifyListResize() {}
};

// QSort comparator.
static INT Compare( const class FTreeItem* T1, const class FTreeItem* T2 )
{
	return appStricmp( *T1->GetCaption(), *T2->GetCaption() );
}

// Property list item.
class WINDOW_API FPropertyItem : public FTreeItem
{
public:
	// Variables.
	UProperty*      Property;
	INT				Offset;
	INT				ArrayIndex;
	INT				OldWidth;
	WEdit*			EditControl;
	WEdit*			KeyEditControl;
	WTrackBar*		TrackControl;
	WComboBox*		ComboControl;
	WLabel*			HolderControl;
	UBOOL			ComboChanged;
	FName			Name;
	FString			Text;
	BITFIELD		bRadiiAng : 1;
	BITFIELD		bIsMapValue : 1;
	BITFIELD		bIsLightColor : 1;
	BITFIELD		bIsPolyFlags : 1;

	// Constructors.
	FPropertyItem(WPropertiesBase* InOwnerProperties, FTreeItem* InParent, UProperty* InProperty, FName InName, INT InOffset, INT InArrayIndex, UBOOL bIsMap = 0);

	BYTE* GetReadAddress(UProperty* InProperty, FTreeItem* Top, INT Index = INDEX_NONE);

	FArray* GetArrayAddress(INT Index = INDEX_NONE)
	{
		guardSlow(FPropertyItem::GetArrayAddress);
		if (!Parent)
			return NULL;
		BYTE* AdrV = Parent->GetReadAddress(Property, this, Index);
		if (!AdrV)
			return NULL;
		AdrV += Offset;
		return (FArray*)AdrV;
		unguardSlow;
	}
	UProperty* GetParentProperty();

	// FTreeItem interface.
	void Serialize(FArchive& Ar);
	QWORD GetId() const
	{
		guard(FPropertyItem::GetId);
		return Name.GetIndex() | ((QWORD)1 << 32) | (((QWORD)(ArrayIndex + 1)) << 33);
		unguard;
	}
	FString GetCaption() const
	{
		guard(FPropertyItem::GetCaption);
		return *Name;
		unguard;
	}
	virtual INT OnSetCursor()
	{
		guard(FPropertyItem::OnSetCursor);
		FPoint P = OwnerProperties->GetCursorPos() - GetRect().Min;
		if( Abs(P.X-OwnerProperties->GetDividerWidth())<=2 )
		{
			SetCursor(LoadCursorW(hInstanceWindow,MAKEINTRESOURCEW(IDC_SplitWE)));
			return 1;
		}
		return 0;
		unguard;
	}
	virtual void OnItemLeftMouseDown(FPoint P);
	virtual void OnItemRightMouseDown(FPoint P);
	virtual void GetPropertyText(FString& Str, BYTE* ReadValue, UBOOL bRealValue=0);
	void SetValue(const TCHAR* Value);
	void Draw(HDC hDC);
	INT GetHeight()
	{
		guard(FPropertyItem::GetHeigth);
		return MulDiv(16, WWindow::DPIY, 96);
		unguard;
	}
	void SetFocusToItem();
	void OnItemDoubleClick();
	void OnItemSetFocus();
	void OnItemKillFocus(UBOOL Abort);
	void Expand();
	void Collapse();
	COLORREF GetTextColor(UBOOL Selected);
	INT GetArrayIndex() const
	{
		guardSlow(FPropertyItem::GetArrayIndex);
		if (ArrayIndex==INDEX_NONE)
			return FTreeItem::GetArrayIndex();
		return ArrayIndex;
		unguardSlow;
	}

	// FControlSnoop interface.
	void SnoopChar(WWindow* Src, INT Char);
	void ComboSelectionEndCancel();
	void ComboSelectionEndOk();
	void OnTrackBarThumbTrack();
	void OnTrackBarThumbPosition();
	void OnChooseColorButton();
	void OnArrayAdd();
	void OnArrayEmpty();
	void OnArrayInsert();
	void OnArrayDelete();
	void OnBrowseButton();
	void OnUseCurrentButton();
	void OnClearButton();

	// FPropertyItem interface.
	virtual void Advance();
	virtual void SendToControl();
	virtual void ReceiveFromControl();
	FString GetDescription();
	void NotifyListResize();
};

// An abstract list header.
class WINDOW_API FHeaderItem : public FTreeItem
{
protected:
	UBOOL bDrawLightColor{};

public:
	// Constructors.
	FHeaderItem()
	{}
	FHeaderItem( WPropertiesBase* InOwnerProperties, FTreeItem* InParent, UBOOL InExpandable )
	: FTreeItem( InOwnerProperties, InParent, InExpandable )
	{}

	// FTreeItem interface.
	void Draw(HDC hDC);
	void OnChooseHSLColorButton();
	void OnItemSetFocus();
	void OnItemKillFocus(UBOOL Abort);
	FColor GetLightColor(UBOOL bFullBright);

	INT GetHeight()
	{
		guard(FHeaderItem::GetHeigth);
		return MulDiv(16, WWindow::DPIY, 96);
		unguard;
	}
};

// An category header list item.
class WINDOW_API FCategoryItem : public FHeaderItem
{
public:
	// Variables.
	FName Category;
	UClass* BaseClass;

	// Constructors.
	FCategoryItem( WPropertiesBase* InOwnerProperties, FTreeItem* InParent, UClass* InBaseClass, FName InCategory, UBOOL InExpandable )
	:	FHeaderItem( InOwnerProperties, InParent, InExpandable )
	,	Category    ( InCategory )
	,	BaseClass	( InBaseClass )
	{
		check(BaseClass);
	}

	// FTreeItem interface.
	void Serialize( FArchive& Ar )
	{
		guard(FCategoryItem::Serialize);
		FHeaderItem::Serialize( Ar );
		Ar << Category << BaseClass;
		unguard;
	}
	QWORD GetId() const
	{
		guard(FCategoryItem::GetId);
		return Category.GetIndex() + ((QWORD)2<<32);
		unguard;
	}
	virtual FString GetCaption() const
	{
		guard(FCategoryItem::GetText);
		return *Category;
		unguard;
	}
	void Expand()
	{
		guard(FCategoryItem::Expand);
		for (TFieldIterator<UProperty> It(BaseClass); It; ++It)
			if (It->Category == Category && AcceptFlags(It->PropertyFlags))
				Children.AddItem(new FPropertyItem(OwnerProperties, this, *It, It->GetFName(), It->Offset, -1));
		FTreeItem::Expand();
		unguard;
	}
	void Collapse()
	{
		guard(FCategoryItem::Collapse);
		FTreeItem::Collapse();
		EmptyChildren();
		unguard;
	}
};

// For hidden properties...
class WINDOW_API FHiddenCategoryItem : public FHeaderItem
{
public:
	// Variables.
	UClass* BaseClass;

	// Constructors.
	FHiddenCategoryItem(WPropertiesBase* InOwnerProperties, FTreeItem* InParent, UClass* InBaseClass)
		: FHeaderItem(InOwnerProperties, InParent, 1)
		, BaseClass(InBaseClass)
	{
		verify(BaseClass!=NULL);
	}

	// FTreeItem interface.
	void Serialize(FArchive& Ar)
	{
		guard(FHiddenCategoryItem::Serialize);
		FHeaderItem::Serialize(Ar);
		Ar << BaseClass;
		unguard;
	}
	QWORD GetId() const
	{
		guard(FHiddenCategoryItem::GetId);
		return BaseClass->GetIndex() + ((QWORD)2 << 32);
		unguard;
	}
	virtual FString GetCaption() const
	{
		guard(FHiddenCategoryItem::GetText);
		return BaseClass->GetName();
		unguard;
	}
	void Expand()
	{
		guard(FHiddenCategoryItem::Expand);
		for (TFieldIterator<UProperty> It(BaseClass); (It && It->GetOwnerClass()==BaseClass); ++It)
			if (!AcceptFlags(It->PropertyFlags) || It->Category == NAME_None)
				Children.AddItem(new FPropertyItem(OwnerProperties, this, *It, It->GetFName(), It->Offset, -1));
		FTreeItem::Expand();
		unguard;
	}
	void Collapse()
	{
		guard(FHiddenCategoryItem::Collapse);
		FTreeItem::Collapse();
		EmptyChildren();
		unguard;
	}
};

class WINDOW_API FHiddenCategoryList : public FHiddenCategoryItem
{
public:
	// Constructors.
	FHiddenCategoryList(WPropertiesBase* InOwnerProperties, FTreeItem* InParent, UClass* InBaseClass)
		: FHiddenCategoryItem(InOwnerProperties, InParent, InBaseClass)
	{}

	// FTreeItem interface.
	QWORD GetId() const
	{
		guard(FHiddenCategoryList::GetId);
		return (BaseClass ? BaseClass->GetIndex() : 0) + ((QWORD)3 << 32);
		unguard;
	}
	virtual FString GetCaption() const
	{
		guard(FHiddenCategoryItem::GetText);
		return TEXT("<Hidden Properties>");
		unguard;
	}
	COLORREF GetTextColor(UBOOL Selected)
	{
		guard(FHiddenCategoryItem::GetTextColor);
		return Selected ? RGB(255, 128, 128) : RGB(128, 0, 0);
		unguard;
	}
	void Expand()
	{
		guard(FHiddenCategoryItem::Expand);
		TArray<UClass*> CatList;
		for (TFieldIterator<UProperty> It(BaseClass); It; ++It)
			if (!AcceptFlags(It->PropertyFlags) || It->Category == NAME_None)
				CatList.AddUniqueItem(It->GetOwnerClass());
		for(INT i=0; i< CatList.Num(); ++i)
			Children.AddItem(new FHiddenCategoryItem(OwnerProperties, this, CatList(i)));
		FTreeItem::Expand();
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WProperties.
-----------------------------------------------------------------------------*/

// General property editing control.
class WINDOW_API WProperties : public WPropertiesBase
{
	W_DECLARE_ABSTRACT_CLASS(WProperties,WPropertiesBase,CLASS_Transient);
	DECLARE_WINDOWCLASS(WProperties,WWindow,Window)

	// Variables.
	TArray<QWORD>		Remembered;
	QWORD				SavedTop{}, SavedCurrent{};
	WDragInterceptor*	DragInterceptor{};
	INT					DividerWidth{};
	static TArray<WProperties*> PropertiesWindows;
	WToolTipPopup		ToolTipP;
	INT TooltipIndex;
	UBOOL bAllowForceRefresh{};

	// Structors.
	WProperties()
	{}
	WProperties( FName InPersistentName, WWindow* InOwnerWindow=NULL )
	:	WPropertiesBase	( InPersistentName, InOwnerWindow )
	,	DragInterceptor	( NULL )
	,	DividerWidth	( MulDiv(128, DPIX, 96) )
	,	ToolTipP		( this )
	{
		guard(WProperties::WProperties);
		if (PersistentName != NAME_None)
		{
			GConfig->GetInt(TEXT("WindowPositions"), *(FString(*PersistentName) + TEXT(".Split")), DividerWidth);
			DividerWidth = Clamp<INT>(DividerWidth, 16, 400);
		}
		PropertiesWindows.AddItem( this );
		List.DoubleClickDelegate     = FDelegate(this,(TDelegate)&WProperties::OnListDoubleClick);
		List.SelectionChangeDelegate = FDelegate(this,(TDelegate)&WProperties::OnListSelectionChange);
		bAllowForceRefresh = 1;

		unguard;
	}

	void OnTimer()
	{
		ToolTipP.Tick();
	}

	// WWindow interface.
	void Serialize( FArchive& Ar )
	{
		guard(WProperties::Serialize);
		WPropertiesBase::Serialize( Ar );
		GetRoot()->Serialize( Ar );
		unguard;
	}
	void DoDestroy()
	{
		guard(WProperties::DoDestroy);
		PropertiesWindows.RemoveItem( this );
		WWindow::DoDestroy();
		unguard;
	}
	void OnClose()
	{
		guard(WProperties::OnClose);
		ForceParentFocus();
		WPropertiesBase::OnClose();
		unguard;
	}
	INT OnSetCursor()
	{
		guard(WProperties::OnSetCursor);
		if( ::IsWindow(List.hWnd) && List.GetCount() )
		{
			FPoint P = GetCursorPos();
			INT Index = List.ItemFromPoint( P );
			FTreeItem* Item = Index>0 ? GetListItem(Index) : NULL;
			if (TooltipIndex != Index)
			{
				TooltipIndex = Index;
				SetTimer(hWnd, 0, 50, NULL);

				UBOOL HideTool = 1;
				if (Item)
				{
					FString InfoText = Item->GetDescription();
					if (InfoText.Len())
					{
						FPoint Pnt;
						FRect Rc = List.GetItemRect(Index);
						Pnt.X = P.X + 6;
						Pnt.Y = Rc.Max.Y + 8;
						FPoint RP = List.ClientToScreen(Pnt);

						ToolTipP.OpenTooltip(*InfoText, RP.X, RP.Y, 5.f, 0.5f);
						HideTool = 0;
						TooltipIndex = Index;
					}
				}
				if(HideTool)
					ToolTipP.HideToolTip();
			}
			return Item ? Item->OnSetCursor() : 0;
		}
		return 0;
		unguard;
	}
	void OnDestroy()
	{
		guard(WProperties::OnDestroy);
		WWindow::OnDestroy();
		_DeleteWindows.AddItem( this );
		unguard;
	}
	void OpenChildWindow( INT InControlId )
	{
		guard(WProperties::OpenChildWindow);
		HWND hWndParent = InControlId ? GetDlgItem(OwnerWindow->hWnd,InControlId) : OwnerWindow->hWnd;
		check(hWndParent);
		FRect R;
		::GetClientRect( hWndParent, R );
		PerformCreateWindowEx
		(
			0, NULL, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			0, 0, R.Width(), R.Height(),
			hWndParent, NULL, hInstance
		);
		List.OpenWindow( 1, 0, 0, 1 );
		Show(1);
		unguard;
	}
	void OpenWindow( HWND hWndParent=NULL )
	{
		guard(WProperties::OpenWindow);
		PerformCreateWindowEx
		(
			WS_EX_TOOLWINDOW,
			*GetRoot()->GetCaption(),
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX | WS_MINIMIZEBOX,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			MulDiv(512, DPIX, 96),
			MulDiv(512, DPIY, 96),
			hWndParent ? hWndParent : OwnerWindow ? OwnerWindow->hWnd : NULL,
			NULL,
			hInstance
		);
		List.OpenWindow( 1, 0, 0, 1 );
		unguard;
	}
	void OnActivate( UBOOL Active )
	{
		guard(WProperties::OnActivate);
		if( Active==1 )
		{
			SetFocus( List );
			if( !FocusItem )
				SetItemFocus( 1 );
		}
		else
		{
			SetItemFocus( 0 );
		}
		unguard;
	}
	void OnSize( DWORD Flags, INT NewX, INT NewY )
	{
		guard(WProperties::OnSize);
		WWindow::OnSize( Flags, NewX, NewY );
		if( List.hWnd )
		{
			SetItemFocus( 0 );
			InvalidateRect( List, NULL, FALSE );
			ResizeList();
			SetItemFocus( 1 );
		}
		unguard;
	}
	void OnPaint()
	{
		guard(WProperties::OnPaint);
		if( GetUpdateRect( *this, NULL, 0 ) )
		{
			PAINTSTRUCT PS;
			HDC   hDC      = BeginPaint( *this, &PS );
			FRect Rect     = GetClientRect();
			FRect ListRect = List.GetClientRect();
			Rect.Min.Y     = ListRect.Max.Y;
			FillRect( hDC, Rect, (HBRUSH)(COLOR_BTNFACE+1) );
			EndPaint( *this, &PS );
		}
		unguard;
	}

	// Delegates.
	void OnListDoubleClick()
	{
		if( FocusItem )
			FocusItem->OnItemDoubleClick();
	}
	void OnListSelectionChange()
	{
		SetItemFocus( 1 );
	}

	// FControlSnoop interface.
	void SnoopLeftMouseDown( WWindow* Src, FPoint P )
	{
		guard(WProperties::SnoopLeftMouseDown);
		if( Src==&List )
		{
			INT Index = List.ItemFromPoint( P );
			if( Index>=0 )
			{
				List.SetCurrent( Index, 0 );
				FTreeItem* Item = GetListItem( Index );
				Item->OnItemLeftMouseDown( P-Item->GetRect().Min );
			}
		}
		ToolTipP.HideToolTip();
		unguard;
	}
	void SnoopRightMouseDown( WWindow* Src, FPoint P )
	{
		guard(WProperties::SnoopRightMouseDown);
		if( Src==&List )
		{
			INT Index = List.ItemFromPoint( P );
			if( Index>=0 )
			{
				List.SetCurrent( Index, 0 );
				FTreeItem* Item = GetListItem( Index );
				Item->OnItemRightMouseDown( P-Item->GetRect().Min );
			}
		}
		ToolTipP.HideToolTip();
		unguard;
	}
	void SnoopChar( WWindow* Src, INT Char )
	{
		guard(WProperties::SnoopChar);
		if( FocusItem )
			FocusItem->SnoopChar( Src, Char );
		unguard;
	}
	void SnoopKeyDown( WWindow* Src, INT Char )
	{
		guard(WProperties::SnoopChar);
		if( Char==9 )
			PostMessageW( List, WM_KEYDOWN, (GetKeyState(16)&0x8000)?VK_UP:VK_DOWN, 0 );
		WPropertiesBase::SnoopKeyDown( Src, Char );
		unguard;
	}

	// WPropertiesBase interface.
	INT GetDividerWidth()
	{
		guard(WProperties::GetDividerWidth);
		return DividerWidth;
		unguard;
	}

	// WProperties interface.
	virtual void BeginSplitterDrag()
	{
		guard(WProperties::BeginDrag);
		SetItemFocus( NULL );
		DragInterceptor            = new WDragInterceptor( this, FPoint(0,INDEX_NONE), GetClientRect(), FPoint(3,3) );
		DragInterceptor->DragPos   = FPoint(GetDividerWidth(),GetCursorPos().Y);
		DragInterceptor->DragClamp = FRect(GetClientRect().Inner(FPoint(64,0)));
		DragInterceptor->OpenWindow();
		unguard;
	}
	void OnFinishSplitterDrag( WDragInterceptor* Drag, UBOOL Success )
	{
		guard(WProperties::OnFinishSplitterDrag);
		if( Success )
		{
			DividerWidth += Drag->DragPos.X - Drag->DragStart.X;
			if( PersistentName!=NAME_None )
				GConfig->SetInt( TEXT("WindowPositions"), *(FString(*PersistentName)+TEXT(".Split")), DividerWidth );
			InvalidateRect( *this, NULL, 0 );
			UpdateWindow( *this );
		}
		DragInterceptor = NULL;
		unguard;
	}
	virtual void SetValue( const TCHAR* Value )
	{
		guard(WProperties::SetValue);
		if( FocusItem )
			FocusItem->SetValue( Value );
		unguard;
	}
	virtual void SetItemFocus( UBOOL FocusCurrent )
	{
		guard(WProperties::SetItemFocus);
		if( FocusItem )
			FocusItem->OnItemKillFocus( 0 );
		FocusItem = NULL;
		if( FocusCurrent && List.GetCount()>0 )
			FocusItem = GetListItem( List.GetCurrent() );
		if( FocusItem )
			FocusItem->OnItemSetFocus();
		unguard;
	}
	virtual void ResizeList()
	{
		guard(WProperties::ResizeList);
		FRect ClientRect = GetClientRect();
		FRect R(0,0,0,4);//!!why?
		R = R.DPIScaled(WWindow::DPIX, WWindow::DPIY);
		for( INT i=List.GetCount()-1; i>=0; i-- )
			R.Max.Y += List.GetItemHeight( i );
		AdjustWindowRect( R, GetWindowLongPtrW(List,GWL_STYLE), 0 );
		List.MoveWindow( FRect(0,0,ClientRect.Width(),Min(ClientRect.Height(),R.Height())), 1 );

		for (INT i = 0; i < List.GetCount(); i++)
			GetListItem(i)->NotifyListResize();
		unguard;
	}
	virtual void ForceRefresh()
	{
		guard(WProperties::ForceRefresh);

		if( !bAllowForceRefresh ) return;

		// Disable editing.
		SetItemFocus( 0 );

		// Remember which items were expanded.
		if( List.GetCount() )
		{
			Remembered.Empty();
			SavedTop     = GetListItem(List.GetTop())->GetId();
			SavedCurrent = List.GetSelected(List.GetCurrent()) ? GetListItem(List.GetCurrent())->GetId() : 0;
			for( INT i=0; i<List.GetCount(); i++ )
			{
				FTreeItem* Item = GetListItem(i);
				if( Item->Expanded )
					Remembered.AddItem( Item->GetId() );
			}
		}

		// Empty it and add root items.
		List.Empty();
		GetRoot()->EmptyChildren();
		GetRoot()->Expanded=0;
		GetRoot()->Expand();

		// Restore expansion state of the items.
		INT CurrentIndex=-1, TopIndex=-1;
		for( INT i=0; i<List.GetCount(); i++ )
		{
			FTreeItem* Item = GetListItem(i);
			QWORD      Id   = Item->GetId();
			if( Item->Expandable && !Item->Expanded )
			{
				INT j;
				for( j=0; j<Remembered.Num(); j++ )
					if( Remembered(j)==Id )
						break;
				if( j<Remembered.Num() )
					Item->Expand();
			}
			if( Id==SavedTop     ) TopIndex     = i;
			if( Id==SavedCurrent ) CurrentIndex = i;
		}

		// Adjust list size.
		ResizeList();

		// Set indices.
		if( TopIndex>=0 ) List.SetTop( TopIndex );
		if( CurrentIndex>=0 ) List.SetCurrent( CurrentIndex, 1 );

		unguard;
	}
};

/*-----------------------------------------------------------------------------
	FPropertyItemBase.
-----------------------------------------------------------------------------*/

class WINDOW_API FPropertyItemBase : public FHeaderItem
{
public:
	// Variables.
	FString Caption;
	DWORD FlagMask{};
	UClass* BaseClass{};

	// Structors.
	FPropertyItemBase()
	{}
	FPropertyItemBase( WPropertiesBase* InOwnerProperties, FTreeItem* InParent, DWORD InFlagMask, const TCHAR* InCaption )
	:	FHeaderItem	( InOwnerProperties, InParent, 1 )
	,	Caption		( InCaption )
	,	FlagMask	( InFlagMask )
	,	BaseClass	( NULL )
	{}

	// FTreeItem interface.
	void Serialize( FArchive& Ar )
	{
		guard(FPropertyItemBase::Serialize);
		FHeaderItem::Serialize( Ar );
		Ar << BaseClass;
		unguard;
	}

	// FHeaderItem interface.
	UBOOL AcceptFlags( DWORD InFlags )
	{
		guard(FPropertyItemBase::AcceptFlags);
		return (InFlags&FlagMask)==FlagMask;
		unguard;
	}
	UClass* GetBaseClass()
	{
		guardSlow(FPropertyItemBase::GetBaseClass);
		return BaseClass;
		unguardSlow;
	}
	void Collapse()
	{
		guard(FPropertyItemBase::Collapse);
		FTreeItem::Collapse();
		EmptyChildren();
		unguard;
	}
	FString GetCaption() const
	{
		guard(FPropertyItemBase::GetCaption);
		return Caption;
		unguard;
	}
	QWORD GetId() const
	{
		guard(FPropertyItemBase::GetId);
		return (QWORD)BaseClass + (QWORD)4;
		unguard;
	}

	UProperty* FindProperty(const TCHAR* PropName) const;
};

/*-----------------------------------------------------------------------------
	WObjectProperties.
-----------------------------------------------------------------------------*/

// Object properties root.
class WINDOW_API FObjectsItem : public FPropertyItemBase
{
public:
	// Variables.
	UBOOL ByCategory{};
	TArray<UObject*> _Objects;

	// Structors.
	FObjectsItem()
	{}
	FObjectsItem( WPropertiesBase* InOwnerProperties, FTreeItem* InParent, DWORD InFlagMask, const TCHAR* InCaption, UBOOL InByCategory )
	:	FPropertyItemBase( InOwnerProperties, InParent, InFlagMask, InCaption )
	,	ByCategory( InByCategory )
	{}

	// FTreeItem interface.
	void Serialize(FArchive& Ar);
	BYTE* GetReadAddress(UProperty* InProperty, FTreeItem* Top, INT Index = INDEX_NONE);
	INT GetItemCount() const
	{
		return _Objects.Num();
	}
	void SetProperty(UProperty* Property, FTreeItem* Offset, const TCHAR* Value, INT ExtraFlags = 0);
	void NotePropertyChange(UProperty* Property, FTreeItem* Offset);
	void Expand();
	UBOOL CopyFullProperties(FString* Output);
	void SaveObjectConfig() {}

	// FHeaderItem interface.
	FString GetCaption() const
	{
		guard(FObjectsItem::GetCaption);

		if( Caption.Len() )
			return Caption;
		else if( !BaseClass )
			return LocalizeGeneral("PropNone",TEXT("Window"));
		else if( _Objects.Num()==1 )
			return FString::Printf( *LocalizeGeneral(TEXT("PropSingle"),TEXT("Window")), BaseClass->GetName() );
		else
			return FString::Printf( *LocalizeGeneral(TEXT("PropMulti"),TEXT("Window")), BaseClass->GetName(), _Objects.Num() );

		unguard;
	}

	// FObjectsItem interface.
	virtual void SetObjects(UObject** InObjects, INT Count);

	// Validate selected classes here.
	void ValidateSelection();

	UObject* GetParentOuter()
	{
		guard(FObjectsItem::GetParentOuter);
		for( INT i=0; i<_Objects.Num(); ++i )
			if( _Objects(i) )
				return _Objects(i);
		return NULL;
		unguard;
	}

	virtual void NotifyEditProperty(UProperty* P, UProperty* SubProperty, INT ArrayIndex)
	{
		guard(FObjectsItem::NotifyEditProperty);
		for (INT i = 0; i < _Objects.Num(); ++i)
			if (_Objects(i))
				_Objects(i)->onPropertyFocus(P, SubProperty, ArrayIndex);
		unguard;
	}
	virtual void NotifyEditObject(UObject* Obj) const
	{
		guardSlow(FObjectsItem::NotifyEditObject);
		for (INT i = 0; i < _Objects.Num(); ++i)
			if (_Objects(i))
				_Objects(i)->onEditSubObject(Obj);
		unguardSlow;
	}
};

// Multiple selection object properties.
class WINDOW_API WObjectProperties : public WProperties
{
	W_DECLARE_CLASS(WObjectProperties,WProperties,CLASS_Transient);
	DECLARE_WINDOWCLASS(WObjectProperties,WProperties,Window)

	// Variables.
	FObjectsItem Root;

	// Structors.
	WObjectProperties()
	{}
	WObjectProperties( FName InPersistentName, DWORD InFlagMask, const TCHAR* InCaption, WWindow* InOwnerWindow, UBOOL InByCategory )
	:	WProperties	( InPersistentName, InOwnerWindow )
	,	Root		( this, NULL, InFlagMask, InCaption, InByCategory )
	{}

	// WPropertiesBase interface.
	FTreeItem* GetRoot()
	{
		guard(WObjectProperties::GetRoot);
		return &Root;
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WClassProperties.
-----------------------------------------------------------------------------*/

// Object properties root.
class WINDOW_API FClassItem : public FPropertyItemBase
{
public:
	// Structors.
	FClassItem()
	{}
	FClassItem( WPropertiesBase* InOwnerProperties, FTreeItem* InParent, DWORD InFlagMask, const TCHAR* InCaption, UClass* InBaseClass )
	:	FPropertyItemBase( InOwnerProperties, InParent, InFlagMask, InCaption )
	{
		BaseClass = InBaseClass;
	}

	// FTreeItem interface.
	BYTE* GetReadAddress(UProperty* InProperty, FTreeItem* Top, INT Index = INDEX_NONE)
	{
		guard(FClassItem::GetReadAddress);
		return &BaseClass->Defaults(0);
		unguard;
	}
	void SetProperty(UProperty* Property, FTreeItem* Offset, const TCHAR* Value, INT ExtraFlags = 0);
	void NotePropertyChange(UProperty* Property, FTreeItem* Offset);
	UBOOL CopyFullProperties(FString* Output);
	void SaveObjectConfig()
	{
		guard(FClassItem::SaveObjectConfig);
		BaseClass->SetFlags(RF_SourceModified);
		unguard;
	}
	void Expand()
	{
		guard(FClassItem::Expand);
		TArray<FName> Categories;
		UBOOL bHasHidden = 0;
		for (TFieldIterator<UProperty> It(BaseClass); It; ++It)
		{
			if (AcceptFlags(It->PropertyFlags) && It->Category != NAME_None)
				Categories.AddUniqueItem(It->Category);
			else bHasHidden = 1;
		}
		for( INT i=0; i<Categories.Num(); i++ )
			Children.AddItem(new FCategoryItem(OwnerProperties,this,BaseClass,Categories(i),1) );
		if(bHasHidden)
			Children.AddItem(new FHiddenCategoryList(OwnerProperties, this, BaseClass));
		FTreeItem::Expand();
		unguard;
	}
	UObject* GetParentOuter()
	{
		guard(FClassItem::GetParentOuter);
		return BaseClass;
		unguard;
	}
};


// Multiple selection object properties.
class WINDOW_API WClassProperties : public WProperties
{
	W_DECLARE_CLASS(WClassProperties,WProperties,CLASS_Transient);
	DECLARE_WINDOWCLASS(WClassProperties,WProperties,Window)

	// Variables.
	FClassItem Root;

	// Structors.
	WClassProperties()
	{}
	WClassProperties( FName InPersistentName, DWORD InFlagMask, const TCHAR* InCaption, UClass* InBaseClass )
	:	WProperties	( InPersistentName )
	,	Root		( this, NULL, InFlagMask, InCaption, InBaseClass )
	{}

	// WPropertiesBase interface.
	FTreeItem* GetRoot()
	{
		guard(WClassProperties::GetRoot);
		return &Root;
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WConfigProperties.
-----------------------------------------------------------------------------*/

// Object configuration header.
class WINDOW_API FObjectConfigItem : public FPropertyItemBase
{
public:
	// Variables.
	FString		ClassName;
	FString		CaptionID;
	FName		CategoryFilter;
	UClass*		Class{};
	UObject*	OverrideRes{};
	UBOOL		Failed{};
	UBOOL		Immediate;

	// Structors.
	FObjectConfigItem( WPropertiesBase* InOwnerProperties, FTreeItem* InParent, const TCHAR* InCaption, const TCHAR* InCaptionID, const TCHAR* InClass, UBOOL InImmediate, FName InCategoryFilter )
	:	FPropertyItemBase( InOwnerProperties, InParent, CPF_Config, InCaption )
	,	ClassName		( InClass )
	,	CaptionID		( InCaptionID )
	,	Class			( NULL )
	,	Failed			( 0 )
	,	Immediate		( InImmediate )
	,	CategoryFilter	( InCategoryFilter )
	,	OverrideRes		( NULL )
	{}

	// FTreeItem interface.
	BYTE* GetReadAddress(UProperty* InProperty, FTreeItem* Top, INT Index = INDEX_NONE)
	{
		guard(FObjectsItem::GetReadAddress);
		return &Class->Defaults(0);
		unguard;
	}
	UProperty* FindProperty(const TCHAR* PropName) const;

	void SetProperty(UProperty* Property, FTreeItem* Offset, const TCHAR* Value, INT ExtraFlags = 0);
	UBOOL CopyFullProperties(FString* Output)
	{
		return FALSE;
	}
	void NotePropertyChange(UProperty* Property, FTreeItem* Offset);

	void SaveObjectConfig()
	{
		guard(FObjectConfigItem::SaveObjectConfig);
		Class->GetDefaultObject()->SaveConfig();
		unguard;
	}
	void OnResetToDefaultsButton()
	{
		guard(FObjectConfigItem::OnResetToDefaultsButton);
		LazyLoadClass();
		if( Class )
		{
			UObject::ResetConfig( Class );
			InvalidateRect( OwnerProperties->List, NULL, 1 );
			UpdateWindow( OwnerProperties->List );
		}
		Redraw();
		unguard;
	}
	void OnItemSetFocus()
	{
		FPropertyItemBase::OnItemSetFocus();
		if (!Parent || !Parent->Buttons.Num())
			AddButton( *LocalizeGeneral(TEXT("DefaultsButton"),TEXT("Window")), FDelegate(this,(TDelegate)&FObjectConfigItem::OnResetToDefaultsButton) );
	}
	void Expand()
	{
		guard(FObjectsItem::Expand);
		LazyLoadClass();
		if( Class )
		{
			if( Children.Num()==0 )
			{
				INT NumCategories = 0;
				TArray<FPreferencesInfo> NewPrefs;
				UObject::GetPreferences(NewPrefs, *CaptionID, 0);
				for (INT i = 0; i<NewPrefs.Num(); i++)
				{
					INT j;
					for (j = 0; j<Children.Num(); j++)
					{
						if (appStricmp(*Children(j)->GetCaption(), *NewPrefs(i).Caption) == 0)
							break;
					}
					if (j == Children.Num())
					{
						if (NewPrefs(i).Class != TEXT(""))
						{
							Children.AddItem(new FObjectConfigItem(OwnerProperties, this, *NewPrefs(i).LangCaption, *NewPrefs(i).Caption, *NewPrefs(i).Class, NewPrefs(i).Immediate, NewPrefs(i).Category));
							NumCategories++;
						}

						// STUFF NEEDS TO BE MOVED INSIDE CPP FILE TO BREAK CIRCULAR DEPENDENCY.
						//else
						//{
						//Children.AddItem( newFConfigItem( NewPrefs(i), OwnerProperties, this ) );
						//NumCategories++;
						//}
					}
				}
				Class->GetDefaultObject()->LoadConfig( 1 );//!!
				for (TFieldIterator<UProperty> It(Class); It; ++It)
				{
					if
						((AcceptFlags(It->PropertyFlags))
						&& (Class == It->GetOwnerClass() || !(It->PropertyFlags&CPF_GlobalConfig))
						&& (CategoryFilter == NAME_None || It->Category == CategoryFilter))
						Children.AddItem(new FPropertyItem(OwnerProperties, this, *It, It->GetFName(), It->Offset, -1));
				}
				// Sort categories and properties seperate, so categories stay on top.
				if (Sorted)
				{
					if (NumCategories>1)
						Sort(&Children(0), NumCategories);
					if (NumCategories<Children.Num() - 1)
						Sort(&Children(NumCategories), Children.Num() - NumCategories);
				}

				UBOOL SavedSorted = 0;
				Exchange(SavedSorted, Sorted);
				FTreeItem::Expand();
				Exchange(SavedSorted, Sorted);
			}
			else
			{
				FTreeItem::Expand(); // Causes sort. Does it even matter here?
			}
		}
		else
		{
			Expandable = 0;
			Redraw();
		}
		unguard;
	}

	// FObjectConfigItem interface.
	void LazyLoadClass()
	{
		guard(FObjectConfigItem::LazyLoadClass);
		if( !Class && !Failed )
		{
			Class = UObject::StaticLoadClass( UObject::StaticClass(), NULL, *ClassName, NULL, LOAD_NoWarn, NULL );
			if( !Class )
			{
				Failed = 1;
				Caption = FString::Printf( *LocalizeError(TEXT("FailedConfigLoad"),TEXT("Window")), *ClassName );
			}
		}
		unguard;
	}

	// FTreeItem interface.
	void Serialize( FArchive& Ar )
	{
		guard(FObjectConfigItem::Serialize);
		FPropertyItemBase::Serialize( Ar );
		Ar << CategoryFilter << Class;
		unguard;
	}
	UObject* GetParentOuter()
	{
		guard(FClassItem::GetParentOuter);
		return Class;
		unguard;
	}
};

// An configuration list item.
class WINDOW_API FConfigItem : public FHeaderItem
{
public:
	// Variables.
	FPreferencesInfo Prefs;

	// Constructors.
	FConfigItem()
	{}
	FConfigItem( const FPreferencesInfo& InPrefs, WPropertiesBase* InOwnerProperties, FTreeItem* InParent )
	: FHeaderItem( InOwnerProperties, InParent, 1 )
	, Prefs( InPrefs )
	{}

	// FTreeItem interface.
	QWORD GetId() const
	{
		guard(FConfigItem::GetId);
		return reinterpret_cast<uintptr_t>(this) + ((QWORD)3<<32);
		unguard;
	}
	virtual FString GetCaption() const
	{
		guard(FConfigItem::GetText);
		return Prefs.LangCaption;
		unguard;
	}
	void Expand()
	{
		guard(FConfigItem::Expand);
		TArray<FPreferencesInfo> NewPrefs;
		UObject::GetPreferences( NewPrefs, *Prefs.Caption, 0 );
		for( INT i=0; i<NewPrefs.Num(); i++ )
		{
			INT j;
			for( j=0; j<Children.Num(); j++ )
			{
				if( appStricmp( *Children(j)->GetCaption(), *NewPrefs(i).Caption )==0 )
					break;
			}
			if( j==Children.Num() )
			{
				if( NewPrefs(i).Class!=TEXT("") )
					Children.AddItem(new FObjectConfigItem( OwnerProperties, this, *NewPrefs(i).LangCaption, *NewPrefs(i).Caption, *NewPrefs(i).Class, NewPrefs(i).Immediate, NewPrefs(i).Category ) );
				else
					Children.AddItem(new FConfigItem( NewPrefs(i), OwnerProperties, this ) );
			}
		}
		FTreeItem::Expand();
		unguard;
	}
	void Collapse()
	{
		guard(FConfigItem::Collapse);
		FTreeItem::Collapse();
		EmptyChildren();
		unguard;
	}
};

// Configuration properties.
class WINDOW_API WConfigProperties : public WProperties
{
	W_DECLARE_CLASS(WConfigProperties,WProperties,CLASS_Transient);
	DECLARE_WINDOWCLASS(WConfigProperties,WProperties,Window)

	// Variables.
	FConfigItem Root;

	// Structors.
	WConfigProperties()
	{}
	WConfigProperties(FName InPersistentName, const TCHAR* InTitle, const TCHAR* InLangTitle = nullptr)
	:	WProperties	( InPersistentName )
	,	Root		( FPreferencesInfo(), this, NULL )
	{
		Root.Prefs.Caption = InTitle;
		Root.Prefs.LangCaption = InLangTitle ? InLangTitle : InTitle;
	}

	// WPropertiesBase interface.
	FTreeItem* GetRoot()
	{
		guard(WConfigProperties::GetRoot);
		return &Root;
		unguard;
	}
};

// New class in 227j
class WINDOW_API FEditObjectItem : public FHeaderItem
{
public:
	// Variables.
	UObject** EditClassPtr;
	UObject* OldObjectPtr;
	UObjectProperty* Property;
	UClassProperty* ClassProperty;
	WComboBox*		ComboControl;
	WLabel*			HolderControl;

	// Constructors.
	FEditObjectItem( WPropertiesBase* InOwnerProperties, FTreeItem* InParent, UObject** InObject, UProperty* InProp )
	:	FHeaderItem( InOwnerProperties, InParent, (*InObject!=NULL && !Cast<UClassProperty>(InProp)) )
	,	EditClassPtr( InObject )
	,	OldObjectPtr(*InObject)
	,	Property(CastChecked<UObjectProperty>(InProp))
	,	ClassProperty(Cast<UClassProperty>(InProp))
	,	ComboControl(NULL)
	,	HolderControl(NULL)
	{
		check(EditClassPtr);
	}

	void Draw( HDC hDC )
	{
		guard(FEditObjectItem::Draw);
		if( OldObjectPtr!=*EditClassPtr )
		{
			OldObjectPtr = *EditClassPtr;
			Expandable = (!ClassProperty && *EditClassPtr!=NULL);
			Collapse();
		}
		FHeaderItem::Draw(hDC);
		unguard;
	}

	BYTE* GetReadAddress(UProperty* InProperty, FTreeItem* Top, INT Index = INDEX_NONE)
	{
		guard(FEditObjectItem::GetReadAddress);
		return (BYTE*)*EditClassPtr;
		unguard;
	}
	UProperty* FindProperty(const TCHAR* PropName) const
	{
		guard(FEditObjectItem::FindProperty);
		return (*EditClassPtr) ? FindField<UProperty>((*EditClassPtr)->GetClass(), PropName) : nullptr;
		unguard;
	}

	// FTreeItem interface.
	void Serialize( FArchive& Ar )
	{
		guard(FEditObjectItem::Serialize);
		FHeaderItem::Serialize( Ar );
		Ar << *EditClassPtr;
		unguard;
	}
	QWORD GetId() const
	{
		guard(FEditObjectItem::GetId);
		return reinterpret_cast<uintptr_t>(Property) + ((QWORD)4 << 32);
		unguard;
	}
	virtual FString GetCaption() const
	{
		guard(FEditObjectItem::GetText);
		if( ComboControl && !ClassProperty && (Property->PropertyFlags & CPF_EditInLine) )
			return FString::Printf(TEXT("New [%ls]"),*ComboControl->GetString(ComboControl->GetCurrent()));
		return *EditClassPtr ? (*EditClassPtr)->GetFullName() : TEXT("None");
		unguard;
	}
	UBOOL CopyFullProperties(FString* Output);
	void Expand()
	{
		guard(FEditObjectItem::Expand);
		if( *EditClassPtr && !ClassProperty )
		{
			// Expand to show categories.
			TArray<FName> Categories;
			UBOOL bHasHidden = 0;
			for (TFieldIterator<UProperty> It((*EditClassPtr)->GetClass()); It; ++It)
			{
				if (AcceptFlags(It->PropertyFlags) && It->Category != NAME_None)
					Categories.AddUniqueItem(It->Category);
				else bHasHidden = 1;
			}
			for( INT i=0; i<Categories.Num(); i++ )
				Children.AddItem(new FCategoryItem(OwnerProperties,this,(*EditClassPtr)->GetClass(),Categories(i),1) );
			if (bHasHidden)
				Children.AddItem(new FHiddenCategoryList(OwnerProperties, this, (*EditClassPtr)->GetClass()));
		}
		FTreeItem::Expand();
		unguard;
	}
	void Collapse()
	{
		guard(FEditObjectItem::Collapse);
		FTreeItem::Collapse();
		EmptyChildren();
		unguard;
	}
	void SetFocusToItem()
	{
		guard(FEditObjectItem::SetFocusToItem);
		if( ComboControl )
			SetFocus( *ComboControl );
		unguard;
	}
	void OnItemSetFocus()
	{
		guard(FEditObjectItem::OnItemSetFocus);
		FTreeItem::OnItemSetFocus();
		BYTE bEditInLine = ((Property->PropertyFlags & CPF_EditInLine)!=0 && !ClassProperty);
		BYTE bEditConst = ((Property->PropertyFlags & CPF_EditConst)!=0);
		if( !bEditConst && ((!*EditClassPtr && GIsEditor) || !bEditInLine) )
		{
			if( bEditInLine )
				AddButton( TEXT("New"), FDelegate(this,(TDelegate)&FEditObjectItem::OnCreateNew) );

			// Combo box.
			FRect Rect = GetRect() + FRect(0,0,-1-ButtonWidth,-1);
			Rect.Min.X = OwnerProperties->GetDividerWidth();

			HolderControl = new WLabel( &OwnerProperties->List );
			HolderControl->Snoop = this;
			HolderControl->OpenWindow( 0 );
			FRect HolderRect = Rect.Right(20) + FRect(0,0,0,1);
			HolderControl->MoveWindow( HolderRect, 0 );

			Rect = Rect + FRect(-2,-6,-2,0) - HolderRect.Min;

			ComboControl = new WComboBox( HolderControl );
			ComboControl->Snoop = this;
			ComboControl->SelectionEndOkDelegate     = FDelegate(this,(TDelegate)&FEditObjectItem::ComboSelectionEnd);
			ComboControl->SelectionEndCancelDelegate = FDelegate(this,(TDelegate)&FEditObjectItem::ComboSelectionEndCancel);
			ComboControl->OpenWindow( 0 );
			ComboControl->MoveWindow( Rect, 1 );

			// Find classes matching this property.
			UClass* ParentClass = (Property->PropertyClass ? Property->PropertyClass : UObject::StaticClass());

			if( bEditInLine )
			{
				for( TObjectIterator<UClass> It; It; ++It )
				{
					if( !(It->ClassFlags & (CLASS_Abstract | CLASS_NoUserCreate | CLASS_Transient)) && It->IsChildOf(ParentClass) )
						ComboControl->AddString(It->GetPathName());
				}
				if( ComboControl->GetCount() )
					ComboControl->SetCurrent(0);
			}
			else
			{
				ComboControl->AddString(TEXT("None"));
				if( ClassProperty )
				{
					ParentClass = ClassProperty->MetaClass;
					for( TObjectIterator<UClass> It; It; ++It )
					{
						if( !ParentClass || It->IsChildOf(ParentClass) )
							ComboControl->AddString(It->GetPathName());
					}
				}
				else
				{
					for( FObjectIterator It; It; ++It )
					{
						if( It->GetClass()->IsChildOf(ParentClass) )
							ComboControl->AddString(It->GetPathName());
					}
				}
				MatchComboSelection();
			}

			Redraw();

			if( ComboControl )
				ComboControl->Show(1);
			if( HolderControl )
				HolderControl->Show(1);
			SetFocusToItem();
		}
		if (Parent)
			Parent->NotifyEditObject(*EditClassPtr);
		unguard;
	}
	void MatchComboSelection()
	{
		guard(FEditObjectItem::MatchComboSelection);
		ComboControl->SetCurrent(ComboControl->FindStringExact(*EditClassPtr ? (*EditClassPtr)->GetPathName() : TEXT("None")));
		unguard;
	}
	void OnItemKillFocus( UBOOL Abort )
	{
		guard(FEditObjectItem::OnKillFocus);
		if (ComboControl)
		{
			ComboControl->Show(0);
			ComboControl->DelayedDestroy();
			ComboControl = NULL;
		}
		if (HolderControl)
		{
			HolderControl->Show(0);
			HolderControl->DelayedDestroy();
			HolderControl = NULL;
		}
		FTreeItem::OnItemKillFocus( Abort );
		unguard;
	}
	void ComboSelectionEnd()
	{
		guard(FEditObjectItem::ComboSelectionEnd);
		if (ComboControl)
		{
			if (ClassProperty)
				ChangePtr(FindObject<UClass>(ANY_PACKAGE, *ComboControl->GetString(ComboControl->GetCurrent()), 1));
			else if (!(Property->PropertyFlags & CPF_EditInLine))
				ChangePtr(FindObject<UObject>(ANY_PACKAGE, *ComboControl->GetString(ComboControl->GetCurrent())));
		}
		Redraw();
		unguard;
	}
	void ComboSelectionEndCancel()
	{
		guard(FEditObjectItem::ComboSelectionEndCancel);
		if (ComboControl && (!(Property->PropertyFlags & CPF_EditInLine) || ClassProperty))
			MatchComboSelection();
		Redraw();
		unguard;
	}
	void OnCreateNew()
	{
		guard(FEditObjectItem::OnCreateNew);
		check(ComboControl);
		FString Value = ComboControl->GetString(ComboControl->GetCurrent());
		UClass* Class = FindObject<UClass>(NULL,*Value,1);
		if( Class )
		{
			UObject* ParentObj = GetParentOuter();
			UObject* NewObj = ConstructObject<UObject>(Class, ParentObj->TopOuter());
			NewObj->OnCreateObjectNew(ParentObj, Property);
			ChangePtr(NewObj);
			if (ComboControl)
			{
				ComboControl->Show(0);
				ComboControl->DelayedDestroy();
				ComboControl = NULL;
			}
			if (HolderControl)
			{
				HolderControl->Show(0);
				HolderControl->DelayedDestroy();
				HolderControl = NULL;
			}
			Redraw();
		}
		unguard;
	}
	void SetProperty( UProperty* Property, FTreeItem* Offset, const TCHAR* Value, INT ExtraFlags = 0)
	{
		guard(FEditObjectItem::SetProperty);
		if( *EditClassPtr )
		{
			if( OwnerProperties->NotifyHook )
				OwnerProperties->NotifyHook->NotifyPreChange( OwnerProperties );
			UProperty::_ImportObject = (*EditClassPtr)->GetClass();
			Property->ImportText(Value, Offset->GetReadAddress(Property, this), (PPF_Localized | PPF_PropWindow) | ExtraFlags);
			UProperty::_ImportObject = NULL;
			(*EditClassPtr)->PostEditChange();
			(*EditClassPtr)->onPropertyChange(Property, Offset->GetParentProperty());
			if( OwnerProperties->NotifyHook )
				OwnerProperties->NotifyHook->NotifyPostChange( OwnerProperties );
		}
		unguard;
	}
	void ChangePtr( UObject* NewObj )
	{
		guard(FEditObjectItem::ChangePtr);
		if( GIsEditor )
			Parent->SetProperty(Property,Parent,NewObj ? NewObj->GetPathName() : TEXT("None")); // Allow undo.
		*EditClassPtr = NewObj;
		if (Parent)
			Parent->NotifyEditObject(NewObj);
		unguard;
	}
	virtual void NotifyEditProperty(UProperty* P, UProperty* SubProperty, INT ArrayIndex)
	{
		guard(FEditObjectItem::NotifyEditProperty);
		if (*EditClassPtr)
			(*EditClassPtr)->onPropertyFocus(P, SubProperty, ArrayIndex);
		if (Parent)
			Parent->NotifyEditObject(*EditClassPtr);
		unguard;
	}
	virtual void NotifyEditObject(UObject* Obj) const
	{
		guard(FEditObjectItem::NotifyEditObject);
		if (*EditClassPtr)
			(*EditClassPtr)->onEditSubObject(Obj);
		if (Parent)
			Parent->NotifyEditObject(*EditClassPtr); // Notify down the tree.
		unguard;
	}
	UProperty* GetParentProperty()
	{
		return NULL; // Don't go under this!
	}
	UClass* GetBaseClass()
	{
		guardSlow(FEditObjectItem::GetBaseClass);
		return (*EditClassPtr) ? (*EditClassPtr)->GetClass() : nullptr;
		unguardSlow;
	}
};

/*-----------------------------------------------------------------------------
	WWizardPage.
-----------------------------------------------------------------------------*/

// A wizard page.
class WINDOW_API WWizardPage : public WDialog
{
	W_DECLARE_ABSTRACT_CLASS(WWizardPage,WDialog,CLASS_Transient);
	DECLARE_WINDOWCLASS(WWizardPage,WDialog,Window)

	// Variables.
	WWizardDialog* Owner{};

	// Constructor.
	WWizardPage()
	{}
	WWizardPage( const TCHAR* PageName, INT ControlId, WWizardDialog* InOwner )
	: WDialog	    ( PageName, ControlId, (WWindow*)InOwner )
	, Owner         ( InOwner )
	{}

	// WWizardPage interface.
	virtual void OnCurrent()
	{}
	virtual WWizardPage* GetNext()
	{
		return NULL;
	}
	virtual const FString GetBackText()
	{
		return *LocalizeGeneral(TEXT("BackButton"),TEXT("Window"));
	}
	virtual const FString GetNextText()
	{
		return *LocalizeGeneral(TEXT("NextButton"),TEXT("Window"));
	}
	virtual const FString GetFinishText()
	{
		return TEXT("");
	}
	virtual const FString GetCancelText()
	{
		return *LocalizeGeneral(TEXT("CancelButton"),TEXT("Window"));
	}
	virtual UBOOL GetShow()
	{
		return 1;
	}
	virtual void OnCancel()
	{
		guard(WWizardPage::OnCancel);
		((WDialog*)Owner)->EndDialog( 0 );
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	WWizardDialog.
-----------------------------------------------------------------------------*/

// The wizard frame dialog.
class WINDOW_API WWizardDialog : public WDialog
{
	W_DECLARE_CLASS(WWizardDialog,WDialog,CLASS_Transient);
	DECLARE_WINDOWCLASS(WWizardDialog,WDialog,Window)

	// Variables.
	WCoolButton BackButton;
	WCoolButton NextButton;
	WCoolButton FinishButton;
	WCoolButton CancelButton;
	WLabel PageHolder;
	TArray<WWizardPage*> Pages;
	WWizardPage* CurrentPage;

	// Constructor.
	WWizardDialog()
	: WDialog	    ( TEXT("WizardDialog"), IDDIALOG_WizardDialog )
	, PageHolder    ( this, IDC_PageHolder )
	, BackButton    ( this, IDC_Back,   FDelegate(this,(TDelegate)&WWizardDialog::OnBack  ) )
	, NextButton    ( this, IDC_Next,   FDelegate(this,(TDelegate)&WWizardDialog::OnNext  ) )
	, FinishButton  ( this, IDC_Finish, FDelegate(this,(TDelegate)&WWizardDialog::OnFinish) )
	, CancelButton  ( this, IDCANCEL,   FDelegate(this,(TDelegate)&WWizardDialog::OnCancel) )
	, Pages         ()
	, CurrentPage   ( NULL )
	{}

	// WDialog interface.
	void OnInitDialog()
	{
		guard(WWizardDialog::OnInitDialog);
		WDialog::OnInitDialog();
		SendMessageW( *this, WM_SETICON, ICON_BIG, (WPARAM)LoadIconW(hInstance,MAKEINTRESOURCEW((GIsEditor?IDICON_Editor:IDICON_Mainframe))) );
		RefreshPage();
		unguard;
	}

	// WWizardDialog interface.
	virtual void Advance( WWizardPage* NewPage )
	{
		guard(WWizardDialog::Advanced);
		check(NewPage);
		Pages.AddItem( NewPage );
		if( hWnd )
			RefreshPage();
		if( !Pages.Last()->GetShow() )
			OnNext();
		unguard;
	}
	virtual void RefreshPage()
	{
		guard(WWizardDialog::RefreshPage);
		if( Pages.Num() )
		{
			WWizardPage* Page = Pages.Last();
			if( !Page->hWnd )
				Page->OpenChildWindow( IDC_PageHolder, 1 );
			BackButton  .SetVisibleText( Pages.Num()>1 ? *Page->GetBackText() : TEXT("") );
			NextButton  .SetVisibleText( *Page->GetNextText  () );
			FinishButton.SetVisibleText( *Page->GetFinishText() );
			CancelButton.SetVisibleText( *Page->GetCancelText() );
			if( Pages.Num()>1 )
				Pages(Pages.Num()-2)->Show(FALSE);
			Pages.Last()->OnCurrent();
		}
		unguard;
	}
	virtual void OnDestroy()
	{
		guard(WWizardDialog::OnDestroy);
		for (INT i = 0; i < Pages.Num(); i++)
		{
			Pages(i)->Show(FALSE);
			Pages(i)->DelayedDestroy();
		}
		WDialog::OnDestroy();
		unguard;
	}
	virtual void OnBack()
	{
		guard(WWizardDialog::OnBack);
		if( Pages.Num()>1 )
		{
			Pages(Pages.Num() - 1)->Show(FALSE);
			Pages(Pages.Num() - 2)->Show(TRUE);
			Pages.Pop()->DelayedDestroy();
			RefreshPage();
			if( !Pages.Last()->GetShow() )
				OnBack();
		}
		unguard;
	}
	virtual void OnNext()
	{
		guard(WWizardDialog::OnNext);
		if( Pages.Num() && Pages.Last()->GetNextText() )
		{
			WWizardPage* GotNext = Pages.Last()->GetNext();
			if( GotNext )
				Advance( GotNext );
		}
		unguard;
	}
	virtual void OnFinish()
	{
		guard(WWizardDialog::OnFinish);
		EndDialog( 1 );
		unguard;
	}
	virtual void OnCancel()
	{
		guard(WWizardDialog::OnCancel);
		Pages.Last()->OnCancel();
		unguard;
	}
	void OnClose()
	{
		guard(WLog::OnClose);
		Pages.Last()->OnCancel();
		throw TEXT("NoRoute");
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	FCustomCursor.
-----------------------------------------------------------------------------*/

struct FAnimCursorData
{
	BYTE* Data;
	INT USize, VSize, UClip, VClip, HotSpotX, HotSpotY;
	FColor* Palette;

	FAnimCursorData()
	{}
	FAnimCursorData(BYTE* D, INT U, INT V, INT UC, INT VC, INT HX, INT HY, FColor* Pal)
		: Data(D), USize(U), VSize(V), UClip(UC), VClip(VC), HotSpotX(HX), HotSpotY(HY), Palette(Pal)
	{}
};

class WINDOW_API FCustomCursor
{
private:
	HCURSOR Cursor;

public:
	FCustomCursor()
		: Cursor(NULL)
	{}
	~FCustomCursor() noexcept(false);

	// Create cursor from raw .cur or .ani data stream.
	UBOOL CreateCursorRaw(BYTE* Data, INT DataSize);

	// Create cursor from multiple frames of P8 palette texture (AnimRate is defined in X * (1/60)).
	UBOOL CreateCursorTex(FAnimCursorData* Data, INT NumFrames, INT AnimRate=10);

	inline HCURSOR GetCursor() const
	{
		return Cursor;
	}
};

/*-----------------------------------------------------------------------------
	FCustomIcon.
-----------------------------------------------------------------------------*/

struct FAnimIconData
{
	BYTE* Data;
	INT USize, VSize, UClip, VClip;
	FColor* Palette;

	FAnimIconData()
	{}
	FAnimIconData(BYTE* D, INT U, INT V, INT UC, INT VC, FColor* Pal)
		: Data(D), USize(U), VSize(V), UClip(UC), VClip(VC), Palette(Pal)
	{}
};

class WINDOW_API FCustomBitmap
{
private:
	HBITMAP Bmp;

public:
	FCustomBitmap()
		: Bmp(NULL)
	{}
	~FCustomBitmap() noexcept(false);

	// Create icon from a P8 palette texture.
	UBOOL CreateIconTex(const FAnimIconData& Data, const FColor* MaskColor = NULL);

	inline HBITMAP& GetBitmap()
	{
		return Bmp;
	}
};

/*-----------------------------------------------------------------------------
	Custom Right-Click DropDown menu.
-----------------------------------------------------------------------------*/

struct WINDOW_API FRightClickMenu
{
private:
	HMENU hRightClickMenu;
	TArray<HMENU> PopupStack, AllGroups;

public:
	~FRightClickMenu() noexcept(false);
	FRightClickMenu();
	INT OpenMenu(HWND MainWnd);
	void AddItem(INT Id = 0, const TCHAR* Text = NULL, UBOOL bDisabled = 0, UBOOL bChecked = 0);
	HMENU AddPopup(const TCHAR* Text);
	void EndPopup();
	HMENU GetCurrentStack();
	void RemoveItem(INT Id, HMENU StackLevel = NULL);
};

typedef void (*MenuCallbackFunc)(INT);

struct FPersistentRCMenu;
struct FMenuGroup;
struct FMenuItem;

struct WINDOW_API FMenuItemBase : public FOutputDevice
{
	friend FPersistentRCMenu;
	friend FMenuGroup;
	friend FMenuItem;
protected:
	FMenuGroup* GroupOwner;
	FPersistentRCMenu* MenuOwner;
	INT Offset;
	UBOOL bIsGroup, bIsHidden;
	FMenuItemBase* Next;

	FMenuItemBase(UBOOL Group, FMenuGroup* O, FPersistentRCMenu* M, UBOOL Hidden)
		: GroupOwner(O), MenuOwner(M), Offset(0), bIsGroup(Group), bIsHidden(Hidden), Next(NULL)
	{}
	void MoveOffset(INT Dir);

public:
	virtual ~FMenuItemBase() noexcept(false);

	inline UBOOL IsHidden() const
	{
		return bIsHidden;
	}
};
struct WINDOW_API FMenuItem : public FMenuItemBase
{
	friend FPersistentRCMenu;
	friend FMenuGroup;
protected:
	UBOOL bIsDisabled, bIsChecked;
	INT InternalID;
	FString CurText, OrgText, PostFixText;

	FMenuItem(FMenuGroup* O, FPersistentRCMenu* M, INT IntID, UBOOL Hidden, UBOOL Disable, UBOOL Check, MenuCallbackFunc C, INT vID, const TCHAR* Txt, const TCHAR* PostTxt=NULL);
	void UpdateMenu();

public:
	MenuCallbackFunc Callback;
	INT ID;

	~FMenuItem() noexcept(false);

	void Serialize(const TCHAR* V, EName Event);
	void SetDisabled(UBOOL bDisabled);
	void SetChecked(UBOOL bChecked);
	void SetHidden(UBOOL bHidden);
	void Execute();

	inline UBOOL IsDisabled() const
	{
		return bIsDisabled;
	}
	inline UBOOL IsChecked() const
	{
		return bIsChecked;
	}
	inline const TCHAR* GetText() const
	{
		return *CurText;
	}
};
struct WINDOW_API FMenuGroup : public FMenuItemBase
{
	friend FMenuItemBase;
	friend FPersistentRCMenu;
	friend FMenuItem;
protected:
	HMENU MenuHandle;
	FString CurText;

	FMenuItemBase* List;

	// Temporary data.
	FMenuItemBase* Current;
	INT ItemOffset;

	inline void AddItem(FMenuItemBase* I)
	{
		I->Offset = ItemOffset;
		if(!I->bIsHidden)
			++ItemOffset;
		if (Current)
			Current->Next = I;
		else List = I;
		Current = I;
	}

	FMenuGroup(FPersistentRCMenu* O);
	FMenuGroup(FPersistentRCMenu* O, FMenuGroup* P, const TCHAR* Str, UBOOL Hidden);
public:
	~FMenuGroup() noexcept(false);

	void Serialize(const TCHAR* V, EName Event);
	void SetHidden(UBOOL bHidden);

	inline UBOOL IsHidden() const
	{
		return bIsHidden;
	}
	inline const TCHAR* GetText() const
	{
		return *CurText;
	}

	FMenuItem* AddItem(const TCHAR* Text, MenuCallbackFunc Callback = NULL, INT ID = 0, UBOOL bDisabled = 0, UBOOL bChecked = 0, UBOOL bHidden = 0, const TCHAR* PostText = NULL);
	void AddLineBreak();
};

// Easy persistent menu system.
struct WINDOW_API FPersistentRCMenu
{
	friend struct FMenuItem;
	friend struct FMenuGroup;
protected:
	FMenuGroup MainGroup;
	TArray<FMenuGroup*> Stack;
	TArray<FMenuItem*> AllItems;
	FMenuGroup* TopStack;

public:
	UBOOL bMenuCreated;

	FPersistentRCMenu();
	~FPersistentRCMenu() noexcept(false);

	FMenuItem* AddItem(const TCHAR* Text, MenuCallbackFunc Callback = NULL, INT ID = 0, UBOOL bDisabled = 0, UBOOL bChecked = 0, UBOOL bHidden = 0, const TCHAR* PostText = NULL);
	void AddLineBreak();
	FMenuGroup* AddPopup(const TCHAR* Text, UBOOL bHidden = 0);
	void EndPopup();

	INT OpenMenu(HWND MainWnd);
};

class WINDOW_API UWindowManager : public USubsystem
{
	DECLARE_CLASS(UWindowManager, UObject, CLASS_Transient, Window);

	// Constructor.
	UWindowManager();

	// FExec interface.
	UBOOL Exec(const TCHAR* Cmd, FOutputDevice& Ar);

	// UObject interface.
	void Serialize(FArchive& Ar);
	void Destroy();

	// USubsystem interface.
	void Tick(FLOAT DeltaTime);
};

#if __STATIC_LINK
#define AUTO_INITIALIZE_REGISTRANTS_WINDOW \
	UWindowManager::StaticClass();
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

