#include "LegendEngine.h"
#include <string>
#include <vector>



// setters -------------------------------------
void LegendEngine::setInput(int n, bool state) {
	if (n == 0 && state == true) {
		runRight();
	}
	else if (n == 1 && state == true) {
		runLeft(); 
	}
	else if (n == 2 && state == true) {
		runUp();
	}
	else if (n == 3 && state == true) {
		runDown();
	}
	playerPos();
	collision();
	calculateBorder();
	
}
void LegendEngine::setIdle(bool state) {
	player.idle = state;
}
// getters -------------------------------------

// -----------------------------------------------------------------------------
void LegendEngine::attackSword() {
	/* DO SOME ATTACK*/
	//std::cout << "Attack" << std::endl;
}
//----------------------------TMTPMTMPT-----------------------------------------
void LegendEngine::createMap() {
	for (int n = 0; n < 100; n++) {			// rows 
		for (int i = 0; i < 192; i++) {		// colums
			map[i][n] = '0';
		}
	}
}
//---------------------------------------------------------------------
void LegendEngine::createBox(int xBox, int yBox, int sizeX, int sizeY) {
	for (int n = 0; n < sizeY; n++) {		// rows
		for (int i = 0; i < sizeX; i++) {	// colums
			map[xBox + i][yBox + n] = '1';
		}
	}
}
//---------------------------------------------------------------------
void LegendEngine::exitGame() {				// stänger ner spelet 
	running = false;
}
//---------------------------------------------------------------------
void LegendEngine::newGame() {				// startar spel från menyn
	player.x = 1920 / 2;
	player.y = 1080 / 2;
	playerPos();
	player.face = 'D';
	background.cX = 1792;
	background.cY = 1176;
}
//---------------------------------------------------------------------
void LegendEngine::choice() {	 						// vad för aval man väljer i menyn
	int ymenu = 300, xmenu = 100;
	if (pause == true) {				// meny för paus
		if (a == 0) {
			menuActive = false;			// resume
		}
		else if (a == 1) {				// new game
			newGame();
			menuActive = false;
		}
		else if (a == 2) {				// instuctions
			//instructions();
		}
		else if (a == 3) {				// exit
			exitGame();
		}
	}
	else {								// meny när man startar spelet
		if (a == 0) {					// new game
			newGame();
			menuActive = false;
		}
		else if (a == 1) {				// instructions
			//instructions();
		}
		else if (a == 2) {				// exit
			exitGame();
		}
	}
}
//---------------------------------------------------------------------
void LegendEngine::getActive() {				// hämtar aktivt menyval
	int active;
	if (pause == true) {		// om man har pausat 
		if (y < 180) {
			active = 0;
		}
		else if (y < 230) {
			active = 1;
		}
		else if (y < 260) {
			active = 2;
		}
		else {
			active = 3;
		}
	}
	else {						// I originalmenyn
		if (y < 180) {
			active = 0;
		}
		else if (y < 230) {
			active = 1;
		}
		else {
			active = 2;
		}
	}

	if (active != a) {			// om man faktiskt har bytt val då ändras valet
		a = active;
	}
}
//---------------------------------------------------------------------
void LegendEngine::runRight() {
	player.x += 10;
	player.face = 'R';
}
//---------------------------------------------------------------------
void LegendEngine::runLeft() {
	player.x -= 10;
	player.face = 'L';
}
//---------------------------------------------------------------------
void LegendEngine::runUp() {
	player.y -= 10;
	player.face = 'U';
}
//---------------------------------------------------------------------
void LegendEngine::runDown() {
	player.y += 10;
	player.face = 'D';
}
//---------------------------------------------------------------------
void LegendEngine::playerPos() {

	player.posX = player.x / 10;
	player.posY = player.y / 10;
	for (int n = 0; n < 10; n++) {
		for (int i = 0; i < 9; i++) {
			map[player.posX + i][player.posY + n] = 'C';
		}
	}

	if (player.face == 'U') {
		for (int n = 0; n < 9; n++) {
			map[player.posX + n][player.posY + 10] = '0';
		}
	}
	else if (player.face == 'D') {
		for (int n = 0; n < 9; n++) {
			map[player.posX + n][player.posY - 1] = '0';
		}
	}
	else if (player.face == 'R') {
		for (int n = 0; n < 10; n++) {
			map[player.posX - 1][player.posY + n] = '0';
		}
	}
	else if (player.face == 'L') {
		for (int n = 0; n < 10; n++) {
			map[player.posX + 9][player.posY + n] = '0';
		}
	}

	//dispMap();
}
//---------------------------------------------------------------------
void LegendEngine::collision() {
	for (int n = 0; n < 4; n++) {
		hittmp[n] = '0';
	}

	for (int n = 0; n < 10; n++) {
		if (map[player.posX + 9][player.posY + n] == '1') {
			hittmp[0] = 'R';
		}
		if (map[player.posX - 1][player.posY + n] == '1') {
			hittmp[1] = 'L';
		}
	}
	for (int n = 0; n < 9; n++) {
		if (map[player.posX + n][player.posY - 1] == '1') {
			hittmp[2] = 'U';
		}
		if (map[player.posX + n][player.posY + 10] == '1') {
			hittmp[3] = 'D';
		}
	}
}
//---------------------------------------------------------------------
void LegendEngine::playerAnimation() {
	player.sizeX = 15;
	player.sizeY = 16;
	if (player.face == 'D') {
		player.cX = 0;
		if (player.idle != true) {
			player.cY += 30;
			if (player.cY > 30) {
				player.cY = 0;
			}
		}
	}
	else if (player.face == 'L') {
		player.cX = 30;
		if (player.idle != true) {
			player.cY += 30;
			if (player.cY > 30) {
				player.cY = 0;
			}
		}
	}
	else if (player.face == 'U') {
		player.cX = 60;
		if (player.idle != true) {
			player.cY += 30;
			if (player.cY > 30) {
				player.cY = 0;
			}
		}
	}
	else if (player.face == 'R') {
		player.cX = 90;
		if (player.idle != true) {
			player.cY += 30;
			if (player.cY > 30) {
				player.cY = 0;
			}
		}
	}
}
//---------------------------------------------------------------------
void LegendEngine::swordAnimation() {
	if (player.face == 'D') {
		player.sizeX = 16;
		player.sizeY = 28;
		player_Hei = 160;
		player.cX = 0;
		player.cY += 25;
		if (player.cY > 85) {
			attackSword();
			player.sizeY = 17;
			player.cY = 59;
			player_Hei = 96;
			player.attack = false;
		}
		if (player.attack == false) {
			player.cY = 0;
			playerAnimation();
		}
	}
	else if (player.face == 'L') {
		player.sizeX = 28;
		player.sizeY = 17;
		player_Wid = 160;
		player.cX = 23;
		player.cY += 29;
		player.x -= 72;
		if (player.cY > 90) {
			attackSword();
			player.x += 72;
			player.sizeX = 15;
			player.sizeY = 16;
			player.cY = 59;
			player_Wid = 90;
			player.attack = false;
		}
		if (player.attack == false) {
			player.x += 72;
			player.cY = 0;
			playerAnimation();
		}
	}
	else if (player.face == 'U') {
		player.sizeX = 16;
		player.sizeY = 28;
		player_Hei = 160;
		player.cX = 60;
		player.cY += 25;
		player.y -= 60;
		if (player.cY > 85) {
			attackSword();
			player.y += 60;
			player.sizeY = 17;
			player.cY = 59;
			player_Hei = 96;
			player.attack = false;
		}
		if (player.attack == false) {
			player.y += 60;
			player.cY = 0;
			playerAnimation();
		}
	}
	else if (player.face == 'R') {
		player.sizeX = 28;
		player.sizeY = 17;
		player_Wid = 160;
		player.cX = 84;
		player.cY += 29;
		if (player.cY > 90) {
			attackSword();
			player.sizeX = 15;
			player.sizeY = 16;
			player.cY = 59;
			player_Wid = 90;
			player.attack = false;
		}
		if (player.attack == false) {
			player.cY = 0;
			playerAnimation();
		}
	}
}
//---------------------------------------------------------------------
void LegendEngine::calculateBorder() {
	if (player.y <= 0) {
		entrance('U');
	}
	else if (player.y > 920) {
		entrance('D');
	}
	else if (player.x > 1800) {
		entrance('R');
	}
	else if (player.x < 0) {
		entrance('L');
	}
}
//---------------------------------------------------------------------
void LegendEngine::entrance(char dir) {
	//y = 168 px, x = 256px
	if (dir == 'U') {
		background.cY -= 168;
		player.y = 930;
	}
	else if (dir == 'D') {
		background.cY += 168;
		player.y = 10;
	}
	else if (dir == 'R') {
		background.cX += 256;
		player.x = 10;
	}
	else if (dir == 'L') {
		background.cX -= 256;
		player.x = 1790;
	}
}
//---------------------------------------------------------------------

