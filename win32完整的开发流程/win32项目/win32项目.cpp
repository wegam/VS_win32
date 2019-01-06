

#include "stdafx.h"
#include "win32项目.h"


#include"stdio.h "

#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS

#define MAX_LOADSTRING 100

// 全局变量: 

unsigned char rxdbuffer[2048] = { 0 };

HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

//--------------------线程串口
//原文：https ://blog.csdn.net/wowocpp/article/details/80594010
DWORD ThreadProcWrite(LPVOID pParam);
DWORD ThreadProcRead(LPVOID pParam);
OVERLAPPED Wol = { 0 };
OVERLAPPED Rol = { 0 };
HANDLE hThreadWrite;
HANDLE hThreadRead;

 


HWND Login_Button;			//登录按钮
HANDLE hCom;
HWND hWnd;
// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WindowProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	AboutCRC16(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	AboutProl(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	SerailConfiguration(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	ClearData(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	UserProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	dislpay(HWND hDlg, unsigned char *p, int len, char RTflag);
unsigned short		CRC16_MODBUS(unsigned char *puchMsg, unsigned int usDataLen);
unsigned char		GetListBoxdData(HWND hWnd, int nIDDlgItem);
unsigned char		CheckListBoxdData(HWND hWnd);
unsigned char		SetEnableWindow(HWND hWnd, int nIDDlgItem);
unsigned char		SetDisableWindow(HWND hWnd, int nIDDlgItem);
int Serial_Init(HWND hDlg,LPCWSTR COMx, int BaudRate)
{
	DCB com_dcb;    //参数设置
	COMMTIMEOUTS tim_out;

	//Robocon_com = CreateFile(COMx, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	hCom = CreateFile(COMx, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL);
	if (hCom == (HANDLE)-1)
	{
		/*printf("Serial open fail\n");*/
		MessageBox(hDlg, TEXT("串口打开失败\n"), MB_OK, TRUE);
		return -1;
	}

	if (GetCommState(hCom, &com_dcb))//获取COM状态，配置参数
	{
		com_dcb.BaudRate			= BaudRate;//波特率
		com_dcb.fBinary				= TRUE;		// 设置二进制模式，此处必须设置TRUE 
		com_dcb.fParity				= FALSE;	// 支持奇偶校验
		com_dcb.fOutxCtsFlow		= FALSE;	// No CTS output flow control
		com_dcb.fOutxDsrFlow		= FALSE;	// No DSR output flow control
		com_dcb.fDtrControl			= DTR_CONTROL_DISABLE;// No DTR flow control
		com_dcb.fDsrSensitivity		= FALSE;	// DSR sensitivity 
		com_dcb.fTXContinueOnXoff	= TRUE;		// XOFF continues Tx
		com_dcb.fOutX				= FALSE;	// No XON/XOFF out flow control
		com_dcb.fInX				= FALSE;	// No XON/XOFF in flow control
		com_dcb.fErrorChar			= FALSE;	// Disable error replacement
		com_dcb.fNull				= FALSE;	// Disable null stripping
		com_dcb.fRtsControl			= RTS_CONTROL_DISABLE;//No RTS flow control
		com_dcb.fAbortOnError		= FALSE;	// 当串口发生错误，并不终止串口读写 
		com_dcb.ByteSize			= 8;		//数据位,范围:4-8 
		com_dcb.Parity				= NOPARITY;	//校验位
		com_dcb.StopBits			= ONESTOPBIT;     //停止位
										   //com_dcb.fBinary = TRUE;
										   //com_dcb.fParity = TRUE;
	}


	if (!SetCommState(hCom, &com_dcb))
	{
		/*printf("Serial set fail\n");*/
		MessageBox(hDlg, TEXT("串口配置失败\n"), MB_OK, TRUE);
		return -1;
	}
	SetupComm(hCom, 1024, 1024);//读写缓冲区

	PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);//清除错误标志

	memset(&tim_out, 0, sizeof(tim_out));
	tim_out.ReadIntervalTimeout			= MAXDWORD;//读写时间间隔设置	
	tim_out.ReadTotalTimeoutMultiplier	= 10;
	tim_out.ReadTotalTimeoutConstant	= 0;
	tim_out.WriteTotalTimeoutMultiplier = 50;
	tim_out.WriteTotalTimeoutConstant	= 2000;
	SetCommTimeouts(hCom, &tim_out);


	CreateEvent(NULL, TRUE, FALSE, NULL);
	CreateEvent(NULL, TRUE, FALSE, NULL);
	SetCommMask(hCom, EV_RXCHAR);	//事件--接收到一个字节
	return 1;
}

//https://blog.csdn.net/rabbitjerry/article/details/75384688
DWORD WINAPI ThreadWrite(LPVOID lpParameter)//进程1
{
	char outputData[100] = { 0x00 };//输出数据缓存
	if (hCom == INVALID_HANDLE_VALUE)
	{
		puts("打开串口失败");
		return 0;
	}
	DWORD strLength = 0;
	while (1)
	{
		for (int i = 0; i<100; i++)
		{
			outputData[i] = 0;
		}
		fgets(outputData, 100, stdin);     // 从控制台输入字符串
		strLength = strlen(outputData);
		printf("string length is %d\n", strLength);              // 打印字符串长度
		WriteFile(hCom, outputData, strLength, &strLength, NULL); // 串口发送字符串
		fflush(stdout);
		PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);            //清空缓冲区
		Sleep(100);
	}
	return 0;
}



DWORD WINAPI ThreadRead(LPVOID lpParameter)
{
	if (hCom == INVALID_HANDLE_VALUE)   //INVALID_HANDLE_VALUE表示出错，会设置GetLastError
	{
		puts("打开串口失败");
		return 0;
	}
	unsigned char getputData[1024] = { 0x00 };//输入数据缓存
									// 利用错误信息来获取进入串口缓冲区数据的字节数

	DWORD dwErrors;     // 错误信息
	COMSTAT Rcs;        // COMSTAT结构通信设备的当前信息
	int Len = 0;
	DWORD length = 512;               //用来接收读取的字节数
	static	char rxdflg = 0;
	static  int  rxdlength = 0;
	static	int	 rxdtime = 0;
	while (1)
	{
		//for (int i = 0; i<1024; i++)
		//{
		//	getputData[i] = 0;
		//}
		ClearCommError(hCom,
			&dwErrors,
			&Rcs);                                // 获取读缓冲区数据长度
		Len = Rcs.cbInQue;
		ReadFile(hCom, getputData, Len, &length, NULL);   //获取字符串
		PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);  //清空缓冲区
		if (Len>0&&Len<512)
		{
			int i = 0;
			rxdflg = 1;
			rxdtime = 0;
			//printf("%s\n", getputData);
			//fflush(stdout);
			if (rxdlength + Len < 1024)
			{
				for (i = 0; i < Len; i++)
				{
					rxdbuffer[rxdlength + i] = getputData[i];
				}
			}
			
			rxdlength += Len;
		}
		else
		{
			if ((rxdtime++>5)&&(rxdflg))
			{
				dislpay(hWnd, rxdbuffer, rxdlength, 2);
				rxdflg = 0;
				rxdlength = 0;
			}			
		}
		Sleep(1);
	}
	return 0;
}

//------------------------串口接收线程
DWORD ThreadProcRead(LPVOID   pParam)
{
	BYTE    myByte[20];
	DWORD    dwRes;
	DWORD    dwRead;

	DWORD   dwRet;

	BOOL bResult;

	memset(myByte, 0, sizeof(myByte));

	Rol.hEvent = CreateEvent(NULL,          //创建Rol的hEvent成员为无信号状态
		TRUE,
		FALSE,
		NULL);

	if (Rol.hEvent == NULL)
	{
		printf("hEvent is Empty\r\n");
		return -1;
	}


	bResult = ReadFile(hCom, //串口句柄
		&myByte,     //存放读取数据
		9,         //要读取的字节数
		NULL,
		&Rol);      //指向创建hCom时的Rol的指针

	if (bResult) {
		printf("success read out A\r\n");
		//在这里加入处理读取数据代码,数据存放在myByte数组中          
	}

	dwRet = GetLastError();

	if (!bResult && (dwRet == ERROR_IO_PENDING)) {

		dwRes = WaitForSingleObject(Rol.hEvent, 5000);   //5秒超时

		switch (dwRes)
		{
		case  WAIT_OBJECT_0:

			if (!GetOverlappedResult(hCom,
				&Rol,
				&dwRead,    //实际读出的字节数
				TRUE))     //TRUE表示直到操作完成函数才返回
			{
				//操作失败,可以使用GetLastError()获取错误信息
			}
			else
			{
				//操作成功完成,数据读取存入myByte中

				//这里加入处理数据的代码

				printf("success read out dwRead = %d,%s\r\n", dwRead, myByte);
			}
			break;
		case   WAIT_TIMEOUT:
			//读操作失败,原因是超时
			printf("读操作失败,原因是超时\r\n");
			break;
		default:
			//这里加入默认处理代码
			break;
		}

	}

	CloseHandle(Rol.hEvent);

	return 0;

}
//------------------------串口发送线程
DWORD ThreadProcWrite(LPVOID   pParam)
{
	BYTE    myByte[10] = "AT\r\n";

	DWORD    dwRes;
	DWORD    dwWrite;


	BOOL bResult;

	Wol.Internal = 0;        //设置OVERLAPPED结构Wol
	Wol.InternalHigh = 0;
	Wol.Offset = 0;
	Wol.OffsetHigh = 0;
	Wol.hEvent = CreateEvent(NULL,          //创建Wol的hEvent成员为无信号状态
		TRUE,
		FALSE,
		NULL);

	if (Wol.hEvent == NULL)
	{
		printf("hEvent 空");
		return -1;
	}

	bResult = WriteFile(hCom,         //串口句柄
		&myByte,     //存放待发送数据
		4,         //欲发送的字节数
		NULL,
		&Wol);       //指向创建hCom时的Wol的指针

	if (bResult)
	{

		printf("send success \r\n");

	}

	if (!bResult) {

		dwRes = WaitForSingleObject(Wol.hEvent, 500);   //5ms超时
		switch (dwRes)
		{
		case   WAIT_OBJECT_0:
			if (!GetOverlappedResult(hCom,
				&Wol,
				&dwWrite,
				TRUE))     //TRUE表示直到操作完成函数才返回
			{
				//操作失败,可以使用GetLastError()获取错误信息
			}
			else
			{
				//发送数据成功
				printf("send success dwWrite = %d \r\n", dwWrite);
				//这里加入发送成功的处理代码
			}
			break;
		case   WAIT_TIMEOUT:
			//读操作失败,原因是超时
			break;
		default:
			//这里加入默认处理代码
			break;
		}

	}

	CloseHandle(Wol.hEvent);

	return 0;
}

//串口发送函数
DWORD Com_Send(HWND hDlg, unsigned char *p, int len)
{
	DWORD write_bytes = len;
	//COMSTAT comstate;
	//DWORD dwErrorFlag;
	BOOL writeFlag=0;
	OVERLAPPED m_osWrite;
	
	DWORD error = 0;
	m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);//信号状态设置
	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;

	writeFlag = WriteFile(hCom, p, write_bytes, &write_bytes, &m_osWrite);//接收
	if (!writeFlag)
	{

		if (GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_osWrite.hEvent, 1000); //后台发送
			return write_bytes;
		}
		/*printf("Write fail\n");*/
		MessageBox(hDlg, TEXT("发送失败\n"), MB_OK, TRUE);
		return 0;
	}
	//----------------------更新接收计数
	/*int sendcount = 0;
	sendcount = GetDlgItemInt(hDlg, IDS_SendCount, NULL, FALSE);
	sendcount += len;
	SetDlgItemInt(hDlg, IDS_SendCount, sendcount, TRUE);*/
	//----------------------更新接收显示
	dislpay(hDlg, p,len, 1);
	return write_bytes;//写入字节数
}
//串口读取函数
DWORD Com_Recv(HWND hDlg, unsigned char *p, int len)
{
	DWORD read_bytes = len;
	BOOL readstate;
	DWORD dwErrorFlag;
	COMSTAT comstate;
	OVERLAPPED m_osRead = { 0 };

	m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_osRead.hEvent = NULL)
		return 0;

	memset(&comstate, 0, sizeof(COMSTAT));

	ClearCommError(hCom, &dwErrorFlag, &comstate);

	if (!comstate.cbInQue)//缓冲区无数据
		return 0;

	if (read_bytes > comstate.cbInQue)//缓冲区实际数据量
		read_bytes = comstate.cbInQue;

	readstate = ReadFile(hCom, p, read_bytes, &read_bytes, &m_osRead);    //读取到p

	if (!readstate)
	{
		if (GetLastError() == ERROR_IO_PENDING)  //后台接收
		{
			GetOverlappedResult(hCom, &m_osRead, &read_bytes, TRUE);
			return read_bytes;//读取字节数
		}

		/*printf("Read fail\n");*/
		MessageBox(hDlg, TEXT("Read fail\n"), MB_OK, TRUE);
		return 0;
	}
	//----------------------更新接收计数
	//int sendcount = 0;
	//sendcount = GetDlgItemInt(hDlg, IDS_ReceiveCount, NULL, FALSE);
	//sendcount += read_bytes;
	//SetDlgItemInt(hDlg, IDS_ReceiveCount, sendcount, TRUE);
	//----------------------更新接收显示
	dislpay(hDlg, p, read_bytes, 2);
	return read_bytes;
}
//显示
INT_PTR CALLBACK dislpay(HWND hDlg, unsigned char *p, int len,char RTflag)
{
	HWND editWindow;
	int i = 0;
	int j = 9;
	int line;

	wchar_t str[1024] = { 0 };
	wchar_t* temp=str;
	if (1 == RTflag)
	{
		editWindow = GetDlgItem(hDlg, IDE_Send);
	}
	else
	{
		editWindow = GetDlgItem(hDlg, IDE_Receive);
	}
	line = SendMessage(editWindow, EM_GETLINECOUNT, 0, 0);
	swprintf_s(temp, 15, L"%0.8d:", line); //将x=1234输出到buffer

	for (i = 0; i < len; i++)
	{
		unsigned char data = p[i]&0xFF;
		temp = &str[j];
		swprintf_s(temp, 20, L"%0.2X ", data); //将x=1234输出到buffer
		j += 3;
		//wprintf(temp);
		//SetDlgItemText(hDlg, IDE_Send, (LPCWSTR)str);
	}	
	//-------------------------层行插入数据：每行要加入回车换行符
	temp = &str[j];
	swprintf_s(temp, 3, L"\r\n"); //将x=1234输出到buffer
	//-------------------------定位光标在新行起始处
	SendMessage(editWindow, EN_SETFOCUS, 0, 0);
	SendMessage(editWindow, EM_SETSEL, -2, -1);
	SendMessage(editWindow, EM_REPLACESEL, 0, (LPARAM)str);
	//----------------------更新计数
	int sendcount = 0;
	if (1 == RTflag)
	{
		sendcount = GetDlgItemInt(hDlg, IDS_SendCount, NULL, FALSE);
		sendcount += len;
		SetDlgItemInt(hDlg, IDS_SendCount, sendcount, TRUE);
	}
	else
	{
		sendcount = GetDlgItemInt(hDlg, IDS_ReceiveCount, NULL, FALSE);
		sendcount += len;
		SetDlgItemInt(hDlg, IDS_ReceiveCount, sendcount, TRUE);
	}
	
	
	return (INT_PTR)FALSE;
}

//int serial_open(LPCWSTR COMx, int BaudRate)
//{
//
//	hCom = CreateFile(COMx, //COM1口    
//		GENERIC_READ | GENERIC_WRITE, //允许读和写    
//		0, //独占方式    
//		NULL,
//		OPEN_EXISTING, //打开而不是创建     
//		0, //重叠方式FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED  (同步方式设置为0)
//		NULL);
//	if (hCom == INVALID_HANDLE_VALUE)
//	{
//		//printf("打开COM失败!\n");
//		MessageBox(hWnd, TEXT("打开串口失败"), MB_OK, TRUE);
//		return FALSE;
//	}
//	SetupComm(hCom, 1024, 1024); //输入缓冲区和输出缓冲区的大小都是1024 
//
//								 //设定读写超时 
//								 /*COMMTIMEOUTS TimeOuts;
//								 TimeOuts.ReadIntervalTimeout=1000;
//								 TimeOuts.ReadTotalTimeoutMultiplier=500;
//								 TimeOuts.ReadTotalTimeoutConstant=5000; //设定写超时
//								 TimeOuts.WriteTotalTimeoutMultiplier=500;
//								 TimeOuts.WriteTotalTimeoutConstant = 2000;
//								 SetCommTimeouts(hCom, &TimeOuts); //设置超时
//								 */
//	DCB dcb;
//	GetCommState(hCom, &dcb);
//	dcb.BaudRate = BaudRate;        //设置波特率为BaudRate
//	dcb.ByteSize = 4;				//每个字节有8位 
//	dcb.Parity = NOPARITY;            //无奇偶校验位 
//	dcb.StopBits = ONESTOPBIT;        //一个停止位
//	SetCommState(hCom, &dcb);        //设置参数到hCom
//	PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);//清空缓存区        //PURGE_TXABORT 中断所有写操作并立即返回，即使写操作还没有完成。
//												   //PURGE_RXABORT 中断所有读操作并立即返回，即使读操作还没有完成。
//												   //PURGE_TXCLEAR 清除输出缓冲区 
//												   //PURGE_RXCLEAR 清除输入缓冲区  
//	return TRUE;
//}
//------------------------一、WinMain入口点
// win32项目.cpp : 定义应用程序的入口点。
//
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);		//二、设计与注册窗口类

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))	//三、创建和显示窗口
    {
        return FALSE;
    }
	// 线程创建
	HANDLE HRead, HWrite;
	HWrite = CreateThread(NULL, 0, ThreadWrite, NULL, 0, NULL);
	HRead = CreateThread(NULL, 0, ThreadRead, NULL, 0, NULL);

	//DWORD dwReadThreadID;
	//DWORD dwReadParam;

	//hThreadRead = CreateThread(NULL,
	//	0,
	//	(LPTHREAD_START_ROUTINE)ThreadProcRead,
	//	&dwReadParam,
	//	0,
	//	&dwReadThreadID
	//);

	//if (hThreadRead == NULL) {

	//	printf("Create Read Thread Fail \r\n");
	//}


	//DWORD dwWriteThreadID;
	//DWORD dwWriteParam;

	//hThreadWrite = CreateThread(NULL,
	//	0,
	//	(LPTHREAD_START_ROUTINE)ThreadProcWrite,
	//	&dwWriteParam,
	//	0,
	//	&dwWriteThreadID
	//);
	//if (hThreadWrite == NULL) {

	//	printf("Create Write Thread Fail \r\n");
	//}


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32));	//调入加速键表。该函数调入指定的加速键表。

    MSG msg;



    // 五、消息循环 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
			/*TranslateMessage是用于转换按键信息的，因为键盘按下和弹起会
			发送WM_KEYDOWN和WM_KEYUP消息，但如果我们只想知道用户输了哪
			些字符，这个函数可以把这些消息转换为WM_CHAR消息，它表示的就
			是键盘按下的那个键的字符，如“A”，这样我们处理起来就更方便了。*/
            DispatchMessage(&msg);
			/*DispatchMessage函数是必须调用的，它的功能就相当于一根传送带，
			每收到一条消息，DispatchMessage函数负责把消息传到WindowProc让
			我们的代码来处理，如果不调用这个函数，我们定义的WindowProc就永
			远接收不到消息，你就不能做消息响应了，你的程序就只能从运行就开始死掉了，没有响应。*/
        }		
		CheckListBoxdData(hWnd);
    }
	CloseHandle(HRead);
	CloseHandle(HWrite);
    return (int) msg.wParam;
}


//------------------------二、设计与注册窗口类
//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	//**************在注册窗口类之前必须填充所有WNDCLASS结构体
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;//指定窗口类型，各种“类风格”,可以使用按位或操作符组合起来
												  //CS_VREDRAW	移动或者调整窗口的高度（垂直方向）时，重绘整个窗口
												  //CS_HREDRAW	移动或者调整窗口的宽度（水平方向）时，重绘整个窗口
    wcex.lpfnWndProc    = WindowProc;				//指定窗口过程（必须是回调函数）
    wcex.cbClsExtra     = 0;					//预留的额外空间，一般为 0
    wcex.cbWndExtra     = 0;					//预留的额外空间，一般为 0
    wcex.hInstance      = hInstance;			//应用程序的实例句柄
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32));	//为所有基于该窗口类的窗口设定一个图标
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);					//为所有基于该窗口类的窗口设定一个鼠标指针
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);							//指定窗口背景色
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32);						//指定窗口菜单
    wcex.lpszClassName  = szWindowClass;									//指定窗口类名

	//wcex.style = CS_HREDRAW | CS_VREDRAW;//指定窗口类型，各种“类风格”,可以使用按位或操作符组合起来
	//									 //CS_VREDRAW	移动或者调整窗口的高度（垂直方向）时，重绘整个窗口
	//									 //CS_HREDRAW	移动或者调整窗口的宽度（水平方向）时，重绘整个窗口
	//wcex.lpfnWndProc = WindowProc;				//指定窗口过程（必须是回调函数）
	//wcex.cbClsExtra = 0;					//预留的额外空间，一般为 0
	//wcex.cbWndExtra = 0;					//预留的额外空间，一般为 0
	//wcex.hInstance = hInstance;			//应用程序的实例句柄
	//wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	//wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32));	//为所有基于该窗口类的窗口设定一个图标
	//wcex.hCursor = nullptr;					//为所有基于该窗口类的窗口设定一个鼠标指针
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);							//指定窗口背景色
	//wcex.lpszMenuName = nullptr;						//指定窗口菜单
	//wcex.lpszClassName = szWindowClass;									//指定窗口类名
    

    return RegisterClassExW(&wcex);		//向操作系统注册窗口类:向系统注册，这样系统才能知道有这个窗口类的存在	
}
//------------------------三、创建和显示窗口
//窗口类注册完成后，就应该创建窗口，然后显示窗口，调用CreateWindow创建窗口，
//如果成功，会返回一个窗口的句柄，我们对这个窗口的操作都要用到这个句柄。
//什么是句柄呢？其实它就是一串数字，只是一个标识而已，内存中会存在各种资源，
//如图标、文本等，为了可以有效标识这些资源，每一个资源都有其唯一的标识符，这样，
//通过查找标识符，就可以知道某个资源存在于内存中哪一块地址中，就好比你出身的时候，
//长辈都要为你取个名字，你说名字用来干吗？名字就是用来标识你的，不然，你见到A叫小明，
//遇到B又叫小明，那谁知道哪个才是小明啊？就好像你上大学去报到号，会为你分配一个可以在
//本校学生中唯一标识你的学号，所有学生的学号都是不同的，这样，只要通过索引学号，就可
//以找到你的资料。
//CreateWindow函数返回一个HWND类型，它就是窗口类的句柄

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中
   /*HWND hWnd;*/
   //--------------------------------居中
   RECT rc, rc1, rctomove;
   int width = GetSystemMetrics(SM_CXSCREEN);
   int height = GetSystemMetrics(SM_CYSCREEN);
   rc.left = 0;
   rc.top = 0;
   rc.right = width;
   rc.bottom = height;
   

  hWnd = CreateWindowW(szWindowClass, // 窗口类名称
							//szTitle,// 窗口标题
							TEXT("win32完整的开发流程"),	// 窗口标题
							//WS_OVERLAPPEDWINDOW,	// 窗口风格，或称窗口外观样式
													//WS_OVERLAPPEDWINDOW相当于（WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX）
							WS_OVERLAPPEDWINDOW^WS_THICKFRAME,		//禁止改变大小
							0,	// 初始 x 坐标
							0,				// 初始 y 坐标
							800,	// 初始 x 方向尺寸
							550,				// 初始 y 方向尺寸
							nullptr,	// 父窗口句柄,没有父窗口，为NULL 
							nullptr,	// 窗口菜单句柄,没有菜单，为NULL
							hInstance,	// 当前应用程序的实例句柄
							nullptr);	// 创建参数,没有附加数据，为NULL

  GetClientRect(hWnd, &rc1);//该函数获取窗口客户区的大小

  rctomove.left		= (rc.right - rc.left) / 2 - (rc1.right - rc1.left) / 2;
  rctomove.right	= (rc.right - rc.left) / 2 + (rc1.right - rc1.left) / 2;
  rctomove.top		= (rc.bottom - rc.top) / 2 - (rc1.bottom - rc1.top) / 2;
  rctomove.bottom	= (rc.bottom - rc.top) / 2 + (rc1.bottom - rc1.top) / 2;
  ::SetWindowPos(hWnd, HWND_TOPMOST, rctomove.left, rctomove.top, rc1.right - rc1.left, rc1.bottom - rc1.top, SWP_NOZORDER |SWP_FRAMECHANGED | SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);


   //HWND hWnd = CreateWindowW(szWindowClass, // 窗口类名称
			//								//szTitle,// 窗口标题
	  // TEXT("win32完整的开发流程"),	// 窗口标题
	  // WS_OVERLAPPEDWINDOW,	// 窗口风格，或称窗口外观样式
			//				//WS_OVERLAPPEDWINDOW相当于（WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX）
	  // CW_USEDEFAULT,	// 初始 x 坐标
	  // 0,				// 初始 y 坐标
	  // CW_USEDEFAULT,	// 初始 x 方向尺寸
	  // 0,				// 初始 y 方向尺寸
	  // nullptr,	// 父窗口句柄,没有父窗口，为NULL 
	  // nullptr,	// 窗口菜单句柄,没有菜单，为NULL
	  // hInstance,	// 当前应用程序的实例句柄
	  // nullptr);	// 创建参数,没有附加数据，为NULL

   if (!hWnd)		//检查窗口是否创建成功
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);	//显示窗口
	//------------------------四、更新窗口（可选）
   UpdateWindow(hWnd);			//更新窗口

   return TRUE;
}

//------------------------六、消息响应
//
//  函数: WindowProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);               // 分析菜单选择:
    switch (message)
    {
    case WM_COMMAND:	//按键消息
        {
			switch (wmEvent)
			{
				case LBN_SELCHANGE:		//listbox
					switch (wmId)
					{
						HWND hwndList;
					case IDL_Addr1:
						hwndList = GetDlgItem(hWnd, IDL_Addr1);
						SendMessage(hwndList, LB_GETCURSEL, 0, 0);
						break;
					case IDL_Addr2:
						hwndList = GetDlgItem(hWnd, IDL_Addr2);
						SendMessage(hwndList, LB_GETCURSEL, 0, 0);
						break;
					case IDL_Addr3:
						hwndList = GetDlgItem(hWnd, IDL_Addr3);
						SendMessage(hwndList, LB_GETCURSEL, 0, 0);
						break;
					}
					break;
				case BN_CLICKED:		//checkbox
					switch (wmId)
					{
						//HWND hwndList;
					case IDC_HexSend:		//hex发送
						if (SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == TRUE)//是否打勾了
							SendMessage((HWND)lParam, BM_SETCHECK, BST_UNCHECKED, 0);//取消打勾
						else 
							SendMessage((HWND)lParam, BM_SETCHECK, BST_CHECKED, 0);//打勾
						break;
					case IDC_HexRecv:	//hex接收
						if (SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == TRUE)//是否打勾了
							SendMessage((HWND)lParam, BM_SETCHECK, BST_UNCHECKED, 0);//取消打勾
						else
							SendMessage((HWND)lParam, BM_SETCHECK, BST_CHECKED, 0);//打勾
						break;
						break;
					}
					break;
			}
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case IDM_CRC16:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_CRC16), hWnd, AboutCRC16);
				break;
			case IDM_Prol:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_Prol), hWnd, AboutProl);
				break;
			
			case IDB_PortCtl:
				SerailConfiguration(hWnd,message,wParam,lParam);
				break;
			case IDB_ClearData:
				ClearData(hWnd, message, wParam, lParam);
				break;
            case IDM_EXIT:
                DestroyWindow(hWnd);	//销毁指定的窗口。这个函数通过发送WM_DESTROY 消息和 WM_NCDESTROY 消息使窗口无效并移除其键盘焦点。
										//这个函数还销毁窗口的菜单，清空线程的消息队列，销毁与窗口过程相关的定时器，解除窗口对剪贴板的拥有权，打断剪贴板器的查看链。
                break;
			case IDB_LayPower:
				UserProc(hWnd, message, wParam, lParam);
				break;
			case IDB_RedCtl:
				UserProc(hWnd, message, wParam, lParam);
				break;
			case IDB_GreCtl:
				UserProc(hWnd, message, wParam, lParam);
				break;
			case IDB_BluCtl:
				UserProc(hWnd, message, wParam, lParam);
				break;
			case IDB_RGBCtl:
				UserProc(hWnd, message, wParam, lParam);
				break;
			case IDB_LockCtl:
				UserProc(hWnd, message, wParam, lParam);
				break;
			case IDB_BKLigthCtl:
				UserProc(hWnd, message, wParam, lParam);
				break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;	
	case WM_CREATE:
		//---------------------------------------静态字符
		Login_Button = CreateWindow(TEXT("static"), TEXT("串口号："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 0, 80, 20, hWnd, (HMENU)IDS_COMx, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("波特率："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 60, 80, 20, hWnd, (HMENU)IDS_BaudRate, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("校验位："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 120, 80, 20, hWnd, (HMENU)奇偶校验, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("停止位："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 180, 80, 20, hWnd, (HMENU)停止位, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		
		Login_Button = CreateWindow(TEXT("static"), TEXT("接收计数："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 280, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("发送计数："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 325, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		Login_Button = CreateWindow(TEXT("static"), TEXT("状态栏："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 5, 470, 800, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		Login_Button = CreateWindow(TEXT("static"), TEXT("Address1："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 5, 410, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("Address2："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, 410, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("Address3："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 400, 410, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		//---------------------------------------添加按键
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开串口"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 375, 80, 30, hWnd, (HMENU)IDB_PortCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("清空数据"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 405, 80, 30, hWnd, (HMENU)IDB_ClearData, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("发送数据"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 435, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		EnableWindow(Login_Button, FALSE);		//暂时禁用控件

		Login_Button = CreateWindow(TEXT("button"), TEXT("打开RGB"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 5, 435, 80, 30, hWnd, (HMENU)IDB_RGBCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开红灯"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 435, 80, 30, hWnd, (HMENU)IDB_RedCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开黄灯"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, 435, 80, 30, hWnd, (HMENU)IDB_GreCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开蓝灯"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 300, 435, 80, 30, hWnd, (HMENU)IDB_BluCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开门锁"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 400, 435, 80, 30, hWnd, (HMENU)IDB_LockCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开背光"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 435, 80, 30, hWnd, (HMENU)IDB_BKLigthCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开供电"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 600, 435, 80, 30, hWnd, (HMENU)IDB_LayPower, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		//---------------------------------------选择框
		Login_Button = CreateWindow(TEXT("button"), TEXT("Hex接收"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 700, 235, 80, 20, hWnd, (HMENU)IDC_HexRecv, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, BM_SETCHECK, BST_CHECKED, 0);//打勾
		EnableWindow(Login_Button, FALSE);		//暂时禁用控件
		Login_Button = CreateWindow(TEXT("button"), TEXT("Hex发送"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 700, 255, 80, 20, hWnd, (HMENU)IDC_HexSend, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, BM_SETCHECK, BST_CHECKED, 0);//打勾
		EnableWindow(Login_Button, FALSE);		//暂时禁用控件
		//---------------------------------------下拉列表
		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("串口号"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 30, 80, 100, hWnd, (HMENU)IDC_COMx, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		for (int i = 0; i <= 16; i++)
		{
			wchar_t str[80];
			//sprintf(str, "%x", i); //将10.8转为字符串
			//itoa(i, str,10);
			swprintf_s(str, 16, L"COM%d", i); //将x=1234输出到buffer
			wprintf(str);
			SendMessage(Login_Button, CB_ADDSTRING, i, (LPARAM)(str));
		}
		SendMessage(Login_Button, CB_SETCURSEL, 2, 0);//设置默认值

		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("波特率"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 90, 80, 100, hWnd, (HMENU)IDB_BaudRate, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, CB_ADDSTRING, 0, (LPARAM)TEXT("1200"));
		SendMessage(Login_Button, CB_ADDSTRING, 1, (LPARAM)TEXT("2400"));
		SendMessage(Login_Button, CB_ADDSTRING, 2, (LPARAM)TEXT("4800"));
		SendMessage(Login_Button, CB_ADDSTRING, 3, (LPARAM)TEXT("9600"));
		SendMessage(Login_Button, CB_ADDSTRING, 4, (LPARAM)TEXT("14400"));
		SendMessage(Login_Button, CB_ADDSTRING, 5, (LPARAM)TEXT("19200"));
		SendMessage(Login_Button, CB_ADDSTRING, 6, (LPARAM)TEXT("38400"));
		SendMessage(Login_Button, CB_ADDSTRING, 7, (LPARAM)TEXT("56000"));
		SendMessage(Login_Button, CB_ADDSTRING, 8, (LPARAM)TEXT("57600"));
		SendMessage(Login_Button, CB_ADDSTRING, 9, (LPARAM)TEXT("115200"));
		SendMessage(Login_Button, CB_ADDSTRING, 10, (LPARAM)TEXT("128000"));
		SendMessage(Login_Button, CB_ADDSTRING, 11, (LPARAM)TEXT("256000"));
		SendMessage(Login_Button, CB_SETCURSEL, 5, 0);//设置默认值

		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("校验位"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 150, 80, 100, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, CB_ADDSTRING, 0, (LPARAM)TEXT("奇校验"));
		SendMessage(Login_Button, CB_ADDSTRING, 1, (LPARAM)TEXT("偶校验"));
		SendMessage(Login_Button, CB_ADDSTRING, 2, (LPARAM)TEXT("无校验"));
		SendMessage(Login_Button, CB_SETCURSEL, 2, 0);//设置默认值
		EnableWindow(Login_Button, FALSE);		//暂时禁用控件

		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("停止位"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 210, 80, 100, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, CB_ADDSTRING, 0, (LPARAM)TEXT("1位"));
		SendMessage(Login_Button, CB_ADDSTRING, 1, (LPARAM)TEXT("1.5位"));
		SendMessage(Login_Button, CB_ADDSTRING, 2, (LPARAM)TEXT("2位"));
		SendMessage(Login_Button, CB_SETCURSEL, 0, 0);//设置默认值
		EnableWindow(Login_Button, FALSE);		//暂时禁用控件

		

		//---------------------------------------下拉列表Address1
		Login_Button = CreateWindow(TEXT("Listbox"), TEXT("Address1"), WS_CHILD |  WS_VSCROLL|WS_BORDER | WS_VISIBLE | LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, 100, 410, 80, 20, hWnd, (HMENU)IDL_Addr1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		wchar_t str[80];
		for (int i = 0; i <= 16; i++)
		{
			//sprintf(str, "%x", i); //将10.8转为字符串
			//itoa(i, str,10);
			swprintf_s(str,4,L"%d", i); //将x=1234输出到buffer
			wprintf(str);
			SendMessage(Login_Button, LB_ADDSTRING, i, (LPARAM)(str));	
			//SendMessage(Login_Button, CB_SETCURSEL, 1, 0);//设置默认值
		}
		SendMessage(Login_Button, LB_ADDSTRING, 17, (LPARAM)(TEXT("All")));
		SendMessage(Login_Button, LB_SETCURSEL, (WPARAM)2, 0);
		//---------------------------------------下拉列表Address2
		Login_Button = CreateWindow(TEXT("Listbox"), TEXT("Address2"), WS_CHILD | WS_VSCROLL | WS_BORDER | WS_VISIBLE | LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, 300, 410, 80, 20, hWnd, (HMENU)IDL_Addr2, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		for (int i = 0; i <= 16; i++)
		{
			//sprintf(str, "%x", i); //将10.8转为字符串
			//itoa(i, str,10);
			swprintf_s(str, 4, L"%d", i); //将x=1234输出到buffer
			wprintf(str);
			SendMessage(Login_Button, LB_ADDSTRING, i, (LPARAM)(str));
		}
		SendMessage(Login_Button, LB_ADDSTRING, 17, (LPARAM)(TEXT("All")));
		SendMessage(Login_Button, LB_SETCURSEL, (WPARAM)1, 0);
		//---------------------------------------下拉列表Address3
		Login_Button = CreateWindow(TEXT("Listbox"), TEXT("Address3"), WS_CHILD | WS_VSCROLL | WS_BORDER | WS_VISIBLE | LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, 500, 410, 80, 20, hWnd, (HMENU)IDL_Addr3, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		for (int i = 0; i <= 16; i++)
		{
			//sprintf(str, "%x", i); //将10.8转为字符串
			//itoa(i, str,10);
			swprintf_s(str, 4, L"%d", i); //将x=1234输出到buffer
			wprintf(str);
			SendMessage(Login_Button, LB_ADDSTRING, i, (LPARAM)(str));
		}
		SendMessage(Login_Button, LB_ADDSTRING, 17, (LPARAM)(TEXT("All")));
		SendMessage(Login_Button, LB_SETCURSEL, (WPARAM)1, 0);

		//---------------------------------------显示框
		Login_Button = CreateWindow(TEXT("edit"), TEXT("接收区："), WS_CHILD | WS_VISIBLE | WS_BORDER /*边框*/ | ES_AUTOHSCROLL /*水平滚动*/| ES_AUTOVSCROLL /*垂直滚动*/ | ES_MULTILINE/*多行*/ | WS_VSCROLL/*垂直滚动条*/ | WS_HSCROLL/*垂直滚动条*/| ES_READONLY/*只读*/| ES_WANTRETURN/*支持回车换行*/, 0, 0, 695, 300, hWnd, (HMENU)IDE_Receive, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("edit"), TEXT("发送区："), WS_CHILD | WS_VISIBLE | WS_BORDER /*边框*/ | ES_AUTOHSCROLL /*水平滚动*/ | ES_AUTOVSCROLL /*垂直滚动*/ | ES_MULTILINE/*多行*/ | WS_VSCROLL/*垂直滚动条*/ | WS_HSCROLL/*垂直滚动条*/ /*| ES_READONLY只读*/ | ES_WANTRETURN/*支持回车换行*/, 0, 305, 695, 100, hWnd, (HMENU)IDE_Send, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		Login_Button = CreateWindow(TEXT("edit"), TEXT("接收计数"), WS_CHILD | WS_VISIBLE | WS_BORDER /*边框*/ | ES_READONLY/*只读*/ , 700, 300, 80, 20, hWnd, (HMENU)IDS_ReceiveCount, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("edit"), TEXT("发送计数"), WS_CHILD | WS_VISIBLE | WS_BORDER /*边框*/  | ES_READONLY/*只读*/, 700, 345, 80, 20, hWnd, (HMENU)IDS_SendCount, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
			
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
			/*RECT rect;

			GetClientRect(hWnd, &rect);
			DrawText(hdc, TEXT("绘制窗口1行\n绘制窗口2行\n"), -1, &rect,

				DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_WORD_ELLIPSIS);*/

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);	//该函数向系统表明有个线程有终止请求。通常用来响应WM_DESTROY消息。
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
// “关于CRC16”框的消息处理程序。
INT_PTR CALLBACK AboutCRC16(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDE_CRC16,TEXT("\
转换结果低8位在前，高8位在后			\
\r\nvoid  InvertUint8(unsigned char *dBuf, unsigned char *srcBuf)	\
\r\n{																\
\r\n	int	i = 0;													\
\r\n	unsigned char tmp[4];										\
\r\n	tmp[0] = 0;													\
\r\n	for (i = 0; i< 8; i++)										\
\r\n	{															\
\r\n		if (srcBuf[0] & (1 << i))								\
\r\n			tmp[0] |= 1 << (7 - i);								\
\r\n	}															\
\r\n	dBuf[0] = tmp[0];											\
\r\n}																\
\r\nvoid  InvertUint16(unsigned short *dBuf, unsigned short *srcBuf)	\
\r\n{																\
\r\n	int	i = 0;													\
\r\n	unsigned short tmp[4];										\
\r\n	tmp[0] = 0;													\
\r\n	for (i = 0; i< 16; i++)										\
\r\n	{															\
\r\n		if (srcBuf[0] & (1 << i))								\
\r\n		tmp[0] |= 1 << (15 - i);								\
\r\n	}															\
\r\n	dBuf[0] = tmp[0];											\
\r\n }																\
\r\nunsigned short CRC16_MODBUS(unsigned char *puchMsg, unsigned int usDataLen)	\
\r\n{													\
\r\n	unsigned short wCRCin = 0xFFFF;					\
\r\n	unsigned short wCPoly = 0x8005;					\
\r\n	unsigned char wChar = 0;						\
\r\n	int	i = 0;										\
\r\n	while (usDataLen--)								\
\r\n	{												\
\r\n		wChar = *(puchMsg++);						\
\r\n		InvertUint8(&wChar, &wChar);				\
\r\n		wCRCin ^= (wChar << 8);						\
\r\n		for (i = 0; i < 8; i++)						\
\r\n		{											\
\r\n			if (wCRCin & 0x8000)					\
\r\n				wCRCin = (wCRCin << 1) ^ wCPoly;	\
\r\n			else									\
\r\n				wCRCin = wCRCin << 1;				\
\r\n		}											\
\r\n	}												\
\r\n	InvertUint16(&wCRCin, &wCRCin);					\
\r\n	return (wCRCin);								\
\r\n }													\
			"));
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
// “关于协议”框的消息处理程序。
INT_PTR CALLBACK AboutProl(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDE_Prol, TEXT("\
帧格式			\
\r\n0x7E	:起始符	\
\r\nmsg	:消息体\
\r\nCRC16	:2字节校验码\
\r\n0x7F	:结束符\
\r\n	\
\r\nmsg格式（消息格式)		\
\r\nlen	:消息体长度(不含此字节)	\
\r\ncmd	:命令号\
\r\naddress1	:地址1\
\r\naddress2	:地址2\
\r\naddress3	:地址3\
\r\ndata0	:数据0\
\r\n~	:数据\
\r\ndatan	:数据n\
			"));
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
// “打开串口”框的消息处理程序。
INT_PTR CALLBACK SerailConfiguration(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDB_PortCtl)
		{
			static char portflag = 0;
			//MessageBox(hDlg, TEXT("你点击了打开串口"), TEXT("提示框名"), MB_OK);
			//SetWindowText(hDlg, TEXT("关闭串口"));
			//SetDlgItemText(hDlg, 打开串口, TEXT("关闭串口"));
			if (0 == portflag)
			{
				//LPCWSTR COMx;
				//TCHAR  text[20];
				//HWND hCombox;
				//int BaudRate;
				//hCombox = GetDlgItem(hDlg, IDB_BaudRate);
				////GetDlgItemText(combox, 串口号, text, 10);
				//SendMessage(hCombox, CB_GETLBTEXT, 1, (LPARAM)text);

				//BaudRate = (int)SendMessage(hCombox, CB_GETCURSEL, 0, 0L);
				//BaudRate = GetDlgItemInt(hDlg, 波特率, NULL, TRUE);
				HWND hDct;
				//LPCWSTR COMx;
				TCHAR  text[20];
				int BaudRate;
				
				//-------------------------获取相关参数
				GetDlgItemText(hDlg, IDC_COMx, (LPWSTR)text, 10);
				BaudRate = GetDlgItemInt(hDlg, IDB_BaudRate, NULL, false);
				portflag = Serial_Init(hDlg, (LPWSTR)text, BaudRate);
				if (1 == portflag)	//打开串口成功
				{
					//-------------------------锁定相关控件
					hDct = GetDlgItem(hDlg, IDC_COMx);
					EnableWindow(hDct, FALSE);
					hDct = GetDlgItem(hDlg, IDB_BaudRate);
					EnableWindow(hDct, FALSE);
					//-------------------------更新控件
					SetDlgItemText(hDlg, IDB_PortCtl, TEXT("关闭串口"));	//修改控件文字
					SetDlgItemInt(hDlg, IDS_ReceiveCount, 0, TRUE);
					SetDlgItemInt(hDlg, IDS_SendCount, 0, TRUE);
					SetDlgItemText(hDlg, IDE_Receive, TEXT(""));	//修改控件文字
					SetDlgItemText(hDlg, IDE_Send, TEXT(""));	//修改控件文字
					portflag = 1;
				}				
			}
			else
			{				
				if (0 == CloseHandle(hCom))
				{
					MessageBox(hDlg, TEXT("关闭串口失败"),MB_OK,NULL);
				}
				else
				{
					HWND hDct;
					//-------------------------解锁相关控件
					hDct = GetDlgItem(hDlg, IDC_COMx);
					EnableWindow(hDct, TRUE);
					hDct = GetDlgItem(hDlg, IDB_BaudRate);
					EnableWindow(hDct, TRUE);
					//-------------------------更新控件
					SetDlgItemText(hDlg, IDB_PortCtl, TEXT("打开串口"));
					portflag = 0;
				}				
			}
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
// “清除数据”框的消息处理程序。
INT_PTR CALLBACK ClearData(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDB_ClearData)
		{
			SetDlgItemText(hDlg, IDE_Receive, TEXT(""));
			SetDlgItemText(hDlg, IDE_Send, TEXT(""));
			SetDlgItemText(hDlg, IDE_Receive, TEXT(""));
			SetDlgItemInt(hDlg, IDS_ReceiveCount, 0, TRUE);
			SetDlgItemInt(hDlg, IDS_SendCount, 0, TRUE);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
// “按键处理”框的消息处理程序。
INT_PTR CALLBACK UserProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		HWND hDct;
		
		int wmId = LOWORD(wParam);
		unsigned char TxdBuffer[32] = { 0x7E,0x07,0x02 };
		char sendlen = 12;
		wchar_t	text[50] = { 0 };
		switch (wmId)
		{			
		case IDB_ClearData:
				SetDlgItemText(hDlg, IDE_Receive, TEXT(""));
				SetDlgItemText(hDlg, IDE_Receive, TEXT(""));
				SetDlgItemText(hDlg, IDE_Receive, TEXT(""));
				SetDlgItemInt(hDlg, IDS_ReceiveCount, 0, TRUE);
				SetDlgItemInt(hDlg, IDS_SendCount, 0, TRUE);
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
		case IDB_LayPower:
			GetDlgItemText(hDlg, IDB_LayPower, text, 6);
			if (0 == memcmp(TEXT("打开供电"), text, 6))	//获取控件字符
			{
				TxdBuffer[2] = 0x05;
				TxdBuffer[6] = 0x01;
				TxdBuffer[7] = 0x00;
				TxdBuffer[8] = 0x00;
				SetDlgItemText(hDlg, IDB_LayPower, TEXT("关闭供电"));
			
			}
			else
			{
				TxdBuffer[2] = 0x05;
				TxdBuffer[6] = 0x00;
				TxdBuffer[7] = 0x00;
				TxdBuffer[8] = 0x00;
				SetDlgItemText(hDlg, IDB_LayPower, TEXT("打开供电"));
			}
			goto sendData;
	
			case IDB_RedCtl:
				GetDlgItemText(hDlg, IDB_RedCtl, text, 6);
				if (0 == memcmp(TEXT("打开红灯"), text, 6))	//获取控件字符
				{					
					TxdBuffer[6] = 0x01;
					TxdBuffer[7] = 0x00;
					TxdBuffer[8] = 0x00;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("关闭显示"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("打开黄灯"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("打开蓝灯"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("打开RGB"));
				}
				else
				{
					TxdBuffer[6] = 0x00;
					TxdBuffer[7] = 0x00;
					TxdBuffer[8] = 0x00;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("打开红灯"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("打开黄灯"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("打开蓝灯"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("打开RGB"));
				}
								
				goto sendData;
			case IDB_GreCtl:
				GetDlgItemText(hDlg, IDB_GreCtl, text, 6);
				if (0 == memcmp(TEXT("打开黄灯"), text, 6))	//获取控件字符
				{
					TxdBuffer[6] = 0x00;
					TxdBuffer[7] = 0x01;
					TxdBuffer[8] = 0x00;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("打开红灯"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("关闭显示"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("打开蓝灯"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("打开RGB"));
				}
				else
				{
					TxdBuffer[6] = 0x00;
					TxdBuffer[7] = 0x00;
					TxdBuffer[8] = 0x00;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("打开红灯"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("打开黄灯"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("打开蓝灯"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("打开RGB"));
				}
				
				goto sendData;
			case IDB_BluCtl:
				GetDlgItemText(hDlg, IDB_BluCtl, text, 6);
				if (0 == memcmp(TEXT("打开蓝灯"), text, 6))	//获取控件字符
				{					
					TxdBuffer[6] = 0x00;
					TxdBuffer[7] = 0x00;
					TxdBuffer[8] = 0x01;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("打开红灯"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("打开黄灯"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("关闭显示"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("打开RGB"));
				}
				else
				{
					TxdBuffer[6] = 0x00;
					TxdBuffer[7] = 0x00;
					TxdBuffer[8] = 0x00;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("打开红灯"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("打开黄灯"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("打开蓝灯"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("打开RGB"));
				}
				goto sendData;
			case IDB_RGBCtl:
				GetDlgItemText(hDlg, IDB_RGBCtl, text, 6);
				if (0==memcmp(TEXT("打开RGB"), text, 6))	//获取控件字符
				{					
					TxdBuffer[6] = 0x01;
					TxdBuffer[7] = 0x01;
					TxdBuffer[8] = 0x01;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("打开红灯"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("打开黄灯"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("打开蓝灯"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("关闭显示"));
				}
				else
				{
					TxdBuffer[6] = 0x00;
					TxdBuffer[7] = 0x00;
					TxdBuffer[8] = 0x00;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("打开红灯"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("打开黄灯"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("打开蓝灯"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("打开RGB"));
				}
			
			sendData:

				char address1 = GetListBoxdData(hDlg, IDL_Addr1);
				char address2 = GetListBoxdData(hDlg, IDL_Addr2);
				char address3 = GetListBoxdData(hDlg, IDL_Addr3);

				TxdBuffer[3] = address1;
				TxdBuffer[4] = address2;
				TxdBuffer[5] = address3;
				TxdBuffer[11] = 0x7F;

				//-----------------------计算CRC
				unsigned short crc16 = 0;
				crc16 = CRC16_MODBUS(&TxdBuffer[1], TxdBuffer[1] + 1);
				TxdBuffer[TxdBuffer[1] + 2] = crc16 & 0xFF;
				TxdBuffer[TxdBuffer[1] + 3] = (crc16 >> 8) & 0xFF;

				Com_Send(hDlg, TxdBuffer, sendlen);
				
				return (INT_PTR)TRUE;
				//itoa
		}
		break;
	}
	return (INT_PTR)FALSE;
}
//----------------------获取listbox值
unsigned char GetListBoxdData(HWND hWnd, int nIDDlgItem)
{
	//SendMessage(HWND hWnd,)
	HWND hDlg;
	hDlg = GetDlgItem(hWnd, nIDDlgItem);
	wchar_t text[128] = { 0xFF };
	unsigned char data = 0;
	unsigned char Item = (unsigned char)SendMessage(hDlg, LB_GETCURSEL, 0, 0);//柜地址
	text[2] = 0;
	SendMessage(hDlg, LB_GETTEXT, (WPARAM)Item, (LPARAM)text);
	if (0 == text[2])
	{
		data = atoi((char*)&text[0]);
		if (0 != text[1])
		{
			data = data * 10 + atoi((char*)&text[1]);
		}
	}
	else
	{
		data = (unsigned char)0xFF;
	}
	return	data;
}
//----------------------检查listbox设置
unsigned char CheckListBoxdData(HWND hWnd)
{
	if (0 == GetListBoxdData(hWnd, IDL_Addr1))	//地址段1为0
	{
		SetDisableWindow(hWnd, IDL_Addr2);
		SetDisableWindow(hWnd, IDL_Addr3);
		SetDisableWindow(hWnd, IDB_RedCtl);
		SetDisableWindow(hWnd, IDB_GreCtl);
		SetDisableWindow(hWnd, IDB_BluCtl);
		SetDisableWindow(hWnd, IDB_RGBCtl);

		SetDisableWindow(hWnd, IDB_LockCtl);
		SetDisableWindow(hWnd, IDB_BKLigthCtl);

		SetDisableWindow(hWnd, IDB_LayPower);
	}
	else
	{
		SetEnableWindow(hWnd, IDB_LayPower);
		if (0 == (GetListBoxdData(hWnd, IDL_Addr2) && GetListBoxdData(hWnd, IDL_Addr3)))	//ADDR2或者ADDR3有一个为0
		{
			SetEnableWindow(hWnd, IDL_Addr2);
			SetEnableWindow(hWnd, IDL_Addr3);
			SetDisableWindow(hWnd, IDB_RedCtl);
			SetDisableWindow(hWnd, IDB_GreCtl);
			SetDisableWindow(hWnd, IDB_BluCtl);
			SetDisableWindow(hWnd, IDB_RGBCtl);

			SetEnableWindow(hWnd, IDB_LockCtl);
			SetEnableWindow(hWnd, IDB_BKLigthCtl);
		}
		else
		{
			SetEnableWindow(hWnd, IDL_Addr2);
			SetEnableWindow(hWnd, IDL_Addr3);
			SetEnableWindow(hWnd, IDB_RedCtl);
			SetEnableWindow(hWnd, IDB_GreCtl);
			SetEnableWindow(hWnd, IDB_BluCtl);
			SetEnableWindow(hWnd, IDB_RGBCtl);

			SetDisableWindow(hWnd, IDB_LockCtl);
			SetDisableWindow(hWnd, IDB_BKLigthCtl);
		}
	}
	return	1;
}
//----------------------获取listbox值
unsigned char SetEnableWindow(HWND hWnd, int nIDDlgItem)
{
	//SendMessage(HWND hWnd,)
	HWND hDlg;
	hDlg = GetDlgItem(hWnd, nIDDlgItem);
	if (NULL == hDlg)
		return 0;
	EnableWindow(hDlg, TRUE);		//暂时禁用控件
	return	1;
}
//----------------------获取listbox值
unsigned char SetDisableWindow(HWND hWnd, int nIDDlgItem)
{
	//SendMessage(HWND hWnd,)
	HWND hDlg;
	hDlg = GetDlgItem(hWnd, nIDDlgItem);
	if (NULL == hDlg)
		return 0;
	EnableWindow(hDlg, FALSE);		//暂时禁用控件
	return	1;
}



/*******************************************************************************
*函数名			:	InvertUint8
*功能描述		:	函数功能说明
*输入				:
*返回值			:	无
*修改时间		:	无
*修改说明		:	无
*注释				:	wegam@sina.com
*******************************************************************************/
void InvertUint8(unsigned char *dBuf, unsigned char *srcBuf)
{
	int	i = 0;
	unsigned char tmp[4];
	tmp[0] = 0;
	for (i = 0; i< 8; i++)
	{
		if (srcBuf[0] & (1 << i))
			tmp[0] |= 1 << (7 - i);
	}
	dBuf[0] = tmp[0];
}
/*******************************************************************************
*函数名			:	InvertUint16
*功能描述		:	函数功能说明
*输入				:
*返回值			:	无
*修改时间		:	无
*修改说明		:	无
*注释				:	wegam@sina.com
*******************************************************************************/
void InvertUint16(unsigned short *dBuf, unsigned short *srcBuf)
{
	int	i = 0;
	unsigned short tmp[4];
	tmp[0] = 0;
	for (i = 0; i< 16; i++)
	{
		if (srcBuf[0] & (1 << i))
			tmp[0] |= 1 << (15 - i);
	}
	dBuf[0] = tmp[0];
}
/*******************************************************************************
*函数名			:	CRC16_MODBUS
*功能描述		:	多项式x16+x15+x5+1（0x8005），初始值0xFFFF，低位在前，高位在后，结果与0x0000异或
*输入				:
*返回值			:	无
*修改时间		:	无
*修改说明		:	无
*注释				:	wegam@sina.com
*******************************************************************************/

unsigned short CRC16_MODBUS(unsigned char *puchMsg, unsigned int usDataLen)
{
	unsigned short wCRCin = 0xFFFF;
	unsigned short wCPoly = 0x8005;
	unsigned char wChar = 0;
	int	i = 0;

	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		InvertUint8(&wChar, &wChar);
		wCRCin ^= (wChar << 8);
		for (i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin, &wCRCin);
	return (wCRCin);
}
