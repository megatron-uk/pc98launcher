/* gfx.h, GVRAM Graphical function prototypes for the pc98launcher.
 Copyright (C) 2020  John Snowdon
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <dpmi.h>

#ifndef __HAS_BMP
#include "bmp.h"
#define __HAS_BMP
#endif

#define GFX_VERBOSE		0				// Turn on/off gfx-specific debug output
#define GFX_ROWS		400				// NUmbe of pixels in a row
#define GFX_COLS		640				// Number of pixels in a column
#define GFX_ROW_SIZE	GFX_COLS 		// NUmber of bytes in a row (pixels per row * 2)
#define GFX_COL_SIZE 	GFX_ROWS		// NUmber of bytes in a column
#define GFX_PIXEL_SIZE	1				// 1 byte per pixel

#define VRAM_START		0xC00000		// Start of graphics vram
#define VRAM_END		VRAM_START + (GFX_ROW_SIZE * GFX_COL_SIZE)		// End of graphics vram

#define RGB_BLACK		0x0000			// Simple RGB definition for a black 16bit pixel (5551 representation?)
#define RGB_WHITE		0xFFFF			// Simple RGB definition for a white 16bit pixel (5551 representation?)

#define GFX_ERR_UNSUPPORTED_BPP		-254
#define GFX_ERR_MISSING_BMPHEADER		-253
#define GFX_TEXT_OK           				-252        // Output of text data ok
#define GFX_TEXT_INVALID      				-251        // Attempted output of an unsupported font glyph (too wide, too heigh, etc)

unsigned char	*vram;							// Pointer to a location in the local graphics buffer
unsigned char	vram_buffer[(GFX_ROW_SIZE * GFX_COL_SIZE)];
__dpmi_meminfo	vram_dpmi;						// DPMI descriptor for far-memory location of framebuffer
int				vram_dpmi_selector;				// Memory region selector handle

// =============================
//
// PEGC IO port addresses
//
// =============================

#define PEGC_FB_LOCATION_LOW			0x00F00000 // The VRAM framebuffer is located at 16MB if the 15-16MB hole is present
#define PEGC_FB_LOCATION_HIGH			0xFFF00000 // The VRAM framebuffer is located at 4095MB if the 15-16MB hole is missing, or we have >=16MB of RAM
#define PEGC_FB_SIZE						0x00080000 // Framebuffer is 512KB
#define PEGC_H_SIZE						640
#define PEGC_V_SIZE						400
#define PEGC_PAGE_0_OFFSET				0
#define PEGC_PAGE_1_OFFSET				PEGC_FB_SIZE / 2

#define MEMORY_HOLE_CHECK_ADDR		0x043B	// If bit 2 is set at this address, then the memory hole at 15-16MB is not present (it is being used by RAM)
#define GPU_STATUS_PORT					0x09A0	// Port for writing and reading back various graphics mode statuss (clock rate, mono/colour, planar or chunky mode, sync type etc)

// ===========================
//
// Status query values to be written to above port
//
// ===========================

#define GPU_STATUS_COLOUR_MODE		0x01
#define GPU_STATUS_SCREEN_ON_OFF		0x03
#define GPU_STATUS_PALETTE_MODE		0x04
#define GPU_STATUS_SYNC_MODE			0x05
#define GPU_STATUS_EGC_MODE			0x07
#define GPU_STATUS_GDC_CLOCK			0x09
#define GPU_STATUS_PIXEL_DEPTH			0x0A
#define GPU_STATUS_PLANAR_OR_CHUNKY	0x0B

// ===========================
//
// Control ports for various PEGC hardware
//
// ===========================

#define PEGC_MODE_ADDR					0x6A 	// Toggles planar or packed-pixel/chunky mode
#define PEGC_GDC_COMMAND_ADDR		0xA2	// Command port for GPU mode (start, stop, reset, zoom, scroll etc)
#define PEGC_DISP_SCREEN_SEL_ADDR		0xA4	// Select screen to display in multi-page mode
#define PEGC_DRAW_SCREEN_SEL_ADDR		0xA6	// Select screen to write to in multi-page mode
#define PEGC_PALLETE_SEL_ADDR			0xA8	// Selects the palette entry to use
#define PEGC_GREEN_ADDR				0xAA	// green component address
#define PEGC_RED_ADDR					0xAC	// red
#define PEGC_BLUE_ADDR					0xAE	// blue
#define PEGC_SCANFREQ_ADDR			0x09A8	// Set either 24KHz (640x400) or 31KHz (640x480) mode

#define GDC_COMMAND_RESET1			0x00
#define GDC_COMMAND_RESET2			0x01
#define GDC_COMMAND_STOP2				0x05
#define GDC_COMMAND_RESET3			0x09
#define GDC_COMMAND_STOP1				0x0C
#define GDC_COMMAND_START				0x0D


// Memory mapped IO ports to the PEGC hardware itself
#define PEGC_PIXELMODE_ADDR			0xE0100 // Sets the colour depth
#define PEGC_FB_CONTROL_ADDR			0xE0102 // Sets whether the linear VRAM framebuffer is at F00000h-F7FFFFh (set with 0x01) or not (0x00) 

// ===========================
//
// Control values to be written to above ports
//
// ===========================

#define PEGC_PIXELMODE_PACKED			0x00	// Set for packed pixel mode
#define PEGC_PIXELMODE_PLANAR			0x01	// Set for planar pixel mode

#define PEGC_SCANFREQ_24				0x00	// 24KHz
#define PEGC_SCANFREQ_31				0x01	// 31KHz

#define PEGC_FB_ON						0x01	// Enable PEGC VRAM linear framebuffer
#define PEGC_FB_OFF						0x00	// Disable PEGC VRAM linear framebuffer

#define PEGC_BPPMODE_16c				0x20	// Enable 16 colour mode
#define PEGC_BPPMODE_256c				0x21	// Enable 256 colour mode
#define PEGC_BPPMODE_DUAL_PAGE		0x68	// Enable single graphics page (in 640x400)
#define PEGC_BPPMODE_SINGLE_PAGE		0x69	// Enable single graphics page (in 640x400 or 640x480)

/* **************************** */
/* Function prototypes */
/* **************************** */

int		gfx_Bitmap(int x, int y, bmpdata_t *bmpdata);
void	gfx_Clear();
int		gfx_Close();
void	gfx_Flip();
int		gfx_GetXYaddr(int x, int y);
int		gfx_Init();
void	gfx_TextOff();
void	gfx_TextOn();