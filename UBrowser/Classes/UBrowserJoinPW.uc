//=============================================================================
// UBrowserJoinPW.
//=============================================================================
class UBrowserJoinPW expands UWindowFramedWindow;

var UBrowserJoinPWD EditArea;

function Created()
{
	Super.Created();
	EditArea = UBrowserJoinPWD(ClientArea);
}

static final function ShowPasswordMenu( UWindowRootWindow R, string Header, string URL )
{
	local UBrowserJoinPW PWWindow;

	R.Console.bQuickKeyEnable = True;
	R.Console.LaunchUWindow();
	PWWindow = UBrowserJoinPW(R.CreateWindow(class'UBrowserJoinPW',(R.WinWidth-300)/2, (R.WinHeight-80)/2,295,80,,true));
	PWWindow.WindowTitle = Header$" - "$URL;
	PWWindow.EditArea.JoiningServerInfo = None;
	PWWindow.EditArea.ReceiveAddressInfo(URL);
}

defaultproperties
{
	ClientClass=Class'UBrowser.UBrowserJoinPWD'
	bLeaveOnscreen=True
}