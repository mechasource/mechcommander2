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

//#include "brush.h"
//#include "action.h"
//#include "editorobjects.h"

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
BuildingBrush:
**************************************************************************************************/
class BuildingBrush: public Brush
{
	public:

		BuildingBrush( int32_t group, int32_t indexInGroup, int32_t Alignment );
		virtual ~BuildingBrush();

		virtual bool beginPaint();
		virtual Action* endPaint();
		virtual bool paint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY );
		virtual bool canPaint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags );
		virtual void render( int32_t ScreenMouseX, int32_t ScreenMouseY);
		virtual void update( int32_t screenX, int32_t screenY );
		void rotateBrush( int32_t direction );

		
		class BuildingAction : public Action
		{
		public:
			
			virtual ~BuildingAction(){}
			virtual bool redo();
			virtual bool undo();
			virtual void addBuildingInfo(EditorObject& info);

			class OBJ_INFO_PTR_LIST : public EList<EditorObject *, EditorObject *> {
			public:
				~OBJ_INFO_PTR_LIST() {
					EIterator it;
					for (it = Begin(); !it.IsDone(); it++) {
						delete (*it);
					}
				}
			};
			
			OBJ_INFO_PTR_LIST objInfoPtrList;
		};

		protected:

		// suppression
		BuildingBrush( const BuildingBrush& buildingBrush );
		BuildingBrush& operator=( const BuildingBrush& buildingBrush );
		BuildingBrush();

		int32_t group;
		int32_t indexInGroup;
		float curRotation;

		BuildingAction*		pAction;

		ObjectAppearance*	pCursor;

		int32_t					alignment;
};


//*************************************************************************************************
#endif  // end of file ( BuildingBrush.h )
