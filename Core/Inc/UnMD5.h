/*=============================================================================
	UnMD5: Unreal MD5 lookup table

Revision history:
	* Created by Michiel Hendriks
=============================================================================*/

#ifdef UTPG_MD5
#ifndef UTPG_MD5_H
#define UTPG_MD5_H

// Name translation table:
//
// FMD5Record		-> FZQ5Erpbeq
// MD5Checksum	-> ZQ5Purpxfhz
// UMD5Table		-> UZQ5Gnoyr


// rot13(FMD5Record) = ZQ5Erpbeq
class CORE_API FZQ5Erpbeq
{
public:
  FString filename;
  FGuid   guid;
  INT     generation;
  FString ZQ5Purpxfhz; // = rot13(MD5Checksum)
  DWORD   filetime;
	FString	language;
	FString PartialHash; // don't save to file

  FZQ5Erpbeq() 
  {}
  FZQ5Erpbeq( FString InFilename, FGuid InGuid, INT InGeneration, FString InZQ5Purpxfhz, DWORD InFiletime, FString InLanguage) 
  : filename(InFilename)
  , guid(InGuid)
  , generation(InGeneration)
  , ZQ5Purpxfhz(InZQ5Purpxfhz)
  , filetime(InFiletime)
	, language(InLanguage)
  {}
  CORE_API friend FArchive& operator<<( FArchive& Ar, FZQ5Erpbeq& record );
};

// rot13(UMD5Table) = ZQ5Gnoyr
class CORE_API UZQ5Gnoyr
{
  TArray<FZQ5Erpbeq*>  List;
public:
  CORE_API friend FArchive& operator<<( FArchive& Ar, UZQ5Gnoyr& table );
  void addRecord(FZQ5Erpbeq* record);
  void removeRecord(INT i);
  FZQ5Erpbeq* UZQ5Gnoyr::getRecord(INT i);
  INT count() { return List.Num(); }
  FString lookupZQ5(FString inFilename, FGuid InGuid, INT InGeneration, FString InLanguage);
  FZQ5Erpbeq* lookupZQ5Erpbeq(FString inFilename, FGuid InGuid, INT InGeneration, FString InLanguage);
	FString lookupZQ5filename(FString inFilename, INT InGeneration, FString InLanguage);
};

#endif
#endif

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
