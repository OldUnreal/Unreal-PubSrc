/* ============================================================================
	SwFMODMacro.h:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */
#ifndef SWFMODMACRO_H
#define SWFMODMACRO_H


/* ----------------------------------------------------------------------------
	Build
---------------------------------------------------------------------------- */

//
// Print error to log and continue
//
#define SWF_DO_WARN 0

//
// Print error to log and terminate, overrides SWF_DO_WARN
//
#define SWF_DO_FAIL 0


/* ----------------------------------------------------------------------------
	Types
---------------------------------------------------------------------------- */
#define UINT unsigned int


/* ----------------------------------------------------------------------------
	Logging
-------------------------------------------------------------------------------
 Types:
 - INFO = public info for player
 - DEV = hidden info, can be unsuppressed by player for troubleshooting
 - WARN = public warning
 - DEBUG = for debugging only, shouldn't be visible in public builds
---------------------------------------------------------------------------- */
#define SWF_LOG							GLog->Logf
#define SWF_PLOG						*SwTimeStr(), __FUNC_NAME__
#define SWF_PLOGD						*SwTimeStr(), __FUNC_NAME__, appFromAnsi(__FILE__), __LINE__

#define SWF_SEP							FString(TEXT(" | "))
#define SWF_STSTR(msg)					FString::Printf( TEXT(#msg) TEXT("=%ls, "), *ToStr(msg) )
#define SWF_STATIC_FORMAT(code,str)		FString::Printf( TEXT("%ls :: [") TEXT(#code) TEXT("]"), *ToStr(str))

#define SWF_LOG_INFO(str)				SwDebugLog( SWF_NAME, ToStr(str) );
#define SWF_LOG_DEV(str)				SwDebugLog( NAME_DevSound, ToStr(str),0,0,0,true );
#define SWF_LOG_WARN(str)				SwDebugLog( SWF_NAME_WARNING, ToStr(str), __FUNC_NAME__,0,0, true );
#define SWF_LOG_DEBUG(str)				SwDebugLog( SWF_NAME, ToStr(str), __FUNC_NAME__, __FILE__, __LINE__, true );

#define SWF_LOG_INFO_STATIC(code,str)	SWF_LOG_INFO( SWF_STATIC_FORMAT(code,str) );
#define SWF_LOG_DEV_STATIC(code,str)	SWF_LOG_DEV( SWF_STATIC_FORMAT(code,str) );
#define SWF_LOG_WARN_STATIC(code,str)	SWF_LOG_WARN( SWF_STATIC_FORMAT(code,str) );
#define SWF_LOG_DEBUG_STATIC(code,str)	SWF_LOG_DEBUG( SWF_STATIC_FORMAT(code,str) );



/* ----------------------------------------------------------------------------
	Failure reporting
---------------------------------------------------------------------------- */
#define SWF_FAIL(text)				appFailAssert( appToAnsi(*FString::Printf( TEXT("%ls :: %ls :: FAILED: ") TEXT(#text), SWF_PLOG )), __FILE__, __LINE__ );
#define SWF_FAIL_FMOD(text,msg)		appFailAssert( appToAnsi(*FString::Printf( TEXT("%ls :: %ls :: FAILED: ") TEXT(#text) TEXT(" FMOD: %ls !"), SWF_PLOG, *ToStr(result))), __FILE__, __LINE__  );

#define SWF_FMOD_HANDLER_ERROR(result,text) ++GSwfErrorCount;

#define SWF_FMOD_CALL(text)		{result = text; if( result != FMOD_OK ){ SWF_FMOD_HANDLER_ERROR(result,text); }}
#define SWF_FMOD_RCALL(text)	{result = text; if( result != FMOD_OK ){ SWF_FMOD_HANDLER_ERROR(result,text); return; }}
#define SWF_FMOD_RCALL0(text)	{result = text; if( result != FMOD_OK ){ SWF_FMOD_HANDLER_ERROR(result,text); return 0; }}



/* ----------------------------------------------------------------------------
	Parameter verification
---------------------------------------------------------------------------- */
#define SWF_VERIFY_FLOAT(text)				{ text = SwVerifyFloat(text) ? text : 0.0f; }
#define SWF_VERIFY_FVECTOR(text)			{ SWF_VERIFY_FLOAT(text.X); SWF_VERIFY_FLOAT(text.Y); SWF_VERIFY_FLOAT(text.Z); }
#define SWF_VERIFY_FMODVECTOR(text)			{ SWF_VERIFY_FLOAT(text.x); SWF_VERIFY_FLOAT(text.y); SWF_VERIFY_FLOAT(text.z); }
#define SWF_VERIFY_FVECTOR_NORMAL(text)		{ text = text.SafeNormal(); SWF_VERIFY_FVECTOR(text); }
#define SWF_VERIFY_FMODVECTOR_NORMAL(text)	{ FVector fv = FVector(text.x,text.y,text.z).SafeNormal(); text.x=fv.X; text.y=fv.Y; text.z=fv.Z; SWF_VERIFY_FMODVECTOR(text); }



/* ----------------------------------------------------------------------------
	If should warn
---------------------------------------------------------------------------- */
#if SWF_DO_WARN
	#undef SWF_FMOD_HANDLER_ERROR
	#define SWF_FMOD_HANDLER_ERROR(result,text) ++GSwfErrorCount; SWF_LOG_WARN_STATIC(text,result);

	#undef SWF_VERIFY_FLOAT
	#define SWF_VERIFY_FLOAT(text) {if(!SwVerifyFloat(text)) { SWF_LOG_WARN_STATIC(text,text); text = 0.0f; }}
#endif



/* ----------------------------------------------------------------------------
	If should fail
---------------------------------------------------------------------------- */
#if SWF_DO_FAIL
	#undef SWF_FMOD_HANDLER_ERROR
	#define SWF_FMOD_HANDLER_ERROR(result,text) ++GSwfErrorCount; SWF_FAIL_FMOD(result,0);

	#undef SWF_VERIFY_FLOAT
	#define SWF_VERIFY_FLOAT(text) {if(!SwVerifyFloat(text)) { SWF_FAIL(text); }}
#endif




#endif
/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
