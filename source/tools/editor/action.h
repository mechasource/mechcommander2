//--------------------------------------------------------------------------------
//
// Actions.h - header file for the Action Base object and the Action Mgr.
//				Actions are things that can be undone
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef ACTION_H
#define ACTION_H

class EditorObject;
class ObjectAppearance;

class ActionUndoMgr;
class Action;

// Abstract base class for all action objects
class Action
{

public:

	virtual ~Action(void) {}
	virtual bool redo(void) = 0;
	virtual bool undo(void) = 0;
	Action& operator=(const Action& src);
	PCWSTR getDescription(void)
	{
		return m_strDescription.c_str(void);
	}

protected:
	std::wstring m_strDescription;

	// suppressed
	Action(PCWSTR pStr)
	{
		m_strDescription.assign(pStr);
	}

	// if you call this, make sure you set the description
	Action(void) { }
};

typedef struct VertexInfo
{
	VertexInfo(uint32_t row, uint32_t column);
	~VertexInfo(void) {}

	uint32_t row;
	uint32_t column;
	uint32_t terrainData;
	uint32_t textureData;
	float elevation;

private:
	// make sure the list class doesn't try and use this
	VertexInfo& operator=(const VertexInfo&);

} VertexInfo;

// for undo redo buffer, since it deals with
// the same stuff as the tile brush, I put
// it here
class ActionPaintTile : public Action
{
public:

	ActionPaintTile(void) {}

	// virtual overrides
	virtual bool redo(void);
	virtual bool undo(void);

	bool doRedo(void); // so we don't go through virtual functions

	ActionPaintTile(PCWSTR pStr)
		: Action(pStr) {}

	void addChangedVertexInfo(uint32_t row, uint32_t column);
	void addVertexInfo(VertexInfo&);
	bool getOldHeight(uint32_t row, uint32_t column, float& oldHeight);

private:
	typedef std::list< VertexInfo /*, const VertexInfo& */ > VERTEX_INFO_LIST;
	VERTEX_INFO_LIST	vertexInfoList;
};

class ModifyBuildingAction : public Action
{
public:

	virtual ~ModifyBuildingAction(void);
	virtual bool redo(void);
	virtual bool undo(void);
	bool doRedo(void); // so we don't go through virtual functions
	virtual void addBuildingInfo(EditorObject& info);
	virtual bool isNotNull(void)
	{
		return (!buildingCopyPtrs.empty());
	}
	virtual void updateNotedObjectPositions(void);

private:
	typedef std::list< EditorObject* /*, EditorObject**/ > OBJ_INFO_PTR_LIST;
	typedef std::list< ObjectAppearance /*, ObjectAppearance&*/ > OBJ_APPEAR_LIST;

	typedef std::list< __m64 /*, __m64&*/ > OBJ_ID_LIST;	// MMX Data Type

	OBJ_INFO_PTR_LIST buildingCopyPtrs;
	OBJ_APPEAR_LIST buildingAppearanceCopies;
	OBJ_INFO_PTR_LIST buildingPtrs;
	OBJ_ID_LIST buildingIDs;
};

// this mgr holds all the actions
class ActionUndoMgr
{

public:
	ActionUndoMgr(void);
	~ActionUndoMgr(void);

	void AddAction(Action* pAction);
	bool Redo(void);
	bool Undo(void);
	void Reset(void);

	bool HaveUndo(void) const;
	bool HaveRedo(void) const;

	PCWSTR GetUndoString(void);
	PCWSTR GetRedoString(void);

	void NoteThatASaveHasJustOccurred(void);
	bool ThereHasBeenANetChangeFromWhenLastSaved(void);

	static ActionUndoMgr*	instance;

private:

	typedef std::list< Action* /*, Action**/ > ACTION_LIST;
	typedef uint32_t ACTION_POS;

	void				EmptyUndoList(void);
	ACTION_LIST			m_listUndoActions;
	ACTION_POS			m_CurrentPos;
	ACTION_POS			m_PosOfLastSave;
}; // class SActionMgr

#endif// ACTION_H
