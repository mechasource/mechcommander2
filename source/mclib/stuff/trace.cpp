//===========================================================================//
// File:	analysis.cpp                                                     //
// Contents: Utilities for using logic analyzer with parallel port dongle    //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
//#include "stuffheaders.hpp"

//#define TRACE_ENABLED
#if defined(TRACE_ENABLED)

#include <gameos.hpp>
#include <toolos.hpp>
#include <stuff/filestream.hpp>
#include <stuff/trace.hpp>

//##########################################################################
//############################    Trace    #################################
//##########################################################################

uint8_t Stuff::Trace::NextTraceID = 0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Stuff::Trace::Trace(PCSTR name, Type type) : Plug(DefaultData)
{
	traceNumber  = NextTraceID++;
	traceType	= (uint8_t)type;
	traceName	= name;
	lastActivity = 0;
	Check_Object(Stuff::TraceManager::Instance);
	Stuff::TraceManager::Instance->Add(this);
}

#if defined(USE_TIME_ANALYSIS)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::Trace::PrintUsage(float usage)
{
	// Check_Object(this);
	SPEW((GROUP_STUFF_TRACE, "%f+", usage));
}
#endif

//##########################################################################
//###########################    BitTrace    ###############################
//##########################################################################

uint8_t Stuff::BitTrace::NextActiveLine = 0;
int32_t Stuff::BitTrace::NextBit		= 0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Stuff::BitTrace::BitTrace(PCSTR name) : Trace(name, BitType)
{
	activeLine = NextActiveLine++;
	bitFlag	= (NextBit < 32) ? uint32_t(1 << NextBit++) : uint32_t(0);
#if defined(USE_ACTIVE_PROFILE)
	DEBUG_STREAM << name << " used trace line "
				 << static_cast<int32_t>(activeLine) << "!\n";
	if (!IsLineValidImplementation(activeLine))
	{
		STOP(("Invalid active trace line!"));
	}
#endif
	BitTrace::ResetTrace();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::BitTrace::DumpTraceStatus()
{
	// Check_Object(this);
	SPEW((GROUP_STUFF_TRACE, "%d = %d+", static_cast<int32_t>(activeLine),
		traceUp));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::BitTrace::ResetTrace()
{
#if defined(USE_TIME_ANALYSIS)
	traceUp		= 0;
	lastUpTime  = 0;
	totalUpTime = 0;
	Stuff::TraceManager::Instance->activeBits &= ~bitFlag;
#endif
#if defined(USE_ACTIVE_PROFILE)
	ClearLineImplementation(activeLine);
#endif
}

#if defined(USE_TIME_ANALYSIS)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::BitTrace::StartTiming()
{
	// Check_Object(this);
	totalUpTime = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
float Stuff::BitTrace::CalculateUsage(int64_t when, int64_t sample_time)
{
	if (traceUp > 0)
	{
		totalUpTime += when - lastActivity;
	}
	float result = static_cast<float>(totalUpTime / sample_time);
	SPEW((GROUP_STUFF_TRACE, "%4fs, +", totalUpTime));
	return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::BitTrace::PrintUsage(float usage)
{
	// Check_Object(this);
	SPEW((GROUP_STUFF_TRACE, "%4f%% CPU+", (usage * 100.0f)));
#if defined(USE_ACTIVE_PROFILE)
	SPEW((GROUP_STUFF_TRACE, " (active on line %d)",
		static_cast<int32_t>(activeLine)));
#endif
}

#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::BitTrace::Set(void)
{
	// Check_Object(this);
	if (!traceUp++)
	{
#if defined(USE_ACTIVE_PROFILE)
		SetLineImplementation(activeLine);
#endif
		Stuff::TraceManager::Instance->activeBits |= bitFlag;
#if defined(USE_TIME_ANALYSIS) || defined(USE_TRACE_LOG)
		int64_t now = gos_GetHiResTime();
#endif
#if defined(USE_TIME_ANALYSIS)
		lastActivity = now;
#endif
#if defined(USE_TRACE_LOG)
		// Check_Object(traceManager);
		IncrementSampleCount();
		MemoryStream* log = GetTraceLog();
		if (log)
		{
			Check_Object(log);
			TraceSample* sample = Cast_Pointer(TraceSample*, log->GetPointer());
			sample->sampleLength = sizeof(*sample);
			sample->sampleType   = TraceSample::GoingUp;
			sample->traceNumber  = traceNumber;
			sample->sampleTime   = now;
			log->AdvancePointer(sample->sampleLength);
		}
#endif
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::BitTrace::Clear(void)
{
	// Check_Object(this);
	if (--traceUp == 0)
	{
		Stuff::TraceManager::Instance->activeBits &= ~bitFlag;
#if defined(USE_TIME_ANALYSIS) || defined(USE_TRACE_LOG)
		int64_t now = gos_GetHiResTime();
#endif
#if defined(USE_TIME_ANALYSIS)
		lastUpTime = now - lastActivity;
#if 0 // HACK	
		Verify(lastUpTime >= 0.0f)
		totalUpTime += lastUpTime;
#else
		if (lastUpTime >= 0.0f)
		{
			totalUpTime += lastUpTime;
		}
#endif
#endif
#if defined(USE_TRACE_LOG)
		// Check_Object(traceManager);
		IncrementSampleCount();
		MemoryStream* log = GetTraceLog();
		if (log)
		{
			Check_Object(log);
			TraceSample* sample = Cast_Pointer(TraceSample*, log->GetPointer());
			sample->sampleLength = sizeof(*sample);
			sample->sampleType   = TraceSample::GoingDown;
			sample->traceNumber  = traceNumber;
			sample->sampleTime   = now;
			log->AdvancePointer(sample->sampleLength);
		}
#endif
#if defined(USE_ACTIVE_PROFILE)
		ClearLineImplementation(activeLine);
#endif
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::BitTrace::TestInstance(void) { Verify(traceUp >= 0); }

//##########################################################################
//########################    TraceManager    ##############################
//##########################################################################

Stuff::TraceManager* Stuff::TraceManager::Instance = nullptr;

void Stuff::TraceManager::InitializeClass(void)
{
	Verify(!Stuff::TraceManager::Instance);
	Stuff::TraceManager::Instance = new Stuff::TraceManager;
	Register_Object(Stuff::TraceManager::Instance);
}

void Stuff::TraceManager::TerminateClass()
{
	Unregister_Object(Stuff::TraceManager::Instance);
	delete Stuff::TraceManager::Instance;
	Stuff::TraceManager::Instance = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Stuff::TraceManager::TraceManager(void) : traceChain(nullptr)
{
	sampleStart		   = 0;
	actualSampleCount  = 0;
	ignoredSampleCount = 0;
	traceCount		   = 0;
	activeTraceLog	 = nullptr;
	allocatedTraceLog  = nullptr;
	activeBits		   = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Stuff::TraceManager::~TraceManager()
{
	// Check_Object(this);
	if (allocatedTraceLog)
	{
		Check_Object(allocatedTraceLog);
		allocatedTraceLog->Rewind();
		TraceSample* samples =
			Cast_Pointer(TraceSample*, allocatedTraceLog->GetPointer());
		Unregister_Object(allocatedTraceLog);
		delete allocatedTraceLog;
		activeTraceLog = allocatedTraceLog = nullptr;
		Unregister_Pointer(samples);
		delete[] samples;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::TraceManager::Add(Trace* trace)
{
	// Check_Object(this);
	traceCount = (uint8_t)(traceCount + 1);
	traceChain.Add(trace);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::TraceManager::DumpTracesStatus()
{
	ChainIteratorOf<Trace*> traces(&traceChain);
	Trace* trace;
	while ((trace = traces.ReadAndNext()) != nullptr)
	{
		Check_Object(trace);
		SPEW((GROUP_STUFF_TRACE, "%s: +", trace->traceName));
		trace->DumpTraceStatus();
		SPEW((GROUP_STUFF_TRACE, ""));
	}
	SPEW((GROUP_STUFF_TRACE, ""));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::TraceManager::ResetTraces()
{
	ChainIteratorOf<Trace*> traces(&traceChain);
	Trace* trace;
	while ((trace = traces.ReadAndNext()) != nullptr)
	{
		Check_Object(trace);
		trace->ResetTrace();
	}
#if defined(USE_TRACE_LOG)
	actualSampleCount  = 0;
	ignoredSampleCount = 0;
	if (allocatedTraceLog)
	{
		allocatedTraceLog->Rewind();
	}
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
PCSTR
Stuff::TraceManager::GetNameOfTrace(int32_t bit_no)
{
	//
	// Set up the iterator
	//
	ChainIteratorOf<Trace*> traces(&traceChain);
	Trace* trace;
	while ((trace = traces.ReadAndNext()) != nullptr)
	{
		Check_Object(trace);
		if (trace->traceType == Trace::BitType)
		{
			if (!bit_no)
			{
				break;
			}
			--bit_no;
		}
	}
	Check_Object(trace);
	return trace ? trace->traceName : nullptr;
}

#if defined(USE_TIME_ANALYSIS)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::TraceManager::StartTimingAnalysis()
{
	sampleStart = gos_GetHiResTime();
	ChainIteratorOf<Trace*> traces(&traceChain);
	Trace* trace;
	while ((trace = traces.ReadAndNext()) != nullptr)
	{
		Check_Object(trace);
		trace->StartTiming();
		trace->lastActivity = sampleStart;
	}
#if defined(USE_TRACE_LOG)
	actualSampleCount  = 0;
	ignoredSampleCount = 0;
	if (allocatedTraceLog)
	{
		allocatedTraceLog->Rewind();
	}
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t Stuff::TraceManager::SnapshotTimingAnalysis(bool print)
{
	int64_t now  = gos_GetHiResTime();
	int64_t time = now - sampleStart;
	if (time < SMALL)
	{
		return false;
	}
	ChainIteratorOf<Trace*> traces(&traceChain);
	Trace* trace;
	if (print)
	{
		SPEW((GROUP_STUFF_TRACE, "TIMING ANALYSIS"));
		SPEW((GROUP_STUFF_TRACE, "Sample length: %4fs", time));
	}
	while ((trace = traces.ReadAndNext()) != nullptr)
	{
		Check_Object(trace);
		if (print)
		{
			SPEW((GROUP_STUFF_TRACE, "%s: +", trace->traceName));
			float usage = trace->CalculateUsage(now, time);
			trace->PrintUsage(usage);
			SPEW((GROUP_STUFF_TRACE, ""));
		}
		trace->StartTiming();
		trace->lastActivity = now;
	}
	SPEW((GROUP_STUFF_TRACE, ""));
	sampleStart = now;
	return true;
}
#endif

#if defined(USE_TRACE_LOG)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::TraceManager::CreateTraceLog(
	size_t max_trace_count, bool start_logging)
{
	// Check_Object(this);
	Verify(!allocatedTraceLog);
	TraceSample* samples = new TraceSample[max_trace_count];
	Register_Pointer(samples);
	allocatedTraceLog =
		new MemoryStream(samples, max_trace_count * sizeof(TraceSample));
	Register_Object(allocatedTraceLog);
	if (start_logging)
	{
		activeTraceLog = allocatedTraceLog;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::TraceManager::SaveTraceLog(PCSTR filename)
{
	// Check_Object(this);
	if (allocatedTraceLog)
	{
		Check_Object(allocatedTraceLog);
		//
		//--------------------------------------------------------
		// Rewind the memory stream and save it out in a disk file
		//--------------------------------------------------------
		//
		FileStream output(filename, FileStream::WriteOnly);
		size_t size = allocatedTraceLog->GetBytesUsed();
		if (size > 0)
		{
			uint8_t trace_count = GetTraceCount();
			//
			//----------------------------
			// Write out the record header
			//----------------------------
			//
			output << static_cast<int32_t>('RVNO') << sizeof(int32_t) << 2;
			//
			//---------------------------------------------------------
			// Write out the header section after figuring out its size
			//---------------------------------------------------------
			//
			ChainIteratorOf<Trace*> traces(&traceChain);
			Trace* trace;
			size_t header_size = sizeof(int32_t);
			while ((trace = traces.ReadAndNext()) != nullptr)
			{
				header_size += 2 * sizeof(int32_t);
				header_size += (strlen(trace->traceName) + 4) & ~3;
			}
			output << static_cast<int32_t>('HDRS') << header_size;
			output << static_cast<int32_t>(trace_count);
			traces.First();
			while ((trace = traces.ReadAndNext()) != nullptr)
			{
				size_t str_len = strlen(trace->traceName) + 1;
				header_size	= sizeof(int32_t) + ((str_len + 4) & ~3);
				output << header_size << static_cast<int32_t>(trace->traceType);
				output.WriteBytes(trace->traceName, str_len);
				while (str_len & 3)
				{
					output << '\0';
					++str_len;
				}
			}
			output << static_cast<int32_t>('LOGS') << ((size + 3) & ~3);
			allocatedTraceLog->Rewind();
			output.WriteBytes(allocatedTraceLog->GetPointer(), size);
			while (size & 3)
			{
				output << '\0';
				++size;
			}
		}
		//
		//-------------------
		// Release the memory
		//-------------------
		//
		output.Close();
		TraceSample* samples =
			Cast_Pointer(TraceSample*, allocatedTraceLog->GetPointer());
		Unregister_Object(allocatedTraceLog);
		delete allocatedTraceLog;
		activeTraceLog = allocatedTraceLog = nullptr;
		Unregister_Pointer(samples);
		delete[] samples;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::TraceManager::MarkTraceLog()
{
	// Check_Object(this);
	if (activeTraceLog)
	{
		Check_Object(activeTraceLog);
		int64_t now = gos_GetHiResTime();
		TraceSample* sample =
			Cast_Pointer(TraceSample*, activeTraceLog->GetPointer());
		sample->sampleLength = sizeof(*sample);
		sample->sampleType   = TraceSample::Marker;
		sample->sampleTime   = now;
		sample->traceNumber  = 0;
		activeTraceLog->AdvancePointer(sample->sampleLength);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::TraceManager::SuspendTraceLogging()
{
	// Check_Object(this);
	if (activeTraceLog)
	{
		Check_Object(activeTraceLog);
		int64_t now = gos_GetHiResTime();
		TraceSample* sample =
			Cast_Pointer(TraceSample*, activeTraceLog->GetPointer());
		sample->sampleLength = sizeof(*sample);
		sample->sampleType   = TraceSample::SuspendSampling;
		sample->sampleTime   = now;
		sample->traceNumber  = 0;
		activeTraceLog->AdvancePointer(sample->sampleLength);
		activeTraceLog = nullptr;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::TraceManager::ResumeTraceLogging()
{
	// Check_Object(this);
	if (allocatedTraceLog && !activeTraceLog)
	{
		Check_Object(allocatedTraceLog);
		activeTraceLog = allocatedTraceLog;
		int64_t now	= gos_GetHiResTime();
		TraceSample* sample =
			Cast_Pointer(TraceSample*, activeTraceLog->GetPointer());
		sample->sampleLength = sizeof(*sample);
		sample->sampleType   = TraceSample::ResumeSampling;
		sample->sampleTime   = now;
		sample->traceNumber  = 0;
		activeTraceLog->AdvancePointer(sample->sampleLength);
	}
}

#if 0
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Stuff::TraceManager::WriteClassBlocks(MemoryStream& stream)
{
	stream << static_cast<int32_t>('MSGS')
		   << RegisteredClass::DefaultData->WriteClassBlock(stream, false);
	RegisteredClass::DefaultData->WriteClassBlock(stream, true);
}
#endif

#endif

#if defined(USE_ACTIVE_PROFILE)

void Stuff::TraceManager::SetLineImplementation(uint8_t) {}

void Stuff::TraceManager::ClearLineImplementation(uint8_t) {}

bool Stuff::TraceManager::IsLineValidImplementation(uint8_t) { return true; }

#endif

#endif
