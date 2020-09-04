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

#include "pegc.h"
#ifndef __HAS_BMP
#include "bmp.h"
#define __HAS_BMP
#endif

#define GFX_VERBOSE		0			// Turn on/off gfx-specific debug output
#define GFX_ROWS			400			// NUmbe of pixels in a row
#define GFX_COLS			640			// Number of pixels in a column
#define GFX_ROW_SIZE		GFX_COLS 	// NUmber of bytes in a row (pixels per row * 2)
#define GFX_COL_SIZE 	GFX_ROWS		// NUmber of bytes in a column
#define GFX_PIXEL_SIZE	1			// 1 byte per pixel

#define VRAM_START		0xC00000		// Start of graphics vram
#define VRAM_END			VRAM_START + (GFX_ROW_SIZE * GFX_COL_SIZE)		// End of graphics vram

#define RGB_BLACK		0x0000			// Simple RGB definition for a black 16bit pixel (5551 representation?)
#define RGB_WHITE		0xFFFF			// Simple RGB definition for a white 16bit pixel (5551 representation?)

#define GFX_ERR_UNSUPPORTED_BPP		-254
#define GFX_ERR_MISSING_BMPHEADER	-253
#define GFX_TEXT_OK           		-252 // Output of text data ok
#define GFX_TEXT_INVALID      		-251 // Attempted output of an unsupported font glyph (too wide, too heigh, etc)

unsigned char	*vram;				// Pointer to a location in the local graphics buffer
unsigned char	vram_buffer[(GFX_ROW_SIZE * GFX_COL_SIZE)];
__dpmi_meminfo	vram_dpmi;			// DPMI descriptor for far-memory location of framebuffer
int				vram_dpmi_selector;	// Memory region selector handle

/* **************************** */
/* Function prototypes */
/* **************************** */

int		gfx_Bitmap(int x, int y, bmpdata_t *bmpdata);
int 		gfx_Box(int x1, int y1, int x2, int y2, unsigned char palette);
int 		gfx_BoxFill(int x1, int y1, int x2, int y2, unsigned char palette);
int		gfx_BoxFillTranslucent(int x1, int y1, int x2, int y2, unsigned char palette);
void		gfx_Clear();
int		gfx_Close();
void		gfx_Flip();
int		gfx_GetXYaddr(int x, int y);
int		gfx_Init();
void		gfx_TextOff();
void		gfx_TextOn();