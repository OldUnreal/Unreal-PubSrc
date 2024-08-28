/*=============================================================================
    UnTravelData.h
	Written by .:..:
=============================================================================*/

class APlayerPawn;

struct FTravelProperty
{
	FName Prop;
	FString Value;
	INT ElementIndex;

	FTravelProperty()
	{}
	FTravelProperty(const TCHAR* PropName, const TCHAR* PropValue, INT Index)
		: Prop(PropName), Value(PropValue),ElementIndex(Index)
	{}
	friend FArchive& operator<< (FArchive &Ar, FTravelProperty &T )
	{
		return Ar << T.Prop;
	}
};
struct FTravelItems
{
	UClass* Class;
	FString ClassStr;
	INT ItemIndex;
	TArray<FTravelProperty> Props;
	AActor* Temp;

	FTravelItems()
		: Class(NULL), ItemIndex(0), Temp(NULL)
	{}
	FTravelItems(const TCHAR* C, INT ix = 0)
		: Class(NULL), ClassStr(C), ItemIndex(ix), Temp(NULL)
	{}
	FTravelItems(UClass* C, INT ix, UBOOL bIsPlayer)
		: Class(C), ClassStr(bIsPlayer ? TEXT("Player") : C->GetPathName()), ItemIndex(ix), Temp(NULL)
	{}
	friend FArchive& operator<< (FArchive &Ar, FTravelItems &T )
	{
		return Ar << T.Class << T.Props;
	}
};
struct FTravelPlayer
{
	TArray<FTravelItems> Items;

	FTravelPlayer()
	{}
	friend FArchive& operator<< (FArchive &Ar, FTravelPlayer&T )
	{
		return Ar << T.Items;
	}
};
class ENGINE_API UTravelDataManager : public FObjectImportHandle
{
private:
	TMap<FString, FTravelPlayer*> Data;
	FString INIFile, OrgININame;
	UBOOL bIniSaveMode;
	TMap<INT, AActor*> SpawnedActors;
	TSingleMap<FString> NotFoundList;

	FTravelPlayer* LoadPlayerData(const TCHAR* UserID);

public:
	UTravelDataManager(const TCHAR* ININame, UBOOL bSaveToIni);
	~UTravelDataManager() noexcept(false);

	UBOOL SaveTravelActors(AActor* Player, const TCHAR* UserID);
	UBOOL LoadTravelActors(AActor* Player, const TCHAR* UserID);
	UBOOL DeleteTravelActors(const TCHAR* UserID, UBOOL bIsPawn = 1);

	void CleanupAllInventory();
	void SaveToINI();

	static void SPSaveTravelActors(AActor* Player, TMap<FString, FString>& Map, const TCHAR* UserID);
	static UBOOL SPLoadTravelActors(AActor* Player, TMap<FString, FString>& Map, const TCHAR* UserID);

	// FObjectImportHandle
	UObject* FindByIndex(INT InIndex);

	// Serializer.
	friend FArchive& operator<< (FArchive &Ar, UTravelDataManager &T )
	{
		for (TMap<FString, FTravelPlayer*>::TIterator It(T.Data); It; ++It)
			Ar << *It.Value();
		return Ar;
	}

	inline const TCHAR* GetINI() const
	{
		return *OrgININame;
	}
	inline UBOOL SaveToINI() const
	{
		return bIniSaveMode;
	}
};
