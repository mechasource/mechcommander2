#ifndef LINKBRUSH_H
#define LINKBRUSH_H
/*************************************************************************************************\
LinkBrush.h			: Interface for the LinkBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef BRUSH_H
#include "Brush.h"
#endif

#ifndef ACTION_H
#include "Action.h"
#endif

#ifndef BUILDINGLINK_H
#include "BuildingLink.h"
#endif

#include <mclib.h>
class EditorObject;

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
LinkBrush:
**************************************************************************************************/
class LinkBrush: public Brush
{
	public:

	LinkBrush( bool bLink );
	virtual ~LinkBrush();
	virtual bool beginPaint();
	virtual Action* endPaint();
	virtual bool paint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY  );
	virtual bool canPaint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags );
	virtual bool canPaintSelection( ){ return false; }
	virtual void render( int32_t screenX, int32_t screenY );

	const EditorObject*		parent;
	Stuff::Vector3D			parentPos;
	bool					bLink;


	bool unPaint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY  );
	bool canUnPaint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags );

	struct LinkInfo
	{
		
		enum TYPE
		{
			ADD = 0,
			REMOVE,
			EDIT
		};

		LinkInfo( BuildingLink* m_pOriginal, TYPE type = EDIT  );
		
		BuildingLink m_LinkCopy;  // copy of link
		int32_t  type;

	};
	
	class LinkAction : public Action
	{
	
	public:
		
		
		LinkAction();
		~LinkAction( ){}
		
		virtual bool redo();
		virtual bool undo();

		EList< LinkInfo, const LinkInfo& > changedLinks;
		AddToListOnce( const LinkAction::LinkInfo& info );
	};

	LinkAction* pAction;

};


//*************************************************************************************************
#endif  // end of file ( LinkBrush.h )
