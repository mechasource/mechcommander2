//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

class CZonePing 
{
// CZonePing
public:
	CZonePing();
	~CZonePing();

    PVOID operator new(size_t size);
    PVOID operator new[](size_t size);
    void operator delete( PVOID ptr );
    void operator delete[] ( PVOID ptr );


// IZonePing
public:
    typedef void PingCallbackFunc( ULONG inet, ULONG latency, PVOID context );


    int __stdcall StartupServer();
    virtual HRESULT __stdcall StartupClient( ULONG ping_interval_sec );
    virtual HRESULT __stdcall Shutdown();
    virtual HRESULT __stdcall Add( ULONG inet );
    virtual HRESULT __stdcall Ping( ULONG inet );
    virtual HRESULT __stdcall Remove( ULONG inet );
    virtual HRESULT __stdcall RemoveAll();
    virtual HRESULT __stdcall Lookup( ULONG inet, ULONG *pLatency );
    virtual HRESULT __stdcall RegisterCallback( ULONG inet, PingCallbackFunc * pfn, PVOID context);


private:
    class ZonePing
    {
    public: 
        ZonePing(ULONG inet = 0);

	    enum PINGSTATE { UNKNOWN, PINGER, PINGEE };

        PVOID operator new(size_t size);
        PVOID operator new[](size_t size);
        void operator delete( PVOID ptr );
        void operator delete[] ( PVOID ptr );

        ULONG     m_inet;
        ULONG     m_latency;
        ULONG     m_samples;
        ULONG     m_tick;

        // mdm - for use with callbacks
        PingCallbackFunc * m_pCallback;
        PVOID    m_context;

        PINGSTATE m_state;
        ZonePing* m_pNext;

    };


protected:
    ZonePing * FindNextItem( ZonePing * pPing, bool * bWrapped );
    inline ULONG GetListIndex(ULONG inet)
	{
		return ( inet & 0x000000FF ) % m_PingIntervalSec;
	}

    void ScanForMissed( void );

    inline ULONG GetTickDelta( ULONG now, ULONG then )
    {
        if ( now >= then )
        {
            return now - then;
        }
        else
        {
            return INFINITE - then + now;
        }
    }

    inline ULONG Get13BitTickDelta( ULONG now, ULONG then )
    {
        if ( now >= then )
        {
            return now - then;
        }
        else
        {
            return 0x1FFF - then + now;
        }
    }

    BOOL    CreateSocket();

    ZonePing* m_PingArray;
    ULONG     m_PingEntries;
    ZonePing * m_pCurrentItem;

    ULONG*    m_inetArray;
    ULONG     m_inetAlloc;

    ULONG  m_PingIntervalSec;
    ULONG  m_CurInterval;

    SOCKET m_Socket;
    BOOL   m_bWellKnownPort;
    HANDLE m_hWellKnownPortEvent;

    HANDLE m_hStopEvent;
    CRITICAL_SECTION m_pCS[1];

    HANDLE m_hPingerThread;
    HANDLE m_hPingeeThread;

    static ULONG WINAPI PingerThreadProc( LPVOID p );
    static ULONG WINAPI PingeeThreadProc( LPVOID p );

    void PingerThread();
    void PingeeThread();

    HANDLE m_hStartupMutex;
    int32_t   m_refCountStartup;

    ULONG  m_inetLocal[4];  // allow up to 4 ip address
};
