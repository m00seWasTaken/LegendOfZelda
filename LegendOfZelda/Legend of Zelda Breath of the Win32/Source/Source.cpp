#define WIN32_LEAN_AND_MEAN	
#include <Windows.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
// Globla variabler ---------------------------------------------------------
LPCTSTR		ClsName = "Bgstuff";
HWND		hWnd;
double		CPUFreq = 0.0;
bool		running = true;
int			innerWidth, innerHeight;
// pic and window
int			app_Wid = 1920;
int			app_Hei = 1080;
int			bg_Wid = 1920;
int			bg_Hei = 1080;
int			x, y;
// player animation
int			xpic = 144, ypic = 0;
int			playerX = app_Wid / 2 - 72, playerY = 275;

struct BG {
	HDC hdc;
	HBITMAP map;
	int x;
	int y;
	int cX;					// ETT RUM:	y=176 px, x=256px
	int cy;
	int sizeX = 4096;
	int sizeY = 1344;
};
struct user {
	HDC hdc;
	HBITMAP map;
	int x;
	int y;
	int cX;					// en sprite: y=176 px, x=256px
	int cy;
	int sizeX = 4096;
	int sizeY = 1344;
};
// All mighty
BG background;
user player;
// fonter
HFONT		myFonts[2];
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

void		exitGame();				// avsluta spelet 

// Funktioner för windows ----------------------------------------------------
LRESULT		CALLBACK	winProc(HWND, UINT, WPARAM, LPARAM);
ATOM 		doRegister(HINSTANCE);
BOOL 		initInstance(HINSTANCE, int);
int			initalizeAll(HWND);
void		releaseAll(HWND);
void		update();				// Alla uppdateringar
void		render();				// Ritar ut bilden i f�nstret
bool		framerate(int);			// Uppdateringsfrekvensen
double		getFreq();
inline __int64 performanceCounter() noexcept {
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);
	return li.QuadPart;
};
//---------------------------------------------------------------------------
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
		if (framerate(20) == true) {
			render();
			update();	// om menyn är igång upptatera inga animationer
		}
	}
	return 0;
}
//-------------------------------------------------------------------------
bool framerate(int timeStamp) {
	static __int64 last = performanceCounter();
	if (((double)((performanceCounter() - last)) / CPUFreq) > timeStamp) {
		last = performanceCounter();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK winProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

	switch (Msg) {
	case WM_CREATE:
		initalizeAll(hWnd);
		break;
	case WM_LBUTTONDOWN:

		break;
	case WM_MOUSEMOVE:

		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		running = false;
		break;
	case WM_KEYDOWN:
		
		break;
	case WM_KEYUP:							

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
void exitGame() {				// stänger ner spelet 
	running = false;
	PostQuitMessage(0);
}

//---------------------------------------------------------------------------
void update() {
	static int counter = 0;
	counter++;
	
}

//---------------------------------------------------------------------
void render() {

	//TransparentBLT(hdc till, x utskrift på skrämen, y utskrift på skärmen, hur brett den ska rita ut, hur högt, hdc ifrån, vart från bilden börjar ta x, vart från bilden börja ta y, x antal pixlar att ta, y antalet pixalr att ta, färg att ta bort)

	TransparentBlt(bufferHDC, 0, 0, app_Wid, app_Hei, background.hdc, 1792, 1176, 256, 168, COLORREF(RGB(255, 0, 255)));
	TransparentBlt(bufferHDC, 0, 0, app_Wid, app_Hei, player.hdc, 0, 0, 25.25, 20.2, COLORREF(RGB(255, 0, 255)));

	// dubbelbuffring
	BitBlt(hDC, 0, 0, innerWidth, innerHeight, bufferHDC, 0, 0, SRCCOPY);
	
	/*
	static bool yes = true;
	int langd = bgs.size() - 1;
	if (menuActive == true) {	// om menyn är igång kör endast menyn
		printMenu();
		BitBlt(hDC, 0, 0, innerWidth, innerHeight, bufferHDC, 0, 0, SRCCOPY);
		return;
	}
	for (int n = langd; n >= 0; n--) {
		// background
		TransparentBlt(bufferHDC, bgs[n].x, 0, app_Wid, app_Hei, bgs[n].hDCbg, 0, 0, bg_Wid, bg_Hei, COLORREF(RGB(255, 0, 255)));

		if (bgs[n].x < 0) {		// rörelse till höger
			TransparentBlt(bufferHDC, app_Wid + bgs[n].x, 0, -bgs[n].x, app_Hei, bgs[n].hDCbg, 0, 0, -bgs[n].x * 2, bg_Hei, COLORREF(RGB(255, 0, 255)));
		}
		else if (bgs[n].x > 0) { // rörelse till vänster
			TransparentBlt(bufferHDC, bgs[n].x - app_Wid, 0, app_Wid, app_Hei, bgs[n].hDCbg, 0, 0, bg_Wid, bg_Hei, COLORREF(RGB(255, 0, 255)));
		}
		if (n == 1) {
			// player
			if (idleR == true) {
				TransparentBlt(bufferHDC, playerX, playerY, 144, 144, playerHDC, xpic, ypic, 144, 144, COLORREF(RGB(255, 0, 255)));
			}
			else if (idleL == true) {
				TransparentBlt(bufferHDC, playerX, playerY, 144, 144, playerHDC, xpic, ypic, 144, 144, COLORREF(RGB(255, 0, 255)));
			}
		}
	}
	// monster
	for (int n = 0; n < monstLangd; n++) {
		TransparentBlt(bufferHDC, monst[n].x, monst[n].y, monst[n].width, monst[n].height, monsterHDC, monst[n].cX, monst[n].cY, 24, 24, COLORREF(RGB(255, 0, 255)));
	}
	// explosions
	for (int n = 0; n < expLangd; n++) {
		TransparentBlt(bufferHDC, exps[n].x, exps[n].y, exps[n].width, exps[n].height, expHDC, exps[n].cX, exps[n].cY, 320, 240, COLORREF(RGB(255, 0, 255)));
	}

	// visar poängen för spelaren
	std::string output = "Score: " + std::to_string(points) + " ";

	TextOut(bufferHDC, 50, 20, output.c_str(), output.length());
	*/
	//dubbelbuff av allt
	
}
//---------------------------------------------------------------------------
int	initalizeAll(HWND hWnd) {
	srand(time(NULL));

	// H�mta f�nstrets riktiga bredd & h�jd
	RECT		windowRect;
	GetClientRect(hWnd, &windowRect);
	innerWidth = windowRect.right;
	innerHeight = windowRect.bottom;

	hDC = GetDC(hWnd);			// Koppla f�nstret till en DC

	// skapar fonter
	myFonts[0] = CreateFontA(28, 0, 100, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
	myFonts[1] = CreateFontA(60, 0, 100, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));

	std::string text = "bilder/linkWorldmap.bmp";
	background.hdc = CreateCompatibleDC(hDC);
	background.map = (HBITMAP)LoadImage(NULL, (LPCTSTR)(text.c_str()), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	oldBitmap[0] = (HBITMAP)SelectObject(background.hdc, background.map);
	
	std::string text = "bilder/linkSprites.bmp";
	background.hdc = CreateCompatibleDC(hDC);
	background.map = (HBITMAP)LoadImage(NULL, (LPCTSTR)(text.c_str()), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	oldBitmap[0] = (HBITMAP)SelectObject(background.hdc, background.map);


	/*
	playerHDC = CreateCompatibleDC(hDC);	// Skapa en hdc f�r spelaren
	text = "bilder/linkSprites.bmp";
	sprites = (HBITMAP)LoadImage(NULL, (LPCTSTR)(text.c_str()), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	oldBitmap[8] = (HBITMAP)SelectObject(playerHDC, sprites);
	*/
	// Buffer
	bufferHDC = CreateCompatibleDC(hDC);				// Skapa en hdc f�r bakgrundsbilden
	bitmapbuff = CreateCompatibleBitmap(hDC, innerWidth, innerHeight);
	SelectObject(bufferHDC, bitmapbuff);
	//Ger CPU-frekvensen som anv�nds med performanceCounter();
	CPUFreq = getFreq();

	return 1;
}
//---------------------------------------------------------------------------
void releaseAll(HWND hWnd) {
	//Ta bort hdc till f�nstret och imageHDC
	ReleaseDC(hWnd, hDC);
	DeleteDC(hDC);
}
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
double getFreq() {
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	return double(li.QuadPart) / 1000.0;
}
//---------------------------------------------------------------------------