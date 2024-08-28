
// First bits is always this after compression.
#define COMPKeyValidation 172 // bits: 10101100
struct FZLibFileHeader
{
	INT OriginalSize,CompressedSize;
	BYTE CompKey;
	FString OriginalFileName;

	inline bool ValidateZLib( FArchive& Ar )
	{
		return (CompKey==COMPKeyValidation && CompressedSize==Ar.TotalSize());
	}
	friend FArchive& operator<<( FArchive& Ar, FZLibFileHeader& H )
	{
		H.CompKey = COMPKeyValidation;
		Ar<<H.OriginalSize<<H.CompressedSize<<H.CompKey;
		if( !Ar.IsLoading() || H.ValidateZLib(Ar) )
			Ar<<H.OriginalFileName;
		return Ar;
	}
};
enum EZCompressReturnType
{
	ZCOM_Success,
	ZCOM_Failure,
};

// Compress a set of data:
// CompLevel must be in a range of 1-9 (1 = best speed, 9 = best compression)
// RealFileName should be set to a name string to original filename before compression.
CORE_API EZCompressReturnType ZLibCompressData( FArchive& Input, FArchive& Output, BYTE CompLevel, const TCHAR* RealFileName );
CORE_API EZCompressReturnType ZLibDeCompressData( FArchive& Input, FArchive& Output );
