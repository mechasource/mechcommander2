
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
	int				init( CSVFile* fileName, int chassisID );
	void			setFitID( int id ){ fitID = id; }
	int				getNameID() const { return chassisNameID; }
	int				getEncyclopediaID() const { return encyclopediaID; }
	const EString& getFileName() const { return fileName; }
	int				getHouseID() const{ return houseID; }
	float			getMaxWeight() const{ return maxWeight; }
	const EString&	getMechClass() const  { return mechClass; }
	int32_t			getArmor() const { return baseArmor; }
	int				getArmorClass() const;
	float			getSpeed() const { return speed; }
	int				getDisplaySpeed() const;
	void			setScale( float newScale ) { scale = newScale; }
	float			getScale() const { return scale; }
	int32_t			getMaxArmor() const { return maxArmor; }
	int				getSensorID() const;
	int				getECM() const;
	bool			jumpJetsAllowed() const { return canHaveJumpJets; }

protected:

	int					refCount;
	
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

	static int			weightClasses[]; // mech weights

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
		
		int init( CSVFile* fileName, LogisticsChassis*, int blockID );

		int		compareWeight( LogisticsVariant* );
		
		inline bool			isAvailable() const { return (availableToUser && !bHidden); }
		ULONG	getID() const { return ID; }
		float			getMaxWeight() const { return chassis->maxWeight; }
		int32_t			getChassisName() const { return chassis->chassisNameID; }
		const EString&		getName() const { return variantName; }
		int32_t			getEncyclopediaID() const { return chassis->encyclopediaID; }
		int32_t			getHelpID() const { return chassis->helpID; }
		int32_t			getBaseCost() const { return chassis->baseCost; }
		int32_t			getComponentCount() const { return componentCount; }
		int				canAddComponent( LogisticsComponent* pComponent, int32_t& x, int32_t& y ) const;
		int				getCost() const;
		int				getWeight() const;
		const EString&		getMechClass() const;
		int				getChassisID() const { return chassis->ID; }
		int				getArmor() const;
		int				getArmorClass() { return chassis->getArmorClass(); }
		int				getJumpRange() const;
		int				getMaxJumpRange() const;
		int				getSpeed() const;
		int				getDisplaySpeed() const;

		int				getVariantID() const { return ID & 0x00ffffff; }
		const EString&		getSmallIconFileName() const { return chassis->iconFileNames[0]; }
		const EString&	getMediumIconFileName() const { return chassis->iconFileNames[1]; }
		const EString&		getLargeIconFileName() const { return chassis->iconFileNames[2]; }
		int				getComponentsWithLocation( int32_t& count, int32_t* IDArray, int32_t* xLocationArray, int32_t* yLocationArray );
		int				removeComponent( int32_t xCoord, int32_t yCoord );
		int				addComponent( LogisticsComponent*, int32_t& xCoord, int32_t& yCoord );
		inline			bool canDelete( ) const { return !isDesignerMech(); }
		inline			bool isDesignerMech() const { return bDesignerMech; }
		int				getComponents( int32_t& count, int32_t* array );
		int				getComponents( int32_t& count, LogisticsComponent** array );

		int				getHeat() const;
		int				getMaxHeat() const;
		LogisticsVariant& operator=( const LogisticsVariant&  );

		bool			operator==( const LogisticsVariant& src );
		const EString&		getFileName(){ return chassis->fileName; }

		int32_t			save( FitIniFile& file, int32_t counter );

		const LogisticsChassis* getChassis() const { return chassis; }
		bool			addComponent( int idFromFitFile, int32_t& x, int32_t& y );
		void			setName( PCSTR name ); // will allocate for you

		void		setAvailable( bool available );

		int			getIconIndex() const { return chassis->iconPictureIndex; }

		int			getComponentAreaWidth() const{ return chassis->componentAreaWidth; } 
		int			getComponentAreaHeight() const { return chassis->componentAreaHeight; } 

		LogisticsComponent*	getCompAtLocation( int i, int j, int32_t& realI, int32_t& realJ );
		int			getComponentLocation( LogisticsComponent* pComp, int32_t& i, int32_t& j );

		int			getOptimalRangeString( int32_t& color ) const;
		int32_t		getHouseID(){ return chassis->houseID; }
		int32_t		getMaxArmor() const { return chassis->maxArmor; }
		int			getSensorID() const { return chassis->getSensorID(); }
		int			getECM() const { return chassis->getECM(); }

		bool		allComponentsAvailable() const;

		int			getFileID() const { return fileID; }



		
	protected:

	
		const LogisticsChassis::ComponentInfo* getComponentAtLocation(int32_t x, int32_t y) const;

		
		ULONG		ID; // bottom 8 bits = mech chassis, next 8 = number within
		int					fileID; // in csv file
		
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
	int  getComponents( int32_t& count, LogisticsComponent** array );

protected:
	int32_t				componentCount;
	ComponentInfo		components[54]; // I count a max of 54 components, might want to check though

};


//*************************************************************************************************
#endif  // end of file ( LogisticsVariant.h )
