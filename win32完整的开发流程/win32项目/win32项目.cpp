

#include "stdafx.h"
#include "win32项目.h"


#include"stdio.h "

#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

HWND Login_Button;			//登录按钮

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WindowProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


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
    }

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
   HWND hWnd;
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
							500,				// 初始 y 方向尺寸
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

            case IDM_EXIT:
                DestroyWindow(hWnd);	//销毁指定的窗口。这个函数通过发送WM_DESTROY 消息和 WM_NCDESTROY 消息使窗口无效并移除其键盘焦点。
										//这个函数还销毁窗口的菜单，清空线程的消息队列，销毁与窗口过程相关的定时器，解除窗口对剪贴板的拥有权，打断剪贴板器的查看链。
                break;


            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_CREATE:
		//---------------------------------------静态字符
		Login_Button = CreateWindow(TEXT("static"), TEXT("串口号："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 0, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("波特率："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 60, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("校验位："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 120, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("停止位："), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 180, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		//---------------------------------------添加按键
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开RGB"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 5, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开红灯"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开黄灯"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开蓝灯"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 300, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开门锁"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 400, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开背光"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("打开串口"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 240, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("清空数据"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 370, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("发送数据"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		//---------------------------------------下拉列表
		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("串口号"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 30, 80, 100, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, CB_ADDSTRING, 0, (LPARAM)TEXT("COM1"));
		SendMessage(Login_Button, CB_ADDSTRING, 1, (LPARAM)TEXT("COM2"));
		SendMessage(Login_Button, CB_ADDSTRING, 2, (LPARAM)TEXT("COM3"));
		SendMessage(Login_Button, CB_ADDSTRING, 3, (LPARAM)TEXT("COM4"));
		SendMessage(Login_Button, CB_SETCURSEL, 2, 0);//设置默认值

		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("波特率"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 90, 80, 100, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, CB_ADDSTRING, 0, (LPARAM)TEXT("2400"));
		SendMessage(Login_Button, CB_ADDSTRING, 1, (LPARAM)TEXT("9600"));
		SendMessage(Login_Button, CB_ADDSTRING, 2, (LPARAM)TEXT("19200"));
		SendMessage(Login_Button, CB_ADDSTRING, 3, (LPARAM)TEXT("115200"));
		SendMessage(Login_Button, CB_SETCURSEL, 2, 0);//设置默认值

		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("校验位"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 150, 80, 100, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, CB_ADDSTRING, 0, (LPARAM)TEXT("奇校验"));
		SendMessage(Login_Button, CB_ADDSTRING, 1, (LPARAM)TEXT("偶校验"));
		SendMessage(Login_Button, CB_ADDSTRING, 2, (LPARAM)TEXT("无校验"));
		SendMessage(Login_Button, CB_SETCURSEL, 2, 0);//设置默认值

		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("停止位"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 210, 80, 100, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, CB_ADDSTRING, 0, (LPARAM)TEXT("1位"));
		SendMessage(Login_Button, CB_ADDSTRING, 1, (LPARAM)TEXT("1.5位"));
		SendMessage(Login_Button, CB_ADDSTRING, 2, (LPARAM)TEXT("2位"));
		SendMessage(Login_Button, CB_SETCURSEL, 0, 0);//设置默认值

		//---------------------------------------显示框
		Login_Button = CreateWindow(TEXT("edit"), TEXT("接收区："), WS_CHILD | WS_VISIBLE | WS_BORDER /*边框*/ | ES_AUTOHSCROLL /*水平滚动*/| ES_AUTOVSCROLL /*垂直滚动*/ | ES_MULTILINE/*多行*/ | WS_VSCROLL/*垂直滚动条*/ | WS_HSCROLL/*垂直滚动条*/| ES_READONLY/*只读*/, 0, 0, 680, 200, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("edit"), TEXT("发送区："), WS_CHILD | WS_VISIBLE | WS_BORDER /*边框*/ | ES_AUTOHSCROLL /*水平滚动*/ | ES_AUTOVSCROLL /*垂直滚动*/ | ES_MULTILINE/*多行*/ | WS_VSCROLL/*垂直滚动条*/ | WS_HSCROLL/*垂直滚动条*/ /*| ES_READONLY只读*/, 0, 205, 680, 200, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
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
