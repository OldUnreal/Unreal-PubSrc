/*=============================================================================
	UnHook: Memory scanning for hooks

Revision history:
	* Created by Michiel Hendriks
=============================================================================*/

#ifndef UNHOOK_H
#define UNHOOK_H

#ifdef UTPG_HOOK_SCAN
// perform memory scanning
UBOOL MemPageScan(FOutputDevice& Ar);
// scan a single memory page
//UBOOL scanSinglePage(FOutputDevice& Ar, MEMORY_BASIC_INFORMATION* mbi);
// find something in the memory
//UBOOL findInMem(char *p, char *pend, char *find, int findLen);
// used to set the memory string
void SetMemSearchString(char *str1, char *str2);

#endif
#endif

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
