//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ 生成的包含文件。
// 供 win32项目.rc 使用
//
#define IDC_MYICON                      2
#define IDD_WIN32_DIALOG                102
#define IDS_APP_TITLE                   103
#define IDD_ABOUTBOX                    103
#define IDM_ABOUT                       104
#define IDM_EXIT                        105
#define IDI_WIN32                       107
#define IDI_SMALL                       108
#define IDC_WIN32                       109
#define IDB_BUTTON_LOGIN                111
#define IDB_COMBOX_LOGIN                112
#define IDR_MAINFRAME                   128
#define IDM_NEW                         129
#define IDD_DIALOG1                     132
#define IDB_BaudRate                    133
#define IDD_DIALOG2                     134

#define 奇偶校验						135
#define 停止位							136
#define IDB_PortCtl                     137
#define IDB_ClearData                   138
#define IDE_Receive                     139
#define IDE_Send                        140
#define IDS_ReceiveCount                141
#define IDS_SendCount                   142
#define IDS_BaudRate                    143
#define IDC_COMx                        144
#define IDS_COMx                        145
#define IDB_RedCtl                      146
#define IDB_GreCtl                      147
#define IDB_BluCtl                      148
#define IDB_RGBCtl                      149
#define IDB_LockCtl                     150
#define IDB_BKLigthCtl                  151
#define IDB_LayPower					157
#define IDL_Addr1                       152
#define IDL_Addr2                       153
#define IDL_Addr3                       154
#define IDC_HexRecv                     155
#define IDC_HexSend                     156
#define IDC_LIST1                       1011
#define IDM_Calculator                  1012
#define IDM_ComboT                      1013
#define IDC_RADIO1                      1020
#define IDC_EDIT1                       1023
#define IDS_STATUS						1024	//状态栏
#define IDE_CRC16                       1101
#define IDM_CRC16                       1102
#define IDD_CRC16                       1103
#define IDD_Prol                        1111
#define IDM_Prol                        1112
#define IDE_Prol                        1113
#define IDC_STATIC                      -1



typedef enum eucmd
{
	NoneCmd = 0,    //不支持的命令/空命令
	AMPACK = 1,    //应答类型，只带一个数据，不带地址，数据表示状态
	LED = 2,    //LED控制，带三个数据，带地址，0-表示关，1-表示开
	CTL = 3,    //开锁命令，只带一个数据，带地址，0-表示读锁状态，1表示开锁
	ICR = 4,    //读卡器，带地址，第一个数据为0表示读卡，为1表示写卡，后面跟随卡数据
	PWD = 5,     //层板供电控制
	STA = 6,     //状态：主要为上报状态
	Up = 0x80, //上传标志
	Down = 0x7F  //下发与标志
}eucmddef;

typedef struct _stcmd
{
	unsigned char cmd : 6;   //CMD1~CMD63---低6位  
	unsigned char rw : 1;   //0-write,1-read--高7位
	unsigned char dir : 1;   //0-down,1-up---高8位

}stcmddef;
typedef struct _stampaddr
{
	unsigned char address1;
	unsigned char address2;
	unsigned char address3;
}stampaddrdef;

typedef struct _stampcrc16
{
	unsigned char crcl;
	unsigned char crch;
}stampcrc16def;
typedef struct _stmsg
{
	unsigned char length;   //不包含此位
	stcmddef 			cmd;
	stampaddrdef	addr;
	unsigned char data[128];
}stmsgdef;
typedef struct _stampphy
{
	unsigned char head;
	stmsgdef      msg;
	stampcrc16def crc16;
	unsigned char end;
}stampphydef;



typedef struct _stStatus        //状态：
{
	unsigned short lockstd : 1;		//锁状态：0-开状态，1-锁状态
	unsigned short unlockerr : 1;	//开锁错误/失败:0-无，1-开锁失败
	unsigned short Online : 1;		//设备掉线:0-掉线，1-在线
}stStadef;

unsigned char crccheck(unsigned char* pframe, unsigned short* length);
unsigned char ackcheck(unsigned char* pframe);
// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NO_MFC                     1
#define _APS_NEXT_RESOURCE_VALUE        135
#define _APS_NEXT_COMMAND_VALUE         32787
#define _APS_NEXT_CONTROL_VALUE         1024
#define _APS_NEXT_SYMED_VALUE           110
#endif
#endif
