//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// ablt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "string.h"

void initABL(void);
void closeABL(void);
 
extern "C" int32_t __cdecl 
main(int32_t argc, PSTR argv[])
{
	if ((argc < 2) || (argc > 3)) {
		printf("Try again.\n");
		scanf(" ");
		return(0);
	}

	printf("ABLT - MechCommander 2 ABL Tester v0.2\n");
	printf("\n");

	initABL();
	uint8_t s[256];

	int32_t handle;
	int32_t numErrs = 0;
	int32_t numLines = 0;
	int32_t numFiles = 0;
	if (argc == 3) {
		//------------------------
		// load the environment...
		ABLFile* bFile = new ABLFile;
		if (bFile->open(argv[1]) != 0) {
			printf("Cannot open environment file\n");
			scanf("Press any key to exit...");
			return(0);
		}
		printf("Loading Environment...\n");
		while (!bFile->eof()) {
			bFile->readString(s);
			if (s[strlen((PSTR)s)-1] == 10)
				s[strlen((PSTR)s)-1] = NULL;
			numErrs = 0;
			numLines = 0;
			numFiles = 0;
			if ((s[0] == 'l') && (s[1] == ' ')){
				handle = (int32_t)ABLi_loadLibrary((PSTR)&s[2], &numErrs, &numLines, &numFiles, false);
				printf("     Loaded: %s [%d lines, %d files]\n", &s[2], numLines, numFiles);
				}
			else if ((s[0] == 'm') && (s[1] == ' ')) {
				handle = ABLi_preProcess((PSTR)&s[2], &numErrs, &numLines, &numFiles, false);
				printf("     Loaded: %s [%d lines, %d files]\n", &s[2], numLines, numFiles);
			}
		}
		bFile->close();
		delete bFile;
		bFile = NULL;
		printf("\n");
	}

	numErrs = 0;
	numLines = 0;
	numFiles = 0;
	handle = ABLi_preProcess(argv[argc-1], &numErrs, &numLines, &numFiles, false);
	printf("SUCCESS: %s [%d lines, %d files]\n", argv[argc-1], numLines, numFiles);
	scanf(" ");

	closeABL();
	return(0);
}


