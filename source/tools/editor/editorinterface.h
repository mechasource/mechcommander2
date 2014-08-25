//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef EDITORINTERFACE_H
#define EDITORINTERFACE_H

//#include <mclib.h>
//#include "action.h"
//#include "editorobjectmgr.h"
//#include "editordata.h"
//#include "editortacmap.h"
//#include "objective.h"

// forward declarations
class DlgFileOpen;
class Brush;
class MainMenu;
class Menu;

// global resource handle
extern uint32_t gameResourceHandle;
extern bool DebuggerActive;

//--------------------------------------------------------------------------------------
//
// Mech Commander 2 -- Copyright (c) 1998 FASA Interactive
//
// the EditorInterface class handles all the messages for the editor
//
// the Editor class merely holds everything needed to make this go.
//
// 
//--------------------------------------------------------------------------------------

// this class handles and routes all messages...
class EditorInterface : public CWnd
{
private:
	
	

public:

	static EditorInterface* instance(){ return s_instance; }
	
	EditorInterface(void);
	~EditorInterface(void);

	void handleNewMenuMessage( int32_t specificMessage );

	void init( PCSTR fileName );

	void terminate(void);

	void ChangeCursor( int32_t ID );

	int32_t MissionSettings(void);

	int32_t Team( int32_t team );
	int32_t Player( int32_t player );
	/* When in "ObjectSelectOnlyMode", the interface is put in selection mode and all
	features are disabled except those pertaining to selection of objects. This mode is
	engaged from the objectives dialog. */
	bool ObjectSelectOnlyMode() { return bObjectSelectOnlyMode; }
	void ObjectSelectOnlyMode(bool val) { bObjectSelectOnlyMode = val; }
	CObjectivesEditState objectivesEditState;	/* persistent storage for the objective(s) dialog */
	void SelectionMode() { Select(void); }
	
	int32_t RefractalizeTerrain( int32_t threshold );

	virtual void handleLeftButtonDown( int32_t PosX, int32_t PosY ); // mouse button down
	virtual void handleLeftButtonDbl( int32_t PosX, int32_t PosY ){} // mouse button dbl click
	virtual void handleLeftButtonUp( int32_t PosX, int32_t PosY ); // pop ups etc need this
	virtual void handleKeyDown( int32_t Key );
	virtual void handleMouseMove( int32_t PosX, int32_t PosY );

	void update (void);

	virtual void render(void);
	void initTacMap(void);
	void updateTacMap() { tacMap.UpdateMap(void); }

	void syncHScroll(void);
	void syncVScroll(void);
	void syncScrollBars() { syncHScroll(void); syncVScroll(void); }

	int32_t Width(){  RECT tmp; GetWindowRect( &tmp ); return tmp.right - tmp.left; }
	int32_t Height() { RECT tmp; GetWindowRect( &tmp ); return tmp.bottom - tmp.top; }

	void SetBusyMode(bool bRedrawWindow = true);
	void UnsetBusyMode(void);

	bool SafeRunGameOSLogic(void);

	bool ThisIsInitialized() { return this->bThisIsInitialized; }

	afx_msg void UpdateButton( CCmdUI* button );

	int32_t Quit(void);
	int32_t Save(void);
	int32_t SaveAs(void);
	int32_t QuickSave(void);
	int32_t PromptAndSaveIfNecessary(void);

	ActionUndoMgr				undoMgr;

		//{{AFX_VIRTUAL(EditorInterface)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(uint32_t message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	protected:
		//{{AFX_MSG(EditorInterface)
		afx_msg int32_t OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(uint32_t nFlags, CPoint point);
	afx_msg void OnLButtonUp(uint32_t nFlags, CPoint point);
	afx_msg void OnMouseMove(uint32_t nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, uint32_t nHitTest, uint32_t message);
	afx_msg void OnKeyUp(uint32_t nChar, uint32_t nRepCnt, uint32_t nFlags);
	afx_msg void OnRButtonDown(uint32_t nFlags, CPoint point);
	afx_msg void OnRButtonUp(uint32_t nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(uint32_t nFlags, int16_t zDelta, CPoint pt);
	afx_msg void OnHScroll(uint32_t nSBCode, uint32_t nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(uint32_t nSBCode, uint32_t nPos, CScrollBar* pScrollBar);
	afx_msg void OnSysKeyDown(uint32_t nChar, uint32_t nRepCnt, uint32_t nFlags);
	afx_msg void OnLButtonDblClk(uint32_t nFlags, CPoint point);
	afx_msg void OnPaint(void);
	afx_msg void OnViewRefreshtacmap(void);
	afx_msg void OnUpdateMissionPlayerPlayer3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMissionPlayerPlayer4(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMissionPlayerPlayer5(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMissionPlayerPlayer6(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMissionPlayerPlayer7(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMissionPlayerPlayer8(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMissionTeamTeam3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMissionTeamTeam4(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMissionTeamTeam5(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMissionTeamTeam6(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMissionTeamTeam7(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMissionTeamTeam8(CCmdUI* pCmdUI);
	afx_msg void OnDestroy(void);
	afx_msg void OnForestTool(void);
	afx_msg void OnOtherEditforests(void);
	afx_msg void OnViewOrthographiccamera(void);
	afx_msg void OnViewShowpassabilitymap(void);
	afx_msg void OnMButtonUp(uint32_t nFlags, CPoint point);
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	afx_msg void OnCommand(WPARAM wParam);
	

private:

	// Message handlers
	int32_t Undo(void);
	int32_t Redo(void);	
	int32_t FileOpen(void);
	int32_t New(void);
	int32_t PaintDirtRoad(void);
	int32_t PaintRocks(void);
	int32_t PaintPaved(void);
	int32_t PaintTwoLaneDirtRoad(void);
	int32_t PaintDamagedRoad(void);
	int32_t PaintRunway(void);
	int32_t PaintBridge(void);
	int32_t PaintDamagedBridge(void);
	int32_t Erase(void);
	int32_t Select(void);
	int32_t Flatten(void);
	int32_t Fog(void);
	int32_t PurgeTransitions(void);
	int32_t ShowTransitions(void);
	int32_t Light(void);
	int32_t AssignElevation(void);

	int32_t paintBuildings( int32_t message );
	int32_t PaintTerrain( int32_t type );
	int32_t PaintOverlay( int32_t type, int32_t message );

	int32_t NewHeightMap(void);
	int32_t SaveCameras(void);
	int32_t SelectSlopes(void);
	int32_t SelectAltitude(void);
	int32_t SelectTerrainType(void);
	int32_t Waves(void);
	int32_t SaveHeightMap(void);

	int32_t DragSmooth(void);
	int32_t DragRough(void);

	int32_t SmoothRadius(void);
	int32_t Alignment( int32_t specific );
	int32_t Damage( bool bDamage );
	int32_t Link( bool bLink );
	int32_t LayMines(void);
	int32_t SelectDetailTexture(void);
	int32_t SelectWaterTexture(void);
	int32_t SelectWaterDetailTexture(void);
	int32_t TextureTilingFactors(void);
	int32_t ReloadBaseTexture(void);
	int32_t SetBaseTexture(void);
	int32_t DropZone( bool bVTol );
	int32_t UnitSettings( );
	
	int32_t SetSky (int32_t skyId);
	
	int32_t CampaignEditor(void);

	// helpers
	void KillCurBrush(void);

	void addBuildingsToNewMenu(void);

	void rotateSelectedObjects( int32_t direction );

	//-------------------------------------------
	// Data to control scroll, rotation and zoom
	float						baseFrameLength;
	
	float						zoomInc;
	float						rotationInc;
	float						scrollInc;
	
	float						screenScrollLeft;
	float						screenScrollRight;
	float						screenScrollUp;
	float						screenScrollDown;
	
	float						realRotation;
	float						degPerSecRot;

	Brush*						curBrush;
	Brush*						prevBrush;
	bool						prevSelecting;
	bool						prevPainting;
	bool						prevDragging;
	int32_t							oldCursor;
	bool						painting;
	bool						selecting;
	bool						dragging;
	bool						highlighted;

	int32_t							currentBrushID;
	int32_t							currentBrushMenuID;

	MainMenu					*m_pMainMenu;

	int32_t							smoothRadius;
	bool						bSmooth;

	static EditorInterface*		s_instance;
	Stuff::Vector3D				lastClickPos;

	HCURSOR						hCursor;
	int32_t							curCursorID;
	int32_t							lastX;
	int32_t							lastY;
	int32_t						lastKey;
	bool 						bObjectSelectOnlyMode;
	CMenu**						menus;

	EditorTacMap				tacMap;
	HACCEL						m_hAccelTable;

	bool						rightDrag;
	uint32_t				lastRightClickTime;

	HBITMAP m_hSplashBitMap;
	HCURSOR m_hBusyCursor;
	int32_t m_AppIsBusy;
	bool m_bInRunGameOSLogicCall;
	bool bThisIsInitialized;
};



class Editor // simply holds everything else
{
public:

	EditorObjectMgr					objectMgr;
	EditorData						data;

	~Editor(){ destroy(void); }

	void destroy (void);

	void init( PSTR loader );
	
	void render(void);
	
	void update(void);

	void resaveAll(void);		//Used by autoBuild to automagically resave all maps with correct data.	
};


class TeamsAction : public Action
{
public:
	TeamsAction() : Action() {}
	TeamsAction(const CTeams &teams) : Action() { PreviousTeams(teams); }
	virtual ~TeamsAction() {}
	virtual bool redo() { return undo(void); }
	virtual bool undo(void);
	CTeams PreviousTeams(void);
	void PreviousTeams(const CTeams &teams);

private:
	CTeams m_previousTeams;
};

#endif
