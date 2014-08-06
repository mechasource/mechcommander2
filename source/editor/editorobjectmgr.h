#ifndef EDITOROBJECTMGR_H
#define EDITOROBJECTMGR_H
/*************************************************************************************************\
EditorObjectMgr.h	: Interface for the EditorObjectMgr component.  This thing holds lists
						of all the objects on the map.  
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/



#ifndef ELIST_H
#include "Elist.h"
#endif

#ifndef EDITOROBJECTS_H
#include "EditorObjects.h"
#endif

#ifndef FOREST_H
#include "Forest.h"
#endif

#include "mclib.h"


class ObjectAppearance;
class AppearanceType;
class EditorObject;	
class Mech;
class BuildingLink;

class EditorObjectMgr
{
private:

	struct Building;
	
public:

	static EditorObjectMgr* instance(){ return s_instance; }

	enum BuildingType
	{
		UNSPECIAL = -1,
		NORMAL_BUILDING = 0,
		DROP_ZONE = 1,
		TURRET_CONTROL,
		GATE_CONTROL,
		POWER_STATION,
		TURRET_GENERATOR,
		SENSOR_CONTROL,
		EDITOR_GATE,
		EDITOR_TURRET,
		SENSOR_TOWER,
		EDITOR_BRIDGE,
		BRIDGE_CONTROL,
		SPOTLIGHT,
		SPOTLIGHT_CONTROL,
		DROPZONE,
		NAV_MARKER,
		WALL,
		LOOKOUT,
		RESOURCE_BUILDING,
		HELICOPTER
	};

	class EDITOR_OBJECT_LIST : public EList< EditorObject*, EditorObject*>
	{
	public:
		void AddUnique(EditorObject* item)
		{
			if ((0 < Count()) && (INVALID_ITERATOR != Find(item)))
			{
				return;
			}
			Append(item);
		}
		void RemoveIfThere(EditorObject* item)
		{
			if (0 < Count())
			{
				EIterator it = Find(item);
				if (INVALID_ITERATOR != it)
				{
					Delete(it);
				}
			}
		}
	};

	
	EditorObjectMgr();  // should only be one of these
	~EditorObjectMgr();

	void		render();
	
	void		update();

	void		renderShadows();

	void		init( const char* bldgListFileName, const char* objectFileName );

	EditorObject*	addBuilding( const Stuff::Vector3D& position, 
								unsigned long group, 
								unsigned long indexWithinGroup,
								int alignment,
								float rotation = 0.0,
								float height = 1.0,
								bool bSnapToCell = true ); // returns NULL if failure

	bool addBuilding( EditorObject* pObjectThatWeWillCopy );

	EditorObject*	addDropZone( const Stuff::Vector3D& position, 
								int alignment, bool bVTol ); // returns NULL if failure

	bool		deleteBuilding( const EditorObject* pInfo ); // should only be one building in a spot
	bool		deleteObject( const EditorObject* pInfo){ return deleteBuilding( pInfo ); } // only object FOR NOW

	/* This function (getObjectAtPosition(...)) is for use in determining objects selected by the
	mouse, not really for general use. It appears to return an object that a) is visible on screen,
	and b) has the property that the screen projection of the given position is contained in
	(intersect with) the screen projection of the object. */
	EditorObject* getObjectAtPosition( const Stuff::Vector3D& position );
	/*This function (getObjectAtCell(...)) returns an object that occupies the given cell (i.e. it
	doesn't have to be centered at the given cell).*/
	EditorObject* getObjectAtCell( long cellJ, long cellI );

	/*This function (getObjectAtLocation(...)) returns an object whose x and y positions are the
	same as (or very close to) those given. Screen visibility, object geometry, etc., are not
	considerred.*/
	EditorObject* getObjectAtLocation( float x, float y );

	EditorObject* getBuilding( const EditorObject &building );
	
	bool		canAddBuilding( const Stuff::Vector3D& position,
								float rotation,
								unsigned long group,
								unsigned long indexWithinGroup );

	bool		canAddDropZone( const Stuff::Vector3D& position, int alignment, bool bVTol );

	int			getBuildingGroupCount() const;		// mechs count too!
	int			getNumberBuildingsInGroup( int Group ) const;
	void		getBuildingGroupNames( const char** names, int& numberOfNames ) const;
	void		getNamesOfObjectsInGroup( const char* groupName, const char** names, int& numberOfNames ) const;
	void		getBuildingNamesInGroup( int Group, const char** names, int& numberOfNames ) const;
	int			getNumberOfVariants( int group, int indexInGroup ) const;
	void		getVariantNames( int group, int indexInGroup, const char** names, int& numberOfNames ) const;
	const char*	getGroupName( int group ) const;
	const char*	getObjectName( int ID ) const;

	int			getUnitGroupCount() const;
	void		getUnitGroupNames( const char** names, int* IDs, int& numberOfNames ) const;


	bool		save( PacketFile&  file, int whichPacket );
	bool		load( PacketFile&  file, int whichPacket );

	bool		saveMechs( FitIniFile& file );
	bool		loadMechs( FitIniFile& file ); // need to implement this

	bool		saveDropZones( FitIniFile& file );
	bool		loadDropZones( FitIniFile& file );

	bool		saveForests( FitIniFile& file );
	bool		loadForests( FitIniFile& file );

	int			getFitID( int id ) const;

	bool		moveBuilding( EditorObject* pInfo, long cellI, long cellJ );
	bool		getBlocksLineOfFire( int id ) const;
	bool		getIsHoverCraft( int id ) const;

	inline	int getID( long group, long index ) { return (getType(group,index) << 24) | (group << 16) | (index << 8); }

	void		clear(); // delete all objects

	__int64		getImpassability( int id );

	inline static unsigned long getGroup( long id );
	inline static unsigned long getIndexInGroup( long id );
	inline int					getAppearanceType( int ID );

	bool getBuildingFromID( int fitID, unsigned long& group, unsigned long& index, bool canBeMech );

	void unselectAll();
	void select( const Stuff::Vector4D& pos1, const Stuff::Vector4D& pos2 );
	void select( EditorObject &object, bool bSelect = true );
	void objectSelectionChanged(void) /* use this to notify the EditorObjectMgr when selection states of objects are changed externally */
	{ selectedObjectsNeedsToBeSynched = true; }
	bool hasSelection();
	int  getSelectionCount(); // returns the number of selected items
	EDITOR_OBJECT_LIST getSelectedObjectList();

	void deleteSelectedObjects();

	void adjustObjectsToNewTerrainHeights();

	ObjectAppearance* getAppearance( unsigned long group,
											 unsigned long indexWithinGroup );
	ObjectAppearance* getAppearance( Building* pBuilding );

	// LINKS!
	BuildingLink* getLinkWithParent( const EditorObject* pObj );
	BuildingLink* getLinkWithChild( const EditorObject* pObj );
	BuildingLink* getLinkWithBuilding( const EditorObject* pObj ); // parent or child
	void addLink( BuildingLink* );
	void deleteLink( BuildingLink* );

	inline BuildingType	getSpecialType( int ID );
	inline bool isCapturable( int ID );
	inline float getScale( int ID );
	inline bool isAlignable( int ID );
	inline int getObjectTypeNum( int ID );
	inline const char* getFileName( int ID ) const;
	inline const char* getTGAFileName( int ID ) const;
	inline DWORD getTacMapColor( int ID ) const;

	typedef EList< EditorObject*, EditorObject*>  BUILDING_LIST;	// buildings on the map
	typedef EList< Unit*, Unit* > UNIT_LIST;

	void getSelectedUnits( UNIT_LIST& );
	UNIT_LIST getUnits() { return units; }
	BUILDING_LIST getBuildings() { return buildings; }

	unsigned long getNextAvailableSquadNum() { return nextAvailableSquadNum; }
	void registerSquadNum(unsigned long squadNum);
	void unregisterSquadNum(unsigned long squadNum) {}
	void resetAvailableSquadNums() { nextAvailableSquadNum = 1; }

	int getNextAvailableForestID();
	long createForest( const Forest& settings );
	void editForest( long& ID,  const Forest& newSettings );
	void removeForest( const Forest& settings );
	const Forest* getForest( long ID );
	long getForests( Forest** pForests, long& count );
	void selectForest( long ID );

	private:
		
		struct Building
		{
			char				name[64];
			char**				varNames;
			int					nameID;
			char				fileName[64];
			AppearanceType*		appearanceType;
			int					type;
			unsigned long		fitID;
			__int64				impassability;
			bool				blocksLineOfFire;
			BuildingType		specialType;
			bool				capturable;
			bool				alignable;
			DWORD				writeOnTacMap;
			char				tgaName[64];
			long				objectTypeNum;
			char				forestId;
			float				scale;
			bool				isHoverCraft;
			~Building();
		};

		struct Group
		{
			char							name[64];
			EList< Building, Building& >	buildings;
		};

		typedef EList< EditorObject*, EditorObject*>  BUILDING_LIST;	// buildings on the map
		typedef EList< Unit*, Unit* > UNIT_LIST;
		typedef EList< Group, Group& >	GROUP_LIST;
		typedef EList< BuildingLink*, BuildingLink* > LINK_LIST;
		typedef EList< DropZone*, DropZone* > DROP_LIST;
		typedef EList< Forest*, Forest* > FOREST_LIST;
		
		GROUP_LIST						groups;							// list of possible buildings
		Group							dropZoneGroup;
		BUILDING_LIST					buildings;
		UNIT_LIST						units;
		LINK_LIST						links;
		DROP_LIST						dropZones;
		FOREST_LIST						forests;

		EDITOR_OBJECT_LIST selectedObjects;
		bool selectedObjectsNeedsToBeSynched;
		void syncSelectedObjectPointerList();	// makes sure that the "selectedObjects" list corresponds to the objects marked as selected

		// HELPERS
		int ExtractNextString( unsigned char*& pFileLine, char* pBuffer, int bufferLength );
		int ExtractNextInt( unsigned char*& pFileLine );
		float ExtractNextFloat( unsigned char*& pFileLine );

		int  getType( unsigned long group, unsigned long indexWithinGroup );
		void getRandomTreeFromGroup( int treeGroup, int& group, int& index );


		void doForest( const Forest& forest );


		unsigned long nextAvailableSquadNum;

		static EditorObjectMgr*	s_instance; // the one and only instance of this object
};

inline unsigned long EditorObjectMgr::getGroup( long id )
{
	return ((id >> 16) & 0x00ff);
}
inline unsigned long EditorObjectMgr::getIndexInGroup( long id )
{
	return ((id >> 8) & 0x00ff);
}
inline EditorObjectMgr::BuildingType	EditorObjectMgr::getSpecialType( int ID )
{
	return groups[getGroup( ID )].buildings[getIndexInGroup( ID )].specialType;
}

inline bool EditorObjectMgr::isAlignable( int ID )
{
	return groups[getGroup( ID )].buildings[getIndexInGroup( ID )].alignable;
}

inline int EditorObjectMgr::getAppearanceType( int ID )
{
	return groups[getGroup( ID )].buildings[getIndexInGroup( ID )].appearanceType->getAppearanceClass();
}

inline int EditorObjectMgr::getObjectTypeNum( int ID )
{
	return groups[getGroup( ID )].buildings[getIndexInGroup( ID )].objectTypeNum;
}

inline const char* EditorObjectMgr::getFileName( int ID ) const
{
	return groups[getGroup( ID )].buildings[getIndexInGroup( ID )].fileName;
}

inline const char* EditorObjectMgr::getTGAFileName( int ID ) const
{
	return groups[getGroup( ID )].buildings[getIndexInGroup( ID )].tgaName;
}

inline DWORD EditorObjectMgr::getTacMapColor( int ID ) const
{
	return groups[getGroup( ID )].buildings[getIndexInGroup( ID )].writeOnTacMap;
}

inline float EditorObjectMgr::getScale( int ID )
{
	return groups[getGroup( ID )].buildings[getIndexInGroup( ID )].scale;
}

//*************************************************************************************************
#endif  // end of file ( EditorObjectMgr.h )
