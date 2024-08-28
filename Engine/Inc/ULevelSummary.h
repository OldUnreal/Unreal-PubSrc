/*=============================================================================
	ULevelSummary.h: Level summary.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

	// Constructors.
	ULevelSummary()
	{}

	// UObject interface.
	void PostLoad()
	{
		guard(ULevelSummary::PostLoad);
		Super::PostLoad();
		FString Text=TEXT("");
		Text = Localize( TEXT("LevelInfo0"), TEXT("Title"), GetOuter()->GetName(), NULL, 1 );
		if( Text.Len() )
			Title = Text;
		Text = Localize( TEXT("LevelInfo0"), TEXT("IdealPlayerCount"), GetOuter()->GetName(), NULL, 1 );
		if( Text.Len() )
			IdealPlayerCount = Text;
		unguard;
	}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
