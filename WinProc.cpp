/*//////////////////////文件说明//////////////////////////
//														//
//文件名: WinProc.cpp					       			//
//功  能: 功能函数的定义和实现							//
//作  者:												//
//														//
////////////////////////////////////////////////////////*/

#include "WinProc.h"

/*###########################################################
功能：主窗口的消息处理回调函数
参数：
返回值：
###########################################################*/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC							hDC,hDCMem;//,hDCScreen;		
	HINSTANCE					hLib;						//shell32.dll 实例句柄
	static	FARPROC				fpAbout;					//ShellAbout 函数入口
	static	HICON				hIcon;						//程序图标句柄
	static	HINSTANCE			hInstance;					//程序实例句柄
	static	HMENU				hMenu;						//程序主菜单句柄
	PAINTSTRUCT					ps;
	HBRUSH						hBrush;
	HPEN						hPen;
	HBITMAP						hBmp;
	int							ix=0,iy=0;
	RECT						rcWnd,rcRefresh;			//窗口区域和刷新区域
	static tagMouseState		mousePos;					//鼠标状态结构体
	HRSRC						hrWare;						//wave资源句柄
	HGLOBAL						hGlobal;					
	LPCTSTR						keyWinMine=TEXT("Software\\Microsoft\\winmine\\");	//注册表主键
	HKEY						hKey,hKeySub;				//注册表键句柄
	DWORD						dwTemp;						//临时变量
	LPCTSTR						lpsTime[3]={TEXT("Time1"),TEXT("Time2"),TEXT("Time3")};
	LPCTSTR						lpsName[3]={TEXT("Name1"),TEXT("Name2"),TEXT("Name3")};
	TCHAR						lpAnonymity[255];			//临时变量
	DWORD 						dwSize1=sizeof(DWORD),dwType1=REG_DWORD,dwType2=REG_SZ,dwSize2=sizeof(lpAnonymity);
	//##########################################
	switch (message)
	{
	//################################
	case WM_CREATE:
		hDC=GetDC(hWnd);
		hInstance=((LPCREATESTRUCT)lParam)->hInstance;
		hMenu=((LPCREATESTRUCT)lParam)->hMenu;
		flagState=0x000;
		/*/////////////////////////////////////////////////////////
		//注册表读取已保存的信息，如果相关键值不存在则创建
		*//////////////////////////////////////////////////////////
		if (RegOpenKeyEx(HKEY_CURRENT_USER,keyWinMine,0,KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS)
		{
			RegCreateKey(HKEY_CURRENT_USER,keyWinMine,&hKey);			
		}			
		if (RegQueryValueEx(hKey,TEXT("Height"),NULL,&dwType1,(LPBYTE)&mineLineNum,&dwSize1)!=ERROR_SUCCESS)
		{
			mineLineNum=BEGINER_LN_NUM;
			RegSetValueEx(hKey,TEXT("Height"),NULL,REG_DWORD,(LPBYTE)&mineLineNum,dwSize1);
		}
		if (RegQueryValueEx(hKey,TEXT("Width"),NULL,&dwType1,(LPBYTE)&mineRowNum,&dwSize1)!=ERROR_SUCCESS)
		{
			mineRowNum=BEGINER_COL_NUM;
			RegSetValueEx(hKey,TEXT("Width"),NULL,REG_DWORD,(LPBYTE)&mineRowNum,dwSize1);
		}			
		if (RegQueryValueEx(hKey,TEXT("Mines"),NULL,&dwType1,(LPBYTE)&mineBomb,&dwSize1)!=ERROR_SUCCESS)
		{
			mineBomb=BEGINER_BOMB_NUM;
			RegSetValueEx(hKey,TEXT("Mines"),NULL,REG_DWORD,(LPBYTE)&mineBomb,dwSize1);
		}			
		if (RegQueryValueEx(hKey,TEXT("Sound"),NULL,&dwType1,(LPBYTE)&dwTemp,&dwSize1)!=ERROR_SUCCESS)
		{
			dwTemp=0;
			RegSetValueEx(hKey,TEXT("Sound"),NULL,REG_DWORD,(LPBYTE)&dwTemp,dwSize1);
		}
		flagState=(flagState&0xffef)+dwTemp*FLAG_SOUND;
		if (RegQueryValueEx(hKey,TEXT("Difficulty"),NULL,&dwType1,(LPBYTE)&dwTemp,&dwSize1)!=ERROR_SUCCESS)
		{
			dwTemp=0;
			RegSetValueEx(hKey,TEXT("Difficulty"),NULL,REG_DWORD,(LPBYTE)&dwTemp,dwSize1);
		}		
		flagState=(flagState&0xf0ff)+dwTemp*0x0100;	
		if (RegQueryValueEx(hKey,TEXT("Mark"),NULL,&dwType1,(LPBYTE)&dwTemp,&dwSize1)!=ERROR_SUCCESS)
		{
			dwTemp=1;
			RegSetValueEx(hKey,TEXT("Mark"),NULL,REG_DWORD,(LPBYTE)&dwTemp,dwSize1);
		}
		flagState=(flagState&0xfff7)+dwTemp*FLAG_MARK;
		if (RegQueryValueEx(hKey,TEXT("Color"),NULL,&dwType1,(LPBYTE)&dwTemp,&dwSize1)!=ERROR_SUCCESS)
		{
			dwTemp=1;
			RegSetValueEx(hKey,TEXT("Color"),NULL,REG_DWORD,(LPBYTE)&dwTemp,dwSize1);
		}
		flagState=(flagState&0xfffb)+dwTemp*FLAG_COLOR;
		if (RegQueryValueEx(hKey,TEXT("AlreadyPlayed"),NULL,&dwType1,(LPBYTE)&dwTemp,&dwSize1)!=ERROR_SUCCESS)
		{
			dwTemp=1;
			RegSetValueEx(hKey,TEXT("AlreadyPlayed"),NULL,REG_DWORD,(LPBYTE)&dwTemp,dwSize1);
		}
		flagState=(flagState&0xfffe)+1*FLAG_ALREADPLAYED;
		dwTemp=999;	
		LoadString(hInstance,IDS_NORMAL_NAME,lpAnonymity,sizeof(lpAnonymity));	
		for (ix=0;ix<3;ix++)
		{				
			if (RegOpenKeyEx(hKey,lpsTime[ix],NULL,KEY_WRITE,&hKeySub)!=ERROR_SUCCESS)
			{
				RegSetValueEx(hKey,lpsTime[ix],NULL,REG_DWORD,(LPBYTE)&dwTemp,dwSize1);
				RegCloseKey(hKeySub);
			}
			if (RegOpenKeyEx(hKey,lpsName[ix],NULL,KEY_WRITE,&hKeySub)!=ERROR_SUCCESS)
			{		
				RegSetValueEx(hKey,lpsName[ix],NULL,REG_SZ,(LPBYTE)&lpAnonymity,dwSize2);
				RegCloseKey(hKeySub);
			}
		}	
		/*/////////////////////////////////////////////////////////
		//初始化窗口信息
		*//////////////////////////////////////////////////////////
		flagState=(flagState&0x0fff)+0x4000;
		IinitializeMapInfo(hWnd,(flagState&0x0f00)/0x0100,mineRowNum,mineLineNum,mineBomb,flagState & 0x0001,FALSE,FACE_TYPE_SMILE_POP);	
		CheckMenuItem(hMenu,IDM_MARK,((flagState&0x0008)==FLAG_MARK) ? MF_CHECKED:MF_UNCHECKED);
		CheckMenuItem(hMenu,IDM_COLOR,((flagState&0x0004)==FLAG_COLOR) ? MF_CHECKED:MF_UNCHECKED);
		CheckMenuItem(hMenu,IDM_SOUND,((flagState&0x0010)==FLAG_SOUND) ? MF_CHECKED:MF_UNCHECKED);			
		OnCheckLevelMenu(hMenu,(flagState&0x0f00)/0x0100);		
		//计算窗口大小，初始化地图数据
		rcWnd=getLevelWindowSize(hWnd);
		if (RegQueryValueEx(hKey,TEXT("Xpos"),NULL,&dwType1,(LPBYTE)&dwTemp,&dwSize1)==ERROR_SUCCESS)
		{
			rcWnd.left=dwTemp;
		}
		
		if (RegQueryValueEx(hKey,TEXT("Ypos"),NULL,&dwType1,(LPBYTE)&dwTemp,&dwSize1)==ERROR_SUCCESS)
		{
			rcWnd.top=dwTemp;	
		}
			
		CreateRandomMap(hWnd);
		MoveWindow(hWnd,rcWnd.left,rcWnd.top,rcWnd.right,rcWnd.bottom,TRUE);
		RegCloseKey(hKey);
		ReleaseDC(hWnd,hDC);
		/*/////////////////////////////////////////////////////////
		//调入Windows自带的 shell32.dll,为调用"关于"(ShellAbout)做准备
		*//////////////////////////////////////////////////////////
		hLib=LoadLibrary("shell32.dll");
		if (hLib)
		{
			fpAbout=GetProcAddress(hLib,"ShellAboutW");
		}
		hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDC_APPICON)) ;
		return 0;
	case WM_PAINT:
		hDC = BeginPaint (hWnd, &ps);
		hDCMem= CreateCompatibleDC(hDC);
		GetClientRect(hWnd,&rcWnd);
		hPen=(HPEN)GetStockObject(NULL_PEN);
		SelectObject(hDC,hPen);
		hBrush=CreateSolidBrush(RGB(192,192,192));
		SelectObject(hDC,hBrush);
		Rectangle(hDC,3,3,rcWnd.right+1,rcWnd.bottom+1);//绘制底层背景		
		DeleteObject(hBrush);
		hBrush=CreateSolidBrush(RGB(8+(flagState&0x0004)*30,8+(flagState&0x0004)*30,8+(flagState&0x0004)*30));
		SelectObject(hDC,hBrush);
		//绘制表情背景
		Rectangle(	hDC,
					MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2-1,
					(MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2-1,
					MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2+FACE_BITMAP_WIDTH+2,
					(MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2+FACE_BITMAP_HEIGHT+2);

		DeleteObject(hBrush);
		DeleteObject(hPen);

		//表情贴图
		hBmp=LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_SMILE_BLACK+(flagState&0x0004)/0x0004));
		SelectObject(hDCMem,hBmp);
		BitBlt(hDC,MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2,(MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2,FACE_BITMAP_WIDTH,FACE_BITMAP_HEIGHT,hDCMem,0, ((flagState & 0xf000)/0x1000) *FACE_BITMAP_HEIGHT,SRCCOPY);
		DeleteObject(hBmp);

		//计雷器贴图
		hBmp=LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_NUM_BLACK+(flagState&0x0004)/0x0004));
		SelectObject(hDCMem,hBmp);
		BitBlt(hDC,COUNT_LEFTPOS,COUNT_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,hDCMem,0,NUM_BITMAP_HEIGHT*(bombCount<0?0:11-(int)floor(bombCount/100)),SRCCOPY);
		BitBlt(hDC,COUNT_LEFTPOS+NUM_BITMAP_WIDTH,COUNT_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,hDCMem,0,NUM_BITMAP_HEIGHT*(11-(int)floor((abs(bombCount)-(int)floor(abs(bombCount)/100)*100)/10)),SRCCOPY);
		BitBlt(hDC,COUNT_LEFTPOS+NUM_BITMAP_WIDTH*2,COUNT_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,hDCMem,0,NUM_BITMAP_HEIGHT*(11-(abs(bombCount)-(int)floor(abs(bombCount)/10)*10)),SRCCOPY);
		/*///////////////////////////////////////////////////////////////////////////
		//计时器贴图
		//
		*////////////////////////////////////////////////////////////////////////////
		BitBlt(hDC,rcWnd.right-TIME_RIGHTSTND-TIME_WIDTH,TIME_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,hDCMem,0,NUM_BITMAP_HEIGHT*(11-(timeCount<1000?(int)floor(timeCount/100):9)),SRCCOPY);
		BitBlt(hDC,rcWnd.right-TIME_RIGHTSTND-TIME_WIDTH+NUM_BITMAP_WIDTH,TIME_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,hDCMem,0,NUM_BITMAP_HEIGHT*(11-(timeCount<1000?(int)floor((timeCount-(int)floor(timeCount/100)*100)/10):9)),SRCCOPY);
		BitBlt(hDC,rcWnd.right-TIME_RIGHTSTND-TIME_WIDTH+NUM_BITMAP_WIDTH*2,TIME_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,hDCMem,0,NUM_BITMAP_HEIGHT*(11-(timeCount<1000?(timeCount-(int)floor(timeCount/10)*10):9)),SRCCOPY);
		DeleteObject(hBmp);	
		//立体边框描绘
		rcRefresh=CreateRect(MAIN_LEFTSTND-LINE_LIGHTEN_THICK,MAIN_TOPSTND-LINE_LIGHTEN_THICK,rcWnd.right-MAIN_RIGHTSTND+LINE_LIGHTEN_THICK,rcWnd.bottom-MAIN_BOTTOMSTND+LINE_LIGHTEN_THICK);
		DrawSolidFrame(hDC,rcRefresh,LINE_LIGHTEN_THICK);

		rcRefresh=CreateRect(FUN_LEFTPOS-LINE_LIGHTEN_THICK,FUN_TOPPOS-LINE_LIGHTEN_THICK,rcWnd.right-FUN_RIGHTSTND+LINE_LIGHTEN_THICK,MAIN_TOPSTND-FUN_BOTTOMSTND+LINE_LIGHTEN_THICK);
		DrawSolidFrame(hDC,rcRefresh,LINE_LIGHTEN_NORMAL);
		
		rcRefresh=CreateRect(COUNT_LEFTPOS-LINE_LIGHTEN_THIN,COUNT_TOPPOS-LINE_LIGHTEN_THIN,COUNT_RIGHTPOS+LINE_LIGHTEN_THIN,COUNT_BOTTOMPOS+LINE_LIGHTEN_THIN);
		DrawSolidFrame(hDC,rcRefresh,LINE_LIGHTEN_THIN);

		rcRefresh=CreateRect(rcWnd.right-TIME_RIGHTSTND-TIME_WIDTH-LINE_LIGHTEN_THIN,TIME_TOPPOS-LINE_LIGHTEN_THIN,rcWnd.right-TIME_RIGHTSTND+LINE_LIGHTEN_THIN,TIME_TOPPOS+TIME_HEIGHT+LINE_LIGHTEN_THIN);
		DrawSolidFrame(hDC,rcRefresh,LINE_LIGHTEN_THIN);

		//雷区贴图
		hBmp=LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_MINE_BLACK+(flagState&0x0004)/0x0004));
		SelectObject(hDCMem,hBmp);
		for(ix=0;ix<mineRowNum;ix++)
		{
			for(iy=0;iy<mineLineNum;iy++)
			{
				BitBlt(hDC,MAIN_LEFTSTND+MINE_BITMAP_WIDTH*ix,MAIN_TOPSTND+MINE_BITMAP_HEIGHT*iy,MINE_BITMAP_WIDTH,MINE_BITMAP_HEIGHT,hDCMem,0,MINE_BITMAP_HEIGHT* (btLandmineMapinfo[ix][iy]%0x10),SRCCOPY);
			}
		}		
		DeleteObject(hBmp);
		ReleaseDC(hWnd,hDCMem);
		DeleteObject(hDCMem);
		EndPaint (hWnd, &ps) ;
		return 0 ;
	case WM_COMMAND:
		switch(LOWORD(wParam)) {
			case IDM_BEGIN:				//开始
				//初始窗口信息
				IinitializeMapInfo(hWnd,(flagState & 0x0f00)/0x0100,mineRowNum,mineLineNum,mineBomb,TRUE,FALSE,FACE_TYPE_SMILE_POP); //??
				//生成随机地图
				CreateRandomMap(hWnd);
				//刷新窗口
				InvalidateRect(hWnd,NULL,TRUE);
				break;
			case IDM_BEGINNER:			//初级
				//初始窗口信息
				IinitializeMapInfo(hWnd,0x0000,BEGINER_COL_NUM,BEGINER_LN_NUM,BEGINER_BOMB_NUM,TRUE,FALSE,FACE_TYPE_SMILE_POP); //??
				//初始窗口信息得到窗口大小
				rcWnd=getLevelWindowSize(hWnd);
				//生成随机地图
				CreateRandomMap(hWnd);
				//改变窗口大小
				MoveWindow(hWnd,rcWnd.left,rcWnd.top,rcWnd.right,rcWnd.bottom,TRUE);
				//设置等级菜单
				OnCheckLevelMenu(hMenu,LEVEL_BEGINNER);
				break;
			case IDM_INTERMEDIATE:		//中级
				IinitializeMapInfo(hWnd,0x0001,INTER_COL_NUM,INTER_LN_NUM,INTER_BOMB_NUM,TRUE,FALSE,FACE_TYPE_SMILE_POP); //??
				rcWnd=getLevelWindowSize(hWnd);
				CreateRandomMap(hWnd);
				MoveWindow(hWnd,rcWnd.left,rcWnd.top,rcWnd.right,rcWnd.bottom,TRUE);
				OnCheckLevelMenu(hMenu,LEVEL_INTER);			
				break;
			case IDM_ADVANCED:			//高级
				IinitializeMapInfo(hWnd,0x0002,ADVANCED_COL_NUM,ADVANCED_LN_NUM,ADVANCED_BOMB_NUM,TRUE,FALSE,FACE_TYPE_SMILE_POP); //??
				rcWnd=getLevelWindowSize(hWnd);
				CreateRandomMap(hWnd);
				MoveWindow(hWnd,rcWnd.left,rcWnd.top,rcWnd.right,rcWnd.bottom,TRUE);
				OnCheckLevelMenu(hMenu,LEVEL_ADVANCED);
				break;
			case IDM_CUSTOMDEFINE:		//自定义等级
				//显示自定义窗口
				if (DialogBox(hInstance,MAKEINTRESOURCE(IDD_CUSTOMDLG),hWnd,WinDlgProc))
				{
					//如果返回是 TRUE,即"确定"
					IinitializeMapInfo(hWnd,0x0003,mineRowNum,mineLineNum,mineBomb,TRUE,FALSE,FACE_TYPE_SMILE_POP); //??
					rcWnd=getLevelWindowSize(hWnd);
					CreateRandomMap(hWnd);
					MoveWindow(hWnd,rcWnd.left,rcWnd.top,rcWnd.right,rcWnd.bottom,TRUE);
					OnCheckLevelMenu(hMenu,LEVEL_CUSTOM);
				}
				break;
			case IDM_MARK:				//设置标记 flagState & FLAG_MARK == FLAG_MARK 为标记 
				flagState=(((flagState&0x0008)==FLAG_MARK)?0:0x0008)+(flagState&0xfff7);
				CheckMenuItem(hMenu,IDM_MARK,((flagState&0x0008)==FLAG_MARK) ? MF_CHECKED:MF_UNCHECKED);
				break;
			case IDM_COLOR:				//设置颜色	flagState & FLAG_COLOR == FLAG_COLOR 为彩色
				flagState=(((flagState&0x0004)==FLAG_COLOR)?0:0x0004)+(flagState&0xfffb);
				CheckMenuItem(hMenu,IDM_COLOR,((flagState&0x0004)==FLAG_COLOR) ? MF_CHECKED:MF_UNCHECKED);
				InvalidateRect(hWnd,NULL,TRUE);
				break;
			case IDM_SOUND:				//设置声音	flagState & FLAG_SOUND == FLAG_SOUND 为发声
				flagState=(((flagState&0x0010)==FLAG_SOUND)?0:0x0010)+(flagState&0xffef);
				CheckMenuItem(hMenu,IDM_SOUND,((flagState&0x0010)==FLAG_SOUND) ? MF_CHECKED:MF_UNCHECKED);
				break;
			case IDM_HEROLIST:			//显示排行榜
				DialogBox(hInstance,MAKEINTRESOURCE(IDD_HEROLISTDLG),hWnd,WinDlgProc);
				break;
			case IDM_EXIT:				//退出
				SendMessage(hWnd,WM_DESTROY,wParam,lParam);
				break;
			case IDM_ABOUT:				//关于
				//load字符串资源
				LoadString(hInstance,IDS_AUTHER,lpAnonymity,sizeof(lpAnonymity));
				//调用shell32.dll的ShellAbout函数
				ShellAbout(hWnd,TEXT("扫雷 ..."),lpAnonymity,hIcon);
				break;
			case IDM_CONTENTS:			//帮助：目录
				//打开系统的帮助，调用htmlhelp api函数
				LoadString(hInstance,IDS_HELPFILEPATH_WIN,lpAnonymity,sizeof(lpAnonymity));
				if (HtmlHelp(NULL,lpAnonymity,HH_DISPLAY_TOC,NULL)==0)
				{	//系统帮助不存在，在当前目录寻找
					GetCurrentDirectory(sizeof(lpAnonymity),lpAnonymity);
					strcat(lpAnonymity,TEXT("\\help\\winmine.chm"));
					if (HtmlHelp(NULL,lpAnonymity,HH_DISPLAY_TOC,NULL)==0)
						//仍然无法打开，显示错误消息
						MessageBox(hWnd,TEXT("产生一个错误:\r\n应用程序无法找到帮助文件\"WinMine.chm\"\t\r\n请确认文件存在，或者重新安装程序。"),TEXT("错误信息"),MB_ICONERROR);
				}
				break;
			case IDM_SEARCHHELP:		//帮助：索引
				LoadString(hInstance,IDS_HELPFILEPATH_WIN,lpAnonymity,sizeof(lpAnonymity));
				if (HtmlHelp(NULL, lpAnonymity, HH_DISPLAY_INDEX,0)==0)
				{
					GetCurrentDirectory(sizeof(lpAnonymity),lpAnonymity);
					strcat(lpAnonymity,TEXT("\\help\\winmine.chm"));
					if (HtmlHelp(NULL,lpAnonymity,HH_DISPLAY_INDEX,NULL)==0)
						MessageBox(hWnd,TEXT("产生一个错误:\r\n应用程序无法找到帮助文件\"WinMine.chm\"\t\r\n请确认文件存在，或者重新安装程序。"),TEXT("错误信息"),MB_ICONERROR);
				}				
				break;
			case IDM_USEHELP:			//帮助: 使用帮助
				LoadString(hInstance,IDS_HELPFILEPATH_HELP,lpAnonymity,sizeof(lpAnonymity));
				if (HtmlHelp(NULL, lpAnonymity, HH_DISPLAY_TOC,0)==0)
				{
					GetCurrentDirectory(sizeof(lpAnonymity),lpAnonymity);
					strcat(lpAnonymity,TEXT("\\help\\nthelp.chm"));
					if (HtmlHelp(NULL,lpAnonymity,HH_DISPLAY_INDEX,NULL)==0)
						MessageBox(hWnd,TEXT("产生一个错误:\r\n应用程序无法找到帮助文件\"NThelp.chm\"\t\r\n请确认文件存在，或者重新安装程序。"),TEXT("错误信息"),MB_ICONERROR);
				}	
				break;
		}
	
		return 0;
	case WM_LBUTTONDOWN:				//左键按下
		SetCapture(hWnd);				//捕获鼠标
		if (flagState&0x0001)
		{
			RefreshSmileFaceBmp(hWnd,FACE_TYPE_EXCLAIM);
		}

		//得到鼠标位置信息
		mousePos=getMousePosition(hWnd,hInstance,LOWORD(lParam),HIWORD(lParam),mousePos.mIDbutton);	
		if (wParam&MK_RBUTTON)			//同时右键已经按下
		{
			mousePos.mIDbutton=DEF_DBBUTTONDOWN;	//处理双键按下消息
			OnMouseClick(hWnd,hInstance,mousePos,DEF_DBBUTTONDOWN);
		} 
		else							//右键没有按下
		{	
			mousePos.mIDbutton=DEF_LBUTTONDOWN;		//处理左键按下消息
			OnMouseClick(hWnd,hInstance,mousePos,DEF_LBUTTONDOWN);
		}
		return 0;
	case WM_LBUTTONUP:					//左键弹起
		ReleaseCapture();				//释放鼠标捕获
		mousePos=getMousePosition(hWnd,hInstance,LOWORD(lParam),HIWORD(lParam),mousePos.mIDbutton);
		//同时右键按下或者鼠标状态信息仍显示当前处于双键按下
		if (wParam&MK_RBUTTON||mousePos.mIDbutton==DEF_DBBUTTONDOWN)				
		{
			mousePos.mIDbutton=DEF_DBBUTTONUP;	//处理双键按下消息，鼠标状态设为左键弹起
			OnMouseClick(hWnd,hInstance,mousePos,DEF_DBBUTTONUP);
			if (flagState&0x0001)
			{
				RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_POP);
			}		

		} 
		else	//同时右键没有按下并且者鼠标状态信息仍显示当前不处于双键按下状态
		{	
			if (mousePos.mIDbutton==DEF_DBBUTTONUP) //如果鼠标左键弹起状态
			{
				mousePos.mIDbutton=DEF_LBUTTONUP;	//设置鼠标左键弹起状态
				return 0;
			}
			OnMouseClick(hWnd,hInstance,mousePos,DEF_LBUTTONUP);	//处理鼠标左键弹起消息
		}
		return 0;
	case WM_MOUSEMOVE:	//鼠标移动
		mousePos=getMousePosition(hWnd,hInstance,LOWORD(lParam),HIWORD(lParam),mousePos.mIDbutton);
		if (wParam&MK_LBUTTON)	//同时左键已经按下
		{			
			OnMouseClick(hWnd,hInstance,mousePos,DEF_MOUSEMOVING); //处理鼠标移动消息
		} 	
		return 0;
	case WM_RBUTTONDOWN:		//鼠标右键按下
		mousePos=getMousePosition(hWnd,hInstance,LOWORD(lParam),HIWORD(lParam),mousePos.mIDbutton);
		if (wParam&MK_LBUTTON)		//同时左键已经按下
		{
			mousePos.mIDbutton=DEF_DBBUTTONDOWN;	//处理双键按下消息，设置鼠标为双键按下状态
			OnMouseClick(hWnd,hInstance,mousePos,DEF_DBBUTTONDOWN);
		} 
		else						//左键没有按下
		{
			OnMouseClick(hWnd,hInstance,mousePos,DEF_RBUTTONDOWN);	//处理右键弹起消息
		}
		return 0;
	case WM_RBUTTONUP:			//鼠标右键弹起
		mousePos=getMousePosition(hWnd,hInstance,LOWORD(lParam),HIWORD(lParam),mousePos.mIDbutton);
		if (wParam&MK_LBUTTON)	//同时鼠标左键已经按下
		{
			mousePos.mIDbutton=DEF_DBBUTTONUP;		//处理鼠标双键弹起消息，设置鼠标状态为双键弹起
			OnMouseClick(hWnd,hInstance,mousePos,DEF_DBBUTTONUP);
			if (flagState&0x0001)
			{
				RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_POP);
			}
		} 
		return 0;
	case WM_IME_NOTIFY:			//不处理该消息		
		return 0;
	case WM_TIMER:				//计时器消息
		timeCount++;			//时间增加
		//得到计时器大小，刷新该区域
		GetClientRect(hWnd,&rcWnd);
		rcRefresh=CreateRect(rcWnd.right-TIME_RIGHTSTND-TIME_WIDTH-LINE_LIGHTEN_THIN,TIME_TOPPOS-LINE_LIGHTEN_THIN,rcWnd.right-TIME_RIGHTSTND+LINE_LIGHTEN_THIN,TIME_TOPPOS+TIME_HEIGHT+LINE_LIGHTEN_THIN);
		InvalidateRect(hWnd,&rcRefresh,FALSE);
		if (flagState & 0x0010)		//如果播放声音
		{
			hrWare=FindResource(hInstance,MAKEINTRESOURCE(IDW_WAVE_TIME),TEXT("WAVE"));
			hGlobal=LoadResource(hInstance,hrWare);
			PlaySoundW((LPCWSTR)LockResource(hGlobal),NULL,SND_MEMORY|SND_ASYNC);
			DeleteObject(hrWare);
			DeleteObject(hGlobal);
		}
		return 0;
	case WM_DESTROY:				//程序关闭时
		//写注册表，没有的键值同时创建
		if (RegOpenKeyEx(HKEY_CURRENT_USER,keyWinMine,0,KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS)
		{
			RegCreateKey(HKEY_CURRENT_USER,keyWinMine,&hKey);
		}
		RegSetValueEx(hKey,TEXT("Height"),NULL,REG_DWORD,(LPBYTE)&mineLineNum,dwSize1);
		RegSetValueEx(hKey,TEXT("Width"),NULL,REG_DWORD,(LPBYTE)&mineRowNum,dwSize1);
		RegSetValueEx(hKey,TEXT("Mines"),NULL,REG_DWORD,(LPBYTE)&mineBomb,dwSize1);
		dwTemp=(flagState&0x0f00)/0x0100;
		RegSetValueEx(hKey,TEXT("Difficulty"),NULL,REG_DWORD,(LPBYTE)&dwTemp,dwSize1);
		dwTemp=(flagState&0x0008)/0x0008;
		RegSetValueEx(hKey,TEXT("Mark"),NULL,REG_DWORD,(LPBYTE)&dwTemp,dwSize1);
		dwTemp=(flagState&0x0004)/0x0004;
		RegSetValueEx(hKey,TEXT("Color"),NULL,REG_DWORD,(LPBYTE)&dwTemp,dwSize1);
		dwTemp=(flagState&0x0010)/0x0010;
		RegSetValueEx(hKey,TEXT("Sound"),NULL,REG_DWORD,(LPBYTE)&dwTemp,dwSize1);
		dwTemp=(flagState&0x0001)/0x0001;
		RegSetValueEx(hKey,TEXT("AlreadyPlayed"),NULL,REG_DWORD,(LPBYTE)&dwTemp,dwSize1);
		dwTemp=getLevelWindowSize(hWnd).left;
		RegSetValueEx(hKey,TEXT("Xpos"),NULL,REG_DWORD,(LPBYTE)&dwTemp,dwSize1);
		dwTemp=getLevelWindowSize(hWnd).top;
		RegSetValueEx(hKey,TEXT("Ypos"),NULL,REG_DWORD,(LPBYTE)&dwTemp,dwSize1);
		dwTemp=999;	
		LoadString(hInstance,IDS_NORMAL_NAME,lpAnonymity,sizeof(lpAnonymity));	
		//排行榜部分
		for (ix=0;ix<3;ix++)
		{				
			if (RegOpenKeyEx(hKey,lpsTime[ix],NULL,KEY_WRITE,&hKeySub)!=ERROR_SUCCESS)
			{
				RegSetValueEx(hKey,lpsTime[ix],NULL,dwType1,(LPBYTE)&dwTemp,dwSize1);
				RegCloseKey(hKeySub);
			}
			if (RegOpenKeyEx(hKey,lpsName[ix],NULL,KEY_WRITE,&hKeySub)!=ERROR_SUCCESS)
			{		
				RegSetValueEx(hKey,lpsName[ix],NULL,dwType2,(LPBYTE)&lpAnonymity,dwSize2);
				RegCloseKey(hKeySub);
			}
		}

		RegCloseKey(hKey);
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hWnd, message, wParam, lParam) ;
}

/*###########################################################
//功能：对话框消息处理
//参数：
//返回值：
//#########################################################*/
BOOL CALLBACK WinDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	int						width,height,bomb;
	static HINSTANCE		hInstance;
	LPCSTR					keyWinMine=TEXT("Software\\Microsoft\\winmine\\");
	HKEY					hKey;
	DWORD					dwTime;
	TCHAR					lpBuff[255],lpAnonymity[255],lpOutput[255],lpFormat[255];
	LPCTSTR					lpsTime[3]={TEXT("Time1"),TEXT("Time2"),TEXT("Time3")};
	LPCTSTR					lpsName[3]={TEXT("Name1"),TEXT("Name2"),TEXT("Name3")};
	int						i;
	DWORD 					dwSize1=sizeof(DWORD),dwType1=REG_DWORD,dwType2=REG_SZ,dwSize2=sizeof(lpBuff);
	switch (message)
	{
	case WM_CREATE:			//创建
		hInstance=((LPCREATESTRUCT)lParam)->hInstance;	//实例句柄
		break;
	case WM_INITDIALOG:		//初始化
		//初始化 自定义对话框 为当前数据
		SetDlgItemInt(hDlg,IDC_CUSTOM_HEIGHT,mineRowNum,TRUE);	
		SetDlgItemInt(hDlg,IDC_CUSTOM_WIDTH,mineLineNum,TRUE);
		SetDlgItemInt(hDlg,IDC_CUSTOM_BOMB,mineBomb,TRUE);
		//初始化排行榜为注册表保存数据，如果没有键值 设置默认
		LoadString(hInstance,IDS_SECOND,lpFormat,sizeof(lpFormat));
		if (RegOpenKeyEx(HKEY_CURRENT_USER,keyWinMine,0,KEY_READ,&hKey)==ERROR_SUCCESS)
		{
			for (i=0;i<3;i++)
			{
				if (RegQueryValueEx(hKey,lpsTime[i],NULL,&dwType1,(LPBYTE)&dwTime,&dwSize1)!=ERROR_SUCCESS)//时间
				{
					dwTime=999;
				}
				sprintf(lpOutput,lpFormat,dwTime);			//格式化
				SetDlgItemText(hDlg,IDC_BEGINER_TIME+i,(LPCSTR)lpOutput);		//显示时间到排行榜对话框	
				if (RegQueryValueEx(hKey,lpsName[i],NULL,&dwType2,(LPBYTE)&lpBuff,&dwSize2)!=ERROR_SUCCESS)//姓名
				{
					LoadString(hInstance,IDS_NORMAL_NAME,lpBuff,sizeof(lpBuff));
				}			
				SetDlgItemText(hDlg,IDC_BEGINER_NAME+i,(LPCSTR)lpBuff);			//显示姓名到排行榜对话框
			}
			RegCloseKey(hKey);
		}
		//初始化输入新纪录对话框
		LoadString(hInstance,IDS_RECORD_BEGINER+(flagState&0x0f00)/0x0100,lpBuff,sizeof(lpBuff));
		SetDlgItemText(hDlg,IDC_RECORD_MSG,lpBuff);
		LoadString(hInstance,IDS_NORMAL_NAME,lpBuff,sizeof(lpBuff));
		SetDlgItemText(hDlg,IDC_RECORD_NAME,lpBuff);
		break;
	case WM_COMMAND:		//控件消息处理
		switch(LOWORD(wParam))
		{
		case IDC_CUSTOM_OK:		//自定义确认
			//设置长、宽、雷为对话框输入，如果越界设为最值
			width=(GetDlgItemInt(hDlg,IDC_CUSTOM_WIDTH,FALSE,TRUE)<MAX_LN_NUM)?GetDlgItemInt(hDlg,IDC_CUSTOM_WIDTH,FALSE,TRUE):MAX_LN_NUM;
			mineLineNum=width>MIN_LN_NUM?width:MIN_LN_NUM;
			height=GetDlgItemInt(hDlg,IDC_CUSTOM_HEIGHT,FALSE,TRUE)<MAX_COL_NUM?GetDlgItemInt(hDlg,IDC_CUSTOM_HEIGHT,FALSE,TRUE):MAX_COL_NUM;
			mineRowNum=height>MIN_COL_NUM?height:MIN_COL_NUM;
			bomb=GetDlgItemInt(hDlg,IDC_CUSTOM_BOMB,FALSE,TRUE)<MAX_BOMB_NUM?GetDlgItemInt(hDlg,IDC_CUSTOM_BOMB,FALSE,TRUE):MAX_BOMB_NUM;
			mineBomb=bomb>MIN_BOMB_NUM?bomb:MIN_BOMB_NUM;
			mineBomb=mineBomb<(mineRowNum*mineLineNum*0.8)?mineBomb:(int)floor(mineRowNum*mineLineNum*0.8);
			EndDialog(hDlg,TRUE);
			break;
		case IDC_CUSTOM_CANCEL:	//自定义取消，不处理，关闭
			EndDialog(hDlg,FALSE);
			break;
		case IDC_HEROLIST_OK:	//排行榜确定，不处理，关闭
			EndDialog(hDlg,TRUE);
			break;
		case IDC_HEROLIST_RENEW:	//排行榜重置，并写入注册表
			LoadString(hInstance,IDS_NORMAL_NAME,lpAnonymity,sizeof(lpAnonymity));
			if (RegOpenKeyEx(HKEY_CURRENT_USER,keyWinMine,0,KEY_WRITE,&hKey)==ERROR_SUCCESS)
			{			
				for (i=0;i<3;i++)
				{	
					dwTime=999;
					RegSetValueEx(hKey,lpsTime[i],NULL,dwType1,(LPBYTE)&dwTime,dwSize1);
					RegSetValueEx(hKey,lpsName[i],NULL,dwType2,(LPBYTE)&lpAnonymity,dwSize2);
				}
				RegCloseKey(hKey);
				SendMessage(hDlg,WM_INITDIALOG,wParam,lParam);
			}
			break;
		case IDC_RECORD_OK:			//输入新纪录确定
			GetDlgItemText(hDlg,IDC_RECORD_NAME,lpBuff,sizeof(lpBuff));
			if (strlen(lpBuff)>0)	//输入不为空
			{
				if (RegOpenKeyEx(HKEY_CURRENT_USER,keyWinMine,0,KEY_WRITE,&hKey)==ERROR_SUCCESS)
				{
					RegSetValueEx(hKey,lpsTime[(flagState&0x0f00)/0x0100],NULL,dwType1,(LPBYTE)&timeCount,dwSize1);
					RegSetValueEx(hKey,lpsName[(flagState&0x0f00)/0x0100],NULL,dwType2,(LPBYTE)&lpBuff,dwSize2);
					RegCloseKey(hKey);
				}
			}
			EndDialog(hDlg,TRUE);			
			break;
		}
	}

		return FALSE;				//不处理的消息返回FALSE
}

/*###########################################################
功能：初始化地图基本信息
参数：窗口句柄、等级、宽、高、雷、标志数、时间计数标志、表情标志
返回值：成功 返回TRUE
###########################################################*/

BOOL IinitializeMapInfo(HWND hWnd,int level,int col,int ln,int bomb,int playFlag,int timeFlag,int faceFlag){
	//越界设为最值
	mineRowNum=(col<MAX_COL_NUM) ? col:MAX_COL_NUM;
	mineRowNum=(col>MIN_COL_NUM) ? col:MIN_COL_NUM;
	mineLineNum=(ln<MAX_LN_NUM) ? ln:MAX_LN_NUM;
	mineLineNum=(ln>MIN_LN_NUM) ? ln:MIN_LN_NUM;
	mineBomb=(bomb<MAX_BOMB_NUM) ? bomb:MAX_BOMB_NUM;
	mineBomb=(bomb>MIN_BOMB_NUM) ? bomb:MIN_BOMB_NUM;
	//设置标志位
	flagState=(flagState & 0xf0ff)+level*0x0100;
	flagState=(flagState & 0xfffe)+playFlag*0x0001;
	flagState=(flagState & 0xfffd)+timeFlag*0x0002;
	flagState=(flagState & 0x0fff)+faceFlag*0x1000;
	bombCount=bomb;
	timeCount=0;
	openCount=mineRowNum*mineLineNum-mineBomb;
	//停止当前已有的计时器
	KillTimer(hWnd,FLAG_TIMECOUNT);
	return TRUE; 
}



/*###########################################################
//功能：生成随机地图
//参数：窗口句柄
//返回值：
//###########################################################*/
BOOL CreateRandomMap(HWND hWnd){
	btLandmineMapinfo[0][0]=(BYTE)0;
	DWORD ixMine,iyMine,iMineBomb,icBomb=0,x,y;
	RECT rcMine;
	//初始为0
	for(int i=0;i<mineRowNum;i++)
	{
		for(int j=0;j<mineLineNum;j++) btLandmineMapinfo[i][j]=(BYTE)0;
	}
	//分4部分
	ixMine=(int)floor(mineRowNum/2);
	iyMine=(int)floor(mineLineNum/2);
	iMineBomb=(int)floor(mineBomb/4);
    srand((unsigned)time(NULL));		//得到时间随机
	//设置左上
	while (icBomb<iMineBomb) {
		x=rand()%ixMine;
		y=rand()%iyMine;
		if (btLandmineMapinfo[x][y]!=0x10) {
			btLandmineMapinfo[x][y]=(BYTE)0x10;
			icBomb++;
		}
	 
	}
	icBomb=0;
	//设置右上
	while (icBomb<iMineBomb) {
		x=rand()%(mineRowNum-ixMine)+ixMine;
		y=rand()%iyMine;
		if (btLandmineMapinfo[x][y]!=0x10) {
			btLandmineMapinfo[x][y]=(BYTE)0x10;
			icBomb++;
		}
	}
	icBomb=0;
	//设置左下
	while (icBomb<iMineBomb) {
		x=rand()%ixMine;
		y=rand()%(mineLineNum-iyMine)+iyMine;
		if (btLandmineMapinfo[x][y]!=0x10) {
			btLandmineMapinfo[x][y]=(BYTE)0x10;
			icBomb++;
		}
	}
	icBomb=0;
	//设置右下
	while (icBomb<(mineBomb-3*iMineBomb)) {
		x=rand()%(mineRowNum-ixMine)+ixMine;
		y=rand()%(mineLineNum-iyMine)+iyMine;
		if (btLandmineMapinfo[x][y]!=0x10) {
			btLandmineMapinfo[x][y]=(BYTE)0x10;
			icBomb++;
		}
		
	}
	rcMine=CreateRect(MAIN_LEFTSTND,MAIN_TOPSTND,100000,100000);
	InvalidateRect(hWnd,&rcMine,TRUE);
	return TRUE;
}


/*###########################################################
功能：得到鼠标的位置和状态
参数：窗口句柄、实例句柄、x坐标、y坐标、当前类型
返回值：当前鼠标的信息结构体
###########################################################*/

tagMouseState getMousePosition(HWND hWnd,HINSTANCE hInstance,int cxMousePos,int cyMousePos,int mouseType){
	tagMouseState state;
	//雷区，得到雷区的位置
	if (cxMousePos>MAIN_LEFTSTND&&cxMousePos<(MAIN_LEFTSTND+mineRowNum*MINE_BITMAP_WIDTH)&&cyMousePos>MAIN_TOPSTND&&cyMousePos<(MAIN_TOPSTND+mineLineNum*MINE_BITMAP_HEIGHT))
	{	
		state.mXpos=(int)floor((cxMousePos-MAIN_LEFTSTND)/MINE_BITMAP_WIDTH);
		state.mYpos=(int)floor((cyMousePos-MAIN_TOPSTND)/MINE_BITMAP_HEIGHT);
		state.mIDpos=MOUSE_ON_MINE;
	} //表情区
	else if (cxMousePos>=(MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2)
			&&cxMousePos<=(MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2+FACE_BITMAP_WIDTH)
			&&cyMousePos>=((MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2)
			&&cyMousePos<=((MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2+FACE_BITMAP_HEIGHT)) 
	{
		state.mIDpos=MOUSE_ON_FACE;
	}else//其他区域
	{	
		state.mXpos=cxMousePos>MAIN_LEFTSTND?(int)floor((cxMousePos-MAIN_LEFTSTND)/MINE_BITMAP_WIDTH):-2;
		state.mXpos=cxMousePos<(MAIN_LEFTSTND+mineRowNum*MINE_BITMAP_WIDTH)?state.mXpos:mineRowNum+1;
		state.mYpos=cyMousePos>MAIN_TOPSTND?(int)floor((cyMousePos-MAIN_TOPSTND)/MINE_BITMAP_HEIGHT):-2;
		state.mYpos=cyMousePos<(MAIN_TOPSTND+mineLineNum*MINE_BITMAP_HEIGHT)?state.mYpos:mineLineNum+1;
		state.mIDpos=MOUSE_ON_OTHER;
	}
	state.mIDbutton=mouseType;
	return state;
}


/*###########################################################
功能：左键弹起后改变地图上方格的信息
参数：窗口句柄、实例句柄、鼠标信息
返回值：
###########################################################*/

BOOL OnChangeMineInfo(HWND hWnd,HINSTANCE hInstance,tagMouseState mouse){
	tagMouseState lastMouse;	//上次的鼠标信息
	RECT rcRefresh=CreateRect(mouse.mXpos,mouse.mYpos,0,0);	//刷新区域

	if (mouse.mXpos<0||mouse.mYpos<0||mouse.mXpos>=mineRowNum||mouse.mYpos>=mineLineNum){ //不在雷区，返回
		return FALSE;
	}
	if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]>=0x20)		//大于0x20，还原
	{
		OnChangeMineUpon(mouse);
	}
	//棋子或者数字，已翻开的雷等，不处理
	if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10==0x01||btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10>0x02)
	{
		return FALSE;
	}
	//雷或者雷+?
	if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x10||btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x12) 
	{	
		btLandmineMapinfo[mouse.mXpos][mouse.mYpos]=(BYTE)0x13;		//		设置为爆炸的雷
		setGameLost(hWnd,hInstance);								//		游戏失败
	}
	//不是雷
	else if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x00||btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x02)
	{
		btLandmineMapinfo[mouse.mXpos][mouse.mYpos]=0x0f-getRoundMineNum(hWnd,mouse.mXpos,mouse.mYpos);//得到周围的雷数
		openCount--;//空格数减少
		if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x0f) //如果是0
		{
			lastMouse=mouse;
			for (int i=0;i<3;i++)
			{
				for (int j=0;j<3;j++)
				{	
					lastMouse.mXpos=mouse.mXpos-1+i;
					lastMouse.mYpos=mouse.mYpos-1+j;
					OnChangeMineInfo(hWnd,hInstance,lastMouse); //递归展开周围，直到数字边界
				}
			}
		}
	}	
	InvalidateRect(hWnd,&rcRefresh,TRUE);	
	return TRUE;
}

/*###########################################################
功能：游戏失败
参数：窗口句柄、实例句柄
返回值：
###########################################################*/

BOOL setGameLost(HWND hWnd,HINSTANCE hInstance){
	RECT rcRefrush=CreateRect(MAIN_LEFTSTND,MAIN_TOPSTND,1000,1000);
	KillTimer(hWnd,FLAG_TIMECOUNT);	//结束计时器
	if ((flagState&0x0010)!=0x0000)	// 播放声音
	{
		HRSRC hrWare=FindResource(hInstance,MAKEINTRESOURCE(IDW_WAVE_BOMB),TEXT("WAVE"));
		HGLOBAL hGlobal=LoadResource(hInstance,hrWare);
		PlaySoundW((LPCWSTR)LockResource(hGlobal),NULL,SND_MEMORY|SND_ASYNC);
	}

	for(int i=0;i<mineRowNum;i++)	//展开所有雷
	{
		for(int j=0;j<mineLineNum;j++)
		{
			if (btLandmineMapinfo[i][j]==0x10) 
			{				
				btLandmineMapinfo[i][j]=0x15;	//未爆的雷
			}
			if (btLandmineMapinfo[i][j]==0x01)
			{
				btLandmineMapinfo[i][j]=0x04;	//标错的棋子
			}
		}
	}
	flagState=flagState&0xfffe;
	InvalidateRect(hWnd,&rcRefrush,TRUE);	
	RefreshSmileFaceBmp(hWnd,FACE_TYPE_GRIEF);
	return TRUE;
}

/*###########################################################
功能：获得当前等级窗口的位置和大小信息
参数：窗口句柄
返回值：区域 左上的坐标（left，top）大小（right，bottom）
###########################################################*/

RECT getLevelWindowSize(HWND hWnd){
	RECT rcFrame,rcWindow,rcClient;
	GetWindowRect(hWnd,&rcWindow);
	GetClientRect(hWnd,&rcClient);
	rcFrame.left=rcWindow.left;		//窗口左边位置
	rcFrame.top=rcWindow.top;		//窗口上边位置
		//窗口的宽度
	rcFrame.right=rcClient.left-rcWindow.left+rcWindow.right-rcClient.right+mineRowNum*MINE_BITMAP_WIDTH+MAIN_LEFTSTND+MAIN_RIGHTSTND;
		//窗口的高度
	rcFrame.bottom=rcClient.top-rcWindow.top+rcWindow.bottom-rcClient.bottom+mineLineNum*MINE_BITMAP_HEIGHT+MAIN_TOPSTND+MAIN_BOTTOMSTND;
	return rcFrame;
}

/*###########################################################
功能：描绘3D边框
参数：HDC，区域、线宽
返回值：
###########################################################*/

BOOL DrawSolidFrame(HDC hDC,RECT rcFrame,int iLighten){
	HPEN hPen;

	for(int iCount=0;iCount<iLighten;iCount++){	
		hPen=CreatePen(PS_SOLID,0,RGB(250,250,250));
		SelectObject(hDC,hPen);
		MoveToEx(hDC,rcFrame.right-iCount-1,rcFrame.top+iCount,NULL);	//右上
		LineTo(hDC,rcFrame.right-iCount-1,rcFrame.bottom-iCount-1);		//右下
		LineTo(hDC,rcFrame.left+iCount,rcFrame.bottom-iCount-1);		//左下
		DeleteObject(hPen);
		hPen=CreatePen(PS_SOLID,0,RGB(8+(flagState&0x0004)*30,8+(flagState&0x0004)*30,8+(flagState&0x0004)*30));
		SelectObject(hDC,hPen);
		LineTo(hDC,rcFrame.left+iCount,rcFrame.top+iCount);				//左上
		LineTo(hDC,rcFrame.right-iCount,rcFrame.top+iCount);			//右上
		DeleteObject(hPen);
	}
	return TRUE;
}

/*###########################################################
功能：	生成一个矩形区域，如果后两个参数为0，得到的是一个雷的大小的区域，否则是两点参数的区域
参数：		
返回值：
###########################################################*/
RECT CreateRect(int left,int top,int right,int bottom){
	RECT rcReturn;
	rcReturn.left=(right==0)?MAIN_LEFTSTND+left*MINE_BITMAP_WIDTH:left;
	rcReturn.top=(bottom==0)?MAIN_TOPSTND+top*MINE_BITMAP_HEIGHT:top;
	rcReturn.right=(right==0)?rcReturn.left+MINE_BITMAP_WIDTH:right;
	rcReturn.bottom=(bottom==0)?rcReturn.top+MINE_BITMAP_HEIGHT:bottom;
	return rcReturn;
}

/*###########################################################
功能：获得周围雷的数目
参数：窗口句柄、所在位置
返回值：周围雷的数目
###########################################################*/
BYTE getRoundMineNum(HWND hWnd,int iCol,int iLn){
	BYTE btNum=0;
	for(int i=iCol-1;i<=iCol+1;i++) {
		for(int j=iLn-1;j<=iLn+1;j++){
			//大于0x10为雷
			if (btLandmineMapinfo[i][j]>=0x10&&i>=0&&j>=0&&i<mineRowNum&&j<mineLineNum) {
				btNum++;
			}
		}
	}
	return btNum;
	
}

/*###########################################################
功能：获得周围旗子的数目
参数：窗口句柄、所在位置
返回值：周围旗子的数目
###########################################################*/
BYTE getRoundFlagNum(HWND hWnd,int iCol,int iLn){
	BYTE btNum=0;
	for(int i=iCol-1;i<=iCol+1;i++) {
		for(int j=iLn-1;j<=iLn+1;j++){
			//btLandmineMapinfo[i][j]%0x10==0x01 为标记成棋子
			if (btLandmineMapinfo[i][j]%0x10==0x01&&i>=0&&j>=0&&i<mineRowNum&&j<mineLineNum) {
				btNum++;
			}
		}
	}
	return btNum;	
}

/*###########################################################
功能：刷新表情图片区域
参数：窗口句柄、表情类型
返回值：
###########################################################*/

BOOL RefreshSmileFaceBmp(HWND hWnd,int iFaceType){
	RECT rcRefresh=CreateRect(MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2,(MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2,MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2+FACE_BITMAP_WIDTH,(MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2+FACE_BITMAP_HEIGHT);
	flagState = (flagState&0x0fff)+iFaceType*0x1000;
	InvalidateRect(hWnd,&rcRefresh,TRUE);
	return TRUE;
}

/*###########################################################
功能：鼠标消息处理
参数：窗口句柄、实例句柄、当前鼠标结构体、自定义消息类型
返回值：
###########################################################*/

BOOL OnMouseClick(HWND hWnd,HINSTANCE hInstance,tagMouseState mouse,int nIDMessage){
	static	tagMouseState lastMouse;	//前一次的鼠标结构体
	tagMouseState tempMouse;			//临时鼠标结构体
	TCHAR lpMessage[255];				//消息字符串
	int i,j;							//临时变量
	BYTE temp;							//临时变量
	RECT rcRefresh;						//刷新区域
	switch(mouse.mIDpos)
	{
	case MOUSE_ON_MINE:					//鼠标在雷区
		if ((!(flagState & FLAG_ALREADPLAYED)))
			{
				return 0;	//游戏结束或尚未开始，返回
			}
		switch(nIDMessage)
		{
		case DEF_LBUTTONDOWN:				//左键按下
			if (OnChangeMineDown(mouse))	//改变地图的数据，使之显示为按下的
			{
				rcRefresh=CreateRect(mouse.mXpos,mouse.mYpos,0,0);
				InvalidateRect(hWnd,&rcRefresh,TRUE);
			}
			lastMouse=mouse;				//纪录上次为当前
			break;
		case DEF_LBUTTONUP:					//鼠标弹起
			if (lastMouse.mIDpos!=MOUSE_ON_MINE)
			{
				KillTimer(hWnd,FLAG_TIMECOUNT);
				flagState=flagState&0xfffe;
			}
			if (mouse.mIDpos!=MOUSE_ON_MINE)
			{
				return 0;
			}
			if (!(flagState & 0x0002))		//未开始计时
			{
				if (SetTimer(hWnd,FLAG_TIMECOUNT,1000,NULL)==0) //计时器启动失败
				{
					LoadString(hInstance,IDS_TIMER_ERROR,lpMessage,sizeof(lpMessage));
					if (MessageBox(hWnd,lpMessage,"严重错误!",MB_ICONSTOP)==IDOK)
					{
						SendMessage(hWnd,WM_DESTROY,NULL,NULL); //退出程序
					}
				}
					if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]>=0x20) //大于0x20 还原
				{
					OnChangeMineUpon(mouse);
				}//第一次是雷
				if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x10||btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x12)
				{
					i=0;
					do{	
						if (mouse.mYpos+i>=mineRowNum||mouse.mXpos+i>=mineLineNum) //交换不能解决，重新生成地图
						{
							CreateRandomMap(hWnd);
							i=0;
						}//交换位置
						temp=btLandmineMapinfo[mineRowNum-mouse.mXpos-i][mineLineNum-mouse.mYpos-i];
						btLandmineMapinfo[mineRowNum-mouse.mXpos-i][mineLineNum-mouse.mYpos-i]=btLandmineMapinfo[mouse.mXpos][mouse.mYpos];
						btLandmineMapinfo[mouse.mXpos][mouse.mYpos]=temp;
						i++;
					}while (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]>=0x10);
					
				}
				flagState = (flagState & 0xfffd)+0x0002; //开始计时器
			}
			lastMouse=mouse;
			RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_POP);
			OnChangeMineInfo(hWnd,hInstance,mouse);			//改变地图信息
			break;
		case DEF_RBUTTONDOWN:			//雷区右键按下消息
			if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10>0x02)//已经翻开，返回
			{
				break;
			}
			if ((btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10)<0x01+(flagState & 0x0008)/0x0008) 
			{
				btLandmineMapinfo[mouse.mXpos][mouse.mYpos]++;//如果标记：未翻开 -〉棋子-〉问号；如果不标记：未翻开 -〉棋子
			}
			else if ((btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10)==0x01+(flagState & 0x0008)/0x0008)
			{
				btLandmineMapinfo[mouse.mXpos][mouse.mYpos]-=(1+(flagState & 0x0008)/0x0008);//返回为未翻开的状态
			}else if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10==0x02&&(flagState & 0x0008)==0)
			{
				btLandmineMapinfo[mouse.mXpos][mouse.mYpos]-=2;
				bombCount--;
			}
			//计雷，凑出来的二元函数，正好满足规律
			bombCount=bombCount-((btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10)%2)*(0x01+(((flagState & 0x0008)/0x0008+0x01)%0x02))+(btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10)/0x02+((flagState & 0x0008)/0x0008+0x01)%0x02;
			rcRefresh=CreateRect(mouse.mXpos,mouse.mYpos,0,0);
			InvalidateRect(hWnd,&rcRefresh,TRUE);
			rcRefresh=CreateRect(COUNT_LEFTPOS,COUNT_TOPPOS,COUNT_RIGHTPOS,COUNT_BOTTOMPOS);
			InvalidateRect(hWnd,&rcRefresh,TRUE);
		    break;
		case DEF_DBBUTTONDOWN:			//双键按下
			for (i=0;i<3;i++)			//对周围共9个格处理左键按下的消息
			{
				for (j=0;j<3;j++)
				{
					//处理越界
					tempMouse.mXpos=(mouse.mXpos-1+i>=0)?mouse.mXpos-1+i:0;
					tempMouse.mXpos=(mouse.mXpos-1+i<mineRowNum)?tempMouse.mXpos:mineRowNum;					
					tempMouse.mYpos=mouse.mYpos-1+j>=0?mouse.mYpos-1+j:0;
					tempMouse.mYpos=(mouse.mYpos-1+j<mineLineNum)?tempMouse.mYpos:mineRowNum;
					tempMouse.mIDpos=MOUSE_ON_MINE;
					tempMouse.mIDbutton=DEF_LBUTTONDOWN;
					OnMouseClick(hWnd,hInstance,tempMouse,DEF_LBUTTONDOWN);  //左键按下处理
				}				
			}
			lastMouse=mouse;		//纪录之前为当前
			break;
		case DEF_DBBUTTONUP:		//双键弹起，对周围共9个格处理左键按下的还原
			for (i=0;i<3;i++)			
			{
				for (j=0;j<3;j++)
				{
					tempMouse.mXpos=(mouse.mXpos-1+i>=0)?mouse.mXpos-1+i:0;
					tempMouse.mXpos=(mouse.mXpos-1+i<mineRowNum)?tempMouse.mXpos:mineRowNum;					
					tempMouse.mYpos=mouse.mYpos-1+j>=0?mouse.mYpos-1+j:0;
					tempMouse.mYpos=(mouse.mYpos-1+j<mineLineNum)?tempMouse.mYpos:mineRowNum;
					OnChangeMineUpon(tempMouse);		//还原
					rcRefresh=CreateRect(tempMouse.mXpos,tempMouse.mYpos,0,0);
					InvalidateRect(hWnd,&rcRefresh,TRUE);//刷新
				}	
			}

			if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10>6)		//如果是翻开的数字
			{
				i=getRoundFlagNum(hWnd,mouse.mXpos,mouse.mYpos);		//得到旗子数
				j=getRoundMineNum(hWnd,mouse.mXpos,mouse.mYpos);		//得到雷数
				if (i==j)	//两者相等
				{
					for (int i=0;i<3;i++)								//翻开周围9格（会递归）
					{
						for (int j=0;j<3;j++)
						{	
							tempMouse.mXpos=(mouse.mXpos-1+i>=0)?mouse.mXpos-1+i:0;
							tempMouse.mXpos=(mouse.mXpos-1+i<=mineRowNum)?tempMouse.mXpos:mineRowNum;					
							tempMouse.mYpos=mouse.mYpos-1+j>=0?mouse.mYpos-1+j:0;
							tempMouse.mYpos=(mouse.mYpos-1+j<=mineLineNum)?tempMouse.mYpos:mineRowNum;
							OnChangeMineInfo(hWnd,hInstance,tempMouse);
						}
					}
				}
			}

			lastMouse=mouse;			//纪录之前为当前
			break;	
		case DEF_MOUSEMOVING:			//鼠标移动
			switch(mouse.mIDbutton)
			{
			case DEF_LBUTTONDOWN:		//左键按下移动
				if (lastMouse.mXpos!=mouse.mXpos||lastMouse.mYpos!=mouse.mYpos)		//位置变化
				{
					//之前位于雷区
					if (lastMouse.mXpos>=0&&lastMouse.mYpos>=0&&lastMouse.mXpos<mineRowNum&&lastMouse.mYpos<mineLineNum)
					{
						if (btLandmineMapinfo[lastMouse.mXpos][lastMouse.mYpos]>=0x20) //按下了
						{
							OnChangeMineUpon(lastMouse);								//还原
						}

					}
					rcRefresh=CreateRect(lastMouse.mXpos,lastMouse.mYpos,0,0);
					InvalidateRect(hWnd,&rcRefresh,TRUE);
					if (mouse.mIDpos==MOUSE_ON_MINE)									//现在是雷区状态
					{
						if (OnChangeMineDown(mouse))									//使之双键按下
						{
							rcRefresh=CreateRect(mouse.mXpos,mouse.mYpos,0,0);			
							InvalidateRect(hWnd,&rcRefresh,TRUE);
						}
						lastMouse=mouse;
					 }

				}
				break;
			case DEF_DBBUTTONDOWN:														//双键按下移动
				if (lastMouse.mXpos!=mouse.mXpos||lastMouse.mYpos!=mouse.mYpos)			//位置改变了
				{
					if (lastMouse.mIDpos==MOUSE_ON_MINE)								//之前位于雷区
					{
						OnMouseClick(hWnd,hInstance,lastMouse,DEF_DBBUTTONUP);			//对之前双键弹起消息处理
					}
					if (mouse.mXpos>=0&&mouse.mYpos>=0)									
					{
						OnMouseClick(hWnd,hInstance,mouse,DEF_DBBUTTONDOWN);			//对当前双键按下处理
					}
					lastMouse=mouse;   //纪录之前为当前
				}
			    break;
			}

			break;
		}
		break;
	case MOUSE_ON_FACE:				//位于表情区域
		switch(nIDMessage)
		{
		case DEF_LBUTTONDOWN:		//左键按下
			RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_PUSH);			//笑脸按下
			lastMouse=mouse;
			break;
		case DEF_LBUTTONUP:			//左键弹起
			if (flagState&0x0001==0x0000)			//游戏未开始则开始
			{
				flagState=(flagState&0xfffe)+0x0001;
			}
			//初始化等
			IinitializeMapInfo(hWnd,(flagState&0x0f00)/0x0100,mineRowNum,mineLineNum,mineBomb,TRUE,FALSE,FACE_TYPE_SMILE_POP); //??
			CreateRandomMap(hWnd);
			InvalidateRect(hWnd,NULL,TRUE);
			RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_POP);
			lastMouse=mouse;
			break;
		}
		break;
	case MOUSE_ON_OTHER:							//位于其他区域
		if (!(flagState & FLAG_ALREADPLAYED))		//未开始返回
			{
				return FALSE;
			}
		switch(nIDMessage)
		{
		case DEF_LBUTTONDOWN:						//左键按下
			RefreshSmileFaceBmp(hWnd,FACE_TYPE_EXCLAIM);		//惊叹表情
			break;
		case DEF_LBUTTONUP:							//左键弹起
			lastMouse.mIDpos=MOUSE_ON_MINE;
			OnMouseClick(hWnd,hInstance,mouse,DEF_MOUSEMOVING);		//处理雷区左键按下移动，刷新之前
			OnChangeMineUpon(lastMouse);							//还原之前
			rcRefresh=CreateRect(lastMouse.mXpos,lastMouse.mYpos,0,0);	
			InvalidateRect(hWnd,&rcRefresh,TRUE);
		    RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_POP);
		    break;
		case DEF_RBUTTONDOWN:
		    break;
		case DEF_MOUSEMOVING:	//鼠标移动
			if (lastMouse.mIDpos==MOUSE_ON_FACE)
			{
				RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_POP);
			}
			else if (lastMouse.mIDpos==MOUSE_ON_MINE)
			{		
				mouse.mIDpos=MOUSE_ON_MINE;			//处理雷区移动消息，刷新之前
				OnMouseClick(hWnd,hInstance,mouse,DEF_MOUSEMOVING);
			}
			break;
		}
	    break;
	default:
	    break;
	}

	if (openCount==0)//&&bombCount==0)		//判断结束：空格为0，剩下的都是雷（标记和为标记旗子的）
	{
		setGameSucceed(hWnd,hInstance);		//处理游戏成功
	}
	return FALSE;
}

/*###########################################################
功能：设置等级改变后菜单的check
参数：菜单句柄、等级
返回值：
###########################################################*/
BOOL OnCheckLevelMenu(HMENU hMenu,int level){
	CheckMenuItem(hMenu,IDM_ADVANCED,MF_UNCHECKED);//所有未打钩
	CheckMenuItem(hMenu,IDM_INTERMEDIATE,MF_UNCHECKED);
	CheckMenuItem(hMenu,IDM_BEGINNER,MF_UNCHECKED);
	CheckMenuItem(hMenu,IDM_CUSTOMDEFINE,MF_UNCHECKED);
	CheckMenuItem(hMenu,IDM_BEGINNER+level,MF_CHECKED); //钩选当前等级
	flagState=((flagState&0xf0ff)+level*0x0100);		//改变状态
	return TRUE;
}
/*###########################################################
功能：处理游戏成功
参数：
返回值：
###########################################################*/

BOOL setGameSucceed(HWND hWnd,HINSTANCE hInstance){
	KillTimer(hWnd,FLAG_TIMECOUNT);			//结束计数
	openCount=-1;							//重置空格计数
	RECT rcRefresh;
	
	LPCTSTR			keyWinMine=TEXT("Software\\Microsoft\\winmine\\");
	LONG			lErrorID=0;
	DWORD 			dwSize=sizeof(DWORD);
	HKEY			hKey;
	DWORD			dwTime;
	LPCTSTR			lpsLevel[3]={TEXT("Time1"),TEXT("Time2"),TEXT("Time3")};
	if ((flagState&0x0010)!=0)
	{
		HRSRC			hrWare=FindResource(hInstance,MAKEINTRESOURCE(IDW_WAVE_DONE),TEXT("WAVE")); //播放声音资源
		HGLOBAL			hGlobal=LoadResource(hInstance,hrWare);
		PlaySoundW((LPCWSTR)LockResource(hGlobal),NULL,SND_MEMORY|SND_ASYNC);
	}

	flagState=flagState& 0xfffe;				//设置结束状态，未计时
	RefreshSmileFaceBmp(hWnd,FACE_TYPE_COOL);	//表情-酷
	for (int i=0;i<mineRowNum;i++)				//标记未标记的雷为棋子
	{
		for (int j=0;j<mineLineNum;j++)
		{
			if (btLandmineMapinfo[i][j]>=0x10)
			{
				btLandmineMapinfo[i][j]=0x11;
				rcRefresh=CreateRect(i,j,0,0);
				InvalidateRect(hWnd,&rcRefresh,TRUE);
			}
		}
	}
	if ((flagState&0x0f00)>0x0200)	//如果登记为自定义，返回，不处理排行和注册表
	{
		return FALSE;
	}
	lErrorID=RegOpenKeyEx(HKEY_CURRENT_USER,keyWinMine,0,KEY_READ,&hKey);//读取当前保存消息
	if (lErrorID==ERROR_SUCCESS)
	{
		lErrorID=RegQueryValueEx(hKey,lpsLevel[(flagState&0x0f00)/0x0100],NULL,NULL,(LPBYTE)&dwTime,&dwSize);
		if (lErrorID==ERROR_SUCCESS)
		{
			if (timeCount<(int)dwTime)  //破纪录了
			{
				DialogBox(hInstance,MAKEINTRESOURCE(IDD_RECORDDLG),hWnd,WinDlgProc);  //输入新纪录
			}
		} 
		else
		{
			ErrorMessageBox(hWnd,hInstance,IDS_ERRORID,lErrorID);
		}
	
	}else
	{
		ErrorMessageBox(hWnd,hInstance,IDS_ERRORID,lErrorID);
	}
	RegCloseKey(hKey);
	return TRUE;
}

/*###########################################################
功能：错误消息对话框
参数：窗口句柄、实例句柄、消息、消息id
返回值：
###########################################################*/

BOOL ErrorMessageBox(HWND hWnd,HINSTANCE hInstance,UINT IDMessage,UINT IDError){
	TCHAR lpFormat[255],lpOutput[255];
	if (IDError!=0)
	{
		LoadString(hInstance,IDMessage,lpFormat,sizeof(lpFormat));
		sprintf(lpOutput,lpFormat,IDError);
	}
	else
	{
		LoadString(hInstance,IDMessage,lpOutput,sizeof(lpOutput));
	} 
	MessageBox(hWnd,lpOutput,"错误",MB_ICONERROR);
	return TRUE;
}

/*###########################################################
功能：是方格显示为按下的状态
参数：鼠标位置结构体
返回值：
###########################################################*/

BOOL OnChangeMineDown(tagMouseState mouse){
	if (mouse.mXpos<0||mouse.mXpos>=mineRowNum||mouse.mYpos<0||mouse.mYpos>=mineLineNum) //越界，不处理
	{
		return FALSE;
	}
	if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10==0x01||btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10>0x02)
	{
		return FALSE;					//标记棋子或翻开的不处理
	}
	else if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]<0x20)			//如果未经状态处理
	{
		if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10==0)		//未翻开状态
		{
			btLandmineMapinfo[mouse.mXpos][mouse.mYpos]+=0x2f;			//+0x2f 显示为 0的状态
		} 
		else if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10==0x02)//问号
		{
			btLandmineMapinfo[mouse.mXpos][mouse.mYpos]+=0x24;			//+0x24 显示为按下的问号
		}
		return TRUE;
	}
	return TRUE;
}

/*###########################################################
功能：还原显示为按下状态的方格
参数：鼠标位置结构体
返回值：
###########################################################*/

BOOL OnChangeMineUpon(tagMouseState mouse){
	if (mouse.mXpos<0||mouse.mXpos>=mineRowNum||mouse.mYpos<0||mouse.mYpos>=mineLineNum)
	{
		return FALSE;
	}
	if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]>=0x20)
	{
		btLandmineMapinfo[mouse.mXpos][mouse.mYpos]=btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x20;
	}
	else return FALSE;
	if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10==0x0f)
	{
		btLandmineMapinfo[mouse.mXpos][mouse.mYpos]-=0x0f;
	} 
	else if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10==0x06)
	{
		btLandmineMapinfo[mouse.mXpos][mouse.mYpos]-=0x04;
	}
	return TRUE;
}