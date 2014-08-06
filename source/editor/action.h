#ifndef ACTION_H
#define ACTION_H
//--------------------------------------------------------------------------------
//
// Actions.h - header file for the Action Base object and the Action Mgr.
//				Actions are things that can be undone
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#define MAX_DESC_LENGTH 256

class ActionUndoMgr;
class Action;

#ifndef ELIST_H
#include "Elist.h"
#endif

// Abstract base class for all action objects
class Action 
{

public:

	virtual ~Action(){}
	virtual bool redo() = 0;
	virtual bool undo() = 0;
	Action& operator=( const Action& src );
	const char* getDescription(){  return m_strDescription; }


	char  m_strDescription[MAX_DESC_LENGTH];


protected:
	
	// suppressed
	Action( const char* pStr )
	{ 
		gosASSERT( strlen( pStr ) < MAX_DESC_LENGTH );
		strcpy( m_strDescription, pStr );
	}

	// if you call this, make sure you set the description
	Action(){ m_strDescription[0] = 0; }


};

struct VertexInfo
{
	VertexInfo( long row, long column );
	~VertexInfo(){}
	int row;
	int column;
	int terrainData;
	int textureData;
	float elevation;

private:
	// make sure the list class doesn't try and use this
	VertexInfo& operator=( const VertexInfo& );

};

typedef EList< VertexInfo, const VertexInfo& > VERTEX_INFO_LIST;

// for undo redo buffer, since it deals with 
// the same stuff as the tile brush, I put
// it here
class ActionPaintTile : public Action
{
	public:

		ActionPaintTile(){}

		// virtual overrides
		virtual bool redo();
		virtual bool undo();
		
		bool doRedo(); // so we don't go through virtual functions

		ActionPaintTile( const char* pStr ) 
			: Action( pStr ){}

		void addChangedVertexInfo( int row, int column );
		void addVertexInfo( VertexInfo& );
		bool getOldHeight( int row, int column, float& oldHeight );

		VERTEX_INFO_LIST	vertexInfoList;	

	private:

};


#ifndef EDITOROBJECTS_H
#include "EditorObjects.h"
#endif
class ModifyBuildingAction : public Action
{
public:
	
	virtual ~ModifyBuildingAction();
	virtual bool redo();
	virtual bool undo();
	bool doRedo(); // so we don't go through virtual functions
	virtual void addBuildingInfo(EditorObject& info);
	virtual bool isNotNull()
	{
		return (0 < buildingCopyPtrs.Count());
	}
	virtual void updateNotedObjectPositions();

private:
	typedef EList< EditorObject *, EditorObject *> OBJ_INFO_PTR_LIST;
	typedef EList< ObjectAppearance, ObjectAppearance&> OBJ_APPEAR_LIST;
	struct CObjectID
	{
		float x;
		float y;
	};
	typedef EList<CObjectID, CObjectID&> OBJ_ID_LIST;

	OBJ_INFO_PTR_LIST buildingCopyPtrs;
	OBJ_APPEAR_LIST buildingAppearanceCopies;

	OBJ_INFO_PTR_LIST buildingPtrs;
	OBJ_ID_LIST buildingIDs;
};





// this mgr holds all the actions
class ActionUndoMgr
{

public:	
	ActionUndoMgr( );
	~ActionUndoMgr();
	
	void AddAction( Action* pAction );
	bool Redo();
	bool Undo();
	void Reset();

	bool HaveUndo() const;
	bool HaveRedo() const;

	const char* GetUndoString();
	const char* GetRedoString();

	void NoteThatASaveHasJustOccurred();
	bool ThereHasBeenANetChangeFromWhenLastSaved();

	static ActionUndoMgr*	instance;

private:

	typedef EList< Action*, Action* >	ACTION_LIST;
	typedef long	ACTION_POS;

	void				EmptyUndoList();
	ACTION_LIST			m_listUndoActions;
	ACTION_POS			m_CurrentPos;
	ACTION_POS			m_PosOfLastSave;
}; // class SActionMgr

#endif// ACTION_H