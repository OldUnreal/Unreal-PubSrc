/*=============================================================================
	FFileManagerUnicode.h: Unicode conversion helpers for FFileManager classes
	Copyright 2019-2021 OldUnreal. All Rights Reserved.
	Revision history:
		* Created by Stijn Volckaert
=============================================================================*/

#pragma once

class FArchiveUnicodeWriterHelper
{
public:
	FArchiveUnicodeWriterHelper(const TCHAR* Filename, DWORD Flags, FOutputDevice* Error = GNull)
	{
		Ar = GFileManager->CreateFileWriter(Filename, Flags, Error);
	}

	~FArchiveUnicodeWriterHelper()
	{
		delete Ar;
	}

	UBOOL IsOpen()
	{
		return Ar != nullptr;
	}

	void SetEncoding(FileEncoding Enc, bool LogBOM = false)
	{
		this->Encoding = Enc;

		switch (Enc)
		{
		case ENCODING_ANSI:
			MaxSerializedCharLen = 1;
			FromTcharFunc = appToAnsiInPlace;
			break;

		case ENCODING_UTF16LE_BOM:
			BOM = UTF16LE_BOM;
		case ENCODING_UTF16LE:
			ToTcharFunc = appFromAnsiInPlace;
			MaxSerializedCharLen = 2;
			break;

		case ENCODING_UTF8_BOM:
			BOM = UTF8_BOM;
		case ENCODING_UTF8:
			FromTcharFunc = appToUtf8InPlace;
			MaxSerializedCharLen = 4;
			break;
		}

		if (BOM && LogBOM && Ar)
			Ar->Serialize(const_cast<char*>(BOM), (INT)strlen(BOM));
	}

	void Flush()
	{
		if (Ar)
			Ar->Flush();
	}

	void WriteString(const TCHAR* C)
	{
		if (!Ar)
			return;

		auto Len = appStrlen(C);

		if (!FromTcharFunc)
		{
			Ar->Serialize(const_cast<TCHAR*>(C), Len * sizeof(TCHAR));
			return;
		}

		// stijn: len+1 because the string conversion functions always null-terminate
		ANSICHAR* Tmp = ((Len + 1) * MaxSerializedCharLen < sizeof(LogBuf))
			? reinterpret_cast<ANSICHAR*>(LogBuf)
			: new ANSICHAR[(Len + 1) * MaxSerializedCharLen];

		FromTcharFunc(Tmp, C, (Len + 1) * MaxSerializedCharLen);
		Ar->Serialize(Tmp, (INT)strlen(Tmp));
		if (Tmp != reinterpret_cast<ANSICHAR*>(LogBuf))
			delete[] Tmp;
	}
	void WriteString(const ANSICHAR* C)
	{
		if (!Ar)
			return;

		auto Len = strlen(C);

		if (!ToTcharFunc)
		{
			Ar->Serialize(const_cast<ANSICHAR*>(C), (INT)Len);
			return;
		}

		TCHAR* Tmp = (Len < 1023) ? LogBuf : new TCHAR[Len + 1];
		ToTcharFunc(Tmp, C, Len + 1);
		Ar->Serialize(Tmp, (INT)Len);
		if (Tmp != LogBuf)
			delete[] Tmp;
	}

	FArchive*& GetAr()
	{
		return Ar;
	}

protected:
	FileEncoding Encoding = ENCODING_ANSI;
	size_t(*FromTcharFunc) (ANSICHAR*, const TCHAR*, size_t) = nullptr;
	size_t(*ToTcharFunc) (TCHAR*, const ANSICHAR*, size_t) = nullptr;
	size_t MaxSerializedCharLen = 1;
	const char* BOM = nullptr;
	FArchive* Ar = nullptr;
	TCHAR LogBuf[1024]{};
};
