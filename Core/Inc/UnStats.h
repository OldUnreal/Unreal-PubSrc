/*=============================================================================
	UnStats.h: Unreal benchmark stat tracker.
	Written by Marco
=============================================================================*/

#if STATS
struct FStatGroup;

struct FStatValueBase
{
	FStatValueBase* Next;
	const TCHAR* Name;

	inline FStatValueBase(FStatGroup* Parent, const TCHAR* UI);
	virtual FString GetValue() { return TEXT(""); }
	virtual void Tick( BYTE bUpdate ){}
	virtual void Reset() {}

private:
	FStatValueBase(){}
};
struct CORE_API FStatGroup
{
	static FStatGroup* GStats;
	static BYTE bShowStats;

	FStatValueBase* Values;
	FStatGroup* Next;
	const TCHAR* GroupName;
	BYTE bVisible;

	inline void AddStatValue(FStatValueBase* Value)
	{
		if (!Values)
			Values = Value;
		else
		{
			for (FStatValueBase* V = Values; V; V = V->Next)
				if (!V->Next)
				{
					V->Next = Value;
					break;
				}
		}
	}
	inline void RemoveStatValue(FStatValueBase* Value)
	{
		if (Values==Value)
			Values = Value->Next;
		else
		{
			for (FStatValueBase* V = Values; V; V = V->Next)
				if (V->Next==Value)
				{
					V->Next = Value->Next;
					break;
				}
		}
		Value->Next = NULL;
	}
	inline void Reset()
	{
		for (FStatValueBase* V = Values; V; V = V->Next)
			V->Reset();
	}
	inline void TickValues(BYTE bUpdate)
	{
		for (FStatValueBase* V = Values; V; V = V->Next)
			V->Tick(bUpdate);
	}

	FStatGroup(const TCHAR* N)
		: Values(NULL), GroupName(N), bVisible(0)
	{
		Next = GStats;
		GStats = this;
	}
	~FStatGroup()
	{
		// Unlink!
		if (GStats == this)
			GStats = Next;
		else
		{
			for (FStatGroup* G = GStats; G; G = G->Next)
			{
				if (G->Next == this)
				{
					G->Next = Next;
					break;
				}
			}
		}
	}

	static void Tick();
	static UBOOL Exec(const TCHAR* Cmd, FOutputDevice& Ar);
private:
	FStatGroup(){}
};

FStatValueBase::FStatValueBase(FStatGroup* Parent, const TCHAR* UI)
	: Name(UI)
{
	Parent->AddStatValue(this);
}

#define HISTORY_SIZE 32
#define HISTORY_MASK 31

struct FStatStaticCount : public FStatValueBase
{
	QWORD Count = 0, LastFrame = 0;

	FStatStaticCount(FStatGroup* Parent, const TCHAR* UI)
		: FStatValueBase(Parent, UI), Count(0), LastFrame(0)
	{
	}
	void Tick(BYTE bUpdate)
	{
		if (bUpdate)
        {
			LastFrame = Count;
        }
	}
	FString GetValue()
	{
		return FString::Printf(TEXT("%lld"), LastFrame);
	}
};
struct FStatValueCounter : public FStatValueBase
{
	QWORD Calls,LastFrame,History[HISTORY_SIZE];
	INT HistoryCounter;

	FStatValueCounter(FStatGroup* Parent, const TCHAR* UI)
		: FStatValueBase(Parent, UI), Calls(0), LastFrame(0), HistoryCounter(0)
	{
		appMemzero(History, sizeof(History));
	}
	void Tick(BYTE bUpdate)
	{
		if (bUpdate)
		{
			LastFrame = Calls;
			History[HistoryCounter++ & HISTORY_MASK] = Calls;
		}
		Calls = 0;
	}
	void Reset()
	{
		Calls = 0;
	}
	FString GetValue()
	{
		QWORD Avg = 0;
		for (INT i = 0; i < HISTORY_SIZE; ++i)
			Avg += History[i];
		return FString::Printf(TEXT("%llu (%llu avg)"), LastFrame, (Avg / HISTORY_SIZE));
	}
};
struct FStatValueTimer : public FStatValueBase
{
	QWORD Time, LastFrame, History[HISTORY_SIZE];
	INT HistoryCounter;

	FStatValueTimer(FStatGroup* Parent, const TCHAR* UI)
		: FStatValueBase(Parent,UI), Time(0), LastFrame(0), HistoryCounter(0)
	{
		appMemzero(History, sizeof(History));
	}
	void Tick(BYTE bUpdate)
	{
		if (bUpdate)
		{
			LastFrame = Time;
			History[HistoryCounter++ & HISTORY_MASK] = Time;
		}
		Time = 0;
	}
	void Reset()
	{
		Time = 0;
	}
	FString GetValue()
	{
		QWORD Avg = 0;
		for (BYTE i = 0; i < HISTORY_SIZE; ++i)
			Avg += History[i];
		return FString::Printf(TEXT("%.2f MS (%.2f MS avg)"),((DOUBLE)LastFrame * GSecondsPerCycle * 1000.0), ((DOUBLE)Avg / HISTORY_SIZE * GSecondsPerCycle * 1000.0));
	}
};

#define clockTimer(timer) timer-=appCycles()
#define unclockTimer(timer) timer+=appCycles()

struct FStatTimerScope
{
	FStatValueTimer& Timer;
	QWORD C;

	FStatTimerScope(FStatValueTimer& T)
		: Timer(T), C(appCycles())
	{}
	~FStatTimerScope()
	{
		Timer.Time += (appCycles() - C);
	}
};

struct FMemStats : public FStatGroup
{
	FStatStaticCount AllocCount;
	FStatValueCounter AllocCounter;
	FStatValueTimer AllocTime;

	FMemStats();
};
struct FUScriptStats : public FStatGroup
{
	FStatValueCounter CallCount,MaxRecursion;
	FStatValueTimer ExecTime;

	FUScriptStats();
};
struct FThreadStats : public FStatGroup
{
	struct FRunningThreads : public FStatValueBase
	{
		DWORD ThreadCount;

		FString GetValue();
		FRunningThreads(FStatGroup* Parent, const TCHAR* UI)
			: FStatValueBase(Parent, UI), ThreadCount(0)
		{}
	};
	FRunningThreads NumThreads;
	FStatValueCounter NewThreads,ExitThreads;

	FThreadStats();
};
CORE_API extern FMemStats GMemStats;
CORE_API extern FUScriptStats GUScriptStats;
CORE_API extern FThreadStats GThreadStats;

#endif
