/* utils.c, misc. helper utils and functions for the pc98Launcher.
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
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

char *byte2bin(char n, char *buf){
	// Prints an 8bit value as its binary representation
	
	#define BITS (sizeof(n) * CHAR_BIT)

	static char static_buf[BITS + 1];
	short i;

	if (buf == NULL)
		buf = static_buf;

	for (i = BITS - 1; i >= 0; --i) {
		buf[i] = (n & 1) ? '1' : '0';
		n >>= 1;
	}

	buf[BITS] = '\0';
	return buf;

	#undef BITS
}

char *short2bin(short n, char *buf){
	// Prints a 16bit value as its binary representation
	
	#define BITS (sizeof(n) * CHAR_BIT)
	static char static_buf[BITS + 1];
	short i;

	if (buf == NULL)
		buf = static_buf;

	for (i = BITS - 1; i >= 0; --i) {
		buf[i] = (n & 1) ? '1' : '0';
		n >>= 1;
	}

	buf[BITS] = '\0';
	return buf;

	#undef BITS
}