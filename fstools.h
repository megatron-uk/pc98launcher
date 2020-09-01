/* fstools.h, Common defaults used in file search utilities for x68Launcher.
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

// Constants
#define FS_VERBOSE		1		// Enable/disable fstools verbose/debug output
#define DIR_BUFFER_SIZE	65		// Size of array for accepting directory paths
#define MAX_DRIVES		26		// Maximum number of drive letters
#define DRIVE_LETTERS	{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' }

// Fuction prototypes
int 		dirFromPath(char *path, char *buffer);
int 		dirHasData(char *path);
int 		drvLetterToNum(char drive_letter);
char	drvNumToLetter(int drive_number);
int 		findDirs(char *path, gamedata_t *gamedata, int startnum);
int 		isDir(char *path);