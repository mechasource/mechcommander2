#ifndef DAMAGEBRUSH_H
#define DAMAGEBRUSH_H
/*************************************************************************************************\
DamageBrush.h			: Interface for the DamageBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef BRUSH_H
#include "Brush.h"
#endif

#include "Action.h"
#include "EditorObjects.h"



//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
DamageBrush:
**************************************************************************************************/
class DamageBrush: public Brush
{
	public:
	
	DamageBrush(bool bDamage ){ damage = bDamage; pAction = NULL; }
	virtual ~DamageBrush(){}
	virtual bool beginPaint();
	virtual Action* endPaint();
	virtual bool paint( Stuff::Vector3D& worldPos, int screenX, int screenY  );
	virtual bool canPaint( Stuff::Vector3D& worldPos, int screenX, int screenY, int flags ); 
	virtual bool canPaintSelection( );
	virtual Action* applyToSelection();
	bool	damage;

	private:
		ModifyBuildingAction* pAction;
};


//*************************************************************************************************
#endif  // end of file ( DamageBrush.h )
