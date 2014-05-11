//WinMain.cpp

#define MAX 15
#define MAX2 15

#include<Windows.h>
#include"resource.h"
#include<stdlib.h>
#include<ctime>

LRESULT CALLBACK WindowProc(  __in  HWND hWnd,
							  __in  UINT uMsg,
							  __in  WPARAM wParam,
							  __in  LPARAM lParam);

void PaintChess(int player, int nx, int ny);
BOOL IsWin(int player, int nx, int ny);
void GameOver(int player);
void ChangePlayer(int& player);


HDC hDC, hDC1, hDC2, hDCBack;       //hDC for window; hDC1 for player1 to paint chess          and hDCBack for remove last chess
HPEN hPen, hPen1, hPen2, hPenBack, hPenBack2;         //hPenBack is black to redraw line, hPenBack2 is green to redraw ellipse
HBRUSH hBrush, hBrush1, hBrush2, hBrushBack;     //hBrush1 for player1
int cx, cy;                //the length and width of window
int nx, ny;                //the number of box
POINT pt;                  //the position of mouse
BOOL bState = false;       //the state of game
int player;                //player can be 1 and -1
int ChessBoard[MAX][MAX2] = {0};  //0 represent empty
PAINTSTRUCT ps;
int i, j, count = 0;         //just to be convient to count
HWND hWnd;
POINT LastPoint;                //to storage last box of chess
int LastPlayer = 0;
HINSTANCE hInst;


HDC hdcMem;
HBITMAP hBmp;



int APIENTRY WinMain( HINSTANCE hInstance,
				      HINSTANCE hPrevInstance,
					  LPSTR lpCmdLine,
					  int nCmdShow)
{

	srand( time(0) );
	LPCWSTR lpClassName = TEXT("freeAppName");
	LPCWSTR lpWindowName = TEXT("freeFiveChess");

	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDI_QUESTION);
	wc.hIcon = LoadIcon(NULL, IDI_SHIELD);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = lpClassName;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wc);

	hWnd = 	CreateWindow(lpClassName, lpWindowName,
						 WS_OVERLAPPEDWINDOW,
						 400, 150,
						 459, 479,
						 NULL,
						 NULL,
						 hInstance,
						 NULL);

	ShowWindow(hWnd, SW_SHOWNORMAL);

	UpdateWindow(hWnd);


	MessageBox(NULL, TEXT("Left button to play  while  Right button to  \"retract\""), TEXT("How to play"), MB_OK|MB_ICONASTERISK);

	MSG msg;
	while( GetMessage(&msg, NULL, 0 , 0) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return msg.wParam;
}



LRESULT CALLBACK WindowProc(  __in  HWND hWnd,
							  __in  UINT uMsg,
							  __in  WPARAM wParam,
							  __in  LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CREATE:
		{
			hInst = LPCREATESTRUCT(lParam)->hInstance;

			bState = true;
			player = 1;

			hDC1 = GetDC(hWnd);
			hDC2 = GetDC(hWnd);
			hDCBack = GetDC(hWnd);

			hPen1 = CreatePen(PS_SOLID, 1, RGB(255,255,255) );
			hPen2 = CreatePen(PS_SOLID, 1, RGB(0,0,0) );
			hPenBack = CreatePen(PS_SOLID, 1, RGB(0,0,0) );              //2 is more compatible with the system pen
			hPenBack2 = CreatePen(PS_SOLID, 1, RGB(0,255,0) );
			SelectObject(hDC1, hPen1);
			SelectObject(hDC2, hPen2);
			SelectObject(hDCBack, hPenBack);

			hBrush1 = CreateSolidBrush( RGB(255,255,255) );	
			hBrush2 = CreateSolidBrush( RGB(rand()%255,rand()%255,rand()%255) );
			hBrushBack = CreateSolidBrush( RGB(0,255,0) );
			SelectObject(hDC1, hBrush1);
			SelectObject(hDC2, hBrush2);
			SelectObject(hDCBack, hBrushBack);
			
		}break;

	case WM_SIZE:
		{
			cx = LOWORD(lParam);
			cy = HIWORD(lParam);
			InvalidateRect(hWnd,NULL,TRUE); 
		}break;

	case WM_PAINT:
		{
			hDC = BeginPaint(hWnd, &ps);


			hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));		
			hdcMem = CreateCompatibleDC(hDC);
			SelectObject(hdcMem, hBmp);

			StretchBlt(hDC, 0,0,600,600, hdcMem, 0,0,600,600, SRCCOPY);






			hBrush = CreateSolidBrush( RGB(0,255,0) );
			SelectObject(hDC, hBrush);

			//for(int x=0; x<MAX +1; x++)                                                      //to make chess in the corner
			//	for(int y=0; y<MAX2 +1; y++)
			//	{
			//		Rectangle(hDC, x*cx/MAX -cx/MAX/2, y*cy/MAX2 -cy/MAX2/2, (x+1)*cx/MAX -cx/MAX/2, (y+1)*cy/MAX2 -cy/MAX2/2);
			//	}

			for(int i=0;i<MAX;i++)
				for(int j=0;j<MAX2;j++)
				{
					ChessBoard[i][j] = 0;                                  //to reset the game
				}

			SelectObject(hDC, GetStockObject(BLACK_BRUSH) );
			DeleteObject(hBrush);
			EndPaint(hWnd, &ps);
		}break;



	case WM_LBUTTONDOWN:                                       //key code
		{
			if(MK_SHIFT & wParam )									//crack by pressing Shift when click
			{
				pt.x = LOWORD(lParam);
				pt.y = HIWORD(lParam);
				nx = pt.x / (cx/MAX);
				ny = pt.y / (cy/MAX2);

				PaintChess(player, nx, ny);	
				IsWin(player, nx, ny);
			
				if( IsWin(player, nx, ny) )
				{
					GameOver(player);
				}
				ChangePlayer(player);
			}

			else
			{
				pt.x = LOWORD(lParam);
				pt.y = HIWORD(lParam);
				nx = pt.x / (cx/MAX);
				ny = pt.y / (cy/MAX2);

				if( ChessBoard[nx][ny]==0 )
				{
					PaintChess(player, nx, ny);	
					if( IsWin(player, nx, ny) )
					{
						GameOver(player);
					}
					else ChangePlayer(player);
				}
			}

		}break;


	case WM_RBUTTONDOWN:                                         //crack
		{
			ChangePlayer(player);
			PaintChess(0, LastPoint.x, LastPoint.y);

		}break;

	case WM_DESTROY:
		{
			SelectObject(hDC1, GetStockObject(BLACK_PEN));
			SelectObject(hDC2, GetStockObject(BLACK_BRUSH));
			SelectObject(hDC1, GetStockObject(BLACK_PEN));
			SelectObject(hDC2, GetStockObject(BLACK_BRUSH));
			SelectObject(hDCBack, GetStockObject(BLACK_PEN));
			SelectObject(hDCBack, GetStockObject(BLACK_BRUSH));

			DeleteObject(hPen1);
			DeleteObject(hPen2);
			DeleteObject(hPenBack);
			DeleteObject(hPenBack2);
			DeleteObject(hBrush1);
			DeleteObject(hBrush2);
			DeleteObject(hBrushBack);
			ReleaseDC(hWnd, hDC1);
			ReleaseDC(hWnd, hDC2);
			ReleaseDC(hWnd, hDCBack);

			PostQuitMessage( 0 );
		}break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam,lParam);
	}
}

void PaintChess(int player, int nx, int ny)
{
	LastPoint.x = nx;
	LastPoint.y = ny;
	LastPlayer = player;

	switch(player)
	{
	case 1:
		{
			Ellipse(hDC1, nx*cx/MAX, ny*cy/MAX2, (nx+1)*cx/MAX, (ny+1)*cy/MAX2);
			ChessBoard[nx][ny] = 1;
			
		}break;


	case -1:
		{
			hDC2 = GetDC(hWnd);
			hBrush2 = CreateSolidBrush( RGB(rand()%255,rand()%255,rand()%255) );
			SelectObject(hDC2, hBrush2);

			Ellipse(hDC2, nx*cx/MAX, ny*cy/MAX2, (nx+1)*cx/MAX, (ny+1)*cy/MAX2);
			ChessBoard[nx][ny] = -1;

			SelectObject(hDC2, GetStockObject(BLACK_BRUSH));
			DeleteObject(hBrush2);
		}break;

	case 0:
		{
			ChessBoard[nx][ny] = 0;

			SelectObject(hDCBack, hPenBack2);
			Ellipse(hDCBack, nx*cx/MAX, ny*cy/MAX2, (nx+1)*cx/MAX, (ny+1)*cy/MAX2);

			SelectObject(hDCBack, hPenBack);
			//(hDCBack, nx*cx/MAX +cx/MAX/2, ny*cy/MAX2 +cy/MAX2/2)  which is the center of ellipse
			MoveToEx(hDCBack, nx*cx/MAX, ny*cy/MAX2 +cy/MAX2/2, NULL);                    //redraw the horizon    
			LineTo(hDCBack, nx*cx/MAX +2*cx/MAX/2, ny*cy/MAX2 +cy/MAX2/2);
			MoveToEx(hDCBack, nx*cx/MAX +cx/MAX/2, ny*cy/MAX2, NULL);                //redraw the vertical
			LineTo(hDCBack, nx*cx/MAX +cx/MAX/2, ny*cy/MAX2 +2*cy/MAX2/2);

		}
	}
}


BOOL IsWin(int player, int nx, int ny)
{
	int i, j, count = 0;
	int tempx = nx, tempy = ny;


	for(i=0;i<5;i++)                     //turn horizental to win
	{
		tempy = ny;
		tempx = nx - i;

		for(j=0; j<5 ;j++)
		{		
			if( player==ChessBoard[tempx][tempy] )
			{
				count++;				
			}
			else 
			{
				break;
			}
			tempx++;
		}

		if(5==count)
		{
			return true;
		}
		else 
		{
			count = 0;
		}
	}



	for(i=0;i<5;i++)                     //turn vertical to win
	{
		tempx = nx;
		tempy = ny - i;

		for(j=0; j<5 ;j++)
		{		
			if( player==ChessBoard[tempx][tempy] )
			{
				count++;				
			}
			else 
			{
				break;
			}
			tempy++;
		}

		if(5==count)
		{
			return true;
		}
		else 
		{
			count = 0;
		}
	}



	for(i=0;i<5;i++)                     //turn right and up to win
	{
		tempx = nx - i;
		tempy = ny + i;

		for(j=0; j<5 ;j++)
		{		
			if( player==ChessBoard[tempx][tempy] )
			{
				count++;				
			}
			else 
			{
				break;
			}
			tempx++;
			tempy--;
		}

		if(5==count)
		{
			return true;
		}
		else 
		{
			count = 0;
		}
	}



	for(i=0;i<5;i++)                     //turn right and down to win
	{
		tempx = nx - i;
		tempy = ny - i;

		for(j=0; j<5 ;j++)
		{		
			if( player==ChessBoard[tempx][tempy] )
			{
				count++;				
			}
			else 
			{
				break;
			}
			tempx++;
			tempy++;
		}

		if(5==count)
		{
			return true;
		}
		else 
		{
			count = 0;
		}
	}

	return false;
}

void GameOver(int player)
{
	if(1==player)
	{
		MessageBox(NULL, TEXT("Player1 (White_Chess) surely  !!!WIN!!!"), TEXT("The result is..."), MB_OK|MB_ICONASTERISK);
	}
	if(-1==player)
	{
		MessageBox(NULL, TEXT("Player2 (Colorful_Chess) surely  !!!WIN!!!"), TEXT("The result is..."), MB_OK|MB_ICONASTERISK);
	}

	int res	= MessageBox(NULL, TEXT("Do you want to play again?"), TEXT("Retry or Not"), MB_YESNOCANCEL|MB_ICONQUESTION);
	if(IDYES==res)
	{
		MessageBox(NULL, TEXT("Press Shift while clicking can  \"EAT\" others chess"), TEXT("Cracker comes ..."), MB_OK|MB_ICONWARNING);
		InvalidateRect(hWnd, NULL, TRUE);
	}
	else if(IDNO==res)
	{
		PostQuitMessage( 0 );
	}
}


void ChangePlayer(int& player)
{
	if(1==player)
	{
		player = -1;
	}
	else
	{
		player = 1;
	}
}