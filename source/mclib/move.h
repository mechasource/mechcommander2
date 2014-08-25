//***************************************************************************
//
//	Move.h -- Defs for Movement/Pathfinding routines
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef MOVE_H
#define MOVE_H

//***************************************************************************

//--------------
// Include Files
#ifndef MCLIB_H
#include <mclib.h>
#endif

#ifndef DMOVEMGR_H
#include "dmovemgr.h"
#endif

#ifndef TERRAIN_H
#include <terrain.h>
#endif

#ifndef DOBJCLASS_H
#include "dobjclass.h"
#endif

#ifndef DOBJBLCK_H
#include "dobjblck.h"
#endif

#ifndef DGAMELOG_H
#include "dgamelog.h"
#endif

#include <gameos.hpp>

//***************************************************************************

#define	USE_FORESTS			0

#define	MAX_GLOBALMAP_DOORS		10000
#define	MAX_GLOBALMAP_AREAS		10000

#define	NUM_EXTRA_DOOR_LINKS	2

//#define	INNER_SPHERE	TEAM1
//#define	CLANS			TEAM2
//#define	ALLIED			TEAM3
//#define NEUTRAL			0xff		//NEUTRALS are NOT part of the eight Teams!
									//These are inanimate things like buildings, trees, rocks, etc.

#define	SIMPLE_PATHMAP	0
#define	SECTOR_PATHMAP	1

typedef enum {
	DOOR_OFFSET_START,
	DOOR_OFFSET_GOAL,
	NUM_DOOR_OFFSETS
} GlobalMapDoorOffset;

#define	SECTOR_DIM				30

#define	MOVEMAP_WIDTH			12			// Used outside of move file
#define	MOVEMAP_MAX_CELLOBJECTS	4

#define	TILE_PRESERVED_FLAG		0x8000

#define	NUM_DIRECTIONS			8
#define	HPRIME_NOT_CALCED		0xFFFFFFFF

#define	MOVEFLAG_OPEN			1
#define	MOVEFLAG_CLOSED			2
#define	MOVEFLAG_STEP			4
#define	MOVEFLAG_GOAL			8
#define	MOVEFLAG_MOVER_HERE		16
#define	MOVEFLAG_GOAL_BLOCKED	32
#define	MOVEFLAG_OFFMAP			64

#define	CELLSTATE_OPEN			0
#define	CELLSTATE_BLOCKED		1

#define	FOREST_INCREMENT		2

//----------------
// cell move costs
#define	COST_CLEAR				100
#define	COST_BLOCKED			10000

#define	OVERLAYINDEX_NULL		41
#define	IS_DIRTROAD_OVERLAY		0
#define	IS_PAVEDROAD_OVERLAY	1
#define IS_RAILROAD_OVERLAY		2

#define	SCENARIOMAP_MAX_ELEVATION			63
#define	SCENARIOMAP_MAX_TILETYPE			127
#define	SCENARIOMAP_MAX_OVERLAYTYPE			127
//#define	SCENARIOMAP_LAST_BASETYPE_INDEX		63

//TileType and OverlayType now in terrain.h!
typedef enum {
	OVERLAY_WEIGHT_VEHICLE,
	OVERLAY_WEIGHT_MECH,
	OVERLAY_WEIGHT_OTHER1,
	OVERLAY_WEIGHT_OTHER2,
	OVERLAY_WEIGHT_OTHER3,
	NUM_OVERLAY_WEIGHT_CLASSES
} OverlayWeightClass;

//------------------
// class Definitions
#define	MOVEPARAM_NONE						0
#define	MOVEPARAM_FACE_TARGET				1
#define	MOVEPARAM_DIR_OFFSET				2
#define	MOVEPARAM_BACK_UP					4
#define	MOVEPARAM_SOMEWHERE_ELSE			8
#define	MOVEPARAM_MOVING_MOVERS				16
#define	MOVEPARAM_MYSTERY_PARAM				32
#define	MOVEPARAM_STATIONARY_MOVERS			64
#define	MOVEPARAM_AVOID_PATHLOCKS			128
#define	MOVEPARAM_INIT						256
#define	MOVEPARAM_RECALC					512
#define	MOVEPARAM_STEP_TOWARD_TARGET		1024
#define	MOVEPARAM_PLAYER					2048
#define	MOVEPARAM_RADIO_RESULT				4096
#define	MOVEPARAM_ESCAPE_TILE				8192
#define MOVEPARAM_STEP_ADJACENT_TARGET		16384
#define	MOVEPARAM_SWEEP_MINES				32768
#define	MOVEPARAM_FOLLOW_ROADS				65536
#define	MOVEPARAM_WATER_SHALLOW				131072
#define	MOVEPARAM_WATER_DEEP				262144
#define	MOVEPARAM_RANDOM_OPTIMAL			524288
#define	MOVEPARAM_JUMP						1048576

#define	TACORDER_PARAM_NONE					0
#define	TACORDER_PARAM_RUN					(1<<0)
#define	TACORDER_PARAM_WAIT					(1<<1)
#define	TACORDER_PARAM_FACE_OBJECT			(1<<2)
#define	TACORDER_PARAM_LAY_MINES			(1<<3)
#define	TACORDER_PARAM_PURSUE				(1<<4)
#define	TACORDER_PARAM_OBLITERATE			(1<<5)
#define	TACORDER_PARAM_ESCAPE_TILE			(1<<6)
#define	TACORDER_PARAM_SCAN					(1<<7)
#define	TACORDER_PARAM_ATTACK_DFA			(1<<8)
#define	TACORDER_PARAM_ATTACK_RAMMING		(1<<9)
#define	TACORDER_PARAM_RANGE_RAMMING		(1<<10)
#define	TACORDER_PARAM_RANGE_LONGEST		(1<<11)
#define	TACORDER_PARAM_RANGE_OPTIMAL		(1<<12)
#define	TACORDER_PARAM_RANGE_SHORT			(1<<13)
#define	TACORDER_PARAM_RANGE_MEDIUM			(1<<14)
#define	TACORDER_PARAM_RANGE_LONG			(1<<15)
#define	TACORDER_PARAM_AIM_HEAD				(1<<16)
#define	TACORDER_PARAM_AIM_LEG				(1<<17)
#define	TACORDER_PARAM_AIM_ARM				(1<<18)
#define	TACORDER_PARAM_DONT_SET_ORDER		(1<<19)
#define	TACORDER_PARAM_JUMP					(1<<20)
#define	TACORDER_PARAM_DONT_KEEP_MOVING		(1<<21)
#define	TACORDER_PARAM_TACTIC_FLANK_RIGHT	(1<<22) //4194304
#define	TACORDER_PARAM_TACTIC_FLANK_LEFT	(1<<23) //8388608
#define	TACORDER_PARAM_TACTIC_FLANK_REAR	(1<<24) //16777216
#define	TACORDER_PARAM_TACTIC_STOP_AND_FIRE	(1<<25) //33554432
#define	TACORDER_PARAM_TACTIC_TURRET		(1<<26) //67108864
#define	TACORDER_PARAM_TACTIC_JOUST			(1<<27) //134217728

#define	TACORDER_ATTACK_MASK				0x00000300
#define	TACORDER_RANGE_MASK					0x0000FC00
#define	TACORDER_AIM_MASK					0x00070000
#define	TACORDER_TACTIC_MASK				0x0FC00000

#define	RELPOS_FLAG_ABS					1
#define	RELPOS_FLAG_PASSABLE_START		2
#define	RELPOS_FLAG_PASSABLE_GOAL		4

#define MAX_GLOBAL_PATH					50

#define	NUM_MOVE_LEVELS					2

#define	MAX_WALL_OBJECTS				2000
#define	MAX_GATES_OBJECT				1000
#define	MAX_CELL_COORDS					5000
#define	FILL_STACK_SIZE					810000


//***************************************************************************


#pragma pack(1)

//---------------------------------------------------------------------------

#define	CELL_INFO_FLAG_PASSABLE			1
#define	CELL_INFO_FLAG_LOS				2
#define	CELL_INFO_FLAG_WATER_DEEP		4
#define	CELL_INFO_FLAG_WATER_SHALLOW	8

#define	SPECIAL_NONE					0
#define	SPECIAL_WALL					1
#define	SPECIAL_GATE					2
#define	SPECIAL_LAND_BRIDGE				3
#define	SPECIAL_FOREST					4

#define	MINE_NONE						0
#define	MINE_INTACT						1
#define	MINE_EXPLODED					2

typedef struct _ScenarioMapCellInfo {
	uint8_t		terrain;
	uint8_t		overlay;
	bool				road;
	bool				gate;
	bool				forest;
	char				mine;
	uint8_t		specialType;
	int16_t				specialID;
	bool				passable;
	int32_t				lineOfSight;
} MissionMapCellInfo;

//---------------------------------------------------------------------------

#define	MAPCELL_TERRAIN_SHIFT			0
#define	MAPCELL_TERRAIN_MASK			0x0000000F

#define	MAPCELL_OVERLAY_SHIFT			4
#define	MAPCELL_OVERLAY_MASK			0x00000030

#define	MAPCELL_MOVER_SHIFT				6
#define	MAPCELL_MOVER_MASK				0x00000040

#define	MAPCELL_UNUSED1_SHIFT			7				// THIS BIT AVAILABLE!
#define	MAPCELL_UNUSED1_MASK			0x00000080

#define	MAPCELL_GATE_SHIFT				8
#define	MAPCELL_GATE_MASK				0x00000100

#define	MAPCELL_OFFMAP_SHIFT			9
#define	MAPCELL_OFFMAP_MASK				0x00000200

#define	MAPCELL_PASSABLE_SHIFT			10
#define	MAPCELL_PASSABLE_MASK			0x00000400

#define	MAPCELL_PATHLOCK_SHIFT			11
#define	MAPCELL_PATHLOCK_MASK			0x00001800
#define	MAPCELL_PATHLOCK_BASE			0x00000800

#define	MAPCELL_MINE_SHIFT				13
#define	MAPCELL_MINE_MASK				0x0001E000

#define	MAPCELL_PRESERVED_SHIFT			17
#define	MAPCELL_PRESERVED_MASK			0x00020000

#define	MAPCELL_HEIGHT_SHIFT			18
#define	MAPCELL_HEIGHT_MASK				0x003C0000

#define	MAPCELL_DEBUG_SHIFT				22
#define	MAPCELL_DEBUG_MASK				0x00C00000

#define	MAPCELL_WALL_SHIFT				24
#define	MAPCELL_WALL_MASK				0x01000000

#define	MAPCELL_ROAD_SHIFT				25
#define	MAPCELL_ROAD_MASK				0x02000000

#define	MAPCELL_SHALLOW_SHIFT			26
#define	MAPCELL_SHALLOW_MASK			0x04000000

#define	MAPCELL_DEEP_SHIFT				27
#define	MAPCELL_DEEP_MASK				0x08000000

#define	MAPCELL_FOREST_SHIFT			28
#define	MAPCELL_FOREST_MASK				0x10000000

//------------------------------------------------------
// The following are used ONLY when building map data...
#define	MAPCELL_BUILD_WALL_SHIFT		29
#define	MAPCELL_BUILD_WALL_MASK			0x20000000

#define	MAPCELL_BUILD_GATE_SHIFT		30
#define	MAPCELL_BUILD_GATE_MASK			0x40000000

#define	MAPCELL_BUILD_LAND_BRIDGE_SHIFT	11
#define	MAPCELL_BUILD_LAND_BRIDGE_MASK	0x00000800

#define	MAPCELL_BUILD_SPECIAL_MASK		0x60000800

#define	MAPCELL_BUILD_NOT_SET_SHIFT		31
#define	MAPCELL_BUILD_NOT_SET_MASK		0x80000000

typedef struct _MapCell {
	uint32_t		data;

	uint32_t getTerrain (void) {
		return((data & MAPCELL_TERRAIN_MASK) >> MAPCELL_TERRAIN_SHIFT);
	}

	void setTerrain (uint32_t terrain) {
		data &= (MAPCELL_TERRAIN_MASK ^ 0xFFFFFFFF);
		data |= (terrain << MAPCELL_TERRAIN_SHIFT);
	}

	uint32_t getOverlay (void) {
		return((data & MAPCELL_OVERLAY_MASK) >> MAPCELL_OVERLAY_SHIFT);
	}

	void setOverlay (uint32_t overlay) {
		data &= (MAPCELL_OVERLAY_MASK ^ 0xFFFFFFFF);
		data |= (overlay << MAPCELL_OVERLAY_SHIFT);
	}

	bool getMover (void) {
		return((data & MAPCELL_MOVER_MASK) ? true : false);
	}

	void setMover (bool moverHere ) {
		data &= (MAPCELL_MOVER_MASK ^ 0xFFFFFFFF);
		if (moverHere)
			data |= MAPCELL_MOVER_MASK;
	}

	uint32_t getGate (void) {
		return((data & MAPCELL_GATE_MASK) >> MAPCELL_GATE_SHIFT);
	}

	void setGate (uint32_t gate) {
		data &= (MAPCELL_GATE_MASK ^ 0xFFFFFFFF);
		data |= (gate << MAPCELL_GATE_SHIFT);
	}
	
	bool getPassable (void) {
		return((data & MAPCELL_PASSABLE_MASK) ? true : false);
	}

	void setPassable (bool passable) {
		data &= (MAPCELL_PASSABLE_MASK ^ 0xFFFFFFFF);
		if (passable)
			data |= MAPCELL_PASSABLE_MASK;
	}

	bool getPathlock (uint32_t level) {
		return((data & (MAPCELL_PATHLOCK_BASE << level)) ? true : false);
	}

	void setPathlock (uint32_t level, bool pathlock) {
		uint32_t bit = MAPCELL_PATHLOCK_BASE << level;
		data &= (bit ^ 0xFFFFFFFF);
		if (pathlock)
			data |= bit;
	}

	uint32_t getMine (void) {
		return((data & MAPCELL_MINE_MASK) >> MAPCELL_MINE_SHIFT);
	}

	void setMine (uint32_t mine) {
		data &= (MAPCELL_MINE_MASK ^ 0xFFFFFFFF);
		data |= (mine << MAPCELL_MINE_SHIFT);
	}

	bool getPreserved (void) {
		return((data & MAPCELL_PRESERVED_MASK) ? true : false);
	}

	void setPreserved (bool preserved) {
		data &= (MAPCELL_PRESERVED_MASK ^ 0xFFFFFFFF);
		if (preserved)
			data |= MAPCELL_PRESERVED_MASK;
	}

	void setLocalHeight (uint32_t localElevation) {
		data &= (MAPCELL_HEIGHT_MASK ^ 0xFFFFFFFF);
		data |= (localElevation << MAPCELL_HEIGHT_SHIFT);
	}
	
	uint32_t getLocalHeight (void) {
		return((data & MAPCELL_HEIGHT_MASK) >> MAPCELL_HEIGHT_SHIFT);
	}

	void setDebug (uint32_t value) {
		data &= (MAPCELL_DEBUG_MASK ^ 0xFFFFFFFF);
		data |= (value << MAPCELL_DEBUG_SHIFT);
	}

	uint32_t getDebug (void) {
		return((data & MAPCELL_DEBUG_MASK) >> MAPCELL_DEBUG_SHIFT);
	}

	bool getWall (void) {
		return((data & MAPCELL_WALL_MASK) ? true : false);
	}

	void setWall (bool wallHere) {
		data &= (MAPCELL_WALL_MASK ^ 0xFFFFFFFF);
		if (wallHere)
			data |= MAPCELL_WALL_MASK;
	}

	bool getRoad (void) {
		return((data & MAPCELL_ROAD_MASK) ? true : false);
	}

	void setRoad (bool roadHere) {
		data &= (MAPCELL_ROAD_MASK ^ 0xFFFFFFFF);
		if (roadHere)
			data |= MAPCELL_ROAD_MASK;
	}

	bool getShallowWater (void) {
		return((data & MAPCELL_SHALLOW_MASK) ? true : false);
	}

	void setShallowWater (bool shallowWaterHere) {
		data &= (MAPCELL_SHALLOW_MASK ^ 0xFFFFFFFF);
		if (shallowWaterHere)
			data |= MAPCELL_SHALLOW_MASK;
	}

	bool getDeepWater (void) {
		return((data & MAPCELL_DEEP_MASK) ? true : false);
	}

	void setDeepWater (bool deepWaterHere) {
		data &= (MAPCELL_DEEP_MASK ^ 0xFFFFFFFF);
		if (deepWaterHere)
			data |= MAPCELL_DEEP_MASK;
	}

	bool getBuildGate (void) {
		return((data & MAPCELL_BUILD_GATE_MASK) ? true : false);
	}

	void setBuildGate (bool set) {
		data &= (MAPCELL_BUILD_GATE_MASK ^ 0xFFFFFFFF);
		if (set)
			data |= MAPCELL_BUILD_GATE_MASK;
	}

	bool getBuildWall (void) {
		return((data & MAPCELL_BUILD_WALL_MASK) ? true : false);
	}

	void setBuildWall (bool set) {
		data &= (MAPCELL_BUILD_WALL_MASK ^ 0xFFFFFFFF);
		if (set)
			data |= MAPCELL_BUILD_WALL_MASK;
	}

	bool getBuildLandBridge (void) {
		return((data & MAPCELL_BUILD_LAND_BRIDGE_MASK) ? true : false);
	}

	void setBuildLandBridge (bool set) {
		data &= (MAPCELL_BUILD_LAND_BRIDGE_MASK ^ 0xFFFFFFFF);
		if (set)
			data |= MAPCELL_BUILD_LAND_BRIDGE_MASK;
	}

	bool getForest (void) {
		return((data & MAPCELL_FOREST_MASK) ? true : false);
	}

	void setForest (bool set) {
		data &= (MAPCELL_FOREST_MASK ^ 0xFFFFFFFF);
		if (set)
			data |= MAPCELL_FOREST_MASK;
	}

	bool getOffMap (void) {
		return((data & MAPCELL_OFFMAP_MASK) ? true : false);
	}

	void setOffMap (bool set) {
		data &= (MAPCELL_OFFMAP_MASK ^ 0xFFFFFFFF);
		if (set)
			data |= MAPCELL_OFFMAP_MASK;
	}

	bool getBuildSpecial (void) {
		return((data & MAPCELL_BUILD_SPECIAL_MASK) ? true : false);
	}

	bool getBuildNotSet (void) {
		return((data & MAPCELL_BUILD_NOT_SET_MASK) ? true : false);
	}

	void setBuildNotSet (bool set) {
		data &= (MAPCELL_BUILD_NOT_SET_MASK ^ 0xFFFFFFFF);
		if (set)
			data |= MAPCELL_BUILD_NOT_SET_MASK;
	}

} MapCell;

typedef MapCell* MapCellPtr;

typedef struct _PreservedCell {
	int16_t				row;
	int16_t				col;
	uint32_t		data;
} PreservedCell;

typedef PreservedCell* PreservedCellPtr;

#pragma pack()

//---------------------------------------------------------------------------

extern float VerticesMapSideDivTwo;
extern float MetersMapSideDivTwo;

#define	MAX_DEBUG_CELLS		1000

class MissionMap {

	public:

		MapCellPtr			map;
		int32_t				height;
		int32_t				width;
		int32_t				planet;
		bool				preserveCells;
		int32_t				numPreservedCells;
		PreservedCell		preservedCells[MAX_MOVERS];
		int32_t				numDebugCells;
		int32_t				debugCells[MAX_DEBUG_CELLS][3];

		void				(*placeMoversCallback) (void);
		
	public:

		PVOID operator new (size_t mySize);
		void operator delete (PVOID us);
		
		void init (void) {
			map = NULL;
			height = 0;
			width = 0;
			preserveCells = false;
			numPreservedCells = 0;
			placeMoversCallback = NULL;
			numDebugCells = 0;  
		}
		
		MissionMap (void) {
			init(void);
		}
		
		void destroy (void);

		~MissionMap (void) {
			destroy(void);
		}

		void init (int32_t h, int32_t w);

		int32_t init (int32_t curHeight, int32_t curWidth, int32_t curPlanet, MissionMapCellInfo* mapData);

		void setPassable (int32_t row, int32_t col, PSTR footPrint, bool passable);

		int32_t init (PacketFile* packetFile, int32_t whichPacket = 0);

		int32_t initOld (File* mapFile);

		int32_t write (PacketFile* packetFile, int32_t whichPacket = 0);

		uint32_t getTerrain (int32_t row, int32_t col) {
			return(map[row * width + col].getTerrain());
		}

		void setTerrain (int32_t row, int32_t col, uint32_t terrain) {
			map[row * width + col].setTerrain(terrain);
		}

		uint32_t getOverlay (int32_t row, int32_t col) {
			return(map[row * width + col].getOverlay());
		}

		void setOverlay (int32_t row, int32_t col, uint32_t overlay) {
			map[row * width + col].setOverlay(overlay);
		}

		bool getMover (int32_t row, int32_t col) {
			return(map[row * width + col].getMover());
		}

		void setMover (int32_t row, int32_t col, bool moverHere) {
			map[row * width + col].setMover(moverHere);
		}

		uint32_t getGate (int32_t row, int32_t col) {
			return(map[row * width + col].getGate());
		}

		void setGate (int32_t row, int32_t col, uint32_t gate) {
			map[row * width + col].setGate(gate);
		}

		bool getPassable (int32_t row, int32_t col) {
			return(map[row * width + col].getPassable());
		}

		void setPassable (int32_t row, int32_t col, bool passable) {
			map[row * width + col].setPassable(passable);
		}

		bool getPassable (Stuff::Vector3D cellPosition);

		bool getPathlock (int32_t level, int32_t row, int32_t col) {
			return(map[row * width + col].getPathlock(level));
		}

		void setPathlock (int32_t level, int32_t row, int32_t col, bool pathlock) {
			map[row * width + col].setPathlock(level, pathlock);
		}

		uint32_t getMine (int32_t row, int32_t col) {
			return(map[row * width + col].getMine());
		}

		void setMine (int32_t row, int32_t col, uint32_t mine) {
			map[row * width + col].setMine(mine);
		}

		bool getPreserved (int32_t row, int32_t col) {
			return(map[row * width + col].getPreserved());
		}

		void setPreserved (int32_t row, int32_t col, bool preserved) {
			map[row * width + col].setPreserved(preserved);
		}

		void setLocalHeight (int32_t row, int32_t col, uint32_t localElevation) {
			map[row * width + col].setLocalHeight(localElevation);
		}

		uint32_t getLocalHeight (int32_t row, int32_t col) {
			return(map[row * width + col].getLocalHeight());
		}

		void setCellDebug (int32_t row, int32_t col, uint32_t value, uint32_t level) {
			if( row >= 0 && row < height && col >= 0 && col < width )
			{
				uint32_t curValue = map[row * width + col].getDebug(void);
				if (curValue && (curValue != value)) {
					for (int32_t i = 0; i < numDebugCells; i++)
						if (debugCells[i][0] == row)
							if (debugCells[i][1] == col) {
								if (debugCells[i][2] < level)
									break;
								return;
							}
				}
				if (value == 0) {
					// remove it...
					for (int32_t i = 0; i < numDebugCells; i++)
						if (debugCells[i][0] == row)
							if (debugCells[i][1] == col) {
								numDebugCells--;
								debugCells[i][0] = debugCells[numDebugCells][0];
								debugCells[i][1] = debugCells[numDebugCells][1];
								debugCells[i][2] = debugCells[numDebugCells][2];
								break;
							}
					map[row * width + col].setDebug(0);
					}
				else {
					if (numDebugCells < MAX_DEBUG_CELLS) {
						debugCells[numDebugCells][0] = row;
						debugCells[numDebugCells][1] = col;
						debugCells[numDebugCells][2] = level;
						numDebugCells++;
						map[row * width + col].setDebug(value);
					}
				}
			}
		}

		void clearCellDebugs (void) {
			for (int32_t i = 0; i < numDebugCells; i++)
				map[debugCells[i][0] * width + debugCells[i][1]].setDebug(0);
			numDebugCells = 0;
		}

		void clearCellDebugs (int32_t level) {
			int32_t i = 0;
			while (i < numDebugCells) {
				if (debugCells[i][2] == level) {
					numDebugCells--;
					map[debugCells[i][0] * width + debugCells[i][1]].setDebug(0);
					debugCells[i][0] = debugCells[numDebugCells][0];
					debugCells[i][1] = debugCells[numDebugCells][1];
					debugCells[i][2] = debugCells[numDebugCells][2];
					}
				else
					i++;
			}
		}

		uint32_t getCellDebug (int32_t row, int32_t col) {
			return(map[row * width + col].getDebug());
		}

		bool getWall (int32_t row, int32_t col) {
			return(map[row * width + col].getWall());
		}

		void setWall (int32_t row, int32_t col, bool wallHere) {
			map[row * width + col].setWall(wallHere);
		}

		bool getRoad (int32_t row, int32_t col) {
			return(map[row * width + col].getRoad());
		}

		void setRoad (int32_t row, int32_t col, bool roadHere) {
			map[row * width + col].setRoad(roadHere);
		}

		bool getShallowWater (int32_t row, int32_t col) {
			return(map[row * width + col].getShallowWater());
		}

		void setShallowWater (int32_t row, int32_t col, bool shallowWaterHere) {
			map[row * width + col].setShallowWater(shallowWaterHere);
		}

		bool getDeepWater (int32_t row, int32_t col) {
			return(map[row * width + col].getDeepWater());
		}

		void setDeepWater (int32_t row, int32_t col, bool deepWaterHere) {
			map[row * width + col].setDeepWater(deepWaterHere);
		}

		bool getBuildGate (int32_t row, int32_t col) {
			return(map[row * width + col].getBuildGate());
		}

		void setBuildGate (int32_t row, int32_t col, bool set) {
			map[row * width + col].setBuildGate(set);
		}
		
		bool getBuildWall (int32_t row, int32_t col) {
			return(map[row * width + col].getBuildWall());
		}

		void setBuildWall (int32_t row, int32_t col, bool set) {
			map[row * width + col].setBuildWall(set);
		}

		bool getBuildLandBridge (int32_t row, int32_t col) {
			return(map[row * width + col].getBuildLandBridge());
		}

		void setBuildLandBridge (int32_t row, int32_t col, bool set) {
			map[row * width + col].setBuildLandBridge(set);
		}
		
		bool getForest (int32_t row, int32_t col) {
			return(map[row * width + col].getForest());
		}

		void setForest (int32_t row, int32_t col, bool set) {
			map[row * width + col].setForest(set);
		}

		bool getOffMap (int32_t row, int32_t col) {
			return(map[row * width + col].getOffMap());
		}

		void setOffMap (int32_t row, int32_t col, bool set) {
			map[row * width + col].setOffMap(set);
		}

		bool getBuildSpecial (int32_t row, int32_t col) {
			return(map[row * width + col].getBuildSpecial());
		}

		bool getBuildNotSet (int32_t row, int32_t col) {
			return(map[row * width + col].getBuildNotSet());
		}

		void setBuildNotSet (int32_t row, int32_t col, bool set) {
			map[row * width + col].setBuildNotSet(set);
		}

		int32_t getHeight (void) {
			return(height);
		}
		
		int32_t getWidth (void) {
			return(width);
		}

		bool inBounds (int32_t row, int32_t col) {
			return((row > -1) && (row < height) && (col > -1) && (col < width));
		}

		MapCellPtr getCell (int32_t row, int32_t col) {
			gosASSERT(inBounds(row, col));
			return(&map[row * width + col]);
		}

		void preserveCell (int32_t row, int32_t col) {
			preservedCells[numPreservedCells].data = map[row * width + col].data;
			preservedCells[numPreservedCells].row = row;
			preservedCells[numPreservedCells].col = col;
			map[row * width + col].setPreserved(1);
			numPreservedCells++;
		}

		int32_t getLOF (Stuff::Vector3D position);

		uint32_t cellPassable (Stuff::Vector3D cellPosition);

		void lineOfSensor (Stuff::Vector3D start, Stuff::Vector3D target, int32_t& numBlockingTiles, int32_t& numBlockingObjects);

		int32_t getOverlayWeight (int32_t row, int32_t col, int32_t moverOverlayWeightClass, int32_t moverRelation);

		void spreadState (int32_t r, int32_t c, int32_t radius);

		int32_t placeObject (Stuff::Vector3D position, float radius);

		void placeTerrainObject (int32_t row,
								 int32_t col,
								 int32_t objectClass,
								 int32_t objectTypeID,
								 int64_t footPrint,
								 bool blocksLineOfFire,
								 int32_t mineType);

		void print (PSTR fileName);
};

typedef MissionMap* MissionMapPtr;

//***************************************************************************


class MovePath;
typedef MovePath* MovePathPtr;


//---------------------------------------------------------------------------
#define	MAX_STEPS_PER_MOVEPATH 200

typedef struct _PathStep {
	int16_t					cell[2];							// obvious
	float					distanceToGoal;						// dist, in meters, to goal from this step
	Stuff::Vector3D			destination;						// world pos of this step
	char					direction;							// 0 thru 7 direction into this step
	int16_t					area;
} PathStep;

typedef PathStep* PathStepPtr;

class MovePath {

	public:
	
		Stuff::Vector3D		goal;								// world pos of path goal
		Stuff::Vector3D		target;								// world pos of object target
		int32_t				numSteps;							// if paused or no steps, == 0, else == numStepsWhenNotPaused
		int32_t				numStepsWhenNotPaused;				// total number of steps
		int32_t				curStep;							// cuurent step we're headed for
		int32_t				cost;								// total cost of path
		PathStep			stepList[MAX_STEPS_PER_MOVEPATH];	// actual steps :)
		bool				marked;								// is it currently marked
		int32_t				globalStep;							// if part of a complex path
		
	public:
	
		PVOID operator new (size_t mySize);
		void operator delete (PVOID us);
		
		void init (void) {
			goal.Zero(void);
			numSteps = 0;
			numStepsWhenNotPaused = 0;
			curStep = 0;
			cost = 0;
			marked = false;
			globalStep = -1;
		}
		
		int32_t init (int32_t numberOfSteps);

		void clear (void);

		void setDirection (int32_t stepNumber, char direction) {
			stepList[stepNumber].direction = direction;
		}

		int32_t getDirection (int32_t stepNumber) {
			return(stepList[stepNumber].direction);
		}

		void setDistanceToGoal (int32_t stepNumber, float distance) {
			stepList[stepNumber].distanceToGoal = distance;
		}

		float getDistanceToGoal (int32_t stepNumber) {
			return(stepList[stepNumber].distanceToGoal);
		}

		void setDestination (int32_t stepNumber, Stuff::Vector3D	v) {
			stepList[stepNumber].destination = v;
		}

		Stuff::Vector3D getDestination (int32_t stepNumber) {
			return(stepList[stepNumber].destination);
		}

		void setCell (int32_t stepNumber, int32_t r, int32_t c) {
			stepList[stepNumber].cell[0] = r;
			stepList[stepNumber].cell[1] = c;
		}

		float getDistanceLeft (Stuff::Vector3D position, int32_t stepNumber = -1);

		void lock (int32_t level, int32_t start, int32_t range, bool setting);

		bool isLocked (int32_t level, int32_t start, int32_t range, bool* reachedEnd = NULL);

		bool isBlocked (int32_t start, int32_t range, bool* reachedEnd = NULL);

		int32_t crossesBridge (int32_t start, int32_t range);

		int32_t crossesCell (int32_t start, int32_t range, int32_t cellR, int32_t cellC);

		int32_t crossesClosedClanGate (int32_t start, int32_t range);

		int32_t crossesClosedISGate (int32_t start, int32_t range);

		int32_t crossesClosedGate (int32_t start, int32_t range);

		MovePath (void) {
			init(void);
		}
		
		void destroy (void);

		~MovePath (void) {
			destroy(void);
		}

		void setCurStep (int32_t _curStep) {
			curStep = _curStep;
		}

		int32_t getNumSteps (void);

		int32_t getCost (void) {
			return(cost);
		}
};

//******************************************************************************************


//******************************************************************************************
#define	GLOBAL_FLAG_SPECIAL_IMPOSSIBLE	0x80
#define	GLOBAL_FLAG_SPECIAL_CALC		0x40
#define	GLOBAL_FLAG_NORMAL_OPENS		0x20
#define	GLOBAL_FLAG_NORMAL_CLOSES		0x10

#define	GLOBAL_CONFIDENCE_BAD			0
#define GLOBAL_CONFIDENCE_AT_LEAST		1
#define	GLOBAL_CONFIDENCE_GOOD			2

#define	GLOBALPATH_EXISTS_UNKNOWN		0
#define	GLOBALPATH_EXISTS_TRUE			1
#define	GLOBALPATH_EXISTS_FALSE			2

#pragma pack(1)

typedef struct _DoorLink {
	int16_t					doorIndex;
	char					doorSide;
	int32_t					cost;
	int32_t					openCost;
} DoorLink;

typedef DoorLink* DoorLinkPtr;

typedef struct _GlobalMapDoor {
	//----------------
	// Map layout data
	int16_t					row;
	int16_t					col;
	char					length;					// in cells
	bool					open;
	char					teamID;
	int16_t					area[2];
	int16_t					areaCost[2];
	char					direction[2];
	int16_t					numLinks[2];
	DoorLinkPtr				links[2];
	//------------------
	// Pathfinding  data
	int32_t					cost;
	int32_t					parent;
	int32_t					fromAreaIndex;
	uint32_t			flags;
	int32_t					g;
	int32_t					hPrime;
	int32_t					fPrime;
} GlobalMapDoor;

typedef GlobalMapDoor* GlobalMapDoorPtr;

typedef struct _DoorInfo {
	int16_t					doorIndex;
	char					doorSide;
} DoorInfo;

typedef DoorInfo* DoorInfoPtr;

typedef enum _AreaType {
	AREA_TYPE_NORMAL,
	AREA_TYPE_WALL,
	AREA_TYPE_GATE,
	AREA_TYPE_LAND_BRIDGE,
	AREA_TYPE_FOREST,
	NUM_AREA_TYPES
} AreaType;

typedef enum _WaterType {
	WATER_TYPE_NONE,
	WATER_TYPE_SHALLOW,
	WATER_TYPE_DEEP,
	NUM_WATER_TYPES
} WaterType;

typedef struct _GlobalMapArea {
	//----------------
	// Map layout data
	int16_t					sectorR;
	int16_t					sectorC;
	DoorInfoPtr				doors;
	AreaType				type;
	int16_t					numDoors;
	int32_t					ownerWID;
	char					teamID;
	bool					offMap;
	bool					open;
	pint16_t					cellsCovered;
} GlobalMapArea;

#pragma pack()

typedef GlobalMapArea* GlobalMapAreaPtr;

typedef struct _GlobalPathStep {
	int32_t					startDoor;
	int32_t					thruArea;
	int32_t					goalDoor;
	Stuff::Vector3D			start;			// "start" in this area
	Stuff::Vector3D			goal;			// "goal" in this area
	int32_t					goalCell[2];	// which cell did we actually exit thru
	int32_t					costToGoal;
} GlobalPathStep;

typedef GlobalPathStep* GlobalPathStepPtr;

#define	MAX_SPECIAL_AREAS		1500
#define	MAX_SPECIAL_SUB_AREAS	25
#define	MAX_CELLS_PER_SUB_AREA	49
#define	MAX_OFFMAP_AREAS		60
#define	MAX_GAME_OBJECT_CELLS	64

typedef struct _GameObjectFootPrint {
	int16_t				cellPositionRow;
	int16_t				cellPositionCol;
	int16_t				preNumCells;
	int16_t				numCells;
	int16_t				cells[MAX_GAME_OBJECT_CELLS][2];
} GameObjectFootPrint;

typedef struct _GlobalSpecialAreaInfo {
	uint8_t			type;
	int16_t					numSubAreas;
	int16_t					subAreas[MAX_SPECIAL_SUB_AREAS];
	int16_t					numCells;
	int16_t					cells[MAX_CELLS_PER_SUB_AREA][2];
} GlobalSpecialAreaInfo;

class GlobalMap {

	public:

		int32_t						height;				// in cells
		int32_t						width;				// in cells
		int32_t						sectorDim;			// in cells
		int32_t						sectorHeight;		// in sectors
		int32_t						sectorWidth;		// in sectors
		int32_t						numAreas;
		int32_t						numDoors;
		int32_t						numDoorInfos;
		int32_t						numDoorLinks;

		pint16_t						areaMap;
		GlobalMapAreaPtr			areas;
		GlobalMapDoorPtr			doors;
		DoorInfoPtr					doorInfos;
		DoorLinkPtr					doorLinks;
		GlobalMapDoorPtr			doorBuildList;
#ifdef USE_PATH_COST_TABLE
		puint8_t				pathCostTable;
#endif
		puint8_t				pathExistsTable;

		int32_t						numSpecialAreas;
		GlobalSpecialAreaInfo*		specialAreas;	// used when building data

		bool						closes;
		bool						opens;

		int32_t						goalArea;
		int32_t						goalSector[2];

		int32_t						startCell[2];
		int32_t						goalCell[2];

		bool						blank;
		bool						hover;
		bool						useClosedAreas;
		char						moverTeamID;
		bool						badLoad;
		bool						calcedPathCost;

		int32_t						numOffMapAreas;
		int16_t						offMapAreas[MAX_OFFMAP_AREAS];

		bool (*isGateDisabledCallback) (int32_t objectWID);
		bool (*isGateOpenCallback) (int32_t objectWID);

		static int32_t					minRow;
		static int32_t					maxRow;
		static int32_t					minCol;
		static int32_t					maxCol;

		static GameLogPtr			log;
		static bool					logEnabled;

	public:

		PVOID operator new (size_t mySize);
		void operator delete (PVOID us);
		
		void init (void) {
			height = 0;
			width = 0;
			sectorDim = 30;
			sectorHeight = 0;
			sectorWidth = 0;

			numAreas = 0;
			areaMap = NULL;
			areas = NULL;

			numDoors = 0;
			numDoorInfos = 0;
			numDoorLinks = 0;
			doors = NULL;
			doorInfos = NULL;
			doorLinks = NULL;
			doorBuildList = NULL;

			goalSector[0] = goalSector[1] = 0;
			blank = false;
			hover = false;
			useClosedAreas = false;
			badLoad = false;
			calcedPathCost = false;

			startCell[0] = -1;
			startCell[1] = -1;
			goalCell[0] = -1;
			goalCell[1] = -1;

			specialAreas = NULL;
			closes = false;
			opens = false;
			numOffMapAreas = 0;

			log = NULL;
			logEnabled = false;

			isGateDisabledCallback = NULL;
			isGateOpenCallback = NULL;
		}

		GlobalMap (void) {
			init(void);
		}
		
		void destroy (void);

		~GlobalMap (void) {
			destroy(void);
		}

		void init (int32_t h, int32_t w);

		int32_t build (MissionMapCellInfo* mapData);

		int32_t init (PacketFilePtr mapFile, int32_t whichPacket = 0);

		int32_t write (PacketFile* packetFile, int32_t whichPacket = 0);

		int32_t setTempArea (int32_t row, int32_t col, int32_t cost);

		bool fillNorthSouthBridgeArea (int32_t row, int32_t col, int32_t area);

		bool fillEastWestBridgeArea (int32_t row, int32_t col, int32_t area);

		bool fillNorthSouthRailroadBridgeArea (int32_t row, int32_t col, int32_t area);

		bool fillEastWestRailroadBridgeArea (int32_t row, int32_t col, int32_t area);

		bool fillSpecialArea (int32_t row, int32_t col, int32_t area, int32_t wallGateID);

		bool fillArea (int32_t row, int32_t col, int32_t area, bool offMap);

		void calcSectorAreas (int32_t sectorR, int32_t sectorC);

		void beginDoorProcessing (void);

		int32_t numAreaDoors (int32_t area);

		void getAreaDoors (int32_t area, DoorInfoPtr dorrList);

		void addDoor (int32_t adjArea, int32_t curArea, int32_t row, int32_t col, int32_t length, int32_t dir);

		void endDoorProcessing (void);

		void calcAreas (void);

		void calcSpecialAreas (MissionMapCellInfo* mapData);

		void calcCellsCovered (void);

		void calcSpecialTypes (void);

		void calcGlobalDoors (void);

		void calcAreaDoors (void);

		int32_t calcLinkCost (int32_t startDoor, int32_t thruArea, int32_t goalDoor);

		void changeAreaLinkCost (int32_t area, int32_t cost);

		void calcDoorLinks (void);

		int32_t getPathCost (int32_t startArea, int32_t goalArea, bool withSpecialAreas, int32_t& confidence, bool calcIt);

#ifdef USE_PATH_COST_TABLE
		void initPathCostTable (void);

		void resetPathCostTable (void);

		void calcPathCostTable (void);

		void setPathCost (int32_t startArea, int32_t goalArea, bool withSpecialAreas, uint8_t cost);

		void setPathFlag (int32_t startArea, int32_t goalArea, uint8_t flag, bool set);

		int32_t getPathFlag (int32_t startArea, int32_t goalArea, uint8_t flag);
#endif
		void clearPathExistsTable (void);

		void setPathExists (int32_t fromArea, int32_t toArea, uint8_t set);

		uint8_t getPathExists (int32_t fromArea, int32_t toArea);

		int32_t exitDirection (int32_t doorIndex, int32_t fromArea);

		void setStartDoor (int32_t startArea);

		void resetStartDoor (int32_t startArea);

		void setAreaTeamID (int32_t area, char teamID);

		void setAreaOwnerWID (int32_t area, int32_t objWID);

		void setGoalDoor (int32_t goalArea);

		void resetGoalDoor (int32_t goalArea);

		int32_t calcHPrime (int32_t area);

		//void propogateCost (int32_t area, int32_t g);
		void propogateCost (int32_t door, int32_t cost, int32_t fromAreaIndex, int32_t g);

		int32_t calcPath (int32_t startArea,
					   int32_t goalArea,
					   GlobalPathStepPtr path,
					   int32_t startRow = -1,
					   int32_t startCol = -1,
					   int32_t goalRow = -1,
					   int32_t goalCol = -1);

		int32_t calcPath (Stuff::Vector3D start, Stuff::Vector3D goal, GlobalPathStepPtr path);

		int32_t calcArea (int32_t row, int32_t col) {
			int32_t areaId = areaMap[row * width + col];
			if (areaId < 0)
				return(-1);
			return(areaId);
		}

		void getDoorTiles (int32_t area, int32_t door, GlobalMapDoorPtr areaDoor);

		void getSectorCoords (int32_t area, int32_t& sectorR, int32_t& sectorC) {
			sectorR = areas[area].sectorR;
			sectorC = areas[area].sectorC;
		}

		Stuff::Vector3D getDoorWorldPos (int32_t area, int32_t door, int32_t* goalCell);

		void openDoor (int32_t door);

		void closeDoor (int32_t door);

		void closeArea (int32_t area);

		void closeArea (int32_t row, int32_t col) {
			int32_t area = calcArea(row, col);
			gosASSERT(area > -1);
			closeArea(area);
		}

		void openArea (int32_t area);

		void openArea (int32_t row, int32_t col) {
			int32_t area = calcArea(row, col);
			gosASSERT(area > -1);
			openArea(area);
		}

		void openOffMapAreas (void) {
			for (int32_t i = 0; i < numOffMapAreas; i++)
				openArea(offMapAreas[i]);
		}

		void closeOffMapAreas (void) {
			for (int32_t i = 0; i < numOffMapAreas; i++)
				closeArea(offMapAreas[i]);
		}

		bool getDoorOpen (int32_t door) {
			return(doors[door].open);
		}

		bool isClosedArea (int32_t area) {
			return(!areas[area].open);
		}

		bool getAdjacentAreaCell (int32_t area, int32_t adjacentArea, int32_t& cellRow, int32_t& cellCol);

		void print (PSTR fileName);

		static bool toggleLog (void);

		static void writeLog (PSTR s);
};

typedef GlobalMap* GlobalMapPtr;

//******************************************************************************************

#define	NUM_ADJ_CELLS	8

typedef struct _MoveMapNode {
	int16_t				adjCells[NUM_ADJ_CELLS];
	int32_t				cost;								// normal cost to travel here, based upon terrain
	int32_t				parent;								// where we came from (parent cell)
	uint32_t		flags;								// CLOSED, OPEN, STEP flags
	int32_t				g;									// known cost from START to this node
	int32_t				hPrime;								// estimated cost from this node to GOAL
	int32_t				fPrime;								// = g + hPrime

	void setFlag (uint32_t flag) {
		flags |= flag;
	}

	void clearFlag (uint32_t flag) {
		flags &= (flag ^ 0xFFFFFFFF);
	}
} MoveMapNode;

typedef MoveMapNode* MoveMapNodePtr;

#define	DISTANCE_TABLE_DIM 80

class MoveMap {

	public:

		int32_t				ULr;		// upper-left cell row
		int32_t				ULc;		// upper-left cell col
		int32_t				width;
		int32_t				height;
		int32_t				minRow;
		int32_t				maxRow;
		int32_t				minCol;
		int32_t				maxCol;
		int32_t				maxWidth;
		int32_t				maxHeight;
		MoveMapNodePtr		map;
		int32_t*				mapRowStartTable;
		int32_t*				mapRowTable;
		int32_t*				mapColTable;
		int32_t				moveLevel;
		Stuff::Vector3D		start;
		int32_t				startR;
		int32_t				startC;
		Stuff::Vector3D		goal;			// actual world-coord goal
		int32_t				goalR;			// cell goal row relative to move map
		int32_t				goalC;			// cell goal col relative to move map
		int32_t				thruAreas[2];
		int32_t				door;
		int32_t				doorSide;
		int32_t				doorDirection;	// if goal is not a door, set to -1
		Stuff::Vector3D		target;			// actual world-coord target
		int32_t				clearCost;		// cost, in tenths of secs, to move to clear cell
		int32_t				jumpCost;		// cost, in tenths of secs, to jump to cell
		int32_t				numOffsets;		// set by calcMovePath function (don't touch:)
		float				calcTime;
		int32_t*				overlayWeightTable;
		int32_t				moverWID;
		int32_t				moverTeamID;
		bool				moverLayingMines;
		bool				moverWithdrawing;
		bool				travelOffMap;
		bool				cannotEnterOffMap;

		void				(*blockedDoorCallback) (int32_t moveLevel, int32_t door, PSTR openCells);
		void				(*placeStationaryMoversCallback) (MoveMapPtr map);

		static float		distanceFloat[DISTANCE_TABLE_DIM][DISTANCE_TABLE_DIM];
		static int32_t			distanceLong[DISTANCE_TABLE_DIM][DISTANCE_TABLE_DIM];
		static int32_t			forestCost;

	protected:

		bool adjacentCellOpen (int32_t mapCellIndex, int32_t dir);
		bool adjacentCellOpenJUMP (int32_t r, int32_t c, int32_t dir);
		void propogateCost (int32_t mapCellIndex, int32_t cost, int32_t g);
		void propogateCostJUMP (int32_t r, int32_t c, int32_t cost, int32_t g);
		int32_t calcHPrime (int32_t r, int32_t c);
		
	public:

		PVOID operator new (size_t mySize);
		void operator delete (PVOID us);
		
		void init (void) {
			ULr = 0;
			ULc = 0;
			maxHeight = 0;
			maxWidth = 0;
			height = 0;
			width = 0;
			minRow = 0;
			maxRow = 0;
			minCol = 0;
			maxCol = 0;
			map = NULL;
			mapRowTable = NULL;
			moveLevel = 0;
			startR = -1;
			startC = -1;
			goalR = -1;
			goalC = -1;
			thruAreas[0] = -1;
			thruAreas[1] = -1;
			goal.Zero(void);
			door = -1;
			doorSide = -1;
			doorDirection = -1;
			target.x = -999999.0;
			target.y = -999999.0;
			target.z = -999999.0;
			clearCost = 1;
			jumpCost = 0;
			numOffsets = 8;
			calcTime = 0.0;
			travelOffMap = false;
			cannotEnterOffMap = true;
			overlayWeightTable = NULL;
			blockedDoorCallback = NULL;
			placeStationaryMoversCallback = NULL;
		}
		
		MoveMap (void) {
			init(void);
		}
		
		void destroy (void);

		~MoveMap (void) {
			destroy(void);
		}

		void init (int32_t h, int32_t w);

		int32_t init (FitIniFile* mapFile);

		int32_t setUp (int32_t ULr,
					int32_t ULc,
					int32_t height,
					int32_t width,
					int32_t moveLevel,
					Stuff::Vector3D* startPos,
					int32_t startRow,
					int32_t startCol,
					Stuff::Vector3D goalPos,
					int32_t goalRow,
					int32_t goalCol,
					int32_t clearCellCost,
					int32_t jumpCellCost,
					int32_t offsets,
					uint32_t params = MOVEPARAM_NONE);

		int32_t setUp (int32_t moveLevel,
					Stuff::Vector3D* startPos,
					int32_t startRow,
					int32_t startCol,
					int32_t thruArea[2],
					int32_t goalDoor,
					Stuff::Vector3D finalGoal,
					int32_t clearCellCost,
					int32_t jumpCellCost,
					int32_t offsets,
					uint32_t params = MOVEPARAM_NONE);

		void clear (void);

		void placeMovers (bool stationaryOnly);

		void setOverlayWeightTable (int32_t* table) {
			overlayWeightTable = table;
		}

		void setStart (Stuff::Vector3D* startPos, int32_t startCellRow, int32_t startCellCol);

		void setGoal (Stuff::Vector3D goalPos, int32_t goalCellRow, int32_t goalCellCol);

		void setGoal (int32_t thruArea, int32_t goalDoor);

		int32_t markGoals (Stuff::Vector3D finalGoal);

		int32_t markEscapeGoals (Stuff::Vector3D finalGoal);

		void setTarget (Stuff::Vector3D targetPos);

		char getCost (int32_t row, int32_t col) {
			return(map[row * width + col].cost);
		}

		void setCost (int32_t row, int32_t col, int32_t newCost);

		void adjustCost (int32_t row, int32_t col, int32_t costAdj) {
			int32_t index = row * width + col;
			int32_t cost = map[index].cost + costAdj;
			if (cost < 1)
				cost = 1;
			map[index].cost = cost;
		}

		void setStart (int32_t row, int32_t col) {
			startR = row;
			startC = col;
		}

		void setClearCost (int32_t cost) {
			clearCost = cost;
		}

		void setJumpCost (int32_t cost, int32_t offsets = 8) {
			jumpCost = cost;
			numOffsets = offsets;
		}

		void setMover (int32_t watchID, int32_t teamID = 0, bool layingMines = false, bool withdrawing = false) {
			moverWID = watchID;
			moverTeamID = teamID;
			moverLayingMines = layingMines;
			moverWithdrawing = withdrawing;
		}

		int32_t calcPath (MovePathPtr path, Stuff::Vector3D* goalWorldPos, int32_t* goalCell);

		int32_t calcPathJUMP (MovePathPtr path, Stuff::Vector3D* goalWorldPos, int32_t* goalCell);

		int32_t calcEscapePath (MovePathPtr path, Stuff::Vector3D* goalWorldPos, int32_t* goalCell);

		float getDistanceFloat (int32_t rowDelta, int32_t colDelta) {
			return(distanceFloat[rowDelta][colDelta]);
		}

		float getDistanceLong (int32_t rowDelta, int32_t colDelta) {
			return(distanceLong[rowDelta][colDelta]);
		}

		void writeDebug (File* debugFile);

		bool inBounds (int32_t row, int32_t col) {
			return((row >= minRow) && (row <= maxRow) && (col >= minCol) && (col <= maxCol));
		}
};

//---------------------------------------------------------------------------

inline void MoveMap::setCost (int32_t row, int32_t col, int32_t newCost) {

	map[row * maxWidth + col].cost = newCost;
}

//---------------------------------------------------------------------------

void SaveMapCells (PSTR fileName,  int32_t height, int32_t width,  MissionMapCellInfo* mapData);

MissionMapCellInfo* LoadMapCells (PSTR fileName, int32_t& height, int32_t& width);

void DeleteMapCells (MissionMapCellInfo* mapData);

void MOVE_init (int32_t moveRange);
void MOVE_buildData (int32_t height, int32_t width, MissionMapCellInfo* mapData, int32_t numSpecialAreas, GameObjectFootPrint* specialAreaFootPrints);
int32_t MOVE_saveData (PacketFile* packetFile, int32_t whichPacket = 0);
int32_t MOVE_readData (PacketFile* packetFile, int32_t whichPacket);
void MOVE_cleanup (void);

//int32_t BuildAndSaveMoveData (PSTR fileName, int32_t height, int32_t width, MissionMapCellInfo* mapData);

//***************************************************************************

extern MissionMapPtr		GameMap;
extern GlobalMapPtr			GlobalMoveMap[3];
extern MoveMapPtr			PathFindMap[2];
extern int32_t					SimpleMovePathRange;

//***************************************************************************

#endif

