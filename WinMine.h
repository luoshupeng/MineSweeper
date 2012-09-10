/**********************����˵��**************************/
//��Ŀ����: ɨ��					           			//
//��Ŀ����: ����WIN32 API�Ĵ���Ӧ�ó���					//
//��    ��: ͬMicrosoft(TM) Windows(TM) "ɨ��" ����		//
//�ļ�����: WinMine.exe									//
//��Ŀ�ļ�: WinMine.h WinProc.h resource.h htmlhelp.h	//
//			WinMine.cpp	WinProc.cpp						//
//�������: winmm.lib comctl32.lib htmlhelp.lib			//
//��    ��:	������		������							//
//��    λ: �й���ѧԺ�о���Ժ							//
//��������: 2006��3��19��								//
//��������: ��������									//
//��ǰ�汾: ��ʽ�汾 v1.0.0.1							//
//����ƽ̨: Windows XP Professional Service Pack 2 ZH-CN//
//��ϵ��ʽ: wangbaochen@gmail.com  TEL:13366036900		//
/********************************************************/


/*//////////////////////�ļ�˵��//////////////////////////
//														//
//�ļ���: WinMine.h					           			//
//��  ��: ȫ�ֱ������ꡢ�������Ͷ���					//
//��  ��:												//
//														//
////////////////////////////////////////////////////////*/

//########################################################
//	�������Ͻ�����(12,55)
//
#define MAIN_LEFTSTND		12
#define MAIN_TOPSTND		55
#define MAIN_RIGHTSTND		8
#define MAIN_BOTTOMSTND		8

//########################################################
//	���������Ͻ�����(12,12)
//
#define FUN_LEFTPOS			12		
#define FUN_TOPPOS			12
#define FUN_RIGHTSTND		8
#define FUN_BOTTOMSTND		12

//########################################################
//	���������Ͻ�����(12,12)
//
#define COUNT_LEFTPOS		17		
#define COUNT_TOPPOS		16
#define COUNT_RIGHTPOS		56
#define COUNT_BOTTOMPOS		39

//########################################################
//	��ʱ��
//		
#define TIME_TOPPOS			16
#define TIME_RIGHTSTND		15
#define TIME_WIDTH			39
#define TIME_HEIGHT			23

//########################################################
//	�������м����߼�
//	����������������
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
//�Զ������״̬
//
#define DEF_LBUTTONUP		1000 //�������
#define DEF_LBUTTONDOWN		1001 //�������
#define DEF_RBUTTONDOWN		2000 //�Ҽ�����
#define DEF_DBBUTTONDOWN	3000 //˫������
#define DEF_DBBUTTONUP		3001
#define DEF_DBBUTTONMOVE	3002
#define DEF_MOUSEMOVING		4000 //����ƶ�

//########################################################
//����ͼƬ 16*16 ����ͼƬ 24*24
//
#define MINE_BITMAP_WIDTH	16
#define MINE_BITMAP_HEIGHT	16
#define FACE_BITMAP_WIDTH	24
#define FACE_BITMAP_HEIGHT	24
#define NUM_BITMAP_WIDTH	13
#define NUM_BITMAP_HEIGHT	23	
//########################################################
//�����߿���
//
#define LINE_LIGHTEN_THICK		3
#define LINE_LIGHTEN_NORMAL		2
#define LINE_LIGHTEN_THIN		1
//########################################################
//���λ��״̬
//
#define MOUSE_ON_MINE		181
#define MOUSE_ON_FACE		182
#define MOUSE_ON_OTHER		183

//########################################################
//�Զ������
//
#define FACE_TYPE_SMILE_PUSH	0x0000
#define FACE_TYPE_COOL			0x0001
#define FACE_TYPE_GRIEF			0x0002
#define FACE_TYPE_EXCLAIM		0x0003
#define FACE_TYPE_SMILE_POP		0x0004		

//########################################################
//�Զ���ȼ�
//
#define LEVEL_BEGINNER			0x0000
#define LEVEL_INTER				0x0001
#define LEVEL_ADVANCED			0x0002	
#define LEVEL_CUSTOM			0x0003

//########################################################
//�Զ���״̬��־
//
#define FLAG_ALREADPLAYED		0x0001
#define FLAG_ALREADTIME			0x0002
#define FLAG_COLOR				0x0004	
#define FLAG_MARK				0x0008
#define FLAG_SOUND				0x0010
#define FLAG_TIMECOUNT			0x1000
//###############################################################
//ȫ�ֱ����������ͼ����Ϣ
//
static BYTE btLandmineMapinfo[MAX_COL_NUM][MAX_LN_NUM];


//###############################################################
//�ṹ�壺�����߼�λ������
//mXpos:	��		mYpos:�� 
//mIDpos:	λ������(MOUSE_ON_MINE MOUSE_ON_FACE MOUSE_ON_OTHER)
//mIDbutton:��Ϣ����(	DEF_LBUTTONUP		//�������
//						DEF_LBUTTONDOWN		//�������
//						DEF_RBUTTONDOWN		//�Ҽ�����
//						DEF_DBBUTTONDOWN	//˫������
//						DEF_DBBUTTONUP		//˫������
//						DEF_MOUSEMOVING		4//����ƶ�	)
struct tagMouseState{int mXpos;int mYpos;int mIDpos;int mIDbutton;};

//###############################################################
//ȫ�ֱ�����
//mineRowNum:	��ȷ������
//mineLineNum:	�߶ȷ������
//mineBomb:		����
//timeCount:	ʱ�����	
//bombCount:	���׼���
//openCount:	�����ո����	
//flagState:	״̬���� -->|--	flagState&0x0001	��Ϸ�Ƿ�ʼ
//							|--	flagState&0x0002	��Ϸ�Ƿ��ʱ
//							|--	flagState&0x0004	�����Ƿ��ɫ
//							|--	flagState&0x0008	�Ƿ����ʺ�(?)
//							|--	flagState&0x0010	�Ƿ񲥷�����
//							|--	flagState&0x0100	��Ϸ�Ǽ�
//							|--	flagState&0x1000	��ǰ�����־
	
static int mineRowNum,mineLineNum,mineBomb,timeCount,bombCount,openCount,flagState;