//----------------------------------------------------------------------------
//
// LinkBrush.cpp - the link brush links turrets to turret controls, gates to 
//						gate controls etc.
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "LinkBrush.h"
#include "BuildingLink.h"
#include "EditorObjectMgr.h"

LinkBrush::LinkBrush( bool Link ) 
{
	// here we need to get all of the links on the map
	// and add an interface element for each one. 
	parentPos.x = parentPos.y = -1;
	bLink = Link;
	parent = NULL;
}

LinkBrush::~LinkBrush()
{
}

bool LinkBrush::beginPaint()
{
	pAction = new LinkAction();

	return true;

}

Action* LinkBrush::endPaint()
{
	if ( pAction && pAction->changedLinks.Count() )
	{
		return pAction;
	}
	else
		delete pAction;

	return NULL;
}

bool LinkBrush::paint( Stuff::Vector3D& worldPos, int screenX, int screenY )
{
	if ( !bLink )
		return unPaint( worldPos, screenX, screenY );
	
	const EditorObject* pObject = EditorObjectMgr::instance()->getObjectAtPosition( worldPos );

	if ( !pObject )
		return 0;

	if (BuildingLink::TypeCanBeParent( pObject ) && 
		( !parent || ( parent && !BuildingLink::CanLink( parent, pObject ) ) ) )
	{
		
		parent = pObject;
		parentPos = parent->getPosition();
			
		BuildingLink* pLink = EditorObjectMgr::instance()->getLinkWithParent( pObject );

		if ( pLink )
		{
			pAction->AddToListOnce( LinkInfo(pLink, LinkInfo::EDIT) );
		}
		else
		{
			BuildingLink* pLink = new BuildingLink( pObject );

			EditorObjectMgr::instance()->addLink( pLink );

			pAction->AddToListOnce( LinkInfo(pLink, LinkInfo::ADD) );
		}

	}

	else // we already have a parnet, this is a child building
	{
		if ( !parent )
		{
			return false;
		}

		BuildingLink* pLink = EditorObjectMgr::instance()->getLinkWithParent( parent );

		if ( !pLink )
		{
			pLink = new BuildingLink( parent );

			EditorObjectMgr::instance()->addLink( pLink );
			pAction->AddToListOnce( LinkInfo(pLink, LinkInfo::ADD) );

		}
		
		BuildingLink* pOldLink = EditorObjectMgr::instance()->getLinkWithBuilding( pObject );

		if ( pOldLink && pOldLink != pLink )
		{
			pAction->AddToListOnce( LinkInfo(pOldLink, LinkInfo::EDIT ) );
			pOldLink->RemoveObject( pObject );
			pAction->AddToListOnce( LinkInfo( pLink, LinkInfo::EDIT  ));
		}
		if ( !pLink->AddChild( pObject ) )
		{
			SPEW((0,"LinkBrush failed to add a link to a parent\n"));
		
		}

		if ( pObject->getSpecialType() != EditorObjectMgr::POWER_STATION )
			(const_cast<EditorObject*>(pObject))->setAlignment( parent->getAlignment() ); 

		return true;
	}


	return false;
	
	
}


bool LinkBrush::canPaint( Stuff::Vector3D& pos, int x, int y, int flags ) 
{ 
	if ( !bLink )
		return canUnPaint( pos, x, y, flags );
	
	const EditorObject* pObject = EditorObjectMgr::instance()->getObjectAtPosition( pos );
		
	if ( !pObject )
		return false;

	if ( BuildingLink::TypeCanBeParent( pObject) ) // see if this can be a parent
	{
		return  true;
	}

	else if ( parent ) 
	{
		if ( BuildingLink::CanLink( parent, pObject) )
			return true;
	}


	return false;
}

bool LinkBrush::canUnPaint( Stuff::Vector3D& pos, int x, int y, int flags )
{ 

	const EditorObject* pBuilding = EditorObjectMgr::instance()->getObjectAtPosition( pos );
	
	if ( !pBuilding )
		return false;

	if ( EditorObjectMgr::instance()->getLinkWithParent( pBuilding ) )
		return true;

	else if ( EditorObjectMgr::instance()->getLinkWithBuilding( pBuilding) )
		return true; 


	return false;
	 
} 

int LinkBrush::LinkAction::AddToListOnce( const LinkBrush::LinkInfo& Info )
{
	for( EList< LinkInfo, const LinkInfo& >::EIterator iter = changedLinks.Begin();
	!iter.IsDone(); iter++ )
	{
		if ( (*iter).m_LinkCopy.GetParentPosition() == Info.m_LinkCopy.GetParentPosition() )
		{
			return 0;  // assume if parents are in the same place, we have the same link
		}
	}

	changedLinks.Append( Info );
	return 1;
}

LinkBrush::LinkAction::LinkAction( )
{ 
	
}

bool LinkBrush::LinkAction::undo( )
{
	// go through each of the objects in the list
	for ( EList<LinkInfo, const LinkInfo& >::EIterator iter = changedLinks.End(); 
	 !iter.IsDone(); iter-- )
	{
		 if ( (*iter).type == LinkInfo::ADD )
		{
			// change the type to remove, and take it out of the map
			(*iter).type = LinkInfo::REMOVE;

			Stuff::Vector3D pos = (*iter).m_LinkCopy.GetParentPosition();
			
			const EditorObject* pBuilding = EditorObjectMgr::instance()->getObjectAtLocation( pos.x, pos.y );
			gosASSERT( pBuilding );

			if ( pBuilding )
			{
				BuildingLink* pLink = EditorObjectMgr::instance()->getLinkWithBuilding( pBuilding );

				if ( pLink )
				{

					(*iter).m_LinkCopy = *(pLink);
					EditorObjectMgr::instance()->deleteLink( pLink );
				}
			}  
		}
		else if ( (*iter).type == LinkInfo::REMOVE )
		{
			// change the type to remove, and take it out of the map
			(*iter).type = LinkInfo::ADD;

			EditorObjectMgr::instance()->addLink( new BuildingLink((*iter).m_LinkCopy) );

			// make sure each of the buildings in the link has the right alignment
			int LinkCount = (*iter).m_LinkCopy.GetLinkCount();
			Stuff::Vector3D* pPoints = new Stuff::Vector3D[LinkCount];
			(*iter).m_LinkCopy.GetChildrenPositions( pPoints, LinkCount );

			Stuff::Vector3D pos = (*iter).m_LinkCopy.GetParentPosition();

			const EditorObject* pBuilding = EditorObjectMgr::instance()->getObjectAtLocation( pos.x, pos.y );
			if ( !pBuilding )
				return false;

			int align = pBuilding->getAlignment();

			for ( int i = 0; i < LinkCount; ++i )
			{
				const EditorObject* pObject = EditorObjectMgr::instance()->getObjectAtLocation( pPoints[i].x, pPoints[i].y );
				if ( pObject )
				{
					(const_cast<EditorObject*>(pObject))->setAlignment( align );
				}
			}

		}
		else if ( (*iter).type == LinkInfo::EDIT )
		{
			// need to find the original link, look for one with matching
			// parents

			Stuff::Vector3D pos = (*iter).m_LinkCopy.GetParentPosition();
			const EditorObject* pBuilding = EditorObjectMgr::instance()->getObjectAtLocation( pos.x, pos.y );
			if ( pBuilding )
			{
				BuildingLink* pLink = EditorObjectMgr::instance()->getLinkWithParent( pBuilding );

				if ( pLink )
				{
					BuildingLink tmp( *pLink );
					*pLink = (*iter).m_LinkCopy;
					(*iter).m_LinkCopy = tmp;
				}
			}
			else
			{

				SPEW(( 0,"LinkBursh::LinkAction::Undo failed because the map didn't have the link\n") );
				return false;
			}

		}

	}

	return true;

}

bool LinkBrush::unPaint( Stuff::Vector3D& pos, int XPos, int yPos )
{
 
	const EditorObject* pBuilding = EditorObjectMgr::instance()->getObjectAtPosition( pos );

	if ( pBuilding )
	{

		BuildingLink* pLink = EditorObjectMgr::instance()->getLinkWithParent( pBuilding );

		if ( pLink ) // we are deleting a parent link
		{
			pAction->AddToListOnce( LinkInfo( pLink, LinkInfo::REMOVE ) );

			EditorObjectMgr::instance()->deleteLink( pLink );

			parent = NULL;
			parentPos.x = parentPos.y = 0;

			return true;
		}
		
		else 
		{
			pLink = EditorObjectMgr::instance()->getLinkWithBuilding( pBuilding );

			if ( pLink )
			{
				pAction->AddToListOnce( LinkInfo( pLink, LinkInfo::EDIT ) );
				pLink->RemoveObject( pBuilding );
				
			}

			return true;
		}
	}
	
	return false;
}

bool LinkBrush::LinkAction::redo()
{
	return undo();
}

LinkBrush::LinkInfo::LinkInfo( BuildingLink* pOriginal, LinkBrush::LinkInfo::TYPE Type ) : 
m_LinkCopy( *pOriginal )
{
	type = Type;
}

void LinkBrush::render( int screenX, int screenY )
{

	if ( parent )
	{
		Stuff::Vector3D parentPos = parent->getPosition();
		Stuff::Vector4D screenPos;
		Stuff::Vector4D curScreen;
		curScreen.x = screenX;
		curScreen.y = screenY;
		curScreen.z = 0.1f;		//Gotta set Z or QNAN and crash!
		curScreen.w = 0.9999f;	//Gotta set W, too!

		eye->projectZ( parentPos, screenPos );
	
		LineElement elem( curScreen, screenPos, 0xffff0000, 0, -1 );
		elem.draw();

	}
}