
#pragma once

#ifndef LOGISTICSVARIANT_H
#define LOGISTICSVARIANT_H
/*************************************************************************************************\
LogisticsVariant.h			: Interface for the LogisticsVariant component.  This thing
					is a simplified mech/vehicle object without AI.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

// forward declarations
class MechAppearance;
class LogisticsComponent;
class CSVFile;
class FitIniFile;

#include "estring.h"

//#pragma warning (disable:4514)


enum LOGISTICS_COMPONENT_LOCATION
{
	HEAD = 0,
	CHEST = 1,
	LEGS = 2
};


class LogisticsChassis
{
public:

	LogisticsChassis( );
	int32_t				init( CSVFile* fileName, int32_t chassisID );
	void			setFitID( int32_t id ){ fitID = id; }
	int32_t				getNameID() const { return chassisNameID; }
	int32_t				getEncyclopediaID() const { return encyclopediaID; }
	const EString& getFileName() const { return fileName; }
	int32_t				getHouseID() const{ return houseID; }
	float			getMaxWeight() const{ return maxWeight; }
	const EString&	getMechClass() const  { return mechClass; }
	int32_t			getArmor() const { return baseArmor; }
	int32_t				getArmorClass() const;
	float			getSpeed() const { return speed; }
	int32_t				getDisplaySpeed() const;
	void			setScale( float newScale ) { scale = newScale; }
	float			getScale() const { return scale; }
	int32_t			getMaxArmor() const { return maxArmor; }
	int32_t				getSensorID() const;
	int32_t				getECM() const;
	bool			jumpJetsAllowed() const { return canHaveJumpJets; }

protected:

	int32_t					refCount;
	
	float				maxWeight;
	int32_t				chassisNameID;	// resource ID
	int32_t				baseCost;
	int32_t				maxHeat;
	int32_t				baseArmor;
	int32_t				maxArmor;

	float					speed;
	float				scale;

	int32_t				iconPictureIndex;
	int32_t				ID;
	EString				fileName;
	EString				mechClass;
	EString				iconFileNames[3];

	int32_t				componentAreaWidth;
	int32_t				componentAreaHeight;

	bool canHaveAdvSensor;//can Mech Have Advanced Sensor installed.
	bool canHaveOptics;//can Mech Have Optics Installed.
	bool canHaveGlobalSensor;//can Mech have Global Sensor installed.
	bool canHaveECM;//can Mech have ECM Installed.
	bool canHaveActiveProbe;//can Mech have Active Probe Installed.
	bool canHaveJumpJets;
	bool canHaveExtendedSensor;

	int32_t				encyclopediaID;
	int32_t				helpID;
	int32_t				houseID;

	int32_t				fitID;

	MechAppearance*		appearance; 

	friend class		LogisticsVariant;
	friend class		LogisticsMech;

	static int32_t			weightClasses[]; // mech weights

	friend class LogisticsData;

	struct ComponentInfo
	{
		int32_t										xCoord;
		int32_t										yCoord;
		LOGISTICS_COMPONENT_LOCATION location;
		LogisticsComponent*				component;
	};
	


};

class LogisticsVariant
{
protected:

	public:

		LogisticsVariant();
		LogisticsVariant( const LogisticsChassis* pChassis, bool isDesignerMech );
		LogisticsVariant( const LogisticsVariant&  ); // going to need copy c'tor
		virtual ~LogisticsVariant();
		
		int32_t init( CSVFile* fileName, LogisticsChassis*, int32_t blockID );

		int32_t		compareWeight( LogisticsVariant* );
		
		inline bool			isAvailable() const { return (availableToUser && !bHidden); }
		uint32_t	getID() const { return ID; }
		float			getMaxWeight() const { return chassis->maxWeight; }
		int32_t			getChassisName() const { return chassis->chassisNameID; }
		const EString&		getName() const { return variantName; }
		int32_t			getEncyclopediaID() const { return chassis->encyclopediaID; }
		int32_t			getHelpID() const { return chassis->helpID; }
		int32_t			getBaseCost() const { return chassis->baseCost; }
		int32_t			getComponentCount() const { return componentCount; }
		int32_t				canAddComponent( LogisticsComponent* pComponent, int32_t& x, int32_t& y ) const;
		int32_t				getCost() const;
		int32_t				getWeight() const;
		const EString&		getMechClass() const;
		int32_t				getChassisID() const { return chassis->ID; }
		int32_t				getArmor() const;
		int32_t				getArmorClass() { return chassis->getArmorClass(); }
		int32_t				getJumpRange() const;
		int32_t				getMaxJumpRange() const;
		int32_t				getSpeed() const;
		int32_t				getDisplaySpeed() const;

		int32_t				getVariantID() const { return ID & 0x00ffffff; }
		const EString&		getSmallIconFileName() const { return chassis->iconFileNames[0]; }
		const EString&	getMediumIconFileName() const { return chassis->iconFileNames[1]; }
		const EString&		getLargeIconFileName() const { return chassis->iconFileNames[2]; }
		int32_t				getComponentsWithLocation( int32_t& count, int32_t* IDArray, int32_t* xLocationArray, int32_t* yLocationArray );
		int32_t				removeComponent( int32_t xCoord, int32_t yCoord );
		int32_t				addComponent( LogisticsComponent*, int32_t& xCoord, int32_t& yCoord );
		inline			bool canDelete( ) const { return !isDesignerMech(); }
		inline			bool isDesignerMech() const { return bDesignerMech; }
		int32_t				getComponents( int32_t& count, int32_t* array );
		int32_t				getComponents( int32_t& count, LogisticsComponent** array );

		int32_t				getHeat() const;
		int32_t				getMaxHeat() const;
		LogisticsVariant& operator=( const LogisticsVariant&  );

		bool			operator==( const LogisticsVariant& src );
		const EString&		getFileName(){ return chassis->fileName; }

		int32_t			save( FitIniFile& file, int32_t counter );

		const LogisticsChassis* getChassis() const { return chassis; }
		bool			addComponent( int32_t idFromFitFile, int32_t& x, int32_t& y );
		void			setName( PCSTR name ); // will allocate for you

		void		setAvailable( bool available );

		int32_t			getIconIndex() const { return chassis->iconPictureIndex; }

		int32_t			getComponentAreaWidth() const{ return chassis->componentAreaWidth; } 
		int32_t			getComponentAreaHeight() const { return chassis->componentAreaHeight; } 

		LogisticsComponent*	getCompAtLocation( int32_t i, int32_t j, int32_t& realI, int32_t& realJ );
		int32_t			getComponentLocation( LogisticsComponent* pComp, int32_t& i, int32_t& j );

		int32_t			getOptimalRangeString( int32_t& color ) const;
		int32_t		getHouseID(){ return chassis->houseID; }
		int32_t		getMaxArmor() const { return chassis->maxArmor; }
		int32_t			getSensorID() const { return chassis->getSensorID(); }
		int32_t			getECM() const { return chassis->getECM(); }

		bool		allComponentsAvailable() const;

		int32_t			getFileID() const { return fileID; }



		
	protected:

	
		const LogisticsChassis::ComponentInfo* getComponentAtLocation(int32_t x, int32_t y) const;

		
		uint32_t		ID; // bottom 8 bits = mech chassis, next 8 = number within
		int32_t					fileID; // in csv file
		
		LogisticsChassis*	chassis;
		EString				variantName;	// in file
		
		int32_t				componentCount;
		LogisticsChassis::ComponentInfo		components[54]; // I count a max of 54 components, might want to check though

		bool				availableToUser;
		bool				bDesignerMech;
		bool				bHidden;


		// HELPERS
		bool				hasJumpJets() const;
		bool				hasECM() const;
		bool				hasSensor() const;

		// SUPPRESSED
		
		friend class LogisticsMech;


};

class LogisticsVehicle : public LogisticsChassis
{

public:
	void init( FitIniFile& file );
	int32_t  getComponents( int32_t& count, LogisticsComponent** array );

protected:
	int32_t				componentCount;
	ComponentInfo		components[54]; // I count a max of 54 components, might want to check though

};


//*************************************************************************************************
#endif  // end of file ( LogisticsVariant.h )
