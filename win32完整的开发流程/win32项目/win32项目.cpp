

#include "stdafx.h"
#include "win32��Ŀ.h"


#include"stdio.h "

#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

HWND Login_Button;			//��¼��ť

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WindowProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


//------------------------һ��WinMain��ڵ�
// win32��Ŀ.cpp : ����Ӧ�ó������ڵ㡣
//
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �ڴ˷��ô��롣

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);		//���������ע�ᴰ����

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))	//������������ʾ����
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32));	//������ټ����ú�������ָ���ļ��ټ���

    MSG msg;

    // �塢��Ϣѭ�� 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
			/*TranslateMessage������ת��������Ϣ�ģ���Ϊ���̰��º͵����
			����WM_KEYDOWN��WM_KEYUP��Ϣ�����������ֻ��֪���û�������
			Щ�ַ�������������԰���Щ��Ϣת��ΪWM_CHAR��Ϣ������ʾ�ľ�
			�Ǽ��̰��µ��Ǹ������ַ����硰A�����������Ǵ��������͸������ˡ�*/
            DispatchMessage(&msg);
			/*DispatchMessage�����Ǳ�����õģ����Ĺ��ܾ��൱��һ�����ʹ���
			ÿ�յ�һ����Ϣ��DispatchMessage�����������Ϣ����WindowProc��
			���ǵĴ������������������������������Ƕ����WindowProc����
			Զ���ղ�����Ϣ����Ͳ�������Ϣ��Ӧ�ˣ���ĳ����ֻ�ܴ����оͿ�ʼ�����ˣ�û����Ӧ��*/
        }
    }

    return (int) msg.wParam;
}


//------------------------���������ע�ᴰ����
//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	//**************��ע�ᴰ����֮ǰ�����������WNDCLASS�ṹ��
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;//ָ���������ͣ����֡�����,����ʹ�ð�λ��������������
												  //CS_VREDRAW	�ƶ����ߵ������ڵĸ߶ȣ���ֱ����ʱ���ػ���������
												  //CS_HREDRAW	�ƶ����ߵ������ڵĿ�ȣ�ˮƽ����ʱ���ػ���������
    wcex.lpfnWndProc    = WindowProc;				//ָ�����ڹ��̣������ǻص�������
    wcex.cbClsExtra     = 0;					//Ԥ���Ķ���ռ䣬һ��Ϊ 0
    wcex.cbWndExtra     = 0;					//Ԥ���Ķ���ռ䣬һ��Ϊ 0
    wcex.hInstance      = hInstance;			//Ӧ�ó����ʵ�����
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32));	//Ϊ���л��ڸô�����Ĵ����趨һ��ͼ��
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);					//Ϊ���л��ڸô�����Ĵ����趨һ�����ָ��
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);							//ָ�����ڱ���ɫ
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32);						//ָ�����ڲ˵�
    wcex.lpszClassName  = szWindowClass;									//ָ����������

	//wcex.style = CS_HREDRAW | CS_VREDRAW;//ָ���������ͣ����֡�����,����ʹ�ð�λ��������������
	//									 //CS_VREDRAW	�ƶ����ߵ������ڵĸ߶ȣ���ֱ����ʱ���ػ���������
	//									 //CS_HREDRAW	�ƶ����ߵ������ڵĿ�ȣ�ˮƽ����ʱ���ػ���������
	//wcex.lpfnWndProc = WindowProc;				//ָ�����ڹ��̣������ǻص�������
	//wcex.cbClsExtra = 0;					//Ԥ���Ķ���ռ䣬һ��Ϊ 0
	//wcex.cbWndExtra = 0;					//Ԥ���Ķ���ռ䣬һ��Ϊ 0
	//wcex.hInstance = hInstance;			//Ӧ�ó����ʵ�����
	//wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	//wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32));	//Ϊ���л��ڸô�����Ĵ����趨һ��ͼ��
	//wcex.hCursor = nullptr;					//Ϊ���л��ڸô�����Ĵ����趨һ�����ָ��
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);							//ָ�����ڱ���ɫ
	//wcex.lpszMenuName = nullptr;						//ָ�����ڲ˵�
	//wcex.lpszClassName = szWindowClass;									//ָ����������
    

    return RegisterClassExW(&wcex);		//�����ϵͳע�ᴰ����:��ϵͳע�ᣬ����ϵͳ����֪�������������Ĵ���	
}
//------------------------������������ʾ����
//������ע����ɺ󣬾�Ӧ�ô������ڣ�Ȼ����ʾ���ڣ�����CreateWindow�������ڣ�
//����ɹ����᷵��һ�����ڵľ�������Ƕ�������ڵĲ�����Ҫ�õ���������
//ʲô�Ǿ���أ���ʵ������һ�����֣�ֻ��һ����ʶ���ѣ��ڴ��л���ڸ�����Դ��
//��ͼ�ꡢ�ı��ȣ�Ϊ�˿�����Ч��ʶ��Щ��Դ��ÿһ����Դ������Ψһ�ı�ʶ����������
//ͨ�����ұ�ʶ�����Ϳ���֪��ĳ����Դ�������ڴ�����һ���ַ�У��ͺñ�������ʱ��
//������ҪΪ��ȡ�����֣���˵���������������־���������ʶ��ģ���Ȼ�������A��С����
//����B�ֽ�С������˭֪���ĸ�����С�������ͺ������ϴ�ѧȥ�����ţ���Ϊ�����һ��������
//��Уѧ����Ψһ��ʶ���ѧ�ţ�����ѧ����ѧ�Ŷ��ǲ�ͬ�ģ�������ֻҪͨ������ѧ�ţ��Ϳ�
//���ҵ�������ϡ�
//CreateWindow��������һ��HWND���ͣ������Ǵ�����ľ��

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����
   HWND hWnd;
   //--------------------------------����
   RECT rc, rc1, rctomove;
   int width = GetSystemMetrics(SM_CXSCREEN);
   int height = GetSystemMetrics(SM_CYSCREEN);
   rc.left = 0;
   rc.top = 0;
   rc.right = width;
   rc.bottom = height;
   

  hWnd = CreateWindowW(szWindowClass, // ����������
							//szTitle,// ���ڱ���
							TEXT("win32�����Ŀ�������"),	// ���ڱ���
							//WS_OVERLAPPEDWINDOW,	// ���ڷ�񣬻�ƴ��������ʽ
													//WS_OVERLAPPEDWINDOW�൱�ڣ�WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX��
							WS_OVERLAPPEDWINDOW^WS_THICKFRAME,		//��ֹ�ı��С
							0,	// ��ʼ x ����
							0,				// ��ʼ y ����
							800,	// ��ʼ x ����ߴ�
							500,				// ��ʼ y ����ߴ�
							nullptr,	// �����ھ��,û�и����ڣ�ΪNULL 
							nullptr,	// ���ڲ˵����,û�в˵���ΪNULL
							hInstance,	// ��ǰӦ�ó����ʵ�����
							nullptr);	// ��������,û�и������ݣ�ΪNULL

  GetClientRect(hWnd, &rc1);//�ú�����ȡ���ڿͻ����Ĵ�С

  rctomove.left		= (rc.right - rc.left) / 2 - (rc1.right - rc1.left) / 2;
  rctomove.right	= (rc.right - rc.left) / 2 + (rc1.right - rc1.left) / 2;
  rctomove.top		= (rc.bottom - rc.top) / 2 - (rc1.bottom - rc1.top) / 2;
  rctomove.bottom	= (rc.bottom - rc.top) / 2 + (rc1.bottom - rc1.top) / 2;
  ::SetWindowPos(hWnd, HWND_TOPMOST, rctomove.left, rctomove.top, rc1.right - rc1.left, rc1.bottom - rc1.top, SWP_NOZORDER |SWP_FRAMECHANGED | SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);


   //HWND hWnd = CreateWindowW(szWindowClass, // ����������
			//								//szTitle,// ���ڱ���
	  // TEXT("win32�����Ŀ�������"),	// ���ڱ���
	  // WS_OVERLAPPEDWINDOW,	// ���ڷ�񣬻�ƴ��������ʽ
			//				//WS_OVERLAPPEDWINDOW�൱�ڣ�WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX��
	  // CW_USEDEFAULT,	// ��ʼ x ����
	  // 0,				// ��ʼ y ����
	  // CW_USEDEFAULT,	// ��ʼ x ����ߴ�
	  // 0,				// ��ʼ y ����ߴ�
	  // nullptr,	// �����ھ��,û�и����ڣ�ΪNULL 
	  // nullptr,	// ���ڲ˵����,û�в˵���ΪNULL
	  // hInstance,	// ��ǰӦ�ó����ʵ�����
	  // nullptr);	// ��������,û�и������ݣ�ΪNULL

   if (!hWnd)		//��鴰���Ƿ񴴽��ɹ�
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);	//��ʾ����
	//------------------------�ġ����´��ڣ���ѡ��
   UpdateWindow(hWnd);			//���´���

   return TRUE;
}

//------------------------������Ϣ��Ӧ
//
//  ����: WindowProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:	//������Ϣ
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;

            case IDM_EXIT:
                DestroyWindow(hWnd);	//����ָ���Ĵ��ڡ��������ͨ������WM_DESTROY ��Ϣ�� WM_NCDESTROY ��Ϣʹ������Ч���Ƴ�����̽��㡣
										//������������ٴ��ڵĲ˵�������̵߳���Ϣ���У������봰�ڹ�����صĶ�ʱ����������ڶԼ������ӵ��Ȩ����ϼ��������Ĳ鿴����
                break;


            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_CREATE:
		//---------------------------------------��̬�ַ�
		Login_Button = CreateWindow(TEXT("static"), TEXT("���ںţ�"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 0, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("�����ʣ�"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 60, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("У��λ��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 120, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("ֹͣλ��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 180, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		//---------------------------------------��Ӱ���
		Login_Button = CreateWindow(TEXT("button"), TEXT("��RGB"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 5, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("�򿪺��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("�򿪻Ƶ�"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("������"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 300, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("������"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 400, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("�򿪱���"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("�򿪴���"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 240, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("�������"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 370, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("��������"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 410, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		//---------------------------------------�����б�
		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("���ں�"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 30, 80, 100, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, CB_ADDSTRING, 0, (LPARAM)TEXT("COM1"));
		SendMessage(Login_Button, CB_ADDSTRING, 1, (LPARAM)TEXT("COM2"));
		SendMessage(Login_Button, CB_ADDSTRING, 2, (LPARAM)TEXT("COM3"));
		SendMessage(Login_Button, CB_ADDSTRING, 3, (LPARAM)TEXT("COM4"));
		SendMessage(Login_Button, CB_SETCURSEL, 2, 0);//����Ĭ��ֵ

		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("������"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 90, 80, 100, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, CB_ADDSTRING, 0, (LPARAM)TEXT("2400"));
		SendMessage(Login_Button, CB_ADDSTRING, 1, (LPARAM)TEXT("9600"));
		SendMessage(Login_Button, CB_ADDSTRING, 2, (LPARAM)TEXT("19200"));
		SendMessage(Login_Button, CB_ADDSTRING, 3, (LPARAM)TEXT("115200"));
		SendMessage(Login_Button, CB_SETCURSEL, 2, 0);//����Ĭ��ֵ

		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("У��λ"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 150, 80, 100, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, CB_ADDSTRING, 0, (LPARAM)TEXT("��У��"));
		SendMessage(Login_Button, CB_ADDSTRING, 1, (LPARAM)TEXT("żУ��"));
		SendMessage(Login_Button, CB_ADDSTRING, 2, (LPARAM)TEXT("��У��"));
		SendMessage(Login_Button, CB_SETCURSEL, 2, 0);//����Ĭ��ֵ

		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("ֹͣλ"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 210, 80, 100, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, CB_ADDSTRING, 0, (LPARAM)TEXT("1λ"));
		SendMessage(Login_Button, CB_ADDSTRING, 1, (LPARAM)TEXT("1.5λ"));
		SendMessage(Login_Button, CB_ADDSTRING, 2, (LPARAM)TEXT("2λ"));
		SendMessage(Login_Button, CB_SETCURSEL, 0, 0);//����Ĭ��ֵ

		//---------------------------------------��ʾ��
		Login_Button = CreateWindow(TEXT("edit"), TEXT("��������"), WS_CHILD | WS_VISIBLE | WS_BORDER /*�߿�*/ | ES_AUTOHSCROLL /*ˮƽ����*/| ES_AUTOVSCROLL /*��ֱ����*/ | ES_MULTILINE/*����*/ | WS_VSCROLL/*��ֱ������*/ | WS_HSCROLL/*��ֱ������*/| ES_READONLY/*ֻ��*/, 0, 0, 680, 200, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("edit"), TEXT("��������"), WS_CHILD | WS_VISIBLE | WS_BORDER /*�߿�*/ | ES_AUTOHSCROLL /*ˮƽ����*/ | ES_AUTOVSCROLL /*��ֱ����*/ | ES_MULTILINE/*����*/ | WS_VSCROLL/*��ֱ������*/ | WS_HSCROLL/*��ֱ������*/ /*| ES_READONLYֻ��*/, 0, 205, 680, 200, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
			
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
			/*RECT rect;

			GetClientRect(hWnd, &rect);
			DrawText(hdc, TEXT("���ƴ���1��\n���ƴ���2��\n"), -1, &rect,

				DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_WORD_ELLIPSIS);*/

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);	//�ú�����ϵͳ�����и��߳�����ֹ����ͨ��������ӦWM_DESTROY��Ϣ��
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// �����ڡ������Ϣ�������
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
