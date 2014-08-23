#ifndef EDITORINTERFACE_H
#define EDITORINTERFACE_H
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//#pragma warning( disable : 4786 )


#ifndef MCLIB_H
#include <mclib.h>
#endif

#ifndef ACTION_H
#include "Action.h"
#endif

#ifndef EDITOROBJECTMGR_H
#include "editorobjectmgr.h"
#endif

#ifndef EDITORDATA_H
#include "editordata.h"
#endif

#ifndef EDITORTACMAP_H
#include "EditorTacMap.h"
#endif

#include "Objective.h"

#include "stdafx.h"

// forward declarations
class DlgFileOpen;
class Brush;
class MainMenu;
class Menu;

// global resource handle
extern uint32_t gameResourceHandle;
extern bool DebuggerActive;

// ARM
namespace Microsoft
{
	namespace Xna
	{
		namespace Arm
		{
			struct IProviderEngine;
			struct IProviderAsset;
		}
	}
}

extern Microsoft::Xna::Arm::IProviderEngine * armProvider;


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
	
	EditorInterface();
	~EditorInterface();

	void handleNewMenuMessage( int32_t specificMessage );

	void init( PCSTR fileName );

	void terminate();

	void ChangeCursor( int32_t ID );

	int32_t MissionSettings();

	int32_t Team( int32_t team );
	int32_t Player( int32_t player );
	/* When in "ObjectSelectOnlyMode", the interface is put in selection mode and all
	features are disabled except those pertaining to selection of objects. This mode is
	engaged from the objectives dialog. */
	bool ObjectSelectOnlyMode() { return bObjectSelectOnlyMode; }
	void ObjectSelectOnlyMode(bool val) { bObjectSelectOnlyMode = val; }
	CObjectivesEditState objectivesEditState;	/* persistent storage for the objective(s) dialog */
	void SelectionMode() { Select(); }
	
	int32_t RefractalizeTerrain( int32_t threshold );

	virtual void handleLeftButtonDown( int32_t PosX, int32_t PosY ); // mouse button down
	virtual void handleLeftButtonDbl( int32_t PosX, int32_t PosY ){} // mouse button dbl click
	virtual void handleLeftButtonUp( int32_t PosX, int32_t PosY ); // pop ups etc need this
	virtual void handleKeyDown( int32_t Key );
	virtual void handleMouseMove( int32_t PosX, int32_t PosY );

	void update (void);

	virtual void render();
	void initTacMap();
	void updateTacMap() { tacMap.UpdateMap(); }

	void syncHScroll();
	void syncVScroll();
	void syncScrollBars() { syncHScroll(); syncVScroll(); }

	int32_t Width(){  RECT tmp; GetWindowRect( &tmp ); return tmp.right - tmp.left; }
	int32_t Height() { RECT tmp; GetWindowRect( &tmp ); return tmp.bottom - tmp.top; }

	void SetBusyMode(bool bRedrawWindow = true);
	void UnsetBusyMode();

	bool SafeRunGameOSLogic();

	bool ThisIsInitialized() { return this->bThisIsInitialized; }

	afx_msg void UpdateButton( CCmdUI* button );

	int32_t Quit();
	int32_t Save();
	int32_t SaveAs();
	int32_t QuickSave();
	int32_t PromptAndSaveIfNecessary();

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
	afx_msg void OnPaint();
	afx_msg void OnViewRefreshtacmap();
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
	afx_msg void OnDestroy();
	afx_msg void OnForestTool();
	afx_msg void OnOtherEditforests();
	afx_msg void OnViewOrthographiccamera();
	afx_msg void OnViewShowpassabilitymap();
	afx_msg void OnMButtonUp(uint32_t nFlags, CPoint point);
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	afx_msg void OnCommand(WPARAM wParam);
	

private:

	// Message handlers
	int32_t Undo();
	int32_t Redo();	
	int32_t FileOpen();
	int32_t New();
	int32_t PaintDirtRoad();
	int32_t PaintRocks();
	int32_t PaintPaved();
	int32_t PaintTwoLaneDirtRoad();
	int32_t PaintDamagedRoad();
	int32_t PaintRunway();
	int32_t PaintBridge();
	int32_t PaintDamagedBridge();
	int32_t Erase();
	int32_t Select();
	int32_t Flatten();
	int32_t Fog();
	int32_t PurgeTransitions();
	int32_t ShowTransitions();
	int32_t Light();
	int32_t AssignElevation();

	int32_t paintBuildings( int32_t message );
	int32_t PaintTerrain( int32_t type );
	int32_t PaintOverlay( int32_t type, int32_t message );

	int32_t NewHeightMap();
	int32_t SaveCameras();
	int32_t SelectSlopes();
	int32_t SelectAltitude();
	int32_t SelectTerrainType();
	int32_t Waves();
	int32_t SaveHeightMap();

	int32_t DragSmooth();
	int32_t DragRough();

	int32_t SmoothRadius();
	int32_t Alignment( int32_t specific );
	int32_t Damage( bool bDamage );
	int32_t Link( bool bLink );
	int32_t LayMines();
	int32_t SelectDetailTexture();
	int32_t SelectWaterTexture();
	int32_t SelectWaterDetailTexture();
	int32_t TextureTilingFactors();
	int32_t ReloadBaseTexture();
	int32_t SetBaseTexture();
	int32_t DropZone( bool bVTol );
	int32_t UnitSettings( );
	
	int32_t SetSky (int32_t skyId);
	
	int32_t CampaignEditor();

	// helpers
	void KillCurBrush();

	void addBuildingsToNewMenu();

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

	~Editor(){ destroy(); }

	void destroy (void);

	void init( PSTR loader );
	
	void render();
	
	void update();

	void resaveAll();		//Used by autoBuild to automagically resave all maps with correct data.	
};


class TeamsAction : public Action
{
public:
	TeamsAction() : Action() {}
	TeamsAction(const CTeams &teams) : Action() { PreviousTeams(teams); }
	virtual ~TeamsAction() {}
	virtual bool redo() { return undo(); }
	virtual bool undo();
	CTeams PreviousTeams();
	void PreviousTeams(const CTeams &teams);

private:
	CTeams m_previousTeams;
};

#endif
