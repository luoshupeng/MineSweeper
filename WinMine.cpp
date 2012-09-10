/*//////////////////////文件说明//////////////////////////
//														//
//文件名: WinMine.cpp				           			//
//功  能: WinMain主函数的定义							//
//作  者:												//
//														//
////////////////////////////////////////////////////////*/

#include "WinProc.h"

/*###########################################################
功能：主窗口函数，程序入口
参数：
返回值：
###########################################################*/

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND			hWnd;
	HACCEL			hAccel;
	MSG				msg ;
	WNDCLASS		wndclass ;
	TCHAR			szAppName[20],szAppCaption[20];
	
	LoadString(hInstance,IDS_APPMANE,szAppName,20);
	LoadString(hInstance,IDS_APPCAPTION,szAppCaption,20);
	wndclass.style         = CS_HREDRAW | CS_VREDRAW |CS_OWNDC;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (hInstance,MAKEINTRESOURCE(IDC_APPICON));
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"), 
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	RegisterClass (&wndclass);
	
	hWnd = CreateWindow (szAppName,					// window class name
						 szAppCaption,				// window caption
						 WS_SYSMENU | WS_MINIMIZEBOX |WS_OVERLAPPED,        // window style
						 CW_USEDEFAULT,              // initial x position
						 CW_USEDEFAULT,              // initial y position
						 CW_USEDEFAULT,              // initial x size
						 CW_USEDEFAULT,              // initial y size
						 NULL,                       // parent window handle
						 NULL,						 // window menu handle
						 hInstance,                  // program instance handle
						 NULL) ;                     // creation parameters
	hAccel=LoadAccelerators(hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR));
	ShowWindow (hWnd, iCmdShow) ;
	UpdateWindow (hWnd) ;	

	
	while (GetMessage (&msg, NULL, 0, 0))
	{	

		if (!TranslateAccelerator(hWnd,hAccel,&msg))
		{
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ;
		}
	}
	return msg.wParam ;
}