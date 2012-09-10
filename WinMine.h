/**********************程序说明**************************/
//项目名称: 扫雷					           			//
//项目类型: 基于WIN32 API的窗口应用程序					//
//功    能: 同Microsoft(TM) Windows(TM) "扫雷" 程序		//
//文件名称: WinMine.exe									//
//项目文件: WinMine.h WinProc.h resource.h htmlhelp.h	//
//			WinMine.cpp	WinProc.cpp						//
//引用类库: winmm.lib comctl32.lib htmlhelp.lib			//
//作    者:	王宝臣		张文兴							//
//单    位: 中国科学院研究生院							//
//生成日期: 2006年3月19日								//
//程序语言: 简体中文									//
//当前版本: 正式版本 v1.0.0.1							//
//测试平台: Windows XP Professional Service Pack 2 ZH-CN//
//联系方式: wangbaochen@gmail.com  TEL:13366036900		//
/********************************************************/


/*//////////////////////文件说明//////////////////////////
//														//
//文件名: WinMine.h					           			//
//功  能: 全局变量、宏、数据类型定义					//
//作  者:												//
//														//
////////////////////////////////////////////////////////*/

//########################################################
//	雷区左上角坐标(12,55)
//
#define MAIN_LEFTSTND		12
#define MAIN_TOPSTND		55
#define MAIN_RIGHTSTND		8
#define MAIN_BOTTOMSTND		8

//########################################################
//	功能区左上角坐标(12,12)
//
#define FUN_LEFTPOS			12		
#define FUN_TOPPOS			12
#define FUN_RIGHTSTND		8
#define FUN_BOTTOMSTND		12

//########################################################
//	记雷区左上角坐标(12,12)
//
#define COUNT_LEFTPOS		17		
#define COUNT_TOPPOS		16
#define COUNT_RIGHTPOS		56
#define COUNT_BOTTOMPOS		39

//########################################################
//	计时区
//		
#define TIME_TOPPOS			16
#define TIME_RIGHTSTND		15
#define TIME_WIDTH			39
#define TIME_HEIGHT			23

//########################################################
//	初级、中级、高级
//	行数、列数、雷数
#define BEGINER_COL_NUM		9
#define BEGINER_LN_NUM		9
#define BEGINER_BOMB_NUM	10

#define INTER_COL_NUM		16
#define INTER_LN_NUM		16
#define INTER_BOMB_NUM		40

#define ADVANCED_COL_NUM	30
#define ADVANCED_LN_NUM		16
#define ADVANCED_BOMB_NUM	99

#define MAX_COL_NUM			30
#define MAX_LN_NUM			24
#define MAX_BOMB_NUM		667
#define MIN_COL_NUM			9
#define MIN_LN_NUM			9
#define MIN_BOMB_NUM		10

//########################################################
//自定义鼠标状态
//
#define DEF_LBUTTONUP		1000 //左键弹起
#define DEF_LBUTTONDOWN		1001 //左键按下
#define DEF_RBUTTONDOWN		2000 //右键按下
#define DEF_DBBUTTONDOWN	3000 //双键按下
#define DEF_DBBUTTONUP		3001
#define DEF_DBBUTTONMOVE	3002
#define DEF_MOUSEMOVING		4000 //鼠标移动

//########################################################
//雷区图片 16*16 表情图片 24*24
//
#define MINE_BITMAP_WIDTH	16
#define MINE_BITMAP_HEIGHT	16
#define FACE_BITMAP_WIDTH	24
#define FACE_BITMAP_HEIGHT	24
#define NUM_BITMAP_WIDTH	13
#define NUM_BITMAP_HEIGHT	23	
//########################################################
//立体线框宽度
//
#define LINE_LIGHTEN_THICK		3
#define LINE_LIGHTEN_NORMAL		2
#define LINE_LIGHTEN_THIN		1
//########################################################
//鼠标位置状态
//
#define MOUSE_ON_MINE		181
#define MOUSE_ON_FACE		182
#define MOUSE_ON_OTHER		183

//########################################################
//自定义表情
//
#define FACE_TYPE_SMILE_PUSH	0x0000
#define FACE_TYPE_COOL			0x0001
#define FACE_TYPE_GRIEF			0x0002
#define FACE_TYPE_EXCLAIM		0x0003
#define FACE_TYPE_SMILE_POP		0x0004		

//########################################################
//自定义等级
//
#define LEVEL_BEGINNER			0x0000
#define LEVEL_INTER				0x0001
#define LEVEL_ADVANCED			0x0002	
#define LEVEL_CUSTOM			0x0003

//########################################################
//自定义状态标志
//
#define FLAG_ALREADPLAYED		0x0001
#define FLAG_ALREADTIME			0x0002
#define FLAG_COLOR				0x0004	
#define FLAG_MARK				0x0008
#define FLAG_SOUND				0x0010
#define FLAG_TIMECOUNT			0x1000
//###############################################################
//全局变量：保存地图的信息
//
static BYTE btLandmineMapinfo[MAX_COL_NUM][MAX_LN_NUM];


//###############################################################
//结构体：鼠标的逻辑位置属性
//mXpos:	列		mYpos:行 
//mIDpos:	位置类型(MOUSE_ON_MINE MOUSE_ON_FACE MOUSE_ON_OTHER)
//mIDbutton:消息类型(	DEF_LBUTTONUP		//左键弹起
//						DEF_LBUTTONDOWN		//左键按下
//						DEF_RBUTTONDOWN		//右键按下
//						DEF_DBBUTTONDOWN	//双键按下
//						DEF_DBBUTTONUP		//双键弹起
//						DEF_MOUSEMOVING		4//鼠标移动	)
struct tagMouseState{int mXpos;int mYpos;int mIDpos;int mIDbutton;};

//###############################################################
//全局变量：
//mineRowNum:	宽度方向格数
//mineLineNum:	高度方向格数
//mineBomb:		雷数
//timeCount:	时间计数	
//bombCount:	地雷计数
//openCount:	翻开空格计数	
//flagState:	状态计数 -->|--	flagState&0x0001	游戏是否开始
//							|--	flagState&0x0002	游戏是否计时
//							|--	flagState&0x0004	界面是否彩色
//							|--	flagState&0x0008	是否标记问号(?)
//							|--	flagState&0x0010	是否播放声音
//							|--	flagState&0x0100	游戏登记
//							|--	flagState&0x1000	当前表情标志
	
static int mineRowNum,mineLineNum,mineBomb,timeCount,bombCount,openCount,flagState;