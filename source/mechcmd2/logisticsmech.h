//===========================================================================//
//LogisticsMech.h			: Interface for the LogisticsMech component.     //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef LOGISTICSMECH_H
#define LOGISTICSMECH_H

//#include "logisticsvariant.h"

class LogisticsPilot;

class LogisticsMech
{
	public:

		LogisticsMech( LogisticsVariant*, int count );
		~LogisticsMech();

		inline int getID(){ return ID; }
		inline int getInstanceID() { return ID >> 24; }
		void	setPilot( LogisticsPilot* pPilot );
		inline LogisticsPilot* getPilot( ) { return pilot; }
		inline int getForceGroup() { return forceGroup; }
		inline void setForceGroup( int bInForceGroup ) { forceGroup = bInForceGroup; }
		inline PCSTR getFileName(){ return pVariant->chassis->fileName; }
		inline bool		isAvailable() const { return pVariant->availableToUser; }
		ULONG	getID() const { return pVariant->ID; }
		float			getMaxWeight() const { return pVariant->chassis->maxWeight; }
		int32_t			getChassisName() const { return pVariant->chassis->chassisNameID; }
		const EString&		getName() const { return pVariant->variantName; }
		int32_t			getEncyclopediaID() const { return pVariant->chassis->encyclopediaID; }
		int32_t			getHelpID() const { return pVariant->chassis->helpID; }
		int32_t			getBaseCost() const { return pVariant->chassis->baseCost; }
		int32_t			getComponentCount() const { return pVariant->componentCount; }
		int				canAddComponent( LogisticsComponent* pComponent, int32_t x, int32_t y ) const
		{ return pVariant->canAddComponent( pComponent, x, y ); }
		int				getCost() const { return pVariant->getCost(); }
		int				getWeight() const { return pVariant->getWeight(); }
		PCSTR		getMechClass() const { return pVariant->getMechClass(); }
		int				getChassisID() const { return pVariant->chassis->ID; }
		int				getArmor() const { return pVariant->getArmor(); }
		int				getJumpRange() const { return pVariant->getJumpRange(); }
		int				getSpeed() const { return pVariant->getSpeed(); }
		int				getDisplaySpeed() const { return pVariant->getDisplaySpeed(); }
		int				getVariantID() const { return pVariant->ID; }
		const EString&		getSmallIconFileName() const { return pVariant->chassis->iconFileNames[0]; }
		const EString&		getMediumIconFileName() const { return pVariant->chassis->iconFileNames[1]; }
		const EString&		getLargeIconFileName() const { return pVariant->chassis->iconFileNames[2]; }
		int				getComponentsWithLocation( int32_t& count, int32_t* IDArray, int32_t* xLocationArray, int32_t* yLocationArray )
		{
			return pVariant->getComponentsWithLocation( count, IDArray, xLocationArray, yLocationArray );
		}

		int				getComponents( int32_t& count, int32_t* IDArray )
		{
			return pVariant->getComponents( count, IDArray );
		}

		void			setVariant( LogisticsVariant* pVariant );

		int				getFitID() const { return pVariant->chassis->fitID; }
		int32_t			save( FitIniFile& file, int32_t count );

		void			setColors( ULONG base, ULONG high1, ULONG high2 );
		void			getColors( ULONG& base, ULONG& high1, ULONG& high2 ) const;
		int				getIconIndex() const { return pVariant->chassis->iconPictureIndex; }

		LogisticsVariant* getVariant() { return pVariant; }

	

	private:
		LogisticsVariant* pVariant;

		ULONG	pilotID;
		int				forceGroup; // 0 if none, 1 if otherwise
		LogisticsPilot*	pilot;
		int				ID;
		int32_t			baseColor;
		int32_t			highlightColor1;
		int32_t			highlightColor2;

		// DATA

		// HELPER FUNCTIONS

		// suppress these puppies
		LogisticsMech();
		LogisticsMech& operator=( const LogisticsMech& );

};


//*************************************************************************************************
#endif  // end of file ( LogisticsMech.h )
