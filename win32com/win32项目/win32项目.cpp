// win32项目.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "win32项目.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
HWND hWnd;
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    NewWin(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Calculator(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ComboxTest(HWND, UINT, WPARAM, LPARAM);

int ComboxInit(void);

HANDLE Robocon_com;

HANDLE hCom; //全局变量，串口句柄

/**************************************************************
*   串口通信
*   包含初始化函数，接收函数，发送函数
*   仅写了字符发送和接收，需要可以增加单字节发送和接收
*   通过宏SERIAL_COM修改使用COM口,SERIAL_BOUD 修改波特率
*   默认：COM口：com5 波特率:115200 数据位：8 停止位：1 校验：无
*   环境：VS2010
//-------------------- -
//作者：灰同学
//来源：CSDN
//原文：https ://blog.csdn.net/zat907943815/article/details/53256434
//版权声明：本文为博主原创文章，转载请附上博文链接！
***************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winbase.h>
int Serial_Init(LPCWSTR COMx, int BaudRate)
{
	DCB com_dcb;    //参数设置
	COMMTIMEOUTS tim_out;

	Robocon_com = CreateFile(COMx, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

	if (Robocon_com == (HANDLE)-1)
	{
		/*printf("Serial open fail\n");*/
		MessageBox(hWnd, TEXT("Serial open fail\n"), MB_OK, TRUE);
		return -1;
	}

	if (GetCommState(Robocon_com, &com_dcb))//获取COM状态，配置参数
	{
		com_dcb.BaudRate = BaudRate;//波特率
		com_dcb.fBinary = TRUE;		// 设置二进制模式，此处必须设置TRUE 
		com_dcb.fParity = FALSE;	// 支持奇偶校验
		com_dcb.fOutxCtsFlow = FALSE;// No CTS output flow control
		com_dcb.fOutxDsrFlow = FALSE;// No DSR output flow control
		com_dcb.fDtrControl = DTR_CONTROL_DISABLE;// No DTR flow control
		com_dcb.fDsrSensitivity = FALSE;// DSR sensitivity 
		com_dcb.fTXContinueOnXoff = TRUE;// XOFF continues Tx
		com_dcb.fOutX = FALSE;// No XON/XOFF out flow control
		com_dcb.fInX = FALSE;// No XON/XOFF in flow control
		com_dcb.fErrorChar = FALSE;// Disable error replacement
		com_dcb.fNull = FALSE;// Disable null stripping
		com_dcb.fRtsControl = RTS_CONTROL_DISABLE;//No RTS flow control
		com_dcb.fAbortOnError = FALSE;// 当串口发生错误，并不终止串口读写 
		com_dcb.ByteSize = 8;     //数据位,范围:4-8 
		com_dcb.Parity = NOPARITY;//校验位
		com_dcb.StopBits = ONESTOPBIT;     //停止位
		//com_dcb.fBinary = TRUE;
		//com_dcb.fParity = TRUE;
	}
	

	if (!SetCommState(Robocon_com, &com_dcb))
	{
		/*printf("Serial set fail\n");*/
		MessageBox(hWnd, TEXT("Serial set fail\n"), MB_OK, TRUE);
		return -1;
	}
	SetupComm(Robocon_com, 1024, 1024);//读写缓冲区

	PurgeComm(Robocon_com, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);//清除错误标志

	memset(&tim_out, 0, sizeof(tim_out));
	tim_out.ReadIntervalTimeout = MAXDWORD;//读写时间间隔设置
	tim_out.ReadTotalTimeoutConstant = 0;
	tim_out.ReadTotalTimeoutMultiplier = 10;
	tim_out.WriteTotalTimeoutMultiplier = 50;
	tim_out.WriteTotalTimeoutConstant = 2000;
	SetCommTimeouts(Robocon_com, &tim_out);

	return 1;
}
//串口发送函数
DWORD Com_Send(char *p, int len)
{
	DWORD write_bytes = len;
	COMSTAT comstate;
	DWORD dwErrorFlag;
	BOOL writeFlag;
	OVERLAPPED m_osWrite;

	m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);//信号状态设置
	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;

	writeFlag = WriteFile(Robocon_com, p, write_bytes, &write_bytes, &m_osWrite);//接收

	if (!writeFlag)
	{

		if (GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_osWrite.hEvent, 1000); //后台发送
			return write_bytes;
		}
		/*printf("Write fail\n");*/
		MessageBox(hWnd, TEXT("Write fail\n"), MB_OK, TRUE);
		return 0;
	}
	return write_bytes;//写入字节数
}
//串口读取函数
DWORD Com_Recv(char *p, int len)
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

	ClearCommError(Robocon_com, &dwErrorFlag, &comstate);

	if (!comstate.cbInQue)//缓冲区无数据
		return 0;

	if (read_bytes > comstate.cbInQue)//缓冲区实际数据量
		read_bytes = comstate.cbInQue;

	readstate = ReadFile(Robocon_com, p, read_bytes, &read_bytes, &m_osRead);    //读取到p

	if (!readstate)
	{
		if (GetLastError() == ERROR_IO_PENDING)  //后台接收
		{
			GetOverlappedResult(Robocon_com, &m_osRead, &read_bytes, TRUE);
			return read_bytes;//读取字节数
		}

		/*printf("Read fail\n");*/
		MessageBox(hWnd, TEXT("Read fail\n"), MB_OK, TRUE);
		return 0;
	}
	return read_bytes;
}


int serial_open(LPCWSTR COMx, int BaudRate) 
{

	hCom = CreateFile(COMx, //COM1口    
		GENERIC_READ | GENERIC_WRITE, //允许读和写    
		0, //独占方式    
		NULL,
		OPEN_EXISTING, //打开而不是创建     
		0, //重叠方式FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED  (同步方式设置为0)
		NULL);
	if (hCom == INVALID_HANDLE_VALUE)
	{
		//printf("打开COM失败!\n");
		MessageBox(hWnd, TEXT("打开串口失败"), MB_OK,TRUE);
		return FALSE;
	}
	SetupComm(hCom, 1024, 1024); //输入缓冲区和输出缓冲区的大小都是1024 

								 //设定读写超时 
								 /*COMMTIMEOUTS TimeOuts;
								 TimeOuts.ReadIntervalTimeout=1000;
								 TimeOuts.ReadTotalTimeoutMultiplier=500;
								 TimeOuts.ReadTotalTimeoutConstant=5000; //设定写超时
								 TimeOuts.WriteTotalTimeoutMultiplier=500;
								 TimeOuts.WriteTotalTimeoutConstant = 2000;
								 SetCommTimeouts(hCom, &TimeOuts); //设置超时
								 */
	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = BaudRate;        //设置波特率为BaudRate
	dcb.ByteSize = 4;				//每个字节有8位 
	dcb.Parity = NOPARITY;            //无奇偶校验位 
	dcb.StopBits = ONESTOPBIT;        //一个停止位
	SetCommState(hCom, &dcb);        //设置参数到hCom
	PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);//清空缓存区        //PURGE_TXABORT 中断所有写操作并立即返回，即使写操作还没有完成。
												   //PURGE_RXABORT 中断所有读操作并立即返回，即使读操作还没有完成。
												   //PURGE_TXCLEAR 清除输出缓冲区 
												   //PURGE_RXCLEAR 清除输入缓冲区  
	return TRUE;
}




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
    MyRegisterClass(hInstance);

	Serial_Init(_T("COM2"), CBR_19200);
	char p[12] = {0x7e,0x07,0x02,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0xa1,0x7f};
	Com_Send(p, 12);
	ComboxInit();
    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;//指定窗口类型，各种“类风格”,可以使用按位或操作符组合起来
												  //CS_VREDRAW	移动或者调整窗口的高度（垂直方向）时，重绘整个窗口
												  //CS_HREDRAW	移动或者调整窗口的宽度（水平方向）时，重绘整个窗口
    wcex.lpfnWndProc    = WndProc;				//指定窗口过程（必须是回调函数）
    wcex.cbClsExtra     = 0;					//预留的额外空间，一般为 0
    wcex.cbWndExtra     = 0;					//预留的额外空间，一般为 0
    wcex.hInstance      = hInstance;			//应用程序的实例句柄
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32));	//为所有基于该窗口类的窗口设定一个图标
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);					//为所有基于该窗口类的窗口设定一个鼠标指针
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);							//	指定窗口背景色
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32);						//指定窗口菜单
    wcex.lpszClassName  = szWindowClass;									//指定窗口类名
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);	
}

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

   /*HWND */hWnd = CreateWindowW(szWindowClass, // 窗口类名称
							//szTitle,// 窗口标题
							TEXT("Win32串口"),	// 窗口标题
							WS_OVERLAPPEDWINDOW,	// 窗口风格，或称窗口格式
													//WS_OVERLAPPEDWINDOW相当于（WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX）
							CW_USEDEFAULT,	// 初始 x 坐标
							0,				// 初始 y 坐标
							CW_USEDEFAULT,	// 初始 x 方向尺寸
							0,				// 初始 y 方向尺寸
							nullptr,	// 父窗口句柄
							nullptr,	// 窗口菜单句柄
							hInstance,	// 程序实例句柄
							nullptr);	// 创建参数

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);	//显示窗口
   UpdateWindow(hWnd);			//更新窗口

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:	//按键消息
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case IDM_NEW:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_NewBox), hWnd, NewWin);
				break;
			case IDM_Calculator:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_Calculator), hWnd, Calculator);
				break;
			case IDM_ComboT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_Combox), hWnd, ComboxTest);
				break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case IDC_TEST:
				DestroyWindow(hWnd);

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
			
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
			RECT rect;

			GetClientRect(hWnd, &rect);
			DrawText(hdc, TEXT("绘制窗口1行\n绘制窗口2行\n"), -1, &rect,

				DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_WORD_ELLIPSIS);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
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

// “新窗口”框的消息处理程序。
INT_PTR CALLBACK NewWin(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	//wParam 高两个字节 通知码
	//wParam 低两字节 命令ID
	//lParam 发送命令消息的子窗体句柄。
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{		
	case WM_INITDIALOG:	
		//WM_INITDIALOG消息是对话框才能收到的消息，表明对话框及其所有子控件都创建完毕了。
		//这个状态肯定是在调用显示对话框的函数之前，因此可以在WM_INITDIALOG消息响应函数中添加对编辑框控件的初始化和修改
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		//WM_COMMAND产生的条件：点击菜单， 点击加速键，点击子窗口按钮，点击工具栏按钮。这些时候都有command消息产生。
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));	//是清除一个模态对话框,并使系统中止对对话框的任何处理的函数。
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_TEST)
		{
			//HWND* pEDit= GetDlgItem(hDlg, IDC_EDIT1);
			
			//SetWindowText(hDlg,TEXT("dft"));
			SetDlgItemInt(hDlg, IDC_EDIT1,5,true);
			SetDlgItemText(hDlg, IDC_EDIT2, TEXT("3234"));
			//SetDlgItemText(hDlg, IDC_EDIT2, TEXT("sdfd"));
		}
		if (LOWORD(wParam) == IDC_SetEditText)
		{
			SetDlgItemText(hDlg, IDC_EDIT1, TEXT("编辑框内容已设置"));
			//SetDlgItemText(hDlg, IDC_EDIT2, TEXT("编辑框内容已复制"));
		}
		if (LOWORD(wParam) == IDC_CopyEditdata)
		{
			//SetDlgItemText(hDlg, IDC_EDIT1, TEXT("编辑框内容已设置"));
			wchar_t str[2000];
			GetDlgItemText(hDlg, IDC_EDIT1, str,500);
			SetDlgItemText(hDlg, IDC_EDIT2, str);
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// “计算器”框的消息处理程序。
INT_PTR CALLBACK Calculator(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	//wParam 高两个字节 通知码
	//wParam 低两字节 命令ID
	//lParam 发送命令消息的子窗体句柄。
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		//WM_INITDIALOG消息是对话框才能收到的消息，表明对话框及其所有子控件都创建完毕了。
		//这个状态肯定是在调用显示对话框的函数之前，因此可以在WM_INITDIALOG消息响应函数中添加对编辑框控件的初始化和修改
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		//WM_COMMAND产生的条件：点击菜单， 点击加速键，点击子窗口按钮，点击工具栏按钮。这些时候都有command消息产生。
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));	//是清除一个模态对话框,并使系统中止对对话框的任何处理的函数。
			return (INT_PTR)TRUE;
		}
		//加法按键
		if (LOWORD(wParam) == IDC_ADD)
		{

			//SetDlgItemText(hDlg, IDC_DATA1, TEXT("编辑框内容已设置"));
			//SetDlgItemText(hDlg, IDC_EDIT2, TEXT("编辑框内容已复制"));
			int a = GetDlgItemInt(hDlg, IDC_DATA1, NULL, true);
			int b = GetDlgItemInt(hDlg, IDC_DATA2, NULL, true);
			int c = a+b;
			SetDlgItemInt(hDlg, IDC_RESULT, c, TRUE);
		}
		//减法按键
		if (LOWORD(wParam) == IDC_Subtraction)
		{
			int a = GetDlgItemInt(hDlg, IDC_DATA1, NULL, true);
			int b = GetDlgItemInt(hDlg, IDC_DATA2, NULL, true);
			int c = a - b;
			SetDlgItemInt(hDlg, IDC_RESULT, c, TRUE);
		}
		break;
	}
	return (INT_PTR)FALSE;
}
int ComboxInit(void)
{
	return 0;
}
// “下拉列表测试”框的消息处理程序。
INT_PTR CALLBACK ComboxTest(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	//wParam 高两个字节 通知码
	//wParam 低两字节 命令ID
	//lParam 发送命令消息的子窗体句柄。
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		//WM_INITDIALOG消息是对话框才能收到的消息，表明对话框及其所有子控件都创建完毕了。
		//这个状态肯定是在调用显示对话框的函数之前，因此可以在WM_INITDIALOG消息响应函数中添加对编辑框控件的初始化和修改

		

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		//WM_COMMAND产生的条件：点击菜单， 点击加速键，点击子窗口按钮，点击工具栏按钮。这些时候都有command消息产生。
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));	//是清除一个模态对话框,并使系统中止对对话框的任何处理的函数。
			return (INT_PTR)TRUE;
		}
		//-----------------向Combox添加数据
		HWND hWndComboBox = GetDlgItem(hDlg, IDC_COMBO1);
		TCHAR szMessage[20] = TEXT("2400");
		SendMessage(hWndComboBox, CB_RESETCONTENT, 0, 0);
		SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)TEXT("4800"));
		SendMessage(hWndComboBox, CB_INSERTSTRING, 1, (LPARAM)TEXT("9600"));
		SendMessage(hWndComboBox, CB_INSERTSTRING, 2, (LPARAM)TEXT("19200"));
		SendMessage(hWndComboBox, CB_INSERTSTRING, 3, (LPARAM)TEXT("115200"));
		SendMessage(hWndComboBox, CB_SETCURSEL, 0, 0);//设置默认值
	}
	return (INT_PTR)FALSE;
}