﻿#define WIN32_LEAN_AND_MEAN	
#include <Windows.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

#pragma warning(disable : 4996):
// Globla variabler ---------------------------------------------------------
LPCTSTR		ClsName = "Bgstuff";
HWND		hWnd;
double		CPUFreq = 0.0;
bool		running = true;
int			innerWidth, innerHeight;
// pic and window
int			app_Wid = 1920;
int			app_Hei = 1080;
int			player_Hei = 96;	// player width = 15 = 10 rutor hög
int			player_Wid = 90;	// player Height = 16 = 9 rutor bredd
int			bg_Wid = 1920;
int			bg_Hei = 1080;
int			x , y;
// player animation
int			xpic = 144, ypic = 0;
int			playerX = app_Wid / 2 - 72, playerY = 275;
// player attack
bool swordItem = true; /*jiofrojifWJOÖJÖGFElna<iugfhliWERNGLIUENRGLwngriuonbfliu.nrgbioönbA<BRIOUÖNrg*/
// menu
bool		menuActive = true;
bool		pause = false;
int			a = 0;
// struct
struct BG {
	HDC hdc;
	HBITMAP map;
	int x;
	int y;
	int cX = 1792;					// ETT RUM:	y=168 px, x=256px
	int cY = 1176;
	int sizeX = 256;
	int sizeY = 168;
};
struct user {
	HDC hdc;
	HBITMAP map;
	int x = 1920 / 2;
	int y = 1080 / 2;
	int posX = 1920 / 10;
	int posY = 1080 / 10;
	int cX = 0;					// en lin gå: y=16 px, x=15px
	int cY = 0;
	int sizeX = 15;
	int sizeY = 16;
	char face = 'D';
	bool idle = true;
	bool attack = false;
	int moners;
	int hp = 5;
};
// All mighty
BG			background;
user		player;
char		map[192][100];
char		hittmp;
// fonter
HFONT		myFonts[3];
// Device Contexts ----------------------------------------------------------
HDC			hDC;					// V�r huvudsakliga DC - Till f�nstret
HDC			spritesHDC;				// DC till player
HDC			backgroundHDC;			// DC till background
HDC			bufferHDC;				// hdc till buffer
// BITMAPS ------------------------------------------------------------------
HBITMAP		sprites;				// all the sprites
HBITMAP		oldBitmap[2];			// Lagrar orginalbilderna
HBITMAP		bitmapbuff;				// lagrar bilden till bitmapen
// Funktioner ---------------------------------------------------------------
void		runRight();				// spelaren springer höger
void		runLeft();				// spelaren springer vänster
void		runUp();				// spelaren springer uppåt
void		runDown();				// spelaren springer neråt
void		playerAnimation();		// animation för spelaren
void		swordAni();
// menu
void        newGame();				// startar spel från menyn
void		instructions();			// instruktioner för spelet
void		exitGame();				// avsluta spelet 
void        printMenu();			// skriver ut menyn
void		choice();				// vad för aval man väljer i menyn
void        getActive();			// hämtar aktivt menyval
// game progression
void		entrance(char);			// flytta bakgrunden
void		calculateBorder();		// räkna ut om man borde flyta backgrund
void		playerPos();
char		collision();
void		createBox(int, int, int, int);
// kill kill kill
void		attackSword();
void		swordAni();

// tmp
void		dispMap();
void		createMap();

// Funktioner för windows ---------------------------------------------------
LRESULT		CALLBACK	winProc(HWND, UINT, WPARAM, LPARAM);
ATOM 		doRegister(HINSTANCE);
BOOL 		initInstance(HINSTANCE, int);
int			initalizeAll(HWND);
void		releaseAll(HWND);
void		update();				// Alla uppdateringar
void		render();				// Ritar ut bilden i f�nstret
bool		framerate(int);			// Uppdateringsfrekvensen
double		getFreq();
//---------------------------------------------------------------------
inline __int64 performanceCounter() noexcept {
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);
	return li.QuadPart;
};
//---------------------------------------------------------------------
int WINAPI WinMain(_In_ HINSTANCE hi, _In_opt_ HINSTANCE hp, _In_ LPSTR lp, _In_ int n) {
	UNREFERENCED_PARAMETER(hp);
	UNREFERENCED_PARAMETER(lp);
	if (!(doRegister(hi)) || !(initInstance(hi, n))) {
		MessageBox(NULL, (LPCSTR)"Fel, Kan ej registrera eller skapa f�nstret i windows", (LPCSTR)"ERROR", MB_ICONERROR | MB_OK);
		return 0;
	}
	MSG msg;

	// Ny loop d�r vi sj�lv best�mmer n�r saker ritas ut
	while (running == true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (framerate(12) == true) {
			render();
			update();	// om menyn är igång upptatera inga animationer
		}
	}
	return 0;
}
//---------------------------------------------------------------------
bool framerate(int timeStamp) {
	static __int64 last = performanceCounter();
	if (((double)((performanceCounter() - last)) / CPUFreq) > timeStamp) {
		last = performanceCounter();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------
LRESULT CALLBACK winProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	char hit; 
	switch (Msg) {
	case WM_CREATE:
		initalizeAll(hWnd);
		break;
	case WM_LBUTTONDOWN:
		if (menuActive == true) {			// om man är i menyn kör aktivt val
			choice();
			break;
		}
		dispMap();
		break;
	case WM_MOUSEMOVE:
		if (menuActive == true) {			// om man är i menyn tracka musen
			y = HIWORD(lParam);
			getActive();
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		running = false;
		break;
	case WM_KEYDOWN:
		player.idle = false;
		hittmp = collision();
		playerPos();
		if (wParam == VK_RIGHT && hittmp != 'R') {			// rör spelaren till vänster
			runRight();
		}
		else if (wParam == VK_LEFT && hittmp != 'L') {		// rör spelaren till höger
			runLeft();
		}
		else if (wParam == VK_UP && hittmp != 'U') {			// rör spelaren till höger
			runUp();
		}
		else if (wParam == VK_DOWN && hittmp != 'D') {		// rör spelaren till höger
			runDown();
		}
		else if (wParam == VK_SPACE && swordItem == true && player.attack == false) {		// rör spelaren till höger
			player.cY = 59;
			player.attack = true;
			attackSword();
		}
		else if (wParam == VK_ESCAPE && menuActive == false) {		// öppnar menyn när man trycker på ESC
			menuActive = true;
			pause = true;
		}
		calculateBorder();
		break;
	case WM_KEYUP:							
		player.idle = true;
		break;
	case WM_DESTROY:
		releaseAll(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}
//---------------------------------------------------------------------
void attackSword() {
	/* DO SOME ATTACK*/
	std::cout << "Attack" << std::endl;
}
//----------------------------TMTPMTMPT-----------------------------------------
void dispMap() {
	system("CLS");
	for (int n = 0; n < 100; n++) {			// rows 
		for (int i = 0; i < 192; i++) {		// colums
			if (i != 181) {
				std::cout << map[i][n];
			}
		}
		std::cout << std::endl;
	}
}
void createMap() {
	for (int n = 0; n < 100; n++) {			// rows 
		for (int i = 0; i < 192; i++) {		// colums
			map[i][n] = '0';
		}
	}
}
//---------------------------------------------------------------------
void createBox(int xBox, int yBox, int sizeX, int sizeY) {
	for (int n = 0; n < sizeY; n++) {		// rows
		for (int i = 0; i < sizeX; i++) {	// colums
			map[xBox + i][yBox + n] = '1';
		}
	}
}
//---------------------------------------------------------------------
void exitGame() {				// stänger ner spelet 
	running = false;
	PostQuitMessage(0);
}
//---------------------------------------------------------------------
void newGame() {				// startar spel från menyn
	player.x = 1920 / 2;
	player.y = 1080 / 2;
	playerPos();
	player.face = 'D';
	background.cX = 1792;    
	background.cY = 1176;
}
//---------------------------------------------------------------------
void instructions()	{	// instruktioner för spelet 
	std::string text[5] = { "Walk left and Right using the Arrow Keys","Monsters are bad. Kill them by clicking on them and making cool explosions","Dont get touched by bad monsters.It hurts", "Get points by how many monsters you kill.", "good luck!" };
	int langd = 5;
	int instX = 320, instY = 0;

	for (int n = 0; n < langd; n++) {
		instY = 40 * (n + 1) + 120;
		TextOut(bufferHDC, instX, instY, text[n].c_str(), text[n].size());
	}
}
//---------------------------------------------------------------------
void choice() {	 						// vad för aval man väljer i menyn
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
			instructions();
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
			instructions();
		}
		else if (a == 2) {				// exit
			exitGame();
		}
	}
}
//---------------------------------------------------------------------
void getActive() {				// hämtar aktivt menyval
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
void runRight() {
	player.x += 10;
	player.face = 'R';
}
//---------------------------------------------------------------------
void runLeft() {
	player.x -= 10;
	player.face = 'L';
}
//---------------------------------------------------------------------
void runUp() {
	player.y -= 10;
	player.face = 'U';
}
//---------------------------------------------------------------------
void runDown() {
	player.y += 10;
	player.face = 'D';
}
//---------------------------------------------------------------------
void playerPos() {
	
	// tmp tills en annan lösning, måste ha så den bara tar tillbaka det man har gått på och inte hela banan
	/*
	for (int n = 0; n < 100; n++) {			// rows 
		for (int i = 0; i < 192; i++) {		// colums
			map[i][n] = '0';
		}
	}
	*/

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
char collision() {
	char hit = '0';

	for (int n = 0; n < 10; n++) {
		if (map[player.posX + 10][player.posY + n] == '1') {
			hit = 'R';
		}
		if (map[player.posX - 2][player.posY + n] == '1') {
			hit = 'L';
		}
	}
	for (int n = 0; n < 9; n++) {
		if (map[player.posX + n][player.posY - 2] == '1') {
			hit = 'U';
		}
		if (map[player.posX + n][player.posY + 11] == '1') {
			hit = 'D';
		}
	}

	std::cout << hit;
	return hit;
}
//---------------------------------------------------------------------
void playerAnimation() {
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
void swordAnimation() {
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
void update() {
	static int counter = 0;
	counter++;
	if (counter % 12 == 0) {
		if (player.attack == true) {
			swordAnimation();
		}
		else {
			playerAnimation();
		}
	}
	if (counter % 500 == 0) {
	//	dispMap();
	}
}
//---------------------------------------------------------------------
void printMenu() {
	SetBkMode(bufferHDC, TRANSPARENT);	// så att texten har en transparent backgrund
	static bool first = true;

	if (first == true) {				// kör endast en gång när man startar spelet
			// background
		TransparentBlt(bufferHDC, 0, 0, app_Wid, app_Hei, background.hdc, background.cX, background.cY, background.sizeX, background.sizeY, COLORREF(RGB(255, 0, 255)));
		first = false;
	}

	RECT rect;
	GetClientRect(hWnd, &rect);
	int antal;												// alla strings för texterna
	std::string pauseText[4] = { "RESUME", "NEW GAME", "INSTRUCTIONS", "EXIT" };
	std::string text[3] = { "NEW GAME", "INSTRUCTIONS", "EXIT" };
	std::string header = "LEGEND OF ZELDA: BREATH OF THE WIN32";
	std::string menu;
	if (pause == true) {
		antal = 4;
	}
	else {
		antal = 3;
	}
	SelectObject(bufferHDC, myFonts[1]);					// rubriken
	SetTextColor(bufferHDC, COLORREF(RGB(245, 66, 197)));
	TextOut(bufferHDC, 300, 50, header.c_str(), header.size());

	SelectObject(bufferHDC, myFonts[0]);					// menyvalen
	for (int n = 0; n < antal; n++) {
		if (pause == true) {
			menu = pauseText[n];
		}
		else {
			menu = text[n];
		}

		rect.top = 150 + (n * 40);
		if (n == a) {										// markerar valet man är över
			SetTextColor(bufferHDC, COLORREF(RGB(200, 45, 45)));
		}
		else {
			SetTextColor(bufferHDC, COLORREF(RGB(245, 66, 197)));
		}
		// skriver ut allt till buffern
		TextOut(bufferHDC, 50, rect.top, menu.c_str(), menu.size());
	}

}
//---------------------------------------------------------------------
void calculateBorder() {
	if (player.y < 0) {
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
void entrance(char dir) {
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
void render() {
	static bool yes = true;
	if (menuActive == true) {	// om menyn är igång kör endast menyn
		printMenu();
		BitBlt(hDC, 0, 0, innerWidth, innerHeight, bufferHDC, 0, 0, SRCCOPY);
		return;
	}
	//TransparentBLT(hdc till, x utskrift på skrämen, y utskrift på skärmen, hur brett den ska rita ut, hur högt, hdc ifrån, vart från bilden börjar ta x, vart från bilden börja ta y, x antal pixlar att ta, y antalet pixalr att ta, färg att ta bort)

	TransparentBlt(bufferHDC, 0, 0, app_Wid, app_Hei, background.hdc, background.cX, background.cY, background.sizeX, background.sizeY, COLORREF(RGB(255, 0, 255)));
	TransparentBlt(bufferHDC, player.x, player.y, player_Wid, player_Hei, player.hdc, player.cX, player.cY, player.sizeX, player.sizeY, COLORREF(RGB(255, 0, 255)));

	// dubbelbuffring
	std::string nextHit(1, hittmp);
	std::string cords = std::to_string(player.x) + ", " + std::to_string(player.y) + ", " + nextHit;
	SelectObject(bufferHDC, myFonts[2]);					// rubriken
	SetTextColor(bufferHDC, COLORREF(RGB(2, 0, 110)));
	TextOut(bufferHDC, 5, 5, cords.c_str(), cords.size());
	BitBlt(hDC, 0, 0, innerWidth, innerHeight, bufferHDC, 0, 0, SRCCOPY);
	
}
//---------------------------------------------------------------------
int	initalizeAll(HWND hWnd) {
	srand(time(NULL));

	// H�mta f�nstrets riktiga bredd & h�jd
	RECT		windowRect;
	GetClientRect(hWnd, &windowRect);
	innerWidth = windowRect.right;
	innerHeight = windowRect.bottom;

	hDC = GetDC(hWnd);			// Koppla f�nstret till en DC

	// skapar fonter
	AddFontResource("The Wild Breath of Zelda.otf");
	myFonts[0] = CreateFontA(42, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("The Wild Breath of Zelda"));
	myFonts[1] = CreateFontA(90, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("The Wild Breath of Zelda"));
	myFonts[2] = CreateFontA(90, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));


	std::string text = "bilder/linkWorldmap.bmp";
	background.hdc = CreateCompatibleDC(hDC);
	background.map = (HBITMAP)LoadImage(NULL, (LPCTSTR)(text.c_str()), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	oldBitmap[0] = (HBITMAP)SelectObject(background.hdc, background.map);
	
	text = "bilder/linkSprites.bmp";
	player.hdc = CreateCompatibleDC(hDC);
	player.map = (HBITMAP)LoadImage(NULL, (LPCTSTR)(text.c_str()), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	oldBitmap[1] = (HBITMAP)SelectObject(player.hdc, player.map);

	// Buffer
	bufferHDC = CreateCompatibleDC(hDC);				// Skapa en hdc f�r bakgrundsbilden
	bitmapbuff = CreateCompatibleBitmap(hDC, innerWidth, innerHeight);
	SelectObject(bufferHDC, bitmapbuff);
	//Ger CPU-frekvensen som anv�nds med performanceCounter();
	CPUFreq = getFreq();

	// tmp
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	std::cout << "This works" << std::endl;
	createMap();
	// upper hill
	createBox(108, 0, 84, 51);
	//down hill
	createBox(0, 92, 192, 8);
	createBox(0, 63, 24, 37);
	
	
	return 1;
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
void releaseAll(HWND hWnd) {
	//Ta bort hdc till f�nstret och imageHDC
	ReleaseDC(hWnd, hDC);
	DeleteDC(hDC);
}
//---------------------------------------------------------------------
BOOL initInstance(HINSTANCE hInstance, int nCmdShow) {

	HWND hWnd = CreateWindowEx(
		0, ClsName, (LPCSTR)ClsName, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_BORDER,
		((GetSystemMetrics(SM_CXSCREEN) - app_Wid) >> 1),  //S�tter f�nstret i mitten i x-led;
		((GetSystemMetrics(SM_CYSCREEN) - app_Hei) >> 1),  //S�tter f�nstret i mitten i Y-led;
		app_Wid, app_Hei, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}
//---------------------------------------------------------------------
ATOM doRegister(HINSTANCE hi) {
	WNDCLASSEX wincl;

	wincl.hInstance = hi;
	wincl.lpszClassName = ClsName;
	wincl.lpfnWndProc = winProc;
	wincl.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wincl.cbSize = sizeof(WNDCLASSEX);
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	return RegisterClassEx(&wincl);
}
//---------------------------------------------------------------------
double getFreq() {
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	return double(li.QuadPart) / 1000.0;
}
//---------------------------------------------------------------------