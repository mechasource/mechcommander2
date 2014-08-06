#ifndef EDITORINTERFACE_H
#define EDITORINTERFACE_H
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma warning( disable : 4786 )


#ifndef MCLIB_H
#include "mclib.h"
#endif

#ifndef ACTION_H
#include "Action.h"
#endif

#ifndef EDITOROBJECTMGR_H
#include "EditorObjectMgr.h"
#endif

#ifndef EDITORDATA_H
#include "EditorData.h"
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
extern unsigned long gameResourceHandle;
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

	void handleNewMenuMessage( long specificMessage );

	void init( const char* fileName );

	void terminate();

	void ChangeCursor( int ID );

	int MissionSettings();

	int Team( int team );
	int Player( int player );
	/* When in "ObjectSelectOnlyMode", the interface is put in selection mode and all
	features are disabled except those pertaining to selection of objects. This mode is
	engaged from the objectives dialog. */
	bool ObjectSelectOnlyMode() { return bObjectSelectOnlyMode; }
	void ObjectSelectOnlyMode(bool val) { bObjectSelectOnlyMode = val; }
	CObjectivesEditState objectivesEditState;	/* persistent storage for the objective(s) dialog */
	void SelectionMode() { Select(); }
	
	int RefractalizeTerrain( long threshold );

	virtual void handleLeftButtonDown( int PosX, int PosY ); // mouse button down
	virtual void handleLeftButtonDbl( int PosX, int PosY ){} // mouse button dbl click
	virtual void handleLeftButtonUp( int PosX, int PosY ); // pop ups etc need this
	virtual void handleKeyDown( int Key );
	virtual void handleMouseMove( int PosX, int PosY );

	void update (void);

	virtual void render();
	void initTacMap();
	void updateTacMap() { tacMap.UpdateMap(); }

	void syncHScroll();
	void syncVScroll();
	void syncScrollBars() { syncHScroll(); syncVScroll(); }

	long Width(){  RECT tmp; GetWindowRect( &tmp ); return tmp.right - tmp.left; }
	long Height() { RECT tmp; GetWindowRect( &tmp ); return tmp.bottom - tmp.top; }

	void SetBusyMode(bool bRedrawWindow = true);
	void UnsetBusyMode();

	bool SafeRunGameOSLogic();

	bool ThisIsInitialized() { return this->bThisIsInitialized; }

	afx_msg void UpdateButton( CCmdUI* button );

	int Quit();
	int Save();
	int SaveAs();
	int QuickSave();
	int PromptAndSaveIfNecessary();

	ActionUndoMgr				undoMgr;

		//{{AFX_VIRTUAL(EditorInterface)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	protected:
		//{{AFX_MSG(EditorInterface)
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
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
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	afx_msg void OnCommand(WPARAM wParam);
	

private:

	// Message handlers
	int Undo();
	int Redo();	
	int FileOpen();
	int New();
	int PaintDirtRoad();
	int PaintRocks();
	int PaintPaved();
	int PaintTwoLaneDirtRoad();
	int PaintDamagedRoad();
	int PaintRunway();
	int PaintBridge();
	int PaintDamagedBridge();
	int Erase();
	int Select();
	int Flatten();
	int Fog();
	int PurgeTransitions();
	int ShowTransitions();
	int Light();
	int AssignElevation();

	int paintBuildings( int message );
	int PaintTerrain( int type );
	int PaintOverlay( int type, int message );

	int NewHeightMap();
	int SaveCameras();
	int SelectSlopes();
	int SelectAltitude();
	int SelectTerrainType();
	int Waves();
	int SaveHeightMap();

	int DragSmooth();
	int DragRough();

	int SmoothRadius();
	int Alignment( int specific );
	int Damage( bool bDamage );
	int Link( bool bLink );
	int LayMines();
	int SelectDetailTexture();
	int SelectWaterTexture();
	int SelectWaterDetailTexture();
	int TextureTilingFactors();
	int ReloadBaseTexture();
	int SetBaseTexture();
	int DropZone( bool bVTol );
	int UnitSettings( );
	
	int SetSky (long skyId);
	
	int CampaignEditor();

	// helpers
	void KillCurBrush();

	void addBuildingsToNewMenu();

	void rotateSelectedObjects( int direction );

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
	int							oldCursor;
	bool						painting;
	bool						selecting;
	bool						dragging;
	bool						highlighted;

	int							currentBrushID;
	int							currentBrushMenuID;

	MainMenu					*m_pMainMenu;

	int							smoothRadius;
	bool						bSmooth;

	static EditorInterface*		s_instance;
	Stuff::Vector3D				lastClickPos;

	HCURSOR						hCursor;
	int							curCursorID;
	int							lastX;
	int							lastY;
	long						lastKey;
	bool 						bObjectSelectOnlyMode;
	CMenu**						menus;

	EditorTacMap				tacMap;
	HACCEL						m_hAccelTable;

	bool						rightDrag;
	unsigned long				lastRightClickTime;

	HBITMAP m_hSplashBitMap;
	HCURSOR m_hBusyCursor;
	int m_AppIsBusy;
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

	void init( char* loader );
	
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
