#ifndef EDITOROBJECTS_H
#define EDITOROBJECTS_H
/*************************************************************************************************\
EditorObjects.h			: Interface for the EditorObjects, buildings, mechs etc.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

class FitIniFile;

#include "ObjectAppearance.h"

#ifndef HEAP_H
#include <heap.h>
#endif

#ifndef ELIST_H
#include "elist.h"
#endif

#include "DAprType.h"
#define MAX_PILOT	64
//*************************************************************************************************

class EditorObject
{
public:
	
	EditorObject();
	virtual ~EditorObject();
	EditorObject( const EditorObject& );
	EditorObject& operator=( const EditorObject& );
	virtual void CastAndCopy(const EditorObject &master) { (*this) = master; }
	virtual EditorObject *Clone() { return(new EditorObject(*this)); }

	PVOIDoperator new (size_t mySize);
	void operator delete (PVOID us);

	void				select( bool bSelect );
	inline bool			isSelected() const { return appearInfo->appearance->selected ? true : false; }

	void				setAlignment( int32_t align );
	inline int32_t			getAlignment( )const{ return appearInfo->appearance->teamId; } 
	
	uint32_t		getColor() const;
	int32_t				getID() const { return  id; }

	void				getCells( int32_t& cellJ, int32_t& cellI ) const;
	
	virtual bool save( FitIniFile* file, int32_t warriorNumber ){ return false; }
	virtual bool load( FitIniFile* file, int32_t warriorNumber ){ return false; }
	virtual int32_t			getType() const { return BLDG_TYPE; }

	int32_t					getSpecialType() const;
	int32_t					getGroup() const;
	int32_t					getIndexInGroup() const;

	void				setDamage( bool bDamage );
	bool				getDamage() const;

	PCSTR			getDisplayName() const;

	const Stuff::Vector3D&	getPosition() const { return appearance()->position; }
	
	void 				markTerrain (_ScenarioMapCellInfo* pInfo, int32_t type, int32_t counter)
	{
		appearInfo->appearance->markTerrain(pInfo,type, counter);
	}

	void setAppearance( int32_t Group, int32_t indexInGroup );

	ObjectAppearance* appearance() { return appearInfo->appearance; }
	const ObjectAppearance* appearance() const { return appearInfo->appearance; }

	int32_t				getForestID() const { return forestId; }
	void				setForestID( int32_t newID ) { forestId = newID; }

	void				setScale( int32_t newScale ) { scale = newScale; }
	int32_t				getScale( ) const { return scale; }

protected:

	struct AppearanceInfo
	{
		ObjectAppearance*   appearance;
		int32_t					refCount; // so we only delete once

		PVOIDoperator new (size_t mySize)
		{
			PVOID result = NULL;
			result = systemHeap->Malloc(mySize);
			
			return(result);
		
		}

		void operator delete (PVOID us)
		{
			systemHeap->Free(us);
		}

		AppearanceInfo& operator=( const AppearanceInfo& src );


	};

	int32_t	cellColumn;
	int32_t	cellRow;
	int32_t	id;
	AppearanceInfo* appearInfo;
	int32_t	forestId;
	int32_t	scale; // forest only

	friend class EditorObjectMgr; // the only thing that can move and change these things

};


// THIS CLASS CLEARLY NEEDS FLESHING OUT!
//*************************************************************************************************
class Pilot
{

public:

	Pilot(){info = 0;}

	static void initPilots();

	struct PilotInfo
	{
		PSTR	name;
		PSTR	fileName;
	};
	
	static	PilotInfo s_GoodPilots[MAX_PILOT];
	static	PilotInfo s_BadPilots[MAX_PILOT];
	static	int32_t goodCount;
	static	int32_t badCount;

	PCSTR 		getName() const { return info->name; }
	void	setName( PCSTR );

	void	save( FitIniFile* file, int32_t bGoodGuy );
	void	load( FitIniFile* file, int32_t bGoodGuy );

	PilotInfo* info;
	/*note: The value of info should always be NULL or a pointer to static data. So the default
	assignment/copy operator (shallow copy) is valid. */
	
};

//*************************************************************************************************

class Brain
{
	int32_t numCells;
	int32_t *cellNum;
	int32_t *cellType;
	float *cellData;
	
	int32_t numStaticVars;
	
	char brainName[256];
	
public:
	Brain (void)
	{
		numStaticVars = numCells = 0;
		cellNum = cellType = NULL;
		cellData = NULL;
		
		brainName[0] = 0;
	}

	~Brain (void)
	{
		free(cellNum);
		free(cellType);
		free(cellData);
		
		numStaticVars = numCells = 0;
		cellNum = cellType = NULL;
		cellData = NULL;
		
		brainName[0] = 0;
	}

	Brain( const Brain&  );
	Brain& operator=( const Brain& );
	
	bool save( FitIniFile* file, int32_t warriorNumber, bool usePBrain = false );
	bool load( FitIniFile* file, int32_t warriorNumber );
}; 

//*************************************************************************************************

class CUnitList;

class Unit : public EditorObject
{
public:

	Unit( int32_t alignment );
	Unit( const Unit& src );
	Unit& operator=( const Unit& src );
	virtual ~Unit();
	virtual void CastAndCopy(const EditorObject &master);
	virtual EditorObject *Clone() { return(new Unit(*this)); }

	void setLanceInfo( int32_t newLance, int32_t index ){ lance = newLance; lanceIndex = index; }
	void getLanceInfo( int32_t &newLance, int32_t &index ){ newLance = lance; index = lanceIndex; }
	virtual int32_t			getType() const { return GV_TYPE; }	

	uint32_t getSquad() const { return squad; }
	void setSquad(uint32_t newSquad);

	virtual bool save( FitIniFile* file, int32_t warriorNumber );
	virtual bool load( FitIniFile* file, int32_t warriorNumber );

	void getColors( uint32_t& base, uint32_t& color1, uint32_t& color2 ) const;
	void setColors( uint32_t base, uint32_t color1, uint32_t color2 );

	bool getSelfRepairBehaviorEnabled() const { return selfRepairBehaviorEnabled; }
	void setSelfRepairBehaviorEnabled(bool val) { selfRepairBehaviorEnabled = val; }

	inline Pilot*	getPilot() { return &pilot; }

	void setVariant( uint32_t newVar ){ variant = newVar; }
	inline int32_t getVariant() const { return variant; }

	CUnitList *pAlternativeInstances;
	uint32_t tmpNumAlternativeInstances;
	uint32_t tmpAlternativeStartIndex;

protected:

	bool save( FitIniFile* file, int32_t WarriorNumber, int32_t controlDataType, PSTR objectProfile );

	Brain brain;
	bool selfRepairBehaviorEnabled;
	int32_t lance;		// which lance
	int32_t lanceIndex; // number within lance 1 to 12
	uint32_t squad;
	Pilot	pilot;

	uint32_t baseColor;
	uint32_t highlightColor;
	uint32_t highlightColor2;

	uint32_t						variant;
};


class CUnitList : public EList<Unit, Unit&> {};

//*************************************************************************************************
class DropZone : public EditorObject
{
public:

	DropZone( const Stuff::Vector3D& position, int32_t alignment, bool bVTol );
	DropZone& operator=( const DropZone& src ) { bVTol = src.bVTol; EditorObject::operator=( src ); return *this; }
	virtual void CastAndCopy(const EditorObject &master);
	virtual EditorObject *Clone() { return(new DropZone(*this)); }
	virtual bool save( FitIniFile* file, int32_t number );
	
	bool isVTol() { return bVTol; }
	
private:

	bool bVTol;
};

class NavMarker : public EditorObject
{
	public:

		NavMarker();
		virtual EditorObject *Clone() { return(new NavMarker(*this)); }
		virtual bool save( FitIniFile* file, int32_t number );

};


//*************************************************************************************************


#endif  // end of file ( EditorObjects.h )
