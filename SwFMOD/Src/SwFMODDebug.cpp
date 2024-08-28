/* ============================================================================
	SwFMODDebug.cpp:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */


// Includes.
#include "SwFMOD.h"


/* ----------------------------------------------------------------------------
	USwFMOD
---------------------------------------------------------------------------- */
void USwFMOD::DumpSystem()
{
	guard(USwFMOD::DumpSystem);
	FMOD_RESULT result;
	FString text;

	if( !System )
		return;

	//
	// pre-init
	//

#pragma pack(push)
#pragma pack(8)
	FMOD_ADVANCEDSETTINGS adv;
	appMemset(&adv, 0, sizeof(FMOD_ADVANCEDSETTINGS));
	adv.cbsize = sizeof(FMOD_ADVANCEDSETTINGS);
#pragma pack(pop)

	FMOD_OUTPUTTYPE output;
	int driveridx;
	int softchan;
	int samplerate;
	FMOD_SOUND_FORMAT format;
	int numoutputchannels;
	int maxinputchannels;
	FMOD_DSP_RESAMPLER resamplemethod;
	int bits;
	unsigned int bufferlength;
	int numbuffers;
	FMOD_SPEAKERMODE speakermode;

/*	char drivername[1024];
	FMOD_CAPS caps;
	INT minfreq, maxfreq;
	FMOD_SPEAKERMODE smode;*/

	SWF_FMOD_CALL( System->getOutput(&output) );
//	SWF_FMOD_CALL( System->getNumDrivers() );
//	SWF_FMOD_CALL( System->getDriverName() );
//	SWF_FMOD_CALL( System->getDriverCaps() );
	SWF_FMOD_CALL( System->getDriver(&driveridx) );
	SWF_FMOD_CALL( System->getSoftwareChannels(&softchan) );
	SWF_FMOD_CALL( System->getSoftwareFormat(&samplerate,&format,&numoutputchannels,&maxinputchannels,&resamplemethod,&bits) );
	SWF_FMOD_CALL( System->getDSPBufferSize(&bufferlength,&numbuffers) );
	SWF_FMOD_CALL( System->getAdvancedSettings(&adv) );
	SWF_FMOD_CALL( System->getSpeakerMode(&speakermode) );

	SWF_LOG_DEV_STATIC( System->getOutput, SWF_STSTR(output) );
	SWF_LOG_DEV_STATIC( System->getDriver, SWF_STSTR(driveridx) );
	SWF_LOG_DEV_STATIC( System->getSoftwareChannels, SWF_STSTR(softchan) );
	SWF_LOG_DEV_STATIC( System->getSoftwareFormat, SWF_STSTR(samplerate) + SWF_STSTR(format) + SWF_STSTR(numoutputchannels) + SWF_STSTR(maxinputchannels) + SWF_STSTR(resamplemethod) + SWF_STSTR(bits) );
	SWF_LOG_DEV_STATIC( System->getDSPBufferSize, SWF_STSTR(bufferlength) + SWF_STSTR(numbuffers) );
	SWF_LOG_DEV_STATIC( System->getAdvancedSettings, SWF_STSTR(adv) );
	SWF_LOG_DEV_STATIC( System->getSpeakerMode, SWF_STSTR(speakermode) );




/*	// Driver
	int idx = 0;
	SWF_FMOD_CALL( System->getDriverName( idx, drivername, 1024) );
	SWF_FMOD_CALL( System->getDriverCaps( idx, &caps, &minfreq, &maxfreq, &smode ) );
	SWF_LOG( SWF_NAME, TEXT("Driver: %ls"), *ToStr(appFromAnsi(drivername)) );
	SWF_LOG( SWF_NAME, TEXT("Driver Frequency: %d - %d"), minfreq, maxfreq );
	SWF_LOG( SWF_NAME, TEXT("Driver Caps: %ls"), *FMOD_CAPSToStr(caps) );
	SWF_LOG( SWF_NAME, TEXT("Driver Speaker Mode: %ls"), *FMOD_SPEAKERMODEToStr(smode) );*/



	//
	// System information functions.
	//

	FSwVersion version;
	int hctotal;
	int currentalloced;
	int maxalloced;
	int memtotal;


	SWF_FMOD_CALL( System->getVersion(version.GetRaw()) );
//	SWF_FMOD_CALL( System->getOutputHandle(&version) );
//	SWF_FMOD_CALL( System->getChannelsPlaying(&version) );
	SWF_FMOD_CALL( System->getHardwareChannels(&hctotal) );
//	SWF_FMOD_CALL( System->getCPUUsage(&version) );
	SWF_FMOD_CALL( System->getSoundRAM(&currentalloced,&maxalloced,&memtotal) );
//	SWF_FMOD_CALL( System->getNumCDROMDrives(&version) );
//	SWF_FMOD_CALL( System->getCDROMDriveName(&version) );
//	SWF_FMOD_CALL( System->getSpectrum(&version) );
//	SWF_FMOD_CALL( System->getWaveData(&version) );

	SWF_LOG_DEV_STATIC( System->getVersion, SWF_STSTR(version) );
	SWF_LOG_DEV_STATIC( System->getHardwareChannels, SWF_STSTR(hctotal) );
	SWF_LOG_DEV_STATIC( System->getSoundRAM, SWF_STSTR(currentalloced) + SWF_STSTR(maxalloced) + SWF_STSTR(memtotal));


	//
	// post-init
	//

	float dopplerscale;
	float distancefactor;
	float rolloffscale;

	SWF_FMOD_CALL( System->get3DSettings(&dopplerscale,&distancefactor,&rolloffscale) );

	SWF_LOG_DEV_STATIC( System->get3DSettings, SWF_STSTR(dopplerscale) + SWF_STSTR(distancefactor) + SWF_STSTR(rolloffscale) );

	unguard;
}


void USwFMOD::DumpDrivers()
{
	guard(USwFMOD::DumpDrivers);
	FMOD_RESULT result;

	if( !System )
		return;

	// Enumerate drivers
	INT numdrivers;
	SWF_FMOD_CALL( System->getNumDrivers(&numdrivers) );

	// Print drivers
	for( INT i=0; i<numdrivers; ++i )
	{
		// Name
		char drivername[1024];
		SWF_FMOD_CALL( System->getDriverInfo(i, drivername, 1024,0) );
		SWF_LOG( SWF_NAME, TEXT("Driver[%d] %ls"), i, *ToStr(appFromAnsi(drivername)) );

		// Caps
		FMOD_CAPS caps;
		INT minfreq;
		FMOD_SPEAKERMODE smode;
		SWF_FMOD_CALL( System->getDriverCaps( i, &caps, &minfreq, &smode ) );
		SWF_LOG( SWF_NAME, TEXT("Driver[%d] ControlPanelOutputRate: %d - %d"), i, minfreq );
		SWF_LOG( SWF_NAME, TEXT("Driver[%d] Caps: %ls"), i, *FMOD_CAPSToStr(caps) );
		SWF_LOG( SWF_NAME, TEXT("Driver[%d] Speaker Mode: %ls"), i, *FMOD_SPEAKERMODEToStr(smode) );
	}
	unguard;
}


void USwFMOD::DumpPlugins()
{
	guard(USwFMOD::DumpPlugins);
	FMOD_RESULT result;
	FMOD_PLUGINTYPE plugintype;

	int i;

	// Enumerate plugins
	int numplugins;
	char pname[1024];
	FSwVersion pver;

	// Output plugins
	SWF_FMOD_CALL( System->getNumPlugins(FMOD_PLUGINTYPE_OUTPUT,&numplugins) );
	plugintype = FMOD_PLUGINTYPE_OUTPUT;
	for( i=0; i!=numplugins; ++i )
	{
		SWF_FMOD_CALL( System->getPluginInfo(i, &plugintype, pname, 1024, pver.GetRaw() ) );
		SWF_LOG( SWF_NAME, TEXT("Output Plugin[%d]: %ls v%ls"), i, *ToStr(appFromAnsi(pname)), *ToStr(pver) );
	}

	// Codec plugins
	SWF_FMOD_CALL( System->getNumPlugins(FMOD_PLUGINTYPE_CODEC,&numplugins) );
	plugintype = FMOD_PLUGINTYPE_CODEC;
	for( i=0; i!=numplugins; ++i )
	{
		SWF_FMOD_CALL( System->getPluginInfo(i, &plugintype, pname, 1024, pver.GetRaw() ) );
		SWF_LOG( SWF_NAME, TEXT("Codec Plugin[%d]: %ls v%ls"), i, *ToStr(appFromAnsi(pname)), *ToStr(pver) );
	}

	// DSP plugins
	SWF_FMOD_CALL( System->getNumPlugins(FMOD_PLUGINTYPE_DSP,&numplugins) );
	plugintype  = FMOD_PLUGINTYPE_DSP;
	for( i=0; i!=numplugins; ++i )
	{
		SWF_FMOD_CALL( System->getPluginInfo(i, &plugintype, pname, 1024, pver.GetRaw() ) );
		SWF_LOG( SWF_NAME, TEXT("DSP Plugin[%d]: %ls v%ls"), i, *ToStr(appFromAnsi(pname)), *ToStr(pver) );
	}

	unguard;
}


/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
