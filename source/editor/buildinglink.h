#ifndef BUILDINGLINK_H
#define BUILDINGLINK_H
//----------------------------------------------------------------------------
//
// Link.h - represents a link between buildings
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "Elist.h"

class EditorObject;

class BuildingLink
{

public:

	BuildingLink( const EditorObject* pParent );
	BuildingLink( const BuildingLink& Link );
	BuildingLink& operator=( const BuildingLink& );
	BuildingLink( const Stuff::Vector3D& ParentPosition );

	bool	HasChild( const EditorObject* ) const;
	bool	HasParent( const EditorObject* ) const;
	int		GetLinkCount() const;
	bool	RemoveObject( const EditorObject* );
	const	Stuff::Vector3D& GetParentPosition() const;
	void	SetParentPosition( const Stuff::Vector3D& pos );
	int		GetChildrenPositions( Stuff::Vector3D*, int MaxNumberOfPoints ) const;
	bool	AddChild( const EditorObject* pObject );
	void	SetParentAlignment( int alignment );
	void	FixHeights();

	static	bool TypeCanBeParent( const EditorObject* ); // type gotten from building mgr
	static	bool CanLink( const EditorObject* pParent, const EditorObject* pChild );

	bool operator==( const BuildingLink& Src )const;

	void	render();


private:
	 struct Info
	 {
		 long m_ID;		 
		 Stuff::Vector3D pos;
	 	 bool operator==( const Info& Src )const;
	 };

	 Info	parent;
	 EList< Info, const Info& > children;


	 // Helper function
	 void CopyData( const BuildingLink& );

	 friend class EditorObjectMgr;
};



#endif// BUILDINGLINK_H