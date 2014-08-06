//----------------------------------------------------------------------------
//
// Link.cpp - 
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "mclib.h"
#include "BuildingLink.h"
#include "EditorObjectMgr.h"
#include <algorithm>
#include "celine.h"


BuildingLink::BuildingLink( const EditorObject* pParent )
{
	gosASSERT( pParent );
	
	parent.m_ID = pParent->getID();
	parent.pos = pParent->getPosition();
}

BuildingLink::BuildingLink( const Stuff::Vector3D& pos )
{
	parent.pos = pos;
	parent.m_ID = -1;
}

BuildingLink::BuildingLink( const BuildingLink& Src )
{
	CopyData( Src );
}

BuildingLink& BuildingLink::operator=( const BuildingLink& Src )
{
	if ( &Src != this )
	{
		CopyData( Src );
	}

	return *this;
}

void BuildingLink::CopyData( const BuildingLink& Src )
{
	parent = Src.parent;

	children.Clear();

	for ( EList< Info, const Info& >::EConstIterator iter = Src.children.Begin();
	!iter.IsDone(); iter++ )
	{
		children.Append( *iter );	
	}
}



bool BuildingLink::AddChild( const EditorObject* pObject )
{
	int ID = pObject->getID();
	
	for( EList< Info, const Info& >::EIterator iter = children.Begin();
		!iter.IsDone(); iter++ )
		{
			if ( (*iter).pos == pObject->getPosition() &&
				 (*iter).m_ID == ID )
			{
				SPEW( (0, "Link::AddChild, not adding an object because it is"
					"already there\n" ) );

				return false;
			}
		}

	Info tmp;
	tmp.pos = pObject->getPosition();
	tmp.m_ID = ID;

	children.Append( tmp );

	return true;

}

int BuildingLink::GetLinkCount( ) const
{
	return children.Count();
}


const Stuff::Vector3D& BuildingLink::GetParentPosition() const
{
	return parent.pos;
}

bool BuildingLink::HasChild( const EditorObject* pObject ) const
{
	int ID = pObject->getID();
	pObject->getPosition();

	
	for( EList< Info, const Info& >::EConstIterator iter = children.Begin();
		!iter.IsDone(); iter++ )
	{
		if ( (*iter).pos == pObject->getPosition() )
		{
			if ( (*iter).m_ID == ID )
				return true;
		}
	}

	return false;
}

bool BuildingLink::HasParent( const EditorObject* pObject ) const
{
	int ID = pObject->getID();

	// ignore damage bit and rotation
	if ( parent.pos == pObject->getPosition() && parent.m_ID == ID )
		return true;

	return false;
}

bool BuildingLink::TypeCanBeParent( const EditorObject* pObject )
{
	int Type = pObject->getSpecialType();
	switch (Type)
	{
		case EditorObjectMgr::TURRET_CONTROL:
		case EditorObjectMgr::GATE_CONTROL:
		case EditorObjectMgr::POWER_STATION:
		case EditorObjectMgr::SENSOR_CONTROL:
		case EditorObjectMgr::TURRET_GENERATOR:
		case EditorObjectMgr::BRIDGE_CONTROL:
		case EditorObjectMgr::SPOTLIGHT_CONTROL:
			return true;
			break;

		default:
			break;
	};

	return false;

}
bool BuildingLink::CanLink( const EditorObject* pParent, const EditorObject* pChild  )
{
	int ParentType = pParent->getSpecialType();
	int ChildType = pChild->getSpecialType();
	
	switch (ParentType)
	{
		case EditorObjectMgr::TURRET_CONTROL:
			return (ChildType == EditorObjectMgr::EDITOR_TURRET
					|| ChildType == EditorObjectMgr::LOOKOUT ) ? true : false; 
			break;
		case EditorObjectMgr::GATE_CONTROL:
			return ChildType == EditorObjectMgr::EDITOR_GATE ? true : false;
			break;
		case EditorObjectMgr::POWER_STATION:
			return (ChildType == EditorObjectMgr::BRIDGE_CONTROL
				|| ChildType == EditorObjectMgr::GATE_CONTROL
				|| ChildType == EditorObjectMgr::SPOTLIGHT_CONTROL ) ? true : false;
			break;
		case EditorObjectMgr::SENSOR_CONTROL:
			return ChildType == EditorObjectMgr::SENSOR_TOWER ? true : false;
			break;
		case EditorObjectMgr::TURRET_GENERATOR:
			return ChildType == EditorObjectMgr::TURRET_CONTROL ? true : false;
			break;

		case EditorObjectMgr::SPOTLIGHT_CONTROL:
			return ChildType == EditorObjectMgr::SPOTLIGHT ? true : false;
			break;

		case EditorObjectMgr::BRIDGE_CONTROL:
			return ChildType == EditorObjectMgr::EDITOR_BRIDGE ? true : false;
			break;
		default:
			break;
	};

	return false;
}

bool BuildingLink::RemoveObject( const EditorObject* pObject )
{
	if ( HasParent( pObject ) )
	{
		SPEW(( 0, "BuildingLink::RemoveObject could not remove a parent object\n") );
		return false;
	}

	Stuff::Vector3D pos = pObject->getPosition( );
	long ID = pObject->getID();
	
	for ( EList< Info, const Info& >::EIterator iter = children.Begin();
	!iter.IsDone(); iter++ )
	{
		if ( ID == (*iter).m_ID && pos == (*iter).pos  )
		{
			children.Delete( iter );
			return true;
		}
	}

	SPEW(( 0, "BuildingLInk::RemoveObject failed because it didn't have the object\n") );
	return false;
}

int BuildingLink::GetChildrenPositions( Stuff::Vector3D* pos, int Count ) const
{
	if ( children.Count() > (unsigned long)Count )
		return children.Count();

	int i = 0;
	for ( EList< Info, const Info& >::EConstIterator iter = children.Begin();
	!iter.IsDone(); iter++ )
	{
		pos[i++] = (*iter).pos;
	}

	return i; 
}

static bool isInView(const Point3D &position)
{
	/* the code in this function was largely swiped from "bool BldgAppearance::recalcBounds (void)" on 4/24/00 */
	bool inView = false;

	if (eye)
	{
 		//--------------------------------------------------
		// First, if we are using perspective, figure out
		// if object too far from camera.  Far Clip Plane.
		if (eye->usePerspective)
		{
			Stuff::Point3D Distance;
			Stuff::Point3D eyePosition(eye->getPosition());
			Stuff::Point3D objPosition(position);
	
			Distance.Subtract(objPosition,eyePosition);
			float eyeDistance = Distance.GetApproximateLength();
			if (eyeDistance > Camera::MaxClipDistance)
			{
				//hazeFactor = 1.0f;
				inView = false;
			}
			else if (eyeDistance > Camera::MinHazeDistance)
			{
				Camera::HazeFactor = (eyeDistance - Camera::MinHazeDistance) * Camera::DistanceFactor;
				inView = true;
			}
			else
			{
				Camera::HazeFactor = 0.0f;
				inView = true;
			}
			
			//-----------------------------------------------------------------
			// If inside farClip plane, check if behind camera.
			// Find angle between lookVector of Camera and vector from camPos
			// to Target.  If angle is less then halfFOV, object is visible.
			if (inView)
			{
				Stuff::Vector3D Distance;
				Stuff::Point3D objPosition;
				Stuff::Point3D eyePosition(eye->getCameraOrigin());
				objPosition.x = -position.x;
				objPosition.y = position.z;
				objPosition.z = position.y;
		
				Distance.Subtract(objPosition,eyePosition);
				Distance.Normalize(Distance);
				
				float cosine = Distance * eye->getLookVector();
 				if (cosine > eye->cosHalfFOV)
					inView = true;
				else
					inView = false;
			}
		}
		else
		{
			Camera::HazeFactor = 0.0f;
			inView = true;
		}
	}

	
	return(inView);
}

void BuildingLink::render()
{
	Stuff::Vector4D parentScreen;
	eye->projectZ( parent.pos, parentScreen );

	Stuff::Vector4D childScreen;
	
	for ( EList< Info, const Info& >::EConstIterator iter = children.Begin();
	!iter.IsDone(); iter++ )
	{
		/* Rather than doing true clipping, the link lines are broken into "typical object"
		sized pieces and rendered using course culling and relying on guard band clipping
		in the fashion that buildings are rendered. */
		Stuff::Vector3D diffVect;
		diffVect.Subtract((*iter).pos, parent.pos);
		float lineLength = diffVect.GetLength();
		const float lengthOfATypicalLargeObject = 150.0/*arbitrary*/;
		int numSegments = lineLength / lengthOfATypicalLargeObject;
		if (1 > numSegments)
		{
			numSegments = 1;
		}
		float segmentLength = lineLength / numSegments;
		Stuff::Vector3D segVect = diffVect;
		segVect.Normalize(segVect);
		segVect *= segmentLength;

		Stuff::Point3D p1(parent.pos.x, parent.pos.y, parent.pos.z);
		bool p1IsInView = isInView(p1);
		int i;
		for (i = 0 ; i < numSegments; i += 1)
		{
			Stuff::Point3D p2 = p1;
			p2 += segVect;
			bool p2IsInView = isInView(p2);

			if (p1IsInView || p2IsInView)
			{
				Stuff::Vector4D screenPos1;
				eye->projectZ( p1, screenPos1 );
				Stuff::Vector4D screenPos2;
				eye->projectZ( p2, screenPos2 );

				Stuff::Vector4D vertices[2];
				vertices[0].x = screenPos1.x;
				vertices[0].y = screenPos1.y;
				vertices[1].x = screenPos2.x;
				vertices[1].y = screenPos2.y;

				vertices[0].z = vertices[1].z = 0.1f;
				vertices[0].w = vertices[1].w = 0.9999f;

				LineElement elem( vertices[0], vertices[1], 0xffff0000, 0, -1 );
				elem.draw();
			}

			p1 = p2;
			p1IsInView = p2IsInView;
		}

	}
	
}

void	BuildingLink::SetParentAlignment( int alignment )
{
	EditorObject* pTmp = EditorObjectMgr::instance()->getObjectAtLocation( parent.pos.x, parent.pos.y );

	if ( pTmp )
		pTmp->setAlignment( alignment ) ;
	else
	{
		gosASSERT(false);
	}

	for ( EList<Info, const Info& >::EConstIterator iter = children.Begin(); 
		!iter.IsDone(); iter++ )
		{
			pTmp = EditorObjectMgr::instance()->getObjectAtLocation( (*iter).pos.x, (*iter).pos.y ) ;
			if ( pTmp )
				pTmp->setAlignment( alignment );
			else
			{
				gosASSERT(false);
			}
		}
}

void	BuildingLink::FixHeights()
{
	parent.pos.z = land->getTerrainElevation( parent.pos );

	for ( EList<Info, const Info& >::EIterator iter = children.Begin(); 
		!iter.IsDone(); iter++ )
	{
		(*iter).pos.z = land->getTerrainElevation((*iter).pos );
	}
}
void	BuildingLink::SetParentPosition( const Stuff::Vector3D& pos )
{
	parent.pos = pos;
}




