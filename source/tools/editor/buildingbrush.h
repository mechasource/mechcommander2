/*************************************************************************************************\
BuildingBrush.h		: Interface for the BuildingBrush component. The thing you use to paint 
						buildings
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef BUILDINGBRUSH_H
#define BUILDINGBRUSH_H

#include "brush.h"
#include "action.h"
//#include "editorobjects.h"

namespace Stuff {
	class Vector3D;
}

class Action;
class EditorObject;
class ObjectAppearance;

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
BuildingBrush:
**************************************************************************************************/
class BuildingBrush: public Brush
{
	public:

		BuildingBrush( int32_t group, int32_t indexInGroup, int32_t Alignment );
		virtual ~BuildingBrush(void);

		virtual bool beginPaint(void);
		virtual Action* endPaint(void);
		virtual bool paint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY );
		virtual bool canPaint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags );
		virtual void render( int32_t ScreenMouseX, int32_t ScreenMouseY);
		virtual void update( int32_t screenX, int32_t screenY );
		void rotateBrush( int32_t direction );

		
		class BuildingAction : public Action
		{
		public:
			
			virtual ~BuildingAction(void){}
			virtual bool redo(void);
			virtual bool undo(void);
			virtual void addBuildingInfo(EditorObject& info);

#if _CONSIDERED_OBSOLETE
			class OBJ_INFO_PTR_LIST : 
				public EList<EditorObject *, EditorObject *> {
			public:
				~OBJ_INFO_PTR_LIST(void) {
					EIterator it;
					for (it = Begin(void); !it.IsDone(void); it++) {
						delete (*it);
					}
				}
			};
#endif
		private:
			typedef std::list<EditorObject*> OBJ_INFO_PTR_LIST;
			OBJ_INFO_PTR_LIST objInfoPtrList;
		};

		protected:

		// suppression
		BuildingBrush( const BuildingBrush& buildingBrush );
		BuildingBrush& operator=( const BuildingBrush& buildingBrush );
		BuildingBrush(void);

		int32_t group;
		int32_t indexInGroup;
		float curRotation;

		BuildingAction*		pAction;
		ObjectAppearance*	pCursor;
		int32_t				alignment;
};


//*************************************************************************************************
#endif  // end of file ( BuildingBrush.h )
