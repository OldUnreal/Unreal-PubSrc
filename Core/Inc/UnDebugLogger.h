
#if DEBUG_STACK_ENABLED
#define DEBUGLOGF SLog.Logf
#define DEBUGLOGTICK SLog.Restart()

struct CORE_API FGameTrackerLog : public FOutputDevice
{
public:
	FArchive* FArch;
	INT Num,StackNum;

	FGameTrackerLog();
	~FGameTrackerLog();
	void Serialize( const TCHAR* V, EName Event );
	void Restart();
};
CORE_API extern FGameTrackerLog SLog;

struct FLogStacker
{
	FLogStacker()
	{
		++SLog.StackNum;
	}
	~FLogStacker()
	{
		--SLog.StackNum;
	}
};
#define DEBUGLOGSTACKF FLogStacker LOGStack; SLog.Logf

#else
#define DEBUGLOGF //
#define DEBUGLOGSTACKF //
#define DEBUGLOGTICK
#endif
