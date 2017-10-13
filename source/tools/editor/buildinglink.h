//----------------------------------------------------------------------------
//
// Link.h - represents a link between buildings
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef BUILDINGLINK_H
#define BUILDINGLINK_H

namespace Stuff
{
class Vector3D;
}

class EditorObject;

class BuildingLink
{

  public:
	BuildingLink(const EditorObject* pParent);
	BuildingLink(const BuildingLink& Link);
	BuildingLink& operator=(const BuildingLink&);
	BuildingLink(const Stuff::Vector3D& ParentPosition);

	bool HasChild(const EditorObject*) const;
	bool HasParent(const EditorObject*) const;
	int32_t GetLinkCount(void) const;
	bool RemoveObject(const EditorObject*);
	const Stuff::Vector3D& GetParentPosition(void) const;
	void SetParentPosition(const Stuff::Vector3D& pos);
	int32_t GetChildrenPositions(
		Stuff::Vector3D*, int32_t MaxNumberOfPoints) const;
	bool AddChild(const EditorObject* pObject);
	void SetParentAlignment(int32_t alignment);
	void FixHeights(void);

	static bool TypeCanBeParent(
		const EditorObject*); // type gotten from building mgr
	static bool CanLink(
		const EditorObject* pParent, const EditorObject* pChild);

	bool operator==(const BuildingLink& Src) const;

	void render(void);

  private:
	struct Info
	{
		Info(void) : m_ID(0), ppos(0) {}
		~Info(void);
		int32_t m_ID;
		Stuff::Vector3D* ppos;
		bool operator==(const Info& Src) const;
	};

	Info parent;
	std::list<Info /*, const Info&*/> children;

	// Helper function
	void CopyData(const BuildingLink&);

	friend class EditorObjectMgr;
};

#endif // BUILDINGLINK_H