//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

class CZonePing
{
	// CZonePing
  public:
	CZonePing(void);
	~CZonePing(void);

	PVOID operator new(size_t size);
	PVOID operator new[](size_t size);
	void operator delete(PVOID ptr);
	void operator delete[](PVOID ptr);

	// IZonePing
  public:
	typedef void PingCallbackFunc(
		uint32_t inet, uint32_t latency, PVOID context);

	int32_t __stdcall StartupServer(void);
	virtual HRESULT __stdcall StartupClient(uint32_t ping_interval_sec);
	virtual HRESULT __stdcall Shutdown(void);
	virtual HRESULT __stdcall Add(uint32_t inet);
	virtual HRESULT __stdcall Ping(uint32_t inet);
	virtual HRESULT __stdcall Remove(uint32_t inet);
	virtual HRESULT __stdcall RemoveAll(void);
	virtual HRESULT __stdcall Lookup(uint32_t inet, uint32_t* pLatency);
	virtual HRESULT __stdcall RegisterCallback(
		uint32_t inet, PingCallbackFunc* pfn, PVOID context);

  private:
	class ZonePing
	{
	  public:
		ZonePing(uint32_t inet = 0);

		enum PINGSTATE
		{
			UNKNOWN,
			PINGER,
			PINGEE
		};

		PVOID operator new(size_t size);
		PVOID operator new[](size_t size);
		void operator delete(PVOID ptr);
		void operator delete[](PVOID ptr);

		uint32_t m_inet;
		uint32_t m_latency;
		uint32_t m_samples;
		uint32_t m_tick;

		// mdm - for use with callbacks
		PingCallbackFunc* m_pCallback;
		PVOID m_context;

		PINGSTATE m_state;
		ZonePing* m_pNext;
	};

  protected:
	ZonePing* FindNextItem(ZonePing* pPing, bool* bWrapped);
	inline uint32_t GetListIndex(uint32_t inet)
	{
		return (inet & 0x000000FF) % m_PingIntervalSec;
	}

	void ScanForMissed(void);

	inline uint32_t GetTickDelta(uint32_t now, uint32_t then)
	{
		if (now >= then)
		{
			return now - then;
		}
		else
		{
			return INFINITE - then + now;
		}
	}

	inline uint32_t Get13BitTickDelta(uint32_t now, uint32_t then)
	{
		if (now >= then)
		{
			return now - then;
		}
		else
		{
			return 0x1FFF - then + now;
		}
	}

	BOOL CreateSocket(void);

	ZonePing* m_PingArray;
	uint32_t m_PingEntries;
	ZonePing* m_pCurrentItem;

	uint32_t* m_inetArray;
	uint32_t m_inetAlloc;

	uint32_t m_PingIntervalSec;
	uint32_t m_CurInterval;

	SOCKET m_Socket;
	BOOL m_bWellKnownPort;
	HANDLE m_hWellKnownPortEvent;

	HANDLE m_hStopEvent;
	CRITICAL_SECTION m_pCS[1];

	HANDLE m_hPingerThread;
	HANDLE m_hPingeeThread;

	static uint32_t WINAPI PingerThreadProc(LPVOID p);
	static uint32_t WINAPI PingeeThreadProc(LPVOID p);

	void PingerThread(void);
	void PingeeThread(void);

	HANDLE m_hStartupMutex;
	int32_t m_refCountStartup;

	uint32_t m_inetLocal[4]; // allow up to 4 ip address
};
