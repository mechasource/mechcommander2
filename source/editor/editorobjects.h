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
#include "EList.h"
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

	void *operator new (size_t mySize);
	void operator delete (void *us);

	void				select( bool bSelect );
	inline bool			isSelected() const { return appearInfo->appearance->selected ? true : false; }

	void				setAlignment( int align );
	inline int			getAlignment( )const{ return appearInfo->appearance->teamId; } 
	
	unsigned long		getColor() const;
	long				getID() const { return  id; }

	void				getCells( long& cellJ, long& cellI ) const;
	
	virtual bool save( FitIniFile* file, int warriorNumber ){ return false; }
	virtual bool load( FitIniFile* file, int warriorNumber ){ return false; }
	virtual int			getType() const { return BLDG_TYPE; }

	int					getSpecialType() const;
	int					getGroup() const;
	int					getIndexInGroup() const;

	void				setDamage( bool bDamage );
	bool				getDamage() const;

	const char*			getDisplayName() const;

	const Stuff::Vector3D&	getPosition() const { return appearance()->position; }
	
	void 				markTerrain (_ScenarioMapCellInfo* pInfo, int type, int counter)
	{
		appearInfo->appearance->markTerrain(pInfo,type, counter);
	}

	void setAppearance( int Group, int indexInGroup );

	ObjectAppearance* appearance() { return appearInfo->appearance; }
	const ObjectAppearance* appearance() const { return appearInfo->appearance; }

	long				getForestID() const { return forestId; }
	void				setForestID( int newID ) { forestId = newID; }

	void				setScale( long newScale ) { scale = newScale; }
	long				getScale( ) const { return scale; }

protected:

	struct AppearanceInfo
	{
		ObjectAppearance*   appearance;
		int					refCount; // so we only delete once

		void *operator new (size_t mySize)
		{
			void *result = NULL;
			result = systemHeap->Malloc(mySize);
			
			return(result);
		
		}

		void operator delete (void *us)
		{
			systemHeap->Free(us);
		}

		AppearanceInfo& operator=( const AppearanceInfo& src );


	};

	long	cellColumn;
	long	cellRow;
	long	id;
	AppearanceInfo* appearInfo;
	long	forestId;
	long	scale; // forest only

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
		char*	name;
		char*	fileName;
	};
	
	static	PilotInfo s_GoodPilots[MAX_PILOT];
	static	PilotInfo s_BadPilots[MAX_PILOT];
	static	long goodCount;
	static	long badCount;

	const char* 		getName() const { return info->name; }
	void	setName( const char* );

	void	save( FitIniFile* file, int bGoodGuy );
	void	load( FitIniFile* file, int bGoodGuy );

	PilotInfo* info;
	/*note: The value of info should always be NULL or a pointer to static data. So the default
	assignment/copy operator (shallow copy) is valid. */
	
};

//*************************************************************************************************

class Brain
{
	long numCells;
	long *cellNum;
	long *cellType;
	float *cellData;
	
	long numStaticVars;
	
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
	
	bool save( FitIniFile* file, int warriorNumber, bool usePBrain = false );
	bool load( FitIniFile* file, int warriorNumber );
}; 

//*************************************************************************************************

class CUnitList;

class Unit : public EditorObject
{
public:

	Unit( int alignment );
	Unit( const Unit& src );
	Unit& operator=( const Unit& src );
	virtual ~Unit();
	virtual void CastAndCopy(const EditorObject &master);
	virtual EditorObject *Clone() { return(new Unit(*this)); }

	void setLanceInfo( int newLance, int index ){ lance = newLance; lanceIndex = index; }
	void getLanceInfo( int &newLance, int &index ){ newLance = lance; index = lanceIndex; }
	virtual int			getType() const { return GV_TYPE; }	

	unsigned long getSquad() const { return squad; }
	void setSquad(unsigned long newSquad);

	virtual bool save( FitIniFile* file, int warriorNumber );
	virtual bool load( FitIniFile* file, int warriorNumber );

	void getColors( unsigned long& base, unsigned long& color1, unsigned long& color2 ) const;
	void setColors( unsigned long base, unsigned long color1, unsigned long color2 );

	bool getSelfRepairBehaviorEnabled() const { return selfRepairBehaviorEnabled; }
	void setSelfRepairBehaviorEnabled(bool val) { selfRepairBehaviorEnabled = val; }

	inline Pilot*	getPilot() { return &pilot; }

	void setVariant( unsigned long newVar ){ variant = newVar; }
	inline int getVariant() const { return variant; }

	CUnitList *pAlternativeInstances;
	unsigned long tmpNumAlternativeInstances;
	unsigned long tmpAlternativeStartIndex;

protected:

	bool save( FitIniFile* file, int WarriorNumber, int controlDataType, char* objectProfile );

	Brain brain;
	bool selfRepairBehaviorEnabled;
	int lance;		// which lance
	int lanceIndex; // number within lance 1 to 12
	unsigned long squad;
	Pilot	pilot;

	unsigned long baseColor;
	unsigned long highlightColor;
	unsigned long highlightColor2;

	unsigned long						variant;
};


class CUnitList : public EList<Unit, Unit&> {};

//*************************************************************************************************
class DropZone : public EditorObject
{
public:

	DropZone( const Stuff::Vector3D& position, int alignment, bool bVTol );
	DropZone& operator=( const DropZone& src ) { bVTol = src.bVTol; EditorObject::operator=( src ); return *this; }
	virtual void CastAndCopy(const EditorObject &master);
	virtual EditorObject *Clone() { return(new DropZone(*this)); }
	virtual bool save( FitIniFile* file, int number );
	
	bool isVTol() { return bVTol; }
	
private:

	bool bVTol;
};

class NavMarker : public EditorObject
{
	public:

		NavMarker();
		virtual EditorObject *Clone() { return(new NavMarker(*this)); }
		virtual bool save( FitIniFile* file, int number );

};


//*************************************************************************************************


#endif  // end of file ( EditorObjects.h )
