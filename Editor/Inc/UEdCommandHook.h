	UEdCommandHook() {}
	
	static TArray<UEdCommandHook*> HookClasses;
	static TArray<FCppCmdHook*> CppHooks;

	static UBOOL HookExec(const TCHAR* Cmd, FOutputDevice& Ar);
