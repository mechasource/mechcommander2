//0000000000000000000000000000000000000000000000000000000000000000000000000000
//00                                                                        00
//00  VFX.H: C type definitions & API function prototypes                   00
//00                                                                        00
//00  Source compatible with 32-bit 80386 C/C++                             00
//00                                                                        00
//00  V0.10 of 10-Dec-92: Initial version                                   00
//00  V1.01 of 12-May-93: Added VFX_shape_lookaside(), new polygon calls    00
//00                      PCX_draw() returns void                           00
//00  V1.02 of 18-Jun-93: Added rotation/scaling calls                      00
//00                      intervals parameter added to window_fade()        00
//00                      Added VFX_RECT structure                          00
//00  V1.03 of 28-Jul-93: VERTEX2D structure changed to SCRNVERTEX          00
//00                      Fixed-VFX_POINT data types renamed                00
//00                      Added VFX_bank_reset() call                       00
//00  V1.04 of  4-Sep-93: Indirect function prototypes changed for C++ use  00
//00  V1.05 of 26-Sep-93: Added FLOAT typedef                               00
//00                      VFX_RGB UCHAR members changed to UCHARs           00
//00  V1.06 of 13-Oct-93: Added VFX_pane_refresh(), other new calls         00
//00  V1.07 of 17-Nov-93: Added MetaWare High C support                     00
//00  V1.10 of  3-Dec-93: Modified VFX_pane_refresh(), WINDOW structure     00
//00  V1.15 of 13-Mar-94: Added new VFX.C function prototypes               00
//00                      Added new WINDOW members, PANE_LIST structure     00
//00                                                                        00
//00  Project: 386FX Sound & Light(TM)                                      00
//00   Author: Ken Arnold, John Miles, John Lemberger                       00
//00                                                                        00
//0000000000000000000000000000000000000000000000000000000000000000000000000000
//00                                                                        00
//00  Copyright (C) 1992-1994 Non-Linear Arts, Inc.                         00
//00                                                                        00
//00  Non-Linear Arts, Inc.                                                 00
//00  3415 Greystone #200                                                   00
//00  Austin, TX 78731                                                      00
//00                                                                        00
//00  (512) 346-9595 / FAX (512) 346-9596 / BBS (512) 454-9990              00
//00                                                                        00
//0000000000000000000000000000000000000000000000000000000000000000000000000000

#pragma once

#ifndef VFX_H
#define VFX_H

#ifndef TRUE
#define TRUE -1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// #define SHAPE_FILE_VERSION '01.1' // 1.10 backwards for big-endian compare

//
// MetaWare support
//

//#ifdef __HIGHC__
//#define  _CC(_REVERSE_PARMS | _NEAR_CALL)
//#pragma Global_aliasing_convention("_%r");
//#pragma Align_members(1)
//#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#pragma pack(1)
#endif

#ifndef VFX_CALL
#define VFX_CALL	__stdcall
#endif

#ifndef TYPEDEFS1
#define TYPEDEFS1
typedef int32_t 	FIXED16;	// 16:16 fixed-VFX_POINT type [-32K,+32K]
typedef int32_t 	FIXED30;	// 2:30 fixed-VFX_POINT type [-1.999,+1.999]
typedef int32_t*	PFIXED16;	// 16:16 fixed-VFX_POINT type [-32K,+32K]
typedef int32_t*	PFIXED30;	// 2:30 fixed-VFX_POINT type [-1.999,+1.999]
#endif

#define GIF_SCRATCH_SIZE 20526L   // Temp memory req'd for GIF decompression

#define S_BLACK			264

//
// VFX_map_polygon() flags
//
#define MP_XLAT      0x0001       // Use lookaside table (speed loss = ~9%)
#define MP_XP        0x0002       // Enable transparency (speed loss = ~6%)

//
// VFX_shape_transform() flags
//
#define ST_XLAT      0x0001       // Use shape_lookaside() table
#define ST_REUSE     0x0002       // Use buffer contents from prior call

//
// VFX_line_draw() modes
//  
#define LD_DRAW      0
#define LD_TRANSLATE 1
#define LD_EXECUTE   2

//
// VFX_pane_scroll() modes
//
#define PS_NOWRAP    0
#define PS_WRAP      1

#define NO_COLOR -1

//
// VFX_shape_visible_rectangle() mirror values
//

#define VR_NO_MIRROR 0
#define VR_X_MIRROR  1
#define VR_Y_MIRROR  2
#define VR_XY_MIRROR 3

//
// PANE_LIST.flags values
//

#define PL_FREE      0            // Free and available for assignment
#define PL_VALID     1            // Assigned; to be refreshed
#define PL_CONTAINED 2            // Contained within another pane; don't refresh

//
// VFX data structures
//
typedef uint8_t		STENCIL;
typedef puint8_t	PSTENCIL;

typedef struct _window {
	PUCHAR			buffer;
	int32_t			x_max;
	int32_t			y_max;
	PSTENCIL		stencil;
	puint8_t		shadow;
} WINDOW;
typedef WINDOW*		PWINDOW;

typedef struct _pane {
	PWINDOW			window;
	int32_t			x0;
	int32_t			y0;
	int32_t			x1;
	int32_t			y1;
} PANE;
typedef PANE*		PPANE;

typedef struct _pane_list {
	PPANE			array;
	PULONG			flags;
	size_t			size;
} PANE_LIST;
typedef PANE_LIST*	PPANE_LIST;

typedef union _flex_ptr {
	PVOID			v;
	puint8_t		b;
	puint16_t		w;
	puint32_t		d;
} FLEX_PTR;

typedef struct _vfx_desc {
	int32_t			scrn_width;
	int32_t			scrn_height;
	int32_t			bytes_per_pixel;
	int32_t			ncolors;
	int32_t			npages;
	uint32_t		flags;
} VFX_DESC;

typedef struct _vfx_rgb {
	uint8_t			r;
	uint8_t			g;
	uint8_t			b;
} VFX_RGB;
typedef VFX_RGB*	PVFX_RGB;

typedef struct _vfx_crgb {
	uint8_t			color;
	VFX_RGB			VFX_RGB;	// VFX_RGB VFX_RGB; ?
} VFX_CRGB;
typedef VFX_CRGB*	PVFX_CRGB;

typedef struct _vfx_point {
	int32_t			x;
	int32_t			y;
} VFX_POINT;
typedef VFX_POINT*	PVFX_POINT;

typedef struct _vfx_font {
	int32_t			version;
	int32_t			char_count;
	int32_t			char_height;
	int32_t			font_background;
} FONT;

// Vertex structure used by polygon primitives
typedef struct _scrnvertex {
	int32_t			x;		// Screen X
	int32_t			y;		// Screen Y
	FIXED16			c;		// Color/addition value used by some primitives
	FIXED16			u;		// Texture source X
	FIXED16			v;		// Texture source Y
	FIXED30			w;		// Homogeneous perspective divisor (unused by VFX3D)
} SCRNVERTEX;               
typedef SCRNVERTEX*	PSCRNVERTEX;

typedef struct _vfx_rect {
	int32_t x0;
	int32_t y0;
	int32_t x1;
	int32_t y1;
} VFX_RECT;

#define INT_TO_FIXED16(x)       (((int32_t)(int)(x)) << 16)
#define DOUBLE_TO_FIXED16(x)    ((int32_t) ((x) * 65536.0 + 0.5))
#define FIXED16_TO_DOUBLE(x)    (((double)(x)) / 65536.0)
#define FIXED16_TO_INT(x)       ((int) ((x)<0 ? -(-(x) >> 16) : (x) >> 16))
#define ROUND_FIXED16_TO_INT(x) ((int) ((x)<0 ? -((32768-(x)) >> 16) : ((x)+32768) >> 16))

#define FIXED16_TO_FIXED30(x)   ((x) << 14)
#define FIXED30_TO_FIXED16(x)   ((x) >> 14)
#define FIXED30_TO_DOUBLE(x)    (((double)x) / 1073741824.0)
#define DOUBLE_TO_FIXED30(x)    ((int32_t) (x * 1073741824.0 + 0.5))

#define PIXELS_IN_PANE(pane)    (((pane).x1-(pane).x0+1)*((pane).y1-(pane).y0+1))
#define PIXELS_IN_PANEP(pane)   (((pane)->x1-(pane)->x0+1)*((pane)->y1-(pane)0>y0+1))

//
// Hardware-specific VFX DLL functions
//

/*
extern VFX_DESC *  (VFX_CALL *VFX_describe_driver)(void);
extern void  (VFX_CALL *VFX_init_driver)(void);
extern void  (VFX_CALL *VFX_shutdown_driver)(void);
extern void  (VFX_CALL *VFX_wait_vblank)(void);
extern void  (VFX_CALL *VFX_wait_vblank_leading)(void);
extern void  (VFX_CALL *VFX_area_wipe)(LONG x0, LONG y0, LONG x1, LONG y1, LONG color);
extern void  (VFX_CALL *VFX_window_refresh)(PWINDOW target, LONG x0, LONG y0, LONG x1, LONG y1);
extern void  (VFX_CALL *VFX_window_read)(PWINDOW destination, LONG x0, LONG y0, LONG x1, LONG y1);
extern void  (VFX_CALL *VFX_pane_refresh)(PPANE target, LONG x0, LONG y0, LONG x1, LONG y1);
extern void  (VFX_CALL *VFX_DAC_read)(LONG color_number, PVFX_RGB triplet);
extern void  (VFX_CALL *VFX_DAC_write)(LONG color_number, PVFX_RGB triplet);
extern void  (VFX_CALL *VFX_bank_reset)(void);
extern void  (VFX_CALL *VFX_line_address)(LONG x, LONG y, puint8_t *addr, ULONG *nbytes);
*/

//
// Device-independent VFX API functions (VFXC.C)
//
extern ULONG	VFX_CALL VFX_stencil_size(PWINDOW source, COLORREF transparent_color);
extern PSTENCIL VFX_CALL VFX_stencil_construct(PWINDOW source, PVOID dest, COLORREF transparent_color);
extern void 	VFX_CALL VFX_stencil_destroy(PSTENCIL stencil);
extern PWINDOW	VFX_CALL VFX_window_construct(int32_t width, int32_t height);
extern void 	VFX_CALL VFX_window_destroy(PWINDOW window);
extern PPANE	VFX_CALL VFX_pane_construct(PWINDOW window, int32_t x0, int32_t y0, int32_t x1, int32_t y1);
extern void 	VFX_CALL VFX_pane_destroy(PPANE pane);
extern PPANE_LIST VFX_CALL VFX_pane_list_construct(int32_t n_entries);
extern void 	VFX_CALL VFX_pane_list_destroy(PPANE_LIST list);
extern void 	VFX_CALL VFX_pane_list_clear(PPANE_LIST list);
extern int32_t	VFX_CALL VFX_pane_list_add(PPANE_LIST list, PPANE target);
extern int32_t	VFX_CALL VFX_pane_list_add_area(PPANE_LIST list, PWINDOW window, int32_t x0, int32_t y0, int32_t x1, int32_t y1);
extern void 	VFX_CALL VFX_pane_list_delete_entry(PPANE_LIST list, int32_t entry_num);
extern void 	VFX_CALL VFX_pane_list_refresh(PPANE_LIST list);

//
// Device-independent VFX API functions (VFXA.ASM)
//
extern puint8_t	VFX_CALL VFX_driver_name(PVOID VFXScanDLL);
extern int32_t	VFX_CALL VFX_register_driver(PVOID DLLbase);
extern int32_t	VFX_CALL VFX_line_draw(PPANE pane, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t mode, int32_t parm);
extern int32_t	VFX_CALL VFX_shape_draw(PPANE pane, PVOID shape_table, int32_t shape_number, int32_t hotX, int32_t hotY);
extern int32_t	VFX_CALL VFX_nTile_draw(PPANE pane, PVOID tile, int32_t hotX, int32_t hotY, puint8_t fadeTable = 0);
extern int32_t	VFX_CALL VFX_newShape_count(PVOID shape);
extern void 	VFX_CALL VFX_shape_lookaside(puint8_t table);
extern int32_t	VFX_CALL VFX_shape_translate_draw(PPANE pane, PVOID shape_table, int32_t shape_number, int32_t hotX, int32_t hotY);
extern void 	VFX_CALL VFX_shape_remap_colors(PVOID shape_table, uint32_t shape_number);
extern void 	VFX_CALL VFX_shape_visible_rectangle(PVOID shape_table, int32_t shape_number, int32_t hotX, int32_t hotY, int32_t mirror, int32_t *rectangle);
extern int32_t	VFX_CALL VFX_shape_scan(PPANE pane, COLORREF transparentColor, int32_t hotX, int32_t hotY, PVOID buffer);
extern int32_t	VFX_CALL VFX_newShape_scan(PPANE shapePane, uint8_t xparentColor, uint8_t Hsx, uint8_t Hsy, puint8_t* shapeBuffer);
extern int32_t	VFX_CALL VFX_deltaShape_scan(PPANE shapePane, uint8_t xparentColor, uint8_t Hsx, uint8_t Hsy, puint8_t* shapeBuffer);
extern int32_t	VFX_CALL VFX_tile_scan(PPANE shapePane, uint8_t xparentColor, uint8_t Hsx, uint8_t Hsy, puint8_t* shapeBuffer);
extern int32_t	VFX_CALL VFX_shape_bounds(PVOID shape_table, int32_t shape_num);
extern int32_t	VFX_CALL VFX_shape_origin(PVOID shape_table, int32_t shape_num);
extern int32_t	VFX_CALL VFX_shape_resolution(PVOID shape_table, int32_t shape_num);
extern int32_t	VFX_CALL VFX_shape_minxy(PVOID shape_table, int32_t shape_num);
extern void 	VFX_CALL VFX_shape_palette(PVOID shape_table, int32_t shape_num, PVFX_RGB palette);
extern int32_t	VFX_CALL VFX_shape_colors(PVOID shape_table, int32_t shape_num, PVFX_CRGB colors);
extern int32_t	VFX_CALL VFX_shape_set_colors(PVOID shape_table, int32_t shape_number, PVFX_CRGB colors);
extern int32_t	VFX_CALL VFX_shape_count(PVOID shape_table);
extern int32_t	VFX_CALL VFX_shape_list(PVOID shape_table, puint32_t index_list);
extern int32_t	VFX_CALL VFX_shape_palette_list(PVOID shape_table, puint32_t index_list);
extern int32_t	VFX_CALL VFX_pixel_write(PPANE pane, int32_t x, int32_t y, COLORREF color);
extern int32_t	VFX_CALL VFX_pixel_read(PPANE pane, int32_t x, int32_t y);
extern int32_t	VFX_CALL VFX_rectangle_hash(PPANE pane, int32_t x0, int32_t y0, int32_t x1, int32_t y1, COLORREF color);
extern int32_t	VFX_CALL VFX_pane_wipe(PPANE pane, int32_t color);
extern int32_t	VFX_CALL VFX_pane_copy(PPANE source, int32_t sx, int32_t sy, PPANE target, int32_t tx, int32_t ty, int32_t fill);
extern int32_t	VFX_CALL VFX_pane_scroll(PPANE pane, int32_t dx, int32_t dy, int32_t mode, int32_t parm);
extern void 	VFX_CALL VFX_ellipse_draw(PPANE pane, int32_t xc, int32_t yc, int32_t width, int32_t height, int32_t color);
extern void 	VFX_CALL VFX_ellipse_fill(PPANE pane, int32_t xc, int32_t yc, int32_t width, int32_t height, int32_t color);
extern void 	VFX_CALL VFX_point_transform(PVFX_POINT in, PVFX_POINT out, PVFX_POINT origin, int32_t rot, int32_t x_scale, int32_t y_scale);
extern void 	VFX_CALL VFX_Cos_Sin(int32_t Angle, PFIXED16 Cos, PFIXED16 Sin);
extern void 	VFX_CALL VFX_fixed_mul(FIXED16 M1, FIXED16 M2, PFIXED16 result);
extern int32_t	VFX_CALL VFX_font_height(PVOID font);
extern int32_t	VFX_CALL VFX_character_width(PVOID font, int32_t character);
extern int32_t	VFX_CALL VFX_character_draw(PPANE pane, int32_t x, int32_t y, PVOID font, int32_t character, puint8_t color_translate);
extern void 	VFX_CALL VFX_string_draw(PPANE pane, int32_t x, int32_t y, PVOID font, PSTR string, puint8_t color_translate);
extern int32_t	VFX_CALL VFX_ILBM_draw(PPANE pane, puint8_t ILBM_buffer);
extern void 	VFX_CALL VFX_ILBM_palette(puint8_t ILBM_buffer, PVFX_RGB palette);
extern int32_t	VFX_CALL VFX_ILBM_resolution(puint8_t ILBM_buffer);
extern void 	VFX_CALL VFX_PCX_draw(PPANE pane, puint8_t PCX_buffer);
extern void 	VFX_CALL VFX_PCX_palette(puint8_t PCX_buffer, int32_t PCX_file_size, PVFX_RGB palette);
extern int32_t	VFX_CALL VFX_PCX_resolution(puint8_t PCX_buffer);
extern int32_t	VFX_CALL VFX_GIF_draw(PPANE pane, puint8_t GIF_buffer, PVOID GIF_scratch);
extern void 	VFX_CALL VFX_GIF_palette(puint8_t GIF_buffer, PVFX_RGB palette);
extern int32_t	VFX_CALL VFX_GIF_resolution(puint8_t GIF_buffer);
extern int32_t	VFX_CALL VFX_pixel_fade(PPANE source, PPANE destination, int32_t intervals, int32_t rnd);
extern void 	VFX_CALL VFX_window_fade(PWINDOW buffer, PVFX_RGB palette, int32_t intervals);
extern int32_t	VFX_CALL VFX_color_scan(PPANE pane, LPCOLORREF colors);
extern void 	VFX_CALL VFX_shape_transform(PPANE pane, PVOID shape_table, int32_t shape_number, int32_t hotX, int32_t hotY, PVOID buffer, int32_t rot, int32_t x_scale, int32_t y_scale, int32_t flags);

//
// VFX 3D polygon functions
//
extern void 	VFX_CALL VFX_flat_polygon(PPANE pane, int32_t vcnt, PSCRNVERTEX vlist);
extern void 	VFX_CALL VFX_Gouraud_polygon(PPANE pane, int32_t vcnt, PSCRNVERTEX vlist);
extern void 	VFX_CALL VFX_dithered_Gouraud_polygon(PPANE pane, FIXED16 dither_amount, int32_t vcnt, PSCRNVERTEX vlist);
extern void 	VFX_CALL VFX_map_lookaside(puint8_t table);
extern void 	VFX_CALL VFX_map_polygon(PPANE pane, int32_t vcnt, PSCRNVERTEX vlist, PWINDOW texture, uint32_t flags);
extern void 	VFX_CALL VFX_translate_polygon(PPANE pane, int32_t vcnt, PSCRNVERTEX vlist, PVOID lookaside);
extern void 	VFX_CALL VFX_illuminate_polygon(PPANE pane, FIXED16 dither_amount, int32_t vcnt, PSCRNVERTEX vlist);

#ifdef __cplusplus
}
#endif

//
// MetaWare support
//

//#ifdef __HIGHC__
//#pragma Global_aliasing_convention();
//#pragma Align_members(4)
//#endif

#ifdef WIN32
#pragma pack()
#endif

#endif


