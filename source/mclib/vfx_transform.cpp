#include "vfx.h"

extern wchar_t AlphaTable[256 * 256];
extern enum { CPU_UNKNOWN,
	CPU_PENTIUM,
	CPU_MMX } Processor;
// extern void memfill(PVOID Dest,int32_t Length);
// extern void memclear(PVOID Dest,int32_t Length);

void CopySprite(PANE* pane, PVOID texture, int32_t X, int32_t Y, int32_t width, int32_t height,
	int32_t Flip, int32_t Shrink);
void AG_shape_fill(PANE* pane, PVOID shape_table, int32_t shape_number, int32_t hotX, int32_t hotY);
void AG_shape_draw(PANE* pane, PVOID shape_table, int32_t shape_number, int32_t hotX, int32_t hotY);
void AG_shape_translate_fill(
	PANE* pane, PVOID shape_table, int32_t shape_number, int32_t hotX, int32_t hotY);
extern uint32_t lookaside;

int32_t SqrtCount = 0;

// typedef uint32_t uint32_t;
typedef struct SHAPEHEADER
{
	uint32_t bounds;
	uint32_t origin;
	uint32_t xmin;
	uint32_t ymin;
	uint32_t xmax;
	uint32_t ymax;
} SHAPEHEADER;

//
// Sprite format
//
//
// Marker.
// 0			End of line
// 1			Skip next bytes
// Bit 0 = 0	Repeat next byte [7654321] times
// Bit 0 = 1	string packet [7654321] bytes
//

static uint32_t Sourcewidth, twidth, theight,
	Destwidth; // Used for code optimizing
static int64_t xmask = -1;
static uint32_t tempXmax, tempXmin;
static uint32_t minX, minY, maxY, maxX, SkipLeft, Newwidth, StartofLine, StartofClip, EndofClip;
static uint32_t lines, paneX0, paneX1, paneY0, paneY1;
static uintptr_t SourcePointer;
static uintptr_t DestPointer;
static int32_t X, Y, X1;

static PANE tempPANE;
static WINDOW tempWINDOW;

void AG_shape_transform(PANE* globalPane, PVOID shapeTable, int32_t frameNum, int32_t hotX,
	int32_t hotY, PVOID tempBuffer, int32_t reverse, int32_t scaleUp)
{
	_asm
		{

		mov esi, shapeTable
		mov eax, frameNum

		mov ecx, [esi+eax*8+8]

		mov eax, [esi+ecx+SHAPEHEADER.xmax]
		mov ebx, [esi+ecx+SHAPEHEADER.xmin]
		mov X1, eax
		mov X, ebx
		sub eax, ebx
		inc eax
		mov maxX, eax

		mov eax, [esi+ecx+SHAPEHEADER.ymax]
		mov ebx, [esi+ecx+SHAPEHEADER.ymin]
		mov Y, ebx
		sub eax, ebx
		inc eax
		mov maxY, eax

		}
	tempWINDOW.buffer = (uint8_t*)tempBuffer;
	tempWINDOW.x_max = maxX - 1;
	tempWINDOW.y_max = maxY - 1;
	tempPANE.window = &tempWINDOW;
	tempPANE.x0 = 0;
	tempPANE.y0 = 0;
	tempPANE.x1 = maxX - 1;
	tempPANE.y1 = maxY - 1;
	//-------------------------------------------------------------------------------------
	// Some armor here.  We have still have a bug somewhere that trashes the
	// shape by the time it gets here.  Armor against for now and make Fatal to
	// try to track down!
#define MAX_X 360
#define MAX_Y 360
	if ((maxX * maxY) >= (MAX_Y * MAX_X))
	{
		return;
	}
	//-------------------------------------------------------------------------------------
	// memclear(tempBuffer,maxX*maxY);
	memset(tempBuffer, 0, maxX * maxY);
	AG_shape_draw(&tempPANE, shapeTable, frameNum, -X, -Y);
	if (scaleUp)
	{
		if (reverse)
			CopySprite(globalPane, tempBuffer, hotX - X1, hotY + Y, maxX, maxY, reverse, scaleUp);
		else
			CopySprite(globalPane, tempBuffer, hotX + X, hotY + Y, maxX, maxY, reverse, scaleUp);
	}
	else
	{
		if (reverse)
			CopySprite(globalPane, tempBuffer, hotX - (X1 >> 1), hotY + (Y >> 1), maxX, maxY,
				reverse, scaleUp);
		else
			CopySprite(globalPane, tempBuffer, hotX + (X >> 1), hotY + (Y >> 1), maxX, maxY,
				reverse, scaleUp);
	}
}

void AG_shape_translate_transform(PANE* globalPane, PVOIDshapeTable, int32_t frameNum, int32_t hotX,
	int32_t hotY, PVOIDtempBuffer, int32_t reverse, int32_t scaleUp)
{
	_asm
		{

		mov esi, shapeTable
		mov eax, frameNum

		mov ecx, [esi+eax*8+8]

		mov eax, [esi+ecx+SHAPEHEADER.xmax]
		mov ebx, [esi+ecx+SHAPEHEADER.xmin]
		mov X1, eax
		mov X, ebx
		sub eax, ebx
		inc eax
		mov maxX, eax

		mov eax, [esi+ecx+SHAPEHEADER.ymax]
		mov ebx, [esi+ecx+SHAPEHEADER.ymin]
		mov Y, ebx
		sub eax, ebx
		inc eax
		mov maxY, eax

		}
	tempWINDOW.buffer = (uint8_t*)tempBuffer;
	tempWINDOW.x_max = maxX - 1;
	tempWINDOW.y_max = maxY - 1;
	tempPANE.window = &tempWINDOW;
	tempPANE.x0 = 0;
	tempPANE.y0 = 0;
	tempPANE.x1 = maxX - 1;
	tempPANE.y1 = maxY - 1;
	//-------------------------------------------------------------------------------------
	// Some armor here.  We have still have a bug somewhere that trashes the
	// shape by the time it gets here.  Armor against for now and make Fatal to
	// try to track down!
#define MAX_X 360
#define MAX_Y 360
	if ((maxX * maxY) >= (MAX_Y * MAX_X))
	{
		return;
	}
	//-------------------------------------------------------------------------------------
	// memclear(tempBuffer,maxX*maxY);
	memset(tempBuffer, 0, maxX * maxY);
	AG_shape_translate_fill(&tempPANE, shapeTable, frameNum, -X, -Y);
	if (scaleUp)
	{
		if (reverse)
			CopySprite(globalPane, tempBuffer, hotX - X1, hotY + Y, maxX, maxY, reverse, scaleUp);
		else
			CopySprite(globalPane, tempBuffer, hotX + X, hotY + Y, maxX, maxY, reverse, scaleUp);
	}
	else
	{
		if (reverse)
			CopySprite(globalPane, tempBuffer, hotX - (X1 >> 1), hotY + (Y >> 1), maxX, maxY,
				reverse, scaleUp);
		else
			CopySprite(globalPane, tempBuffer, hotX + (X >> 1), hotY + (Y >> 1), maxX, maxY,
				reverse, scaleUp);
	}
}

void CopySprite(PANE* pane, PVOID texture, int32_t X, int32_t Y, int32_t width, int32_t height,
	int32_t Flip, int32_t ScaleUp)
{
	Destwidth = pane->window->x_max + 1;
	int32_t paneX0 = (pane->x0 < 0) ? 0 : pane->x0;
	int32_t paneY0 = (pane->y0 < 0) ? 0 : pane->y0;
	int32_t paneX1 = (pane->x1 >= (int32_t)Destwidth) ? pane->window->x_max : pane->x1;
	int32_t paneY1 = (pane->y1 >= (pane->window->y_max + 1)) ? pane->window->y_max : pane->y1;
	X += paneX0;
	Y += paneY0;
	SourcePointer = texture;
	Sourcewidth = width;
	if (X < paneX0)
	{
		if (ScaleUp)
		{
			width -= paneX0 - X;
			if (Flip) // Zoomed IN
				SourcePointer -= paneX0 - X;
			else
				SourcePointer += paneX0 - X;
		}
		else
		{
			width -= (paneX0 - X) * 2;
			if (Flip) // Zoomed OUT
				SourcePointer -= (paneX0 - X) * 2;
			else
				SourcePointer += (paneX0 - X) * 2;
		}
		X = paneX0;
	}
	if (Y < paneY0)
	{
		if (ScaleUp)
		{
			height -= paneY0 - Y;
			SourcePointer += (paneY0 - Y) * width;
			Y = paneY0;
		}
		else
		{
			height -= (paneY0 - Y) * 2;
			SourcePointer += ((paneY0 - Y) * 2) * width;
			Y = paneY0;
		}
	}
	if (ScaleUp)
	{
		if (X + width > (paneX1 + 1))
		{
			width = paneX1 + 1 - X;
		}
	}
	else
	{
		if (X + (width >> 1) > (paneX1 + 1))
		{
			width = (paneX1 + 1 - X) << 1;
		}
	}
	if (ScaleUp)
	{
		if (Y + height > (paneY1 + 1))
			height = paneY1 + 1 - Y;
	}
	else
	{
		if (Y + (height >> 1) > (paneY1 + 1))
			height = (paneY1 + 1 - Y) << 1;
	}
	if ((X >= paneX1) || (Y >= paneY1) || (X <= (paneX0 - width)) || (Y <= (paneY0 - height)) || (width <= 0) || (height <= 0))
		return;
	DestPointer = pane->window->buffer + X + Y * Destwidth;
	twidth = width;
	theight = height;
	_asm {

		push ebp
		mov eax, Flip
		mov ebx, ScaleUp

		test eax, eax
		jnz DoFlip
		test ebx, ebx
		jnz UnFlipNormal
		;
		;
		Not Flipped, Shrunk
		;
		mov ebx, theight
		mov edi, DestPointer

		mov esi, SourcePointer
		mov ebp, twidth

		shr ebx, 1
		jz Done
		shr ebp, 1
		jz Done

		xor eax, eax
		mov edx, ebp

		tl0:
		mov al, [edi]
		inc edi

		mov ah, [esi]
		add esi, 2

		mov cl, AlphaTable[eax]
		dec ebp

		mov [edi-1], cl
		jnz tl0

		mov ecx, Destwidth
		sub esi, edx

		sub esi, edx
		add edi, ecx

		sub edi, edx
		mov ecx, Sourcewidth

		mov ebp, edx
		dec ebx

		lea esi, [esi+ecx*2]
		jnz tl0

		jmp Done
		;
		;
		Not flipped, not shrunk
		;
		UnFlipNormal:
		mov edi, DestPointer
		mov esi, SourcePointer
		mov ebx, theight
		mov ebp, twidth
		xor eax, eax
		mov edx, ebp

		tl1:
		mov al, [edi]
		inc edi

		mov ah, [esi]
		inc esi

		cmp ah, 0
		je  SKIPME1

			// mov cl,AlphaTable[eax]
		mov [edi-1], ah

		SKIPME1:
		dec ebp
		jnz tl1

		mov ecx, Destwidth
		sub edi, edx

		add edi, ecx
		mov ebp, edx

		sub esi, edx
		mov ecx, Sourcewidth

		add esi, ecx
		dec ebx

		jnz tl1
		jmp Done


		DoFlip:
		test ebx, ebx
		jnz FlipNormal
		;
		;
		Flipped, Shrunk
		;
		mov ebp, twidth
		mov ebx, theight

		shr ebp, 1
		jz Done
		shr ebx, 1
		jz Done

		mov eax, Sourcewidth
		mov esi, SourcePointer

		mov edi, DestPointer
		mov edx, ebp

		lea esi, [esi+eax*2-1]
		xor eax, eax

		tl2:
		mov al, [edi]
		inc edi

		mov ah, [esi]
		sub esi, 2

		mov cl, AlphaTable[eax]
		dec ebp

		mov [edi-1], cl
		jnz tl2

		mov ecx, Destwidth
		mov ebp, edx

		add esi, edx
		add edi, ecx

		add esi, edx
		mov ecx, Sourcewidth

		sub edi, edx
		dec ebx

		lea esi, [esi+ecx*2]
		jnz tl2

		jmp Done

		;
		;
		Flipped, not shrunk
		;
		FlipNormal:

		mov eax, Sourcewidth
		mov esi, SourcePointer
		mov edi, DestPointer
		mov ebp, twidth
		mov ebx, theight
		lea esi, [esi+eax-1]
		xor eax, eax
		mov edx, ebp

		tl3:
		mov al, [edi]
		inc edi

		mov ah, [esi]
		dec esi

		cmp ah, 0
		je  SKIPME2
			// mov cl,AlphaTable[eax]

		mov [edi-1], ah

		SKIPME2:
		dec ebp
		jnz tl3

		sub edi, edx
		mov ecx, Destwidth

		mov ebp, edx
		add edi, ecx

		mov ecx, Sourcewidth
		add esi, edx

		add esi, ecx
		dec ebx

		jnz tl3

		done:
		pop ebp
	}
	return;
}

/*
;
; int32_t cdecl VFX_shape_draw (PANE *panep, PVOIDshape_table,
;                           int32_t shape_number,int32_t hotX, int32_t hotY)
;
; This function clips and draws a shape to a pane.
;
; The panep parameter specifies the pane.
;
; The shape parameter specifies the shape, which must be in VFX Shape format.
;
; The hotX and hotY parameters specify the location where the shape is to be
; drawn.  The shape's hot spot will end up at the specified location.
;
*/
void AG_shape_fill(PANE* pane, PVOIDshape_table, int32_t shape_number, int32_t hotX, int32_t hotY)
{
	_asm
	{
		mov edi, pane
		nop
		;
		;
		Clip left and right of clipping window
		;
		mov ecx, [edi+PANE.x0]
		mov edx, [edi+PANE.y0]

		mov ebx, ecx
		mov eax, edx

		sar ebx, 31
		xor eax, -1

		sar eax, 31				;
		If less than 0, make 0
		xor ebx, -1

		and ecx, ebx
		and edx, eax

		mov paneX0, ecx
		mov paneY0, edx

		;
		;
		Clip top and bottom of clipping window
		;
		mov ecx, [edi+PANE.x1]
		mov edx, [edi+PANE.y1]

		mov edi, [edi+PANE.window]
		mov esi, shape_table

		mov eax, [edi+WINDOW.x_max]
		xor ebx, ebx

		inc eax
		nop

		sub ecx, eax
		mov Destwidth, eax

		setge bl

		dec ebx					;
		if ecx is less than eax, load ecx with eax
		nop

		and ecx, ebx
		mov ebx, [edi+WINDOW.y_max]

			add ecx, eax
			inc ebx

			xor eax, eax
			sub edx, ebx

			setge al

			dec eax
			mov paneX1, ecx

			and edx, eax
			mov eax, shape_number

			add edx, ebx
			mov ebx, paneY0

			;
		;
		paneX0, Y0 to PaneX1, Y1 are 0, 0 -> 639, 479 or window size to render too
		;
		mov ecx, [esi+eax*8+8]	;
		ESI now points to start of sprite data
		mov paneY1, edx

		lea esi, [esi+ecx+SIZE SHAPEHEADER]
		mov edx, hotY

		mov ecx, [esi+SHAPEHEADER.xmax-SIZE SHAPEHEADER]
		mov eax, [esi+SHAPEHEADER.xmin-SIZE SHAPEHEADER]

		mov tempXmax, ecx					;
		Store Xmax and Xmin
		mov tempXmin, eax

		mov ecx, [esi+SHAPEHEADER.ymax-SIZE SHAPEHEADER]
		mov eax, [esi+SHAPEHEADER.ymin-SIZE SHAPEHEADER]

		sub ecx, eax							;
		ecx = height of sprite
		add eax, edx							;
		eax = top line

		inc ecx								;
		Add one line
		nop
		;
		;
		Now check for lines off the top of the clipping window
		;
	cmp eax, ebx
	jl ClippedTop

	;
	;
	Now check for lines off the bottom of the clipping window
		;
	rw5:
	lea ebx, [eax+ecx-1]					;
		ebx=Last Line
		nop

		sub ebx, paneY1						;
		Check to see if off bottom
		ja ClippedBottom
		rw6:
		mov lines, ecx						;
	eax still equals top line

	;
	;
	Now check clipping in X
	;
	mov ebx, tempXmax

	mov ecx, tempXmin
	mov edx, paneX0

	sub ebx, ecx							;
	ebx = width of sprite
	add ecx, hotX						;
	ecx = offset to left edge

	cmp ecx, edx							;
	Is sprite off left edge of screen?
	jl ClippedLeft

	lea edx, [ebx+ecx-1]
		mov edi, [edi+WINDOW.buffer]			;
		edi points to top left of buffer

		sub edx, paneX1
		jnbe ClippedRight

		;
		;
		Work out screen position
		;
		//NowDraw:
		imul Destwidth

		add eax, ecx
		xor ecx, ecx

		add edi, eax
		mov StartofLine, edi

		;
		;
		;
		Main drawing loop
		;
		;
		lineLoop:
		mov al, [esi]
		inc esi
		shr al, 1
		ja RunPacket
		jnz StringPacket
		jnc EndPacket

		//SkipPacket:
		xor ecx, ecx
		mov al, [esi]
		inc esi
		xor cl, cl
		rp1a:
		mov [edi], cl		;
		Fill with 0 instead of skipping
		inc edi
		dec al
		jnz rp1a
		jmp lineLoop

		jmp RunPacket		;
		Alignement problem with rp1 otherwise

		RunPacket:
		mov cl, [esi]
		inc esi
		rp1:
		mov [edi], cl
		inc edi
		dec al
		jnz rp1
		jmp lineLoop

		StringPacket:
		//
		// 17 cycles / 8 bytes   - 2.125 per byte
		//
		sub al, 8
		jc sp2
		sp1:
		mov ecx, [esi]
		mov ebx, [esi+4]

		mov [edi], ecx
		mov [edi+4], ebx

		add esi, 8
		add edi, 8

		sub al, 8
		jnc sp1

		sp2:
		add al, 8
		jz lineLoop

		sp3:
		mov cl, [esi]
		inc esi

		mov [edi], cl
		inc edi

		dec al
		jnz sp3
		jmp lineLoop

		EndPacket:
		mov edx, Destwidth
		mov edi, StartofLine

		add edi, edx
		mov edx, lines

		dec edx
		mov StartofLine, edi

		mov lines, edx
		jnz lineLoop
		jmp Exit




		;
		;
		;
		Lines are clipped off the bottom of the clip window
		;
		;
		ClippedBottom:
		sub ecx, ebx							;
		Remove lines that go off bottom
		jbe Exit
		jmp rw6
		;
		;
		;
		Lines are off the top of the clip window
		;
		;
		ClippedTop:
		sub ebx, eax							;
		ebx=Lines to skip
		xor eax, eax
		sub ecx, ebx							;
		ecx=Lines in sprite
		jbe Exit							;
		Off top of screen
		rw8:
		mov al, [esi]
		add esi, 2
		shr al, 1
		ja rw8
		jnz rw8StringPacket					;
		Skip over lines in sprite data
		jc rw8
		dec esi
		dec ebx
		jnz rw8
		mov eax, paneY0						;
		eax=top line
		jmp rw5
		rw8StringPacket:
		lea esi, [esi+eax-1]
		jmp rw8
		;
		;
		;
		Sprite is clipped on either left or right
		;
		;
		ClippedLeft:
		sub edx, ecx
		mov edi, [edi+WINDOW.buffer]			;
		edi points to top left of buffer
		cmp ebx, edx
		jbe Exit							;
		Completly off left of screen?
		jmp ClippedX

		ClippedRight:
		cmp ecx, edx							;
		Completly off right of screen?
		jbe Exit

		ClippedX:
		imul Destwidth

		add edi, eax
		mov eax, paneX0

		mov edx, paneX1
		add eax, edi

		add edx, edi							;
		eax=Last pixel on right edge of clip window
		mov StartofClip, eax

		mov EndofClip, edx
		add edi, ecx

		xor ecx, ecx
		mov StartofLine, edi

		;
		;
		Clipped left and / or right drawing loop
		;
		clineLoop:
		mov al, [esi]
		inc esi
		shr al, 1
		ja cRunPacket
		jnz cStringPacket
		jnc cEndPacket


		//cSkipPacket:
		xor ecx, ecx
		mov al, [esi]
		inc esi
		xor cl, cl
		crp1a:
		cmp edi, StartofClip
		jc crp2a
		cmp edi, EndofClip
		jnbe clineLoop
		mov [edi], cl
		crp2a:
		inc edi
		dec al
		jnz crp1a
		jmp clineLoop


		cRunPacket:
		mov cl, [esi]
		inc esi
		crp1:
		cmp edi, StartofClip
		jc crp2
		cmp edi, EndofClip
		jnbe clineLoop
		mov [edi], cl
		crp2:
		inc edi
		dec al
		jnz crp1
		jmp clineLoop


		cStringPacket:
		mov cl, [esi]
		inc esi
		cmp edi, StartofClip
		jc crp3
		cmp edi, EndofClip
		jnbe crp3a
		mov [edi], cl
		crp3:
		inc edi
		dec al
		jnz cStringPacket
		jmp clineLoop

		crp3a:
		and eax, 255
		lea esi, [esi+eax-1]
		jmp clineLoop


		cEndPacket:
		mov edx, Destwidth
		mov eax, EndofClip

		add eax, edx
		mov edi, StartofLine

		mov EndofClip, eax
		nop

		mov eax, StartofClip
		add edi, edx

		add eax, edx
		mov edx, lines

		mov StartofLine, edi
		dec edx

		mov StartofClip, eax
		mov lines, edx

		jnz clineLoop

		Exit:
	}
}

/*
;----------------------------------------------------------------------------
;
; int32_t cdecl VFX_shape_translate_draw (PANE *panep, PVOIDshape_table,
;                           int32_t shape_number,int32_t hotX, int32_t hotY)
;
; This function clips and draws a shape to a pane.  It is identical to
; VFX_shape_draw(), except that each pixel written is translated through a
; 256-byte table which was specified by a prior call to VFX_shape_lookaside().
;
; The panep parameter specifies the pane.
;
; The shape parameter specifies the shape, which must be in VFX Shape format.
;
; The hotX and hotY parameters specify the location where the shape is to be
; drawn.  The shape's hot spot will end up at the specified location.
;
; For more information, see the "VFX Shape Format Description".
;
; Return values:
;
;    0: OK
;   -1: Bad window
;   -2: Bad pane
;   -3: Shape off pane
;   -4: Null shape
;
;----------------------------------------------------------------------------
*/
void AG_shape_translate_fill(
	PANE* pane, PVOIDshape_table, int32_t shape_number, int32_t hotX, int32_t hotY)
{
	_asm
	{
		mov edi, pane
		nop
		;
		;
		Clip left and right of clipping window
		;
		mov ecx, [edi+PANE.x0]
		mov edx, [edi+PANE.y0]

		mov ebx, ecx
		mov eax, edx

		sar ebx, 31
		xor eax, -1

		sar eax, 31				;
		If less than 0, make 0
		xor ebx, -1

		and ecx, ebx
		and edx, eax

		mov paneX0, ecx
		mov paneY0, edx

		;
		;
		Clip top and bottom of clipping window
		;
		mov ecx, [edi+PANE.x1]
		mov edx, [edi+PANE.y1]

		mov edi, [edi+PANE.window]
		mov esi, shape_table

		mov eax, [edi+WINDOW.x_max]
		xor ebx, ebx

		inc eax
		nop

		sub ecx, eax
		mov Destwidth, eax

		setge bl

		dec ebx					;
		if ecx is less than eax, load ecx with eax
		nop

		and ecx, ebx
		mov ebx, [edi+WINDOW.y_max]

			add ecx, eax
			inc ebx

			xor eax, eax
			sub edx, ebx

			setge al

			dec eax
			mov paneX1, ecx

			and edx, eax
			mov eax, shape_number

			add edx, ebx
			mov ebx, paneY0

			;
		;
		paneX0, Y0 to PaneX1, Y1 are 0, 0 -> 639, 479 or window size to render too
		;
		mov ecx, [esi+eax*8+8]	;
		ESI now points to start of sprite data
		mov paneY1, edx

		lea esi, [esi+ecx+SIZE SHAPEHEADER]
		mov edx, hotY

		mov ecx, [esi+SHAPEHEADER.xmax-SIZE SHAPEHEADER]
		mov eax, [esi+SHAPEHEADER.xmin-SIZE SHAPEHEADER]

		mov tempXmax, ecx					;
		Store Xmax and Xmin
		mov tempXmin, eax

		mov ecx, [esi+SHAPEHEADER.ymax-SIZE SHAPEHEADER]
		mov eax, [esi+SHAPEHEADER.ymin-SIZE SHAPEHEADER]

		sub ecx, eax							;
		ecx = height of sprite
		add eax, edx							;
		eax = top line

		inc ecx								;
		Add one line
		nop
		;
		;
		Now check for lines off the top of the clipping window
		;
	cmp eax, ebx
	jl ClippedTop

	;
	;
	Now check for lines off the bottom of the clipping window
		;
	rw5:
	lea ebx, [eax+ecx-1]					;
		ebx=Last Line
		nop

		sub ebx, paneY1						;
		Check to see if off bottom
		ja ClippedBottom
		rw6:
		mov lines, ecx						;
	eax still equals top line

	;
	;
	Now check clipping in X
	;
	mov ebx, tempXmax

	mov ecx, tempXmin
	mov edx, paneX0

	sub ebx, ecx							;
	ebx = width of sprite
	add ecx, hotX						;
	ecx = offset to left edge

	cmp ecx, edx							;
	Is sprite off left edge of screen?
	jl ClippedLeft

	lea edx, [ebx+ecx-1]
		mov edi, [edi+WINDOW.buffer]			;
		edi points to top left of buffer

		sub edx, paneX1
		jnc ClippedRight

		;
		;
		Work out screen position
		;
		//NowDraw:
		imul Destwidth

		add eax, ecx
		xor ecx, ecx

		add edi, eax
		mov StartofLine, edi

		push ebp
		mov ebp, lookaside

		xor ebx, ebx
		nop
		;
		;
		Main drawing loop
		;
		;
		lineLoop:
		mov al, [esi]
		inc esi
		shr al, 1
		ja RunPacket
		jnz StringPacket
		jnc EndPacket

		//SkipPacket:
		xor ecx, ecx
		mov al, [esi]
		inc esi
		xor cl, cl
		rp1a:
		mov [edi], cl
		inc edi
		dec al
		jnz rp1a
		jmp lineLoop

		RunPacket:
		xor ecx, ecx
		mov cl, [esi]
		inc esi
		mov cl, [ecx+ebp]
		rp1:
		mov [edi], cl
		inc edi
		dec al
		jnz rp1
		jmp lineLoop

		StringPacket:
		//
		// 17 cycles / 8 bytes   - 2.125 per byte
		//
		sub al, 8
		jc sp2
		sp1:
		mov cl, [esi]
		mov bl, [esi+4]

		mov cl, [ecx+ebp]
		mov bl, [ebx+ebp]

		mov [edi], cl
		mov [edi+4], bl

		mov cl, [esi+1]
		mov bl, [esi+5]

		mov cl, [ecx+ebp]
		mov bl, [ebx+ebp]

		mov [edi+1], cl
		mov [edi+5], bl

		mov cl, [esi+2]
		mov bl, [esi+6]

		mov cl, [ecx+ebp]
		mov bl, [ebx+ebp]

		mov [edi+2], cl
		mov [edi+6], bl

		mov cl, [esi+3]
		mov bl, [esi+7]

		add esi, 8
		add edi, 8

		mov cl, [ecx+ebp]
		mov bl, [ebx+ebp]

		mov [edi+3-8], cl
		mov [edi+7-8], bl

		sub al, 8
		jnc sp1

		sp2:
		add al, 8
		jz lineLoop

		sp3:
		xor ecx, ecx
		mov cl, [esi]

		mov cl, [ecx+ebp]
		inc esi

		mov [edi], cl
		inc edi

		dec al
		jnz sp3
		jmp lineLoop

		EndPacket:
		mov edx, Destwidth
		mov edi, StartofLine

		add edi, edx
		mov edx, lines

		dec edx
		mov StartofLine, edi

		mov lines, edx
		jnz lineLoop

		pop ebp
		jmp Exit




		;
		;
		;
		Lines are clipped off the bottom of the clip window
		;
		;
		ClippedBottom:
		sub ecx, ebx							;
		Remove lines that go off bottom
		jbe Exit
		jmp rw6
		;
		;
		;
		Lines are off the top of the clip window
		;
		;
		ClippedTop:
		sub ebx, eax							;
		ebx=Lines to skip
		xor eax, eax
		sub ecx, ebx							;
		ecx=Lines in sprite
		jbe Exit							;
		Off top of screen
		rw8:
		mov al, [esi]
		add esi, 2
		shr al, 1
		ja rw8
		jnz rw8StringPacket					;
		Skip over lines in sprite data
		jc rw8
		dec esi
		dec ebx
		jnz rw8
		mov eax, paneY0						;
		eax=top line
		jmp rw5
		rw8StringPacket:
		lea esi, [esi+eax-1]
		jmp rw8
		;
		;
		;
		Sprite is clipped on either left or right
		;
		;
		ClippedLeft:
		sub edx, ecx
		mov edi, [edi+WINDOW.buffer]			;
		edi points to top left of buffer
		cmp ebx, edx
		jbe Exit							;
		Completly off left of screen?
		jmp ClippedX

		ClippedRight:
		cmp ecx, edx							;
		Completly off right of screen?
		jbe Exit

		ClippedX:
		imul Destwidth

		add edi, eax
		mov eax, paneX0

		mov edx, paneX1
		add eax, edi

		add edx, edi							;
		eax=Last pixel on right edge of clip window
		mov StartofClip, eax

		mov EndofClip, edx
		add edi, ecx

		xor ecx, ecx
		mov StartofLine, edi

		push ebp
		mov ebp, lookaside
		;
		;
		Clipped left and / or right drawing loop
		;
		clineLoop:
		mov al, [esi]
		inc esi
		shr al, 1
		ja cRunPacket
		jnz cStringPacket
		jnc cEndPacket


		//cSkipPacket:
		mov al, [esi]
		inc esi
		xor cl, cl
		crp1a:
		cmp edi, StartofClip
		jc crp2a
		cmp edi, EndofClip
		jnc clineLoop
		mov [edi], cl
		crp2a:
		inc edi
		dec al
		jnz crp1a
		jmp clineLoop


		cRunPacket:
		mov cl, [esi]
		inc esi
		crp1:
		cmp edi, StartofClip
		jc crp2
		cmp edi, EndofClip
		jnc clineLoop
		mov cl, [ecx+ebp]
		mov [edi], cl
		crp2:
		inc edi
		dec al
		jnz crp1
		jmp clineLoop


		cStringPacket:
		mov cl, [esi]
		inc esi
		cmp edi, StartofClip
		jc crp3
		cmp edi, EndofClip
		jnc crp3a
		mov cl, [ecx+ebp]
		mov [edi], cl
		crp3:
		inc edi
		dec al
		jnz cStringPacket
		jmp clineLoop

		crp3a:
		and eax, 255
		lea esi, [esi+eax-1]
		jmp clineLoop


		cEndPacket:
		mov edx, Destwidth
		mov eax, EndofClip

		add eax, edx
		mov edi, StartofLine

		mov EndofClip, eax
		nop

		mov eax, StartofClip
		add edi, edx

		add eax, edx
		mov edx, lines

		mov StartofLine, edi
		dec edx

		mov StartofClip, eax
		mov lines, edx

		jnz clineLoop
		pop ebp

		Exit:
	}
}
