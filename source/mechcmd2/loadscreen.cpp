/*************************************************************************************************\
LoadScreen.cpp			: Implementation of the LoadScreen component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#define LOADSCREEN_CPP

#include "stdinc.h"
//#include <windows.h>
//#include <ddraw.h>
#include "loadscreen.h"
#include "mechgui/aanimobject.h"
#include "tgainfo.h"
#include "mclib.h"
#include "prefs.h"
#include "multplyr.h"
#include "mission.h"
#include "gamesound.h"

float loadProgress = 0.0f;

int32_t LoadScreen::xProgressLoc = 0;
int32_t LoadScreen::yProgressLoc = 0;
int32_t LoadScreen::xWaitLoc = 0;
int32_t LoadScreen::yWaitLoc = 0;
bool LoadScreen::turnOffAsyncMouse = false;

TGAFileHeader* LoadScreen::progressTextureMemory = 0;
TGAFileHeader* LoadScreen::progressBackground = 0;
TGAFileHeader* LoadScreen::mergedTexture = 0;
TGAFileHeader* LoadScreen::waitingForPlayersMemory = 0;

LoadScreen* LoadScreenWrapper::enterScreen = nullptr;
LoadScreen* LoadScreenWrapper::exitScreen = nullptr;

extern volatile bool mc2IsInMouseTimer;
extern volatile bool mc2IsInDisplayBackBuffer;

extern void (*AsynFunc)(RECT& WinRect, DDSURFACEDESC2& mouseSurfaceDesc);
extern CPrefs prefs;

//
// Returns the number of bits in a given mask.  Used to determine if we are in
// 555 mode vs 565 mode.
uint16_t
GetNumberOfBits(uint32_t dwMask);

void
MouseTimerInit();
void
MouseTimerKill();

LoadScreenWrapper::LoadScreenWrapper()
{
	// How about, for shits and giggles we toss the static members we created
	// for single player when this inits for multiplayer
	// or any other player!!!!
	if (enterScreen)
		delete enterScreen;
	if (exitScreen)
		delete exitScreen;
	enterScreen = exitScreen = nullptr;
	enterScreen = new LoadScreen;
	exitScreen = new LoadScreen;
	bFirstTime = 0;
}

LoadScreenWrapper::~LoadScreenWrapper()
{
	if (enterScreen)
		delete enterScreen;
	if (exitScreen)
		delete exitScreen;
	enterScreen = exitScreen = nullptr;
}

void
LoadScreenWrapper::init(FitIniFile& file)
{
	enterScreen->init(file);
	//	changeRes();
	bFirstTime = 0;
}

void
LoadScreenWrapper::changeRes()
{
	const std::wstring_view& Appendix = nullptr;
	switch (prefs.resolution)
	{
	case 0:
		Appendix = "_640";
		break;
	case 1:
		break;
	case 2:
		Appendix = "_1024";
		break;
	case 3:
		Appendix = "_1280";
		break;
	case 4:
		Appendix = "_1600";
		break;
	default:
		NODEFAULT;
		// Assert( 0, 0, "Unexpected resolution found in prefs" );
		break;
	}
	wchar_t fileName[256];
	sprintf(fileName, "mcl_loadingscreen");
	if (Appendix)
		strcat(fileName, Appendix);
	FullPathFileName path;
	path.init(artPath, fileName, ".fit");
	FitIniFile outFile;
	if (NO_ERROR != outFile.open(path))
	{
		wchar_t error[256];
		sprintf(error, "couldn't open file %s", path);
		Assert(0, 0, error);
		return;
	}
	// The 0x2 means that we do NOT want to flush this texture when we toss
	// the texture cache before a mission.  BUT we DO want the texture to cache
	// out to make more room for stuff.
	exitScreen->init(outFile, 0x2);
	exitScreen->setupOutAnims();
	LoadScreen::changeRes(outFile);
}

void
LoadScreen::changeRes(FitIniFile& outFile)
{
	if (progressBackground)
		delete[] progressBackground;
	progressBackground = nullptr;
	if (waitingForPlayersMemory)
		delete[] waitingForPlayersMemory;
	waitingForPlayersMemory = nullptr;
	if (!progressBackground)
	{
		wchar_t progressPath[256];
		wchar_t progressBackgroundPath[256];
		int32_t result = outFile.seekBlock("LoadingBar");
		gosASSERT(result == NO_ERROR);
		outFile.readIdLong("XLocation", xProgressLoc);
		outFile.readIdLong("YLocation", yProgressLoc);
		outFile.readIdString("FileName", progressPath, 255);
		outFile.readIdString("BackgroundFileName", progressBackgroundPath, 255);
		File tgaFile;
		FullPathFileName path;
		path.init(artPath, progressBackgroundPath, ".tga");
		if (NO_ERROR != tgaFile.open(path))
		{
			wchar_t error[256];
			sprintf(error, "couldn't open file %s", path);
			Assert(0, 0, error);
			return;
		}
		int32_t size = tgaFile.fileSize();
		progressBackground = (TGAFileHeader*)new wchar_t[size];
		tgaFile.read((uint8_t*)progressBackground, tgaFile.fileSize());
		tgaFile.close();
		path.init(artPath, progressPath, ".tga");
		if (NO_ERROR != tgaFile.open(path))
		{
			wchar_t error[256];
			sprintf(error, "couldn't open file %s", path);
			Assert(0, 0, error);
			return;
		}
		size = tgaFile.fileSize();
		progressTextureMemory = (TGAFileHeader*)new wchar_t[size];
		tgaFile.read((uint8_t*)progressTextureMemory, tgaFile.fileSize());
		mergedTexture = (TGAFileHeader*)new wchar_t[size];
		memcpy(mergedTexture, progressTextureMemory, size);
		tgaFile.close();
		result = outFile.seekBlock("WaitImage");
		gosASSERT(result == NO_ERROR);
		outFile.readIdString("FileName", progressPath, 255);
		outFile.readIdLong("XLocation", xWaitLoc);
		outFile.readIdLong("YLocation", yWaitLoc);
		path.init(artPath, progressPath, ".tga");
		if (NO_ERROR != tgaFile.open(path))
		{
			wchar_t error[256];
			sprintf(error, "couldn't open file %s", path);
			Assert(0, 0, error);
			return;
		}
		size = tgaFile.fileSize();
		waitingForPlayersMemory = (TGAFileHeader*)new wchar_t[size];
		tgaFile.read((uint8_t*)waitingForPlayersMemory, tgaFile.fileSize());
		flipTopToBottom((uint8_t*)(waitingForPlayersMemory + 1),
			waitingForPlayersMemory->pixel_depth, waitingForPlayersMemory->width,
			waitingForPlayersMemory->height);
	}
}

void
LoadScreenWrapper::begin()
{
	waitForResChange = 0;
	bFirstTime = true;
	enterScreen->begin();
}

void
LoadScreenWrapper::update()
{
	if (loadProgress > 99)
		soundSystem->playDigitalSample(LOAD_DOORS_OPENING);
	else if (bFirstTime)
		soundSystem->playDigitalSample(LOAD_DOORS_CLOSING);
	bFirstTime = 0;
	if (waitForResChange) // waiting one more render to force
	{
		status = READYTOLOAD;
		waitForResChange = 0;
	}
	else
	{
		if (Environment.screenwidth == 800)
		{
			enterScreen->update();
			status = enterScreen->getStatus();
			if (status == READYTOLOAD)
			{
				waitForResChange = 1;
				changeRes();
				status = RUNNING;
			}
		}
		else
		{
			exitScreen->update();
			status = exitScreen->getStatus();
		}
	}
}

void
LoadScreenWrapper::render(int32_t xOffset, int32_t yOffset)
{
	if (Environment.screenwidth == 800)
	{
		enterScreen->render(xOffset, yOffset);
	}
	else
		exitScreen->render(xOffset, yOffset);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
LoadScreen::LoadScreen()
{
	progressBackground = 0;
	progressTextureMemory = 0;
	animIndices = 0;
}

//-------------------------------------------------------------------------------------------------

LoadScreen::~LoadScreen()
{
	if (progressBackground)
		delete[] progressBackground;
	if (progressTextureMemory)
		delete[] progressTextureMemory;
	if (mergedTexture)
		delete[] mergedTexture;
	if (animIndices)
		delete[] animIndices;
	if (waitingForPlayersMemory)
		delete[] waitingForPlayersMemory;
	animIndices = nullptr;
	waitingForPlayersMemory = progressBackground = progressTextureMemory = mergedTexture = nullptr;
}

void
LoadScreen::begin()
{
	for (size_t i = 0; i < animObjectsCount; i++)
	{
		// The assign below will overwrite the assign in defaultConstructor,
		// leaking the memory
		animObjects[i].animInfo.destroy();
		animObjects[i].animInfo = inAnims[animIndices[i]];
		animObjects[i].animInfo.begin();
		animObjects[i].update();
	}
	loadProgress = 0;
	//-----------------------------------------------
	// Turn the mouse cursor OFF until load is done.
	// This will be keep ghost images from occuring.
	userInput->mouseOff();
}
void
LoadScreen::init(FitIniFile& file, uint32_t neverFlush)
{
	LogisticsScreen::init(
		file, "Static", "Text", "Rect", "Button", "Edit", "AnimObject", neverFlush);
	file.seekBlock("AnimationTopOut");
	outAnims[0].init(&file, "");
	file.seekBlock("AnimationBottomOut");
	outAnims[1].init(&file, "");
	file.seekBlock("AnimationLeftOut");
	outAnims[2].init(&file, "");
	file.seekBlock("AnimationRightOut");
	outAnims[3].init(&file, "");
	file.seekBlock("AnimationTopIn");
	inAnims[0].init(&file, "");
	file.seekBlock("AnimationBottomIn");
	inAnims[1].init(&file, "");
	file.seekBlock("AnimationLeftIn");
	inAnims[2].init(&file, "");
	file.seekBlock("AnimationRightIn");
	inAnims[3].init(&file, "");
	file.seekBlock("AnimationTop2In");
	inAnims[4].init(&file, "");
	file.seekBlock("AnimationTop2Out");
	outAnims[4].init(&file, "");
	text.init(&file, "AnimObject18");
	if (animObjectsCount)
	{
		wchar_t blockName[256];
		animIndices = new int32_t[animObjectsCount];
		for (size_t i = 0; i < animObjectsCount; i++)
		{
			sprintf(blockName, "AnimObject%ld", i);
			file.seekBlock(blockName);
			file.readIdString("AnimationOut", blockName, 255);
			if (strstr(blockName, "2"))
				animIndices[i] = 4;
			else if (strstr(blockName, "Top"))
				animIndices[i] = 0;
			else if (strstr(blockName, "Bottom"))
				animIndices[i] = 1;
			else if (strstr(blockName, "Left"))
				animIndices[i] = 2;
			else
				animIndices[i] = 3;
		}
	}
}

void
LoadScreen::update()
{
	status = RUNNING;
	LogisticsScreen::update();
	bool bDone = true;
	for (size_t i = 0; i < animObjectsCount; i++)
	{
		bDone &= animObjects[i].isDone();
	}
	if (bDone)
	{
		if (loadProgress < 99.f)
		{
			for (size_t i = 0; i < animObjectsCount; i++)
			{
				// The assign below will overwrite the assign in begin, leaking
				// the memory
				animObjects[i].animInfo.destroy();
				animObjects[i].animInfo = outAnims[animIndices[i]];
				animObjects[i].begin();
				animObjects[i].update();
			}
			status = READYTOLOAD;
			prefs.applyPrefs(true);
			turnOffAsyncMouse = mc2UseAsyncMouse;
			if (!mc2UseAsyncMouse)
				MouseTimerInit();
			mc2UseAsyncMouse = true;
			AsynFunc = ProgressTimer;
		}
		else
		{
			loadProgress = 0;
			status = NEXT;
			// YIKES!!  We could be checking the if before the null and
			// executing after!!  Block the thread!  Wait for thread to finish.
			while (mc2IsInMouseTimer)
				;
			// ONLY set the mouse BLT data at the end of each update.  NO MORE
			// FLICKERING THEN!!!
			// BLOCK THREAD WHILE THIS IS HAPPENING
			mc2IsInDisplayBackBuffer = true;
			AsynFunc = nullptr;
			mc2UseAsyncMouse = turnOffAsyncMouse;
			if (!mc2UseAsyncMouse)
				MouseTimerKill();
			mc2IsInDisplayBackBuffer = false;
			// Force mouse Cursors to smaller or larger depending on new video
			// mode.
			userInput->initMouseCursors("cursors");
			userInput->mouseOn();
		}
	}
	else
	{
		userInput->mouseOff();
	}
}

void
LoadScreen::render(int32_t x, int32_t y)
{
	// ignoring animation information...
	LogisticsScreen::render();
	int32_t curX = animObjects[0].animInfo.getXDelta();
	int32_t curY = animObjects[0].animInfo.getYDelta();
	text.move(x + curX, y + curY);
	text.render();
	text.move(-x - curX, -y - curY);
}

void
ProgressTimer(RECT& WinRect, DDSURFACEDESC2& mouseSurfaceDesc)
{
	if (!LoadScreen::progressBackground)
		return;
	int32_t destX = 0;
	int32_t destY = 0;
	uint8_t* pMem = (uint8_t*)(LoadScreen::mergedTexture + 1);
	int32_t destRight = 0;
	int32_t destBottom = 0;
	int32_t srcwidth = 0;
	int32_t srcDepth = 0;
	if (loadProgress > 0 && loadProgress < 100)
	{
		destX = 0;
		destY = 0;
		int32_t destwidth = LoadScreen::progressBackground->width;
		int32_t destheight = LoadScreen::progressBackground->height;
		float widthIncPerProgress = (float)destwidth * 0.01f;
		int32_t* pLSrc = (int32_t*)(LoadScreen::progressBackground + 1);
		int32_t* pLDest = (int32_t*)(LoadScreen::mergedTexture + 1);
		// merge background and current progress together...
		for (size_t i = 0; i < 2; i++)
		{
			for (size_t y = 0; y < destheight; y++)
			{
				for (size_t x = 0; x < LoadScreen::progressBackground->width; x++)
				{
					if (x < destwidth)
						*pLDest++ = *pLSrc++;
					else
					{
						pLDest++;
						pLSrc++;
					}
				}
			}
			pLSrc = (int32_t*)(LoadScreen::progressTextureMemory + 1);
			pLDest = (int32_t*)(LoadScreen::mergedTexture + 1);
			destwidth = loadProgress * widthIncPerProgress;
		}
		destX = WinRect.left + (LoadScreen::xProgressLoc);
		destY = WinRect.top + (LoadScreen::yProgressLoc);
		pMem = (uint8_t*)(LoadScreen::mergedTexture + 1);
		destRight = destX + LoadScreen::progressBackground->width;
		destBottom = (destY + LoadScreen::progressBackground->height);
		srcwidth = LoadScreen::progressBackground->width;
		srcDepth = LoadScreen::progressBackground->pixel_depth / 8;
	}
	else if (loadProgress == 1000)
	{
		destX = WinRect.left + LoadScreen::xWaitLoc;
		destY = WinRect.top + LoadScreen::yWaitLoc;
		pMem = (uint8_t*)(LoadScreen::waitingForPlayersMemory + 1);
		destRight = destX + LoadScreen::waitingForPlayersMemory->width;
		destBottom = (destY + LoadScreen::waitingForPlayersMemory->height);
		destRight = destRight > WinRect.right ? WinRect.right : destRight;
		destBottom = destBottom > WinRect.bottom ? WinRect.top : destBottom;
		srcwidth = LoadScreen::waitingForPlayersMemory->width;
		srcDepth = LoadScreen::waitingForPlayersMemory->pixel_depth / 8;
	}
	else
		return;
	// now put it on the screen...
	int32_t destwidth = destRight - destX;
	int32_t destheight = destBottom - destY;
	for (size_t y = 0; y < destheight; y++)
	{
		uint8_t* pSrc = pMem + y * srcwidth * srcDepth;
		uint8_t* pDest = (uint8_t*)mouseSurfaceDesc.lpSurface + destX * mouseSurfaceDesc.ddpfpixelformat.dwRGBBitCount / 8 + ((destY + y) * mouseSurfaceDesc.lPitch);
		for (size_t x = 0; x < destwidth; x++)
		{
			uint32_t mcolour = *(int32_t*)pSrc;
			uint8_t baseAlpha = 0;
			uint8_t basecolourRed = (mcolour & 0x00ff0000) >> 16;
			uint8_t basecolourGreen = (mcolour & 0x0000ff00) >> 8;
			uint8_t basecolourBlue = (mcolour & 0x000000ff);
			pSrc += 4;
			if (mouseSurfaceDesc.ddpfpixelformat.dwRGBBitCount == 32)
			{
				(*(int32_t*)pDest) = mcolour;
				pDest += 4;
			}
			else if (mouseSurfaceDesc.ddpfpixelformat.dwRGBBitCount == 24)
			{
				if (!baseAlpha)
				{
					pDest++;
					pDest++;
					pDest++;
				}
				*pDest++ = basecolourRed;
				*pDest++ = basecolourGreen;
				*pDest++ = basecolourBlue;
			}
			else if (mouseSurfaceDesc.ddpfpixelformat.dwRGBBitCount == 16)
			{
				bool in555Mode = false;
				if (GetNumberOfBits(mouseSurfaceDesc.ddpfpixelformat.dwGBitMask) == 5)
					in555Mode = true;
				if (!baseAlpha)
				{
					int32_t clr;
					if (in555Mode)
					{
						clr = (basecolourRed >> 3) << 10;
						clr += (basecolourGreen >> 3) << 5;
						clr += (basecolourBlue >> 3);
					}
					else
					{
						clr = (basecolourRed >> 3) << 11;
						clr += (basecolourGreen >> 2) << 5;
						clr += (basecolourBlue >> 3);
					}
					*pDest++ = clr & 0xff;
					*pDest++ = clr >> 8;
				}
				else
				{
					pDest++;
					pDest++;
				}
			}
		}
	}
}

void
LoadScreen::setupOutAnims()
{
	for (size_t i = 0; i < animObjectsCount; i++)
	{
		// The assign below will overwrite the assign in begin, leaking the
		// memory
		animObjects[i].animInfo.destroy();
		animObjects[i].animInfo = outAnims[animIndices[i]];
		animObjects[i].begin();
		animObjects[i].update();
	}
}

// end of file ( LoadScreen.cpp )
