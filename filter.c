/* filter.c, Apply sorting and filtering to gamedata lists.
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
#include <stdlib.h>
#include <string.h>

#ifndef __HAS_DATA
#include "data.h"
#define __HAS_DATA
#endif
#include "filter.h"
#include "ui.h"

int sortFilterKeys(state_t *state, int items){
	// Sort the list of filter keys by name
	
	qsort(state->filter_strings, items, MAX_STRING_SIZE, strcmp);
	return FILTER_OK;
}

int filter_GetGenres(state_t *state, gamedata_t *gamedata){
	// Get all of the genres set in game metadata
	
	int i;
	int a;
	int c;
	int found;
	int status;
	int next_pos;
	gamedata_t *gamedata_head;
	launchdat_t *launchdat;
	
	launchdat = (launchdat_t *) malloc(sizeof(launchdat_t));
	gamedata_head = gamedata; // Store first item
	
	if (FILTER_VERBOSE){
		printf("%s.%d\t Building genre keyword selection list\n", __FILE__, __LINE__);
	}
	if (FILTER_VERBOSE){
		printf("%s.%d\t Info - Clearing existing filter keywords list\n", __FILE__, __LINE__);
	}
	// Empty list
	for(i =0; i <MAXIMUM_FILTER_STRINGS; i++){
		memset(state->filter_strings[i], '\0', MAX_STRING_SIZE);
	}
	
	i = 0;
	c = 0;
	next_pos = 0;
	while(gamedata != NULL){
		
		// Does game have metadata
		if (gamedata->has_dat){
			
			// Load launch metadata
			status = getLaunchdata(gamedata, launchdat);
			if (status == 0){
				
				// Does the genre field match?
				if (strcmp(launchdat->genre, "") != 0){
				
					// Does this genre already exist?
					found = 0;
					for(a=0;a<MAXIMUM_FILTER_STRINGS;a++){
						if (strcmp(state->filter_strings[a], launchdat->genre) == 0){
							found = 1;
						}
					}
					// This genre isn't found yet, add it to the list of keywords
					if (found == 0){
						if (FILTER_VERBOSE){
							printf("%s.%d\t Info - Found genre: [%s]\n", __FILE__, __LINE__, launchdat->genre);
						}
						strncpy(state->filter_strings[next_pos], launchdat->genre, MAX_STRING_SIZE);
						next_pos++;
					}
				}
			}
		}
		c++;
		gamedata = gamedata->next;
	}
	
	if (FILTER_VERBOSE){
		printf("%s.%d\t Sorting keywords\n", __FILE__, __LINE__);
	}
	sortFilterKeys(state, next_pos);
	for(a=0;a<next_pos;a++){
		if (FILTER_VERBOSE){
			printf("%s.%d\t Info - Keyword %d: [%s]\n", __FILE__, __LINE__, a, state->filter_strings[a]);
		}
	}
	state->available_filter_strings = next_pos;
	
	if (FILTER_VERBOSE){
		printf("%s.%d\t Searched %d games\n", __FILE__, __LINE__, c);
		printf("%s.%d\t Total of %d genre filters added\n", __FILE__, __LINE__, next_pos);
	} 
	free(launchdat);
	return FILTER_OK;
}

int filter_GetSeries(state_t *state, gamedata_t *gamedata){
	// Get all of the series names set in game metadata
	
	int i;
	int a;
	int c;
	int found;
	int status;
	int next_pos;
	gamedata_t *gamedata_head;
	launchdat_t *launchdat;
	
	launchdat = (launchdat_t *) malloc(sizeof(launchdat_t));
	gamedata_head = gamedata; // Store first item
	
	if (FILTER_VERBOSE){
		printf("%s.%d\t Building series keyword selection list\n", __FILE__, __LINE__);
	}
	if (FILTER_VERBOSE){
		printf("%s.%d\t Info - Clearing existing filter keywords list\n", __FILE__, __LINE__);
	}
	// Empty list
	for(i =0; i <MAXIMUM_FILTER_STRINGS; i++){
		memset(state->filter_strings[i], '\0', MAX_STRING_SIZE);
	}
	
	i = 0;
	c = 0;
	next_pos = 0;
	while(gamedata != NULL){
		
		// Does game have metadata
		if (gamedata->has_dat){
			
			// Load launch metadata
			status = getLaunchdata(gamedata, launchdat);
			if (status == 0){
				
				// Does the genre field match?
				if (strcmp(launchdat->series, "") != 0){
				
					// Does this genre already exist?
					found = 0;
					for(a=0;a<MAXIMUM_FILTER_STRINGS;a++){
						if (strcmp(state->filter_strings[a], launchdat->series) == 0){
							found = 1;
						}
					}
					// This genre isn't found yet, add it to the list of keywords
					if (found == 0){
						if (FILTER_VERBOSE){
							printf("%s.%d\t Info - Found series: [%s]\n", __FILE__, __LINE__, launchdat->series);
						}
						strncpy(state->filter_strings[next_pos], launchdat->series, MAX_STRING_SIZE);
						next_pos++;
					}
				}
			}
		}
		c++;
		gamedata = gamedata->next;
	}
	
	if (FILTER_VERBOSE){
		printf("%s.%d\t Sorting keywords\n", __FILE__, __LINE__);
	}
	sortFilterKeys(state, next_pos);
	for(a=0;a<next_pos;a++){
		if (FILTER_VERBOSE){
			printf("%s.%d\t Info - Keyword %d: [%s]\n", __FILE__, __LINE__, a, state->filter_strings[a]);
		}
	}
	state->available_filter_strings = next_pos;
	
	if (FILTER_VERBOSE){
		printf("%s.%d\t Searched %d games\n", __FILE__, __LINE__, c);
		printf("%s.%d\t Total of %d series filters added\n", __FILE__, __LINE__, next_pos);
	} 
	free(launchdat);
	return FILTER_OK;
}

int filter_None(state_t *state, gamedata_t *gamedata){
	// Apply no filter to the list of gamedata - all games
	
	int i;
	gamedata_t *gamedata_head;
	
	gamedata_head = gamedata; // Store first item
	
	if (FILTER_VERBOSE){
		printf("%s.%d\t Building unfiltered selection list\n", __FILE__, __LINE__);
	}
	if (FILTER_VERBOSE){
		printf("%s.%d\t Info - Clearing existing selection list\n", __FILE__, __LINE__);
	}
	// Empty list
	for(i =0; i <SELECTION_LIST_SIZE; i++){
		state->selected_list[i] = NULL;
	}
	if (FILTER_VERBOSE){
		printf("%s.%d\t Info - Clearing existing filter string list\n", __FILE__, __LINE__);
	}
	// Empty filter string list
	for(i =0; i <MAXIMUM_FILTER_STRINGS; i++){
		memset(state->filter_strings[i], '\0', MAX_STRING_SIZE);
	}
	
	i = 0;
	while(gamedata != NULL){
		if (FILTER_VERBOSE){
			printf("%s.%d\t Info - adding Game ID: [%d], %s\n", __FILE__, __LINE__, gamedata->gameid, gamedata->name);
		}
		state->selected_list[i] = gamedata->gameid;
		gamedata = gamedata->next;
		i++;
	}
	if (FILTER_VERBOSE){
		printf("%s.%d\t Total of %d games in list\n", __FILE__, __LINE__, i);
	}
	gamedata = gamedata_head;	// Restore first item
	state->selected_max = i; 	// Number of items in selection list
	state->selected_page = 1;	// Start on page 1
	state->selected_line = 0;	// Start on line 0
	state->total_pages = 0;	
	state->selected_filter_string = 0;
	state->selected_gameid = state->selected_list[0]; 	// Initial game is the 0th element of the selection list
	state->selected_game = getGameid(state->selected_gameid, gamedata);
	for(i = 0; i <= state->selected_max ; i++){
		if (i % ui_browser_max_lines == 0){
			state->total_pages++;
		}
	}
	
	return FILTER_OK;
}

int filter_Genre(state_t *state, gamedata_t *gamedata){
	// Filter all games on a specific genre string
	int i;
	int c;
	int status;
	gamedata_t *gamedata_head;
	launchdat_t *launchdat;
	char filter[MAX_STRING_SIZE];
	
	strncpy(filter, state->filter_strings[state->selected_filter_string], MAX_STRING_SIZE);
	
	launchdat = (launchdat_t *) malloc(sizeof(launchdat_t));
	
	gamedata_head = gamedata; // Store first item
	
	if (FILTER_VERBOSE){
		printf("%s.%d\t Building genre selection list [%s]\n", __FILE__, __LINE__, filter);
	}
	if (FILTER_VERBOSE){
		printf("%s.%d\t Info - Clearing existing selection list\n", __FILE__, __LINE__);
	}
	// Empty list
	for(i =0; i <SELECTION_LIST_SIZE; i++){
		state->selected_list[i] = NULL;
	}
	if (FILTER_VERBOSE){
		printf("%s.%d\t Info - Clearing existing filter string list\n", __FILE__, __LINE__);
	}
	// Empty filter string list
	for(i =0; i <MAXIMUM_FILTER_STRINGS; i++){
		memset(state->filter_strings[i], '\0', MAX_STRING_SIZE);
	}
	
	state->selected_max = 0; 	
	state->selected_page = 1;	
	state->selected_line = 0;	
	state->total_pages = 0;		
	state->selected_gameid = -1;
	state->selected_game = NULL;
	
	i = 0;
	c = 0;
	while(gamedata != NULL){
		
		// Does game have metadata
		if (gamedata->has_dat){
			
			// Load launch metadata
			status = getLaunchdata(gamedata, launchdat);
			if (status == 0){
				
				if (FILTER_VERBOSE){
					printf("%s.%d\t Info - Checking %s == %s\n", __FILE__, __LINE__, launchdat->genre, filter);
				}
				
				// Does the genre field match?
				if (strncmp(launchdat->genre, filter, MAX_STRING_SIZE) == 0){
				
					if (FILTER_VERBOSE){
						printf("%s.%d\t Info - adding Game ID: [%d], %s\n", __FILE__, __LINE__, gamedata->gameid, gamedata->name);
					}
					
					// Add the selected gameid to the selection list
					state->selected_list[i] = gamedata->gameid;
					i++;
				}
			} else {
				if (FILTER_VERBOSE){
					printf("%s.%d\t Warning, metadata not loaded for Game ID: [%d], %s\n", __FILE__, __LINE__, gamedata->gameid, gamedata->name);
				}
			}
		}
		c++;
		gamedata = gamedata->next;
	}
	if (FILTER_VERBOSE){
		printf("%s.%d\t Searched %d games\n", __FILE__, __LINE__, c);
		printf("%s.%d\t Total of %d filtered games in genre list\n", __FILE__, __LINE__, i);
	} 
	
	gamedata = gamedata_head;
	state->selected_max = i; 	// Number of items in selection list
	state->selected_page = 1;	// Start on page 1
	state->selected_line = 0;	// Start on line 0
	state->total_pages = 0;		
	state->selected_filter_string = 0;
	state->selected_gameid = state->selected_list[0]; 	// Initial game is the 0th element of the selection list
	state->selected_game = getGameid(state->selected_gameid, gamedata);
	for(i = 0; i <= state->selected_max ; i++){
		if (i % ui_browser_max_lines == 0){
			state->total_pages++;
		}
	}
	free(launchdat);
	return FILTER_OK;
}

int filter_Series(state_t *state, gamedata_t *gamedata){
	// Filter all games on a specific series string
	int i;
	int c;
	int status;
	gamedata_t *gamedata_head;
	launchdat_t *launchdat;
	char filter[MAX_STRING_SIZE];
	launchdat = (launchdat_t *) malloc(sizeof(launchdat_t));
	gamedata_head = gamedata; // Store first item
	
	strncpy(filter, state->filter_strings[state->selected_filter_string], MAX_STRING_SIZE);
	
	if (FILTER_VERBOSE){
		printf("%s.%d\t Building series selection list [%s]\n", __FILE__, __LINE__, filter);
	}
	if (FILTER_VERBOSE){
		printf("%s.%d\t Info - Clearing existing selection list\n", __FILE__, __LINE__);
	}
	// Empty list
	for(i =0; i <SELECTION_LIST_SIZE; i++){
		state->selected_list[i] = NULL;
	}
	if (FILTER_VERBOSE){
		printf("%s.%d\t Info - Clearing existing filter string list\n", __FILE__, __LINE__);
	}
	// Empty filter string list
	for(i =0; i <MAXIMUM_FILTER_STRINGS; i++){
		memset(state->filter_strings[i], '\0', MAX_STRING_SIZE);
	}
	
	state->selected_max = 0; 	
	state->selected_page = 1;	
	state->selected_line = 0;	
	state->total_pages = 0;		
	state->selected_gameid = -1;
	state->selected_game = NULL;
	
	i = 0;
	c = 0;
	while(gamedata != NULL){
		
		// Does game have metadata
		if (gamedata->has_dat){
			
			// Load launch metadata
			status = getLaunchdata(gamedata, launchdat);
			if (status == 0){
				
				if (FILTER_VERBOSE){
					printf("%s.%d\t Info - Checking %s == %s\n", __FILE__, __LINE__, launchdat->series, filter);
				}
				
				// Does the genre field match?
				if (strncmp(launchdat->series, filter, MAX_STRING_SIZE) == 0){
				
					if (FILTER_VERBOSE){
						printf("%s.%d\t Info - adding Game ID: [%d], %s\n", __FILE__, __LINE__, gamedata->gameid, gamedata->name);
					}
					
					// Add the selected gameid to the selection list
					state->selected_list[i] = gamedata->gameid;
					i++;
				}
			} else {
				if (FILTER_VERBOSE){
					printf("%s.%d\t Warning, metadata not loaded for Game ID: [%d], %s\n", __FILE__, __LINE__, gamedata->gameid, gamedata->name);
				}
			}
		}
		c++;
		gamedata = gamedata->next;
	}
	if (FILTER_VERBOSE){
		printf("%s.%d\t Searched %d games\n", __FILE__, __LINE__, c);
		printf("%s.%d\t Total of %d filtered games in series list\n", __FILE__, __LINE__, i);
	} 
	
	gamedata = gamedata_head;
	state->selected_max = i; 	// Number of items in selection list
	state->selected_page = 1;	// Start on page 1
	state->selected_line = 0;	// Start on line 0
	state->total_pages = 0;		
	state->selected_filter_string = 0;
	state->selected_gameid = state->selected_list[0]; 	// Initial game is the 0th element of the selection list
	state->selected_game = getGameid(state->selected_gameid, gamedata);
	for(i = 0; i <= state->selected_max ; i++){
		if (i % ui_browser_max_lines == 0){
			state->total_pages++;
		}
	}
	free(launchdat);
	return FILTER_OK;
}