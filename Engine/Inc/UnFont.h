#pragma once
//
// A font object, containing information about a set of glyphs.
// The glyph bitmaps are stored in the contained textures, while
// the font database only contains the coordinates of the individual
// glyph.
//
class ENGINE_API UFont : public UObject
{
	DECLARE_CLASS(UFont, UObject, (CLASS_RuntimeStatic | CLASS_SafeReplace | CLASS_NoUserCreate), Engine)

	// Variables.
	INT CharactersPerPage;
	TArray<FFontPage> Pages;
	TMap<TCHAR,TCHAR> CharRemap;
	UBOOL IsRemapped;
	DWORD PolyFlags;
	FString Preview;

	// Constructors.
	UFont();

	// UObject interface.
	void Serialize( FArchive& Ar );
	void StaticConstructor();

	// UFont interface
	TCHAR RemapChar(TCHAR ch)
	{
		TCHAR *p;
		if( !IsRemapped )
			return ch;
		p = CharRemap.Find(ch);
		return p ? *p : 32; // return space if not found.
	}
	virtual UFont* GetRenderFont(FPlane* DrawColor)
	{
		return this;
	}

	// Get Font replacement for better multilanguage support.
	static UFont* GetDrawFont(UFont* F);
};

class ENGINE_API UFontColored : public UFont
{
	DECLARE_CLASS(UFontColored, UFont, (CLASS_RuntimeStatic | CLASS_SafeReplace), Engine)

	// Variables.
	UFont* SourceFont;
	FColor Color;

	// Constructors.
	UFontColored();

	// UObject interface.
	void Serialize(FArchive& Ar);
	void StaticConstructor();

	UFont* GetRenderFont(FPlane* DrawColor)
	{
		if (DrawColor)
			(*DrawColor) *= Color.Vector();
		return SourceFont;
	}
};
