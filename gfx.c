/* gfx.c, Graphical functions for drawing the main screen for the pc98Launcher.
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

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include <go32.h>
#include <dpmi.h>

#include "gfx.h"
#include "pegc.h"
#include "utils.h"
#ifndef __HAS_BMP
#include "bmp.h"
#define __HAS_BMP
#endif

int gfx_Init(){
	// Initialise graphics to a set of configured defaults
	
	int status;
	
	if (GFX_VERBOSE){
		printf("%s.%d\t Initalising gfx mode\n", __FILE__, __LINE__);	
	}
	
	// HSYNC 24 KHz (640x400)
	outportb(PEGC_SCANFREQ_ADDR, PEGC_SCANFREQ_24);

	// 256 color mode
	outportb(PEGC_MODE_ADDR, 0x07);
	outportb(PEGC_MODE_ADDR, PEGC_BPPMODE_256c);
	outportb(PEGC_MODE_ADDR, PEGC_BPPMODE_DUAL_PAGE);
	outportb(PEGC_MODE_ADDR, 0x06);

	// Enable Packed Pixel mode
	_farpokeb(_dos_ds, PEGC_PIXELMODE_ADDR, PEGC_PIXELMODE_PACKED);

	// Set up DPMI mapper for vram framebuffer regionbui
	status = gfx_DPMI();
	if (status < 0){
		if (GFX_VERBOSE){
			printf("%s.%d\t Error, Unable to complete gfx initialisation\n", __FILE__, __LINE__);	
		}
		return -1;	
	}
	
	// Enable linear framebuffer at 16MB and 4095MB
	_farpokeb(_dos_ds, PEGC_FB_CONTROL_ADDR, PEGC_FB_ON);

	// Set screen 0 to be active for drawing and active for display
	outportb(PEGC_DRAW_SCREEN_SEL_ADDR, 0x00);
	outportb(PEGC_DISP_SCREEN_SEL_ADDR, 0x00);
	
	// Graphics mode on
	outportb(PEGC_GDC_COMMAND_ADDR, GDC_COMMAND_START);
	
	// Set local vram_buffer to empty 
	//memset(vram_buffer, 0, (GFX_ROW_SIZE * GFX_COL_SIZE));
	
	gfx_Clear();
	gfx_Flip();
	
	return 0;
}
        
int gfx_Close(){
	//  Return to text mode
	
	if (GFX_VERBOSE){
		printf("%s.%d\t Exiting gfx mode\n", __FILE__, __LINE__);	
	}
	_farpokeb(_dos_ds, PEGC_FB_CONTROL_ADDR, PEGC_FB_OFF);

	// 16 Color mode
	outportb(PEGC_MODE_ADDR, 0x07);
	outportb(PEGC_MODE_ADDR, PEGC_BPPMODE_16c);
	outportb(PEGC_MODE_ADDR, 0x06);

	// Graphics mode off
	outportb(PEGC_GDC_COMMAND_ADDR, GDC_COMMAND_STOP1);
	
	// Free DPMI mapping
	__dpmi_free_physical_address_mapping(&vram_dpmi);
	
	//  Clear anything we did to the screen
	gfx_Clear();
	
	// Text mode on
	gfx_TextOn();
	
	return 0;
}

void gfx_Clear(){
	
	// Set local vram_buffer to empty
	memset(vram_buffer, 0, (GFX_ROW_SIZE * GFX_COL_SIZE));
	
}

int gfx_DPMI(){
	// Enable DPMI mapper to framebuffer far location
	
	int pegc_fb_location;
	
	if (gfx_HasMemoryHole() < 0){
		if (GFX_VERBOSE){
			printf("%s.%d\t Memory hole at 16MB is not available\n", __FILE__, __LINE__);
			printf("%s.%d\t Using VRAM framebuffer at 0x%x\n", __FILE__, __LINE__, PEGC_FB_LOCATION_HIGH);
		}
		pegc_fb_location = PEGC_FB_LOCATION_HIGH;
	} else {
		if (GFX_VERBOSE){
			printf("%s.%d\t Memory hole at 16MB is available\n", __FILE__, __LINE__);
			printf("%s.%d\t Using VRAM framebuffer at 0x%x\n", __FILE__, __LINE__, PEGC_FB_LOCATION_LOW);
		}
		pegc_fb_location = PEGC_FB_LOCATION_LOW;
	}
		
	if (!__djgpp_nearptr_enable()){
		if (GFX_VERBOSE){
			printf("%s.%d\t DPMI Error; Unable to enable near pointers\n", __FILE__, __LINE__);
		}
		return -1;
	}
	
	vram_dpmi.address = pegc_fb_location;
	vram_dpmi.size    = PEGC_FB_SIZE;
	
	if (__dpmi_physical_address_mapping(&vram_dpmi) != 0){
		if (GFX_VERBOSE){
			printf("%s.%d\t DPMI Error; Unable to set DPMI physical address mapping\n", __FILE__, __LINE__);
		}
		return -1;
	}
	vram_dpmi_selector = __dpmi_allocate_ldt_descriptors(1);
	if (vram_dpmi_selector < 0){
		if (GFX_VERBOSE){
			printf("%s.%d\t DPMI Error; Unable to get DPMI ldt descriptor\n", __FILE__, __LINE__);
		}
		__dpmi_free_physical_address_mapping(&vram_dpmi);
		return -1;
	}
	__dpmi_set_segment_base_address(vram_dpmi_selector, vram_dpmi.address);
	__dpmi_set_segment_limit(vram_dpmi_selector, vram_dpmi.size - 1);
	
	if (GFX_VERBOSE){
		printf("%s.%d\t DPMI mapper created for framebuffer\n", __FILE__, __LINE__);
	}
	return 0;
}

void gfx_TextOn(){
	// Text mode on
	outportb(0x62, GDC_COMMAND_START);
}

void gfx_TextOff(){
	// Text mode off
	outportb(0x62, GDC_COMMAND_STOP1);
}

void gfx_Flip(){
	// Copy a buffer of GFX_ROWS * GFX_COLS bytes to
	// the active VRAM framebuffer for display.
	
	movedata(_my_ds(), vram_buffer, vram_dpmi_selector, 0, (GFX_ROWS * GFX_COLS));
}

int gfx_GetXYaddr(int x, int y){
	// Turn a screen x/y coordinate into an offset into a vram buffer
	
	unsigned int addr;
	unsigned short row;
	
	addr = 0x00;
	addr += GFX_ROW_SIZE * y;
	addr += (x * GFX_PIXEL_SIZE);
	
	if ((VRAM_START + addr) > VRAM_END){
		if (GFX_VERBOSE){
			printf("%s.%d\t XY coords beyond VRAM address range\n", __FILE__, __LINE__);
		}
		return -1;
	}
	return addr;
}

int gfx_Bitmap(int x, int y, bmpdata_t *bmpdata){
	// Load bitmap data into vram_buffer at coords x,y
	// X or Y can be negative which starts the first X or Y
	// rows or columns of the bitmap offscreen - i.e. they are clipped
	//
	// Bitmaps wider or taller than the screen are UNSUPPORTED
	
	int row, col;			//  x and y position counters
	int start_addr;		// The first pixel
	int width_bytes;		// Number of bytes in one row of the image
	int skip_cols;			// Skip first or last pixels of a row if the image is partially offscreen
	int skip_bytes;
	int skip_rows;		// Skip this number of rows if the image is patially offscreen
	int total_rows	;		// Total number of rows to read in clip mode
	unsigned char *ptr;	// Pointer to current location in bmp pixel buffer
	
	if (x < 0){
		// Negative values start offscreen at the left
		skip_cols = x;
	} else {
		if ((x + bmpdata->width) > GFX_COLS){
			// Positive values get clipped at the right
			skip_cols = x + bmpdata->width - GFX_COLS;
		} else {
			// Full width can fit on screen
			skip_cols = 0;
		}
	}
	
	if (y < 0){
		// Negative values start off the top of the screen
		skip_rows = y;
	} else {
		if ((y + bmpdata->height) > GFX_ROWS){
			// Positive values get clipped at the bottom of the screen
			skip_rows = y + bmpdata->height - GFX_ROWS;
		} else {
			// Full height can fit on screen
			skip_rows = 0;
		}
	}
	
	if ((skip_cols == 0) && (skip_rows == 0)){

		// Case 1 - bitmap fits entirely onscreen
		width_bytes = bmpdata->width * bmpdata->bytespp;
		
		// Get starting pixel address
		start_addr = gfx_GetXYaddr(x, y);
		if (start_addr < 0){
			if (GFX_VERBOSE){
				printf("%s.%d\t Unable to set VRAM buffer start address\n", __FILE__, __LINE__);
			}
			return -1;
		}
		// Set starting pixel address
		vram = vram_buffer + start_addr;
		
		// memcpy entire rows at a time
		ptr = (unsigned char*) bmpdata->pixels; 
		for(row = 0; row < bmpdata->height; row++){
			memcpy(vram, ptr, width_bytes);
			
			// Go to next row in vram_buffer
			vram += GFX_COLS;
			
			// Increment point
			ptr += bmpdata->width;
		}
		return 0;
		
	} else {
		// Case 2 - image is either vertically or horizontally partially offscreen		
		if (skip_cols < 0){
			x = x + abs(skip_cols);
		}
		if (skip_rows < 0){
			y = y + abs(skip_rows);
		}
		
		// Get starting pixel address - at the new coordinates
		start_addr = gfx_GetXYaddr(x, y);
		if (start_addr < 0){
			if (GFX_VERBOSE){
				printf("%s.%d\t Unable to set VRAM buffer start address\n", __FILE__, __LINE__);
			}
			return -1;
		}
		
		// Set starting pixel address
		vram = vram_buffer + start_addr;
		
		// Set starting point in pixel buffer
		ptr = (unsigned char*) bmpdata->pixels;
		
		// Default to writing a full row of pixels, unless....
		width_bytes = (bmpdata->width * bmpdata->bytespp) ;
		
		// Default to writing all rows, unless....
		total_rows = bmpdata->height;
		
		// If we are starting offscreen at the y axis, jump that many rows into the data
		if (skip_rows < 0){
			ptr += abs(skip_rows) * bmpdata->width;
			total_rows = bmpdata->height - abs(skip_rows);
		}
		if (skip_rows > 0){
			total_rows = bmpdata->height - abs(skip_rows);
		}
	
		if (skip_cols != 0){
			width_bytes = (bmpdata->width * bmpdata->bytespp) - (abs(skip_cols) * bmpdata->bytespp);
		}
		
		// memcpy entire rows at a time, subject to clipping sizes
		for(row = 0; row < total_rows; row++){
			if (skip_cols < 0){
				memcpy(vram, ptr + abs(skip_cols), width_bytes);
			} else {
				memcpy(vram, ptr, width_bytes);
			}
			// Go to next row in vram buffer
			vram += GFX_COLS;
			// Increment pointer to next row in pixel buffer
			ptr += bmpdata->width;
		}
		return 0;
	}
	return -1;
}

int gfx_Box(int x1, int y1, int x2, int y2, unsigned char palette){
        // Draw a box outline with a given palette entry colour
        int row, col;		//  x and y position counters
        int start_addr; 	// The first pixel, at x1,y1
        int temp;		// Holds either x or y, if we need to flip them
        int step;
        
        // Flip y, if it is supplied reversed
        if (y1>y2){
                temp=y1;
                y1=y2;
                y2=temp;
        }
        // Flip x, if it is supplied reversed
        if (x1>x2){
                temp=x1;
                x1=x2;
                x2=temp;
        }
        // Clip the x range to the edge of the screen
        if (x2>GFX_COLS){
                x2 = GFX_COLS - 1;
        }
        // Clip the y range to the bottom of the screen
        if (y2>GFX_ROWS){
                y2 = GFX_ROWS - 1;
        }
        // Get starting pixel address
        start_addr = gfx_GetXYaddr(x1, y1);
        if (start_addr < 0){
                if (GFX_VERBOSE){
                        printf("%s.%d\t Unable to set VRAM start address\n", __FILE__, __LINE__);
                }
                return -1;
        }
        // Set starting pixel address
        vram = vram_buffer + start_addr;
        
        // Step to next row in vram
        step = (GFX_COLS - x2) + x1;

        // Draw top
        for(col = x1; col <= x2; col++){
                *vram = palette;
                // Move to next pixel in line
                vram++;
        }
        // Jump to next line down and start of left side
        vram += (GFX_COLS - x2) + (x1 - 1);
        
        // Draw sides
        for(row = y1; row < (y2-1); row++){     
                *vram = palette;
                vram += (x2 - x1);
                *vram = palette;
                vram += step;
        }
        
        // Draw bottom
        for(col = x1; col <= x2; col++){
                *vram = palette;
                // Move to next pixel in line
                vram++;
        }
        
        return 0;
}

int gfx_BoxFill(int x1, int y1, int x2, int y2, unsigned char palette){
        // Draw a box, fill it with a given palette entry
        int row, col;		//  x and y position counters
        int start_addr;	// The first pixel, at x1,y1
        int temp;		// Holds either x or y, if we need to flip them
        int step;
        
        // Flip y, if it is supplied reversed
        if (y1>y2){
                temp=y1;
                y1=y2;
                y2=temp;
        }
        // Flip x, if it is supplied reversed
        if (x1>x2){
                temp=x1;
                x1=x2;
                x2=temp;
        }
        // Clip the x range to the edge of the screen
        if (x2>GFX_COLS){
                x2 = GFX_COLS - 1;
        }
        // Clip the y range to the bottom of the screen
        if (y2>GFX_ROWS){
                y2 = GFX_ROWS - 1;
        }
        // Get starting pixel address
        start_addr = gfx_GetXYaddr(x1, y1);
        if (start_addr < 0){
                if (GFX_VERBOSE){
                        printf("%s.%d\t Unable to set VRAM start address\n", __FILE__, __LINE__);
                }
                return -1;
        }
        // Set starting pixel address
        vram = vram_buffer + start_addr;
        
        // Step to next row in vram
        step = (GFX_COLS - x2) + (x1 - 1);
        
        // Starting from the first row (y1)
        for(row = y1; row <= y2; row++){
                // Starting from the first column (x1)
                for(col = x1; col <= x2; col++){
                        *vram = palette;
                        vram++;
                }
                vram += step;
        }
        return 0;
}

int gfx_HasMemoryHole(){
	// Checks if the memory hole is set at 16MB to enable us to set the VRAM framebuffer
	
	int x;
	x = inportb(MEMORY_HOLE_CHECK_ADDR);
	
	//printf("x: %s\n", byte2bin(x, NULL));
	
	if (x & 0x04){
		// Memory hole not present
		return -1;
	} else {
		// Memory hole present
		return 0;	
	}
}

int gfx_Puts(int x, int y, fontdata_t *fontdata, char *c){
	// Put a string of text on the screen, at a set of coordinates
	// using a specific font.
	//
	// Note: We only support 8px and 16px wide fonts.
	
	unsigned int	start_offset;
	unsigned int	row_offset;
	unsigned char	font_symbol;
	unsigned char	font_row;
	unsigned char	i, w;
	unsigned char	pos;
	unsigned char	*src, *dst;
	
	// Empty string
	if (strlen(c) < 1){
		return GFX_TEXT_OK;
	}
	
	// Calculate starting address
	start_offset = gfx_GetXYaddr(x, y);
	if (start_offset < 0){
		if (GFX_VERBOSE){
			printf("%s.%d\t Unable to set VRAM start offset\n", __FILE__, __LINE__);
		}
		return -1;
	}
	
	// Reposition write position
	vram = vram_buffer + start_offset;
	
	if (GFX_VERBOSE){
		printf("%s.%d\t Displaying string: [%s] at vram offset 0x%x\n", __FILE__, __LINE__, c, vram);
		printf("%s.%d\t Displaying string: [%s] at screen co-ords %d,%d\n", __FILE__, __LINE__, c, x, y);
	}
		
	if ((fontdata->width == 8) || (fontdata->width == 16)){
		// For every symbol in the string,
		// 1. Look up the appropriate symbol number to ascii character
		// 2. Check if the symbol is in our font table
		// 3. Do a bitmap copy of the symbol into the vram buffer
		// 4. Increment vram buffer pointer
		for (pos = 0; pos < strlen(c); pos+=1){
			
			i = (unsigned char) c[pos];
			if ((i >= fontdata->ascii_start) && (i <= (fontdata->ascii_start + fontdata->n_symbols))){
				font_symbol = i - fontdata->ascii_start;
			} else {
				font_symbol = fontdata->unknown_symbol;
			}
			
			// Output this symbol
			for(font_row = 0; font_row <= fontdata->height; font_row++){
				row_offset = font_row * GFX_COLS;
				src =  (unsigned char*) fontdata->symbol[font_symbol][font_row];
				dst = vram + row_offset;
				memcpy(dst, src, fontdata->width);
			}
		
			// Reposition write position for next symbol
			vram = vram + fontdata->width;
		}
		return GFX_TEXT_OK;
		
	} else {
		// Unsupported font width
		if (GFX_VERBOSE){
			printf("%s.%d\t Error, font is not a supported width (8 or 16 pixels)\n", __FILE__, __LINE__);	
		}
		return GFX_TEXT_INVALID;
	}
	
}