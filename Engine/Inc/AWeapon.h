/*=============================================================================
	AWeapon.h
=============================================================================*/

	AWeapon(){}
	UBOOL Tick( FLOAT DeltaTime, enum ELevelTick TickType );
	void PostScriptDestroyed();
	virtual INT* NativeReplicationRep(NETCOND_PARAMS);
	void OnChangedLevel(ALevelInfo* OldLevel);
	void UpdateAttachment(UBOOL bCloneSkin=TRUE);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
