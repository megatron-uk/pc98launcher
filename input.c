/* input.c, Combined joystick and keyboar user input routines for the pc98Launcher.
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
#include <pc.h>

#include "input.h"

int input_get(){
	// Read joystick or keyboard input and return directions or buttons pressed
	
	int k;
	
	if (kbhit_98()){
		k = bioskey_98(0x00);
		switch(k){
			case(input_select):
				return input_select;
			case(input_cancel):
				return input_cancel;
			case(input_switch):
				return input_switch;
			case(input_up):
				return input_up;
			case(input_down):
				return input_down;
			case(input_left):
				return input_left;
			case(input_right):
				return input_right;
			case(input_scroll_up):
				return input_scroll_up;
			case(input_scroll_down):
				return input_scroll_down;
			case(input_quit):
				return input_quit;
			default:
				return input_none;
		}
	} else {
		return input_none;
	}
}