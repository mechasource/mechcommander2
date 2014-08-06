#define EDITOROBJECTS_CPP
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "EditorObjects.h"

#ifndef INIFILE_H
#include "IniFile.h"
#endif

#ifndef OBJECTAPPEARANCE_H
#include "Objectappearance.h"
#endif

#ifndef TERRAIN_H
#include "Terrain.h"
#endif

#ifndef EDITOROBJECTMGR_H
#include "EditorObjectMgr.h"
#endif

#ifndef BUILDINGLINK_H
#include "BuildingLink.h"
#endif

#ifndef EDITORDATA_H
#include "EditorData.h"
#endif

#include "resource.h"

// ARM
#include "../ARM/Microsoft.Xna.Arm.h"
using namespace Microsoft::Xna::Arm;
extern IProviderEngine* armProvider;

extern unsigned long gameResourceHandle;

Pilot::PilotInfo Pilot::s_BadPilots[MAX_PILOT] = { 0 };
Pilot::PilotInfo Pilot::s_GoodPilots[MAX_PILOT] = { 0 };
long	Pilot::goodCount = 0;
long	Pilot::badCount = 0;

//--------------------------------------------------------------------------------------
void *EditorObject::operator new (size_t mySize)
{
	void *result = NULL;
	result = systemHeap->Malloc(mySize);
	
	return(result);
}

//--------------------------------------------------------------------------------------
void EditorObject::operator delete (void *us)
{
	systemHeap->Free(us);
}

//--------------------------------------------------------------------------------------
EditorObject::EditorObject()
{
	appearInfo = new AppearanceInfo();
	appearInfo->appearance = 0;
	appearInfo->refCount = 1;
	cellColumn = cellRow = id = 0;
	forestId = -1;
	scale = 1.0;
}
EditorObject::EditorObject( const EditorObject& src )
{
	if ( &src != this )
	{
		appearInfo = src.appearInfo;
		appearInfo->refCount++;
		cellColumn = src.cellColumn;
		cellRow = src.cellRow;
		id = src.id;
		forestId = src.forestId;
		scale = src.scale;
	}

}

EditorObject& EditorObject::operator=( const EditorObject& src )
{
	/* do the assignment */
	if ( &src != this )
	{
		/* "destruct" the current object first */
		if (appearInfo)
		{
			gosASSERT(appearInfo->refCount != 0);

			appearInfo->refCount --;
			if ( appearInfo->refCount < 1 )
			{
				delete appearInfo->appearance;
				appearInfo->appearance = NULL;

				delete appearInfo;
				appearInfo = NULL;
			}
		}

		appearInfo = src.appearInfo;
		appearInfo->refCount++;
		cellColumn = src.cellColumn;
		cellRow = src.cellRow;
		id = src.id;
		forestId = src.forestId;
		scale = src.scale;
	}

	return *this;
}

EditorObject::~EditorObject()
{ 
	if (appearInfo)
	{
		gosASSERT(appearInfo->refCount != 0);

		appearInfo->refCount --;
		if ( appearInfo->refCount < 1 )
		{
			delete appearInfo->appearance;
			appearInfo->appearance = NULL;

			delete appearInfo;
			appearInfo = NULL;
		}
	}
}

void EditorObject::setDamage( bool bDamage )
{
	appearance()->setDamage( bDamage );
//	appearance()->setDamageLvl( 1000000 );
}

bool EditorObject::getDamage( ) const
{
	return appearance()->damage ? true : false;
}

const char* EditorObject::getDisplayName() const
{
	return EditorObjectMgr::instance()->getObjectName( id );
}

void EditorObject::setAlignment( int align )
{
	if ( appearance()->teamId != align )
	{
		if (align == 8)			//New magical force Neutral buttons
			align = -1;
		appearance()->teamId = align;
		BuildingLink* pLink = EditorObjectMgr::instance()->getLinkWithParent( this );

		if ( pLink )
		{
			pLink->SetParentAlignment( appearance()->teamId );
		}
	}
}

int EditorObject::getIndexInGroup() const
{
	return EditorObjectMgr::getIndexInGroup( id );
}

int EditorObject::getGroup() const
{
	return EditorObjectMgr::getGroup( id );
}

void EditorObject::setAppearance( int Group, int indexInGroup )
{
	// make sure the thing has changed....
	if ( Group != EditorObjectMgr::getGroup( id ) || indexInGroup != EditorObjectMgr::getIndexInGroup( id ) )
	{
		
		AppearanceInfo* appearInfo2 = new AppearanceInfo();
		appearInfo2->appearance = EditorObjectMgr::instance()->getAppearance( Group, indexInGroup );
		appearInfo2->refCount = 1;
		static long homeRelations[9] = {0, 0, 2, 1, 1, 1, 1, 1, 1};
		gosASSERT((8 > appearance()->teamId) && (-1 <= appearance()->teamId));
		appearInfo2->appearance->setObjectParameters( appearance()->position, appearance()->rotation, appearance()->selected, appearance()->teamId, homeRelations[appearance()->teamId+1]);
		appearInfo2->appearance->setDamage(appearance()->damage);
		id = EditorObjectMgr::instance()->getID( Group, indexInGroup );

		if (appearInfo)
		{
			gosASSERT(appearInfo->refCount != 0);

			appearInfo->refCount --;
			if ( appearInfo->refCount < 1 )
			{
				delete appearInfo->appearance;
				appearInfo->appearance = NULL;
			
				delete appearInfo;
				appearInfo = NULL;
			

			}
		}

		appearInfo = appearInfo2;
	}
}

void EditorObject::select( bool bSelect )
{ 
	//appearance()->selected = bSelect;
	EditorObjectMgr::instance()->select(*this, bSelect);
}

unsigned long EditorObject::getColor() const
{
	switch( appearInfo->appearance->teamId )
	{
	case EDITOR_TEAMNONE:
		return 0;
	case EDITOR_TEAM1:
		return SB_GREEN;
		break;
	case EDITOR_TEAM2:
		return SB_RED;
		break;
	case EDITOR_TEAM3:
		return SB_BLUE;
	case EDITOR_TEAM4:
		return SB_ORANGE;
	case EDITOR_TEAM5:
		return SB_WHITE;
	case EDITOR_TEAM6:
		return SB_GRAY;
	case EDITOR_TEAM7:
		return SB_BLACK;
	case EDITOR_TEAM8:
		return SB_YELLOW;
		break;
	
	default:
		gosASSERT( false );

	}

	return 0xffffffff;
}

void EditorObject::getCells( long& row, long& col ) const
{
	row = cellRow;
	col = cellColumn;
}

EditorObject::AppearanceInfo& EditorObject::AppearanceInfo::operator=( const AppearanceInfo& src )
{
	if ( &src != this )
	{
		// assuming ref count stays the same, since that refers to the graphic...
		appearance->rotation = src.appearance->rotation;
		appearance->position = src.appearance->position;
		appearance->barStatus = src.appearance->barStatus;
		appearance->damage = src.appearance->damage;
		appearance->fadeTable = src.appearance->fadeTable;
		appearance->paintScheme = src.appearance->paintScheme;
		appearance->selected = src.appearance->selected;
	}

	return *this;
}

int	EditorObject::getSpecialType() const
{
	return EditorObjectMgr::instance()->getSpecialType( getID() );
}

//*************************************************************************************************


Unit::Unit( int align )
{
	pAlternativeInstances = new CUnitList;
	unsigned long squadNum = EditorObjectMgr::instance()->getNextAvailableSquadNum();
	setSquad(squadNum);
	EditorObjectMgr::instance()->registerSquadNum(squadNum);
	setSelfRepairBehaviorEnabled(true);
	pilot.info = align == 0 ? &Pilot::s_GoodPilots[0] : &Pilot::s_BadPilots[0];
	baseColor = 0x00ffffff;
	highlightColor = 0x00c0c0c0;
	highlightColor2 = 0x00808080;
	variant = 0;
}

Unit::~Unit()
{
	delete pAlternativeInstances;
}

Unit::Unit( const Unit& src ) : EditorObject( src )
{
	
	if ( &src != this )
	{
		brain = src.brain;
		lance = src.lance;
		lanceIndex = src.lanceIndex;
		pilot = src.pilot;
		unsigned long color1, color2, color3;
		src.getColors(color1, color2, color3);
		setColors(color1, color2, color3);
		setSelfRepairBehaviorEnabled(src.getSelfRepairBehaviorEnabled());
		setVariant(src.getVariant());
		pAlternativeInstances = new CUnitList;
		(*pAlternativeInstances) = *(src.pAlternativeInstances);
		squad = src.squad;
	}

	variant = 0;
}

void Unit::CastAndCopy(const EditorObject &master)
{
	const Unit *pCastedMaster = dynamic_cast<const Unit *>(&master);
	if (0 != pCastedMaster)
	{
		(*this) = (*pCastedMaster);
	}
	else
	{
		gosASSERT(false);
		(*((EditorObject *)this)) = (master);
	}
}

bool Unit::save( FitIniFile* file, int WarriorNumber )
{
	
	bool bIsVehicle = dynamic_cast<GVAppearance*>(appearance()) ? true : false;
	
	if ( !bIsVehicle )
		return Unit::save( file, WarriorNumber, 1, appearance()->teamId == EDITOR_TEAM1 ?
		"PM207300" : "PM101100" );

	else
		return Unit::save( file, WarriorNumber, 2, 
		appearance()->teamId == EDITOR_TEAM1 ? "pv20600" : "pv20500" );
}

bool Unit::save( FitIniFile* file, int WarriorNumber, int controlDataType, char* objectProfile )
{
	// ARM
	if (mechAsset)
	{
		const char * iniFilename = (const char *)EditorObjectMgr::instance()->getFileName( id );
		char buf[512] = {0};

		if (iniFilename && iniFilename[0])
		{
			strcpy(buf, "Data\\TGL\\");
			strcat(buf, iniFilename);
			strcat(buf, ".ini");

			IProviderAssetPtr objAssetPtr = armProvider->OpenAsset(buf, 
								AssetType_Physical, ProviderType_Secondary);
						
			if (getDisplayName()[0])
			{
				objAssetPtr->AddProperty("DisplayName", getDisplayName());
			}

			objAssetPtr->AddProperty("ObjectType", "Mech");
			objAssetPtr->Close();
		}

		mechAsset->AddProperty("Type", "Warrior");

		strcpy(buf, "Data\\Missions\\Profiles\\");
		strcat(buf, objectProfile);
		strcat(buf, ".fit");
		mechAsset->AddRelationship("ObjectProfile", buf);

		strcpy(buf, "Data\\Objects\\");
		strcat(buf, iniFilename);
		strcat(buf, ".cvs");
		mechAsset->AddRelationship("CSVFile", buf);

		strcpy(buf, "Data\\TGL\\");
		strcat(buf, iniFilename);
		strcat(buf, ".ini");
		mechAsset->AddRelationship("AppearanceFile", buf);
	}
	
	pilot.save( file, appearance()->teamId == EDITOR_TEAM1 ? 1 : 0 );
	brain.save( file, WarriorNumber, appearance()->teamId == EDITOR_TEAM1 ? 1 : 0);

	char tmp[256];
	sprintf( tmp, "Part%ld", WarriorNumber );
	file->writeBlock( tmp );
	file->writeIdULong( "ObjectNumber", EditorObjectMgr::instance()->getFitID( id ) );
	file->writeIdULong( "ControlType", 2 );
	int playerNum = appearance()->teamId;
	file->writeIdBoolean( "PlayerPart", playerNum == EDITOR_TEAM1 ? true : false );
	file->writeIdChar( "MyIcon", 0 );
	int teamNum = EditorData::instance->PlayersRef().PlayerRef(playerNum).DefaultTeam();
	file->writeIdChar( "TeamID", teamNum );
	file->writeIdChar( "CommanderID",playerNum );
	file->writeIdULong( "Pilot", WarriorNumber );
	file->writeIdFloat( "PositionX", appearance()->position.x  );
	file->writeIdFloat( "PositionY", appearance()->position.y );
	file->writeIdFloat( "Rotation", appearance()->rotation );
	file->writeIdLong( "Active", 1 );
	file->writeIdLong( "Exists", 1 );
	file->writeIdFloat( "Damage", (float)((int)getDamage()) );
	file->writeIdULong( "BaseColor", baseColor );
	file->writeIdULong( "HighlightColor1", highlightColor );
	file->writeIdULong( "HighlightColor2", highlightColor2 );

	unsigned long tmpULong = 0;
	if (getSelfRepairBehaviorEnabled())
	{
		tmpULong = 1;
	}
	file->writeIdULong( "SelfRepairBehavior", tmpULong );

	file->writeIdULong( "ControlDataType", controlDataType );
	file->writeIdString( "ObjectProfile", objectProfile );
	file->writeIdString( "CSVFile",  EditorObjectMgr::instance()->getFileName( id ) );
	file->writeIdULong( "VariantNumber", variant );

	file->writeIdULong( "SquadNum", getSquad() );
	file->writeIdULong( "NumAlternatives", pAlternativeInstances->Count() );

	return true;
}

bool Unit::load( FitIniFile* file, int warriorNumber )
{
	long result = 0;
	char tmp;
	file->readIdChar( "CommanderID", tmp );
	appearance()->teamId = tmp;
	file->readIdFloat( "PositionX", appearance()->position.x  );
	file->readIdFloat( "PositionY", appearance()->position.y );
	file->readIdFloat( "Rotation", appearance()->rotation );

	land->worldToCell( appearance()->position, cellRow, cellColumn );

	float fDamage = 0.0;
	result = file->readIdFloat( "Damage", fDamage );
	if ((NO_ERR == result) && (0.0 != fDamage))
	{
		setDamage(true);
	}

	file->readIdULong( "BaseColor", baseColor );
	file->readIdULong( "HighlightColor1", highlightColor );
	file->readIdULong( "HighlightColor2", highlightColor2 );

	unsigned long tmpULong = 1;
	result = file->readIdULong( "SelfRepairBehavior", tmpULong );
	if ((NO_ERR == result) && (0 == tmpULong))
	{
		setSelfRepairBehaviorEnabled(false);
	}
	else
	{
		setSelfRepairBehaviorEnabled(true);
	}

	file->readIdULong( "VariantNumber", variant );

	unsigned long squadNum = 1;
	result = file->readIdULong("SquadNum", squadNum);
	if (NO_ERR != result) {
		// the unit should already have a valid default squad assigned at construction
	} else {
		setSquad(squadNum);
		EditorObjectMgr::instance()->registerSquadNum(squadNum);
	}

	tmpNumAlternativeInstances = 0;
	file->readIdULong( "NumAlternatives", tmpNumAlternativeInstances );
	tmpAlternativeStartIndex = 0;
	if (0 < tmpNumAlternativeInstances)
	{
		file->readIdULong( "AlternativeStartIndex", tmpAlternativeStartIndex );
	}

	char blockId[256];
	sprintf(blockId,"Warrior%d",warriorNumber);
	file->seekBlock(blockId);
	pilot.load( file, appearance()->teamId == EDITOR_TEAM1 ? 1 : 0 );

	brain.load( file, warriorNumber);
	
	setColors(baseColor,highlightColor,highlightColor2);
	
	return true;
}

void Unit::getColors( unsigned long& color1, unsigned long& color2, unsigned long& color3 ) const
{
	color1 = baseColor;
	color2 = highlightColor;
	color3 = highlightColor2;
}

void Unit::setColors( unsigned long color1, unsigned long color2, unsigned long color3 ) 
{
	baseColor = color1;
	highlightColor = color2;
	highlightColor2 = color3;
	
	appearance()->resetPaintScheme(color2,color3,color1);
}

Unit& Unit::operator=( const Unit& src )
{
	if ( &src != this )
	{
		brain = src.brain;
		lance = src.lance;
		lanceIndex = src.lanceIndex;
		pilot = src.pilot;
		unsigned long color1, color2, color3;
		src.getColors(color1, color2, color3);
		setColors(color1, color2, color3);
		setSelfRepairBehaviorEnabled(src.getSelfRepairBehaviorEnabled());
		setVariant(src.getVariant());
		(*pAlternativeInstances) = *(src.pAlternativeInstances);
		squad = src.squad;

		EditorObject::operator=( src );
	}

	return *this;
}

void Unit::setSquad(unsigned long newSquad) {
	squad = newSquad;
	CUnitList::EIterator iter = pAlternativeInstances->Begin();
	while (!iter.IsDone())
	{
		(*iter).setSquad(newSquad);
		iter++;
	}
}

//*************************************************************************************************

bool DropZone::save( FitIniFile* file, int number )
{
	Stuff::Vector3D pos = getPosition();
	
	file->writeIdLong( "NumSlots", 4 );
	if ( bVTol )
	{
		file->writeIdBoolean( "IsVTOL", 1 );
		file->writeIdFloat( "PositionX", pos.x );
		file->writeIdFloat( "PositionY", pos.y );
	}

	else
	{

		file->writeIdFloat( "PositionX", pos.x );
		file->writeIdFloat( "PositionY", pos.y );
	
		file->writeIdFloat( "OffsetX0", 0 );
		file->writeIdFloat( "OffsetY0", 0 );
		file->writeIdFloat( "Rotation0", 45);

		file->writeIdFloat( "OffsetX0", -71 );
		file->writeIdFloat( "OffsetY0", 71 );
		file->writeIdFloat( "Rotation0", 45);

		file->writeIdFloat( "OffsetX0", -71 );
		file->writeIdFloat( "OffsetY0", -71 );
		file->writeIdFloat( "Rotation0", 45);

		file->writeIdFloat( "OffsetX0", -142 );
		file->writeIdFloat( "OffsetY0", 0 );
		file->writeIdFloat( "Rotation0", 45);

	}

	return true;
}

DropZone::DropZone( const Stuff::Vector3D& pos, int alignment, bool bvtol )
{
	bVTol = bvtol;
}

void DropZone::CastAndCopy(const EditorObject &master)
{
	const DropZone *pCastedMaster = dynamic_cast<const DropZone *>(&master);
	if (0 != pCastedMaster)
	{
		(*this) = (*pCastedMaster);
	}
	else
	{
		gosASSERT(false);
		(*((EditorObject *)this)) = (master);
	}
}

//*************************************************************************************************
bool Brain::save( FitIniFile* file, int warriorNumber, bool bPlayer )
{
	if (brainName[0])
	{
		file->writeIdString( "Brain", brainName);
		file->writeIdLong( "NumCells", numCells );
		file->writeIdLong( "NumStaticVars", numStaticVars );
		
		char text[256];
		for (long i=0;i<numCells;i++)
		{
			sprintf( text, "Warrior%ldCell%d", warriorNumber, i );
			file->writeBlock(text);
			file->writeIdLong("Cell",  cellNum[i]);
			file->writeIdLong("MemType", cellType[i]);
			file->writeIdFloat("Value", cellData[i]);
		}

		// ARM
		if (mechAsset && brainName[0])
		{
			char buf[512] = {0};
			strcpy(buf, "Data\\Missions\\Warriors\\");
			strcat(buf, brainName);
			strcat(buf, ".abl");
			mechAsset->AddRelationship("Brain", buf);
		}
	}
	else
	{
		if (!bPlayer)
		{
			file->writeIdString( "Brain", "DredAttack01");
			file->writeIdLong( "NumCells", 3 );
			file->writeIdLong( "NumStaticVars", 0 );
			
			char text[256];
			for (long i=0;i<3;i++)
			{
				sprintf( text, "Warrior%ldCell%d", warriorNumber, i );
				file->writeBlock(text);
				
				switch (i)
				{
					case 0:
						file->writeIdLong("Cell",  2);
						file->writeIdLong("MemType", 1);
						file->writeIdFloat("Value", 150);
					break;
					
					case 1:
						file->writeIdLong("Cell",  9);
						file->writeIdLong("MemType", 0);
						file->writeIdFloat("Value", 0.0f);
					break;
					
					case 2:
						file->writeIdLong("Cell",  28);
						file->writeIdLong("MemType", 0);
						file->writeIdFloat("Value", 0.0f);
					break;
				}
			}
		}
		else
		{
			file->writeIdString( "Brain", "PBrain");
			file->writeIdLong( "NumCells", 0 );
			file->writeIdLong( "NumStaticVars", 0 );
		}
	}

	return true;
}

//*************************************************************************************************
bool Brain::load( FitIniFile* file, int warriorNumber )
{
	file->readIdLong( "NumCells", numCells );
	file->readIdLong( "NumStaticVars", numStaticVars );
	file->readIdString( "Brain", brainName, 255);

	// if not explicitly set, then redo
	if ( strcmp( brainName, "PBrain" ) == 0 )
		brainName[0] = 0;
	else if ( stricmp( brainName, "DredAttack01" ) == 0 )
		brainName[0] = 0;

	if (0 < numCells)
	{
		cellNum = (long *)malloc(sizeof(long) * numCells);
		cellType = (long *)malloc(sizeof(long) * numCells); 
		cellData = (float *)malloc(sizeof(float) * numCells);
	}
	else
	{
		cellNum = (long *)0;
		cellType = (long *)0; 
		cellData = (float *)0;
	}

	char text[256];
	for (long i=0;i<numCells;i++)
	{
		sprintf( text, "Warrior%ldCell%d", warriorNumber, i );
		
		file->seekBlock(text);
		file->readIdLong("Cell",  cellNum[i]);
		file->readIdLong("MemType", cellType[i]);
		file->readIdFloat("Value", cellData[i]);
	}
	
	return true;
}

Brain::Brain( const Brain& src )
{
	numCells = src.numCells;
	numStaticVars = src.numStaticVars;
	strcpy( brainName, src.brainName );

	cellNum = (long *)malloc(sizeof(long) * numCells);
	cellType = (long *)malloc(sizeof(long) * numCells); 
	cellData = (float *)malloc(sizeof(float) * numCells);


	for (long i=0;i<numCells;i++)
	{
		cellNum[i] = src.cellNum[i];
		cellType[i] = src.cellType[i];
		cellData[i] = src.cellData[i];
	}
	
}
Brain& Brain::operator=( const Brain& src )
{
	if ( &src != this )
	{
		numCells = src.numCells;
		numStaticVars = src.numStaticVars;
		strcpy( brainName, src.brainName );

		if (0 < numCells)
		{
			cellNum = (long *)malloc(sizeof(long) * numCells);
			cellType = (long *)malloc(sizeof(long) * numCells); 
			cellData = (float *)malloc(sizeof(float) * numCells);

			for (long i=0;i<numCells;i++)
			{
				cellNum[i] = src.cellNum[i];
				cellType[i] = src.cellType[i];
				cellData[i] = src.cellData[i];
			}
		}
		else
		{
			cellNum = (long *)0;
			cellType = (long *)0; 
			cellData = (float *)0;
		}
	}

	return *this;
	
}

void Pilot::save( FitIniFile* file, int bGoodGuy )
{
	file->writeIdString( "Profile", info->fileName );

	// ARM
	if (mechAsset && info->fileName[0])
	{
		char buf[512] = {0};
		strcpy(buf, "Data\\Missions\\Warriors\\");
		strcat(buf, info->fileName);
		strcat(buf, ".fit");
		mechAsset->AddRelationship("Profile", buf);
	}
}

void Pilot::load( FitIniFile* file, int bGoodGuy )
{
	long result = 0;
	char buffer[256];
	result = file->readIdString( "Profile", buffer, 256 );
	bool bFound = 0;
	if (NO_ERR == result)
	{
		for ( int i = 0; i < goodCount; i++ )
		{
			if ( stricmp( buffer, s_GoodPilots[i].fileName ) == 0 )
			{
				info = &s_GoodPilots[i];
				bFound = 1;
				break;
			}
		}

		if ( !bFound )
		{
			for ( int i = 0; i < badCount; i++ )
			{
				if ( stricmp( buffer, s_BadPilots[i].fileName ) == 0 )
				{
					info = &s_BadPilots[i];
					bFound = 1;
					break;
				}
			}
		}
	}

	if ( !bFound )
	{
		info = &s_GoodPilots[0];
	}

	if ( !info )
	{
		Assert( 0, 0, "reassigning invalid pilot" );
		info = bGoodGuy ? &s_GoodPilots[0] : &s_BadPilots[0];
	}
	
 	
}

void Pilot::initPilots()
{
	CSVFile file;

	char path[256];
	strcpy( path, objectPath );
	strcat( path, "pilots.csv" );

	if ( NO_ERR != file.open( path ) )
	{
		STOP(( "couldn't find pilots.csv file" ));
		return;
	}

	char pilotFileName[256];
	strcpy(pilotFileName, "");

	PilotInfo* infos = s_GoodPilots;
	long* counter = &goodCount;

	for ( int i = 0; i < 2; i++ )
	{
		while( true )
		{
			int bytesRead = file.readLine( (BYTE*)pilotFileName, 256 );

			if ( bytesRead < 2 )
				break;

			CString postFix;
			if (0 == i)
			{
				if ((strlen(pilotFileName) > strlen("pmw"))
					&& (0 == strnicmp(pilotFileName, "pmw", strlen("pmw"))))
				{
					/*Good pilots that start with "pmw" are single-player pilots.*/
					CString tmpStr;
					tmpStr.LoadString(IDS_PILOT_SINGLE_PLAYER_VERSION);
					postFix = tmpStr.GetBuffer(0);
				}
				else if ((strlen(pilotFileName) > strlen("pmp_"))
					&& (0 == strnicmp(pilotFileName, "pmp_", strlen("pmp_"))))
				{
					/*Good pilots that start with "pmp_" are multi-player pilots.*/
					CString tmpStr;
					tmpStr.LoadString(IDS_PILOT_MULTIPLAYER_VERSION);
					postFix = tmpStr.GetBuffer(0);
				}
			}

			FitIniFile pilotFile;
			FullPathFileName tmpPath;
			tmpPath.init(warriorPath,pilotFileName,".fit");

			if ( NO_ERR != pilotFile.open( tmpPath ) )
			{
				char errorString[256];
				sprintf( errorString, "Couldn't open file %s", tmpPath);
				Assert( 0, 0, errorString );
				return;
			}

			infos[*counter].fileName = new char[strlen( pilotFileName ) + 1];
			strcpy( infos[*counter].fileName, pilotFileName );

			// if we got this far we have a file, make a pilot
			int result = pilotFile.seekBlock( "General" );
			gosASSERT( result == 0 );

			long tmp;
			result = pilotFile.readIdLong( "descIndex", tmp );
			gosASSERT( result == NO_ERR );

			cLoadString( tmp, pilotFileName, 64);
			strcat(pilotFileName, "  ");
			strncat(pilotFileName, postFix.GetBuffer(0), 64);

			infos[*counter].name = new char[strlen( pilotFileName  ) +1];
			strcpy( infos[*counter].name, pilotFileName );
			
			(*counter)++;

			if ( goodCount > MAX_PILOT )
				return;
		}

		file.close();
		FullPathFileName path;
		path.init(objectPath,"BadPilots",".csv");

		if ( NO_ERR != file.open( path ) )
		{
			STOP(( "couldn't find BadPilots.csv file" ));
			return;
		}

		infos = s_BadPilots;
		counter = &badCount;

	}
}

void Pilot::setName( const char* newName )
{
	for ( int i = 0; i < goodCount; i++ )
	{
		if ( stricmp( newName, s_GoodPilots[i].name ) == 0 )
		{
			info = &s_GoodPilots[i];
			return;
		}
	}

	for ( i = 0; i < badCount; i++ )
	{
		if ( stricmp( newName, s_BadPilots[i].name ) == 0 )
		{
			info = &s_BadPilots[i];
			return;
		}
	}

	gosASSERT( 0 );

}

bool NavMarker::save( FitIniFile* file, int warriorNumber )
{ 
	char text[32];
	sprintf( text, "NavMarker%ld", warriorNumber );
	file->writeBlock( text );

	file->writeIdFloat( "xPos", appearance()->position.x );
	file->writeIdFloat( "yPos", appearance()->position.y );
	file->writeIdFloat( "radius", 256.f );
	return true; 
}

NavMarker::NavMarker()
{
}


//*************************************************************************************************
// end of file ( EditorObjects.cpp )
