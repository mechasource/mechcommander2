/*************************************************************************************************\
Eraser.h			: Interface for the Eraser component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef ERASER_H
#define ERASER_H

//#include "brush.h"
//#include <elist.h>
//#include "action.h"
//#include "buildingbrush.h"
//#include "linkbrush.h"
//#include "editorinterface.h" /*only for the definition of teamsaction*/

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
Eraser:
**************************************************************************************************/

class Eraser: public Brush
{
	public:

		Eraser(){ pCurAction = NULL; }

		virtual bool beginPaint();
		virtual Action* endPaint();
		virtual bool paint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY );
		virtual bool canPaint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags );
		virtual Action* applyToSelection();


	private:

		class EraserAction : public ActionPaintTile
		{
		public:
			EraserAction() { teamsActionIsSet = false; }

			virtual bool undo();
			virtual bool redo();

			BuildingBrush::BuildingAction bldgAction;
			LinkBrush::LinkAction linkAction;
			/*deleting a building that is referred to by an objective condition requires a TeamAction
			to record the objective change*/
			bool teamsActionIsSet;
			TeamsAction teamsAction;

		};

		EraserAction* pCurAction;
		
};


//*************************************************************************************************
#endif  // end of file ( Eraser.h )
