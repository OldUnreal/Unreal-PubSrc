/*=============================================================================
	UnThread.cpp: Unreal Mulithreading implementation.
=============================================================================*/

/*----------------------------------------------------------------------------
	Multi-Threading base.
----------------------------------------------------------------------------*/

#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include <atomic>         // std::atomic

#ifndef _WINDOWS
#include <pthread.h>
#endif

// Forward declarations
struct FWorkQuery;

typedef std::mutex FThreadLock;
typedef std::recursive_mutex FThreadLockRecursive;

// Simple threading base.
class CORE_API FThreadingBase
{
	friend struct FThreadScopeHelper;
private:
	static std::thread::id main_thread_id;
	std::thread*	Thread;

	std::atomic<UBOOL> bIsRunning;
	const TCHAR* ThreadName;

	static FThreadLock ThreadMutex;
	static FThreadingBase* ThreadList;
	FThreadingBase* NextThread;

	FThreadingBase() {}

protected:
	std::atomic<UBOOL> bRequestExit; // Requesting thread to exit.
	std::atomic<UBOOL> bIsPaused;

public:
	FThreadingBase(const TCHAR* Name);
	virtual ~FThreadingBase() noexcept(false);
	void RunThread(); // Start the new thread.
	void ExitThread(); // Initiate thread exit.
	void WaitForThread(); // Wait for this thread to finish from main thread.

	virtual void ShutdownThread(); // Called when about to shutdown Unreal.

	inline UBOOL IsRunning() const
	{
		if (!Thread)
			return FALSE;
		return bIsRunning;
	}
	inline UBOOL IsPaused() const
	{
		return bIsPaused;
	}
	inline UBOOL RequestingExit() const
	{
		return bRequestExit;
	}

	virtual void Main() // To be called from RunThread()
	{
		YieldThread();
	}

	static void YieldThread() // Yield this thread.
	{
		std::this_thread::yield();
	}

	inline const TCHAR* GetName() const
	{
		return ThreadName;
	}

	static void ShutdownThreads();

	static UBOOL IsMainThread()
	{
		return (main_thread_id == std::this_thread::get_id());
	}
	static void SetMainThread()
	{
		main_thread_id = std::this_thread::get_id();
	}

	static INT GetNumCores() // Get estimated number of CPU cores.
	{
		return Max<INT>(std::thread::hardware_concurrency(), 1);
	}

	// Threading iterator: WARNING! Can't create/delete any threads during this iterator!
	class ThreadIterator
	{
		FThreadingBase* Cur;
	public:
		ThreadIterator()
		{
			ThreadMutex.lock();
			Cur = ThreadList;
		}
		~ThreadIterator()
		{
			ThreadMutex.unlock();
		}
		void operator++()
		{
			Cur = Cur->NextThread;
		}
		FThreadingBase* operator*()
		{
			return Cur;
		}
		FThreadingBase* operator->()
		{
			return Cur;
		}
		operator UBOOL()
		{
			return Cur != NULL;
		}
	};
};

class FEventSignal
{
#if _WINDOWS
	HANDLE Event;
#else
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	UBOOL bSignaled;
#endif

	FEventSignal() {}

public:
	FEventSignal(UBOOL bStartSignaled, const TCHAR* EventName)
	{
#if _WINDOWS
		Event = CreateEvent(NULL, TRUE, bStartSignaled, EventName);
		check(Event != NULL);
#else
		pthread_mutex_lock(&lock);
		bSignaled = bStartSignaled;	
		pthread_mutex_unlock(&lock);
#endif
	}
	~FEventSignal()
	{
#if _WINDOWS
		CloseHandle(Event);
#else
		pthread_mutex_lock(&lock);
		bSignaled = TRUE;
		pthread_mutex_unlock(&lock);
		pthread_cond_signal(&cond);
#endif
	}

	// Fire the signal to unlock threads.
	void Trigger()
	{
#if _WINDOWS
		SetEvent(Event);
#else
		pthread_mutex_lock(&lock);
		bSignaled = TRUE;
		pthread_mutex_unlock(&lock);
		pthread_cond_signal(&cond);
#endif
	}

	// Reset signal to pause threads.
	void Reset()
	{
#if _WINDOWS
		ResetEvent(Event);
#else
		pthread_mutex_lock(&lock);
		bSignaled = FALSE;
		pthread_mutex_unlock(&lock);
#endif
	}

	// Wait for this thread to finish the task.
	// @MaxWaitTime max waiting time in MS before aborting (returns FALSE if it aborted), -1 = infinite
	UBOOL WaitForTask(DWORD MaxWaitTime = (DWORD)(-1))
	{
#if _WINDOWS
		return WaitForSingleObject(Event, MaxWaitTime) == WAIT_OBJECT_0;
#else
		if (MaxWaitTime == ((DWORD)(-1)))
		{
			pthread_mutex_lock(&lock);
			while (!bSignaled)
				pthread_cond_wait(&cond, &lock);
			pthread_mutex_unlock(&lock);
			return TRUE;
		}
		else
		{
			UBOOL bResult = TRUE;
			pthread_mutex_lock(&lock);
			if (!bSignaled)
			{
				timespec ts;
				clock_gettime(CLOCK_REALTIME, &ts);
				ts.tv_nsec += ((long)MaxWaitTime * 1000000);
				bResult = (pthread_cond_timedwait(&cond, &lock, &ts) == 0);
			}
			pthread_mutex_unlock(&lock);
			return bResult;
		}
#endif
	}
};

// A thread that performs a constant task the entire game.
class CORE_API FWorkingThread : private FThreadingBase
{
	friend struct FWorkQuery;
private:
	FWorkQuery* WorkList;
	FThreadLock AccessMutex,ExecutionMutex;
	FEventSignal Event;

	FWorkingThread()
		: FThreadingBase(NULL), Event(0, NULL)
	{}
	void Main();
	void ShutdownThread();

public:
	FWorkingThread(const TCHAR* ID);
	~FWorkingThread();

	inline UBOOL IsPaused() const
	{
		return bIsPaused;
	}
	inline void WaitForTask()
	{
		while (!bIsPaused && !GIsRequestingExit)
		{
			ExecutionMutex.lock();
			ExecutionMutex.unlock();
		}
	}
	inline void StartThread()
	{
		if (!IsRunning())
			RunThread();
	}
};

// Workload for constant thread.
/* Works as follow:
FWorkingThread MainThread(TEXT("Hello"));

void ATask()
{
	FWorkQuery* ThisTask = new FWorkQuery(TEXT("World"));
	ThisTask->StartWorking(MainThread);
}
*/
struct CORE_API FWorkQuery
{
	friend class FWorkingThread;
private:
	FWorkQuery* Next;
	FWorkingThread* Thread;
	std::atomic<BYTE> Executing;
	const TCHAR* QueryName;

	FWorkQuery()
	{}

public:
	FWorkQuery(const TCHAR* Name);
	virtual ~FWorkQuery();

	void StartWorking(FWorkingThread* InThread);

	virtual void Main() {}

	inline UBOOL RequestingExit() const
	{
		return (Thread ? Thread->RequestingExit() : TRUE);
	}

	// Is waiting to begin.
	inline UBOOL IsDormant() const
	{
		return Executing == 0;
	}

	// If true, currently executing this task.
	inline UBOOL IsExecuting() const
	{
		return Executing == 1;
	}

	// If true, we've finished this task.
	inline UBOOL IsFinished() const
	{
		return Executing == 2;
	}

	inline const TCHAR* GetName() const
	{
		return QueryName;
	}

	void AbortQuery(); // Abort if still in query or wait til task has finished.
	void Destroy(); // Destroy this query (wait for task to finish if currently working)
};

template<typename C> class FScopeThread
{
private:
	C& Lock;
public:
	FScopeThread(C& L)
		: Lock(L)
	{
		L.lock();
	}
	~FScopeThread()
	{
		Lock.unlock();
	}
};
