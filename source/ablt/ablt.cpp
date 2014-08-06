//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// ablt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "string.h"

void initABL (void);
void closeABL (void);
 
int main(int argc, char* argv[])
{
	if ((argc < 2) || (argc > 3)) {
		printf("Try again.\n");
		scanf(" ");
		return(0);
	}

	printf("ABLT - MechCommander 2 ABL Tester v0.2\n");
	printf("\n");

	initABL();
	unsigned char s[256];

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
			if (s[strlen((char*)s)-1] == 10)
				s[strlen((char*)s)-1] = NULL;
			long numErrs = 0;
			long numLines = 0;
			long numFiles = 0;
			if ((s[0] == 'l') && (s[1] == ' ')){
				long handle = (long)ABLi_loadLibrary((char*)&s[2], &numErrs, &numLines, &numFiles, false);
				printf("     Loaded: %s [%d lines, %d files]\n", &s[2], numLines, numFiles);
				}
			else if ((s[0] == 'm') && (s[1] == ' ')) {
				long handle = ABLi_preProcess((char*)&s[2], &numErrs, &numLines, &numFiles, false);
				printf("     Loaded: %s [%d lines, %d files]\n", &s[2], numLines, numFiles);
			}
		}
		bFile->close();
		delete bFile;
		bFile = NULL;
		printf("\n");
	}

	long numErrs = 0;
	long numLines = 0;
	long numFiles = 0;
	long handle = ABLi_preProcess(argv[argc-1], &numErrs, &numLines, &numFiles, false);
	printf("SUCCESS: %s [%d lines, %d files]\n", argv[argc-1], numLines, numFiles);
	scanf(" ");

	closeABL();
	return(0);
}


