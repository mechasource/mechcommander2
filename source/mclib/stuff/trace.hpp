//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _TRACE_HPP_
#define _TRACE_HPP_

//#if defined(TRACE_ENABLED) 

#include <stuff/chain.hpp>

namespace Stuff {

	//#######################################################################
	//###################    TraceSample    #################################
	//#######################################################################

#pragma pack( push, trace_pack, 4 )
	struct TraceSample
	{
		size_t	sampleLength;
		int64_t	sampleTime;
		uint8_t traceNumber;
		uint8_t sampleType;

		typedef enum Type {
			GoingUp = 0,
			GoingDown,
			Marker,
			IntegerSnapshot,
			ScalarSnapshot,
			SuspendSampling,
			ResumeSampling,
			QueueingEvent,
			DispatchingEvent
		};
		void TestInstance() {}
	};

	template <class T> struct SnapshotOf:
	public TraceSample
	{
		T snapShot;
	};

#pragma pack( pop, trace_pack )

	//#######################################################################
	//#########################    Trace    #################################
	//#######################################################################

	class _declspec(novtable) Trace:
		public Plug
	{
		friend class TraceManager;

	public:
		enum Type {
			BitType,
			IntegerType,
			ScalarType,
			EventType
		};

	protected:
		static uint8_t NextTraceID;

		PCSTR	traceName;
		int64_t	lastActivity;
		uint8_t
			traceNumber,
			traceType;

		Trace(
			PCSTR name,
			Type type
			);

		MemoryStream*
			GetTraceLog();
		void
			IncrementSampleCount();

		virtual void
			DumpTraceStatus()=0;
		virtual void
			ResetTrace()=0;

#if defined(USE_TIME_ANALYSIS)
		virtual void
			StartTiming()=0;
		virtual float
			CalculateUsage(
			int64_t when,
			int64_t sample_time
			)=0;
		virtual void
			PrintUsage(float usage);
#endif
	};

	//#######################################################################
	//########################    BitTrace    ###############################
	//#######################################################################

	class BitTrace:
		public Trace
	{
	protected:
		static uint8_t
			NextActiveLine;

		int
			traceUp;
		int64_t
			lastUpTime,
			totalUpTime;
		uint8_t
			activeLine;
		ULONG
			bitFlag;
		static int
			NextBit;

		void
			DumpTraceStatus();
		void
			ResetTrace();
#if defined(USE_TIME_ANALYSIS)
		void
			StartTiming();
		float
			CalculateUsage(
			int64_t when,
			int64_t sample_time
			);
		void
			PrintUsage(float usage);
#endif

	public:
		BitTrace(PCSTR name);

		void
			Set();
		void
			Clear();
		bool
			IsTraceOn()
		{Check_Object(this); return traceUp>0;}

		int64_t
			GetLastUpTime()
		{Check_Object(this); return lastUpTime;}
		int64_t
			GetTotalUpTime()
		{Check_Object(this); return totalUpTime;}

		void
			TestInstance();
	};

	//#######################################################################
	//########################    TraceOf    ################################
	//#######################################################################

	template <class T> class TraceOf:
	public Trace
	{
	protected:
		int64_t
			weightedSum;
		T
			currentValue;

		TraceSample::Type
			sampleType;

		void
			DumpTraceStatus();
		void
			ResetTrace();
#if defined(USE_TIME_ANALYSIS)
		void
			StartTiming();
		float
			CalculateUsage(
			int64_t when,
			int64_t sample_time
			);
		void
			PrintUsage(float usage);
#endif

	public:
		TraceOf(
			PCSTR name,
			const T& initial_value,
			Type trace_type,
			TraceSample::Type sample_type
			);

		void
			TakeSnapshot(const T& value);
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	template <class T> TraceOf<T>::TraceOf(
		PCSTR name,
		const T& initial_value,
		Type trace_type,
		TraceSample::Type sample_type
		):
	Trace(name, trace_type)
	{
		currentValue = initial_value;
		sampleType = sample_type;
		TraceOf<T>::ResetTrace();
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	template <class T> void
		TraceOf<T>::DumpTraceStatus()
	{
		Check_Object(this);
		Spew(GROUP_STUFF_TRACE, currentValue);
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	template <class T> void
		TraceOf<T>::ResetTrace()
	{
#if defined(USE_TIME_ANALYSIS)
		weightedSum = 0.0;
#endif
	}

#if defined(USE_TIME_ANALYSIS)

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	template <class T> void
		TraceOf<T>::StartTiming()
	{
		Check_Object(this);
		weightedSum = 0.0;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	template <class T> float
		TraceOf<T>::CalculateUsage(
		int64_t when,
		int64_t sample_time
		)
	{
		int64_t last_part = when - lastActivity;
		weightedSum += last_part * currentValue;
		float result = static_cast<float>(weightedSum / sample_time);
		weightedSum = 0.0;
		return result;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	template <class T> void
		TraceOf<T>::PrintUsage(float usage)
	{
		Check_Object(this);
		Spew(GROUP_STUFF_TRACE, usage);
	}

#endif

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	template <class T> void
		TraceOf<T>::TakeSnapshot(const T& value)
	{
		Check_Object(this);

#if defined(USE_TIME_ANALYSIS) || defined(USE_TRACE_LOG)
		int64_t now = gos_GetHiResTime();
#endif

#if defined(USE_TIME_ANALYSIS)
		int64_t last_part = now - lastActivity;
		weightedSum += last_part * currentValue;
		lastActivity = now;
#endif

		currentValue = value;

#if defined(USE_TRACE_LOG)
		IncrementSampleCount();
		MemoryStream *log = GetTraceLog();
		if (log)
		{
			Check_Object(log);
			SnapshotOf<T> *sample =
				Cast_Pointer(SnapshotOf<T>*, log->GetPointer());
			sample->sampleLength = sizeof(*sample);
			sample->sampleType = (uint8_t)sampleType;
			sample->traceNumber = traceNumber;
			sample->sampleTime = now;
			sample->snapShot = currentValue;
			log->AdvancePointer(sample->sampleLength);
		}
#endif
	}

	//#######################################################################
	//#####################    TraceManager    ##############################
	//#######################################################################

	class TraceManager
#if defined(_ARMOR)
		: public Stuff::Signature
#endif
	{
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		friend class Trace;
		friend class BitTrace;

	protected:
		ChainOf<Trace*> traceChain;
		int64_t sampleStart;
		int
			actualSampleCount,
			ignoredSampleCount;
		MemoryStream
			*allocatedTraceLog,
			*activeTraceLog;
		uint8_t traceCount;
		ULONG
			activeBits;

		void
			Add(Trace *trace);

	public:
		TraceManager();
		~TraceManager();

		static TraceManager
			*Instance;

		uint8_t
			GetTraceCount()
		{Check_Object(this); return traceCount;}
		void
			DumpTracesStatus();
		void
			ResetTraces();

		ULONG
			GetBitTraceStatus()
		{Check_Object(this); return activeBits;}
		PCSTR
			GetNameOfTrace(int bit_no);

#if defined(USE_TIME_ANALYSIS)
		void
			StartTimingAnalysis();
		int
			SnapshotTimingAnalysis(bool print=false);
#endif

#if defined(USE_TRACE_LOG)
		void
			CreateTraceLog(
			size_t max_trace_samples,
			bool start_sampling
			);
		void
			SaveTraceLog(PCSTR filename);
		void
			MarkTraceLog();
		void
			SuspendTraceLogging();
		void
			ResumeTraceLogging();
#endif

#if defined(USE_ACTIVE_PROFILE)
		virtual void
			SetLineImplementation(uint8_t line);
		virtual void
			ClearLineImplementation(uint8_t line);
		virtual bool
			IsLineValidImplementation(uint8_t line);
#endif

		void
			TestInstance()
		{}
	};

	inline MemoryStream*
		Trace::GetTraceLog()
	{
		Check_Object(this); Check_Object(TraceManager::Instance);
		return TraceManager::Instance->activeTraceLog;
	}

	inline void
		Trace::IncrementSampleCount()
	{
		Check_Object(this); Check_Object(TraceManager::Instance);
		if (!TraceManager::Instance->activeTraceLog)
		{
			++TraceManager::Instance->ignoredSampleCount;
		}
		else
		{
			++TraceManager::Instance->actualSampleCount;
		}
	}

#endif

}

//#endif
