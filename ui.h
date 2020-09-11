/* ui.h, User interface structure and bmp filenames for the x68Launcher.
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
#ifndef __HAS_MAIN
#include "main.h"
#define __HAS_MAIN
#endif

// Enable/disable logging for the ui.c functions
#define UI_VERBOSE				1

#define splash_logo				"assets\\logo.bmp"
#define splash_progress_x_pos	100
#define splash_progress_y_pos	360
#define splash_progress_width	404
#define splash_progress_height	14
#define splash_progress_chunks	7
#define splash_progress_chunk_size	((splash_progress_width - 4) / splash_progress_chunks)
#define splash_progress_complete (splash_progress_width - 4)

#define ui_font_name				"assets\\font8x16.bmp"
#define ui_font_width			8  // Font symbol width
#define ui_font_height			16 // Font symbol height
#define ui_font_ascii_start		32 // Font table starts at ascii ' '
#define ui_font_total_syms		96 // Total number of symbols in font table
#define ui_font_unknown			95 // Use ascii '?' for missing symbols

// Progress text location
#define ui_progress_font_x_pos	splash_progress_x_pos
#define ui_progress_font_y_pos	(splash_progress_y_pos - (ui_font_height + 2))

// Dark metal theme
#define ui_main					"assets\\light\\light.bmp"
#define ui_list_box				"assets\\light\\box_list.bmp"
#define ui_art_box				"assets\\light\\box_art.bmp"
#define ui_title_box				"assets\\light\\box_titl.bmp"
#define ui_year_box				"assets\\light\\box_year.bmp"
#define ui_genre_box				"assets\\light\\box_genr.bmp"
#define ui_series_box			"assets\\light\\box_seri.bmp"
#define ui_path_box				"assets\\light\\box_path.bmp"
#define ui_company_box			"assets\\light\\box_comp.bmp"
#define ui_check_box				"assets\\light\\cb_check.bmp"
#define ui_check_box_unchecked	"assets\\light\\cb_empty.bmp"
#define ui_select				"assets\\light\\select.bmp"

// Coordinates
#define ui_header_xpos			0
#define ui_header_ypos			0
#define ui_border_left_xpos		0
#define ui_border_left_ypos		12
#define ui_border_central_xpos	230
#define ui_border_central_ypos	12
#define ui_border_right_xpos		(GFX_COLS - 12)
#define ui_border_right_ypos		12
#define ui_under_artwork_xpos	230
#define ui_under_artwork_ypos	268
#define ui_under_browser_xpos	0
#define ui_under_browser_ypos	386

// Location of checkbox widgets 

// Left column
#define ui_checkbox_has_metadata_xpos		304
#define ui_checkbox_has_metadata_ypos		226

#define ui_checkbox_has_images_xpos			304
#define ui_checkbox_has_images_ypos			249

#define ui_checkbox_has_startbat_xpos		304
#define ui_checkbox_has_startbat_ypos		272

// Right column
#define ui_checkbox_has_midi_xpos			439
#define ui_checkbox_has_midi_ypos			226

#define ui_checkbox_has_midi_serial_xpos		439
#define ui_checkbox_has_midi_serial_ypos		249

#define ui_checkbox_filter_active_xpos		439
#define ui_checkbox_filter_active_ypos		272

// Artwork window
#define ui_artwork_xpos			307
#define ui_artwork_ypos			13

// Location of info pane textbox widgets
#define ui_info_name_xpos		10
#define ui_info_name_ypos		305
#define ui_info_name_text_xpos	55
#define ui_info_name_text_ypos	306

#define ui_info_year_xpos		473
#define ui_info_year_ypos		305
#define ui_info_year_text_xpos	518
#define ui_info_year_text_ypos	306

#define ui_info_company_xpos		10
#define ui_info_company_ypos		331
#define ui_info_company_text_xpos	80
#define ui_info_company_text_ypos	332

#define ui_info_genre_xpos		382
#define ui_info_genre_ypos		331
#define ui_info_genre_text_xpos	438
#define ui_info_genre_text_ypos	332

#define ui_info_path_xpos		10
#define ui_info_path_ypos		357
#define ui_info_path_text_xpos	55
#define ui_info_path_text_ypos	358

#define ui_info_series_xpos		382
#define ui_info_series_ypos		357
#define ui_info_series_text_xpos	438
#define ui_info_series_text_ypos	358

// launch window popup
#define ui_launch_popup_xpos		170
#define ui_launch_popup_ypos		150
#define ui_launch_popup_width	300
#define ui_launch_popup_height	100

// artwork window dimensions
#define ui_artwork_width			320	// Width of the window that holds artwork
#define ui_artwork_height		200 // Height of the window that holds artwork

// status bar dimensions
#define ui_status_font_name		"assets\\font8x8.bmp"
#define ui_status_font_width		8 // Font symbol width
#define ui_status_font_height	8 // Font symbol height
#define ui_status_font_ascii_start	32 // Font table starts at ascii ' '
#define ui_status_font_total_syms	96 // Total number of symbols in font table
#define ui_status_font_unknown	31 // Use ascii '?' for missing symbols
#define ui_status_bar_xpos		10
#define ui_status_bar_ypos		496
#define ui_status_bar_width		400
#define ui_status_bar_height		12 // 8px for font, plus 2px above and below
#define ui_status_font_x_pos		1
#define ui_status_font_y_pos		(ui_status_bar_ypos + 2)

// Browser/artwork divider
#define ui_browser_panel_x_pos	10
#define ui_browser_panel_y_pos	10
#define ui_browser_font_x_pos	40
#define ui_browser_font_y_pos	16
#define ui_browser_max_lines		14
#define ui_browser_footer_font_xpos 20
#define ui_browser_footer_font_ypos 280
#define ui_browser_cursor_xpos 	15

// Return codes
#define UI_OK					0
#define UI_ERR_FILE				-1
#define UI_ERR_BMP				-2
#define UI_ERR_FUNCTION_CALL		-3
#define UI_ASSETS_LOADED			255
#define UI_ASSETS_MISSING		254

// Labels for active panes
#define PANE_FIRST				0x01
#define BROWSER_PANE				0x01
#define ARTWORK_PANE				0x02
#define INFO_PANE				0x03
#define LAUNCH_PANE				0x04
#define CONFIRM_PANE				0x05
#define FILTER_PRE_PANE			0x06
#define FILTER_PANE				0x07
#define HELP_PANE				0x08
#define PANE_MAX					0x08

// Functions
void	ui_Init();
void	ui_Close();

// These draw the basic UI elements
int		ui_DrawInfoBox();
int		ui_DrawConfirmPopup(state_t *state, gamedata_t *gamedata, launchdat_t *launchdat);
int		ui_DrawFilterPrePopup(state_t *state, int toggle);
int		ui_DrawLaunchPopup(state_t *state, gamedata_t *gamedata, launchdat_t *launchdat, int toggle);
int		ui_DrawMainWindow();
int		ui_DrawSplash();
int		ui_DrawSplashProgress();
int		ui_DrawStatusBar();
int		ui_DrawTextPanel(int x, int y, int width);
int		ui_DisplayArtwork(FILE *screenshot_file, bmpdata_t *screenshot_bmp, state_t *state, imagefile_t *imagefile);

// Asset loaders
int		ui_LoadAssets();
int		ui_LoadFonts();
int		ui_LoadScreenshot(char *c);

// Output error or status messages
int		ui_ProgressMessage(char *c);
int		ui_StatusMessage(char *c);

// Change focus or selected state of UI elements
int		ui_SwitchPane(state_t *state);
int		ui_ReselectCurrentGame(state_t *state);

// These refresh contents within the various UI elements
int		ui_UpdateBrowserPane(state_t *state, gamedata_t *gamedata);
int		ui_UpdateBrowserPaneStatus(state_t *state);
int		ui_UpdateInfoPane(state_t *state, gamedata_t *gamedata, launchdat_t *launchdat);
