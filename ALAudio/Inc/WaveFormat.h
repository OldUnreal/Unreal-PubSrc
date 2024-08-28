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
	// Find looping points.
	FWaveFileHeader* FileHead = (FWaveFileHeader*)Data;
	if( appStrcmp(FileHead->GetIDStr(),TEXT("RIFF")) )
	{
		debugf(NAME_DevSound, TEXT("No RIFF in '%ls'"), FileHead->GetIDStr());
		return false; // Not a wave format.
	}
	DataEnd = Min(DataEnd,Data+FileHead->ChunkSize+sizeof(WaveChunkHeader));
	Data+=sizeof(FWaveFileHeader);

	WaveChunkHeader* FileFormatHead = (WaveChunkHeader*)Data;
	if( appStrcmp(FileFormatHead->GetIDStr(),TEXT("fmt ")) )
	{
		debugf(NAME_DevSound, TEXT("Audio invalid format '%ls'"), FileFormatHead->GetIDStr());
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
