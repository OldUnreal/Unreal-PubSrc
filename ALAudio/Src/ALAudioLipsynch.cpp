
/*=============================================================================
ALAudioLipsynch.cpp: ALAudio lipsynch support.

Reimplementation of poor mans lipsynch for Deus Ex.

Revision history:
* Created by Sebastian Kaufel

Ideas:
* FFT transform the full sample to calculate some bias,
to better match female/child voices. Default bias should be
about JC/Paul, as they look best.
=============================================================================*/

/*-----------------------------------------------------------------------------
Dependencies.
-----------------------------------------------------------------------------*/



#ifdef _WIN32
#include <comutil.h>
#endif
#include "ALAudio.h"
#ifdef USE_LIPSYNCH
#include "KissFFT/kiss_fft.h"

/*-----------------------------------------------------------------------------
Lipsynch.
-----------------------------------------------------------------------------*/
#ifndef warnf
#define warnf GWarn->Logf
#endif

// Start with same low value as ION did for low end lipsynch.
//#define NUM_LIPSYNCH_SAMPLES 1024
#define NUM_LIPSYNCH_SAMPLES 2048

void UALAudioSubsystem::Lipsynch(ALAudioSoundInstance& Playing)
{
	guard(UALAudioSubsystem::Lipsynch);
	ALint Error;

	// Get pawn.
	APawn* Pawn = (APawn*)Playing.Actor;
	check(Pawn);
	check(Pawn->bIsPawn);
	check(bOpenALSOFT);

	ALfloat* SampleData = NULL;
	ALint NumChannels = 0;
	ALint NumSamples = 0;
	ALint SampleRate = 0;
	ALint SampleIndex = 0;
    ALfloat AvgVolume = 0.0f, MinVolume = 1000.0f, MaxVolume = -1000.f;

	guard(QueryInfo);
	alGetError();
	alGetBufferi(Playing.Sound->ID, AL_CHANNELS, &NumChannels); if ((Error = alGetError()) != AL_NO_ERROR)
	{
	    warnf(TEXT("Failed to query number of channels (%ls)."), appFromAnsi(alGetString(Error)));
	    goto Cleanup;
    }
	alGetBufferi(Playing.Sound->ID, AL_SAMPLE_LENGTH_SOFT, &NumSamples);
	if ((Error = alGetError()) != AL_NO_ERROR)
    {
        warnf(TEXT("Failed to query number of samples (%ls)."), appFromAnsi(alGetString(Error)));
        goto Cleanup;
    }
	alGetBufferi(Playing.Sound->ID, AL_FREQUENCY, &SampleRate);
	if ((Error = alGetError()) != AL_NO_ERROR)
    {
        warnf(TEXT("Failed to query sample rate (%ls)."), appFromAnsi(alGetString(Error)));
        goto Cleanup;
    }
	alGetSourcei(Playing.SourceID, AL_SAMPLE_OFFSET, &SampleIndex);
	if ((Error = alGetError()) != AL_NO_ERROR)
    {
        warnf(TEXT("Failed to query source offset (%ls)."), appFromAnsi(alGetString(Error)));
        goto Cleanup;
    }
	unguard;

	// Just do mono lipsynch and if we have enough samples (at least for now).
	if (NumChannels != 1)
    {
        warnf(TEXT("Skipping lipsynch for non mono audio (%i channels)."), NumChannels);
        goto Cleanup;
    }
	if (NumSamples<NUM_LIPSYNCH_SAMPLES)
	{
	    warnf(TEXT("Skipping lipsynch for to small sample (%i samples)."), NumSamples);
	    goto Cleanup;
    }


	// Determine sample position.
	SampleIndex = Max(SampleIndex + 512 - NUM_LIPSYNCH_SAMPLES / 2, 0);
	SampleIndex = Min(SampleIndex, NumSamples - NUM_LIPSYNCH_SAMPLES - 1);

	// Create new buffer.
	SampleData = new ALfloat[NUM_LIPSYNCH_SAMPLES];
	appMemzero(SampleData, NUM_LIPSYNCH_SAMPLES); // Needed?

	// Download sample data. AL_FLOAT_SOFT values are in the range [-1,1].
	guard(Download);
	alGetBufferSamplesSOFT(Playing.Sound->ID, SampleIndex, NUM_LIPSYNCH_SAMPLES, AL_MONO_SOFT, AL_FLOAT_SOFT, SampleData);
	if ((Error = alGetError()) != AL_NO_ERROR)
    {
        warnf(TEXT("Failed to query sample data (%ls)."), appFromAnsi(alGetString(Error)));
        goto Cleanup;
    }
	unguard;

	// Maybe Apply window function to sample data?

	// Calculate average volume (if needed sum up in pairs for better precission later).
	guard(CalcAvgVolume);
	for (INT i = 0; i<NUM_LIPSYNCH_SAMPLES; i++)
	{
		AvgVolume += Abs(SampleData[i]);
		MinVolume = Min(MinVolume, SampleData[i]);
		MaxVolume = Max(MaxVolume, SampleData[i]);
	}
	AvgVolume /= NUM_LIPSYNCH_SAMPLES;
	unguard;

	// First output :-)
	//debugf( TEXT("Volume of speech samples for %ls is (Min=%f,Avg=%f,Max=%f)."), *Pawn->FamiliarName, MinVolume, AvgVolume, MaxVolume );
	//debugf( TEXT("SampleRate is %i"), SampleRate );

	// Close the mouth if below a threshold.
	if (AvgVolume<0.07)
	{
		//debugf( TEXT("Shutting mouth for to low average volume (%f) on %ls."), AvgVolume, *Pawn->FamiliarName );
		Pawn->nextPhoneme = TEXT("X");
		goto Cleanup;
	}

	// So now lets find out how to use KissFFT...
	guard(KissMe);

	// Query amount of memory kiss needs. That way we can use GMalloc instead of using the default C allocators kiss would use otherwise.
	size_t KissMemLen = 0;
	kiss_fft_alloc(NUM_LIPSYNCH_SAMPLES, 0, NULL, &KissMemLen);
	check(KissMemLen>0);
	void* KissMem = appMalloc(KissMemLen, TEXT("KissCfg"));
	check(KissMem);

	// Create config.
	kiss_fft_cfg KissCfg = kiss_fft_alloc(NUM_LIPSYNCH_SAMPLES, /* is inverse fft */ 0, KissMem, &KissMemLen);
	check(KissCfg);

	// Allocate buffer for kiss fft in/output.
	kiss_fft_cpx* KissIn = new kiss_fft_cpx[NUM_LIPSYNCH_SAMPLES];
	kiss_fft_cpx* KissOut = new kiss_fft_cpx[NUM_LIPSYNCH_SAMPLES];

	// Feed.
	guard(Feed);
	for (INT i = 0; i<NUM_LIPSYNCH_SAMPLES; i++)
	{
		KissIn[i].r = SampleData[i];
		KissIn[i].i = 0.0f;
	}
	unguard;

	// Transform.
	guard(Transform);
	kiss_fft(KissCfg, KissIn, KissOut);
	unguard;

	// Make positive.
	guard(MakePositive);
	for (INT i = 0; i<NUM_LIPSYNCH_SAMPLES / 2; i++)
		KissOut[i].r = Abs(KissOut[i].r);
	unguard;

	// Run ion code..
	guard(Analyze);

	// Seach for frequency with maximal amplitude.
	FLOAT MaxAmp = -99999.0;
	FLOAT MaxFreq = 0;

	// Average over multiple frequencies to get a more stable result.
	// Basic problem problem on averaging is that the distance between the low frequencies is just 3-4 samples.
	for (INT i = 0; i<NUM_LIPSYNCH_SAMPLES / 2; i += 2)
	{
		FLOAT Avg = 0.0f;
		for (INT j = 0; j<NUM_LIPSYNCH_SAMPLES / 1024; j++)
			Avg += KissOut[i].r;

		if (MaxAmp<Avg)
		{
			MaxAmp = Avg;
			MaxFreq = i;
		}
	}
	MaxAmp *= ((FLOAT)NUM_LIPSYNCH_SAMPLES) / 512.0f;

	//debugf( TEXT("SampleRate=%u MaxAmp=%f, MaxFreq=%f"), SampleRate, MaxAmp, MaxFreq );

	// Max of 22050 for 44100Hz sample.
	FLOAT Res = MaxFreq*((FLOAT)SampleRate) / ((FLOAT)NUM_LIPSYNCH_SAMPLES);

	// Maybe increase threshold?
	if (MaxAmp>5.0)
	{
		// Triage.
		if (Res>2000.0) Pawn->nextPhoneme = TEXT("T"); // 46.5
		else if (Res>1500.0) Pawn->nextPhoneme = TEXT("F"); // 34.9
		else if (Res>600.0) Pawn->nextPhoneme = TEXT("A"); // 13.9
		else if (Res>400.0) Pawn->nextPhoneme = TEXT("O"); //  9.3
		else if (Res>250.0) Pawn->nextPhoneme = TEXT("U"); //  5.8
		else if (Res>100.0) Pawn->nextPhoneme = TEXT("M"); //  2.3
		else                   Pawn->nextPhoneme = TEXT("E"); //
	}
	else
	{
		//debugf( TEXT("Shutting mouth for to low average FREQUENCY volume (%f) on %ls."), AvgVolume, *Pawn->FamiliarName );
		Pawn->nextPhoneme = TEXT("X");
	}
	unguard;

	// Free stuff.
	delete[] KissOut;
	delete[] KissIn;
	appFree(KissMem);
	unguard;

	// And the last bit.
Cleanup:
	if (SampleData)
		delete[] SampleData;

	// Summary.
	//debugf( TEXT("Result for %ls: %ls"), *Pawn->FamiliarName, *Pawn->nextPhoneme );
	unguard;
}
#endif

/*-----------------------------------------------------------------------------
The End.
-----------------------------------------------------------------------------*/

