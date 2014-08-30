//-------------------------------------------------------------------------------
//
// Multiple TG Shape Layer
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// Replace MLR for a myriad number of reasons.
//
// Started 4/16/99
//
// FFS
//
//-------------------------------------------------------------------------------

#include "stdafx.h"

//-------------------------------------------------------------------------------
// Include Files
#ifndef MSL_H
#include "msl.h"
#endif

#ifndef CIDENT_H
#include "cident.h"
#endif

#ifndef PATHS_H
#include "paths.h"
#endif

#ifndef MATHFUNC_H
#include "mathfunc.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef CELINE_H
#include "celine.h"
#endif

#ifndef DBASEGUI_H
#include "dbasegui.h"
#endif

#ifndef TIMING_H
#include "timing.h"
#endif

#include <toolOS.hpp>

#include "../ARM/Microsoft.Xna.Arm.h"
using namespace Microsoft::Xna::Arm;

//-------------------------------------------------------------------------------
extern void GetNumberData (PSTR rawData, PSTR result);
extern void GetNameData (PSTR rawData, PSTR result);
extern void GetWordData (PSTR rawData, PSTR result);

extern int32_t		ObjectTextureSize;

#define CURRENT_SHAPE_VERSION		0xBAFDECAF
#define CURRENT_ANIM_VERSION		0xBADDECAF
//#define MAX_PATH					256

//-------------------------------------------------------------------------------
void GetNextLine (PSTR rawData, PSTR result)
{
	int32_t startIndex = 0;
	int32_t endIndex = 0;
	while (	(rawData[startIndex] != '\n') )
	{
		startIndex++;
	}

	startIndex++;
	endIndex = startIndex;
	while (	(rawData[endIndex] != '\n') )
	{
		endIndex++;
	}

	strncpy(result,&rawData[startIndex],endIndex - startIndex);
	result[endIndex-startIndex] = 0;
}
	
//-------------------------------------------------------------------------------
bool useSlerping = false;
extern bool useVertexLighting;
extern bool useFaceLighting;
bool useShadows = true;
extern bool silentMode;			//Used for automated builds to keep errors from popping up.

//-------------------------------------------------------------------------------
// TG_TypeMultiShape
PVOIDTG_TypeMultiShape::operator new (size_t mySize)
{
	PVOID result = TG_Shape::tglHeap->Malloc(mySize);
	return result;
}
 
//-------------------------------------------------------------------------------
 void TG_TypeMultiShape::operator delete (PVOID us)
{
	TG_Shape::tglHeap->Free(us);
}

//-------------------------------------------------------------------------------
// TG_MultiShape & TG_TypeMultiShape
//This function copies the entire multi-shape to the new pointer.
//It does malloc and the newShape is a completely independant copy
//That must be destroyed or memory will leak!
TG_MultiShapePtr TG_TypeMultiShape::CreateFrom (void)
{
	TG_MultiShapePtr newShape = nullptr;
	newShape = new TG_MultiShape;
	gosASSERT(newShape != nullptr);
	
#ifdef _DEBUG
	if (numTG_TypeShapes == 0)
		STOP(("Tried to create a shape with no data Named %s",shapeName));
#endif

	//listOfShapes
	newShape->numTG_Shapes = numTG_TypeShapes;
	newShape->listOfShapes = (TG_ShapeRecPtr)TG_Shape::tglHeap->Malloc(sizeof(TG_ShapeRec) * numTG_TypeShapes);
	gosASSERT(newShape->listOfShapes != nullptr);

	memset(newShape->listOfShapes,0,sizeof(TG_ShapeRec) * numTG_TypeShapes);

	int32_t i, j;
	for (i=0;i<numTG_TypeShapes;i++)
	{
		newShape->listOfShapes[i].node = listOfTypeShapes[i]->CreateFrom();
	}

	//Setup Heirarchy again because pointers are not valid!
	for (i=0;i<numTG_TypeShapes;i++)
	{
		newShape->listOfShapes[i].parentNode = nullptr;
		newShape->listOfShapes[i].currentAnimation = nullptr;
		newShape->listOfShapes[i].shapeToWorld = Stuff::LinearMatrix4D::Identity;
		newShape->listOfShapes[i].worldToShape = Stuff::LinearMatrix4D::Identity;
		newShape->listOfShapes[i].calcedThisFrame = -1;
		newShape->listOfShapes[i].processMe = true;
		newShape->listOfShapes[i].baseRotation.w = 1.0f;
		newShape->listOfShapes[i].baseRotation.x = 
		newShape->listOfShapes[i].baseRotation.y = 
		newShape->listOfShapes[i].baseRotation.z = 0.0f;

		//----------------------------------------------------------------------------------
		// For each shape, look for another node whose NodeId matches this shape's parentId
		for (j=0;j<numTG_TypeShapes;j++)
		{
			if (strcmp(newShape->listOfShapes[i].node->myType->getParentId(),newShape->listOfShapes[j].node->myType->getNodeId()) == 0)
			{
				//-------------------------------
				// Found it!
				newShape->listOfShapes[i].parentNode = &(newShape->listOfShapes[j]);
				break;
			}
		}

		//----------------------------------------------------------------------------
		// Do NOT need to reset nodeCenters as they are correct for each node copied!
	}

	newShape->frameNum = 0.0f;

	newShape->myMultiType = this;
	
	newShape->isHudElement = false;
	
	return newShape;
}	

//-------------------------------------------------------------------------------
int32_t TG_TypeMultiShape::LoadBinaryCopy (PSTR fileName)
{
	File binFile;
	int32_t result = binFile.open(fileName);
	if (result != NO_ERROR)
		return -1;
	
	uint32_t version = binFile.readLong();
	if (version == CURRENT_SHAPE_VERSION)
	{
		numTG_TypeShapes = binFile.readLong();
		numTextures = binFile.readLong();
	
		//ListOfTextures
		if (numTextures)
		{
			listOfTextures = (TG_TexturePtr)TG_Shape::tglHeap->Malloc(sizeof(TG_Texture) * numTextures);
			gosASSERT(listOfTextures != nullptr);
		
			binFile.read((puint8_t)listOfTextures,sizeof(TG_Texture) * numTextures);
		}
		else
		{
			listOfTextures = nullptr;
		}
	
		//listOfShapes
		listOfTypeShapes = (TG_TypeNodePtr *)TG_Shape::tglHeap->Malloc(sizeof(TG_TypeNodePtr) * numTG_TypeShapes);
		gosASSERT(listOfTypeShapes != nullptr);
	
		memset(listOfTypeShapes,0,sizeof(TG_TypeNodePtr) * numTG_TypeShapes);
	
		int32_t i, j;
		for (i=0;i<numTG_TypeShapes;i++)
		{
			int32_t nodeType = binFile.readLong();
			
			if (nodeType == SHAPE_NODE)
			{
				listOfTypeShapes[i] = new TG_TypeShape;
				listOfTypeShapes[i]->init();
				listOfTypeShapes[i]->LoadBinaryCopy(binFile);
			}
			else if (nodeType == TYPE_NODE)
			{
				listOfTypeShapes[i] = new TG_TypeNode;
				listOfTypeShapes[i]->init();
				listOfTypeShapes[i]->LoadBinaryCopy(binFile);
			}
		}
	
		//Setup Heirarchy again because pointers are not valid!
		for (i=0;i<numTG_TypeShapes;i++)
		{
			//----------------------------------------------------------------------------
			// Do NOT need to reset nodeCenters as they are correct for each node copied!
			
			//--------------------------------------------------------
			//ONLY use nodes which are not spotlight or _PAB or LOS_
			if ((strnicmp(listOfTypeShapes[i]->getNodeId(),"_PAB",4) != 0) &&
				(strnicmp(listOfTypeShapes[i]->getNodeId(),"LOS_",4) != 0) && 
				(strnicmp(listOfTypeShapes[i]->getNodeId(),"SpotLight",9) != 0))
			{
				//------------------------------------------------------------------
				// Scan list of vertices and create minBox, maxBox and extentRadius
				for (j=0;j<listOfTypeShapes[i]->GetNumTypeVertices();j++)
				{
					Stuff::Vector3D relNodeCenter;
					relNodeCenter = listOfTypeShapes[i]->GetRelativeNodeCenter();
					
					TG_TypeShapePtr typeShape = (TG_TypeShapePtr)listOfTypeShapes[i];
					
					if (minBox.x > typeShape->listOfTypeVertices[j].position.x + relNodeCenter.x)
						minBox.x = typeShape->listOfTypeVertices[j].position.x + relNodeCenter.x;
												 
					if (maxBox.x < typeShape->listOfTypeVertices[j].position.x + relNodeCenter.x)
						maxBox.x = typeShape->listOfTypeVertices[j].position.x + relNodeCenter.x;
												 
					if (minBox.y > typeShape->listOfTypeVertices[j].position.y + relNodeCenter.y)
						minBox.y = typeShape->listOfTypeVertices[j].position.y + relNodeCenter.y;
												 
					if (maxBox.y < typeShape->listOfTypeVertices[j].position.y + relNodeCenter.y)
						maxBox.y = typeShape->listOfTypeVertices[j].position.y + relNodeCenter.y;
												 
					if (minBox.z > typeShape->listOfTypeVertices[j].position.z + relNodeCenter.z)
						minBox.z = typeShape->listOfTypeVertices[j].position.z + relNodeCenter.z;
												 
					if (maxBox.z < typeShape->listOfTypeVertices[j].position.z + relNodeCenter.z)
						maxBox.z = typeShape->listOfTypeVertices[j].position.z + relNodeCenter.z;
				}
			}
		}
	
		float minBoxLength = minBox.GetLength();
		float maxBoxLength = maxBox.GetLength();
		if (minBoxLength > extentRadius)
			extentRadius = minBoxLength;
			
		if (maxBoxLength > extentRadius)
			extentRadius = maxBoxLength;
	}
	else
	{
		return -1;
	}

	return 0;
}	

//-------------------------------------------------------------------------------
void TG_TypeMultiShape::SaveBinaryCopy (PSTR fileName)
{
	File binFile;
	binFile.create(fileName);
	
	binFile.writeLong(CURRENT_SHAPE_VERSION);
	binFile.writeLong(numTG_TypeShapes);
	binFile.writeLong(numTextures);
	
	//ListOfTextures
	if (numTextures)
	{
		binFile.write((puint8_t)listOfTextures,sizeof(TG_Texture) * numTextures);
	}
	
	//listOfShapes
	for (int32_t i=0;i<numTG_TypeShapes;i++)
	{
		listOfTypeShapes[i]->SaveBinaryCopy(binFile);
	}
}	

//-------------------------------------------------------------------------------
void TG_TypeMultiShape::destroy (void)
{
	for (int32_t i=0;i<numTG_TypeShapes;i++)
	{
		if (listOfTypeShapes[i])
		{
			listOfTypeShapes[i]->destroy();
			delete listOfTypeShapes[i];
			listOfTypeShapes[i] = nullptr;
		}
	}

	if (listOfTypeShapes)
		TG_Shape::tglHeap->Free(listOfTypeShapes);
		
	listOfTypeShapes = nullptr;

	if (listOfTextures)
		TG_Shape::tglHeap->Free(listOfTextures);
	listOfTextures= nullptr;

	numTextures = 0;
	numTG_TypeShapes = 0;
	
#ifdef _DEBUG
	if (shapeName)
	{
		free(shapeName);
		shapeName = nullptr;
	}
#endif
}	

//-------------------------------------------------------------------------------
void TG_MultiShape::destroy (void)
{
	for (int32_t i=0;i<numTG_Shapes;i++)
	{
		if (listOfShapes[i].node)
		{
			listOfShapes[i].node->destroy();
			TG_Shape::tglHeap->Free(listOfShapes[i].node);
		}
	}

	if (listOfShapes)
		TG_Shape::tglHeap->Free(listOfShapes);
	listOfShapes = nullptr;	

	numTG_Shapes = 0;
}	

//-------------------------------------------------------------------------------
//Function returns 0 if OK.  -1 if file not found or file not ASE Format.
//Function runs through each piece of ASE file and creates a separate
//TG_Shape for each one.  First pass is count number of GEOMOBJECTS.
//Second pass is load each one.
int32_t TG_TypeMultiShape::LoadTGMultiShapeFromASE (PSTR fileName, bool forceMakeBinary, IProviderEngine* armProvider)
{
	//-----------------------------------------------------
	// Fully loads and parses an ASE File.  These files are
	// output by 3D Studio MAX's ASCII Scene Exporter.

	//------------------------------------------------------
	// New!  Checks for Binary file to load instead.
	//
	// If Binary is older or equal to ASE, re-parses ASE and
	// saves binary.
	//
	// If Binary does not exist, does the above, too!
	// 
	// If Binary exists and is newer, just loads that.
	// MUCH FASTER!
	//
	bool makeBinary = false;
	int32_t i, j;

	char drive[MAX_PATH];
	char dir[MAX_PATH];
	char name[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(fileName,drive,dir,name,ext);

#ifdef _DEBUG
	shapeName = (PSTR )malloc(strlen(name) + 1);
	strcpy(shapeName,name);
#endif
	
	FullPathFileName binaryFileName;
	binaryFileName.init(tglPath,name,".tgl");
	
	//Load Table is as follows:
	//	ASE		TGL		FST			Result:
	//	0		0		0			STOP
	//	0		0		1			OK to load if Shape version good, STOp Otherwise
	//	0		1		0			Same as above
	//	0		1		1			Same as above
	//	1		0		0			Set MakeBinary true.
	//	1		0		1			Set MakeBinary true.
	//	1		1		0			If TGL older then ase, set Make Bindary true.
	//	1		1		1			Same as above.

	if( !forceMakeBinary)
	{
		if (!fileExists(fileName) && !fileExists(binaryFileName))		//Line 0
		{
	#ifdef _DEBUG
	//		if (!silentMode)
	//			PAUSE(("Unable to locate shape data for %s.  No ASE or TGL File",fileName));
	#endif
			makeBinary = true;
		}
		else if (!fileExists(fileName) && fileExists(binaryFileName))	//Line 1-3
		{
			File binFile;
			
			int32_t result = binFile.open(binaryFileName);
			if (result != NO_ERROR)
			{
				if (!silentMode)
					STOP(("Unable to locate shape data for %s.	TGL File BAD Format",fileName));
			}
			else
			{
				int32_t versionId = binFile.readLong();
				if (versionId != CURRENT_SHAPE_VERSION)
				{
					if (!silentMode)
						STOP(("Unable to locate shape data for %s.	TGL File BAD Format",fileName));
				}
			}
		}
		else if (fileExists(fileName) && (!fileExists(binaryFileName) || (fileExists(binaryFileName) == 2)))	//Line 4-5
		{
			makeBinary = true;
		}
		else if (fileExists(fileName) && ((fileExists(binaryFileName) == 1) && file1OlderThan2(binaryFileName,fileName)))	//Line 6-7
		{
			makeBinary = true;
		}
	}
	else
	{
		makeBinary = true;
	}

	if (!makeBinary)
	{
		return (LoadBinaryCopy(binaryFileName));
	}
	else
	{
		//------------------------------------------
		// Check if exists by getting size
		int32_t aseFileSize = gos_FileSize(fileName);
		if (aseFileSize <= 0)
			return(-1);
	
		//---------------------------------------
		// Create Buffer to read entire file into
		puint8_t aseContents = (puint8_t )malloc(aseFileSize+1);
		gosASSERT(aseContents != nullptr);
	
		//---------------------------------------
		// Open File and read it and close it
		HGOSFILE aseFile;
		gos_OpenFile(&aseFile,fileName,READONLY);
	
#ifdef _DEBUG
		int32_t dataRead = 
#endif
		gos_ReadFile(aseFile,aseContents,aseFileSize);
		gosASSERT(dataRead == aseFileSize);
	
		gos_CloseFile(aseFile);
	
		aseContents[aseFileSize] = 0;
	
		//----------------------------------------
		// Check for valid ASE Header data.
		if (strnicmp(ASE_HEADER,(PSTR )aseContents,strlen(ASE_HEADER)) != 0)
			return(-1);
		
		//---------------------------------------
		// Texture Data Next.
		//If the Material Class is Standard, ONLY one texture possible.
		//If the Material Class is Sub-Object, Multiple textures possible.
		char textureId[256];
		PSTR textureData;
	
		char numberData[256];
		sprintf(textureId,ASE_MATERIAL_COUNT);
		textureData = strstr((PSTR )aseContents,textureId);
		if (!textureData)
			return(-1);
	
		textureData += strlen(ASE_MATERIAL_COUNT)+1;
		GetNumberData(textureData,numberData);
		int32_t numMaterials = atol(numberData);
	
		numTextures = 0;
		puint8_t aseBuffer = aseContents;
		for (int32_t nmt=0;nmt<numMaterials;nmt++)
		{
			sprintf(textureId,ASE_MATERIAL_CLASS);
			textureData = strstr((PSTR )aseBuffer,textureId);
			if (!textureData)
				return(-1);
	
			textureData += strlen(ASE_MATERIAL_CLASS)+1;
			GetWordData(textureData,numberData);
	
			if (strstr(numberData,"Standard"))
			{
				numTextures++;
			}
			else if (strstr(numberData,"Multi"))
			{
				//----------------------------------------------------
				//NOT SUPPORTED YET!  Multiple Textures per building!
				// Support NOW!  Must have trees!
				textureData = strstr(textureData,ASE_SUBMATERIAL_COUNT);
				gosASSERT(textureData != nullptr);
	
				textureData += strlen(ASE_SUBMATERIAL_COUNT);
				GetNumberData(textureData,numberData);
	
				numTextures += atol(numberData);
			}
	
			aseBuffer = (puint8_t )textureData;
		}

		IProviderAssetPtr armAssetPtr = nullptr;

		if (armProvider)
		{
			armAssetPtr = armProvider->OpenAsset(fileName, 
				AssetType_Physical, ProviderType_Primary);
			armAssetPtr->AddProperty("Type", "Shape");
		}
	
		if (numTextures)
		{
			numTextures *= 2;
			listOfTextures = (TG_TexturePtr)TG_Shape::tglHeap->Malloc(sizeof(TG_Texture) * numTextures);
			gosASSERT(listOfTextures != nullptr);
			memset(listOfTextures,0x0,sizeof(TG_Texture) * numTextures);

			PSTR txmData = (PSTR )aseContents;
			for (i=0;i<(numTextures/2);i++)
			{
				//-------------------------------------------------------------------------------
				// Get and store texture Name.  Will need multiple for Multi-Sub if implemented
				char textId[256];
				sprintf(textId,"%s",ASE_MATERIAL_BITMAP_ID,i);
		
				PSTR txmTemp = txmData;
				txmData = strstr(txmData,textId);
				
				if (txmData == nullptr)
				{
#ifdef _DEBUG
					if (!silentMode)
						PAUSE(("WARNING: Material ID %d in Shape %s has no Texture Bitmap!!",(i+1),fileName));
#endif						
					txmData = txmTemp;
					strcpy(listOfTextures[i].textureName,"NULLTXM");
				}
				else
				{
					txmData += strlen(textId);
					GetNameData(txmData,listOfTextures[i].textureName);
				}
				
				if (listOfTextures[i].textureName[0] == 0)
				{
					strcpy(listOfTextures[i].textureName,"NULLTXM");
				}
				
				// ARM
				if (armAssetPtr && strcmp(listOfTextures[i].textureName, "NULLTXM") != 0) 
				{				
					// The filenames refer to art files on some share (\\aas1), just get the base filename
					// and assume it's in the "Data\TGL\128\" directory
					PSTR  baseFilename = strrchr(listOfTextures[i].textureName, '\\');
					
					if (baseFilename)
					{
						char localFilename[1024] = "Data\\TGL\\128\\";
						strcat(localFilename, baseFilename+1);
						armAssetPtr->AddRelationship("Texture", localFilename);
					}
					else
					{
						armAssetPtr->AddRelationship("Texture", listOfTextures[i].textureName);
					}
				}

				listOfTextures[i].mcTextureNodeIndex = 0xffffffff;
				listOfTextures[i].gosTextureHandle = 0xffffffff;
				listOfTextures[i].textureAlpha = false;
			}
			
   			for (i=(numTextures/2);i<numTextures;i++)
   			{
   				//-------------------------------------------------------------------------------
   				// add the word Shadow the texture name so we can load shadows if we need to!
   				strncpy(listOfTextures[i].textureName,listOfTextures[i-(numTextures/2)].textureName,strlen(listOfTextures[i-(numTextures/2)].textureName)-4);
   				strcat(listOfTextures[i].textureName,"X.tga");
   				listOfTextures[i].mcTextureNodeIndex = 0xffffffff;
				listOfTextures[i].gosTextureHandle = 0xffffffff;
   				listOfTextures[i].textureAlpha = true;
   			}

			for (i=0;i<numTextures;i++)
			{
				// Try to touch all of the textures we need.
				char txmName[1024];
				GetTextureName(i,txmName,256);
		
				char texturePath[1024];
				sprintf(texturePath,"%s%d\\",tglPath,ObjectTextureSize);
			
				FullPathFileName textureName;
				textureName.init(texturePath,txmName,"");
			
				if( fileExists(textureName) == 1 )
				{
					File *touchTexture = new File();
					touchTexture->open(textureName);
					touchTexture->close();
					delete touchTexture;
				}
			}
		}
		else
			listOfTextures = nullptr;
	
		//------------------------------------------------------------
		// FIRST PASS.  Count number of GEOMObjects & HelperObjects
		numTG_TypeShapes = 0;
		PSTR aseScan = (PSTR )aseContents;
		aseScan = strstr(aseScan,ASE_OBJECT);
		while (aseScan != nullptr)
		{
			numTG_TypeShapes++;
			aseScan += strlen(ASE_OBJECT)+1;
			aseScan = strstr(aseScan,ASE_OBJECT);
		}
	
		aseScan = (PSTR )aseContents;
		aseScan = strstr(aseScan,ASE_HELP_OBJECT);
		while (aseScan != nullptr)
		{
			//--------------------------------------------------------
			// Check if NodeName is handle_ or WORLD.  These are bad!
			// Do not count them!
			aseScan = strstr(aseScan,ASE_NODE_NAME);
			gosASSERT(aseScan != nullptr);								//Node must have a NAME, right?
			aseScan += strlen(ASE_NODE_NAME)+1;
	
			char chkWord[512];
			GetWordData(aseScan,chkWord);
	
			if (strstr(chkWord,"handle_") == nullptr && strstr(chkWord,"World") == nullptr)
			{
				numTG_TypeShapes++;
			}
	
			aseScan += strlen(ASE_HELP_OBJECT)+1;
			aseScan = strstr(aseScan,ASE_HELP_OBJECT);
		}
	
		listOfTypeShapes = (TG_TypeNodePtr *)TG_Shape::tglHeap->Malloc(sizeof(TG_TypeNodePtr) * (numTG_TypeShapes+1));
		gosASSERT(listOfTypeShapes != nullptr);
	
		memset(listOfTypeShapes,0,sizeof(TG_TypeNodePtr) * (numTG_TypeShapes+1));
	
		//------------------------------------------------------------
		// SECOND PASS.  Load HelperObjects.
		int32_t startIndex = 0;
	
		aseScan = (PSTR )aseContents;
		aseScan = strstr(aseScan,ASE_HELP_OBJECT);
		while (aseScan != nullptr)
		{
			listOfTypeShapes[startIndex] = new TG_TypeNode;
			listOfTypeShapes[startIndex]->init();
	
			//---------------------------------------------------
			// Calling from top will load just first HELPEROBJECT!
			// We now make the "top" the start of each HELPEROBJECT!
			// DO NOT LOAD handle_ helpers and WORLD halpers.
			// Decrement number of shapes accordingly.
			PSTR aseEnd = aseScan + strlen(ASE_HELP_OBJECT)+1;
			aseEnd = strstr(aseEnd,ASE_HELP_OBJECT);
	
			if (aseEnd)
			{
				PSTR aseData = (PSTR )malloc(aseFileSize+1);
				ZeroMemory(aseData, aseFileSize+1);
				memcpy(aseData,aseScan,aseEnd - aseScan);
	
				//--------------------------------------------------------
				// Check if NodeName is handle_ or WORLD.  These are bad!
				// Do not count them!
				aseScan = strstr(aseScan,ASE_NODE_NAME);
				gosASSERT(aseScan != nullptr);								//Node must have a NAME, right?
				aseScan += strlen(ASE_NODE_NAME)+1;
	
				char chkWord[512];
				GetWordData(aseScan,chkWord);
	
				//-------------------------------------------------------------------
				// Is this a forbidden helper object?
				if ((strstr(chkWord,"handle") == nullptr) && (strstr(chkWord,"World") == nullptr))
				{
					int32_t parseResult = listOfTypeShapes[startIndex]->MakeFromHelper((puint8_t )aseData,fileName);
					if (parseResult != 0)
						return(parseResult);
	
					startIndex++;
				}
	
				free(aseData);
			}
			else
			{
				//--------------------------------------------------------
				// Check if NodeName is handle_ or WORLD.  These are bad!
				// Do not count them!
				PSTR scanStart = aseScan;
				scanStart = strstr(scanStart,ASE_NODE_NAME);
				gosASSERT(scanStart != nullptr);								//Node must have a NAME, right?
				scanStart += strlen(ASE_NODE_NAME)+1;
	
				char chkWord[512];
				GetWordData(scanStart,chkWord);
	
				//-------------------------------------------------------------------
				// Is this a forbidden helper object?
				if ((strstr(chkWord,"handle") == nullptr) && (strstr(chkWord,"World") == nullptr))
				{
					//-------------------------------------------------------------------
					int32_t parseResult = listOfTypeShapes[startIndex]->MakeFromHelper((puint8_t )aseScan,fileName);
					if (parseResult != 0)
						return(parseResult);
	
					startIndex++;
				}
			}
	
			aseScan += strlen(ASE_HELP_OBJECT);
			aseScan = strstr(aseScan,ASE_HELP_OBJECT);
		}
	
		//------------------------------------------------------------
		// THIRD PASS.  Load GeomObjects.
		aseScan = (PSTR )aseContents;
		aseScan = strstr(aseScan,ASE_OBJECT);
		while (aseScan != nullptr)
		{
			listOfTypeShapes[startIndex] = new TG_TypeShape;
			listOfTypeShapes[startIndex]->init();
			listOfTypeShapes[startIndex]->CreateListOfTextures(listOfTextures,numTextures);
	
			//---------------------------------------------------
			// Calling from top will load just first GEOMOBJECT!
			// We now make the "top" the start of each GEOMOBJECT!
			PSTR aseEnd = aseScan + strlen(ASE_OBJECT)+1;
			aseEnd = strstr(aseEnd,ASE_OBJECT);
	
			if (aseEnd)
			{
				PSTR aseData = (PSTR )malloc(aseFileSize+1);
				ZeroMemory(aseData, aseFileSize+1);
				memcpy(aseData,aseScan,aseEnd - aseScan);
				
				int32_t parseResult = listOfTypeShapes[startIndex]->ParseASEFile((puint8_t )aseData,fileName);
				if (parseResult != 0)
					return(parseResult);
	
				free(aseData);
			}
			else
			{
				int32_t parseResult = listOfTypeShapes[startIndex]->ParseASEFile((puint8_t )aseScan,fileName);
				if (parseResult != 0)
					return(parseResult);
			}
	
			startIndex++;
			aseScan += strlen(ASE_OBJECT)+1;
			aseScan = strstr(aseScan,ASE_OBJECT);
		}
	
 		//------------------------------------------------------------------
		// FOURTH PASS.  Setup Heirarchy ALWAYS or NO Shape BOUNDS!!!
		{
			aseScan = (PSTR )aseContents;
			for (i=0;i<numTG_TypeShapes;i++)
			{
				//----------------------------------------------------------------------------------
				// For each shape, look for another node whose NodeId matches this shape's parentId
				TG_TypeNodePtr parentNode = nullptr;
				for (j=0;j<numTG_TypeShapes;j++)
				{
					if (strcmp(listOfTypeShapes[i]->getParentId(),listOfTypeShapes[j]->getNodeId()) == 0)
					{
						parentNode = listOfTypeShapes[j];
						break;
					}
				}
	
				listOfTypeShapes[i]->movePosRelativeCenterNode();
	
				if (parentNode)
					listOfTypeShapes[i]->MoveNodeCenterRelative(parentNode->GetNodeCenter());
					
				//--------------------------------------------------------
				//ONLY use nodes which are not spotlight or _PAB or LOS_
				if ((strnicmp(listOfTypeShapes[i]->getNodeId(),"_PAB",4) != 0) &&
					(strnicmp(listOfTypeShapes[i]->getNodeId(),"LOS_",4) != 0) && 
					(strnicmp(listOfTypeShapes[i]->getNodeId(),"SpotLight",9) != 0))
				{
				
					//------------------------------------------------------------------
					// Scan list of vertices and create minBox, maxBox and extentRadius
					for (j=0;j<listOfTypeShapes[i]->GetNumTypeVertices();j++)
					{
						Stuff::Vector3D relNodeCenter;
						relNodeCenter = listOfTypeShapes[i]->GetRelativeNodeCenter();
						
						TG_TypeShapePtr typeShape = (TG_TypeShapePtr)listOfTypeShapes[i];
						if (minBox.x > typeShape->listOfTypeVertices[j].position.x + relNodeCenter.x)
							minBox.x = typeShape->listOfTypeVertices[j].position.x + relNodeCenter.x;
													 
						if (maxBox.x < typeShape->listOfTypeVertices[j].position.x + relNodeCenter.x)
							maxBox.x = typeShape->listOfTypeVertices[j].position.x + relNodeCenter.x;
													 
						if (minBox.y > typeShape->listOfTypeVertices[j].position.y + relNodeCenter.y)
							minBox.y = typeShape->listOfTypeVertices[j].position.y + relNodeCenter.y;
													 
						if (maxBox.y < typeShape->listOfTypeVertices[j].position.y + relNodeCenter.y)
							maxBox.y = typeShape->listOfTypeVertices[j].position.y + relNodeCenter.y;
													 
						if (minBox.z > typeShape->listOfTypeVertices[j].position.z + relNodeCenter.z)
							minBox.z = typeShape->listOfTypeVertices[j].position.z + relNodeCenter.z;
													 
						if (maxBox.z < typeShape->listOfTypeVertices[j].position.z + relNodeCenter.z)
							maxBox.z = typeShape->listOfTypeVertices[j].position.z + relNodeCenter.z;
					}
				}
			}
		
			float minBoxLength = minBox.GetLength();
			float maxBoxLength = maxBox.GetLength();
			if (minBoxLength > extentRadius)
				extentRadius = minBoxLength;
				
			if (maxBoxLength > extentRadius)
				extentRadius = maxBoxLength;
		}

		if (armAssetPtr != nullptr)
		{
			armAssetPtr->Close();
		}
	
		free(aseContents);
		aseContents = nullptr;
	
		SaveBinaryCopy(binaryFileName);
	}

	return(0);
}	

//-------------------------------------------------------------------------------
// TG_TypeMultiShape
PVOID TG_MultiShape::operator new (size_t mySize)
{
	PVOID result = TG_Shape::tglHeap->Malloc(mySize);
	return result;
}
 
//-------------------------------------------------------------------------------
 void TG_MultiShape::operator delete (PVOID us)
{
	TG_Shape::tglHeap->Free(us);
}

//-------------------------------------------------------------------------------
//Function returns 0 if OK.  realLength if textureName is longer then nameLength-1.
//Returns -1 if texture requested is out of range.
//This function digs the texture name(s) out of the ASE file so that the
//User can load and manage them anyway they want to.
int32_t TG_TypeMultiShape::GetTextureName (uint32_t textureNum, PSTR tName, int32_t nameLength)
{
	if (textureNum >= numTextures)
		return(-1);
		
	char baseName[512];
	char extension[256];
	_splitpath(listOfTextures[textureNum].textureName,nullptr,nullptr,baseName,extension);

	char basePath[1024];
	sprintf(basePath,"%s%s",baseName,extension);

	if (nameLength < (strlen(basePath)+1))
		return(strlen(basePath)+1);
		
	strcpy(tName,basePath);
	return(0);
}	

//-------------------------------------------------------------------------------
//Function returns 0 if OK.  -1 if textureNum is out of range of numTextures.
// Assigns a MCTextureNodeIndex to this NOT a GOS handle anymore.  Must go through
// cache in case of too many handles.
int32_t TG_TypeMultiShape::SetTextureHandle (uint32_t textureNum, uint32_t gosTextureHandle)
{
	if (textureNum >= numTextures)
		return(-1);

	listOfTextures[textureNum].mcTextureNodeIndex = gosTextureHandle;
	listOfTextures[textureNum].gosTextureHandle = 0xffffffff;

	return(0);
}	

//-------------------------------------------------------------------------------
//Function returns 0 if OK.  -1 if textureNum is out of range of numTextures.
//This function takes the gosTextureHandle passed in and assigns it to the
//textureNum entry of the listOfTextures;
int32_t TG_TypeMultiShape::SetTextureAlpha (uint32_t textureNum, bool alphaFlag)
{
	if (textureNum >= numTextures)
		return(-1);

	listOfTextures[textureNum].textureAlpha = alphaFlag;

	return(0);
}	

//-------------------------------------------------------------------------------
//This function rotates the heirarchy from this node down.  Used for torso twists, arms, etc.
int32_t TG_MultiShape::SetNodeRotation (PSTR nodeName, Stuff::UnitQuaternion *rot)
{
	for (int32_t i=0;i<numTG_Shapes;i++)
	{
		if (_stricmp(listOfShapes[i].node->myType->getNodeId(),nodeName) == 0)
		{
			//-------------------------------
			// Found it!
			listOfShapes[i].baseRotation = *rot;
			return i;
		}
	}

	return -1;
}	

//----------------------------------------------------------------------------------------------------
//This function returns the world translated position of the vertex asked for with rotation applied.
Stuff::Vector3D TG_MultiShape::GetShapeVertexInEditor(int32_t shapeNum, int32_t vertexNum, float rotation)
{
	Stuff::Vector3D result;
	result.x = result.y = result.z = 0.0f;
	
	if ((shapeNum >= 0) && (shapeNum < numTG_Shapes) && 
		(vertexNum >= 0) && (vertexNum < myMultiType->listOfTypeShapes[shapeNum]->GetNumTypeVertices()))
	{
		//-------------------------------
		// Get the vertex in question.
		Stuff::Vector3D vertexPos;
		TG_TypeShapePtr typeShape = (TG_TypeShapePtr)myMultiType->listOfTypeShapes[shapeNum];
		vertexPos.x = -(typeShape->listOfTypeVertices[vertexNum].position.x + typeShape->GetRelativeNodeCenter().x + GetRootNodeCenter().x);
		vertexPos.z = typeShape->listOfTypeVertices[vertexNum].position.y + typeShape->GetRelativeNodeCenter().y + GetRootNodeCenter().y; 
		vertexPos.y = typeShape->listOfTypeVertices[vertexNum].position.z + typeShape->GetRelativeNodeCenter().z + GetRootNodeCenter().z;
		
		//---------------------------------------
		// Rotate vertex if rotation is NON-zero
		if (rotation != 0.0f)
		{
			Rotate(vertexPos,rotation);
		}
		
		result = vertexPos;


	}
	
	return result;
}

//----------------------------------------------------------------------------------------------------
//This function returns the world translated position of the vertex asked for with rotation applied.
Stuff::Vector3D TG_MultiShape::GetShapeVertexInWorld(int32_t shapeNum, int32_t vertexNum, float rotation)
{
	Stuff::Vector3D result;
	result.x = result.y = result.z = 0.0f;

	if ((shapeNum >= 0) && (shapeNum < numTG_Shapes) && 
		(vertexNum >= 0) && (vertexNum < myMultiType->listOfTypeShapes[shapeNum]->GetNumTypeVertices()))
	{
		//-------------------------------
		// Get the vertex in question.
		Stuff::Point3D vertexPos;
		TG_TypeShapePtr typeShape = (TG_TypeShapePtr)myMultiType->listOfTypeShapes[shapeNum];
		
		vertexPos.Multiply(typeShape->listOfTypeVertices[vertexNum].position,listOfShapes[shapeNum].shapeToWorld);
		
		result.x = -vertexPos.x;
		result.y = vertexPos.z;
		result.z = vertexPos.y;
		
		//---------------------------------------
		// Rotate vertex if rotation is NON-zero
		if (rotation != 0.0f)
		{
			Rotate(result,rotation);
		}
	}
	
	return result;
}

//-------------------------------------------------------------------------------
//This function sets the fog values for the shape.  Straight fog right now.
void TG_MultiShape::SetFogRGB (uint32_t fRGB)
{
	for (int32_t i=0;i<numTG_Shapes;i++)
	{
		listOfShapes[i].node->SetFogRGB(fRGB);
	}
}	

//-------------------------------------------------------------------------------
//This function sets the list of lights used by the TransformShape function
//to light the shape.
//Function returns 0 if lightList entries are all OK.  -1 otherwise.
//
int32_t TG_MultiShape::SetLightList (TG_LightPtr *lightList, uint32_t nLights)
{
	//-----------------------------------------------------
	// Static member of TG_Shape.  Only need to call once.
	// NOT a static function in case lights need to change
	// on a shape by shape basis!
	return (listOfShapes[0].node->SetLightList(lightList,nLights));
}	

//-------------------------------------------------------------------------------
Stuff::Vector3D TG_MultiShape::GetTransformedNodePosition (Stuff::Point3D *pos, Stuff::UnitQuaternion *rot, PSTR nodeId)
{
	Stuff::LinearMatrix4D 	shapeOrigin;
	Stuff::LinearMatrix4D 	localShapeOrigin;
//	Stuff::LinearMatrix4D 	invShapeOrigin;
	
	shapeOrigin.BuildRotation(*rot);
	shapeOrigin.BuildTranslation(*pos);
//	invShapeOrigin.Invert(shapeOrigin);
	
	Stuff::EulerAngles angles(*rot);

	int32_t i=0;

	Stuff::Vector3D result;
	result.x = result.y = result.z = 0.0f;
	
	for (i=0;i<numTG_Shapes;i++)
	{
		//-----------------------------------------------------------------
		// Scan through the list of shapes and dig out the number needed.
		// DO NOT UPDATE THE HEIRARCHY!!!!
		// This thing may not have updated itself this turn yet!!!
		if (_stricmp(listOfShapes[i].node->myType->getNodeId(),nodeId) == 0)
		{
			result.x = -listOfShapes[i].shapeToWorld.entries[3];
			result.z = listOfShapes[i].shapeToWorld.entries[7];
			result.y = listOfShapes[i].shapeToWorld.entries[11];
			
			return result;
		}
	}
	
	return result;
}

//-------------------------------------------------------------------------------
Stuff::Vector3D TG_MultiShape::GetTransformedNodePosition (Stuff::Point3D *pos, Stuff::UnitQuaternion *rot, int32_t nodeId)
{
	Stuff::LinearMatrix4D 	shapeOrigin;
	Stuff::LinearMatrix4D 	localShapeOrigin;
//	Stuff::LinearMatrix4D 	invShapeOrigin;
	
	shapeOrigin.BuildRotation(*rot);
	shapeOrigin.BuildTranslation(*pos);
//	invShapeOrigin.Invert(shapeOrigin);
	
	Stuff::EulerAngles angles(*rot);

	Stuff::Vector3D result;
	result.x = result.y = result.z = 0.0f;
	
	if ((nodeId >= 0) && (nodeId < numTG_Shapes))
	{
		result.x = -listOfShapes[nodeId].shapeToWorld.entries[3];
		result.z = listOfShapes[nodeId].shapeToWorld.entries[7];
		result.y = listOfShapes[nodeId].shapeToWorld.entries[11];
			
		return result;
	}
	
	return result;
}

Stuff::UnitQuaternion moveem;
//-------------------------------------------------------------------------------
//This function does the actual transform math, clip checks and lighting math.
//The matrices passed in are the translation/rotation matrix for the shape and
//Its inverse.  Also will animate.
//Function returns -1 if all vertex screen positions are off screen.
//Function returns 0 if any one vertex screen position is off screen.
//Function returns 1 is all vertex screen positions are on screen.
// NOTE:  THIS IS NOT A RIGOROUS CLIP!!!!!!!!!

float yawRotation = 0.0f;

#ifdef LAB_ONLY
int64_t MCTimeTransformandLight 	= 0;
int64_t MCTimeAnimationandMatrix	= 0;
int64_t MCTimePerShapeTransform		= 0;
#endif

int32_t TG_MultiShape::TransformMultiShape (Stuff::Point3D *pos, Stuff::UnitQuaternion *rot)
{
	//Profile T&L so I can break out GameLogic from T&L
	#ifdef LAB_ONLY
	int64_t x;
	x=GetCycles();
	#endif
	
 	Stuff::LinearMatrix4D 	shapeOrigin;
	Stuff::LinearMatrix4D	shadowOrigin;
	Stuff::LinearMatrix4D 	localShapeOrigin;
	
	shapeOrigin.BuildRotation(*rot);
	shapeOrigin.BuildTranslation(*pos);

	Stuff::EulerAngles angles(*rot);
	yawRotation = angles.yaw;

	shadowOrigin.BuildRotation(Stuff::EulerAngles(-angles.pitch,0.0f,0.0f));
	shadowOrigin.BuildTranslation(*pos);
	
	int32_t i, j;
	Stuff::Point3D camPosition;
	camPosition = *TG_Shape::cameraOrigin;

	Stuff::Matrix4D  shapeToClip, rootShapeToClip;
	Stuff::Point3D backFacePoint;

	TG_ShapeRecPtr childChain[MAX_NODES];

	for (i=0;i<numTG_Shapes;i++)
	{
		//----------------------------------------------
		// Must set each transform!  Animating Textures!
		for (j=0;j<myMultiType->numTextures;j++)
		{
			listOfShapes[i].node->myType->SetTextureHandle(j,myMultiType->listOfTextures[j].mcTextureNodeIndex);
			listOfShapes[i].node->myType->SetTextureAlpha(j,myMultiType->listOfTextures[j].textureAlpha); 
		}

		//-----------------------------------------------------------------
		// Heirarchy Animation Code.
		//
		// Simple, really.  For each shape in list, traverse back UP
		// the heirarchy and store the traversal pointers in a temp list.
		// Starting at the TOP of the heirarchy and for each shape, 
		// check if matrix set for that shape. If so, next node down.  If
		// not, copy above matrix into node and apply animation data.
		// Do this until at end of this heirarchy.
		int32_t curChild = 0;
		childChain[curChild] = &listOfShapes[i];
		while (childChain[curChild]->parentNode)
		{
			curChild++;

			gosASSERT(curChild < MAX_NODES);

			childChain[curChild] = childChain[curChild-1]->parentNode;
		}

		int32_t fNum = float2long(frameNum);

		Stuff::Point3D zero;
		zero.x = zero.y = zero.z = 0.0f;
		for (j=curChild;j>=0;j--)
		{
			if (childChain[j]->calcedThisFrame != turn)
			{
				if (j == curChild)		//This is the ROOT Node.
				{
					//----------------------------------------------------
					// Top O the hierarchy.  Used passed in shapeMatrices
					// Apply any animation data, if data is non-nullptr
					if (childChain[j]->currentAnimation)
					{
						//--------------------------------------
						// Slerp between current and next frame
						Stuff::UnitQuaternion slerpQuat;
						slerpQuat.x = slerpQuat.y = slerpQuat.z = 0.0f;
						slerpQuat.w = 1.0f;
						
						if (childChain[j]->currentAnimation->quat)
							slerpQuat = childChain[j]->currentAnimation->quat[fNum];

						//--------------------------------------
						//First Apply Animation to this local piece of heirarchy.
						// If piece had base rotation, apply it.  Otherwise, no.
						Stuff::UnitQuaternion totalRotation = slerpQuat;
						if ((childChain[j]->baseRotation.w == 1.0f) && 
							(childChain[j]->baseRotation.x == 0.0f) &&
							(childChain[j]->baseRotation.y == 0.0f) && 
							(childChain[j]->baseRotation.z == 0.0f))
						{
						}
						else
						{
							totalRotation.Multiply(slerpQuat,childChain[j]->baseRotation);
							totalRotation.Normalize();
						}

						localShapeOrigin.BuildRotation(totalRotation);
 						  
						if (childChain[j]->currentAnimation->pos)
							localShapeOrigin.BuildTranslation(childChain[j]->currentAnimation->pos[fNum]);		//SPECIAL.  ROOT HAS ITS OWN OFFSETS!
						else
							localShapeOrigin.BuildTranslation(childChain[j]->node->myType->GetNodeCenter());

						childChain[j]->localShapeToWorld = localShapeOrigin;

						//------------------------------------------------------------------
						//Then move the piece of the heirarchy into the frame of the parent
						childChain[j]->shapeToWorld.Multiply(childChain[j]->localShapeToWorld,shapeOrigin);

						childChain[j]->worldToShape.Invert(childChain[j]->shapeToWorld);
						childChain[j]->calcedThisFrame = turn;
					}
					else
					{
						localShapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f,0.0f,0.0f));
						localShapeOrigin.BuildTranslation(childChain[j]->node->myType->GetNodeCenter());

						childChain[j]->localShapeToWorld = localShapeOrigin;

						childChain[j]->shapeToWorld.Multiply(childChain[j]->localShapeToWorld,shapeOrigin);

						childChain[j]->worldToShape.Invert(childChain[j]->shapeToWorld);
						childChain[j]->calcedThisFrame = turn;
					}
				}
				else
				{
					//----------------------------------------------------------------------
					// Not Top O the Heirarchy.  Figure out matrix based on Animation data.
					if (childChain[j]->currentAnimation)
					{
						//--------------------------------------
						// Slerp between current and next frame
						Stuff::UnitQuaternion slerpQuat;
						slerpQuat.x = slerpQuat.y = slerpQuat.z = 0.0f;
						slerpQuat.w = 1.0f;
						
						if (childChain[j]->currentAnimation->quat)
							slerpQuat = childChain[j]->currentAnimation->quat[fNum];

						//--------------------------------------
						//First Apply Animation to this local piece of heirarchy.
						// If piece had base rotation, apply it.  Otherwise, no.
						Stuff::UnitQuaternion totalRotation = slerpQuat;
						if ((childChain[j]->baseRotation.w == 1.0f) && 
							(childChain[j]->baseRotation.x == 0.0f) &&
							(childChain[j]->baseRotation.y == 0.0f) && 
							(childChain[j]->baseRotation.z == 0.0f))
						{
						}
						else
						{
							totalRotation.Multiply(slerpQuat,childChain[j]->baseRotation);
							totalRotation.Normalize();
						}

						localShapeOrigin.BuildRotation(totalRotation);
						
						if (childChain[j]->currentAnimation->pos)
							localShapeOrigin.BuildTranslation(childChain[j]->currentAnimation->pos[fNum]);
						else
							localShapeOrigin.BuildTranslation(childChain[j]->node->myType->GetRelativeNodeCenter());
							
						childChain[j]->localShapeToWorld = localShapeOrigin;

						//------------------------------------------------------------------
						//Then move the piece of the heirarchy into the frame of the parent
						childChain[j]->localShapeToWorld.Multiply(localShapeOrigin,childChain[j+1]->localShapeToWorld);

						//------------------------------------------------------------------
						// Then deal with global translation.
						childChain[j]->shapeToWorld.Multiply(childChain[j]->localShapeToWorld,shapeOrigin);

						childChain[j]->worldToShape.Invert(childChain[j]->shapeToWorld);
						childChain[j]->calcedThisFrame = turn;
					}
					else
					{
						//--------------------------------------
						// NO Animation if we are here.
						//
						// Apply Base Rotation.  If it is Zero, no problem!
						Stuff::UnitQuaternion totalRotation = childChain[j]->baseRotation;
						localShapeOrigin.BuildRotation(totalRotation);
						localShapeOrigin.BuildTranslation(childChain[j]->node->myType->GetRelativeNodeCenter());

						childChain[j]->localShapeToWorld = localShapeOrigin;

						//------------------------------------------------------------------
						//Then move the piece of the heirarchy into the frame of the parent
						localShapeOrigin = childChain[j]->localShapeToWorld;
						childChain[j]->localShapeToWorld.Multiply(localShapeOrigin,childChain[j+1]->localShapeToWorld);

						//------------------------------------------------------------------
						// Then deal with global translation.
						childChain[j]->shapeToWorld.Multiply(childChain[j]->localShapeToWorld,shapeOrigin);

						childChain[j]->worldToShape.Invert(childChain[j]->shapeToWorld);
						childChain[j]->calcedThisFrame = turn;
					}
				}
			}
		}

		if (useFaceLighting || useVertexLighting)
		{
			//----------------------------------------------------
			// Setup Lighting here.
			if (Environment.Renderer != 3)
			{
				for (int32_t iLight=0;iLight<TG_Shape::numLights;iLight++)
				{
					if ((TG_Shape::listOfLights[iLight] != nullptr) && (TG_Shape::listOfLights[iLight]->active))
					{
						switch (TG_Shape::listOfLights[iLight]->lightType)
						{
							case TG_LIGHT_AMBIENT:
							{
								//No Setup needed for Ambient light
							}
							break;
		
							case TG_LIGHT_INFINITE:
							{
								if (TG_Shape::listOfLights[iLight] != nullptr)
								{
									TG_Shape::lightToShape[iLight].Multiply(TG_Shape::listOfLights[iLight]->lightToWorld,listOfShapes[i].worldToShape);
									Stuff::UnitVector3D uVec;
									TG_Shape::lightToShape[iLight].GetLocalForwardInWorld(&uVec);
									TG_Shape::lightDir[iLight] = uVec;
									
									if (listOfShapes[i].parentNode == nullptr)
									{
										TG_Shape::rootLightDir[iLight] = TG_Shape::lightDir[iLight];
										//if (angles.yaw != 0.0f )
										//	RotateLight(TG_Shape::rootLightDir[iLight],-angles.yaw);
									}
								}
							}
							break;
		
							case TG_LIGHT_INFINITEWITHFALLOFF:
							{
								if (TG_Shape::listOfLights[iLight] != nullptr)
								{
									TG_Shape::lightToShape[iLight].Multiply(TG_Shape::listOfLights[iLight]->lightToWorld,listOfShapes[i].worldToShape);
									Stuff::UnitVector3D uVec;
									TG_Shape::lightToShape[iLight].GetLocalForwardInWorld(&uVec);
									TG_Shape::lightDir[iLight] = uVec;
									
									if (listOfShapes[i].parentNode == nullptr)
									{
										TG_Shape::rootLightDir[iLight] = TG_Shape::lightDir[iLight];
									}
								}
							}
							break;
		
							case TG_LIGHT_POINT:
							{
								if (TG_Shape::listOfLights[iLight] != nullptr)
								{
									Stuff::Point3D lightPos;
									lightPos = TG_Shape::listOfLights[iLight]->direction;
		
									Stuff::Point3D shapePosition;
									shapePosition = listOfShapes[i].shapeToWorld;
		
									shapePosition -= lightPos;
									shapePosition.y = 0.0f;
									TG_Shape::lightDir[iLight] = shapePosition;
									//if (angles.yaw != 0.0f )
									//	RotateLight(TG_Shape::lightDir[iLight],-angles.yaw);
								}
								
								if (listOfShapes[i].parentNode == nullptr)
								{
									TG_Shape::rootLightDir[iLight] = TG_Shape::lightDir[iLight];
								}
							}
							break;
							
							case TG_LIGHT_TERRAIN:
							{
								if (TG_Shape::listOfLights[iLight] != nullptr)
								{
									Stuff::Point3D lightPos;
									lightPos = TG_Shape::listOfLights[iLight]->direction;
		
									Stuff::Point3D shapePosition;
									shapePosition = listOfShapes[i].shapeToWorld;
		
									shapePosition -= lightPos;
									shapePosition.y = 0.0f;
									TG_Shape::lightDir[iLight] = shapePosition;
								}
								
								if (listOfShapes[i].parentNode == nullptr)
								{
									TG_Shape::rootLightDir[iLight] = TG_Shape::lightDir[iLight];
								}
							}
							break;
							
							case TG_LIGHT_SPOT:
							{
								if (TG_Shape::listOfLights[iLight] != nullptr)
								{
									Stuff::Point3D lightPos;
									lightPos = TG_Shape::listOfLights[iLight]->direction;
		
									Stuff::Point3D shapePosition;
									shapePosition = listOfShapes[i].shapeToWorld;
		
									shapePosition -= lightPos;
									shapePosition.y = 0.0f;
									TG_Shape::lightDir[iLight] = shapePosition;
									//if (angles.yaw != 0.0f )
									//	RotateLight(TG_Shape::lightDir[iLight],-angles.yaw);
									
									lightPos = TG_Shape::listOfLights[iLight]->spotDir;
									shapePosition = listOfShapes[i].shapeToWorld;
									
									shapePosition -= lightPos;
									shapePosition.y = 0.0f;
									TG_Shape::spotDir[iLight] = shapePosition;
									//if (angles.yaw != 0.0f )
									//	RotateLight(TG_Shape::spotDir[iLight],-angles.yaw);
										
									if (listOfShapes[i].parentNode == nullptr)
									{
										TG_Shape::rootLightDir[iLight] = TG_Shape::spotDir[iLight];
									}
								}
							}
							break;
						}
					}
				}
			}
		}

		shapeToClip.Multiply(listOfShapes[i].shapeToWorld,TG_Shape::worldToClip);
		backFacePoint.Multiply(camPosition,listOfShapes[i].worldToShape);

	#ifdef LAB_ONLY
	x=GetCycles()-x;
	MCTimeAnimationandMatrix += x;
	x=GetCycles();
	#endif
	
		listOfShapes[i].node->MultiTransformShape(&shapeToClip,&backFacePoint,listOfShapes[i].parentNode,isHudElement,alphaValue,isClamped);

		if (useShadows && d_useShadows)
		{
			listOfShapes[i].node->MultiTransformShadows(pos, &(listOfShapes[i].shapeToWorld),yawRotation);
		}
		
	#ifdef LAB_ONLY
	x=GetCycles()-x;
	MCTimePerShapeTransform += x;
	x=GetCycles();
	#endif
	}

	#ifdef LAB_ONLY
	MCTimeTransformandLight = MCTimeAnimationandMatrix + MCTimePerShapeTransform;
	#endif
	return(0);
}	

//-------------------------------------------------------------------------------
//This function takes the current listOfVisibleFaces and draws them using
//gos_DrawTriangle.
void TG_MultiShape::Render (bool refreshTextures, float forceZ)
{
	for (int32_t i=0;i<numTG_Shapes;i++)
	{
		if (listOfShapes[i].processMe && listOfShapes[i].node)
		{
			//-------------------------------------------------------
			// Only need to due for unique instance items like mechs
			if (refreshTextures)
			{
				for (int32_t j=0;j<myMultiType->numTextures;j++)
				{
					listOfShapes[i].node->myType->SetTextureHandle(j,myMultiType->listOfTextures[j].mcTextureNodeIndex);
					listOfShapes[i].node->myType->SetTextureAlpha(j,myMultiType->listOfTextures[j].textureAlpha); 
				}
			}
		
			listOfShapes[i].node->Render(forceZ,isHudElement,alphaValue,isClamped);
		}
	}
}	

//-------------------------------------------------------------------------------
//This function takes the current listOfVisibleFaces and draws them using
//gos_DrawTriangle.
void TG_MultiShape::RenderShadows (bool refreshTextures)
{
	int32_t start = 0;
	for (int32_t i=0;i<numTG_Shapes;i++)
	{
		if (listOfShapes[i].processMe && listOfShapes[i].node)
		{
			//-------------------------------------------------------
			// Only need to due for unique instance items like mechs
			if (refreshTextures)
			{
				for (int32_t j=0;j<myMultiType->numTextures;j++)
				{
					listOfShapes[i].node->myType->SetTextureHandle(j,myMultiType->listOfTextures[j].mcTextureNodeIndex);
					listOfShapes[i].node->myType->SetTextureAlpha(j,myMultiType->listOfTextures[j].textureAlpha); 
				}
			}

			start = listOfShapes[i].node->RenderShadows(start);
		}
	}
}	

//-------------------------------------------------------------------------------
// This function takes the shape named nodeName and all of its children and stops processing
// them forever.  Since we can never re-attach a mech arm in the field, this is OK!
// However, should we want this functionality, it is easy to add!
void TG_MultiShape::StopUsing (PSTR nodeName)
{
	//First, find all shapes which are children of nodeName, including nodeName!
	TG_ShapeRecPtr childChain[MAX_NODES];
	TG_ShapeRecPtr detachables[MAX_NODES];
	
	int32_t curShape = 0;
	int32_t i=0;
	for (i=0;i<numTG_Shapes;i++)
	{
		//----------------------------------------------
		int32_t curChild = 0;
		childChain[curChild] = &listOfShapes[i];
		while (childChain[curChild]->parentNode)
		{
			curChild++;

			gosASSERT(curChild < MAX_NODES);

			childChain[curChild] = childChain[curChild-1]->parentNode;
		}

		Stuff::Point3D zero;
		zero.x = zero.y = zero.z = 0.0f;
		for (int32_t j=curChild;j>=0;j--)
		{
			if (_stricmp(childChain[j]->node->getNodeName(),nodeName) == 0)
			{
				detachables[curShape] = childChain[0];
				curShape++;
				break;
			}
		}	
	}
	
	//Then, mark all of the shapes found with a "don't process me" flag
	for (i=0;i<curShape;i++)
	{
		//Keep the joint_xUARM so that the shoulder sparks work.
		if (_stricmp(detachables[i]->node->getNodeName(),nodeName) != 0)
			detachables[i]->processMe = false;
	}
}

//-------------------------------------------------------------------------------
// This function takes the shape named nodeName and all of its children, detaches them
// from the current heirarchy and stuffs them into a new MultiShape which it passes back
// Uses are endless but for now limited to blowing the arms off of the mechs!
TG_MultiShapePtr TG_MultiShape::Detach (PSTR nodeName)
{
	//First, find all shapes which are children of nodeName, including nodeName!
	TG_ShapeRecPtr childChain[MAX_NODES];
	TG_ShapeRecPtr detachables[MAX_NODES];
	
	int32_t curShape = 0;
	int32_t i=0;
	for (i=0;i<numTG_Shapes;i++)
	{
		//----------------------------------------------
		int32_t curChild = 0;
		childChain[curChild] = &listOfShapes[i];
		while (childChain[curChild]->parentNode)
		{
			curChild++;

			gosASSERT(curChild < MAX_NODES);

			childChain[curChild] = childChain[curChild-1]->parentNode;
		}

		Stuff::Point3D zero;
		zero.x = zero.y = zero.z = 0.0f;
		for (int32_t j=curChild;j>=0;j--)
		{
			if (_stricmp(childChain[j]->node->getNodeName(),nodeName) == 0)
			{
				detachables[curShape] = childChain[0];
				curShape++;
				break;
			}
		}	
	}
	
	if (!curShape)
		return nullptr;

		//OK to return no shape now.
		//STOP(("Unable to detach %s from shape",nodeName));
		
	//Then, create a new TG_MultiShape and stuff it with the shapes found.
	TG_MultiShapePtr resultShape = new TG_MultiShape;
	resultShape->myMultiType = myMultiType;
	resultShape->numTG_Shapes = curShape;
	
	resultShape->listOfShapes = (TG_ShapeRecPtr)TG_Shape::tglHeap->Malloc(sizeof(TG_ShapeRec) * curShape);
	gosASSERT(resultShape->listOfShapes != nullptr);

	memset(resultShape->listOfShapes,0,sizeof(TG_ShapeRec) * curShape);

	resultShape->frameNum = 0;
	
	for (i=0;i<curShape;i++)
	{
		resultShape->listOfShapes[i] = *detachables[i];
		
		if (_stricmp(resultShape->listOfShapes[i].node->getNodeName(),nodeName) == 0)
			resultShape->listOfShapes[i].parentNode = nullptr;		//Set new top O heirarchy.
		
		for (int32_t j=0;j<numTG_Shapes;j++)
		{
			if (&listOfShapes[j] == detachables[i])
			{
				//Mark this shape as removable from the previous heirarchy
				listOfShapes[j].node = nullptr;
				listOfShapes[j].parentNode = nullptr;
			}
		}
	}

	//Rebuild resultShape parent node pointers!
	for (i=0;i<resultShape->numTG_Shapes;i++)
	{
		//----------------------------------------------------------------------------------
		// For each shape, look for another node whose NodeId matches this shape's parentId
		for (int32_t j=0;j<resultShape->numTG_Shapes;j++)
		{
			if (strcmp(resultShape->listOfShapes[i].node->myType->getParentId(),resultShape->listOfShapes[j].node->myType->getNodeId()) == 0)
			{
				//-------------------------------
				// Found it!
				resultShape->listOfShapes[i].parentNode = &(resultShape->listOfShapes[j]);
				break;
			}
		}
	}
	
 	//-------------------------------------------------------------
	// Rebuild the currentShape heirarchy to reflect removed things
	TG_ShapeRecPtr newListOfShapes = (TG_ShapeRecPtr)TG_Shape::tglHeap->Malloc(sizeof(TG_ShapeRec) * (numTG_Shapes - curShape));
	gosASSERT(newListOfShapes != nullptr);

	memset(newListOfShapes,0,sizeof(TG_ShapeRec) * (numTG_Shapes - curShape));
	
	int32_t newShapeIndex = 0;
	for (i=0;i<numTG_Shapes;i++)
	{
		if (listOfShapes[i].node != nullptr)
		{
			newListOfShapes[newShapeIndex] = listOfShapes[i];
			newShapeIndex++;
		}
	}

	//-------------------------------------------------------------
	// Move the new list over the old list.
	if (listOfShapes)
		TG_Shape::tglHeap->Free(listOfShapes);
	listOfShapes = newListOfShapes;
	
	numTG_Shapes -= curShape;

	//Rebuild parent node pointers!
	for (i=0;i<numTG_Shapes;i++)
	{
		//----------------------------------------------------------------------------------
		// For each shape, look for another node whose NodeId matches this shape's parentId
		for (int32_t j=0;j<numTG_Shapes;j++)
		{
			if (strcmp(listOfShapes[i].node->myType->getParentId(),listOfShapes[j].node->myType->getNodeId()) == 0)
			{
				//-------------------------------
				// Found it!
				listOfShapes[i].parentNode = &(listOfShapes[j]);
				break;
			}
		}
	}

	return resultShape;
}

//-------------------------------------------------------------------------------
// Tells me if the passed in nodeName is a child of the parentName.
bool TG_MultiShape::isChildOf (PSTR nodeName, PSTR parentName)
{
	//First, find all shapes which are children of nodeName, including nodeName!
	TG_ShapeRecPtr childChain[MAX_NODES];
	TG_ShapeRecPtr detachables[MAX_NODES];
	
	int32_t curShape = 0;
	int32_t i=0;
	for (i=0;i<numTG_Shapes;i++)
	{
		//----------------------------------------------
		int32_t curChild = 0;
		childChain[curChild] = &listOfShapes[i];
		while (childChain[curChild]->parentNode)
		{
			curChild++;

			gosASSERT(curChild < MAX_NODES);

			childChain[curChild] = childChain[curChild-1]->parentNode;
		}

		Stuff::Point3D zero;
		zero.x = zero.y = zero.z = 0.0f;
		for (int32_t j=curChild;j>=0;j--)
		{
			if (_stricmp(childChain[j]->node->getNodeName(),parentName) == 0)
			{
				detachables[curShape] = childChain[0];
				curShape++;
				break;
			}
		}	
	}
	
	if (!curShape)
		return false;
		
	for (i=0;i<curShape;i++)
	{
		if (_stricmp(detachables[i]->node->getNodeName(), nodeName) == 0)
			return true;
	}
	
	return false;
}

//-------------------------------------------------------------------------------
// class TG_AnimateShape
//-------------------------------------------------------------------------------
PVOIDTG_AnimateShape::operator new (size_t mySize)
{
	PVOID result = TG_Shape::tglHeap->Malloc(mySize);
	return result;
}
 
//-------------------------------------------------------------------------------
 void TG_AnimateShape::operator delete (PVOID us)
{
	TG_Shape::tglHeap->Free(us);
}
//-------------------------------------------------------------------------------
void _TG_Animation::SaveBinaryCopy (File *binFile)
{
	if (_stricmp(nodeId,"NONE") != 0)
	{
		binFile->write((puint8_t)nodeId,TG_NODE_ID);	
		binFile->writeLong(-1);			//ShapeIds ALWAYS start with -1.  We will scan on frame 1 please!
		binFile->writeLong(numFrames);
		binFile->writeFloat(frameRate);
		binFile->writeFloat(tickRate);
		
		if (quat)
			binFile->writeLong(sizeof(Stuff::UnitQuaternion) * numFrames);
		else
			binFile->writeLong(0);
			
		if (pos)
			binFile->writeLong(sizeof(Stuff::Point3D) * numFrames);
		else
			binFile->writeLong(0);
		
		if (quat)
			binFile->write((puint8_t)quat,sizeof(Stuff::UnitQuaternion) * numFrames);
			
		if (pos)
			binFile->write((puint8_t)pos,sizeof(Stuff::Point3D) * numFrames);
	}
}

//-------------------------------------------------------------------------------
void _TG_Animation::LoadBinaryCopy (File *binFile)
{
	binFile->read((puint8_t)nodeId,TG_NODE_ID);	
	shapeId = binFile->readLong();
	numFrames = binFile->readLong();
	frameRate = binFile->readFloat();
	tickRate = binFile->readFloat();
	
	int32_t quatRAM = binFile->readLong();
	int32_t posRAM = binFile->readLong();		
	
	if (quatRAM)
	{
		quat = (Stuff::UnitQuaternion *)TG_Shape::tglHeap->Malloc(sizeof(Stuff::UnitQuaternion) * numFrames); 
		binFile->read((puint8_t)quat,quatRAM);	
	}
	else
		quat = nullptr;
		
	if (posRAM)
	{
		pos = (Stuff::Point3D *)TG_Shape::tglHeap->Malloc(sizeof(Stuff::Point3D) * numFrames); 
		binFile->read((puint8_t)pos,posRAM);	
	}
	else
		pos = nullptr;
}

//-------------------------------------------------------------------------------
int32_t TG_AnimateShape::LoadBinaryCopy (PSTR fileName)
{
	File binFile;
	int32_t result = binFile.open(fileName);
	if (result != NO_ERROR)
		return -1;
		
	int32_t animFileVersion = binFile.readLong();
	if (animFileVersion == CURRENT_ANIM_VERSION)
	{
		count = binFile.readLong();
		
		if (count)
		{
			listOfAnimation = (TG_AnimationPtr)TG_Shape::tglHeap->Malloc(sizeof(TG_Animation) * count);
			gosASSERT(listOfAnimation != nullptr);
 			
			for (int32_t i=0;i<count;i++)
				listOfAnimation[i].LoadBinaryCopy(&binFile);
		}
		else
			listOfAnimation = nullptr;
	}
	else
	{
		return -1;
	}
	
	return 0;
}

//-------------------------------------------------------------------------------
void TG_AnimateShape::SaveBinaryCopy (PSTR fileName)
{
	File binFile;
	binFile.create(fileName);
	
	binFile.writeLong(CURRENT_ANIM_VERSION);
	
	//ListOfAnimations
	actualCount = 0;
	if (count)
	{
 		for (int32_t i=0;i<count;i++)
		{
			if (_stricmp(listOfAnimation[i].nodeId,"NONE") != 0)
				actualCount++;
		}
	}

	binFile.writeLong(actualCount);

	//ListOfAnimations
	if (count)
	{
 		for (int32_t i=0;i<count;i++)
			listOfAnimation[i].SaveBinaryCopy(&binFile);
	}
}

//-------------------------------------------------------------------------------
//This function frees all of the RAM allocated by this class and resets vars to initial state.
void TG_AnimateShape::destroy (void)
{
	if (listOfAnimation)
	{
		for ( int32_t i = 0; i < count; ++i )
		{
			TG_Shape::tglHeap->Free( listOfAnimation[i].pos );
			listOfAnimation[i].pos = nullptr;

			TG_Shape::tglHeap->Free( listOfAnimation[i].quat );
			listOfAnimation[i].quat = nullptr;
		}

		if (listOfAnimation)
			TG_Shape::tglHeap->Free(listOfAnimation);
			
		listOfAnimation = nullptr;
	}
}	

FilePtr tglLogFile = (FilePtr)nullptr;

//-------------------------------------------------------------------------------
//This function loads the animation data contained in the file passed in.
//It sets up a pointer to the multi-shape so that animation data for each
//Node in the Multi-Shape can be loaded.
//It mallocs memory.
int32_t TG_AnimateShape::LoadTGMultiShapeAnimationFromASE (PSTR fileName, TG_TypeMultiShapePtr shape, bool skipIfBinary, bool forceMakeBinary)
{
	//------------------------------------------------------
	// New!  Checks for Binary file to load instead.
	//
	// If Binary is older or equal to ASE, re-parses ASE and
	// saves binary.
	//
	// If Binary does not exist, does the above, too!
	// 
	// If Binary exists and is newer, just loads that.
	// MUCH FASTER!
	//
	bool makeBinary = false;

	char drive[MAX_PATH];
	char dir[MAX_PATH];
	char name[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(fileName,drive,dir,name,ext);

	FullPathFileName binaryFileName;
	binaryFileName.init(tglPath,name,".agl");

	if( !forceMakeBinary )
	{
		if (fileExists(binaryFileName) && !fileExists(fileName))
		{
			File binFile;
			
			int32_t result = binFile.open(binaryFileName);
			if (result != NO_ERROR)
			{
				makeBinary = true;
			}
			else
			{
				int32_t versionId = binFile.readLong();
				if (versionId != CURRENT_ANIM_VERSION)
					makeBinary = true;
			}
		}
		else
		{
			makeBinary = true;
		}
	}
	else
	{
		makeBinary = true;
	}

	if (!makeBinary)
	{
		if (!skipIfBinary)
			return (LoadBinaryCopy(binaryFileName));
		else
			return 0;
	}
	else
	{
		//-----------------------------------------------------
		// Fully loads and parses an ASE File.  These files are
		// output by 3D Studio MAX's ASCII Scene Exporter.
		//
		shapeIdsSet = false;
	
		//------------------------------------------
		// Check if exists by getting size
		int32_t aseFileSize = gos_FileSize(fileName);
		if (aseFileSize <= 0)
			return(-1);
	
		//---------------------------------------
		// Create Buffer to read entire file into
		puint8_t aseContents = (puint8_t )malloc(aseFileSize+1);
		gosASSERT(aseContents != nullptr);
	
		//---------------------------------------
		// Open File and read it and close it
		HGOSFILE aseFile;
		gos_OpenFile(&aseFile,fileName,READONLY);
	
	#ifdef _DEBUG
		int32_t dataRead = 
	#endif
		gos_ReadFile(aseFile,aseContents,aseFileSize);
		gosASSERT(dataRead == aseFileSize);
	
		gos_CloseFile(aseFile);
	
		aseContents[aseFileSize] = 0;
	
		//----------------------------------------
		// Check for valid ASE Header data.
		if (strnicmp(ASE_HEADER,(PSTR )aseContents,strlen(ASE_HEADER)) != 0)
			return(-1);
	
		//------------------------------------------
		// Get first frame of animation from header
		int32_t firstFrame, lastFrame;
		PSTR frameId = strstr((PSTR )aseContents,ASE_ANIM_FIRST_FRAME);
		gosASSERT(frameId != nullptr);
		frameId += strlen(ASE_ANIM_FIRST_FRAME)+1;
	
		char numData[512];
		GetNumberData(frameId,numData);
		firstFrame = atol(numData);
	
		frameId = strstr((PSTR )aseContents,ASE_ANIM_LAST_FRAME);
		gosASSERT(frameId != nullptr);
	
		frameId += strlen(ASE_ANIM_LAST_FRAME)+1;
	
		GetNumberData(frameId,numData);
		lastFrame = atol(numData);
	
		//gosASSERT(firstFrame == 0);
		gosASSERT(firstFrame <= lastFrame);
	
		//if (firstFrame == lastFrame)
			//No Animation data at all. Possible?
	
		int32_t numFrames = (lastFrame - firstFrame) + 1;
	
		frameId = strstr((PSTR )aseContents,ASE_ANIM_FRAME_SPEED);
		gosASSERT(frameId != nullptr);
	
		frameId += strlen(ASE_ANIM_FRAME_SPEED)+1;
	
		GetNumberData(frameId,numData);
		float frameRate = (float)atof(numData);
	
		frameId = strstr((PSTR )aseContents,ASE_ANIM_TICKS_FRAME);
		gosASSERT(frameId != nullptr);
	
		frameId += strlen(ASE_ANIM_TICKS_FRAME)+1;
	
		GetNumberData(frameId,numData);
		float tickRate = (float)atof(numData);
	
		//----------------------------------------------------------------------------
		// For each TG_Shape in MultiShape passed in, find any and all animation Data
		count = shape->GetNumShapes();
		listOfAnimation = (TG_AnimationPtr)TG_Shape::tglHeap->Malloc(sizeof(TG_Animation) * shape->GetNumShapes());
		gosASSERT(listOfAnimation != nullptr);
	
		//----------------------------------------------------------------------------
		// Scan the Nodes.
		actualCount = 0;
		bool countUp = false;
		for (int32_t i=0;i<count;i++)
		{
			PSTR nodeId = shape->GetNodeId(i);
			gosASSERT(nodeId != nullptr);
		
			PSTR animScan = (PSTR )aseContents;
			animScan = strstr(animScan,ASE_ANIMATION);
	
			char nodeName[512];
			sprintf(nodeName,"*NODE_NAME \"%s\"",nodeId);
			while (animScan != nullptr)
			{
				animScan += strlen(ASE_ANIMATION)+1;
	
				//------------------------------------------------------
				// We found a TM_ANIMATION Section.
				// Check if the VERY NEXT LINE is the correct NodeName
				char nextLine[1024];
				GetNextLine(animScan,nextLine);
	
				if (strstr(nextLine,nodeName) == nullptr)
				{
					animScan = strstr(animScan,ASE_ANIMATION);
				}
				else
				{
					animScan += strlen(nodeName)+1;
					break;
				}
			}
	
			if (animScan == nullptr)
			{
				//No Animation Data for this Node.
				strcpy(listOfAnimation[i].nodeId,"NONE");
				listOfAnimation[i].numFrames = numFrames;
				listOfAnimation[i].frameRate = frameRate;
				listOfAnimation[i].tickRate = tickRate;
				listOfAnimation[i].quat = nullptr;
				listOfAnimation[i].pos = nullptr;
				listOfAnimation[i].shapeId = 0xffffffff;
			}
			else
			{
				//Start with No Animation Data for this Node.
				strcpy(listOfAnimation[i].nodeId,"NONE");
				listOfAnimation[i].numFrames = numFrames;
				listOfAnimation[i].frameRate = frameRate;
				listOfAnimation[i].tickRate = tickRate;
				listOfAnimation[i].quat = nullptr;
				listOfAnimation[i].pos = nullptr;
				listOfAnimation[i].shapeId = 0xffffffff;

				//---------------------------------
				// Check for positional data first!
				PSTR scanStart = animScan;
	
				char numData[512];
				char nextLine[1024];
				float timeStamp = firstFrame * tickRate;
	
				//----------------------------------------------------
				// Then the very NEXT LINE most be POS_TRACK data OR
				// there is ONLY rotational Data for this node.
				GetNextLine(animScan,nextLine);
				if (strstr(nextLine,ASE_ANIM_POS_HEADER) != nullptr)
				{
					animScan = strstr(animScan,ASE_ANIM_POS_HEADER);
					if (animScan)
					{
						countUp = true;
						actualCount++;
						animScan += strlen(ASE_ANIM_POS_HEADER);
		
						//-----------------------------------------------------------
						// We have positional data at least.  Store everything off.
						listOfAnimation[i].pos = (Stuff::Point3D *)TG_Shape::tglHeap->Malloc(sizeof(Stuff::Point3D) * numFrames);
		
						strcpy(listOfAnimation[i].nodeId,nodeId);
						listOfAnimation[i].numFrames = numFrames;
						listOfAnimation[i].frameRate = frameRate;
						listOfAnimation[i].tickRate = tickRate;
						listOfAnimation[i].shapeId = 0xffffffff;
		
						Stuff::Point3D thisOffset = shape->GetNodeCenter(i);
		
						char LineData[1024];
						GetNextLine(animScan,LineData);
						animScan += strlen(LineData)+1;
		
						for (int32_t j=0;j<listOfAnimation[i].numFrames;j++)
						{
							PSTR scanData;
							sprintf(nodeName,"%s %d",ASE_ANIM_POS_SAMPLE,(int32_t)timeStamp);
							scanData = strstr(LineData,nodeName);
		
							if (scanData)
							{
								scanData += strlen(nodeName)+1;
		
								GetNumberData(scanData,numData);
								thisOffset.x = -(float)atof(numData);
								scanData += strlen(numData)+1;
		
								GetNumberData(scanData,numData);
								thisOffset.z = (float)atof(numData);
								scanData += strlen(numData)+1;
		
								GetNumberData(scanData,numData);
								thisOffset.y = (float)atof(numData);
								scanData += strlen(numData)+1;
		
								GetNextLine(animScan,LineData);
								animScan += strlen(LineData)+1;
							}
							else
							{
								//Otherwise Node Center did not move.  Use last Node Center
							}
		
							listOfAnimation[i].pos[j] = thisOffset;
							timeStamp += tickRate;
						}
					}
				}
											   
				//-------------------------------------------------------------
				// Check for rotational data. Again, use nextLine.
				
				//----------------------------------------------------
				// Then the very NEXT LINE most be POS_TRACK data OR
				// there is ONLY rotational Data for this node.
				GetNextLine(animScan,nextLine);
				if (strstr(nextLine,ASE_ANIM_ROT_HEADER) != nullptr)
				{
					animScan = scanStart;
					timeStamp = firstFrame * tickRate;
		
					animScan = strstr(animScan,ASE_ANIM_ROT_HEADER);
		
					if (animScan)
					{
						countUp = true;
						actualCount++;
						animScan += strlen(ASE_ANIM_ROT_HEADER);
		
						//-----------------------------------------------------------
						// We have rotational data at least.  Store everything off.
						listOfAnimation[i].quat = (Stuff::UnitQuaternion *)TG_Shape::tglHeap->Malloc(sizeof(Stuff::UnitQuaternion) * numFrames);
						gosASSERT(listOfAnimation[i].quat != nullptr);
		
						//-------------------------------------------
						// Setup basic variables.  May do this twice.
						strcpy(listOfAnimation[i].nodeId,nodeId);
						listOfAnimation[i].numFrames = numFrames;
						listOfAnimation[i].frameRate = frameRate;
						listOfAnimation[i].tickRate = tickRate;
		
						char LineData[1024];
						GetNextLine(animScan,LineData);
						animScan += strlen(LineData)+1;
			
						for (int32_t j=0;j<listOfAnimation[i].numFrames;j++)
						{
							PSTR scanData;
							sprintf(nodeName,"%s %d",ASE_ANIM_ROT_SAMPLE,(int32_t)timeStamp);
							scanData = strstr(LineData,nodeName);
		
							Stuff::UnitQuaternion thisFrame;
	
							float b=0.0f,c=0.0f,d=0.0f,phi=0.0f;
		
							if (scanData)
							{
								scanData += strlen(nodeName)+1;
		
								GetNumberData(scanData,numData);
								b = (float)atof(numData);
								scanData += strlen(numData)+1;
		
								GetNumberData(scanData,numData);
								c = (float)atof(numData);
								scanData += strlen(numData)+1;
		
								GetNumberData(scanData,numData);
								d = (float)atof(numData);
								scanData += strlen(numData)+1;
		
								GetNumberData(scanData,numData);
								phi = (float)atof(numData);
		
								GetNextLine(animScan,LineData);
								animScan += strlen(LineData)+1;
			
								//--------------------------------------------
								// MAX Writes out Quaternions as Angle, Axis.
								// Must Convert to real quaternion here.
								thisFrame.w = (float)cos(phi / 2.0f);
								thisFrame.x = b * (float)sin(phi / 2.0f);
								thisFrame.y = d * (float)sin(-phi / 2.0f);
								thisFrame.z = c * (float)sin(-phi / 2.0f);
							}
							else
							{
								//Otherwise rotation is 0.
								thisFrame.w = 1.0f;
								thisFrame.x = 0.0f;
								thisFrame.y = 0.0f;
								thisFrame.z = 0.0f;
							}
		
							if (!j)
							{
								thisFrame.Normalize();
								listOfAnimation[i].quat[j] = thisFrame;
							}
							else
							{
								thisFrame.Normalize();
								listOfAnimation[i].quat[j].Multiply(listOfAnimation[i].quat[j-1],thisFrame);
								listOfAnimation[i].quat[j].Normalize();
							}
		
							timeStamp += tickRate;
						}
					}
				}
				
				countUp = false;
			}
		}
	
		free(aseContents);
		aseContents = nullptr;
		
		SaveBinaryCopy(binaryFileName);
	}

	return(0);
}	

//-------------------------------------------------------------------------------
//This function copies the pointers to the animation data in this class to the
//TG_MultiShape being drawn.  Nothing else happens.
void TG_AnimateShape::SetAnimationState (TG_MultiShapePtr shape)
{
	int32_t i=0;
	shape->ClearAnimation();

	if (!shapeIdsSet)
	{
		for (int32_t j=0;j<count;j++)
		{
			bool foundNode = false;
			for (i=0;i<shape->GetNumShapes();i++)
			{
				if (_stricmp(listOfAnimation[j].nodeId,shape->GetNodeId(i)) == 0)
				{
					shape->SetCurrentAnimation(i,&listOfAnimation[j]);
					listOfAnimation[j].shapeId = i;
					foundNode = true;
					break;
				}
			}

			if (!foundNode)
				listOfAnimation[j].shapeId = -1;	//This node is missing.  Probably an arm off!
		}
		
		shapeIdsSet = true;
	}
	else
	{
		for (int32_t j=0;j<count;j++)
		{
			if (listOfAnimation[j].shapeId != 0xffffffff)
				shape->SetCurrentAnimation(listOfAnimation[j].shapeId,&listOfAnimation[j]);
		}
	}
}	

//-------------------------------------------------------------------------------
