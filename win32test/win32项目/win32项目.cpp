// win32��Ŀ.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "win32��Ŀ.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    NewWin(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Calculator(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ComboxTest(HWND, UINT, WPARAM, LPARAM);

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
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32));

    MSG msg;

    // ����Ϣѭ��: 
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;//ָ���������ͣ����֡�����,����ʹ�ð�λ��������������
												  //CS_VREDRAW	�ƶ����ߵ������ڵĸ߶ȣ���ֱ����ʱ���ػ���������
												  //CS_HREDRAW	�ƶ����ߵ������ڵĿ�ȣ�ˮƽ����ʱ���ػ���������
    wcex.lpfnWndProc    = WndProc;				//ָ�����ڹ��̣������ǻص�������
    wcex.cbClsExtra     = 0;					//Ԥ���Ķ���ռ䣬һ��Ϊ 0
    wcex.cbWndExtra     = 0;					//Ԥ���Ķ���ռ䣬һ��Ϊ 0
    wcex.hInstance      = hInstance;			//Ӧ�ó����ʵ�����
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32));	//Ϊ���л��ڸô�����Ĵ����趨һ��ͼ��
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);					//Ϊ���л��ڸô�����Ĵ����趨һ�����ָ��
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);							//	ָ�����ڱ���ɫ
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32);						//ָ�����ڲ˵�
    wcex.lpszClassName  = szWindowClass;									//ָ����������
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);			
}

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

   HWND hWnd = CreateWindowW(szWindowClass, // ����������
							//szTitle,// ���ڱ���
							TEXT("Win32ѧϰ��Ŀ"),	// ���ڱ���
							WS_OVERLAPPEDWINDOW,	// ���ڷ�񣬻�ƴ��ڸ�ʽ
													//WS_OVERLAPPEDWINDOW�൱�ڣ�WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX��
							CW_USEDEFAULT,	// ��ʼ x ����
							0,				// ��ʼ y ����
							CW_USEDEFAULT,	// ��ʼ x ����ߴ�
							0,				// ��ʼ y ����ߴ�
							nullptr,	// �����ھ��
							nullptr,	// ���ڲ˵����
							hInstance,	// ����ʵ�����
							nullptr);	// ��������

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);	//��ʾ����
   UpdateWindow(hWnd);			//���´���

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
			RECT rect;

			GetClientRect(hWnd, &rect);
			DrawText(hdc, TEXT("���ƴ���1��\n���ƴ���2��\n"), -1, &rect,

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

// ���´��ڡ������Ϣ�������
INT_PTR CALLBACK NewWin(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	//wParam �������ֽ� ֪ͨ��
	//wParam �����ֽ� ����ID
	//lParam ����������Ϣ���Ӵ�������
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{		
	case WM_INITDIALOG:	
		//WM_INITDIALOG��Ϣ�ǶԻ�������յ�����Ϣ�������Ի����������ӿؼ�����������ˡ�
		//���״̬�϶����ڵ�����ʾ�Ի���ĺ���֮ǰ����˿�����WM_INITDIALOG��Ϣ��Ӧ��������ӶԱ༭��ؼ��ĳ�ʼ�����޸�
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		//WM_COMMAND����������������˵��� ������ټ�������Ӵ��ڰ�ť�������������ť����Щʱ����command��Ϣ������
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));	//�����һ��ģ̬�Ի���,��ʹϵͳ��ֹ�ԶԻ�����κδ���ĺ�����
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
			SetDlgItemText(hDlg, IDC_EDIT1, TEXT("�༭������������"));
			//SetDlgItemText(hDlg, IDC_EDIT2, TEXT("�༭�������Ѹ���"));
		}
		if (LOWORD(wParam) == IDC_CopyEditdata)
		{
			//SetDlgItemText(hDlg, IDC_EDIT1, TEXT("�༭������������"));
			wchar_t str[2000];
			GetDlgItemText(hDlg, IDC_EDIT1, str,500);
			SetDlgItemText(hDlg, IDC_EDIT2, str);
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// ���������������Ϣ�������
INT_PTR CALLBACK Calculator(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	//wParam �������ֽ� ֪ͨ��
	//wParam �����ֽ� ����ID
	//lParam ����������Ϣ���Ӵ�������
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		//WM_INITDIALOG��Ϣ�ǶԻ�������յ�����Ϣ�������Ի����������ӿؼ�����������ˡ�
		//���״̬�϶����ڵ�����ʾ�Ի���ĺ���֮ǰ����˿�����WM_INITDIALOG��Ϣ��Ӧ��������ӶԱ༭��ؼ��ĳ�ʼ�����޸�
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		//WM_COMMAND����������������˵��� ������ټ�������Ӵ��ڰ�ť�������������ť����Щʱ����command��Ϣ������
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));	//�����һ��ģ̬�Ի���,��ʹϵͳ��ֹ�ԶԻ�����κδ���ĺ�����
			return (INT_PTR)TRUE;
		}
		//�ӷ�����
		if (LOWORD(wParam) == IDC_ADD)
		{

			//SetDlgItemText(hDlg, IDC_DATA1, TEXT("�༭������������"));
			//SetDlgItemText(hDlg, IDC_EDIT2, TEXT("�༭�������Ѹ���"));
			int a = GetDlgItemInt(hDlg, IDC_DATA1, NULL, true);
			int b = GetDlgItemInt(hDlg, IDC_DATA2, NULL, true);
			int c = a+b;
			SetDlgItemInt(hDlg, IDC_RESULT, c, TRUE);
		}
		//��������
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
// �������б���ԡ������Ϣ�������
INT_PTR CALLBACK ComboxTest(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	//wParam �������ֽ� ֪ͨ��
	//wParam �����ֽ� ����ID
	//lParam ����������Ϣ���Ӵ�������
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		//WM_INITDIALOG��Ϣ�ǶԻ�������յ�����Ϣ�������Ի����������ӿؼ�����������ˡ�
		//���״̬�϶����ڵ�����ʾ�Ի���ĺ���֮ǰ����˿�����WM_INITDIALOG��Ϣ��Ӧ��������ӶԱ༭��ؼ��ĳ�ʼ�����޸�

		

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		//WM_COMMAND����������������˵��� ������ټ�������Ӵ��ڰ�ť�������������ť����Щʱ����command��Ϣ������
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));	//�����һ��ģ̬�Ի���,��ʹϵͳ��ֹ�ԶԻ�����κδ���ĺ�����
			return (INT_PTR)TRUE;
		}
		//-----------------��Combox�������
		HWND hWndComboBox = GetDlgItem(hDlg, IDC_COMBO1);
		TCHAR szMessage[20] = TEXT("2400");
		SendMessage(hWndComboBox, CB_RESETCONTENT, 0, 0);
		SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)TEXT("4800"));
		SendMessage(hWndComboBox, CB_INSERTSTRING, 1, (LPARAM)TEXT("9600"));
		SendMessage(hWndComboBox, CB_INSERTSTRING, 2, (LPARAM)TEXT("19200"));
		SendMessage(hWndComboBox, CB_INSERTSTRING, 3, (LPARAM)TEXT("115200"));
		SendMessage(hWndComboBox, CB_SETCURSEL, 0, 0);//����Ĭ��ֵ
	}
	return (INT_PTR)FALSE;
}