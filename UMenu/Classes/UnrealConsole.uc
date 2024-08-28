class UnrealConsole extends WindowConsole;

// Show UMenu load game menu.
simulated function ShowLoadGameMenu()
{
	// Create load game dialog.
	Root.CreateWindow(class'UMenuLoadGameWindow', 100, 100, 200, 200, None, True);
}

event PreloginError( string ServerAddr, out string Error, string FailCode )
{
	Super.PreloginError(ServerAddr,Error,FailCode);
	if( (FailCode=="NEEDPW" || FailCode=="WRONGPW" || Error==Class'GameInfo'.Default.NeedPassword || Error==Class'GameInfo'.Default.WrongPassword) && Root )
		class'UBrowserJoinPW'.Static.ShowPasswordMenu(Root,Error,ServerAddr);
}

defaultproperties
{
	RootWindow="UMenu.UMenuRootWindow"
	ConsoleClass=Class'UMenu.UMenuConsoleWindow'
	MouseScale=0.900000
}
