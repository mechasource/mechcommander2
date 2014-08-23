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

		LogisticsMech( LogisticsVariant*, int32_t count );
		~LogisticsMech();

		inline int32_t getID(){ return ID; }
		inline int32_t getInstanceID() { return ID >> 24; }
		void	setPilot( LogisticsPilot* pPilot );
		inline LogisticsPilot* getPilot( ) { return pilot; }
		inline int32_t getForceGroup() { return forceGroup; }
		inline void setForceGroup( int32_t bInForceGroup ) { forceGroup = bInForceGroup; }
		inline PCSTR getFileName(){ return pVariant->chassis->fileName; }
		inline bool		isAvailable() const { return pVariant->availableToUser; }
		uint32_t	getID() const { return pVariant->ID; }
		float			getMaxWeight() const { return pVariant->chassis->maxWeight; }
		int32_t			getChassisName() const { return pVariant->chassis->chassisNameID; }
		const EString&		getName() const { return pVariant->variantName; }
		int32_t			getEncyclopediaID() const { return pVariant->chassis->encyclopediaID; }
		int32_t			getHelpID() const { return pVariant->chassis->helpID; }
		int32_t			getBaseCost() const { return pVariant->chassis->baseCost; }
		int32_t			getComponentCount() const { return pVariant->componentCount; }
		int32_t				canAddComponent( LogisticsComponent* pComponent, int32_t x, int32_t y ) const
		{ return pVariant->canAddComponent( pComponent, x, y ); }
		int32_t				getCost() const { return pVariant->getCost(); }
		int32_t				getWeight() const { return pVariant->getWeight(); }
		PCSTR		getMechClass() const { return pVariant->getMechClass(); }
		int32_t				getChassisID() const { return pVariant->chassis->ID; }
		int32_t				getArmor() const { return pVariant->getArmor(); }
		int32_t				getJumpRange() const { return pVariant->getJumpRange(); }
		int32_t				getSpeed() const { return pVariant->getSpeed(); }
		int32_t				getDisplaySpeed() const { return pVariant->getDisplaySpeed(); }
		int32_t				getVariantID() const { return pVariant->ID; }
		const EString&		getSmallIconFileName() const { return pVariant->chassis->iconFileNames[0]; }
		const EString&		getMediumIconFileName() const { return pVariant->chassis->iconFileNames[1]; }
		const EString&		getLargeIconFileName() const { return pVariant->chassis->iconFileNames[2]; }
		int32_t				getComponentsWithLocation( int32_t& count, int32_t* IDArray, int32_t* xLocationArray, int32_t* yLocationArray )
		{
			return pVariant->getComponentsWithLocation( count, IDArray, xLocationArray, yLocationArray );
		}

		int32_t				getComponents( int32_t& count, int32_t* IDArray )
		{
			return pVariant->getComponents( count, IDArray );
		}

		void			setVariant( LogisticsVariant* pVariant );

		int32_t				getFitID() const { return pVariant->chassis->fitID; }
		int32_t			save( FitIniFile& file, int32_t count );

		void			setColors( uint32_t base, uint32_t high1, uint32_t high2 );
		void			getColors( uint32_t& base, uint32_t& high1, uint32_t& high2 ) const;
		int32_t				getIconIndex() const { return pVariant->chassis->iconPictureIndex; }

		LogisticsVariant* getVariant() { return pVariant; }

	

	private:
		LogisticsVariant* pVariant;

		uint32_t	pilotID;
		int32_t				forceGroup; // 0 if none, 1 if otherwise
		LogisticsPilot*	pilot;
		int32_t				ID;
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
