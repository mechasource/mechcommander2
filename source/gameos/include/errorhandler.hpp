//==========================================================================//
// File:	 ErrorHandler.hpp												//
// Contents: error handling routines										//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#include "string.hpp"

//JJ Fix-A-Roo
typedef LPDIRECT3DVERTEXBUFFER7 (__stdcall *GETDXFUNC)(LPDIRECT3DVERTEXBUFFER7, LPDIRECT3DDEVICE7);
extern GETDXFUNC g_pGetDXVB;

//
const int MAX_MESSAGE_SIZE=65536;	// Maximum total error message size
const int MAX_BUG_NOTES=1024;		// Maximum characters entered as bug notes
const int MAX_LINE_LENGTH=256;		// Maximum characters per line in a file (shown as error line)
const int MAX_SUBJECT_LENGTH=128;	// Maximum length of email subject
const int MAX_RAID_DESC=2048;		// Maximum length of the Raid description (may need to be longer to accomodate RTF info)

extern ULONG DisableErrors;
extern volatile USHORT FPUControl;	// Current FPU control word
extern USHORT FPUDefault;				// Default FPU control word
extern ULONG ShowFrame;
extern float L2SpeedR, L2SpeedW, L2SpeedRW, MainSpeedR, MainSpeedW, MainSpeedRW, VidMemR, VidMemW, VidMemRW, AGPMemR, AGPMemW, AGPMemRW;
extern char SpeedBuffer[16];			// ASCII MHz
extern float ProcessorSpeed;			// Processor speed
extern char LogBuffer[2048];
extern HANDLE LoggingThreadID;
extern bool ThreadDone;
extern bool InDebugger;						// During debugger rendering
extern char CacheInformation[128];

extern void __stdcall InitProcessorSpeed(void);
extern PSTR __stdcall GetProcessor(void);
extern void __stdcall EnableSpewToFile(void);
extern void __stdcall DisableSpewToFile(void);
extern void __stdcall CheckLogFile(void);
extern void __stdcall ShowFTOL(void);

//
// Log file data types
//
const int Log_Type = 4;			// Number of bits for log file 'type' data
enum { Log_Invalid, Log_JoystickCount, Log_JoystickAxis, Log_JoystickButton, Log_JoystickInfo, Log_JoystickEffectStatus, Log_Keypress, Log_GetKey, Log_Endmarker, Log_DataBlock, Log_NetInfo, Log_Media }; 



extern ULONG SpewWrites;
extern char DebuggerName[MAX_PATH+1];
extern bool LaunchDebugger;
extern int ScreenImageSize;

PSTR __stdcall GetFullErrorMessage( HWND hwnd );
void __stdcall WriteBitStream( ULONG Value, ULONG Bits, int Type );
PSTR __stdcall GetBugNotes( HWND hwnd );
PSTR __stdcall SendMail( HWND Window, PSTR Subject, PSTR Text, PSTR Bitmap, PSTR LogFile );
ULONG __stdcall ReadBitStream( ULONG Bits, int Type );
void __stdcall InitLogging(void);
void __stdcall DeleteLogging(void);
void __stdcall UpDateLogFile(void);
void __stdcall Spew( PSTR string );
PSTR __stdcall GetTime(void);
PSTR __stdcall GetExeTime(void);
PSTR __stdcall VersionNumber( PSTR Buffer, ULONG High, ULONG Low );
BOOL __stdcall IsDebuggerAvailable(void);
void __stdcall TriggerDebugger(void);
PSTR __stdcall gosGetUserName(void);
void __stdcall LogRun( PSTR Message );
void __stdcall InitRunLog(void);
void __stdcall GetInstalledAudioVideoCodecs( FixedLengthString& Buffer );
void __stdcall ReadLogData( PUCHAR pData, ULONG Length );
void __stdcall WriteLogData( PUCHAR pData, ULONG Length );

typedef struct _pFTOL
{
	_pFTOL* pNext;
	ULONG	Heap[6];

} pFTOL;


extern pFTOL* ListOfFTOL;



//
// Routines dealing with floating point precision
//
void CheckPrecision(void);



//
// Playback/Record mode?
//
extern ULONG LogMode;								// 0=Logging, 1=Playback
void __stdcall ShowLogFileControls(void);


typedef struct _IMAGEHLP_LINE *PIMAGEHLP_LINE;
typedef struct _tagSTACKFRAME *LPSTACKFRAME;


//
// Machine Details
//
void __stdcall ScanCards( FixedLengthString& Buffer );

//
// Functions in imagehlp.cpp
//
void __stdcall InitExceptionHandler( PSTR CommandLine );
void __stdcall DestroyImageHlp(void);
PSTR __stdcall GetSymbolFromAddress( PSTR Buffer, int Address );
PSTR __stdcall GetLocationFromAddress(PIMAGEHLP_LINE pline, PSTR Buffer, int Address );
void __stdcall InitStackWalk(LPSTACKFRAME sf, PCONTEXT Context );
int __stdcall WalkStack(LPSTACKFRAME sf );

extern ULONG LastOffset;
//extern STACKFRAME ImageHlp_sf;
//extern IMAGEHLP_LINE ImageHlp_pline;

PSTR __stdcall GetExceptionCode( PEXCEPTION_RECORD per );
void __stdcall GetEnvironmentSettings( PSTR CommandLine );

//
// Context information
//
extern EXCEPTION_RECORD SavedExceptRec;
extern CONTEXT SavedContext;



extern bool MathExceptions;
extern bool MathSinglePrecision;
extern float OneOverProcessorSpeed;


//
// Used to setup the ErrorDialogProc
//
extern int ErrorFlags;
extern PSTR ErrorTitle;
extern PSTR ErrorMessage;
extern PSTR ErrorMessageTitle;
extern volatile int ProcessingError;				// Renentrancy test flag for assert error routine
extern PSTR ErrorExceptionText;
extern PUCHAR GotScreenImage;					// Pointer to buffer containing screen image (always 24 bit bmp)
extern int AllowDebugButton;
extern int ErrorFlags,ErrorReturn;

extern NTSTATUS __stdcall ProcessException( PEXCEPTION_POINTERS ep );
extern PSTR __stdcall ErrorNumberToMessage( int hResult );
extern PSTR __stdcall Hex8Number( int Number );
extern void __stdcall GetProcessorDetails(LPSTACKFRAME sf, FixedLengthString& Buffer );
extern void __stdcall GetMachineDetails( FixedLengthString& Buffer );
extern PSTR __stdcall GetLineFromFile( PSTR tempLine, PSTR FileName, int LineNumber );
extern PUCHAR __stdcall GrabScreenImage(void);
extern void __stdcall GetDirectXDetails( FixedLengthString& Buffer );
extern void __stdcall GetGameDetails( FixedLengthString& Buffer, ULONG ErrorFlags );
extern void __stdcall DoDetailedDialog(void);
extern void __stdcall DoSimpleDialog(void);
extern void __stdcall DoColorDialog(void);

bool __stdcall WriteLogFile( PSTR FileName );



#define IDC_CONTINUE					3
#define IDC_IGNORE						4
#define IDI_ICON						104
#define IDD_ERROR1						105
#define IDC_EMAIL						1000
#define IDC_ERRORTEXT					1001
#define IDC_SAVE						1002
#define IDC_DEBUG						1003
#define IDC_EXIT						1004
#define IDC_CAPTURESCREEN				1005
#define IDC_LOGFILES					1006
#define IDC_NOTES						1007
#define IDC_RAID						1008
#define IDC_REPLAY						1009
#define IDC_RAIDDESC					1010



typedef struct _IgnoreAddress
{
	_IgnoreAddress*	pNext;
	ULONG			Address;
} IgnoreAddress;


extern IgnoreAddress*	pIgnore;



#pragma pack(push,1)
typedef struct _LogStruct {
//
// Store time at top of loop
//
	double		CurrentTime;					// Current time at top of the loop
	double		NonPausedCurrentTime;			// Time that does not pause
//	
// Store mouse information at top of loop
//
	float		MouseX;
	float		MouseY;
	int			pXDelta;
	int			pYDelta;
	int			pWheelDelta;
	UCHAR		pButtonsPressed;
	USHORT		Length;							// Length of bitstream (or 0)
} LogStructure;
#pragma pack(pop)


typedef struct _LogHeader {
	ULONG			Magic;							// Magic number for valid log file
	ULONG			Version;						// Version number of log file data
	char			GameName[32];					// Name of application being logged
	char			AppPath[256];					// Path of application being logged
	char			CommandLine[128];				// Command line passed to App
	char			UserName[32];					// Username who created log
	char			LogDate[64];					// Date/Time log created
	char			ExeDate[64];					// Date/Time exe file used to create log
	ULONG			Frames;							// Number of frames of data
	ULONG			Length;							// Size of logging information
	long			StartSeed;						// Random Number seed
	LogStructure*	First;							// Pointer to first frame
	LogStructure*	Current;						// Pointer to current frame
	LogStructure*	Last;							// Pointer to last frame

} LogHeader;


extern LogHeader LogInfo;
extern ULONG LogLoops;
extern ULONG FrameNumber;							// When recording=current frame, playback=last frame
extern ULONG Debounce;

extern void __stdcall EndLogging(void);
extern void __stdcall LoadLogFile(void);
extern void __stdcall CheckLogInSync(void);

//
// Size of blocks of memory allocated for logs
//
#define LOGBLOCKSIZE	(1024*1024*4)				// 4 Meg of log data maximum (About 10 minutes at 60f/s)
#define LOGVERSION		11							// Current version number of log file




extern bool	AllowFail;
extern volatile bool	SpewSilence;						// Set to disable all spews
extern HANDLE hSpewOutput;

void __stdcall ExitGameOS(void);
void __stdcall InitializeSpew(void);
void __stdcall TerminateSpew(void);
void __stdcall DebugColor( UCHAR red, UCHAR green, UCHAR blue );
void __stdcall InitProcessorSpeed(void);
void __stdcall DestroyExceptions(void);


	
