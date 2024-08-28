//
// This class provides the interface with the external COM object. The COM
// object handles the marshalling of data between the Unreal Engine process
// and the Visual Studio process.
//
class FNameDebug
{
	// Loads COM and creates the object
	void Init(void);
	// Releases the object and frees COM
	void Release(void);

public:
	// Constructs the debugging hook
	FNameDebug(void)
	{
		Init();
	}

	// Releases all resources
	virtual ~FNameDebug(void)
	{
		Release();
	}

	// Inserts an FName into the COM map
	void AddFName(int nID,const wchar_t * szString);
};
