#define LOGISTICSMECH_CPP
/*************************************************************************************************\
LogisticsMech.cpp			: Implementation of the LogisticsMech component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "LogisticsMech.h"
#include "LogisticsData.h"
#include "IniFile.h"

LogisticsMech::LogisticsMech(LogisticsVariant* pVar, int32_t Count)
	: pVariant(pVar)
{
	ID = (Count << 24 | pVar->getID());
	pilot = 0;
	forceGroup = 0;
}

LogisticsMech::~LogisticsMech()
{
	if (pilot)
		pilot->setUsed(0);
	pilot = nullptr;
}

void LogisticsMech::setPilot(LogisticsPilot* pPilot)
{
	if (pilot)
		pilot->setUsed(0);
	pilot = pPilot;
	if (pilot)
		pilot->setUsed(1);
}

void LogisticsMech::setVariant(LogisticsVariant* pVar)
{
	pVariant = pVar;
	ID = (LogisticsData::instance->createInstanceID(pVar) << 24 | pVar->getID());
}

int32_t
LogisticsMech::save(FitIniFile& file, int32_t counter)
{
	wchar_t tmp[256];
	sprintf(tmp, "Inventory%ld", counter);
	file.writeBlock(tmp);
	file.writeIdString("Chassis", pVariant->getFileName());
	file.writeIdString("Variant", pVariant->getName());
	if (pilot)
		file.writeIdString("Pilot", pilot->getFileName());
	file.writeIdBoolean("Designer", pVariant->isDesignerMech());
	return 0;
}

void LogisticsMech::setcolours(uint32_t base, uint32_t high1, uint32_t high2)
{
	basecolour = base;
	highlightcolour1 = high1;
	highlightcolour2 = high2;
}

void LogisticsMech::getcolours(uint32_t& base, uint32_t& high1, uint32_t& high2) const
{
	base = basecolour;
	high1 = highlightcolour1;
	high2 = highlightcolour2;
}

// end of file ( LogisticsMech.cpp )
