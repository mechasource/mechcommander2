//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _GUNGAMELIST_H_
#define _GUNGAMELIST_H_

// #include "hash.h"

extern PCSTR GAMELIST_KEY_INSTANCE;
extern PCSTR GAMELIST_KEY_SERVER_NAME;
extern PCSTR GAMELIST_KEY_SERVER_ADDRESS;
extern PCSTR GAMELIST_KEY_SERVER_PORT;
extern PCSTR GAMELIST_KEY_NUM_PLAYERS;
extern PCSTR GAMELIST_KEY_NUM_BOTS;
extern PCSTR GAMELIST_KEY_MAX_PLAYERS;
extern PCSTR GAMELIST_KEY_PRODUCT_NAME;
extern PCSTR GAMELIST_KEY_PRODUCT_VERSION;
extern PCSTR GAMELIST_KEY_MOD_NAME;
extern PCSTR GAMELIST_KEY_MAP_TYPE;
extern PCSTR GAMELIST_KEY_PASSWORD_PROTECTED;
extern PCSTR GAMELIST_KEY_PASSWORD;
extern PCSTR GAMELIST_KEY_BROWSER_HANDLE;
extern PCSTR GAMELIST_KEY_PING;
extern PCSTR GAMELIST_KEY_GAME_TYPE; // rules for game (ctf, team fortress, etc...)
extern PCSTR GAMELIST_KEY_UPDATE_TIME; // whether the game was updated or not in the last EnumSessions
extern PCSTR GAMELIST_KEY_PROTOCOL;
extern PCSTR GAMELIST_KEY_FLAGS;
extern PCSTR GAMELIST_KEY_SERVER_INFO_STATUS;
extern PCSTR GAMELIST_KEY_REFRESH_PENDING;
extern PCSTR GAMELIST_VALUE_PROTOCOL_GUN;
extern PCSTR GAMELIST_VALUE_PROTOCOL_TCPIP_LAN;
extern PCSTR GAMELIST_VALUE_PROTOCOL_INTERNET;
extern PCSTR GAMELIST_VALUE_PROTOCOL_IPX;
extern PCSTR GAMELIST_VALUE_PROTOCOL_GAMESPY;
extern PCSTR GAMELIST_VALUE_REFRESH_PENDING_TRUE;
extern PCSTR GAMELIST_VALUE_REFRESH_PENDING_FALSE;
extern PCSTR GAMELIST_VALUE_SERVER_INFO_STATUS_PENDING;
extern PCSTR GAMELIST_VALUE_SERVER_INFO_STATUS_SUCCESS;
extern PCSTR GAMELIST_VALUE_SERVER_INFO_STATUS_ERROR;
extern PCSTR GAMELIST_VALUE_SERVER_INFO_STATUS_NOSERVER;
extern PCSTR PLAYERLIST_KEY_SERVER_ROWID;
extern PCSTR PLAYERLIST_KEY_PLAYER_NAME;
extern PCSTR PLAYERLIST_KEY_CLAN_NAME; // not yet in MW4 - 8/1/2000
extern PCSTR PLAYERLIST_KEY_UPDATE_TIME; // whether the player was updated or not in the last EnumPlayers
extern PCSTR ZONE_SERVER_KEY;
extern PCSTR ZONE_ADVERTISE_PORT_KEY;
extern PCSTR ZONE_SERVER_INTERNET_ADDRESS;
extern PCSTR ZONE_SERVER_LAN_ADDRESS;
extern PCSTR INVALID_PING_STR;

#define NETFLAGS_CONNECTION_TYPE_MASK	0x000000FF
#define NETFLAGS_DEDICATED_FLAG			0x00000100
#define NETFLAGS_STATSTRACKING_FLAG		0x00000200
#define NETFLAGS_PURESERVER_FLAG		0x00000400
#define NETFLAGS_LOCKED_FLAG			0x00000800
#define NETFLAGS_NOPING_FLAG            0x00001000 // mdm - indicates browsers shouldn't ping the game

#define INVALID_PING_VAL -1


extern "C" int __stdcall GetDirectPlayPort(void);

cint32_t ZONE_PING_INTERVAL = 20;

// mdm - change this back to INTERNET_ADDRESS when shipping externally.
#define ZONE_SERVER_DEFAULT_ADDRESS ZONE_SERVER_LAN_ADDRESS
//#define ZONE_SERVER_DEFAULT_ADDRESS ZONE_SERVER_INTERNET_ADDRESS

//#define PREPEND_PREFIX_TO_GAME_NAMES

// enumerates the current mode of operation for the
// various GUN initialization phases.
enum GUN_STATUS
{
    // values < 0 == error in that phase :
    // i.e. -GS_INIT_BROWSER implies an error
    // occured while initializing the browser
    GS_UNKNOWN_ERROR        = -(0x0fff),
    GS_DISCONNECTED         = 0x0000,
    GS_INIT                 = 0x0001,
    GS_INIT_BROWSER         = 0x0002,
    GS_FIND_QSERVER         = 0x0004,
    GS_QUERY_SERVERS        = 0x0008,
    GS_QUERY_PLAYERS        = 0x0010,
    GS_CLOSING              = 0x0020,
    GS_CLOSED               = 0x0040,
    GS_CONNECTED            = 0x0080
};

enum GUN_REGISTRATION_STATUS
{
	GRS_UNINITIALIZED,		// we haven't tried to register yet since the last shutdown
	GRS_INPROGRESS,			// currently trying to register
	GRS_FAILURE,			// failed to register on last attempt
	GRS_SUCCESS				// game was successfully registered.
};



class CZonePing;

namespace Browse
{

    // control functions for pings
    extern void PingCallback( DWORD inet, DWORD latency, void * context);

    typedef void * POSITION; // for use by CList
    /*
        This non-MFC version of CList pulled from web site at:
    
        http://puddle.etri.re.kr/~shbaek/c/clist.h
    */
    template<class CItem> class CList {
    protected:
        class CListNode {
        public:
            CItem m_Item;
            CListNode* m_pNext;
            CListNode* m_pPrev;
            CListNode() {m_pNext = NULL; m_pPrev = NULL;}

            void * operator new(size_t size) {
                return malloc(size);
            }
            void operator delete( void* ptr )
            {
                free(ptr);
            }
            void operator delete[] ( void* ptr )
            {
                free(ptr);
            }
        };
        CListNode* m_pHead;
        CListNode* m_pTail;
    
    public:
        CList();
        ~CList();
        bool IsEmpty();
        void RemoveAll(void);
        void AddHead(CItem&);
        void AddTail(CItem&);
        bool RemoveHead(void);
        bool RemoveIt(POSITION pos, CItem& ItemRet);
        bool Find(CItem&);
        bool AddNext(POSITION,CItem&);
        bool AddPrev(POSITION,CItem&);
        POSITION GetHeadPosition(void);
        POSITION GetTailPosition(void);
        CItem* GetNext(POSITION&);
        CItem* GetPrev(POSITION&);
        CItem* GetHead(void);
        uint32_t GetSize(void);

        void * operator new(size_t size);
        void operator delete( void* ptr );
        void operator delete[] ( void* ptr );
    };



   /*
    This class is the base class for all table-based read-only lists.
    Both the game and player lists inherit from this base functionality.
    NOTE: The ExtraData class, referenced inside the TableItem struct,
    should be subclassed to provide whatever additional fields the
    specific table needs.  See the implementation of GameList or
    PlayerList for an example of how this is done.
*/

    class TableItemFilter;
    class GameListFilter;
    class IndexedTableView;

class TableList
{
public:
    enum
    {
        INVALID_ID = -1
    };

    // This class is the base class for sub-classes
    // to use when adding extra, class-specific data
    // to the list of items.
    // NOTE: Unfortunately, this class apparently
    // has to be public in order for sub-classes
    // to inherit from it :-(.
    class ExtraData
    {
    public:
        virtual ~ExtraData() {}

        void * operator new(size_t size);
        void operator delete( void* ptr );
        void operator delete[] ( void* ptr );
    };

    // This is the generic struct which defines the
    // node used for implementing field lists.
    struct FieldItem
    {
        PSTR           key; // header name
        PSTR           value; // value of field
        int             iValue; // atoi() applied to 'value'

        // these are needed to override GameOS memory management
        void * operator new(size_t size);
        void operator delete( void* ptr );
        void operator delete[] ( void* ptr );
    };

    typedef CList<FieldItem *> FieldList;

    // This is the generic struct which defines
    // the node used for implementing the ListOfItems
    // .  Note the use of a pointer to extraData,
    // which allows sub-classes to store application-
    // specific data in the ListOfItems nodes.
    struct TableItem
    {
        PSTR  id;
        FieldList    fieldList;
        ExtraData * extraData;

        // these are needed to override GameOS memory management
        void * operator new(size_t size);
        void operator delete( void* ptr );
        void operator delete[] ( void* ptr );
    };

    TableList();
    virtual ~TableList();

    // create a new entry in the table, either with an auto-id
    // or with a specified ID.
    virtual bool CreateEntry( PCSTR  id );

    // data field operations
    virtual bool SetField( PCSTR  id, PCSTR  key, 
                           PCSTR  val );
    PCSTR  GetDataField( PCSTR  id,
                               PCSTR  key );
    virtual bool RemoveField( PCSTR  id, PCSTR  header );
    virtual bool Remove( PCSTR  id );
    void RemoveAll( void );

    // The indexed view is used for array-like access to the
    // list, as well as providing sorting and filtering capabilities.
    IndexedTableView * GetIndexedView( void ) const { return m_IndexedView; }


    // misc operations
    int  GetTotalCount( void ); // total number of table items 
    bool Exists( PCSTR  id );
    void EnumerateAll( CHash<TableItem, PCSTR >::PFITERCALLBACK pfnCB,
                       void * cookie );
    PCSTR  GetIDFromField( PCSTR  key,
                           PCSTR  val );
    PCSTR  GetIDFromField( PCSTR  key,
                            int val ); // compares iVal
    static void __cdecl Lock( void );
    static void __cdecl Unlock( void ); 

    // these are implemented to override the
    // behavior of GUNOS memory management
    void * operator new(size_t size);
    void operator delete( void* ptr );
    void operator delete[] ( void* ptr );

    static FieldItem * FindField( FieldList & pList,
                                  PCSTR  key );

protected:
    

    // hash table stuff
    static DWORD HashLPSTR( PCSTR  Key );
    static bool  HashCompare(TableItem *obj,PCSTR  key);
    typedef CHash<TableItem, PCSTR > ItemHash;
    

    ////////////////////
    // protected methods
    ////////////////////

    void FreeFieldList( FieldList * pList );
    virtual void FreeTableItem( TableItem * pItem );
    void FreeFieldItem( FieldItem * pItem );
    TableItem * FindItem( PCSTR  id );
    static PCSTR  FindFieldVal( FieldList & pList,
                               PCSTR  key );
    static bool ForEachFindField( TableItem * tableItem,
                           MTListNodeHandle handle, 
                           void * pThis );

    // this is the callback required for the m_Items.RemoveAll() call in
    // the RemoveAll function.
    static void FreeTableItemCallback( TableItem * pItem, void * pThis );


    //////////////////////////
    // protected data members
    //////////////////////////

    ItemHash m_Items;

    // extra vars needed or hash foreach callback
    PCSTR  m_SearchKey;
    PCSTR  m_SearchVal;
    PCSTR  m_SearchResult;

    IndexedTableView * m_IndexedView; // make list later if we want multiple views to be possible

private:
    // Friends
    friend IndexedTableView;

};


// for the moment, player lists are just table lists.
typedef TableList PlayerList;

class GameList : public TableList
{
public:
    GameList();
    virtual ~GameList();

    virtual bool CreateEntry( PCSTR  id ); 
    virtual bool SetField( PCSTR  id,
                           PCSTR  key,
                           PCSTR  val );

    bool GetGUID( PCSTR  id, GUID & gameGUID );
    bool GetID( PCSTR  gameName, PCSTR  & id );

    PCSTR  GetReferenceGameID( void );
    PlayerList * GetPlayerList( PCSTR  id );

    // ping management
    static bool _stdcall StartPingServer( void );
    static bool _stdcall StartPingClient( DWORD ping_interval_sec );
    static void _stdcall ShutdownPing( void );
    int  GetPing( PCSTR  id );
    int  GetPing( int index );




    void * operator new(size_t size);
    void * operator new[](size_t size);
    void operator delete( void* ptr );
    void operator delete[] ( void* ptr );

public:

    // protected type definitions
    class GameExtraData : public TableList::ExtraData
    {
    public:

        virtual ~GameExtraData();

        PSTR        name;
        GUID         gameGUID;
        PlayerList   players;

        void * operator new(size_t size);
        void operator delete( void* ptr );
        void operator delete[] ( void* ptr );
    };

protected:
    virtual void FreeTableItem( TableItem * pItem );

    char                m_ReferenceGameID[255];
    static CZonePing * s_ZonePing;
    static bool        s_ZonePingServerStarted;
    static bool        s_ZonePingClientStarted;
};

/*
    This class represents a filter that you can
    attach to an indexed view to indicate which items
    should be allowed to show up in the view.  This 
    is accomplished by configuring the TableListFilter
    using the AddNumericFilter and AddStringFilter 
    methods.  Then, attach the filter to an index view
    and the index view will call FilterItem on each item
    as it is added or changed to determine if the item
    should be in the indexed view.
*/

class TableItemFilter
{
public:
    // Types

    enum FilterType
    {
        FT_NONE,        // no filtering performed - default
        FT_INCLUDE_ALL, // include if all filter criterion match
        FT_INCLUDE_ANY, // include if any filter criterion match
        FT_EXCLUDE_ALL, // exclude if all filter criterion match
        FT_EXCLUDE_ANY  // exclude if any filter criterion match
    };

public:
    // Methods

    TableItemFilter();
    virtual ~TableItemFilter();

    virtual bool Filter( TableList::TableItem & tableItem );

    void SetFilterType( FilterType filterType );
    FilterType GetFilterType( void ) const;

    // general field-based filtering
    void AddNumericFilter( PCSTR  key, int minVal, int maxVal );
    void AddStringFilter( PCSTR  key, PCSTR  val );
    virtual bool ClearFilter(PCSTR  key );

    virtual void ClearAllFilters( void );
    

protected:
    // Types

    enum FilterItemType
    {
        FIT_Numeric,
        FIT_String
    };

    struct FilterItem
    {
        PSTR           key;
        FilterItemType  type;
        int             min,max; // min and max integer values 
        PSTR           strVal; // string value
    };

protected:
    // Member Functions
    void FreeFilterItem( FilterItem * item );

protected:
    // Data

    FilterType          m_FilterType;
    CList<FilterItem>   m_Filters;
};


class GameListFilter : public TableItemFilter
{
public:
    // Types

    enum StatusFlag
    {
        SF_LOCKED = 1,
        SF_PASSWORD_PROTECTED = 2,
        SF_SECURE = 4
    };


public:
    // Methods

    GameListFilter();
    virtual ~GameListFilter();

    virtual bool Filter( TableList::TableItem & tableItem );

    // filter attributes specific to the game list
    void SetPingFilter(uint32_t minPing, uint32_t maxPing);
    uint32_t GetMaxPing( void ) const;
    uint32_t GetMinPing( void ) const;
    void EnablePingFilter( bool val );
    bool GetPingFilterEnabled( void ) const;

    void EnableActiveFilter( bool val );
    bool GetActiveFilterEnabled( void ) const;

    void EnableAvailableFilter( bool val );
    bool GetAvailableFilterEnabled( void ) const;

    bool SetPlayerCountFilter(uint32_t minPlayers,
                              uint32_t maxPlayers ) ;
    uint32_t GetMinPlayers( void ) const;
    uint32_t GetMaxPlayers( void ) const;
    void EnablePlayerCountFilter( bool val );
    bool GetPlayerCountFilterEnabled( void ) const;

    bool SetGameTypeFilter( PCSTR  gameType );
    PCSTR  GetGameTypeFilter( void ) const;
    void EnableGameTypeFilter( bool val ) ;
    bool GetGameTypeFilterEnabled( void ) const;

    bool SetPlayerFilter( PCSTR  playerName );
    PCSTR  GetPlayerFilter( void ) const;
    void EnablePlayerFilter( bool val ) ;
    bool GetPlayerFilterEnabled( void ) const;

    bool SetClanFilter( PCSTR  clanName );
    PCSTR  GetClanFilter( void ) const;
    void EnableClanFilter( bool val ) ;
    bool GetClanFilterEnabled( void ) const;

private:
    // Data

    // pings
    bool    m_pingOn;
    uint32_t     m_minPing, m_maxPing;
    // is game available to be played?
    bool    m_availableOn;
    // does game have at least one player in it?
    bool    m_activeOn;
    // player count
    bool    m_playerCountOn;
    uint32_t     m_minPlayers, m_maxPlayers;
    // game type
    bool    m_gameTypeOn;
    PSTR   m_gameType;
    // player name
    bool    m_playerNameOn;
    PSTR   m_playerName;
    // clan name
    bool    m_clanNameOn;
    PSTR   m_clanName;

};




class IndexedTableView
{
public:
    // Types

    enum SortOrderType
    {
        ASCENDING_NUMERIC,
        DESCENDING_NUMERIC,
        ASCENDING_ALPHABETICAL,
        DESCENDING_ALPHABETICAL
    };

public:
    // Methods

    IndexedTableView( TableList * list );
    virtual ~IndexedTableView( );

    // callback methods
    void CreateEntryNotify( TableList::TableItem & item );
    void SetFieldNotify( TableList::TableItem & item, TableList::FieldItem & field );
    void RemoveFieldNotify( TableList::TableItem * item,
                            PCSTR  key );
    void RemoveNotify( TableList::TableItem & item );

    // policy settings
    // Not currently Supported 
    void SetDynamicSorting( bool val );
    bool GetDynamicSorting( void ) const;
    void SetDynamicFiltering( bool val );
    bool GetDynamicFiltering( void ) const;

    bool GetIndex( int index, PSTR * id );
    int  GetCount( void ) const;

    // filtering operations
    void SetFilter( TableItemFilter & filter );
    TableItemFilter * GetFilter( void ) const;
    void ClearFilter( void );

    bool  Filter( void );

    // sorting operations
    bool Sort( PCSTR  sortKey,
               SortOrderType sortOrder );
    bool SortIfDirty( void );
    void SetResortFrequency( double seconds );
    double GetResortFrequency( void ) const;
    bool GetSortDirty( void ) const;
    void SetSortDirty( bool val );


    // misc operations
    int GetIndexFromID( PCSTR  id );
    bool SetSelection( PCSTR  id );
    PCSTR  GetSelection( void ) const;

    // to override gos memory allocation
    void * operator new(size_t size);
    void * operator new[](size_t size);
    void operator delete( void* ptr );
    void operator delete[] ( void* ptr );

private:
    // Types
    struct SortStruct
    {
        TableList::TableItem * tableItem;
        TableList::FieldItem * sortField;
    };

    typedef SortStruct * SortVec;


protected:
    // Methods
    void Add( TableList::TableItem & item );
    void GrowSorted( void );
    void DeleteSorted( int index );
    int  GetSortedIndex( TableList::TableItem * item );
    void QSort( SortVec & sortVec, int first, int last );
    
    static bool HashForEachCB( TableList::TableItem * item,
                               MTListNodeHandle h, 
                               void * context );

private:
    // Data
    TableList * m_TableList;
    TableItemFilter * m_Filter;

    bool    m_DynamicSorting; // true if sorting is done dynamically
    bool    m_DynamicFiltering; // true if filtering is done dynamically

    // m_SortVec is for full list sorts for speed reasons.
    SortVec m_SortVec;  // for full sorts
    int     m_SortVecSize;
    int     m_SortVecCapacity;

    CList<SortStruct *> m_SortList; // for dynamic inserts

    // control vars for sorting
    PSTR  m_SortKey;
    SortOrderType m_SortOrder;
    // extra vars needed or hash foreach callback
    PCSTR  m_SearchKey;
    PCSTR  m_SearchVal;
    PCSTR  m_SearchResult;
    // mdm - used to remember which item is selected in the list
    PSTR  m_Selection;
    // mdm - true if the list has been changed but not resorted
    double          m_LastSortTime; // gos_GetHiResTime() from the last sort 
    double          m_ResortFrequency; // maximum frequency at which we will resort the list
    bool            m_bSortDirty; // true if the list has been marked as needing sorting


};

} // namespace Browse

#endif
