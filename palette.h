/* palette.h, Palette get/set functions for mapping 8bit bitmap colours
 to those available in the pc98Launcher.
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

#ifndef __HAS_BMP
#include "bmp.h"
#define __HAS_BMP
#endif

#define PALETTE_VERBOSE			0
#define PALETTES_TOTAL			256 // Total number of palette entries
#define PALETTES_RESERVED		32	// Used by the UI bitmap resources
#define PALETTES_UI				16	// Free to be used by gfx primitives etc
#define PALETTES_FREE			208	// Free to be used by artwork and screenshots

#define PALETTE_OK				0
#define PALETTE_NO_PIXELS		1

// 16 colours for drawing UI elements etc
#define PALETTE_UI_BLACK			PALETTES_FREE + PALETTES_RESERVED 
#define PALETTE_UI_WHITE			PALETTES_FREE + PALETTES_RESERVED + 1
#define PALETTE_UI_LGREY			PALETTES_FREE + PALETTES_RESERVED + 2
#define PALETTE_UI_DGREY			PALETTES_FREE + PALETTES_RESERVED + 3
#define PALETTE_UI_RED			PALETTES_FREE + PALETTES_RESERVED + 4
#define PALETTE_UI_GREEN			PALETTES_FREE + PALETTES_RESERVED + 5
#define PALETTE_UI_BLUE			PALETTES_FREE + PALETTES_RESERVED + 6
#define PALETTE_UI_YELLOW		PALETTES_FREE + PALETTES_RESERVED + 7
#define PALETTE_UI_9				PALETTES_FREE + PALETTES_RESERVED + 8
#define PALETTE_UI_10			PALETTES_FREE + PALETTES_RESERVED + 9
#define PALETTE_UI_11			PALETTES_FREE + PALETTES_RESERVED + 10
#define PALETTE_UI_12			PALETTES_FREE + PALETTES_RESERVED + 11
#define PALETTE_UI_13			PALETTES_FREE + PALETTES_RESERVED + 12
#define PALETTE_UI_14			PALETTES_FREE + PALETTES_RESERVED + 13
#define PALETTE_UI_15			PALETTES_FREE + PALETTES_RESERVED + 14
#define PALETTE_UI_16			PALETTES_FREE + PALETTES_RESERVED + 15


unsigned int free_palettes_used;			// Current number of palette entries used
unsigned int reserved_palettes_used;		// Current number of palette entries used


int 		pal_BMP2Palette(bmpdata_t *bmpdata, int reserved);
void 	pal_ResetAll();
void 	pal_ResetFree();
void 	pal_Set(unsigned char idx, unsigned char r, unsigned char g, unsigned char b);
