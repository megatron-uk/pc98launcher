/* fstools.c, Filesystem/directory search utilities for x68Launcher.
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
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "data.h"
#include "fstools.h"

char drvNumToLetter(int drive_number){
	/* Turn a drive number into a drive letter */
	
	static char mapper[MAX_DRIVES] = DRIVE_LETTERS;
	
	if (drive_number > MAX_DRIVES){
		return '\0';	
	} else {
		return mapper[drive_number];
	}
}

int drvLetterToNum(char drive_letter){
	/* Turn a drive letter into a drive number */
	
	int c;
	static char mapper[MAX_DRIVES] = DRIVE_LETTERS;
	
	/* Find the matching position of the drive letter */
	for (c = 0; c < (MAX_DRIVES +1); c++){
		if (mapper[c] == drive_letter){
			return c;	
		}
	}
	return -1;
}

char drvLetterFromPath(char *path){
	/* Return the drive letter part of a path like A:\Games */
	
	/* Must be at least A:\? */
	if (strlen(path) >= 3){
		/* Is it a fully qualified path, where the second character is a : */
		if (strncmp(path + 1, ":", 1) == 0){
			/* Return the 'A' part */
			return path[0];
		} else {
			printf("%s.%d\t Doesn't have a drive letter seperator\n", __FILE__, __LINE__);
			return '\0';	
		}
	} else {
		printf("%s.%d\t Seems like a short path\n", __FILE__, __LINE__);
		return '\0';	
	}
}

int dirFromPath(char *path, char *buffer){
	/* Return the directory part of a path like A:\Games\Folder1 */
	
	int sz;
	
	/* Must be at least A:\? */
	if (strlen(path) >= 3){
		/* is this a fully qualified path, like a:\games */
		if (strncmp(path + 1, ":", 1) == 0){
			/* size of the string */
			sz = strlen(path);
			if (strncmp(path + sz, "\\", 1) == 0){
				/* copy from the character after the first \ to the last \ */
				strncpy(buffer, path + 3, (sz - 4));
				return 0;
			} else {
				/* copy from the character after the first \ to the last character */
				strncpy(buffer, path + 3, (sz - 3));
				return 0;
			}
		} else {
			return -1;
		}
	} else {
		return -1;	
	}
}

int isDir(char *path){
	/* Boolean test to check if a path is a directory or not */

	DIR *dir;
	int dir_type;
	
	dir = opendir(path);
	if (dir != NULL){
		dir_type = 1;
	} else {
		dir_type = 0;	
	}
	closedir(dir);
	return dir_type;
}

int dirHasData(char *path){
	/* Return 1 if a __launch.dat file is found in a given directory, 0 if missing */
	
	FILE *f;
	int found;
	char filepath[DIR_BUFFER_SIZE];
	
	strcpy(filepath, path);
	strcat(filepath, "\\");
	strcat(filepath, GAMEDAT);
	
	f = fopen(filepath, "r");
	if (f != NULL){
		found = 1;
	} else {
		found = 0;	
	}
	fclose(f);
	return found;
}

int findDirs(char *path, gamedata_t *gamedata, int startnum){
	/* Open a search path and return a count of any directories found, creating a gamedata object for each one. */
	
	// path: Fully qualified path to search, e.g. "A:\Games"
	// gamedata: An instance of the linked-list of game data
	// startnum: The starting number to tag each found 'game' with the next auto-incrementing ID
	
	char drive, old_drive;
	char drive_letter, old_drive_letter;
	char status;
	int go;
	int found;
	DIR *dir;
	struct dirent *de;
	
	/* store directory names */
	char old_dir_buffer[DIR_BUFFER_SIZE];
	
	/* hold information about search path */
	char search_drive;
	char search_dirname[DIR_BUFFER_SIZE];
	
	/* initialise counters */
	go = 1;
	found = 0;
	
	/* initialise the directory or search dirname buffer */
	memset(old_dir_buffer, '\0', sizeof(old_dir_buffer));
	memset(search_dirname, '\0', sizeof(search_dirname));
	
	/* split drive and dirname from search path */
	search_drive = drvLetterFromPath(path);
	dirFromPath(path, search_dirname);
	if (FS_VERBOSE){
		printf("%s.%d\t Search scope [drive:%c] [path:%s]\n", __FILE__, __LINE__, search_drive, search_dirname);
	}
	
	/* save curdrive */
	_dos_getdrive(old_drive);
	if (old_drive < 0){
		printf("%s.%d\t Unable to save current drive [status:%d]\n", __FILE__, __LINE__, old_drive);
		return -1;
	}
	
	/* save curdir */
	getcwd(old_dir_buffer, DIR_BUFFER_SIZE);
	//if (old_dir_buffer[0] != '\0'){
	//	printf("%s.%d\t Unable to save current directory [status:%d]\n", __FILE__, __LINE__, status);
	//	return -1;
	//}

	if (isDir(path)){				
		/* change to actual search path */
		status = chdir(path);
		if (status != 0){
			printf("%s.%d\t Unable to change to search path [status:%d][path:%s]\n", __FILE__, __LINE__, status, path);
		} else {
			// Open the directory entry for this search path
			dir = opendir(path);
			if (dir != NULL){
				// Read an entry for the directory entry
				while (de = readdir(dir)){
					// Skip any names that are "." and ".."
					if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0){
						// Only process entries that are of type DT_DIR (e.g. sub-directory names)						
						memset(search_dirname, '\0', sizeof(search_dirname));
						strcpy(search_dirname, path);
						strcat(search_dirname, "\\");
						strcat(search_dirname, de->d_name);
						if (isDir(search_dirname)){
							if (FS_VERBOSE){
								printf("%s.%d\t ID: %d\n", __FILE__, __LINE__, startnum);
								printf("%s.%d\t Name: %s\n", __FILE__, __LINE__, de->d_name);
								printf("%s.%d\t Drive: %c\n", __FILE__, __LINE__, search_drive);
								printf("%s.%d\t Path: %s\n", __FILE__, __LINE__, search_dirname);
								printf("%s.%d\t Full Path: %s\n", __FILE__, __LINE__, search_dirname);
								printf("%s.%d\t Has dat: %d\n", __FILE__, __LINE__, dirHasData(search_dirname));
							}
							found++;
							gamedata = getLastGamedata(gamedata);
							gamedata->next = (gamedata_t *) malloc(sizeof(gamedata_t));
							gamedata->next->gameid = startnum;
							gamedata->next->drive =search_drive;
							strcpy(gamedata->next->path, search_dirname);
							strcpy(gamedata->next->name, de->d_name);
							gamedata->next->has_dat = dirHasData(search_dirname);
							gamedata->next->next = NULL;
							startnum++;
						}
					}
				}
				closedir(dir);
			}
		}

	} else {
		printf("%s.%d\t Not a directory\n", __FILE__, __LINE__);
	}
	
	/* reload current drive and directory */
	_dos_setdrive(old_drive);
	status = chdir(old_dir_buffer);
	if (status != 0){
		printf("%s.%d\t Unable to restore directory [status:%d\n", __FILE__, __LINE__, status);
		return -1;
	}
	
	return found;
}