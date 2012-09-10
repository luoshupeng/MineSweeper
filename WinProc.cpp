/*//////////////////////�ļ�˵��//////////////////////////
//														//
//�ļ���: WinProc.cpp					       			//
//��  ��: ���ܺ����Ķ����ʵ��							//
//��  ��:												//
//														//
////////////////////////////////////////////////////////*/

#include "WinProc.h"

/*###########################################################
���ܣ������ڵ���Ϣ����ص�����
������
����ֵ��
###########################################################*/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC							hDC,hDCMem;//,hDCScreen;		
	HINSTANCE					hLib;						//shell32.dll ʵ�����
	static	FARPROC				fpAbout;					//ShellAbout �������
	static	HICON				hIcon;						//����ͼ����
	static	HINSTANCE			hInstance;					//����ʵ�����
	static	HMENU				hMenu;						//�������˵����
	PAINTSTRUCT					ps;
	HBRUSH						hBrush;
	HPEN						hPen;
	HBITMAP						hBmp;
	int							ix=0,iy=0;
	RECT						rcWnd,rcRefresh;			//���������ˢ������
	static tagMouseState		mousePos;					//���״̬�ṹ��
	HRSRC						hrWare;						//wave��Դ���
	HGLOBAL						hGlobal;					
	LPCTSTR						keyWinMine=TEXT("Software\\Microsoft\\winmine\\");	//ע�������
	HKEY						hKey,hKeySub;				//ע�������
	DWORD						dwTemp;						//��ʱ����
	LPCTSTR						lpsTime[3]={TEXT("Time1"),TEXT("Time2"),TEXT("Time3")};
	LPCTSTR						lpsName[3]={TEXT("Name1"),TEXT("Name2"),TEXT("Name3")};
	TCHAR						lpAnonymity[255];			//��ʱ����
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
		//ע����ȡ�ѱ������Ϣ�������ؼ�ֵ�������򴴽�
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
		//��ʼ��������Ϣ
		*//////////////////////////////////////////////////////////
		flagState=(flagState&0x0fff)+0x4000;
		IinitializeMapInfo(hWnd,(flagState&0x0f00)/0x0100,mineRowNum,mineLineNum,mineBomb,flagState & 0x0001,FALSE,FACE_TYPE_SMILE_POP);	
		CheckMenuItem(hMenu,IDM_MARK,((flagState&0x0008)==FLAG_MARK) ? MF_CHECKED:MF_UNCHECKED);
		CheckMenuItem(hMenu,IDM_COLOR,((flagState&0x0004)==FLAG_COLOR) ? MF_CHECKED:MF_UNCHECKED);
		CheckMenuItem(hMenu,IDM_SOUND,((flagState&0x0010)==FLAG_SOUND) ? MF_CHECKED:MF_UNCHECKED);			
		OnCheckLevelMenu(hMenu,(flagState&0x0f00)/0x0100);		
		//���㴰�ڴ�С����ʼ����ͼ����
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
		//����Windows�Դ��� shell32.dll,Ϊ����"����"(ShellAbout)��׼��
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
		Rectangle(hDC,3,3,rcWnd.right+1,rcWnd.bottom+1);//���Ƶײ㱳��		
		DeleteObject(hBrush);
		hBrush=CreateSolidBrush(RGB(8+(flagState&0x0004)*30,8+(flagState&0x0004)*30,8+(flagState&0x0004)*30));
		SelectObject(hDC,hBrush);
		//���Ʊ��鱳��
		Rectangle(	hDC,
					MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2-1,
					(MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2-1,
					MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2+FACE_BITMAP_WIDTH+2,
					(MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2+FACE_BITMAP_HEIGHT+2);

		DeleteObject(hBrush);
		DeleteObject(hPen);

		//������ͼ
		hBmp=LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_SMILE_BLACK+(flagState&0x0004)/0x0004));
		SelectObject(hDCMem,hBmp);
		BitBlt(hDC,MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2,(MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2,FACE_BITMAP_WIDTH,FACE_BITMAP_HEIGHT,hDCMem,0, ((flagState & 0xf000)/0x1000) *FACE_BITMAP_HEIGHT,SRCCOPY);
		DeleteObject(hBmp);

		//��������ͼ
		hBmp=LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_NUM_BLACK+(flagState&0x0004)/0x0004));
		SelectObject(hDCMem,hBmp);
		BitBlt(hDC,COUNT_LEFTPOS,COUNT_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,hDCMem,0,NUM_BITMAP_HEIGHT*(bombCount<0?0:11-(int)floor(bombCount/100)),SRCCOPY);
		BitBlt(hDC,COUNT_LEFTPOS+NUM_BITMAP_WIDTH,COUNT_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,hDCMem,0,NUM_BITMAP_HEIGHT*(11-(int)floor((abs(bombCount)-(int)floor(abs(bombCount)/100)*100)/10)),SRCCOPY);
		BitBlt(hDC,COUNT_LEFTPOS+NUM_BITMAP_WIDTH*2,COUNT_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,hDCMem,0,NUM_BITMAP_HEIGHT*(11-(abs(bombCount)-(int)floor(abs(bombCount)/10)*10)),SRCCOPY);
		/*///////////////////////////////////////////////////////////////////////////
		//��ʱ����ͼ
		//
		*////////////////////////////////////////////////////////////////////////////
		BitBlt(hDC,rcWnd.right-TIME_RIGHTSTND-TIME_WIDTH,TIME_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,hDCMem,0,NUM_BITMAP_HEIGHT*(11-(timeCount<1000?(int)floor(timeCount/100):9)),SRCCOPY);
		BitBlt(hDC,rcWnd.right-TIME_RIGHTSTND-TIME_WIDTH+NUM_BITMAP_WIDTH,TIME_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,hDCMem,0,NUM_BITMAP_HEIGHT*(11-(timeCount<1000?(int)floor((timeCount-(int)floor(timeCount/100)*100)/10):9)),SRCCOPY);
		BitBlt(hDC,rcWnd.right-TIME_RIGHTSTND-TIME_WIDTH+NUM_BITMAP_WIDTH*2,TIME_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,hDCMem,0,NUM_BITMAP_HEIGHT*(11-(timeCount<1000?(timeCount-(int)floor(timeCount/10)*10):9)),SRCCOPY);
		DeleteObject(hBmp);	
		//����߿����
		rcRefresh=CreateRect(MAIN_LEFTSTND-LINE_LIGHTEN_THICK,MAIN_TOPSTND-LINE_LIGHTEN_THICK,rcWnd.right-MAIN_RIGHTSTND+LINE_LIGHTEN_THICK,rcWnd.bottom-MAIN_BOTTOMSTND+LINE_LIGHTEN_THICK);
		DrawSolidFrame(hDC,rcRefresh,LINE_LIGHTEN_THICK);

		rcRefresh=CreateRect(FUN_LEFTPOS-LINE_LIGHTEN_THICK,FUN_TOPPOS-LINE_LIGHTEN_THICK,rcWnd.right-FUN_RIGHTSTND+LINE_LIGHTEN_THICK,MAIN_TOPSTND-FUN_BOTTOMSTND+LINE_LIGHTEN_THICK);
		DrawSolidFrame(hDC,rcRefresh,LINE_LIGHTEN_NORMAL);
		
		rcRefresh=CreateRect(COUNT_LEFTPOS-LINE_LIGHTEN_THIN,COUNT_TOPPOS-LINE_LIGHTEN_THIN,COUNT_RIGHTPOS+LINE_LIGHTEN_THIN,COUNT_BOTTOMPOS+LINE_LIGHTEN_THIN);
		DrawSolidFrame(hDC,rcRefresh,LINE_LIGHTEN_THIN);

		rcRefresh=CreateRect(rcWnd.right-TIME_RIGHTSTND-TIME_WIDTH-LINE_LIGHTEN_THIN,TIME_TOPPOS-LINE_LIGHTEN_THIN,rcWnd.right-TIME_RIGHTSTND+LINE_LIGHTEN_THIN,TIME_TOPPOS+TIME_HEIGHT+LINE_LIGHTEN_THIN);
		DrawSolidFrame(hDC,rcRefresh,LINE_LIGHTEN_THIN);

		//������ͼ
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
			case IDM_BEGIN:				//��ʼ
				//��ʼ������Ϣ
				IinitializeMapInfo(hWnd,(flagState & 0x0f00)/0x0100,mineRowNum,mineLineNum,mineBomb,TRUE,FALSE,FACE_TYPE_SMILE_POP); //??
				//���������ͼ
				CreateRandomMap(hWnd);
				//ˢ�´���
				InvalidateRect(hWnd,NULL,TRUE);
				break;
			case IDM_BEGINNER:			//����
				//��ʼ������Ϣ
				IinitializeMapInfo(hWnd,0x0000,BEGINER_COL_NUM,BEGINER_LN_NUM,BEGINER_BOMB_NUM,TRUE,FALSE,FACE_TYPE_SMILE_POP); //??
				//��ʼ������Ϣ�õ����ڴ�С
				rcWnd=getLevelWindowSize(hWnd);
				//���������ͼ
				CreateRandomMap(hWnd);
				//�ı䴰�ڴ�С
				MoveWindow(hWnd,rcWnd.left,rcWnd.top,rcWnd.right,rcWnd.bottom,TRUE);
				//���õȼ��˵�
				OnCheckLevelMenu(hMenu,LEVEL_BEGINNER);
				break;
			case IDM_INTERMEDIATE:		//�м�
				IinitializeMapInfo(hWnd,0x0001,INTER_COL_NUM,INTER_LN_NUM,INTER_BOMB_NUM,TRUE,FALSE,FACE_TYPE_SMILE_POP); //??
				rcWnd=getLevelWindowSize(hWnd);
				CreateRandomMap(hWnd);
				MoveWindow(hWnd,rcWnd.left,rcWnd.top,rcWnd.right,rcWnd.bottom,TRUE);
				OnCheckLevelMenu(hMenu,LEVEL_INTER);			
				break;
			case IDM_ADVANCED:			//�߼�
				IinitializeMapInfo(hWnd,0x0002,ADVANCED_COL_NUM,ADVANCED_LN_NUM,ADVANCED_BOMB_NUM,TRUE,FALSE,FACE_TYPE_SMILE_POP); //??
				rcWnd=getLevelWindowSize(hWnd);
				CreateRandomMap(hWnd);
				MoveWindow(hWnd,rcWnd.left,rcWnd.top,rcWnd.right,rcWnd.bottom,TRUE);
				OnCheckLevelMenu(hMenu,LEVEL_ADVANCED);
				break;
			case IDM_CUSTOMDEFINE:		//�Զ���ȼ�
				//��ʾ�Զ��崰��
				if (DialogBox(hInstance,MAKEINTRESOURCE(IDD_CUSTOMDLG),hWnd,WinDlgProc))
				{
					//��������� TRUE,��"ȷ��"
					IinitializeMapInfo(hWnd,0x0003,mineRowNum,mineLineNum,mineBomb,TRUE,FALSE,FACE_TYPE_SMILE_POP); //??
					rcWnd=getLevelWindowSize(hWnd);
					CreateRandomMap(hWnd);
					MoveWindow(hWnd,rcWnd.left,rcWnd.top,rcWnd.right,rcWnd.bottom,TRUE);
					OnCheckLevelMenu(hMenu,LEVEL_CUSTOM);
				}
				break;
			case IDM_MARK:				//���ñ�� flagState & FLAG_MARK == FLAG_MARK Ϊ��� 
				flagState=(((flagState&0x0008)==FLAG_MARK)?0:0x0008)+(flagState&0xfff7);
				CheckMenuItem(hMenu,IDM_MARK,((flagState&0x0008)==FLAG_MARK) ? MF_CHECKED:MF_UNCHECKED);
				break;
			case IDM_COLOR:				//������ɫ	flagState & FLAG_COLOR == FLAG_COLOR Ϊ��ɫ
				flagState=(((flagState&0x0004)==FLAG_COLOR)?0:0x0004)+(flagState&0xfffb);
				CheckMenuItem(hMenu,IDM_COLOR,((flagState&0x0004)==FLAG_COLOR) ? MF_CHECKED:MF_UNCHECKED);
				InvalidateRect(hWnd,NULL,TRUE);
				break;
			case IDM_SOUND:				//��������	flagState & FLAG_SOUND == FLAG_SOUND Ϊ����
				flagState=(((flagState&0x0010)==FLAG_SOUND)?0:0x0010)+(flagState&0xffef);
				CheckMenuItem(hMenu,IDM_SOUND,((flagState&0x0010)==FLAG_SOUND) ? MF_CHECKED:MF_UNCHECKED);
				break;
			case IDM_HEROLIST:			//��ʾ���а�
				DialogBox(hInstance,MAKEINTRESOURCE(IDD_HEROLISTDLG),hWnd,WinDlgProc);
				break;
			case IDM_EXIT:				//�˳�
				SendMessage(hWnd,WM_DESTROY,wParam,lParam);
				break;
			case IDM_ABOUT:				//����
				//load�ַ�����Դ
				LoadString(hInstance,IDS_AUTHER,lpAnonymity,sizeof(lpAnonymity));
				//����shell32.dll��ShellAbout����
				ShellAbout(hWnd,TEXT("ɨ�� ..."),lpAnonymity,hIcon);
				break;
			case IDM_CONTENTS:			//������Ŀ¼
				//��ϵͳ�İ���������htmlhelp api����
				LoadString(hInstance,IDS_HELPFILEPATH_WIN,lpAnonymity,sizeof(lpAnonymity));
				if (HtmlHelp(NULL,lpAnonymity,HH_DISPLAY_TOC,NULL)==0)
				{	//ϵͳ���������ڣ��ڵ�ǰĿ¼Ѱ��
					GetCurrentDirectory(sizeof(lpAnonymity),lpAnonymity);
					strcat(lpAnonymity,TEXT("\\help\\winmine.chm"));
					if (HtmlHelp(NULL,lpAnonymity,HH_DISPLAY_TOC,NULL)==0)
						//��Ȼ�޷��򿪣���ʾ������Ϣ
						MessageBox(hWnd,TEXT("����һ������:\r\nӦ�ó����޷��ҵ������ļ�\"WinMine.chm\"\t\r\n��ȷ���ļ����ڣ��������°�װ����"),TEXT("������Ϣ"),MB_ICONERROR);
				}
				break;
			case IDM_SEARCHHELP:		//����������
				LoadString(hInstance,IDS_HELPFILEPATH_WIN,lpAnonymity,sizeof(lpAnonymity));
				if (HtmlHelp(NULL, lpAnonymity, HH_DISPLAY_INDEX,0)==0)
				{
					GetCurrentDirectory(sizeof(lpAnonymity),lpAnonymity);
					strcat(lpAnonymity,TEXT("\\help\\winmine.chm"));
					if (HtmlHelp(NULL,lpAnonymity,HH_DISPLAY_INDEX,NULL)==0)
						MessageBox(hWnd,TEXT("����һ������:\r\nӦ�ó����޷��ҵ������ļ�\"WinMine.chm\"\t\r\n��ȷ���ļ����ڣ��������°�װ����"),TEXT("������Ϣ"),MB_ICONERROR);
				}				
				break;
			case IDM_USEHELP:			//����: ʹ�ð���
				LoadString(hInstance,IDS_HELPFILEPATH_HELP,lpAnonymity,sizeof(lpAnonymity));
				if (HtmlHelp(NULL, lpAnonymity, HH_DISPLAY_TOC,0)==0)
				{
					GetCurrentDirectory(sizeof(lpAnonymity),lpAnonymity);
					strcat(lpAnonymity,TEXT("\\help\\nthelp.chm"));
					if (HtmlHelp(NULL,lpAnonymity,HH_DISPLAY_INDEX,NULL)==0)
						MessageBox(hWnd,TEXT("����һ������:\r\nӦ�ó����޷��ҵ������ļ�\"NThelp.chm\"\t\r\n��ȷ���ļ����ڣ��������°�װ����"),TEXT("������Ϣ"),MB_ICONERROR);
				}	
				break;
		}
	
		return 0;
	case WM_LBUTTONDOWN:				//�������
		SetCapture(hWnd);				//�������
		if (flagState&0x0001)
		{
			RefreshSmileFaceBmp(hWnd,FACE_TYPE_EXCLAIM);
		}

		//�õ����λ����Ϣ
		mousePos=getMousePosition(hWnd,hInstance,LOWORD(lParam),HIWORD(lParam),mousePos.mIDbutton);	
		if (wParam&MK_RBUTTON)			//ͬʱ�Ҽ��Ѿ�����
		{
			mousePos.mIDbutton=DEF_DBBUTTONDOWN;	//����˫��������Ϣ
			OnMouseClick(hWnd,hInstance,mousePos,DEF_DBBUTTONDOWN);
		} 
		else							//�Ҽ�û�а���
		{	
			mousePos.mIDbutton=DEF_LBUTTONDOWN;		//�������������Ϣ
			OnMouseClick(hWnd,hInstance,mousePos,DEF_LBUTTONDOWN);
		}
		return 0;
	case WM_LBUTTONUP:					//�������
		ReleaseCapture();				//�ͷ���겶��
		mousePos=getMousePosition(hWnd,hInstance,LOWORD(lParam),HIWORD(lParam),mousePos.mIDbutton);
		//ͬʱ�Ҽ����»������״̬��Ϣ����ʾ��ǰ����˫������
		if (wParam&MK_RBUTTON||mousePos.mIDbutton==DEF_DBBUTTONDOWN)				
		{
			mousePos.mIDbutton=DEF_DBBUTTONUP;	//����˫��������Ϣ�����״̬��Ϊ�������
			OnMouseClick(hWnd,hInstance,mousePos,DEF_DBBUTTONUP);
			if (flagState&0x0001)
			{
				RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_POP);
			}		

		} 
		else	//ͬʱ�Ҽ�û�а��²��������״̬��Ϣ����ʾ��ǰ������˫������״̬
		{	
			if (mousePos.mIDbutton==DEF_DBBUTTONUP) //�������������״̬
			{
				mousePos.mIDbutton=DEF_LBUTTONUP;	//��������������״̬
				return 0;
			}
			OnMouseClick(hWnd,hInstance,mousePos,DEF_LBUTTONUP);	//����������������Ϣ
		}
		return 0;
	case WM_MOUSEMOVE:	//����ƶ�
		mousePos=getMousePosition(hWnd,hInstance,LOWORD(lParam),HIWORD(lParam),mousePos.mIDbutton);
		if (wParam&MK_LBUTTON)	//ͬʱ����Ѿ�����
		{			
			OnMouseClick(hWnd,hInstance,mousePos,DEF_MOUSEMOVING); //��������ƶ���Ϣ
		} 	
		return 0;
	case WM_RBUTTONDOWN:		//����Ҽ�����
		mousePos=getMousePosition(hWnd,hInstance,LOWORD(lParam),HIWORD(lParam),mousePos.mIDbutton);
		if (wParam&MK_LBUTTON)		//ͬʱ����Ѿ�����
		{
			mousePos.mIDbutton=DEF_DBBUTTONDOWN;	//����˫��������Ϣ���������Ϊ˫������״̬
			OnMouseClick(hWnd,hInstance,mousePos,DEF_DBBUTTONDOWN);
		} 
		else						//���û�а���
		{
			OnMouseClick(hWnd,hInstance,mousePos,DEF_RBUTTONDOWN);	//�����Ҽ�������Ϣ
		}
		return 0;
	case WM_RBUTTONUP:			//����Ҽ�����
		mousePos=getMousePosition(hWnd,hInstance,LOWORD(lParam),HIWORD(lParam),mousePos.mIDbutton);
		if (wParam&MK_LBUTTON)	//ͬʱ�������Ѿ�����
		{
			mousePos.mIDbutton=DEF_DBBUTTONUP;		//�������˫��������Ϣ���������״̬Ϊ˫������
			OnMouseClick(hWnd,hInstance,mousePos,DEF_DBBUTTONUP);
			if (flagState&0x0001)
			{
				RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_POP);
			}
		} 
		return 0;
	case WM_IME_NOTIFY:			//���������Ϣ		
		return 0;
	case WM_TIMER:				//��ʱ����Ϣ
		timeCount++;			//ʱ������
		//�õ���ʱ����С��ˢ�¸�����
		GetClientRect(hWnd,&rcWnd);
		rcRefresh=CreateRect(rcWnd.right-TIME_RIGHTSTND-TIME_WIDTH-LINE_LIGHTEN_THIN,TIME_TOPPOS-LINE_LIGHTEN_THIN,rcWnd.right-TIME_RIGHTSTND+LINE_LIGHTEN_THIN,TIME_TOPPOS+TIME_HEIGHT+LINE_LIGHTEN_THIN);
		InvalidateRect(hWnd,&rcRefresh,FALSE);
		if (flagState & 0x0010)		//�����������
		{
			hrWare=FindResource(hInstance,MAKEINTRESOURCE(IDW_WAVE_TIME),TEXT("WAVE"));
			hGlobal=LoadResource(hInstance,hrWare);
			PlaySoundW((LPCWSTR)LockResource(hGlobal),NULL,SND_MEMORY|SND_ASYNC);
			DeleteObject(hrWare);
			DeleteObject(hGlobal);
		}
		return 0;
	case WM_DESTROY:				//����ر�ʱ
		//дע���û�еļ�ֵͬʱ����
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
		//���а񲿷�
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
//���ܣ��Ի�����Ϣ����
//������
//����ֵ��
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
	case WM_CREATE:			//����
		hInstance=((LPCREATESTRUCT)lParam)->hInstance;	//ʵ�����
		break;
	case WM_INITDIALOG:		//��ʼ��
		//��ʼ�� �Զ���Ի��� Ϊ��ǰ����
		SetDlgItemInt(hDlg,IDC_CUSTOM_HEIGHT,mineRowNum,TRUE);	
		SetDlgItemInt(hDlg,IDC_CUSTOM_WIDTH,mineLineNum,TRUE);
		SetDlgItemInt(hDlg,IDC_CUSTOM_BOMB,mineBomb,TRUE);
		//��ʼ�����а�Ϊע��������ݣ����û�м�ֵ ����Ĭ��
		LoadString(hInstance,IDS_SECOND,lpFormat,sizeof(lpFormat));
		if (RegOpenKeyEx(HKEY_CURRENT_USER,keyWinMine,0,KEY_READ,&hKey)==ERROR_SUCCESS)
		{
			for (i=0;i<3;i++)
			{
				if (RegQueryValueEx(hKey,lpsTime[i],NULL,&dwType1,(LPBYTE)&dwTime,&dwSize1)!=ERROR_SUCCESS)//ʱ��
				{
					dwTime=999;
				}
				sprintf(lpOutput,lpFormat,dwTime);			//��ʽ��
				SetDlgItemText(hDlg,IDC_BEGINER_TIME+i,(LPCSTR)lpOutput);		//��ʾʱ�䵽���а�Ի���	
				if (RegQueryValueEx(hKey,lpsName[i],NULL,&dwType2,(LPBYTE)&lpBuff,&dwSize2)!=ERROR_SUCCESS)//����
				{
					LoadString(hInstance,IDS_NORMAL_NAME,lpBuff,sizeof(lpBuff));
				}			
				SetDlgItemText(hDlg,IDC_BEGINER_NAME+i,(LPCSTR)lpBuff);			//��ʾ���������а�Ի���
			}
			RegCloseKey(hKey);
		}
		//��ʼ�������¼�¼�Ի���
		LoadString(hInstance,IDS_RECORD_BEGINER+(flagState&0x0f00)/0x0100,lpBuff,sizeof(lpBuff));
		SetDlgItemText(hDlg,IDC_RECORD_MSG,lpBuff);
		LoadString(hInstance,IDS_NORMAL_NAME,lpBuff,sizeof(lpBuff));
		SetDlgItemText(hDlg,IDC_RECORD_NAME,lpBuff);
		break;
	case WM_COMMAND:		//�ؼ���Ϣ����
		switch(LOWORD(wParam))
		{
		case IDC_CUSTOM_OK:		//�Զ���ȷ��
			//���ó�������Ϊ�Ի������룬���Խ����Ϊ��ֵ
			width=(GetDlgItemInt(hDlg,IDC_CUSTOM_WIDTH,FALSE,TRUE)<MAX_LN_NUM)?GetDlgItemInt(hDlg,IDC_CUSTOM_WIDTH,FALSE,TRUE):MAX_LN_NUM;
			mineLineNum=width>MIN_LN_NUM?width:MIN_LN_NUM;
			height=GetDlgItemInt(hDlg,IDC_CUSTOM_HEIGHT,FALSE,TRUE)<MAX_COL_NUM?GetDlgItemInt(hDlg,IDC_CUSTOM_HEIGHT,FALSE,TRUE):MAX_COL_NUM;
			mineRowNum=height>MIN_COL_NUM?height:MIN_COL_NUM;
			bomb=GetDlgItemInt(hDlg,IDC_CUSTOM_BOMB,FALSE,TRUE)<MAX_BOMB_NUM?GetDlgItemInt(hDlg,IDC_CUSTOM_BOMB,FALSE,TRUE):MAX_BOMB_NUM;
			mineBomb=bomb>MIN_BOMB_NUM?bomb:MIN_BOMB_NUM;
			mineBomb=mineBomb<(mineRowNum*mineLineNum*0.8)?mineBomb:(int)floor(mineRowNum*mineLineNum*0.8);
			EndDialog(hDlg,TRUE);
			break;
		case IDC_CUSTOM_CANCEL:	//�Զ���ȡ�����������ر�
			EndDialog(hDlg,FALSE);
			break;
		case IDC_HEROLIST_OK:	//���а�ȷ�����������ر�
			EndDialog(hDlg,TRUE);
			break;
		case IDC_HEROLIST_RENEW:	//���а����ã���д��ע���
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
		case IDC_RECORD_OK:			//�����¼�¼ȷ��
			GetDlgItemText(hDlg,IDC_RECORD_NAME,lpBuff,sizeof(lpBuff));
			if (strlen(lpBuff)>0)	//���벻Ϊ��
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

		return FALSE;				//���������Ϣ����FALSE
}

/*###########################################################
���ܣ���ʼ����ͼ������Ϣ
���������ھ�����ȼ������ߡ��ס���־����ʱ�������־�������־
����ֵ���ɹ� ����TRUE
###########################################################*/

BOOL IinitializeMapInfo(HWND hWnd,int level,int col,int ln,int bomb,int playFlag,int timeFlag,int faceFlag){
	//Խ����Ϊ��ֵ
	mineRowNum=(col<MAX_COL_NUM) ? col:MAX_COL_NUM;
	mineRowNum=(col>MIN_COL_NUM) ? col:MIN_COL_NUM;
	mineLineNum=(ln<MAX_LN_NUM) ? ln:MAX_LN_NUM;
	mineLineNum=(ln>MIN_LN_NUM) ? ln:MIN_LN_NUM;
	mineBomb=(bomb<MAX_BOMB_NUM) ? bomb:MAX_BOMB_NUM;
	mineBomb=(bomb>MIN_BOMB_NUM) ? bomb:MIN_BOMB_NUM;
	//���ñ�־λ
	flagState=(flagState & 0xf0ff)+level*0x0100;
	flagState=(flagState & 0xfffe)+playFlag*0x0001;
	flagState=(flagState & 0xfffd)+timeFlag*0x0002;
	flagState=(flagState & 0x0fff)+faceFlag*0x1000;
	bombCount=bomb;
	timeCount=0;
	openCount=mineRowNum*mineLineNum-mineBomb;
	//ֹͣ��ǰ���еļ�ʱ��
	KillTimer(hWnd,FLAG_TIMECOUNT);
	return TRUE; 
}



/*###########################################################
//���ܣ����������ͼ
//���������ھ��
//����ֵ��
//###########################################################*/
BOOL CreateRandomMap(HWND hWnd){
	btLandmineMapinfo[0][0]=(BYTE)0;
	DWORD ixMine,iyMine,iMineBomb,icBomb=0,x,y;
	RECT rcMine;
	//��ʼΪ0
	for(int i=0;i<mineRowNum;i++)
	{
		for(int j=0;j<mineLineNum;j++) btLandmineMapinfo[i][j]=(BYTE)0;
	}
	//��4����
	ixMine=(int)floor(mineRowNum/2);
	iyMine=(int)floor(mineLineNum/2);
	iMineBomb=(int)floor(mineBomb/4);
    srand((unsigned)time(NULL));		//�õ�ʱ�����
	//��������
	while (icBomb<iMineBomb) {
		x=rand()%ixMine;
		y=rand()%iyMine;
		if (btLandmineMapinfo[x][y]!=0x10) {
			btLandmineMapinfo[x][y]=(BYTE)0x10;
			icBomb++;
		}
	 
	}
	icBomb=0;
	//��������
	while (icBomb<iMineBomb) {
		x=rand()%(mineRowNum-ixMine)+ixMine;
		y=rand()%iyMine;
		if (btLandmineMapinfo[x][y]!=0x10) {
			btLandmineMapinfo[x][y]=(BYTE)0x10;
			icBomb++;
		}
	}
	icBomb=0;
	//��������
	while (icBomb<iMineBomb) {
		x=rand()%ixMine;
		y=rand()%(mineLineNum-iyMine)+iyMine;
		if (btLandmineMapinfo[x][y]!=0x10) {
			btLandmineMapinfo[x][y]=(BYTE)0x10;
			icBomb++;
		}
	}
	icBomb=0;
	//��������
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
���ܣ��õ�����λ�ú�״̬
���������ھ����ʵ�������x���ꡢy���ꡢ��ǰ����
����ֵ����ǰ������Ϣ�ṹ��
###########################################################*/

tagMouseState getMousePosition(HWND hWnd,HINSTANCE hInstance,int cxMousePos,int cyMousePos,int mouseType){
	tagMouseState state;
	//�������õ�������λ��
	if (cxMousePos>MAIN_LEFTSTND&&cxMousePos<(MAIN_LEFTSTND+mineRowNum*MINE_BITMAP_WIDTH)&&cyMousePos>MAIN_TOPSTND&&cyMousePos<(MAIN_TOPSTND+mineLineNum*MINE_BITMAP_HEIGHT))
	{	
		state.mXpos=(int)floor((cxMousePos-MAIN_LEFTSTND)/MINE_BITMAP_WIDTH);
		state.mYpos=(int)floor((cyMousePos-MAIN_TOPSTND)/MINE_BITMAP_HEIGHT);
		state.mIDpos=MOUSE_ON_MINE;
	} //������
	else if (cxMousePos>=(MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2)
			&&cxMousePos<=(MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2+FACE_BITMAP_WIDTH)
			&&cyMousePos>=((MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2)
			&&cyMousePos<=((MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2+FACE_BITMAP_HEIGHT)) 
	{
		state.mIDpos=MOUSE_ON_FACE;
	}else//��������
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
���ܣ���������ı��ͼ�Ϸ������Ϣ
���������ھ����ʵ������������Ϣ
����ֵ��
###########################################################*/

BOOL OnChangeMineInfo(HWND hWnd,HINSTANCE hInstance,tagMouseState mouse){
	tagMouseState lastMouse;	//�ϴε������Ϣ
	RECT rcRefresh=CreateRect(mouse.mXpos,mouse.mYpos,0,0);	//ˢ������

	if (mouse.mXpos<0||mouse.mYpos<0||mouse.mXpos>=mineRowNum||mouse.mYpos>=mineLineNum){ //��������������
		return FALSE;
	}
	if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]>=0x20)		//����0x20����ԭ
	{
		OnChangeMineUpon(mouse);
	}
	//���ӻ������֣��ѷ������׵ȣ�������
	if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10==0x01||btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10>0x02)
	{
		return FALSE;
	}
	//�׻�����+?
	if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x10||btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x12) 
	{	
		btLandmineMapinfo[mouse.mXpos][mouse.mYpos]=(BYTE)0x13;		//		����Ϊ��ը����
		setGameLost(hWnd,hInstance);								//		��Ϸʧ��
	}
	//������
	else if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x00||btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x02)
	{
		btLandmineMapinfo[mouse.mXpos][mouse.mYpos]=0x0f-getRoundMineNum(hWnd,mouse.mXpos,mouse.mYpos);//�õ���Χ������
		openCount--;//�ո�������
		if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x0f) //�����0
		{
			lastMouse=mouse;
			for (int i=0;i<3;i++)
			{
				for (int j=0;j<3;j++)
				{	
					lastMouse.mXpos=mouse.mXpos-1+i;
					lastMouse.mYpos=mouse.mYpos-1+j;
					OnChangeMineInfo(hWnd,hInstance,lastMouse); //�ݹ�չ����Χ��ֱ�����ֱ߽�
				}
			}
		}
	}	
	InvalidateRect(hWnd,&rcRefresh,TRUE);	
	return TRUE;
}

/*###########################################################
���ܣ���Ϸʧ��
���������ھ����ʵ�����
����ֵ��
###########################################################*/

BOOL setGameLost(HWND hWnd,HINSTANCE hInstance){
	RECT rcRefrush=CreateRect(MAIN_LEFTSTND,MAIN_TOPSTND,1000,1000);
	KillTimer(hWnd,FLAG_TIMECOUNT);	//������ʱ��
	if ((flagState&0x0010)!=0x0000)	// ��������
	{
		HRSRC hrWare=FindResource(hInstance,MAKEINTRESOURCE(IDW_WAVE_BOMB),TEXT("WAVE"));
		HGLOBAL hGlobal=LoadResource(hInstance,hrWare);
		PlaySoundW((LPCWSTR)LockResource(hGlobal),NULL,SND_MEMORY|SND_ASYNC);
	}

	for(int i=0;i<mineRowNum;i++)	//չ��������
	{
		for(int j=0;j<mineLineNum;j++)
		{
			if (btLandmineMapinfo[i][j]==0x10) 
			{				
				btLandmineMapinfo[i][j]=0x15;	//δ������
			}
			if (btLandmineMapinfo[i][j]==0x01)
			{
				btLandmineMapinfo[i][j]=0x04;	//��������
			}
		}
	}
	flagState=flagState&0xfffe;
	InvalidateRect(hWnd,&rcRefrush,TRUE);	
	RefreshSmileFaceBmp(hWnd,FACE_TYPE_GRIEF);
	return TRUE;
}

/*###########################################################
���ܣ���õ�ǰ�ȼ����ڵ�λ�úʹ�С��Ϣ
���������ھ��
����ֵ������ ���ϵ����꣨left��top����С��right��bottom��
###########################################################*/

RECT getLevelWindowSize(HWND hWnd){
	RECT rcFrame,rcWindow,rcClient;
	GetWindowRect(hWnd,&rcWindow);
	GetClientRect(hWnd,&rcClient);
	rcFrame.left=rcWindow.left;		//�������λ��
	rcFrame.top=rcWindow.top;		//�����ϱ�λ��
		//���ڵĿ��
	rcFrame.right=rcClient.left-rcWindow.left+rcWindow.right-rcClient.right+mineRowNum*MINE_BITMAP_WIDTH+MAIN_LEFTSTND+MAIN_RIGHTSTND;
		//���ڵĸ߶�
	rcFrame.bottom=rcClient.top-rcWindow.top+rcWindow.bottom-rcClient.bottom+mineLineNum*MINE_BITMAP_HEIGHT+MAIN_TOPSTND+MAIN_BOTTOMSTND;
	return rcFrame;
}

/*###########################################################
���ܣ����3D�߿�
������HDC�������߿�
����ֵ��
###########################################################*/

BOOL DrawSolidFrame(HDC hDC,RECT rcFrame,int iLighten){
	HPEN hPen;

	for(int iCount=0;iCount<iLighten;iCount++){	
		hPen=CreatePen(PS_SOLID,0,RGB(250,250,250));
		SelectObject(hDC,hPen);
		MoveToEx(hDC,rcFrame.right-iCount-1,rcFrame.top+iCount,NULL);	//����
		LineTo(hDC,rcFrame.right-iCount-1,rcFrame.bottom-iCount-1);		//����
		LineTo(hDC,rcFrame.left+iCount,rcFrame.bottom-iCount-1);		//����
		DeleteObject(hPen);
		hPen=CreatePen(PS_SOLID,0,RGB(8+(flagState&0x0004)*30,8+(flagState&0x0004)*30,8+(flagState&0x0004)*30));
		SelectObject(hDC,hPen);
		LineTo(hDC,rcFrame.left+iCount,rcFrame.top+iCount);				//����
		LineTo(hDC,rcFrame.right-iCount,rcFrame.top+iCount);			//����
		DeleteObject(hPen);
	}
	return TRUE;
}

/*###########################################################
���ܣ�	����һ�����������������������Ϊ0���õ�����һ���׵Ĵ�С�����򣬷������������������
������		
����ֵ��
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
���ܣ������Χ�׵���Ŀ
���������ھ��������λ��
����ֵ����Χ�׵���Ŀ
###########################################################*/
BYTE getRoundMineNum(HWND hWnd,int iCol,int iLn){
	BYTE btNum=0;
	for(int i=iCol-1;i<=iCol+1;i++) {
		for(int j=iLn-1;j<=iLn+1;j++){
			//����0x10Ϊ��
			if (btLandmineMapinfo[i][j]>=0x10&&i>=0&&j>=0&&i<mineRowNum&&j<mineLineNum) {
				btNum++;
			}
		}
	}
	return btNum;
	
}

/*###########################################################
���ܣ������Χ���ӵ���Ŀ
���������ھ��������λ��
����ֵ����Χ���ӵ���Ŀ
###########################################################*/
BYTE getRoundFlagNum(HWND hWnd,int iCol,int iLn){
	BYTE btNum=0;
	for(int i=iCol-1;i<=iCol+1;i++) {
		for(int j=iLn-1;j<=iLn+1;j++){
			//btLandmineMapinfo[i][j]%0x10==0x01 Ϊ��ǳ�����
			if (btLandmineMapinfo[i][j]%0x10==0x01&&i>=0&&j>=0&&i<mineRowNum&&j<mineLineNum) {
				btNum++;
			}
		}
	}
	return btNum;	
}

/*###########################################################
���ܣ�ˢ�±���ͼƬ����
���������ھ������������
����ֵ��
###########################################################*/

BOOL RefreshSmileFaceBmp(HWND hWnd,int iFaceType){
	RECT rcRefresh=CreateRect(MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2,(MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2,MAIN_LEFTSTND+(mineRowNum*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2+FACE_BITMAP_WIDTH,(MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2+FACE_BITMAP_HEIGHT);
	flagState = (flagState&0x0fff)+iFaceType*0x1000;
	InvalidateRect(hWnd,&rcRefresh,TRUE);
	return TRUE;
}

/*###########################################################
���ܣ������Ϣ����
���������ھ����ʵ���������ǰ���ṹ�塢�Զ�����Ϣ����
����ֵ��
###########################################################*/

BOOL OnMouseClick(HWND hWnd,HINSTANCE hInstance,tagMouseState mouse,int nIDMessage){
	static	tagMouseState lastMouse;	//ǰһ�ε����ṹ��
	tagMouseState tempMouse;			//��ʱ���ṹ��
	TCHAR lpMessage[255];				//��Ϣ�ַ���
	int i,j;							//��ʱ����
	BYTE temp;							//��ʱ����
	RECT rcRefresh;						//ˢ������
	switch(mouse.mIDpos)
	{
	case MOUSE_ON_MINE:					//���������
		if ((!(flagState & FLAG_ALREADPLAYED)))
			{
				return 0;	//��Ϸ��������δ��ʼ������
			}
		switch(nIDMessage)
		{
		case DEF_LBUTTONDOWN:				//�������
			if (OnChangeMineDown(mouse))	//�ı��ͼ�����ݣ�ʹ֮��ʾΪ���µ�
			{
				rcRefresh=CreateRect(mouse.mXpos,mouse.mYpos,0,0);
				InvalidateRect(hWnd,&rcRefresh,TRUE);
			}
			lastMouse=mouse;				//��¼�ϴ�Ϊ��ǰ
			break;
		case DEF_LBUTTONUP:					//��굯��
			if (lastMouse.mIDpos!=MOUSE_ON_MINE)
			{
				KillTimer(hWnd,FLAG_TIMECOUNT);
				flagState=flagState&0xfffe;
			}
			if (mouse.mIDpos!=MOUSE_ON_MINE)
			{
				return 0;
			}
			if (!(flagState & 0x0002))		//δ��ʼ��ʱ
			{
				if (SetTimer(hWnd,FLAG_TIMECOUNT,1000,NULL)==0) //��ʱ������ʧ��
				{
					LoadString(hInstance,IDS_TIMER_ERROR,lpMessage,sizeof(lpMessage));
					if (MessageBox(hWnd,lpMessage,"���ش���!",MB_ICONSTOP)==IDOK)
					{
						SendMessage(hWnd,WM_DESTROY,NULL,NULL); //�˳�����
					}
				}
					if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]>=0x20) //����0x20 ��ԭ
				{
					OnChangeMineUpon(mouse);
				}//��һ������
				if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x10||btLandmineMapinfo[mouse.mXpos][mouse.mYpos]==0x12)
				{
					i=0;
					do{	
						if (mouse.mYpos+i>=mineRowNum||mouse.mXpos+i>=mineLineNum) //�������ܽ�����������ɵ�ͼ
						{
							CreateRandomMap(hWnd);
							i=0;
						}//����λ��
						temp=btLandmineMapinfo[mineRowNum-mouse.mXpos-i][mineLineNum-mouse.mYpos-i];
						btLandmineMapinfo[mineRowNum-mouse.mXpos-i][mineLineNum-mouse.mYpos-i]=btLandmineMapinfo[mouse.mXpos][mouse.mYpos];
						btLandmineMapinfo[mouse.mXpos][mouse.mYpos]=temp;
						i++;
					}while (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]>=0x10);
					
				}
				flagState = (flagState & 0xfffd)+0x0002; //��ʼ��ʱ��
			}
			lastMouse=mouse;
			RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_POP);
			OnChangeMineInfo(hWnd,hInstance,mouse);			//�ı��ͼ��Ϣ
			break;
		case DEF_RBUTTONDOWN:			//�����Ҽ�������Ϣ
			if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10>0x02)//�Ѿ�����������
			{
				break;
			}
			if ((btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10)<0x01+(flagState & 0x0008)/0x0008) 
			{
				btLandmineMapinfo[mouse.mXpos][mouse.mYpos]++;//�����ǣ�δ���� -������-���ʺţ��������ǣ�δ���� -������
			}
			else if ((btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10)==0x01+(flagState & 0x0008)/0x0008)
			{
				btLandmineMapinfo[mouse.mXpos][mouse.mYpos]-=(1+(flagState & 0x0008)/0x0008);//����Ϊδ������״̬
			}else if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10==0x02&&(flagState & 0x0008)==0)
			{
				btLandmineMapinfo[mouse.mXpos][mouse.mYpos]-=2;
				bombCount--;
			}
			//���ף��ճ����Ķ�Ԫ�����������������
			bombCount=bombCount-((btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10)%2)*(0x01+(((flagState & 0x0008)/0x0008+0x01)%0x02))+(btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10)/0x02+((flagState & 0x0008)/0x0008+0x01)%0x02;
			rcRefresh=CreateRect(mouse.mXpos,mouse.mYpos,0,0);
			InvalidateRect(hWnd,&rcRefresh,TRUE);
			rcRefresh=CreateRect(COUNT_LEFTPOS,COUNT_TOPPOS,COUNT_RIGHTPOS,COUNT_BOTTOMPOS);
			InvalidateRect(hWnd,&rcRefresh,TRUE);
		    break;
		case DEF_DBBUTTONDOWN:			//˫������
			for (i=0;i<3;i++)			//����Χ��9������������µ���Ϣ
			{
				for (j=0;j<3;j++)
				{
					//����Խ��
					tempMouse.mXpos=(mouse.mXpos-1+i>=0)?mouse.mXpos-1+i:0;
					tempMouse.mXpos=(mouse.mXpos-1+i<mineRowNum)?tempMouse.mXpos:mineRowNum;					
					tempMouse.mYpos=mouse.mYpos-1+j>=0?mouse.mYpos-1+j:0;
					tempMouse.mYpos=(mouse.mYpos-1+j<mineLineNum)?tempMouse.mYpos:mineRowNum;
					tempMouse.mIDpos=MOUSE_ON_MINE;
					tempMouse.mIDbutton=DEF_LBUTTONDOWN;
					OnMouseClick(hWnd,hInstance,tempMouse,DEF_LBUTTONDOWN);  //������´���
				}				
			}
			lastMouse=mouse;		//��¼֮ǰΪ��ǰ
			break;
		case DEF_DBBUTTONUP:		//˫�����𣬶���Χ��9������������µĻ�ԭ
			for (i=0;i<3;i++)			
			{
				for (j=0;j<3;j++)
				{
					tempMouse.mXpos=(mouse.mXpos-1+i>=0)?mouse.mXpos-1+i:0;
					tempMouse.mXpos=(mouse.mXpos-1+i<mineRowNum)?tempMouse.mXpos:mineRowNum;					
					tempMouse.mYpos=mouse.mYpos-1+j>=0?mouse.mYpos-1+j:0;
					tempMouse.mYpos=(mouse.mYpos-1+j<mineLineNum)?tempMouse.mYpos:mineRowNum;
					OnChangeMineUpon(tempMouse);		//��ԭ
					rcRefresh=CreateRect(tempMouse.mXpos,tempMouse.mYpos,0,0);
					InvalidateRect(hWnd,&rcRefresh,TRUE);//ˢ��
				}	
			}

			if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10>6)		//����Ƿ���������
			{
				i=getRoundFlagNum(hWnd,mouse.mXpos,mouse.mYpos);		//�õ�������
				j=getRoundMineNum(hWnd,mouse.mXpos,mouse.mYpos);		//�õ�����
				if (i==j)	//�������
				{
					for (int i=0;i<3;i++)								//������Χ9�񣨻�ݹ飩
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

			lastMouse=mouse;			//��¼֮ǰΪ��ǰ
			break;	
		case DEF_MOUSEMOVING:			//����ƶ�
			switch(mouse.mIDbutton)
			{
			case DEF_LBUTTONDOWN:		//��������ƶ�
				if (lastMouse.mXpos!=mouse.mXpos||lastMouse.mYpos!=mouse.mYpos)		//λ�ñ仯
				{
					//֮ǰλ������
					if (lastMouse.mXpos>=0&&lastMouse.mYpos>=0&&lastMouse.mXpos<mineRowNum&&lastMouse.mYpos<mineLineNum)
					{
						if (btLandmineMapinfo[lastMouse.mXpos][lastMouse.mYpos]>=0x20) //������
						{
							OnChangeMineUpon(lastMouse);								//��ԭ
						}

					}
					rcRefresh=CreateRect(lastMouse.mXpos,lastMouse.mYpos,0,0);
					InvalidateRect(hWnd,&rcRefresh,TRUE);
					if (mouse.mIDpos==MOUSE_ON_MINE)									//����������״̬
					{
						if (OnChangeMineDown(mouse))									//ʹ֮˫������
						{
							rcRefresh=CreateRect(mouse.mXpos,mouse.mYpos,0,0);			
							InvalidateRect(hWnd,&rcRefresh,TRUE);
						}
						lastMouse=mouse;
					 }

				}
				break;
			case DEF_DBBUTTONDOWN:														//˫�������ƶ�
				if (lastMouse.mXpos!=mouse.mXpos||lastMouse.mYpos!=mouse.mYpos)			//λ�øı���
				{
					if (lastMouse.mIDpos==MOUSE_ON_MINE)								//֮ǰλ������
					{
						OnMouseClick(hWnd,hInstance,lastMouse,DEF_DBBUTTONUP);			//��֮ǰ˫��������Ϣ����
					}
					if (mouse.mXpos>=0&&mouse.mYpos>=0)									
					{
						OnMouseClick(hWnd,hInstance,mouse,DEF_DBBUTTONDOWN);			//�Ե�ǰ˫�����´���
					}
					lastMouse=mouse;   //��¼֮ǰΪ��ǰ
				}
			    break;
			}

			break;
		}
		break;
	case MOUSE_ON_FACE:				//λ�ڱ�������
		switch(nIDMessage)
		{
		case DEF_LBUTTONDOWN:		//�������
			RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_PUSH);			//Ц������
			lastMouse=mouse;
			break;
		case DEF_LBUTTONUP:			//�������
			if (flagState&0x0001==0x0000)			//��Ϸδ��ʼ��ʼ
			{
				flagState=(flagState&0xfffe)+0x0001;
			}
			//��ʼ����
			IinitializeMapInfo(hWnd,(flagState&0x0f00)/0x0100,mineRowNum,mineLineNum,mineBomb,TRUE,FALSE,FACE_TYPE_SMILE_POP); //??
			CreateRandomMap(hWnd);
			InvalidateRect(hWnd,NULL,TRUE);
			RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_POP);
			lastMouse=mouse;
			break;
		}
		break;
	case MOUSE_ON_OTHER:							//λ����������
		if (!(flagState & FLAG_ALREADPLAYED))		//δ��ʼ����
			{
				return FALSE;
			}
		switch(nIDMessage)
		{
		case DEF_LBUTTONDOWN:						//�������
			RefreshSmileFaceBmp(hWnd,FACE_TYPE_EXCLAIM);		//��̾����
			break;
		case DEF_LBUTTONUP:							//�������
			lastMouse.mIDpos=MOUSE_ON_MINE;
			OnMouseClick(hWnd,hInstance,mouse,DEF_MOUSEMOVING);		//����������������ƶ���ˢ��֮ǰ
			OnChangeMineUpon(lastMouse);							//��ԭ֮ǰ
			rcRefresh=CreateRect(lastMouse.mXpos,lastMouse.mYpos,0,0);	
			InvalidateRect(hWnd,&rcRefresh,TRUE);
		    RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_POP);
		    break;
		case DEF_RBUTTONDOWN:
		    break;
		case DEF_MOUSEMOVING:	//����ƶ�
			if (lastMouse.mIDpos==MOUSE_ON_FACE)
			{
				RefreshSmileFaceBmp(hWnd,FACE_TYPE_SMILE_POP);
			}
			else if (lastMouse.mIDpos==MOUSE_ON_MINE)
			{		
				mouse.mIDpos=MOUSE_ON_MINE;			//���������ƶ���Ϣ��ˢ��֮ǰ
				OnMouseClick(hWnd,hInstance,mouse,DEF_MOUSEMOVING);
			}
			break;
		}
	    break;
	default:
	    break;
	}

	if (openCount==0)//&&bombCount==0)		//�жϽ������ո�Ϊ0��ʣ�µĶ����ף���Ǻ�Ϊ������ӵģ�
	{
		setGameSucceed(hWnd,hInstance);		//������Ϸ�ɹ�
	}
	return FALSE;
}

/*###########################################################
���ܣ����õȼ��ı��˵���check
�������˵�������ȼ�
����ֵ��
###########################################################*/
BOOL OnCheckLevelMenu(HMENU hMenu,int level){
	CheckMenuItem(hMenu,IDM_ADVANCED,MF_UNCHECKED);//����δ��
	CheckMenuItem(hMenu,IDM_INTERMEDIATE,MF_UNCHECKED);
	CheckMenuItem(hMenu,IDM_BEGINNER,MF_UNCHECKED);
	CheckMenuItem(hMenu,IDM_CUSTOMDEFINE,MF_UNCHECKED);
	CheckMenuItem(hMenu,IDM_BEGINNER+level,MF_CHECKED); //��ѡ��ǰ�ȼ�
	flagState=((flagState&0xf0ff)+level*0x0100);		//�ı�״̬
	return TRUE;
}
/*###########################################################
���ܣ�������Ϸ�ɹ�
������
����ֵ��
###########################################################*/

BOOL setGameSucceed(HWND hWnd,HINSTANCE hInstance){
	KillTimer(hWnd,FLAG_TIMECOUNT);			//��������
	openCount=-1;							//���ÿո����
	RECT rcRefresh;
	
	LPCTSTR			keyWinMine=TEXT("Software\\Microsoft\\winmine\\");
	LONG			lErrorID=0;
	DWORD 			dwSize=sizeof(DWORD);
	HKEY			hKey;
	DWORD			dwTime;
	LPCTSTR			lpsLevel[3]={TEXT("Time1"),TEXT("Time2"),TEXT("Time3")};
	if ((flagState&0x0010)!=0)
	{
		HRSRC			hrWare=FindResource(hInstance,MAKEINTRESOURCE(IDW_WAVE_DONE),TEXT("WAVE")); //����������Դ
		HGLOBAL			hGlobal=LoadResource(hInstance,hrWare);
		PlaySoundW((LPCWSTR)LockResource(hGlobal),NULL,SND_MEMORY|SND_ASYNC);
	}

	flagState=flagState& 0xfffe;				//���ý���״̬��δ��ʱ
	RefreshSmileFaceBmp(hWnd,FACE_TYPE_COOL);	//����-��
	for (int i=0;i<mineRowNum;i++)				//���δ��ǵ���Ϊ����
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
	if ((flagState&0x0f00)>0x0200)	//����Ǽ�Ϊ�Զ��壬���أ����������к�ע���
	{
		return FALSE;
	}
	lErrorID=RegOpenKeyEx(HKEY_CURRENT_USER,keyWinMine,0,KEY_READ,&hKey);//��ȡ��ǰ������Ϣ
	if (lErrorID==ERROR_SUCCESS)
	{
		lErrorID=RegQueryValueEx(hKey,lpsLevel[(flagState&0x0f00)/0x0100],NULL,NULL,(LPBYTE)&dwTime,&dwSize);
		if (lErrorID==ERROR_SUCCESS)
		{
			if (timeCount<(int)dwTime)  //�Ƽ�¼��
			{
				DialogBox(hInstance,MAKEINTRESOURCE(IDD_RECORDDLG),hWnd,WinDlgProc);  //�����¼�¼
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
���ܣ�������Ϣ�Ի���
���������ھ����ʵ���������Ϣ����Ϣid
����ֵ��
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
	MessageBox(hWnd,lpOutput,"����",MB_ICONERROR);
	return TRUE;
}

/*###########################################################
���ܣ��Ƿ�����ʾΪ���µ�״̬
���������λ�ýṹ��
����ֵ��
###########################################################*/

BOOL OnChangeMineDown(tagMouseState mouse){
	if (mouse.mXpos<0||mouse.mXpos>=mineRowNum||mouse.mYpos<0||mouse.mYpos>=mineLineNum) //Խ�磬������
	{
		return FALSE;
	}
	if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10==0x01||btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10>0x02)
	{
		return FALSE;					//������ӻ򷭿��Ĳ�����
	}
	else if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]<0x20)			//���δ��״̬����
	{
		if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10==0)		//δ����״̬
		{
			btLandmineMapinfo[mouse.mXpos][mouse.mYpos]+=0x2f;			//+0x2f ��ʾΪ 0��״̬
		} 
		else if (btLandmineMapinfo[mouse.mXpos][mouse.mYpos]%0x10==0x02)//�ʺ�
		{
			btLandmineMapinfo[mouse.mXpos][mouse.mYpos]+=0x24;			//+0x24 ��ʾΪ���µ��ʺ�
		}
		return TRUE;
	}
	return TRUE;
}

/*###########################################################
���ܣ���ԭ��ʾΪ����״̬�ķ���
���������λ�ýṹ��
����ֵ��
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