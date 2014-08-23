#pragma once
//===========================================================================//
// File:	 Profiler.hpp													 //
// Contents: Profiler API's													 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

extern ULONG* TraceBuffer;
extern gos_VERTEX* LineBuffer;





extern UCHAR	ProfileFlags[32];			// Remember graphs activated 
extern ULONG DebuggerTextures;
extern float PercentHistory[512];		// Used to remember cycles per frame
extern bool NewPerformanceRegister;

//
// Structure used to hold statistics
//
typedef struct _Stat
{
	_Stat*			pNext;				// Pointer to Next
	ULONG			Flags;				// flags
	PSTR			TypeName;			// Pointer to type name
	gosType			Type;				// Type of variable
	ULONG			Count;				// Count for average
	PVOID			pVariable;			// Pointer to the variable in question
		
	float			MaxPercentage;		// Max percentage
	float			MinPercentage;		// Min percentage
	float			TotalPercentage;	// Total percentage
	float			MaxValue;			// Highest graph value (if flag set)
	float			Minimum;			// Lowest
	float			Maximum;			// Highest
	float			Total;				// Total (for average)

	float*			History;			// Last 512 values (for graphing)
	bool			BlockOwner;			// Do I own the whole statistic history block?

	char			Name[1];			// Name of statistic

} Stat;


cint32_t StatsInBlock=32;				// Number of stats in a single block

__inline float __stdcall GetHistory( Stat* pStat, int GraphHead )
{
	return pStat->History[GraphHead*StatsInBlock];
}
__inline void __stdcall SetHistory( Stat* pStat, int GraphHead, float Value )
{
	pStat->History[GraphHead*StatsInBlock]=Value;
}


extern Stat*	pStatistics;		// Pointer to chain of statistics
extern ULONG	GraphHead;			// Pointer to current element in statistics History
extern ULONG	NumberStatistics;	// Entries in statistic list
extern Stat* GraphsActive[20];		// Number of frame graphs active
extern ULONG NumberGraphsActive;

void __stdcall UpdateGraphs(void);
void __stdcall InitStatistics(void);
void __stdcall DestroyStatistics(void);
void __stdcall UpdateStatistics(void);
void __stdcall ResetStatistics(void);

#if defined(LAB_ONLY)
void __stdcall SaveStatistics(void);
void __stdcall RestoreStatistics(void);
#endif



