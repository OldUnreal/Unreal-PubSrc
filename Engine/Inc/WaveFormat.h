// Common wave file format.

struct WaveChunkHeader
{
	BYTE ChunkID[4];
	DWORD ChunkSize;

	inline TCHAR* GetIDStr()
	{
		static TCHAR Out[5]={0,0,0,0,0};
		for( BYTE i=0; i<4; ++i )
			Out[i] = (ChunkID[i] ? ChunkID[i] : ' ');
		return Out;
	}
};
struct FWaveFileHeader : public WaveChunkHeader
{
	char RiffFormat[4];
};
struct FFormatChunkData
{
	WORD Compressioncode;
	WORD Numberofchannels;
	INT SampleRate;
	INT Averagebytespersecond;
	WORD Blockalign;
	WORD Significantbitspersample;
	WORD Extraformatbytes;
};
struct SamplerChunkData
{
	INT Manufacturer;
	INT Product;
	INT SamplePeriod;
	INT MIDIUnityNote;
	INT MIDIPitchFraction;
	INT SMPTEFormat;
	INT SMPTEOffset;
	INT NumSampleLoops;
	INT SamplerData;
};
struct FSamplerLoop
{
	INT CuePointID;
	INT Type;
	INT Start;
	INT End;
	INT Fraction;
	INT PlayCount;
};

bool GetSampleLoop( const BYTE* Data, const BYTE* DataEnd, INT* LoopStart, INT* LoopEnd )
{
	// Han: Check if we can read enough bytes!
	if ((Data + sizeof(FWaveFileHeader) >= DataEnd))
	{
		debugf(TEXT("GetSampleLoop: Not enough Data (0)."));
		return false;
	}
	// Find looping points.
	FWaveFileHeader* FileHead = (FWaveFileHeader*)Data;
	if( appStrcmp(FileHead->GetIDStr(),TEXT("RIFF")) )
	{
		//debugf(TEXT("Audio invalid format '%ls'"),FileHead->GetIDStr());
		return false; // Not a wave format.
	}
	DataEnd = Min(DataEnd,Data+FileHead->ChunkSize+sizeof(WaveChunkHeader));
	Data+=sizeof(FWaveFileHeader);

	WaveChunkHeader* FileFormatHead = (WaveChunkHeader*)Data;
	if( appStrcmp(FileFormatHead->GetIDStr(),TEXT("fmt ")) )
	{
		//debugf(TEXT("Audio invalid format '%ls'"),FileFormatHead->GetIDStr());
		return false; // Not a wave format.
	}
	Data+=sizeof(WaveChunkHeader);
	//FFormatChunkData* FormatData = (FFormatChunkData*)Data;
	Data+=FileFormatHead->ChunkSize;

	DataEnd-=(sizeof(WaveChunkHeader)-1);

	// Parse until end of file or until we find sampler chunk.
	while( Data<DataEnd )
	{
		WaveChunkHeader* DataHead = (WaveChunkHeader*)Data;

		if( !DataHead->ChunkID[0] || DataHead->ChunkID[0]>127 ) // Faulty format?
		{
			++Data;
			DataHead = (WaveChunkHeader*)Data;
			if( !DataHead->ChunkID[0] || DataHead->ChunkID[0]>127 )
				break;
		}
		if( !appStrcmp(DataHead->GetIDStr(),TEXT("smpl")) )
		{
			Data+=sizeof(WaveChunkHeader);
			SamplerChunkData* Smpl = (SamplerChunkData*)Data;
			if( Smpl->NumSampleLoops )
			{
				Data+=sizeof(SamplerChunkData);
				FSamplerLoop* Loop = (FSamplerLoop*)Data;
				if( LoopStart )
					*LoopStart = Loop->Start;
				if( LoopEnd )
					*LoopEnd = Loop->End;
				return true;
			}
			break;
		}
		else Data+=sizeof(WaveChunkHeader)+DataHead->ChunkSize;
	}
	return false;
}

bool FindLoopPoint( const BYTE* DataStr, FLOAT& LoopStrt, FLOAT& LoopEnd )
{
	guard(FindLoopPoint);
	int j,i;
	FString Txt(TEXT(""));
	INT StrData;
	for( i=50; i<350; i++ )
	{
		StrData = DataStr[i];
		Txt+=((TCHAR*) &StrData);
	}
	j = Txt.InStr(TEXT("COMMENT=|LOOP:"));
	if( j==-1 )
		return false;
	Txt = Txt.Mid(j+14);
	j = Txt.InStr(TEXT("-"));
	if( j==-1 )
		return false;
	FString SLoop = Txt.Left(j);
	Txt = Txt.Mid(j+1);
	j = Txt.InStr(TEXT("|"));
	if( j==-1 )
		return false;
	Txt = Txt.Left(j);
	LoopStrt = appAtof( *SLoop )/100.f;
	LoopEnd = appAtof( *Txt )/100.f;
	return true;
	unguard;
}
