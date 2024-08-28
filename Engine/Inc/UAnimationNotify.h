/*=============================================================================
	UAnimationNotify.h: Just a header :)
	Copyright 2008-2010 Red Nemesis, All Rights Reserved.

	Revision history:
		* Created by Grzegorz 'Raven' Zawadzki
	Implementation for Unreal 227 by Smirftsch
=============================================================================*/
	UAnimationNotify() {}
	/**
	 * Initialize all variables and do basic checks.
	 * We don't want to calculate everything all the time :)
	 */
	void NotifyInit();
	/**
	 * Called when correct animation is played
	 */
	virtual void OnNotify(float DeltaTime, int Num);
	/**
	 * Main notify code.
	 */
	UBOOL UpdateMeshNotify(float DeltaTime, int num);
	/**
	 * Updates everything. can be called from outside this class.
	 */
	void NativeAnimUpdate(float DeltaTime);

	void StartLipSyncTrack(USound* Sound, FLOAT Pitch = 1.f);