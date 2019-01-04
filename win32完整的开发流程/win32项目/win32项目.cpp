

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
HANDLE Robocon_com;

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WindowProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	SerailConfiguration(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	ClearData(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int Serial_Init(HWND hDlg,LPCWSTR COMx, int BaudRate)
{
	DCB com_dcb;    //��������
	COMMTIMEOUTS tim_out;

	//Robocon_com = CreateFile(COMx, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	Robocon_com = CreateFile(COMx, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL);
	if (Robocon_com == (HANDLE)-1)
	{
		/*printf("Serial open fail\n");*/
		MessageBox(hDlg, TEXT("���ڴ�ʧ��\n"), MB_OK, TRUE);
		return -1;
	}

	if (GetCommState(Robocon_com, &com_dcb))//��ȡCOM״̬�����ò���
	{
		com_dcb.BaudRate			= BaudRate;//������
		com_dcb.fBinary				= TRUE;		// ���ö�����ģʽ���˴���������TRUE 
		com_dcb.fParity				= FALSE;	// ֧����żУ��
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
		com_dcb.fAbortOnError		= FALSE;	// �����ڷ������󣬲�����ֹ���ڶ�д 
		com_dcb.ByteSize			= 8;		//����λ,��Χ:4-8 
		com_dcb.Parity				= NOPARITY;	//У��λ
		com_dcb.StopBits			= ONESTOPBIT;     //ֹͣλ
										   //com_dcb.fBinary = TRUE;
										   //com_dcb.fParity = TRUE;
	}


	if (!SetCommState(Robocon_com, &com_dcb))
	{
		/*printf("Serial set fail\n");*/
		MessageBox(hDlg, TEXT("��������ʧ��\n"), MB_OK, TRUE);
		return -1;
	}
	SetupComm(Robocon_com, 1024, 1024);//��д������

	PurgeComm(Robocon_com, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);//��������־

	memset(&tim_out, 0, sizeof(tim_out));
	tim_out.ReadIntervalTimeout			= MAXDWORD;//��дʱ��������	
	tim_out.ReadTotalTimeoutMultiplier	= 10;
	tim_out.ReadTotalTimeoutConstant	= 0;
	tim_out.WriteTotalTimeoutMultiplier = 50;
	tim_out.WriteTotalTimeoutConstant	= 2000;
	SetCommTimeouts(Robocon_com, &tim_out);


	CreateEvent(NULL, TRUE, FALSE, NULL);
	CreateEvent(NULL, TRUE, FALSE, NULL);

	return 1;
}
//���ڷ��ͺ���
DWORD Com_Send(HWND hDlg, char *p, int len)
{
	DWORD write_bytes = len;
	COMSTAT comstate;
	DWORD dwErrorFlag;
	BOOL writeFlag=0;
	OVERLAPPED m_osWrite;
	
	DWORD error = 0;
	m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);//�ź�״̬����
	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;

	writeFlag = WriteFile(Robocon_com, p, write_bytes, &write_bytes, &m_osWrite);//����
	if (!writeFlag)
	{

		if (GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_osWrite.hEvent, 1000); //��̨����
			return write_bytes;
		}
		/*printf("Write fail\n");*/
		MessageBox(hDlg, TEXT("����ʧ��\n"), MB_OK, TRUE);
		return 0;
	}
	return write_bytes;//д���ֽ���
}
//���ڶ�ȡ����
DWORD Com_Recv(HWND hDlg, char *p, int len)
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

	if (!comstate.cbInQue)//������������
		return 0;

	if (read_bytes > comstate.cbInQue)//������ʵ��������
		read_bytes = comstate.cbInQue;

	readstate = ReadFile(Robocon_com, p, read_bytes, &read_bytes, &m_osRead);    //��ȡ��p

	if (!readstate)
	{
		if (GetLastError() == ERROR_IO_PENDING)  //��̨����
		{
			GetOverlappedResult(Robocon_com, &m_osRead, &read_bytes, TRUE);
			return read_bytes;//��ȡ�ֽ���
		}

		/*printf("Read fail\n");*/
		MessageBox(hDlg, TEXT("Read fail\n"), MB_OK, TRUE);
		return 0;
	}
	return read_bytes;
}


//int serial_open(LPCWSTR COMx, int BaudRate)
//{
//
//	hCom = CreateFile(COMx, //COM1��    
//		GENERIC_READ | GENERIC_WRITE, //�������д    
//		0, //��ռ��ʽ    
//		NULL,
//		OPEN_EXISTING, //�򿪶����Ǵ���     
//		0, //�ص���ʽFILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED  (ͬ����ʽ����Ϊ0)
//		NULL);
//	if (hCom == INVALID_HANDLE_VALUE)
//	{
//		//printf("��COMʧ��!\n");
//		MessageBox(hWnd, TEXT("�򿪴���ʧ��"), MB_OK, TRUE);
//		return FALSE;
//	}
//	SetupComm(hCom, 1024, 1024); //���뻺����������������Ĵ�С����1024 
//
//								 //�趨��д��ʱ 
//								 /*COMMTIMEOUTS TimeOuts;
//								 TimeOuts.ReadIntervalTimeout=1000;
//								 TimeOuts.ReadTotalTimeoutMultiplier=500;
//								 TimeOuts.ReadTotalTimeoutConstant=5000; //�趨д��ʱ
//								 TimeOuts.WriteTotalTimeoutMultiplier=500;
//								 TimeOuts.WriteTotalTimeoutConstant = 2000;
//								 SetCommTimeouts(hCom, &TimeOuts); //���ó�ʱ
//								 */
//	DCB dcb;
//	GetCommState(hCom, &dcb);
//	dcb.BaudRate = BaudRate;        //���ò�����ΪBaudRate
//	dcb.ByteSize = 4;				//ÿ���ֽ���8λ 
//	dcb.Parity = NOPARITY;            //����żУ��λ 
//	dcb.StopBits = ONESTOPBIT;        //һ��ֹͣλ
//	SetCommState(hCom, &dcb);        //���ò�����hCom
//	PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ�����        //PURGE_TXABORT �ж�����д�������������أ���ʹд������û����ɡ�
//												   //PURGE_RXABORT �ж����ж��������������أ���ʹ��������û����ɡ�
//												   //PURGE_TXCLEAR ������������ 
//												   //PURGE_RXCLEAR ������뻺����  
//	return TRUE;
//}
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
							550,				// ��ʼ y ����ߴ�
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
			case IDB_PortCtl:
				SerailConfiguration(hWnd,message,wParam,lParam);
				break;
			case IDB_ClearData:
				ClearData(hWnd, message, wParam, lParam);
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
		Login_Button = CreateWindow(TEXT("static"), TEXT("���ںţ�"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 0, 80, 20, hWnd, (HMENU)IDS_COMx, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("�����ʣ�"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 60, 80, 20, hWnd, (HMENU)IDS_BaudRate, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("У��λ��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 120, 80, 20, hWnd, (HMENU)��żУ��, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("ֹͣλ��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 180, 80, 20, hWnd, (HMENU)ֹͣλ, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		
		Login_Button = CreateWindow(TEXT("static"), TEXT("���ռ�����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 280, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("���ͼ�����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 325, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		Login_Button = CreateWindow(TEXT("static"), TEXT("״̬����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 5, 470, 800, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		Login_Button = CreateWindow(TEXT("static"), TEXT("Address1��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 5, 410, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("Address2��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, 410, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("static"), TEXT("Address3��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 400, 410, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		//---------------------------------------��Ӱ���
		Login_Button = CreateWindow(TEXT("button"), TEXT("�򿪴���"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 375, 80, 30, hWnd, (HMENU)IDB_PortCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("�������"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 405, 80, 30, hWnd, (HMENU)IDB_ClearData, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("��������"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 435, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		Login_Button = CreateWindow(TEXT("button"), TEXT("��RGB"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 5, 435, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("�򿪺��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 435, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("�򿪻Ƶ�"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, 435, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("������"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 300, 435, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("������"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 400, 435, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("�򿪱���"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 435, 80, 30, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		//---------------------------------------ѡ���
		Login_Button = CreateWindow(TEXT("button"), TEXT("Hex����"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 700, 235, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("Hex����"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 700, 255, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		//---------------------------------------�����б�
		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("���ں�"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 30, 80, 100, hWnd, (HMENU)IDC_COMx, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		for (int i = 0; i <= 16; i++)
		{
			wchar_t str[80];
			//sprintf(str, "%x", i); //��10.8תΪ�ַ���
			//itoa(i, str,10);
			swprintf_s(str, 16, L"COM%d", i); //��x=1234�����buffer
			wprintf(str);
			SendMessage(Login_Button, CB_ADDSTRING, i, (LPARAM)(str));
		}
		SendMessage(Login_Button, CB_SETCURSEL, 2, 0);//����Ĭ��ֵ

		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("������"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 90, 80, 100, hWnd, (HMENU)IDB_BaudRate, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
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
		SendMessage(Login_Button, CB_SETCURSEL, 5, 0);//����Ĭ��ֵ

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

		

		//---------------------------------------�����б�Address1
		Login_Button = CreateWindow(TEXT("Listbox"), TEXT("Address1"), WS_CHILD |  WS_VSCROLL|WS_BORDER | WS_VISIBLE | LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, 100, 410, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		wchar_t str[80];
		for (int i = 0; i <= 16; i++)
		{
			//sprintf(str, "%x", i); //��10.8תΪ�ַ���
			//itoa(i, str,10);
			swprintf_s(str,4,L"%d", i); //��x=1234�����buffer
			wprintf(str);
			SendMessage(Login_Button, LB_ADDSTRING, i, (LPARAM)(str));			
		}
		SendMessage(Login_Button, LB_ADDSTRING, 17, (LPARAM)(TEXT("0xFF")));
		SendMessage(Login_Button, LB_SETCURSEL, (WPARAM)0, 0);
		//---------------------------------------�����б�Address2
		Login_Button = CreateWindow(TEXT("Listbox"), TEXT("Address2"), WS_CHILD | WS_VSCROLL | WS_BORDER | WS_VISIBLE | LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, 300, 410, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		for (int i = 0; i <= 16; i++)
		{
			//sprintf(str, "%x", i); //��10.8תΪ�ַ���
			//itoa(i, str,10);
			swprintf_s(str, 4, L"%d", i); //��x=1234�����buffer
			wprintf(str);
			SendMessage(Login_Button, LB_ADDSTRING, i, (LPARAM)(str));
		}
		SendMessage(Login_Button, LB_ADDSTRING, 17, (LPARAM)(TEXT("0xFF")));
		SendMessage(Login_Button, LB_SETCURSEL, (WPARAM)0, 0);
		//---------------------------------------�����б�Address3
		Login_Button = CreateWindow(TEXT("Listbox"), TEXT("Address3"), WS_CHILD | WS_VSCROLL | WS_BORDER | WS_VISIBLE | LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, 500, 410, 80, 20, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		for (int i = 0; i <= 16; i++)
		{
			//sprintf(str, "%x", i); //��10.8תΪ�ַ���
			//itoa(i, str,10);
			swprintf_s(str, 4, L"%d", i); //��x=1234�����buffer
			wprintf(str);
			SendMessage(Login_Button, LB_ADDSTRING, i, (LPARAM)(str));
		}
		SendMessage(Login_Button, LB_ADDSTRING, 17, (LPARAM)(TEXT("0xFF")));
		SendMessage(Login_Button, LB_SETCURSEL, (WPARAM)0, 0);

		

		//---------------------------------------��ʾ��
		Login_Button = CreateWindow(TEXT("edit"), TEXT("��������"), WS_CHILD | WS_VISIBLE | WS_BORDER /*�߿�*/ | ES_AUTOHSCROLL /*ˮƽ����*/| ES_AUTOVSCROLL /*��ֱ����*/ | ES_MULTILINE/*����*/ | WS_VSCROLL/*��ֱ������*/ | WS_HSCROLL/*��ֱ������*/| ES_READONLY/*ֻ��*/, 0, 0, 695, 200, hWnd, (HMENU)IDE_Receive, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("edit"), TEXT("��������"), WS_CHILD | WS_VISIBLE | WS_BORDER /*�߿�*/ | ES_AUTOHSCROLL /*ˮƽ����*/ | ES_AUTOVSCROLL /*��ֱ����*/ | ES_MULTILINE/*����*/ | WS_VSCROLL/*��ֱ������*/ | WS_HSCROLL/*��ֱ������*/ /*| ES_READONLYֻ��*/, 0, 205, 695, 200, hWnd, (HMENU)IDE_Send, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		Login_Button = CreateWindow(TEXT("edit"), TEXT("���ռ���"), WS_CHILD | WS_VISIBLE | WS_BORDER /*�߿�*/ | ES_READONLY/*ֻ��*/, 700, 300, 80, 20, hWnd, (HMENU)IDS_ReceiveCount, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("edit"), TEXT("���ͼ���"), WS_CHILD | WS_VISIBLE | WS_BORDER /*�߿�*/  | ES_READONLY/*ֻ��*/, 700, 345, 80, 20, hWnd, (HMENU)IDS_SendCount, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
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
// ���򿪴��ڡ������Ϣ�������
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
			//MessageBox(hDlg, TEXT("�����˴򿪴���"), TEXT("��ʾ����"), MB_OK);
			//SetWindowText(hDlg, TEXT("�رմ���"));
			//SetDlgItemText(hDlg, �򿪴���, TEXT("�رմ���"));
			if (0 == portflag)
			{
				//LPCWSTR COMx;
				//TCHAR  text[20];
				//HWND hCombox;
				//int BaudRate;
				//hCombox = GetDlgItem(hDlg, IDB_BaudRate);
				////GetDlgItemText(combox, ���ں�, text, 10);
				//SendMessage(hCombox, CB_GETLBTEXT, 1, (LPARAM)text);

				//BaudRate = (int)SendMessage(hCombox, CB_GETCURSEL, 0, 0L);
				//BaudRate = GetDlgItemInt(hDlg, ������, NULL, TRUE);
				HWND hDct;
				LPCWSTR COMx;
				TCHAR  text[20];
				int BaudRate;
				
				//-------------------------��ȡ��ز���
				GetDlgItemText(hDlg, IDC_COMx, (LPWSTR)text, 10);
				BaudRate = GetDlgItemInt(hDlg, IDB_BaudRate, NULL, false);
				portflag = Serial_Init(hDlg, (LPWSTR)text, BaudRate);
				if (1 == portflag)	//�򿪴��ڳɹ�
				{
					//-------------------------������ؿؼ�
					hDct = GetDlgItem(hDlg, IDC_COMx);
					EnableWindow(hDct, FALSE);
					hDct = GetDlgItem(hDlg, IDB_BaudRate);
					EnableWindow(hDct, FALSE);
					//-------------------------���¿ؼ�
					SetDlgItemText(hDlg, IDB_PortCtl, TEXT("�رմ���"));	//�޸Ŀؼ�����
					SetDlgItemInt(hDlg, IDS_ReceiveCount, 0, TRUE);
					SetDlgItemInt(hDlg, IDS_SendCount, 0, TRUE);
					portflag = 1;
					char p[3] = { 0x01,0x02,0x03 };
					Com_Send(hDlg, p, 2);
				}				
			}
			else
			{				
				if (0 == CloseHandle(Robocon_com))
				{
					MessageBox(hDlg, TEXT("�رմ���ʧ��"),MB_OK,NULL);
				}
				else
				{
					HWND hDct;
					//-------------------------������ؿؼ�
					hDct = GetDlgItem(hDlg, IDC_COMx);
					EnableWindow(hDct, TRUE);
					hDct = GetDlgItem(hDlg, IDB_BaudRate);
					EnableWindow(hDct, TRUE);
					//-------------------------���¿ؼ�
					SetDlgItemText(hDlg, IDB_PortCtl, TEXT("�򿪴���"));
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
// ���򿪴��ڡ������Ϣ�������
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
			SetDlgItemText(hDlg, IDE_Receive, TEXT(""));
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
