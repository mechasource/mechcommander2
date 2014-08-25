
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

#include <estring.h>

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
	int32_t				getNameID(void) const { return chassisNameID; }
	int32_t				getEncyclopediaID(void) const { return encyclopediaID; }
	const EString& getFileName(void) const { return fileName; }
	int32_t				getHouseID(void) const{ return houseID; }
	float			getMaxWeight(void) const{ return maxWeight; }
	const EString&	getMechClass(void) const  { return mechClass; }
	int32_t			getArmor(void) const { return baseArmor; }
	int32_t				getArmorClass(void) const;
	float			getSpeed(void) const { return speed; }
	int32_t				getDisplaySpeed(void) const;
	void			setScale( float newScale ) { scale = newScale; }
	float			getScale(void) const { return scale; }
	int32_t			getMaxArmor(void) const { return maxArmor; }
	int32_t				getSensorID(void) const;
	int32_t				getECM(void) const;
	bool			jumpJetsAllowed(void) const { return canHaveJumpJets; }

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

		LogisticsVariant(void);
		LogisticsVariant( const LogisticsChassis* pChassis, bool isDesignerMech );
		LogisticsVariant( const LogisticsVariant&  ); // going to need copy c'tor
		virtual ~LogisticsVariant(void);
		
		int32_t init( CSVFile* fileName, LogisticsChassis*, int32_t blockID );

		int32_t		compareWeight( LogisticsVariant* );
		
		inline bool			isAvailable(void) const { return (availableToUser && !bHidden); }
		uint32_t	getID(void) const { return ID; }
		float			getMaxWeight(void) const { return chassis->maxWeight; }
		int32_t			getChassisName(void) const { return chassis->chassisNameID; }
		const EString&		getName(void) const { return variantName; }
		int32_t			getEncyclopediaID(void) const { return chassis->encyclopediaID; }
		int32_t			getHelpID(void) const { return chassis->helpID; }
		int32_t			getBaseCost(void) const { return chassis->baseCost; }
		int32_t			getComponentCount(void) const { return componentCount; }
		int32_t				canAddComponent( LogisticsComponent* pComponent, int32_t& x, int32_t& y ) const;
		int32_t				getCost(void) const;
		int32_t				getWeight(void) const;
		const EString&		getMechClass(void) const;
		int32_t				getChassisID(void) const { return chassis->ID; }
		int32_t				getArmor(void) const;
		int32_t				getArmorClass() { return chassis->getArmorClass(void); }
		int32_t				getJumpRange(void) const;
		int32_t				getMaxJumpRange(void) const;
		int32_t				getSpeed(void) const;
		int32_t				getDisplaySpeed(void) const;

		int32_t				getVariantID(void) const { return ID & 0x00ffffff; }
		const EString&		getSmallIconFileName(void) const { return chassis->iconFileNames[0]; }
		const EString&	getMediumIconFileName(void) const { return chassis->iconFileNames[1]; }
		const EString&		getLargeIconFileName(void) const { return chassis->iconFileNames[2]; }
		int32_t				getComponentsWithLocation( int32_t& count, int32_t* IDArray, int32_t* xLocationArray, int32_t* yLocationArray );
		int32_t				removeComponent( int32_t xCoord, int32_t yCoord );
		int32_t				addComponent( LogisticsComponent*, int32_t& xCoord, int32_t& yCoord );
		inline			bool canDelete( ) const { return !isDesignerMech(void); }
		inline			bool isDesignerMech(void) const { return bDesignerMech; }
		int32_t				getComponents( int32_t& count, int32_t* array );
		int32_t				getComponents( int32_t& count, LogisticsComponent** array );

		int32_t				getHeat(void) const;
		int32_t				getMaxHeat(void) const;
		LogisticsVariant& operator=( const LogisticsVariant&  );

		bool			operator==( const LogisticsVariant& src );
		const EString&		getFileName(){ return chassis->fileName; }

		int32_t			save( FitIniFile& file, int32_t counter );

		const LogisticsChassis* getChassis(void) const { return chassis; }
		bool			addComponent( int32_t idFromFitFile, int32_t& x, int32_t& y );
		void			setName( PCSTR name ); // will allocate for you

		void		setAvailable( bool available );

		int32_t			getIconIndex(void) const { return chassis->iconPictureIndex; }

		int32_t			getComponentAreaWidth(void) const{ return chassis->componentAreaWidth; } 
		int32_t			getComponentAreaHeight(void) const { return chassis->componentAreaHeight; } 

		LogisticsComponent*	getCompAtLocation( int32_t i, int32_t j, int32_t& realI, int32_t& realJ );
		int32_t			getComponentLocation( LogisticsComponent* pComp, int32_t& i, int32_t& j );

		int32_t			getOptimalRangeString( int32_t& color ) const;
		int32_t		getHouseID(){ return chassis->houseID; }
		int32_t		getMaxArmor(void) const { return chassis->maxArmor; }
		int32_t			getSensorID(void) const { return chassis->getSensorID(void); }
		int32_t			getECM(void) const { return chassis->getECM(void); }

		bool		allComponentsAvailable(void) const;

		int32_t			getFileID(void) const { return fileID; }



		
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
		bool				hasJumpJets(void) const;
		bool				hasECM(void) const;
		bool				hasSensor(void) const;

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
