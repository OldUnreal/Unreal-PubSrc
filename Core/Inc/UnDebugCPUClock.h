
#if _WITH_DEBUG_CPU_COUNT_
struct CORE_API FCPUFunctionClock
{
public:
	const TCHAR* StatName;
	DWORD Clocks,NumCalls;
	static BYTE bWantsDump;
private:
	FCPUFunctionClock* Next;
	static FCPUFunctionClock* FirstCPUClock;

public:
	FCPUFunctionClock( const TCHAR* Name );
	~FCPUFunctionClock();

	static inline FCPUFunctionClock* GetFirst()
	{
		return FirstCPUClock;
	}
	inline FCPUFunctionClock* GetNext()
	{
		return Next;
	}
	static void PrintStats( FOutputDevice* Device );
	static void Reset();
};

struct FCPUClockCounter
{
	FCPUFunctionClock& Frame;
	DWORD NumCPU;

	FCPUClockCounter( FCPUFunctionClock& M )
		: Frame(M), NumCPU(appCycles())
	{
		++M.NumCalls;
	}
	~FCPUClockCounter()
	{
		Frame.Clocks += (appCycles() - NumCPU);
	}
};
#endif
