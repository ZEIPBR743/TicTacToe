//*****************************************************************************
// main.c
// Author: Liyuan Gu
//*****************************************************************************
#include "lab_buttons.h"
#include "BuckyBadger.black.h"		// important information about the image size in here.
#include "Invader.black.h"

/******************************************************************************
 * Global Constants and Variables
 *****************************************************************************/
#define MOVE_PIXELS   	1							// Number of Pixels to move each time.
#define LCD_SIZE_X			240						// X Size of LCD screen in pixels
#define LCD_SIZE_Y			320						// Y Size of LCD screen in pixels
#define GRID_SIZE				LCD_SIZE_X/ 3 - 2			// Side length of a single grid
#define BOARDER_LINE_SIZE 1						// thickness of boarderline 

#define DRAW_CIRCLE			1							//numercal code for circle
#define RED_COLOR		    1							//red
#define MEGENTA_COLOR		2							//red
#define BLUE_COLOR      3

#define LOCAL_ID     150
#define REMOTE_ID    148

#undef DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif


//*****************************************************************************
//*****************************************************************************

void print_circle(int i, int j, int a, int b);
void print_custom_image(int i, int j, int a);  //prints grid with a customized image.
void print_board(bool board[3][3], int x);
void erase_whole_board(int);	//this method erase whole board except for the last one line
bool boardfull(bool board[3][3], bool otherBoard[3][3]);			

	
void erase_a_grid(int i,int j);
bool check_game_won (bool board[3][3]);
uint32_t rx_data;

uint32_t convert(int i, int j);
int convert_back_i(uint32_t x);
int convert_back_j(uint32_t y   );


int main () {
	
int count = 0;
int otherCount = 0;

int i = 1;										//row 0-2
int j = 1;										//column 0-2
bool  mainBoard[3][3];				//user's board
bool otherBoard[3][3];				//other player's board
bool wholeGame = true;			  // bool variable for the entire game 
bool reset = true;


while(reset) {
bool game = true; 						//bool variable to control whether user should select or the other player should slelect

ece210_initialize_board();
ece210_wireless_init(LOCAL_ID,REMOTE_ID);
wholeGame = true;
//menu screen
    ece210_lcd_add_msg("TIC TAC TOE",TERMINAL_ALIGN_CENTER,LCD_COLOR_WHITE);
    ece210_lcd_add_msg("\n",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE2);
     ece210_lcd_add_msg("Player 1 is Blue",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
    ece210_lcd_add_msg("Player 2 is Red", TERMINAL_ALIGN_CENTER, LCD_COLOR_RED);
    ece210_lcd_add_msg("\n",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE2);
    ece210_lcd_add_msg("Goodluck!",TERMINAL_ALIGN_CENTER,LCD_COLOR_WHITE);
	  ece210_wait_mSec(2000);
	
i = 1;
j = 1;
																				// board initialization
	for (int i = 0; i< 3; i++){						// initialize both boards to all false
		for (int j = 0; j< 3; j++){
			mainBoard[i][j] = false;
		}
	}
	
	for (int i = 0; i< 3; i++){						// initialize both boards to all false
		for (int j = 0; j< 3; j++){
			otherBoard[i][j] = false;
		}
	}


while (wholeGame){			// While loop for the entire game	
/*
	to do:
	add determination which player should go first 
	if this user go first, set game = ture;
	else, set game to false.
*/
	// below are temporary test code substitute for above todo


// while loop wait for the other user to select
while (!game){
			int x;
			int y;
	
	ece210_wait_mSec(2000);
	ece210_lcd_draw_rectangle  (0, LCD_SIZE_X, 0, LCD_SIZE_Y , LCD_COLOR_BLACK);
	
			//ece210_lcd_add_msg("j",TERMINAL_ALIGN_LEFT,LCD_COLOR_GREEN2);
		if (ece210_wireless_data_avaiable()) {
			  x = convert_back_i( ece210_wireless_get());
				y = convert_back_j( ece210_wireless_get());
				otherBoard[x][y] = true;
				game = true;
		}
}

print_board(mainBoard, RED_COLOR);
print_board(otherBoard, BLUE_COLOR);


if(check_game_won(otherBoard)) {
			otherCount = otherCount + 1;
		/////////////////////
		//send signals
// 1. that you won		**TODO
		/////////////////////
		//erase_whole_board();
		if (otherCount >= 8) {
			wholeGame = false;
		}
	
		else {
		ece210_lcd_add_msg("You have LOST this round",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
		ece210_lcd_add_msg("Press DOWN for next round",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
		erase_whole_board(40);
			while(1){
				if( AlertButtons)				// AlertButtons check to see if any push button was pushed.
				{
					AlertButtons = false;
					if (btn_down_pressed()) {		// user finish selection
						wholeGame = false;
						game = false;
					break;
					}
				}
			}
		}	
}

if (boardfull (mainBoard, otherBoard) ) {
		ece210_lcd_add_msg("You have TIED this round",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
		ece210_lcd_add_msg("Press DOWN for next round",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
		erase_whole_board(40);
			while(1){
				if( AlertButtons)				// AlertButtons check to see if any push button was pushed.
				{
					AlertButtons = false;
					if (btn_down_pressed()) {		// user finish selection
						wholeGame = false;
						game = false;
					break;
					}
				}
			}
		} 
	//////////////////////////////////////////////////////////////////////////////////////
//While loop for user to select
while(game){	

		ece210_lcd_add_msg("Press DOWN to select",TERMINAL_ALIGN_CENTER,LCD_COLOR_GREEN2);
		erase_whole_board(20);
		print_circle(i, j, 1, 2);	
	
	while(true)	   // Begin USER_SELECTION while loop
  { 
					
				print_board(mainBoard, RED_COLOR);
				print_board(otherBoard, BLUE_COLOR );
				print_circle(i, j, 1, 2);	
		
		//ece210_lcd_add_msg("j",TERMINAL_ALIGN_LEFT,LCD_COLOR_GREEN2);
    if (AlertButtons)				// AlertButtons check to see if any push button was pushed.
    {
      AlertButtons = false;
			//ece210_lcd_add_msg("Press DOWN to select",TERMINAL_ALIGN_CENTER,LCD_COLOR_GREEN2);
			
			if(!mainBoard[i][j] || !otherBoard[i][j]) {
			erase_a_grid(i, j);
			}
														// Below 3 if-statements allow users to select position on grid
	
			if (btn_right_pressed()) { 	//change column (j)

					if (j == 2){
						j = 0;
					}
					else{
						j++;
					}
				}

			if (btn_left_pressed() ) { 	//change column (j)
	
					if (j == 0){
						j = 2;
					}
					else {
						j--;
					}
				}
			
			if (btn_up_pressed() ) {			//change row (i)
			
				if (i == 0) {
					i = 2;	
				}
				else {
					i --;
				}	
			}
			
			if (btn_down_pressed()) {		// user finish selection
																	// Display words **TODO
					if (mainBoard[i][j]||otherBoard[i][j]){
						ece210_lcd_add_msg("Unable to select ",TERMINAL_ALIGN_CENTER,LCD_COLOR_GREEN2);
						erase_whole_board(20);
					}
					else {
						mainBoard[i][j] = true;	
						ece210_wireless_send(convert(i , j));
						game = false;
						break;
					}		
			}
			
			//erase_a_grid(i, j);	

	
		}
}																// End USER_SELECTION while loop		

	mainBoard[i][j] = true;				//set user selection and compare game
  print_board(mainBoard, DRAW_CIRCLE );	


	if(check_game_won(mainBoard)) {
			count = count + 1;
		for (int i = 0; i < count; i ++) {
			ece210_ws2812b_write(i, (5 * i + 6), - 2 * (i - 4 ) * (i - 4) + 32, (40 - 5 * i));
		}
		ece210_lcd_add_msg("You have WON this round",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
		ece210_lcd_add_msg("Press DOWN for next round",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
		erase_whole_board(40);
			while(1){
				if( AlertButtons)				// AlertButtons check to see if any push button was pushed.
				{
					AlertButtons = false;
					if (btn_down_pressed()) {		// user finish selection
						wholeGame = false;
															// Display words **TODO 
					break;
					}
				}
			}
	
		//ece210_lcd_add_msg("your score:" + to_string(count),TERMINAL_ALIGN_CENTER,LCD_COLOR_YELLOW);
		
	}

		if (boardfull (mainBoard, otherBoard) ) {
		ece210_lcd_add_msg("You have TIED this round",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
		ece210_lcd_add_msg("Press DOWN for next round",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
		erase_whole_board(40);
			while(1){
				if( AlertButtons)				// AlertButtons check to see if any push button was pushed.
				{
					AlertButtons = false;
					if (btn_down_pressed()) {		// user finish selection
						wholeGame = false;
					break;
					}
				}
			}
		} 
			
}
	////////END userselection loop///////


		//game = true;
	
	
		if (count >=8) {
			////////send signal/////////
			ece210_lcd_add_msg("You have WON the game!",TERMINAL_ALIGN_CENTER,LCD_COLOR_MAGENTA);
			erase_whole_board(40);
			wholeGame = false;
		}	
  }
  //ece210_initialize_board(); //?????
  //ece210_lcd_add_msg("Animation TEST CODE\n\r",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
  //ece210_lcd_draw_image(x_pos,width_pixels, y_pos,height_pixels, bucky_bitmap,LCD_COLOR_RED, LCD_COLOR_BLACK);
  // draws the whole image 
	
	
}
		//ece210_lcd_draw_image(x_pos,width_pixels, y_pos,height_pixels, bucky_bitmap,LCD_COLOR_RED, LCD_COLOR_BLACK);
									// draw image at the new position using size and bitmap info and forground and background info.	    
	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////// end main///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	
///Draws whole board///////////////////////////////////////////////////////////////////////////////////////////////////
void print_board(bool board[3][3], int x ){ 
	for (int i = 0; i < 3; i ++){				//go through each element passed in from the 2D array
		for (int j = 0; j < 3; j++){
															
				if (board[i][j])
					{								
					print_circle(i,j, 1, x);														//draw the circle element if true on 2D array 
				}	
		}
	}
ece210_lcd_draw_line(80, 0, 80, 240, LCD_COLOR_WHITE);
ece210_lcd_draw_line(160, 0, 160, 240, LCD_COLOR_WHITE);
ece210_lcd_draw_line(0, 80, 240, 80, LCD_COLOR_WHITE);
ece210_lcd_draw_line(0, 160, 240, 160, LCD_COLOR_WHITE);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Method that puts a circle in a specified grid////////////////////////////////////////////////////////////////////////
void print_circle(int i, int j, int radius, int color){ 
	//	extern image = bucky_bitmap;			// *TODO: change image selection
//	if (a == 0){
//		
//	}	
	if (color == RED_COLOR){
	
	if (i == 0 && j == 0){
			ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 1),(LCD_SIZE_X / 6),10, LCD_COLOR_RED);
	}
	if (i == 0 && j == 1){
		  ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 3),(LCD_SIZE_X / 6),10, LCD_COLOR_RED);
	}
	if (i == 0 && j == 2){
			ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 5), (LCD_SIZE_X / 6),10, LCD_COLOR_RED);
	}
if (i == 1 && j == 0){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 1), (LCD_SIZE_X / 6 * 3),10,LCD_COLOR_RED);
	}
if (i == 1 && j == 1){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 3),(LCD_SIZE_X / 6 * 3),10, LCD_COLOR_RED);
	}
if (i == 1 && j == 2){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 5),(LCD_SIZE_X / 6 * 3),10, LCD_COLOR_RED);
	}
if (i == 2 && j == 0){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 1), (LCD_SIZE_X / 6 * 5),10,LCD_COLOR_RED);
	}
if (i == 2 && j == 1){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 3), (LCD_SIZE_X / 6 * 5),10, LCD_COLOR_RED);
	}
if (i == 2 && j == 2){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 5), (LCD_SIZE_X / 6 * 5),10, LCD_COLOR_RED);
	}
}
	
	if (color == BLUE_COLOR){
	
	if (i == 0 && j == 0){
			ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 1),(LCD_SIZE_X / 6),10, LCD_COLOR_BLUE);
	}
	if (i == 0 && j == 1){
		  ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 3),(LCD_SIZE_X / 6),10, LCD_COLOR_BLUE);
	}
	if (i == 0 && j == 2){
			ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 5), (LCD_SIZE_X / 6),10, LCD_COLOR_BLUE);
	}
if (i == 1 && j == 0){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 1), (LCD_SIZE_X / 6 * 3),10,LCD_COLOR_BLUE);
	}
if (i == 1 && j == 1){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 3),(LCD_SIZE_X / 6 * 3),10, LCD_COLOR_BLUE);
	}
if (i == 1 && j == 2){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 5),(LCD_SIZE_X / 6 * 3),10, LCD_COLOR_BLUE);
	}
if (i == 2 && j == 0){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 1), (LCD_SIZE_X / 6 * 5),10,LCD_COLOR_BLUE);
	}
if (i == 2 && j == 1){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 3), (LCD_SIZE_X / 6 * 5),10, LCD_COLOR_BLUE);
	}
if (i == 2 && j == 2){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 5), (LCD_SIZE_X / 6 * 5),10, LCD_COLOR_BLUE);
	}
}
	
if (color == 2){
	
	if (i == 0 && j == 0){
			ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 1),(LCD_SIZE_X / 6),10, LCD_COLOR_MAGENTA);
	}
	if (i == 0 && j == 1){
		  ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 3),(LCD_SIZE_X / 6),10, LCD_COLOR_MAGENTA);
	}
	if (i == 0 && j == 2){
			ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 5), (LCD_SIZE_X / 6),10, LCD_COLOR_MAGENTA);
	}
		if (i == 1 && j == 0){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 1), (LCD_SIZE_X / 6 * 3),10,LCD_COLOR_MAGENTA);
	}
	if (i == 1 && j == 1){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 3),(LCD_SIZE_X / 6 * 3),10, LCD_COLOR_MAGENTA);
	}
	if (i == 1 && j == 2){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 5),(LCD_SIZE_X / 6 * 3),10, LCD_COLOR_MAGENTA);
	}
	if (i == 2 && j == 0){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 1), (LCD_SIZE_X / 6 * 5),10,LCD_COLOR_MAGENTA);
	}
	if (i == 2 && j == 1){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 3), (LCD_SIZE_X / 6 * 5),10, LCD_COLOR_MAGENTA);
	}
	if (i == 2 && j == 2){
		ece210_lcd_draw_circle((LCD_SIZE_X / 6 * 5), (LCD_SIZE_X / 6 * 5),10, LCD_COLOR_MAGENTA);
	}
}
	}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////METHOD **ADD DISCRIPTION
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void erase_a_grid(int i,int j){
	
	
	if (i == 0 && j == 0){
			ece210_lcd_draw_rectangle  (BOARDER_LINE_SIZE, GRID_SIZE, BOARDER_LINE_SIZE, GRID_SIZE, LCD_COLOR_BLACK);
	}
	if (i == 0 && j == 1){
		  ece210_lcd_draw_rectangle  (BOARDER_LINE_SIZE + LCD_SIZE_X / 3, GRID_SIZE, BOARDER_LINE_SIZE, GRID_SIZE, LCD_COLOR_BLACK);
	}
	if (i == 0 && j == 2){
			ece210_lcd_draw_rectangle  (BOARDER_LINE_SIZE + LCD_SIZE_X / 3 * 2, GRID_SIZE, BOARDER_LINE_SIZE, GRID_SIZE, LCD_COLOR_BLACK);
	}
if (i == 1 && j == 0){
		ece210_lcd_draw_rectangle  (BOARDER_LINE_SIZE, GRID_SIZE, BOARDER_LINE_SIZE + LCD_SIZE_X / 3, GRID_SIZE, LCD_COLOR_BLACK);
	}
if (i == 1 && j == 1){
		ece210_lcd_draw_rectangle  (BOARDER_LINE_SIZE + LCD_SIZE_X / 3, GRID_SIZE, BOARDER_LINE_SIZE + LCD_SIZE_X / 3, GRID_SIZE, LCD_COLOR_BLACK);
	}
if (i == 1 && j == 2){
		ece210_lcd_draw_rectangle  (BOARDER_LINE_SIZE + LCD_SIZE_X / 3 * 2, GRID_SIZE,BOARDER_LINE_SIZE + LCD_SIZE_X / 3, GRID_SIZE, LCD_COLOR_BLACK);
	}
if (i == 2 && j == 0){
	ece210_lcd_draw_rectangle  (BOARDER_LINE_SIZE, GRID_SIZE, BOARDER_LINE_SIZE + LCD_SIZE_X / 3 *2 , GRID_SIZE, LCD_COLOR_BLACK);
	}
if (i == 2 && j == 1){
		ece210_lcd_draw_rectangle  (BOARDER_LINE_SIZE + LCD_SIZE_X / 3, GRID_SIZE, BOARDER_LINE_SIZE + LCD_SIZE_X / 3 *2 ,GRID_SIZE, LCD_COLOR_BLACK);
	}
if (i == 2 && j == 2){
		ece210_lcd_draw_rectangle  (BOARDER_LINE_SIZE + LCD_SIZE_X / 3 * 2, GRID_SIZE, BOARDER_LINE_SIZE + LCD_SIZE_X / 3 *2 ,GRID_SIZE, LCD_COLOR_BLACK);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////METHOD **ADD DISCRIPTION
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void erase_whole_board(int a){
	ece210_lcd_draw_rectangle  (0, LCD_SIZE_X, 0, LCD_SIZE_Y - a, LCD_COLOR_BLACK);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////METHOD **ADD DISCRIPTION
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool 
check_game_won(bool board[3][3])
{
	if (board[0][0] && board[0][1] && board[0][2]){
		return true;
	}
	if (board[1][0] && board[1][1] && board[1][2]){
		return true;
	}
	if (board[2][0] && board[2][1] && board[2][2]){
		return true;
	}
	if (board[0][0] && board[1][0] && board[2][0]){
		return true;
	}
	if (board[0][1] && board[1][1] && board[2][1]){
		return true;
	}
	if (board[0][2] && board[1][2] && board[2][2]){
		return true;
	}
	if (board[0][0] && board[1][1] && board[2][2]){
		return true;
	}
	if (board[0][2] && board[1][1] && board[2][0]){
		return true;
	}
	else {
		return false;
	}
}

uint32_t convert(int i, int j){
	if (i == 0 && j == 0){
			return 0x00;
	}
	if (i == 0 && j == 1){
		  return 0x01;
	}
	if (i == 0 && j == 2){
			return 0x02;
	}
if (i == 1 && j == 0){
		return 0x03;
	}
if (i == 1 && j == 1){
		return 0x04;
	}
if (i == 1 && j == 2){
		return 0x05;
	}
if (i == 2 && j == 0){
	  return 0x06;
	}
if (i == 2 && j == 1){
		return 0x07;
	}
if (i == 2 && j == 2){
		return 0x08;
	}
else return 0;
}

int convert_back_i(uint32_t x){
if (x == 0x00000000 || x == 0x00000001 || x == 0x00000002){
		return 0;
	}

if (x == 0x00000003 || x == 0x00000004 || x == 0x00000005){
		return 1;
	}

if (x == 0x00000006 || x == 0x00000007 || x == 0x00000008){
	return 2;
	}
else;
	return 2;
}

int convert_back_j(uint32_t y){
if (y == 0x00000000 || y == 0x00000003 || y == 0x00000006){
		return 0;
	}
if (y == 0x00000001 || y == 0x00000004 || y == 0x00000007){
		return 1;
	}
if (y == 0x00000002 || y == 0x00000005 || y == 0x00000008){
		return 2;
	}
else 
	return 2;
}

bool boardfull(bool board[3][3], bool otherBoard[3][3]){
	int count = 0;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			if (board[i][j] || otherBoard [i][j]){
				count++;
			}
		}
	}
	if (count >=9){
		return true;
	}		
	else{
		return false;
	}
}