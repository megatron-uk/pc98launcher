/* main.h, Global data structures for the x68Launcher.
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

#define MY_NAME "pc98launcher"

#define selection_list_size				999 // Maximum number of entries in the game browser

typedef struct state {
	unsigned int selected_list[selection_list_size];		// A list of game ID's which are currently selected
	unsigned int selected_max;			// Number of items in the current selected list
	unsigned int selected_page;			// Page 'N' of the selected list
	unsigned int selected_line;			// The line in the page indicating the selected game
	unsigned int total_pages;			// Total number of pages in the selected_list
	unsigned int active_pane;
	
	// Info about selected item
	int selected_gameid;					// Currently selected gameid
	gamedata_t *selected_game;			// Currently selected gamedata item
	int has_launchdat;
	int has_images;
	char selected_image[65];				// path + filename of artwork
	
} __attribute__((__packed__)) __attribute__((aligned (2))) state_t;
