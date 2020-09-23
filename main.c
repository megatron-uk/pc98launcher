/* main.c, Main executable segment for the x68Launcher.
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
#include "fstools.h"
#include "filter.h"
#include "gfx.h"
#include "input.h"
#include "palette.h"
#include "ui.h"
#ifndef __HAS_BMP
#include "bmp.h"
#define __HAS_BMP
#endif
#ifndef __HAS_MAIN
#include "main.h"
#define __HAS_MAIN
#endif

int main() {
	/* Lets get this show on the road!!! */
	
	int has_screenshot;
	int has_launchdat;
	int has_images;
	int old_gameid;
	int i;									// Loop counter
	int active_pane;						// Indicator of which UI element is active and consuming input
	int exit;								// Status flag indicating user wants to quit
	int user_input, joy_input, key_input;	// User input state - either a keyboard code or joystick direction/button
	int scrape_dirs;						// NUmber of directories being scraped
	int scrape_progress_chunk_size;			// Size of progress bar increase per directory being scraped
	int progress;							// Progress bar percentage
	int super;								// Supervisor mode flag
	int found, found_tmp;					// Number of gamedirs/games found
	int verbose;							// Controls output of additional logging/text
	int status;								// Generic function return status variable
	char msg[64];							// Message buffer
	FILE *screenshot_file;
	FILE *savefile;
	
	bmpdata_t *bmp = NULL;
	
	state_t *state = NULL;
	bmpdata_t *screenshot_bmp = NULL;	
	bmpstate_t *screenshot_bmp_state = NULL;	
	gamedata_t *gamedata = NULL;			// An initial gamedata record for the first game directory we read
	gamedata_t *gamedata_head = NULL;		// Constant pointer to the start of the gamedata list
	launchdat_t *launchdat = NULL;			// When a single game is selected, we attempt to load its metadata file from disk
	imagefile_t *imagefile = NULL;			// When a single game is selected, we attempt to load a list of the screenshots from metadata
	imagefile_t *imagefile_head = NULL;		// Constant pointer to the start of the game screenshot list
	gamedir_t *gamedir = NULL;				// List of the game search directories, as defined in our INIFILE
	config_t *config = NULL;				// Configuration data as defined in our INIFILE
		
	screenshot_file = NULL;
	has_screenshot = 0;
	has_launchdat = 0;
	has_images = 0;
	old_gameid = -1;
	active_pane = BROWSER_PANE;				// Set initial focus to browser pane
	user_input = joy_input = key_input = 0;	// Initial state of all input variables
	exit = 0;								// Dont exit from main loop unless specified
	scrape_dirs = 0;						// Default to 0 directories found
	progress = 0;							// Default to 0 progress bar size
	found = found_tmp = 0;					// Counter of the number of found directories/gamedata items
	verbose = 1;							// Initial debug/verbose setting; overidden from INIFILE, if set
	
	printf("%s starting...\n", MY_NAME);
	
	//zeroRunBat();
	
	/* ************************************** */
	/* Create a new empty gamedata entry */
	/* ************************************** */
	gamedata = (gamedata_t *) malloc(sizeof(gamedata_t));
	gamedata->next = NULL;
	
	// New bmp data structure
	bmp = (bmpdata_t *) malloc(sizeof(bmpdata_t));
    bmp->pixels = NULL;
	
	/* ************************************** */
	/* Parse the gamedirs that are set */
	/* ************************************** */
	gamedir = (gamedir_t *) malloc(sizeof(gamedir_t));
	gamedir->next = NULL;
	
	/* ************************************** */
	/* Create an instance of a config data */
	/* ************************************** */
	config = (config_t *) malloc(sizeof(config_t));
	config->dir = NULL;
		
	// Screenshot buffer
	screenshot_bmp = (bmpdata_t *) malloc(sizeof(bmpdata_t));
	screenshot_bmp->pixels = NULL;
	
	/* ************************************** */
	/* Create an instance of the UI state data */
	/* ************************************** */
	state = (state_t *) malloc(sizeof(state_t));
	state->selected_max = 0;			// Total amount of items in current filtered selection
	state->selected_page = 1;		// Default to first page of selected games 
	state->selected_line = 0;		// Default to first line selected
	state->total_pages = 0;			// Total number of pages of selected games (selected_max / ui_browser_max_lines)
	state->selected_gameid = -1;		// Current selected game
	state->has_images = 0;			
	state->has_launchdat = 0;
	state->selected_filter = 0;
	state->selected_start = 0;
	state->selected_filter_string = 0;
	state->active_pane = BROWSER_PANE;
	for(i =0; i <SELECTION_LIST_SIZE; i++){
		state->selected_list[i] = NULL;
	}
	
	
	
	/* ************************************** */
	/* Parse our ini file */
	/* ************************************** */
	if (verbose){
		printf("%s.%d\t Loading configuration\n", __FILE__, __LINE__);
	}
	status = getIni(config, verbose);
	if (status != 0){
		printf("%s.%d\t Error unable to parse config file!!!\n", __FILE__, __LINE__);
		printf("\n");
		printf("\n");
		printf("You must have a config file named %s in the same directory as this file\n", INIFILE);
		printf("It should have, at a minimum, the following entries:\n");
		printf("\n");
		printf("[defaults]\n");
		printf("gamedirs=ABC123\n");
		printf("\n");
		printf("Where ABC123 should be replaced by a comma seperate list of one or more paths to your games (e.g. A:\\Games)\n");
		free(config);
		free(gamedir);
		free(gamedata);
		return 1;
	} else {
		printf("\n");
		printf("%s Configuration\n", MY_NAME);
		printf("=========================\n");
		printf("[default]\n");
		printf("verbose=%d\n", config->verbose);
		printf("gamedirs=%s\n", config->dirs);
		printf("save=%d\n", config->save);
		printf("keyboard_test=%d\n", config->keyboard_test);
		printf("preload_names=%d\n", config->preload_names);
		printf("\n");
		if (config->verbose == 0){
			printf("Verbose mode is disabled, you will not receive any further logging after this point\n");
			printf("If required, enable verbose mode by adding: verbose=1 to %s\n", INIFILE);
			printf("\n");
			//gfx_TextOff();
		}
	}
	
	// =======================================
	// Run the keyboard input test, if enabled
	// =======================================
	if (config->keyboard_test == 1){
		input_test();
	}
	
	// ======================
	// Initialise GUI 
	// ======================
	status = gfx_Init();
	if (status != 0){
		printf("ERROR! Unable to initialise graphics mode!\n");
		return status;	
	}
	
	// Do basic UI initialisation
	ui_Init();	
	status = ui_DrawSplash();
	if (status != 0){
		printf("ERROR! Unable to load asset data for initial splash screen!\n");
		ui_Close();
		gfx_Close();
		return status;	
	}
	ui_DrawSplashProgress(1, progress);
	gfx_Flip();
	
	
	// ======================
	// Load UI font data
	// ======================
	status = ui_LoadFonts();
	if (status != UI_OK){
		printf("ERROR! Unable to load asset data for user interface!\n");
		ui_Close();
		gfx_Close();
		return status;
	}
	ui_ProgressMessage("Font loaded!");
	progress += splash_progress_chunk_size;
	ui_DrawSplashProgress(0, progress);
	gfx_Flip();
	

	// ======================
	// Load UI asset data
	// ======================
	ui_ProgressMessage("Loading UI assets, please wait..");
	gfx_Flip();
	status = ui_LoadAssets();
	if (status != UI_OK){
		printf("ERROR! Unable to load asset data for user interface!\n");
		ui_Close();
		gfx_Close();
		return status;
	}
	progress += splash_progress_chunk_size;
	ui_DrawSplashProgress(0, progress);
	gfx_Flip();
	sleep(1);
	
	// ======================
	// Apply any settings from the config file
	// ... including extracting the search directory
	// names.
	// ======================
	if (strlen(config->dirs) >= 3){
		status = getDirList(config, gamedir, config->verbose);
		if (status < 1){
			printf("%s.%d\t Error when extracting game search directories!!!\n", __FILE__, __LINE__);
			printf("\n");
			printf("\n");
			printf("None of the directories listed in %s under the gamedirs setting could be found.\n", INIFILE);
			printf("Check your gamedirs setting and ensure you have at least one directory that exists.\n");
			printf("e.g.\n");
			printf("\n");
			printf("[defaults]\n");
			printf("gamedirs=A:\\Games,A:\\MoreGames,C:\\Stuff\\EvenMoreGames\n");
			printf("\n");
			ui_Close();
			gfx_Close();
			return -1;
		}
	} else {
		printf("%s.%d\t Error when attempting to load game search directories!!!\n", __FILE__, __LINE__);
		printf("\n");
		printf("\n");
		printf("You must have at least one path listed in %s for the gamedirs setting.\n", INIFILE);
		printf("Remember you can use backslashes OR the Yen symbol in path names.\n");
		printf("e.g.\n");
		printf("\n");
		printf("[defaults]\n");
		printf("gamedirs=A:\\Games,A:\\MoreGames,C:\\Stuff\\EvenMoreGames\n");
		printf("\n");
		ui_Close();
		gfx_Close();
		return -1;
	}
	progress += splash_progress_chunk_size;
	ui_DrawSplashProgress(0, progress);
	ui_ProgressMessage("Configuration applied!");
	gfx_Flip();
	
	
	// ======================
	//
	// This section here loops through our game search paths and finds
	// and subdirectories (that should contain games). 
	//
	// ======================
	gamedir = config->dir;
	while (gamedir->next != NULL){
		gamedir = gamedir->next;
		scrape_dirs++;
	}
	progress += splash_progress_chunk_size;
	ui_DrawSplashProgress(0, progress);
	
	// Calculate progress size for each dir scraped
	scrape_progress_chunk_size = splash_progress_chunk_size / scrape_dirs;
	sprintf(msg, "Scraping %d directories for content...", scrape_dirs);
	ui_ProgressMessage(msg);
	gfx_Flip();
	
	
	gamedir = config->dir;
	while (gamedir->next != NULL){
		gamedir = gamedir->next;
		// ======================
		//
		// Show graphical progress update for this directory scraping
		//
		// ======================               
		found_tmp = 0;
		found_tmp = findDirs(gamedir->path, gamedata, found, config);
		found = found + found_tmp;
		sprintf(msg, "Found %d games in %s", found_tmp, gamedir->path);
		ui_ProgressMessage(msg);
		progress += scrape_progress_chunk_size;
		ui_DrawSplashProgress(0, progress);
		gfx_Flip();
		
	}	
	ui_ProgressMessage("Scraped!");
	gfx_Flip();
	
	
	// ========================
	//
	// No gamedirs were found
	//
	// ========================
	if (found < 1){
		free(config);
		free(gamedir);
		free(gamedata);
		printf("%s.%d\t Error no game folders found while scraping your directories!!!\n", __FILE__, __LINE__);
		printf("\n");
		printf("\n");
		printf("You must have at least one path listed in %s for the gamedirs setting.\n", INIFILE);
		printf("Remember you can use backslashes OR the Yen symbol in path names.\n");
		printf("e.g.\n");
		printf("\n");
		printf("[defaults]\n");
		printf("gamedirs=A:\\Games,A:\\MoreGames,C:\\Stuff\\EvenMoreGames\n");
		printf("\n");
		ui_Close();
		gfx_Close();
		return -1;
	} else {
		if (config->verbose){
			printf("%s.%d\t Found %d game directories\n", __FILE__, __LINE__, found);
		}
	}
	
	// =========================
	//
	// Sort the game entries by name
	//
	// =========================
	sprintf(msg, "Sorting %d games...", found);
	ui_ProgressMessage(msg);
	sortGamedata(gamedata, config->verbose);
	progress += splash_progress_chunk_size;
	ui_DrawSplashProgress(0, progress);
	ui_ProgressMessage("Sorted!");
	gfx_Flip();
	
	
	// ======================
	// 
	// Do an initial selection list of all titles
	//
	// ======================
	ui_ProgressMessage("Building initial selection list...");
	gfx_Flip();
	
	if (gamedata->next != NULL){
		gamedata = gamedata->next;
	}
	
	// Apply no-filtering to list, show all games
	status = filter_None(state, gamedata);
	if (config->verbose){
		printf("%s.%d\t Initial selection state\n", __FILE__, __LINE__);
		printf("%s.%d\t Info - selected_max: %d\n", __FILE__, __LINE__, state->selected_max);
		printf("%s.%d\t Info - selected_page: %d\n", __FILE__, __LINE__, state->selected_page);
		printf("%s.%d\t Info - selected_line: %d\n", __FILE__, __LINE__, state->selected_line);
		printf("%s.%d\t Info - total_pages: %d\n", __FILE__, __LINE__, state->total_pages);
		printf("%s.%d\t Info - selected_gameid: %d\n", __FILE__, __LINE__, state->selected_gameid);
	}
	
	// ======================
	//
	// Write a gamelist text file
	//
	// ======================
	if (config->save){
			sprintf(msg, "Saving game list to %s", SAVEFILE);
			ui_ProgressMessage(msg);	
			gfx_Flip();
			
			savefile = fopen(SAVEFILE, "w");
			if (savefile < 0){
					sprintf(msg, "Warning: Unable to create save file. Press any key.");
					ui_ProgressMessage(msg);
					getch();
			} else {
					gamedata_head = gamedata;
					while(gamedata->next != NULL){
							fputs(gamedata->path, savefile);
							fputs("\n", savefile);
							gamedata = gamedata->next;
					}
					gamedata = gamedata_head;
					close(savefile);
			}
	} else {
			ui_ProgressMessage("Not saving game list...");
	}
	ui_DrawSplashProgress(0, splash_progress_complete);
	gfx_Flip();
	
	
	// ======================
	// Load user interface and populate
	// initial window contents
	// ======================
	ui_ProgressMessage("Waiting to load main UI...");
	gfx_Flip();
	sleep(1);
		
	// Also turn any command line text mode off at this point
	if (config->verbose != 1){
		gfx_TextOff();
	}
	
	// Clear VRAM buffer ready for main window contents to be drawn
	gfx_Clear();
	
	// Main UI
	status = ui_DrawMainWindow();
	if (status != UI_OK){
		printf("ERROR! Unable to load draw main UI window!\n");
		ui_Close();
		gfx_Close();
		return status;
	}
	
	// Browser window
	status = ui_UpdateBrowserPane(state, gamedata);
	if (status != UI_OK){
		printf("ERROR! Unable to draw info pane widgets!\n");
		ui_Close();
		gfx_Close();
		return status;
	}
	
	// Info panel
	status = ui_DrawInfoBox();
	if (status != UI_OK){
		printf("ERROR! Unable to draw info pane widgets!\n");
		free(config);
		free(gamedir);
		free(gamedata);
		return status;
	}
	
	// Update info with current selection
	ui_ReselectCurrentGame(state);
	status = ui_UpdateInfoPane(state, gamedata, launchdat);
	if (status != UI_OK){
		printf("ERROR! Unable to update info pane contents!\n");
		free(config);
		free(gamedir);
		free(gamedata);
		return status;
	}
	gfx_Flip();
	
	// Launchdat metadata structure
	launchdat = (launchdat_t *) malloc(sizeof(launchdat_t));	
	
	// ======================
	//
	// Main loop here
	//
	// ======================	
	//ui_StatusMessage("Waiting for user input...");
	while(exit == 0){
		user_input = input_get();
		
		// ==================================================
		//
		// Pop-up to confirm launching our single choice
		//
		// ==================================================
		if (active_pane == CONFIRM_PANE){
			switch(user_input){
				case(input_quit):
					// Exit the application
					exit = 1;
					zeroRunBat();
					break;
				case(input_cancel):
					if (config->verbose){
						printf("%s.%d\t Closing confirm popup\n", __FILE__, __LINE__);	
					}
					active_pane = BROWSER_PANE;
					// exit and redraw main window
					if (config->verbose){
						printf("%s.%d\t Redrawing main screen for Game ID: %d, %s\n", __FILE__, __LINE__, state->selected_gameid, state->selected_game->name);	
					}
					ui_DrawMainWindow();
					ui_UpdateBrowserPane(state, gamedata);
					ui_DrawInfoBox();
					ui_ReselectCurrentGame(state);
					ui_UpdateInfoPane(state, gamedata, launchdat);
					ui_UpdateBrowserPaneStatus(state);
					ui_DisplayArtwork(screenshot_file, screenshot_bmp, state, imagefile);
					gfx_Flip();
					break;
				case(input_select):
					// Quit application and run the file
					if (config->verbose){
						printf("%s.%d\t Writing run.bat\n", __FILE__, __LINE__);	
					}
					writeRunBat(state, launchdat);
					exit = 1;
					break;
				default:
					break;
			}
		}
		// ==================================================
		//
		// Pop-up to confirm which of several choices we want
		// to launch.
		//
		// ==================================================
		if (active_pane == LAUNCH_PANE){
			
			switch(user_input){
				case(input_quit):
					// Exit the application
					exit = 1;
					zeroRunBat();
					break;
				case(input_cancel):
					if (config->verbose){
						printf("%s.%d\t Closing launcher popup\n", __FILE__, __LINE__);	
					}
					active_pane = BROWSER_PANE;
					// exit and redraw main window
					if (config->verbose){
						printf("%s.%d\t Redrawing main screen for Game ID: %d, %s\n", __FILE__, __LINE__, state->selected_gameid, state->selected_game->name);	
					}
					ui_DrawMainWindow();
					ui_UpdateBrowserPane(state, gamedata);
					ui_DrawInfoBox();
					ui_ReselectCurrentGame(state);
					ui_UpdateInfoPane(state, gamedata, launchdat);
					ui_UpdateBrowserPaneStatus(state);
					ui_DisplayArtwork(screenshot_file, screenshot_bmp, state, imagefile);
					gfx_Flip();
					break;
				case(input_up):
					// FLip between start files
					ui_DrawLaunchPopup(state, gamedata, launchdat, 1);
					gfx_Flip();
					break;
				case(input_down):
					// FLip between start files
					ui_DrawLaunchPopup(state, gamedata, launchdat, 1);
					gfx_Flip();
					break;
				case(input_select):
					// Quit application and run the selected start file
					if (config->verbose){
						printf("%s.%d\t Opening confirmation popup\n", __FILE__, __LINE__);	
					}
					active_pane = CONFIRM_PANE;
					ui_DrawConfirmPopup(state, gamedata, launchdat);
					gfx_Flip();
					break;
				default:
					break;
			}
		}
		
		// ==================================================
		//
		// This window shows general help and useage text.
		//
		// ==================================================
		if (active_pane == HELP_PANE){
			switch(user_input){
				case(input_quit):
					// Exit the application
					exit = 1;
					zeroRunBat();
					break;
				default:
					break;
			}
		}
		
		// ==================================================
		//
		// This window lets us decide what type of 
		// filter to apply, or to disable the active filter:
		//
		// Genre filter, Series filter, Disable filter
		//
		// ==================================================
		if (active_pane == FILTER_PRE_PANE){
			switch(user_input){
				case(input_quit):
					// Exit the application
					exit = 1;
					zeroRunBat();
					break;
				case(input_up):
					if (config->verbose){
						printf("%s.%d\t Toggle filter type selection up\n", __FILE__, __LINE__);	
					}
					ui_DrawFilterPrePopup(state, -1);
					gfx_Flip();
					break;
				case(input_down):
					if (config->verbose){
						printf("%s.%d\t Toggle filter type selection down\n", __FILE__, __LINE__);	
					}
					ui_DrawFilterPrePopup(state, 1);
					gfx_Flip();
					break;
				case(input_select):
					// Choose the highlighted selection
					
					if (state->selected_filter == FILTER_NONE){
						// Reselect all games
						status = filter_None(state, gamedata);
						
						if (config->verbose){
							printf("%s.%d\t Closing filter popup(s)\n", __FILE__, __LINE__);	
						}
						active_pane = BROWSER_PANE;
						// exit and redraw main window
						if (config->verbose){
							printf("%s.%d\t Redrawing main screen for Game ID: %d, %s\n", __FILE__, __LINE__, state->selected_gameid, state->selected_game->name);	
						}
						ui_DrawMainWindow();
						ui_UpdateBrowserPane(state, gamedata);
						ui_DrawInfoBox();
						ui_ReselectCurrentGame(state);
						ui_UpdateInfoPane(state, gamedata, launchdat);
						ui_UpdateBrowserPaneStatus(state);
						ui_DisplayArtwork(screenshot_file, screenshot_bmp, state, imagefile);
						gfx_Flip();
						user_input = input_get();
					} else {
						if (config->verbose){
							printf("%s.%d\t Launching filter popup\n", __FILE__, __LINE__);	
						}
						
						// Set filter pane as next one to display
						active_pane = FILTER_PANE;
						
						// Generate the list of keywords
						if (state->selected_filter == FILTER_GENRE){
							filter_GetGenres(state, gamedata);
						}
						
						if (state->selected_filter == FILTER_SERIES){
							filter_GetSeries(state, gamedata);
						}
						
						// Bring up the filter keyword selection pane
						ui_DrawFilterPopup(state, 0);
						gfx_Flip();
						user_input = input_get();
					}
					break;
				case(input_cancel):
					if (config->verbose){
						printf("%s.%d\t Closing filter type popup\n", __FILE__, __LINE__);	
					}
					active_pane = BROWSER_PANE;
					// exit and redraw main window
					if (config->verbose){
						printf("%s.%d\t Redrawing main screen for Game ID: %d, %s\n", __FILE__, __LINE__, state->selected_gameid, state->selected_game->name);	
					}
					ui_DrawMainWindow();
					ui_UpdateBrowserPane(state, gamedata);
					ui_DrawInfoBox();
					ui_ReselectCurrentGame(state);
					ui_UpdateInfoPane(state, gamedata, launchdat);
					ui_UpdateBrowserPaneStatus(state);
					ui_DisplayArtwork(screenshot_file, screenshot_bmp, state, imagefile);
					gfx_Flip();
					break;
				default:
					break;
			}
		}
		
		// ==================================================
		//
		// This window lets us choose which active filter 
		// to apply in the selected filter mode: 
		//
		// Genre: RPG, Adventure, Shooter, etc.
		// Series: Lodoss War, Amaranth, Touhou etc.
		//
		// ==================================================
		if (active_pane == FILTER_PANE){
			switch(user_input){
				case(input_quit):
					// Exit the application
					exit = 1;
					zeroRunBat();
					break;
				case(input_up):
					if (config->verbose){
						printf("%s.%d\t Toggle filter selection up\n", __FILE__, __LINE__);	
					}
					ui_DrawFilterPopup(state, -1);
					gfx_Flip();
					break;
				case(input_down):
					if (config->verbose){
						printf("%s.%d\t Toggle filter selection down\n", __FILE__, __LINE__);	
					}
					ui_DrawFilterPopup(state, 1);
					gfx_Flip();
					break;
				case(input_select):
					if (state->selected_filter == FILTER_GENRE){
						// Now apply the chosen filter
						status = filter_Genre(state, gamedata);
					}
					
					if (state->selected_filter == FILTER_SERIES){
						// Now apply the chosen filter
						status = filter_Series(state, gamedata);
					}
					if (config->verbose){
						printf("%s.%d\t Closing filter popup(s)\n", __FILE__, __LINE__);	
					}
					active_pane = BROWSER_PANE;
					// exit and redraw main window
					if (config->verbose){
						printf("%s.%d\t Redrawing main screen for Game ID: %d, %s\n", __FILE__, __LINE__, state->selected_gameid, state->selected_game->name);	
					}
					ui_DrawMainWindow();
					ui_UpdateBrowserPane(state, gamedata);
					ui_DrawInfoBox();
					ui_ReselectCurrentGame(state);
					ui_UpdateInfoPane(state, gamedata, launchdat);
					ui_UpdateBrowserPaneStatus(state);
					ui_DisplayArtwork(screenshot_file, screenshot_bmp, state, imagefile);
					gfx_Flip();
					user_input = input_get();
					break;
				case(input_cancel):
					if (config->verbose){
						printf("%s.%d\t Closing filter keyword popup\n", __FILE__, __LINE__);	
					}
					active_pane = BROWSER_PANE;
					// exit and redraw main window
					if (config->verbose){
						printf("%s.%d\t Redrawing main screen for Game ID: %d, %s\n", __FILE__, __LINE__, state->selected_gameid, state->selected_game->name);	
					}
					ui_DrawMainWindow();
					ui_UpdateBrowserPane(state, gamedata);
					ui_DrawInfoBox();
					ui_ReselectCurrentGame(state);
					ui_UpdateInfoPane(state, gamedata, launchdat);
					ui_UpdateBrowserPaneStatus(state);
					ui_DisplayArtwork(screenshot_file, screenshot_bmp, state, imagefile);
					gfx_Flip();
					break;
				default:
					break;
			}
		}
		
		// ==================================================
		//
		// Main browser window, listing all of our games
		//
		// ==================================================
		if (active_pane == BROWSER_PANE){
			switch(user_input){
				case(input_quit):
					// Exit the application
					exit = 1;
					zeroRunBat();
					break;
				case(input_help):
					// Show help screen
					if (config->verbose){
						printf("%s.%d\t Attempting launch help popup...\n", __FILE__, __LINE__);	
					}
					//active_pane = HELP_PANE;
					break;
				case(input_filter):
					// Show filter screen
					if (config->verbose){
						printf("%s.%d\t Attempting launch filter pre-popup...\n", __FILE__, __LINE__);	
					}
					active_pane = FILTER_PRE_PANE;
					ui_DrawFilterPrePopup(state, 0);
					gfx_Flip();
					break;
				case(input_select):
					// Start a game or launch a config tool
					if (state->selected_game->has_dat){
						if (config->verbose){
							printf("%s.%d\t Attempting launch...\n", __FILE__, __LINE__);	
						}
						
						if (
								((launchdat->start != NULL) && (strlen(launchdat->start) > 0)) && 
								((launchdat->alt_start != NULL) && (strlen(launchdat->alt_start) > 0))
						){
							// Start and alt_start defined
							if (config->verbose){
								printf("%s.%d\t - Action: Drawing launcher popup\n", __FILE__, __LINE__);	
							}
							active_pane = LAUNCH_PANE;
							state->selected_start = START_MAIN;
							ui_DrawLaunchPopup(state, gamedata, launchdat, 0);
							gfx_Flip();
							
						} else if ((launchdat->start != NULL) && (strcmp(launchdat->start, "") != 0)){
							// Start defined only
							if (config->verbose){
								printf("%s.%d\t - Action: Drawing confirmation popup\n", __FILE__, __LINE__);	
							}
							active_pane = CONFIRM_PANE;
							state->selected_start = START_MAIN;
							ui_DrawConfirmPopup(state, gamedata, launchdat);
							gfx_Flip();
							
						} else if ((launchdat->alt_start != NULL) && (strcmp(launchdat->alt_start, "") != 0)){
							// alt start defined only
							if (config->verbose){
								printf("%s.%d\t - Action: Closing to start game (alt start)\n", __FILE__, __LINE__);	
							}
							active_pane = CONFIRM_PANE;
							state->selected_start = START_ALT;
							ui_DrawConfirmPopup(state, gamedata, launchdat);
							gfx_Flip();
							
						} else {
							// Nothing defined
							// do nothing
							if (config->verbose){
								printf("%s.%d\t - Action: No start file, no action\n", __FILE__, __LINE__);	
							}
						}
					}
					break;
				case(input_up):
					// Up current list by one row
					if (state->selected_line == 0){
						if (state->selected_page == 1){
							// Loop back to last page
							state->selected_page = state->total_pages;
						} else {
							// Go back one page
							state->selected_page--;
						}
						// Reset to line 1 of the new page
						state->selected_line = 0;							
					} else {
						// Move up one line
						state->selected_line--;
					}
					// Detect if selected game has changed
					ui_ReselectCurrentGame(state);
					ui_UpdateBrowserPane(state, gamedata);
					break;
				case(input_down):
					// Down current list by one row
					if ((state->selected_line == ui_browser_max_lines - 1) || (state->selected_line == (state->selected_max - 1))){
						if (state->selected_page == state->total_pages){
							// Go to first page
							state->selected_page = 1;
						} else {
							// Go forward one page
							state->selected_page++;
						}
						// Reset to line 1 of the new page
						state->selected_line = 0;							
					} else {
						// Move down one line
						state->selected_line++;
					}
					// Detect if selected game has changed
					ui_ReselectCurrentGame(state);
					ui_UpdateBrowserPane(state, gamedata);
					break;
				case(input_scroll_up):
					// Scroll list up by one page
					// Detect if selected game has changed
					if (state->selected_page == 1){
						// Loop back to last page
						state->selected_page = state->total_pages;
					} else {
						// Go back one page
						state->selected_page--;
					}
					state->selected_line = 0;
					ui_ReselectCurrentGame(state);
					ui_UpdateBrowserPane(state, gamedata);
					break;
				case(input_scroll_down):
					// Scroll list down by one page
					// Detect if selected game has changed
					if (state->selected_page == state->total_pages){
						// Go to first page
						state->selected_page = 1;
					} else {
						// Go forward one page
						state->selected_page++;
					}
					// Reset to line 1 of the new page
					state->selected_line = 0;	
					ui_ReselectCurrentGame(state);
					ui_UpdateBrowserPane(state, gamedata);
					break;
				case(input_left):
					// Cycle left through artwork
					if (imagefile != NULL){
						if (imagefile->prev != NULL){
							imagefile = imagefile->prev;
						} else {
							imagefile = imagefile_head;
						}
						ui_DisplayArtwork(screenshot_file, screenshot_bmp, state, imagefile);
						gfx_Flip();
					}
					break;
				case(input_right):
					// Scroll right through artwork - if available
					if (imagefile != NULL){
						if (imagefile->next != NULL){
							imagefile = imagefile->next;
						} else {
							imagefile = imagefile_head;
						}
						ui_DisplayArtwork(screenshot_file, screenshot_bmp, state, imagefile);
						gfx_Flip();
					}
					break;
				default:
					break;
			}
			// ===================================================================
			//
			// Once all of the input has been taken care of in the browser window, 
			// this is where we decide whether to repaint the screen or not.
			//
			// ===================================================================
			
			// Only refresh browser, artwork and info panes if the selected game has changed
			if (old_gameid != state->selected_gameid){
							
				// ======================
				// Destroy current list of artwork
				// ======================
				if (state->has_images == 1){
					if (config->verbose){
						printf("%s.%d\t Previous entry had images, clearing list\n", __FILE__, __LINE__);
					}
					imagefile = imagefile_head;
					removeImagefile(imagefile);
					if (imagefile != NULL){
						if (config->verbose){
							printf("%s.%d\t Freeing memory from previous image list\n", __FILE__, __LINE__);
						}
						free(imagefile);
					}
				}
				// Clear artwork window
				gfx_BoxFill(ui_artwork_xpos, ui_artwork_ypos, ui_artwork_xpos + 320, ui_artwork_ypos + 200, PALETTE_UI_BLACK);
				memset(state->selected_image, '\0', sizeof(state->selected_image)); 
				state->has_images = 0;
				
				// ======================
				// Destroy previous launch.dat
				// ======================
				if (state->has_launchdat){
					if (launchdat != NULL){
						if (config->verbose){
							printf("%s.%d\t Previous entry had metadata, freeing memory\n", __FILE__, __LINE__);
						}
						free(launchdat);
					}
				}
				state->has_launchdat = 0;
				
				// ======================
				// Update selection to current game
				// ======================
				gamedata_head = gamedata;
				if (config->verbose){
					printf("%s.%d\t Finding gamedata from list for [%d]\n", __FILE__, __LINE__, state->selected_gameid);
				}
				state->selected_game = getGameid(state->selected_gameid, gamedata);
				if (state->selected_game == NULL){
					// Could not load gamedata object for this id - why?
					// Reset to old gameid
					if (config->verbose){
						printf("%s.%d\t Warning, unable to find gamedata for Game ID %d, reverting to %d\n", __FILE__, __LINE__, state->selected_gameid, old_gameid);
					}
					state->selected_gameid = old_gameid;
					old_gameid = -1;
				} else {
					if (state->selected_game->has_dat){
						if (config->verbose){
							printf("%s.%d\t Allocating memory and loading metadata for [%s]\n", __FILE__, __LINE__, state->selected_game->name);
						}
						launchdat = (launchdat_t *) malloc(sizeof(launchdat_t));	
						status = getLaunchdata(state->selected_game, launchdat);
						if (status != 0){
							if (config->verbose){
								printf("%s.%d\t Error, could not load metadata\n", __FILE__, __LINE__);	
							}
							//ui_StatusMessage("Error, could not load metadata!");
							gfx_Flip();
						} else {
							state->has_launchdat = 1;
						}
					}
					
					// ======================
					// Load list of artwork
					// ======================
					if (state->has_launchdat){
						if (config->verbose){
							printf("%s.%d\t Allocating memory for image list\n", __FILE__, __LINE__);
						}
						imagefile = (imagefile_t *) malloc(sizeof(imagefile_t));
						imagefile->next = NULL;
						imagefile_head = imagefile;
						status = getImageList(launchdat, imagefile);
						if (status > 0){
							state->has_images = 1;
						}
					}
									
					// =======================
					// Select first screenshot/artwork to show
					// =======================
					if (state->has_images){
						if (imagefile->next != NULL){
							if (config->verbose){
								printf("%s.%d\t Selected artwork filename [%s]\n", __FILE__, __LINE__, imagefile->next->filename);
							}
							sprintf(msg, "%s\\%s", state->selected_game->path, imagefile->next->filename);
							strncpy(state->selected_image, msg, 65);
						}
					}
					
					// =======================
					// Updating info and browser pane
					// =======================
					gamedata = gamedata_head;
					ui_UpdateInfoPane(state, gamedata, launchdat);
					ui_UpdateBrowserPaneStatus(state);
					old_gameid = state->selected_gameid;
					gfx_Flip();
	
					// Display artwork/first screenshot
					ui_DisplayArtwork(screenshot_file, screenshot_bmp, state, imagefile);
					gfx_Flip();
				}
				if (config->verbose){
					printf("%s.%d\t New game successfully loaded\n", __FILE__, __LINE__);
				}
			}
		}
	}
	
	ui_Close();
	gfx_Close();
	return 0;
}