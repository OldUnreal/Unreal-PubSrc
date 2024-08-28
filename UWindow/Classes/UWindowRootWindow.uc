//=============================================================================
// UWindowRootWindow - the root window.
//=============================================================================
class UWindowRootWindow extends UWindowWindow;

#exec TEXTURE IMPORT NAME=MouseCursor FILE=Textures\MouseCursor.pcx GROUP="Icons" FLAGS=2 MIPS=OFF
#exec TEXTURE IMPORT NAME=MouseMove FILE=Textures\MouseMove.pcx GROUP="Icons" FLAGS=2 MIPS=OFF
#exec TEXTURE IMPORT NAME=MouseDiag1 FILE=Textures\MouseDiag1.pcx GROUP="Icons" FLAGS=2 MIPS=OFF
#exec TEXTURE IMPORT NAME=MouseDiag2 FILE=Textures\MouseDiag2.pcx GROUP="Icons" FLAGS=2 MIPS=OFF
#exec TEXTURE IMPORT NAME=MouseNS FILE=Textures\MouseNS.pcx GROUP="Icons" FLAGS=2 MIPS=OFF
#exec TEXTURE IMPORT NAME=MouseWE FILE=Textures\MouseWE.pcx GROUP="Icons" FLAGS=2 MIPS=OFF
#exec TEXTURE IMPORT NAME=MouseHand FILE=Textures\MouseHand.pcx GROUP="Icons" FLAGS=2 MIPS=OFF
#exec TEXTURE IMPORT NAME=MouseHSplit FILE=Textures\MouseHSplit.pcx GROUP="Icons" FLAGS=2 MIPS=OFF
#exec TEXTURE IMPORT NAME=MouseVSplit FILE=Textures\MouseVSplit.pcx GROUP="Icons" FLAGS=2 MIPS=OFF
#exec TEXTURE IMPORT NAME=MouseWait FILE=Textures\MouseWait.pcx GROUP="Icons" FLAGS=2 MIPS=OFF

var UWindowWindow		MouseWindow;		// The window the mouse is over
var bool				bMouseCapture;
var float				MouseX, MouseY;
var float				OldMouseX, OldMouseY;
var bool				bHandledWindowEvent;
var WindowConsole		Console;
var UWindowWindow		FocusedWindow;
var UWindowWindow		KeyFocusWindow;		// window with keyboard focus
var MouseCursor			NormalCursor, MoveCursor, DiagCursor1, HandCursor, HSplitCursor, VSplitCursor, DiagCursor2, NSCursor, WECursor, WaitCursor;
var bool				bQuickKeyEnable;
var UWindowHotkeyWindowList	HotkeyWindows;
var float		        GUIScale,GUIFontScale;
var config float        ConfiguredGUIScale;
var config bool         AutoGUIScale;
var float				RealWidth, RealHeight;
var Font				Fonts[10];
var UWindowLookAndFeel	LooksAndFeels[20];
var config string		LookAndFeelClass;
var bool				bRequestQuit;
var float				QuitTime;
var bool				bAllowConsole;

// Debug mode:
var bool				bDisplayDebugMouse,bModifyPageNow,bModifyPageScale,bIgnoreHidesNow;
var float BeginMousePos[2];
var UWindowWindow EditingComponent;

function BeginPlay()
{
	Root = Self;
	MouseWindow = Self;
	KeyFocusWindow = Self;
	GUIScale = 1.0;
}

function UWindowLookAndFeel GetLookAndFeel(String LFClassName)
{
	local int i;
	local class<UWindowLookAndFeel> LFClass;

	LFClass = class<UWindowLookAndFeel>(DynamicLoadObject(LFClassName, class'Class'));

	for (i=0; i<20; i++)
	{
		if (LooksAndFeels[i] == None)
		{
			LooksAndFeels[i] = new LFClass;
			LooksAndFeels[i].Setup();
			return LooksAndFeels[i];
		}

		if (LooksAndFeels[i].Class == LFClass)
			return LooksAndFeels[i];
	}
	Log("Out of LookAndFeel array space!!");
	return None;
}


function Created()
{
	LookAndFeel = GetLookAndFeel(LookAndFeelClass);
	SetScale(ConfiguredGUIScale);
	SetupFonts();

	NormalCursor.tex = Texture'MouseCursor';
	NormalCursor.HotX = 0;
	NormalCursor.HotY = 0;
	NormalCursor.WindowsCursor = Console.Viewport.IDC_ARROW;

	MoveCursor.tex = Texture'MouseMove';
	MoveCursor.HotX = 8;
	MoveCursor.HotY = 8;
	MoveCursor.WindowsCursor = Console.Viewport.IDC_SIZEALL;

	DiagCursor1.tex = Texture'MouseDiag1';
	DiagCursor1.HotX = 8;
	DiagCursor1.HotY = 8;
	DiagCursor1.WindowsCursor = Console.Viewport.IDC_SIZENWSE;

	HandCursor.tex = Texture'MouseHand';
	HandCursor.HotX = 11;
	HandCursor.HotY = 1;
	HandCursor.WindowsCursor = 7;

	HSplitCursor.tex = Texture'MouseHSplit';
	HSplitCursor.HotX = 9;
	HSplitCursor.HotY = 9;
	HSplitCursor.WindowsCursor = Console.Viewport.IDC_SIZEWE;

	VSplitCursor.tex = Texture'MouseVSplit';
	VSplitCursor.HotX = 9;
	VSplitCursor.HotY = 9;
	VSplitCursor.WindowsCursor = Console.Viewport.IDC_SIZENS;

	DiagCursor2.tex = Texture'MouseDiag2';
	DiagCursor2.HotX = 7;
	DiagCursor2.HotY = 7;
	DiagCursor2.WindowsCursor = Console.Viewport.IDC_SIZENESW;

	NSCursor.tex = Texture'MouseNS';
	NSCursor.HotX = 3;
	NSCursor.HotY = 7;
	NSCursor.WindowsCursor = Console.Viewport.IDC_SIZENS;

	WECursor.tex = Texture'MouseWE';
	WECursor.HotX = 7;
	WECursor.HotY = 3;
	WECursor.WindowsCursor = Console.Viewport.IDC_SIZEWE;

	WaitCursor.tex = Texture'MouseWait';
	WaitCursor.HotX = 6;
	WaitCursor.HotY = 9;
	WaitCursor.WindowsCursor = Console.Viewport.IDC_WAIT;


	HotkeyWindows = New class'UWindowHotkeyWindowList';
	HotkeyWindows.Last = HotkeyWindows;
	HotkeyWindows.Next = None;
	HotkeyWindows.Sentinel = HotkeyWindows;

	Cursor = NormalCursor;
}

function MoveMouse(float X, float Y)
{
	local UWindowWindow NewMouseWindow;
	local float tx, ty;

	MouseX = X;
	MouseY = Y;

	if (!bMouseCapture)
		NewMouseWindow = FindWindowUnder(X, Y);
	else
		NewMouseWindow = MouseWindow;

	if (NewMouseWindow != MouseWindow)
	{
		MouseWindow.MouseLeave();
		NewMouseWindow.MouseEnter();
		MouseWindow = NewMouseWindow;
	}

	if (MouseX != OldMouseX || MouseY != OldMouseY)
	{
		OldMouseX = MouseX;
		OldMouseY = MouseY;

		MouseWindow.GetMouseXY(tx, ty);
		MouseWindow.MouseMove(tx, ty);
	}
	if ( bModifyPageNow && EditingComponent!=None )
	{
		if ( !bModifyPageScale )
		{
			EditingComponent.WinLeft+=(X-BeginMousePos[0]);
			EditingComponent.WinTop+=(Y-BeginMousePos[1]);
		}
		else
		{
			EditingComponent.WinWidth+=(X-BeginMousePos[0]);
			EditingComponent.WinHeight+=(Y-BeginMousePos[1]);
		}
		BeginMousePos[0] = X;
		BeginMousePos[1] = Y;
	}
}

function DrawMouse(Canvas C)
{
	if (Console.Viewport.bWindowsMouseAvailable)
	{
		// Set the windows cursor...
		Console.Viewport.CustomCursor = LookAndFeel.MousePointers[MouseWindow.Cursor.WindowsCursor];
	}
	else
	{
		C.DrawColor.R = 255;
		C.DrawColor.G = 255;
		C.DrawColor.B = 255;
		C.bNoSmooth = True;

		C.SetPos(MouseX * GUIScale - MouseWindow.Cursor.HotX, MouseY * GUIScale - MouseWindow.Cursor.HotY);
		C.DrawIcon(MouseWindow.Cursor.tex, 1.0);
	}
	if ( bDisplayDebugMouse )
		DrawMouseDebug(C);
}
function DrawMouseDebug( Canvas C )
{
	local float X,Y;

	MouseWindow.GetMouseXY(X, Y);
	C.Font = Fonts[F_Normal];

	C.DrawColor.R = 0;
	C.DrawColor.G = 0;
	C.DrawColor.B = 0;
	C.SetPos(MouseX * GUIScale - MouseWindow.Cursor.HotX, MouseY * GUIScale - MouseWindow.Cursor.HotY);
	C.DrawText( GetNameOfObj(MouseWindow)$" "$int(MouseX * GUIScale)$", "$int(MouseY * GUIScale)$" ("$int(X)$", "$int(Y)$")");
	C.SetPos(MouseX * GUIScale - MouseWindow.Cursor.HotX, MouseY * GUIScale - MouseWindow.Cursor.HotY+9.f*GUIScale);
	C.DrawText("Parent:"@GetNameOfObj(MouseWindow.ParentWindow)@"Active:"@GetNameOfObj(MouseWindow.ActiveWindow)@"Owner:"@GetNameOfObj(MouseWindow.OwnerWindow));

	C.DrawColor.R = 255;
	C.DrawColor.G = 255;
	C.DrawColor.B = 0;
	C.SetPos(-1 + MouseX * GUIScale - MouseWindow.Cursor.HotX, -1 + MouseY * GUIScale - MouseWindow.Cursor.HotY);
	C.DrawText( GetNameOfObj(MouseWindow)$" "$int(MouseX * GUIScale)$", "$int(MouseY * GUIScale)$" ("$int(X)$", "$int(Y)$")");
	C.SetPos(-1 + MouseX * GUIScale - MouseWindow.Cursor.HotX, -1 + MouseY * GUIScale - MouseWindow.Cursor.HotY+9.f*GUIScale);
	C.DrawText("Parent:"@GetNameOfObj(MouseWindow.ParentWindow)@"Active:"@GetNameOfObj(MouseWindow.ActiveWindow)@"Owner:"@GetNameOfObj(MouseWindow.OwnerWindow));
}
final function string GetNameOfObj( Object Other )
{
	if ( Other==None )
		Return "None";
	Return string(Other.Name);
}

function bool CheckCaptureMouseUp()
{
	local float X, Y;

	if (bMouseCapture)
	{
		MouseWindow.GetMouseXY(X, Y);
		MouseWindow.LMouseUp(X, Y);
		bMouseCapture = False;
		return True;
	}
	return False;
}

function bool CheckCaptureMouseDown()
{
	local float X, Y;

	if (bMouseCapture)
	{
		MouseWindow.GetMouseXY(X, Y);
		MouseWindow.LMouseDown(X, Y);
		bMouseCapture = False;
		return True;
	}
	return False;
}


function CancelCapture()
{
	bMouseCapture = False;
}


function CaptureMouse(optional UWindowWindow W)
{
	bMouseCapture = True;
	if (W != None)
		MouseWindow = W;
	//Log(MouseWindow.Class$": Captured Mouse");
}

function Texture GetLookAndFeelTexture()
{
	Return LookAndFeel.Active;
}

function bool IsActive()
{
	Return True;
}

function AddHotkeyWindow(UWindowWindow W)
{
//	Log("Adding hotkeys for "$W);
	UWindowHotkeyWindowList(HotkeyWindows.Insert(class'UWindowHotkeyWindowList')).Window = W;
}

function RemoveHotkeyWindow(UWindowWindow W)
{
	local UWindowHotkeyWindowList L;

//	Log("Removing hotkeys for "$W);

	L = HotkeyWindows.FindWindow(W);
	if (L != None)
		L.Remove();
}


function WindowEvent(WinMessage Msg, Canvas C, float X, float Y, int Key)
{
	local PlayerPawn P;

	bHandledWindowEvent = false;

	P = GetPlayerOwner();
	switch (Msg)
	{
	case WM_KeyDown:
		if ( bDisplayDebugMouse )
		{
			if ( Key==P.EInputKey.IK_Ctrl )
			{
				bModifyPageScale = False;
				if ( !bModifyPageNow )
				{
					bModifyPageNow = True;
					EditingComponent = None;
				}
				Return;
			}
			else if ( Key==P.EInputKey.IK_Shift )
			{
				bModifyPageScale = True;
				if ( !bModifyPageNow )
				{
					bModifyPageNow = True;
					EditingComponent = None;
				}
				Return;
			}
		}
		if (HotKeyDown(Key, X, Y))
			return;
		break;
	case WM_KeyUp:
		if ( bDisplayDebugMouse && (Key==P.EInputKey.IK_Ctrl || Key==P.EInputKey.IK_Shift) )
		{
			bModifyPageNow = False;
			EditingComponent = None;
			Return;
		}
		else if (HotKeyUp(Key, X, Y))
			return;
		break;
	case WM_LMouseDown:
		if ( bModifyPageNow && !EditingComponent )
		{
			BeginMousePos[0] = MouseX;
			BeginMousePos[1] = MouseY;
			if ( MouseWindow && MouseWindow!=Self )
				EditingComponent = MouseWindow;
			Return;
		}
		break;
	case WM_LMouseUp:
		if ( bModifyPageNow )
		{
			EditingComponent = None;
			Return;
		}
		break;
	case WM_Paint:
		if ( bIgnoreHidesNow )
		{
			bIgnoreHidesNow = False;
			if ( !HasActiveWindow() )
			{
				Console.CloseUWindow();
				Return;
			}
		}
	}

	Super.WindowEvent(Msg, C, X, Y, Key);
}


function bool HotKeyDown(int Key, float X, float Y)
{
	local UWindowHotkeyWindowList l;

	l = UWindowHotkeyWindowList(HotkeyWindows.Next);
	while( l )
	{
		if (l.Window != Self && l.Window.HotKeyDown(Key, X, Y)) return True;
		l = UWindowHotkeyWindowList(l.Next);
	}

	return False;
}

function bool HotKeyUp(int Key, float X, float Y)
{
	local UWindowHotkeyWindowList l;

	l = UWindowHotkeyWindowList(HotkeyWindows.Next);
	while( l )
	{
		if ( l.Window != Self && l.Window.HotKeyUp(Key, X, Y) ) return True;
		l = UWindowHotkeyWindowList(l.Next);
	}

	return False;
}

function CloseActiveWindow()
{
	EscClosing();
	if (bHandledWindowEvent)
		return;

	if (ActiveWindow != None)
		ActiveWindow.EscClose();
	else
		Console.CloseUWindow();
}

function Resized()
{
	ResolutionChanged(WinWidth, WinHeight);
}

function SetScale(float NewScale)
{
    ConfiguredGUIScale = NewScale;
	CalculateEffectiveGUIScale();
	
	WinWidth = RealWidth / GUIScale;
	WinHeight = RealHeight / GUIScale;

	ClippingRegion.X = 0;
	ClippingRegion.Y = 0;
	ClippingRegion.W = WinWidth;
	ClippingRegion.H = WinHeight;

	SetupFonts();

	Resized();
}

function CalculateEffectiveGUIScale()
{
	local float Scale;
	local int ClosestSupportedScale;

	if (AutoGUIScale)
	{
		Scale = RealHeight / 720;

		ClosestSupportedScale = int(Scale * 100.f);
		
		// make sure it's divisible by 25
		ClosestSupportedScale -= ClosestSupportedScale % 25;
		// clamp
		ClosestSupportedScale = Max(Min(ClosestSupportedScale, 300), 100);

		GUIScale = float(ClosestSupportedScale) / 100.0;
		//Log("Calculated Effective GUI Scale:"@GUIScale);
	}
	else
	{
		GUIScale = ConfiguredGUIScale;
		//Log("Using Configured GUI Scale:"@GUIScale);
	}
}

static final function Font GetLocaleFont( string FontID )
{
	return Font(DynamicLoadObject(Localize("FontStyle",FontID,"UWindow","UWindowFonts."$FontID), class'Font'));
}
function SetupFonts()
{
	local int i;

	GUIFontScale = GUIScale;
	if(GUIFontScale >= 3.f)
	{
		GUIFontScale /= 3.f;
		Fonts[F_Normal] = GetLocaleFont("Tahoma30");
		Fonts[F_Normal] = GetLocaleFont("Tahoma30");
		Fonts[F_Bold] = GetLocaleFont("TahomaB30");
		Fonts[F_Large] = GetLocaleFont("Tahoma40");
		Fonts[F_LargeBold] = GetLocaleFont("TahomaB40");
	}
	else if(GUIFontScale >= 2.75)
	{
		GUIFontScale /= 2.75f;
		Fonts[F_Normal] = GetLocaleFont("Tahoma27");
		Fonts[F_Bold] = GetLocaleFont("TahomaB27");
		Fonts[F_Large] = GetLocaleFont("Tahoma37");
		Fonts[F_LargeBold] = GetLocaleFont("TahomaB37");
	}
	else if(GUIFontScale >= 2.5)
	{
		GUIFontScale /= 2.5;
		Fonts[F_Normal] = GetLocaleFont("Tahoma25");
		Fonts[F_Bold] = GetLocaleFont("TahomaB25");
		Fonts[F_Large] = GetLocaleFont("Tahoma35");
		Fonts[F_LargeBold] = GetLocaleFont("TahomaB35");
	}
	else if(GUIFontScale >= 2.25)
	{
		GUIFontScale /= 2.25;
		Fonts[F_Normal] = GetLocaleFont("Tahoma22");
		Fonts[F_Bold] = GetLocaleFont("TahomaB22");
		Fonts[F_Large] = GetLocaleFont("Tahoma32");
		Fonts[F_LargeBold] = GetLocaleFont("TahomaB32");
	}
	else if(GUIFontScale >= 2.f)
	{
		GUIFontScale /= 2.f;
		Fonts[F_Normal] = GetLocaleFont("Tahoma20");
		Fonts[F_Bold] = GetLocaleFont("TahomaB20");
		Fonts[F_Large] = GetLocaleFont("Tahoma30");
		Fonts[F_LargeBold] = GetLocaleFont("TahomaB30");
	}
	else if(GUIFontScale >= 1.75)
	{
		GUIFontScale /= 1.75f;
		Fonts[F_Normal] = GetLocaleFont("Tahoma17");
		Fonts[F_Bold] = GetLocaleFont("TahomaB17");
		Fonts[F_Large] = GetLocaleFont("Tahoma27");
		Fonts[F_LargeBold] = GetLocaleFont("TahomaB27");
	}
	else if(GUIFontScale >= 1.5)
	{
		GUIFontScale /= 1.5f;
		Fonts[F_Normal] = GetLocaleFont("Tahoma15");
		Fonts[F_Bold] = GetLocaleFont("TahomaB15");
		Fonts[F_Large] = GetLocaleFont("Tahoma25");
		Fonts[F_LargeBold] = GetLocaleFont("TahomaB25");
	}
	else if(GUIFontScale >= 1.25)
	{
		GUIFontScale /= 1.25f;
		Fonts[F_Normal] = GetLocaleFont("Tahoma12");
		Fonts[F_Bold] = GetLocaleFont("TahomaB12");
		Fonts[F_Large] = GetLocaleFont("Tahoma22");
		Fonts[F_LargeBold] = GetLocaleFont("TahomaB20");
	}
	else
	{
		Fonts[F_Normal] = GetLocaleFont("Tahoma10");
		Fonts[F_Bold] = GetLocaleFont("TahomaB10");
		Fonts[F_Large] = GetLocaleFont("Tahoma20");
		Fonts[F_LargeBold] = GetLocaleFont("TahomaB20");
	}
	
	/* Make sure none of the fonts are NULL once were here */
	for( i=0; i<ArrayCount(Fonts); ++i )
		if ( !Fonts[i] )
			Fonts[i] = Font'MedFont';
}

function ChangeLookAndFeel(string NewLookAndFeel)
{
	LookAndFeelClass = NewLookAndFeel;
	SaveConfig();

	// Completely restart UWindow system on the next paint
	Console.ResetUWindow();
}

function HideWindow()
{
}

function SetMousePos(float X, float Y)
{
	Console.MouseX = X;
	Console.MouseY = Y;
}

function QuitGame()
{
	bRequestQuit = True;
	QuitTime = 0;
}

function DoQuitGame()
{
	SaveConfig();
	Console.SaveConfig();
	Console.ViewPort.Actor.SaveConfig();
	Close();
	Console.Viewport.Actor.ConsoleCommand("exit");
}

function Tick(float Delta)
{
	if (bRequestQuit)
	{
		// Give everything time to close itself down (ie sockets).
		if (QuitTime > 0.25)
			DoQuitGame();
		QuitTime += Delta;
	}

	Super.Tick(Delta);
}

function NotifyBeforeLevelChange()
{
	// 227: Allow custom menus to get gc'd
	MouseWindow = Self;
	KeyFocusWindow = Self;
	FocusedWindow = None;
	
	Super.NotifyBeforeLevelChange();
}

//===================================================================================
// New in 227: Copy text to UWindow clip board.
final function CopyText( string Text )
{
	class'PlayerPawn'.Static.CopyToClipboard(Text);
}
// Paste text from UWindow clip board.
final function string PasteText()
{
	Return class'PlayerPawn'.Static.PasteFromClipboard();
}

// Check if should end UWindow state (no windows visible):
function CheckUWindowActivation()
{
	if ( bIgnoreHidesNow || HasActiveWindow() )
		Return;
	Console.CloseUWindow();
}

function SetContextHelp(string Text); // implemented in subclasses
//===================================================================================

defaultproperties
{
	ConfiguredGUIScale=1
	AutoGUIScale=true
	bAllowConsole=True
}