

#include "stdafx.h"
#include "win32��Ŀ.h"


#include"stdio.h "

#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 

unsigned char rxdbuffer[2048] = { 0 };

HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

//--------------------�̴߳���
//ԭ�ģ�https ://blog.csdn.net/wowocpp/article/details/80594010
DWORD ThreadProcWrite(LPVOID pParam);
DWORD ThreadProcRead(LPVOID pParam);
OVERLAPPED Wol = { 0 };
OVERLAPPED Rol = { 0 };
HANDLE hThreadWrite;
HANDLE hThreadRead;

 


HWND Login_Button;			//��¼��ť
HANDLE hCom;
HWND hWnd;
// �˴���ģ���а����ĺ�����ǰ������: 
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
	DCB com_dcb;    //��������
	COMMTIMEOUTS tim_out;

	//Robocon_com = CreateFile(COMx, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	hCom = CreateFile(COMx, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL);
	if (hCom == (HANDLE)-1)
	{
		/*printf("Serial open fail\n");*/
		MessageBox(hDlg, TEXT("���ڴ�ʧ��\n"), MB_OK, TRUE);
		return -1;
	}

	if (GetCommState(hCom, &com_dcb))//��ȡCOM״̬�����ò���
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


	if (!SetCommState(hCom, &com_dcb))
	{
		/*printf("Serial set fail\n");*/
		MessageBox(hDlg, TEXT("��������ʧ��\n"), MB_OK, TRUE);
		return -1;
	}
	SetupComm(hCom, 1024, 1024);//��д������

	PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);//��������־

	memset(&tim_out, 0, sizeof(tim_out));
	tim_out.ReadIntervalTimeout			= MAXDWORD;//��дʱ��������	
	tim_out.ReadTotalTimeoutMultiplier	= 10;
	tim_out.ReadTotalTimeoutConstant	= 0;
	tim_out.WriteTotalTimeoutMultiplier = 50;
	tim_out.WriteTotalTimeoutConstant	= 2000;
	SetCommTimeouts(hCom, &tim_out);


	CreateEvent(NULL, TRUE, FALSE, NULL);
	CreateEvent(NULL, TRUE, FALSE, NULL);
	SetCommMask(hCom, EV_RXCHAR);	//�¼�--���յ�һ���ֽ�
	return 1;
}

//https://blog.csdn.net/rabbitjerry/article/details/75384688
DWORD WINAPI ThreadWrite(LPVOID lpParameter)//����1
{
	char outputData[100] = { 0x00 };//������ݻ���
	if (hCom == INVALID_HANDLE_VALUE)
	{
		puts("�򿪴���ʧ��");
		return 0;
	}
	DWORD strLength = 0;
	while (1)
	{
		for (int i = 0; i<100; i++)
		{
			outputData[i] = 0;
		}
		fgets(outputData, 100, stdin);     // �ӿ���̨�����ַ���
		strLength = strlen(outputData);
		printf("string length is %d\n", strLength);              // ��ӡ�ַ�������
		WriteFile(hCom, outputData, strLength, &strLength, NULL); // ���ڷ����ַ���
		fflush(stdout);
		PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);            //��ջ�����
		Sleep(100);
	}
	return 0;
}



DWORD WINAPI ThreadRead(LPVOID lpParameter)
{
	if (hCom == INVALID_HANDLE_VALUE)   //INVALID_HANDLE_VALUE��ʾ����������GetLastError
	{
		puts("�򿪴���ʧ��");
		return 0;
	}
	unsigned char getputData[1024] = { 0x00 };//�������ݻ���
									// ���ô�����Ϣ����ȡ���봮�ڻ��������ݵ��ֽ���

	DWORD dwErrors;     // ������Ϣ
	COMSTAT Rcs;        // COMSTAT�ṹͨ���豸�ĵ�ǰ��Ϣ
	int Len = 0;
	DWORD length = 512;               //�������ն�ȡ���ֽ���
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
			&Rcs);                                // ��ȡ�����������ݳ���
		Len = Rcs.cbInQue;
		ReadFile(hCom, getputData, Len, &length, NULL);   //��ȡ�ַ���
		PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);  //��ջ�����
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

//------------------------���ڽ����߳�
DWORD ThreadProcRead(LPVOID   pParam)
{
	BYTE    myByte[20];
	DWORD    dwRes;
	DWORD    dwRead;

	DWORD   dwRet;

	BOOL bResult;

	memset(myByte, 0, sizeof(myByte));

	Rol.hEvent = CreateEvent(NULL,          //����Rol��hEvent��ԱΪ���ź�״̬
		TRUE,
		FALSE,
		NULL);

	if (Rol.hEvent == NULL)
	{
		printf("hEvent is Empty\r\n");
		return -1;
	}


	bResult = ReadFile(hCom, //���ھ��
		&myByte,     //��Ŷ�ȡ����
		9,         //Ҫ��ȡ���ֽ���
		NULL,
		&Rol);      //ָ�򴴽�hComʱ��Rol��ָ��

	if (bResult) {
		printf("success read out A\r\n");
		//��������봦���ȡ���ݴ���,���ݴ����myByte������          
	}

	dwRet = GetLastError();

	if (!bResult && (dwRet == ERROR_IO_PENDING)) {

		dwRes = WaitForSingleObject(Rol.hEvent, 5000);   //5�볬ʱ

		switch (dwRes)
		{
		case  WAIT_OBJECT_0:

			if (!GetOverlappedResult(hCom,
				&Rol,
				&dwRead,    //ʵ�ʶ������ֽ���
				TRUE))     //TRUE��ʾֱ��������ɺ����ŷ���
			{
				//����ʧ��,����ʹ��GetLastError()��ȡ������Ϣ
			}
			else
			{
				//�����ɹ����,���ݶ�ȡ����myByte��

				//������봦�����ݵĴ���

				printf("success read out dwRead = %d,%s\r\n", dwRead, myByte);
			}
			break;
		case   WAIT_TIMEOUT:
			//������ʧ��,ԭ���ǳ�ʱ
			printf("������ʧ��,ԭ���ǳ�ʱ\r\n");
			break;
		default:
			//�������Ĭ�ϴ������
			break;
		}

	}

	CloseHandle(Rol.hEvent);

	return 0;

}
//------------------------���ڷ����߳�
DWORD ThreadProcWrite(LPVOID   pParam)
{
	BYTE    myByte[10] = "AT\r\n";

	DWORD    dwRes;
	DWORD    dwWrite;


	BOOL bResult;

	Wol.Internal = 0;        //����OVERLAPPED�ṹWol
	Wol.InternalHigh = 0;
	Wol.Offset = 0;
	Wol.OffsetHigh = 0;
	Wol.hEvent = CreateEvent(NULL,          //����Wol��hEvent��ԱΪ���ź�״̬
		TRUE,
		FALSE,
		NULL);

	if (Wol.hEvent == NULL)
	{
		printf("hEvent ��");
		return -1;
	}

	bResult = WriteFile(hCom,         //���ھ��
		&myByte,     //��Ŵ���������
		4,         //�����͵��ֽ���
		NULL,
		&Wol);       //ָ�򴴽�hComʱ��Wol��ָ��

	if (bResult)
	{

		printf("send success \r\n");

	}

	if (!bResult) {

		dwRes = WaitForSingleObject(Wol.hEvent, 500);   //5ms��ʱ
		switch (dwRes)
		{
		case   WAIT_OBJECT_0:
			if (!GetOverlappedResult(hCom,
				&Wol,
				&dwWrite,
				TRUE))     //TRUE��ʾֱ��������ɺ����ŷ���
			{
				//����ʧ��,����ʹ��GetLastError()��ȡ������Ϣ
			}
			else
			{
				//�������ݳɹ�
				printf("send success dwWrite = %d \r\n", dwWrite);
				//������뷢�ͳɹ��Ĵ������
			}
			break;
		case   WAIT_TIMEOUT:
			//������ʧ��,ԭ���ǳ�ʱ
			break;
		default:
			//�������Ĭ�ϴ������
			break;
		}

	}

	CloseHandle(Wol.hEvent);

	return 0;
}

//���ڷ��ͺ���
DWORD Com_Send(HWND hDlg, unsigned char *p, int len)
{
	DWORD write_bytes = len;
	//COMSTAT comstate;
	//DWORD dwErrorFlag;
	BOOL writeFlag=0;
	OVERLAPPED m_osWrite;
	
	DWORD error = 0;
	m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);//�ź�״̬����
	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;

	writeFlag = WriteFile(hCom, p, write_bytes, &write_bytes, &m_osWrite);//����
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
	//----------------------���½��ռ���
	/*int sendcount = 0;
	sendcount = GetDlgItemInt(hDlg, IDS_SendCount, NULL, FALSE);
	sendcount += len;
	SetDlgItemInt(hDlg, IDS_SendCount, sendcount, TRUE);*/
	//----------------------���½�����ʾ
	dislpay(hDlg, p,len, 1);
	return write_bytes;//д���ֽ���
}
//���ڶ�ȡ����
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

	if (!comstate.cbInQue)//������������
		return 0;

	if (read_bytes > comstate.cbInQue)//������ʵ��������
		read_bytes = comstate.cbInQue;

	readstate = ReadFile(hCom, p, read_bytes, &read_bytes, &m_osRead);    //��ȡ��p

	if (!readstate)
	{
		if (GetLastError() == ERROR_IO_PENDING)  //��̨����
		{
			GetOverlappedResult(hCom, &m_osRead, &read_bytes, TRUE);
			return read_bytes;//��ȡ�ֽ���
		}

		/*printf("Read fail\n");*/
		MessageBox(hDlg, TEXT("Read fail\n"), MB_OK, TRUE);
		return 0;
	}
	//----------------------���½��ռ���
	//int sendcount = 0;
	//sendcount = GetDlgItemInt(hDlg, IDS_ReceiveCount, NULL, FALSE);
	//sendcount += read_bytes;
	//SetDlgItemInt(hDlg, IDS_ReceiveCount, sendcount, TRUE);
	//----------------------���½�����ʾ
	dislpay(hDlg, p, read_bytes, 2);
	return read_bytes;
}
//��ʾ
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
	swprintf_s(temp, 15, L"%0.8d:", line); //��x=1234�����buffer

	for (i = 0; i < len; i++)
	{
		unsigned char data = p[i]&0xFF;
		temp = &str[j];
		swprintf_s(temp, 20, L"%0.2X ", data); //��x=1234�����buffer
		j += 3;
		//wprintf(temp);
		//SetDlgItemText(hDlg, IDE_Send, (LPCWSTR)str);
	}	
	//-------------------------���в������ݣ�ÿ��Ҫ����س����з�
	temp = &str[j];
	swprintf_s(temp, 3, L"\r\n"); //��x=1234�����buffer
	//-------------------------��λ�����������ʼ��
	SendMessage(editWindow, EN_SETFOCUS, 0, 0);
	SendMessage(editWindow, EM_SETSEL, -2, -1);
	SendMessage(editWindow, EM_REPLACESEL, 0, (LPARAM)str);
	//----------------------���¼���
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
	// �̴߳���
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
		CheckListBoxdData(hWnd);
    }
	CloseHandle(HRead);
	CloseHandle(HWrite);
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
   /*HWND hWnd;*/
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
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);               // �����˵�ѡ��:
    switch (message)
    {
    case WM_COMMAND:	//������Ϣ
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
					case IDC_HexSend:		//hex����
						if (SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == TRUE)//�Ƿ����
							SendMessage((HWND)lParam, BM_SETCHECK, BST_UNCHECKED, 0);//ȡ����
						else 
							SendMessage((HWND)lParam, BM_SETCHECK, BST_CHECKED, 0);//��
						break;
					case IDC_HexRecv:	//hex����
						if (SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == TRUE)//�Ƿ����
							SendMessage((HWND)lParam, BM_SETCHECK, BST_UNCHECKED, 0);//ȡ����
						else
							SendMessage((HWND)lParam, BM_SETCHECK, BST_CHECKED, 0);//��
						break;
						break;
					}
					break;
			}
            // �����˵�ѡ��: 
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
                DestroyWindow(hWnd);	//����ָ���Ĵ��ڡ��������ͨ������WM_DESTROY ��Ϣ�� WM_NCDESTROY ��Ϣʹ������Ч���Ƴ�����̽��㡣
										//������������ٴ��ڵĲ˵�������̵߳���Ϣ���У������봰�ڹ�����صĶ�ʱ����������ڶԼ������ӵ��Ȩ����ϼ��������Ĳ鿴����
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
		EnableWindow(Login_Button, FALSE);		//��ʱ���ÿؼ�

		Login_Button = CreateWindow(TEXT("button"), TEXT("��RGB"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 5, 435, 80, 30, hWnd, (HMENU)IDB_RGBCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("�򿪺��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 435, 80, 30, hWnd, (HMENU)IDB_RedCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("�򿪻Ƶ�"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, 435, 80, 30, hWnd, (HMENU)IDB_GreCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("������"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 300, 435, 80, 30, hWnd, (HMENU)IDB_BluCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("������"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 400, 435, 80, 30, hWnd, (HMENU)IDB_LockCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("�򿪱���"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 435, 80, 30, hWnd, (HMENU)IDB_BKLigthCtl, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("button"), TEXT("�򿪹���"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 600, 435, 80, 30, hWnd, (HMENU)IDB_LayPower, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		//---------------------------------------ѡ���
		Login_Button = CreateWindow(TEXT("button"), TEXT("Hex����"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 700, 235, 80, 20, hWnd, (HMENU)IDC_HexRecv, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, BM_SETCHECK, BST_CHECKED, 0);//��
		EnableWindow(Login_Button, FALSE);		//��ʱ���ÿؼ�
		Login_Button = CreateWindow(TEXT("button"), TEXT("Hex����"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 700, 255, 80, 20, hWnd, (HMENU)IDC_HexSend, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, BM_SETCHECK, BST_CHECKED, 0);//��
		EnableWindow(Login_Button, FALSE);		//��ʱ���ÿؼ�
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
		EnableWindow(Login_Button, FALSE);		//��ʱ���ÿؼ�

		Login_Button = CreateWindow(TEXT("Combobox"), TEXT("ֹͣλ"), CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 700, 210, 80, 100, hWnd, (HMENU)IDB_BUTTON_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage(Login_Button, CB_ADDSTRING, 0, (LPARAM)TEXT("1λ"));
		SendMessage(Login_Button, CB_ADDSTRING, 1, (LPARAM)TEXT("1.5λ"));
		SendMessage(Login_Button, CB_ADDSTRING, 2, (LPARAM)TEXT("2λ"));
		SendMessage(Login_Button, CB_SETCURSEL, 0, 0);//����Ĭ��ֵ
		EnableWindow(Login_Button, FALSE);		//��ʱ���ÿؼ�

		

		//---------------------------------------�����б�Address1
		Login_Button = CreateWindow(TEXT("Listbox"), TEXT("Address1"), WS_CHILD |  WS_VSCROLL|WS_BORDER | WS_VISIBLE | LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, 100, 410, 80, 20, hWnd, (HMENU)IDL_Addr1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		wchar_t str[80];
		for (int i = 0; i <= 16; i++)
		{
			//sprintf(str, "%x", i); //��10.8תΪ�ַ���
			//itoa(i, str,10);
			swprintf_s(str,4,L"%d", i); //��x=1234�����buffer
			wprintf(str);
			SendMessage(Login_Button, LB_ADDSTRING, i, (LPARAM)(str));	
			//SendMessage(Login_Button, CB_SETCURSEL, 1, 0);//����Ĭ��ֵ
		}
		SendMessage(Login_Button, LB_ADDSTRING, 17, (LPARAM)(TEXT("All")));
		SendMessage(Login_Button, LB_SETCURSEL, (WPARAM)2, 0);
		//---------------------------------------�����б�Address2
		Login_Button = CreateWindow(TEXT("Listbox"), TEXT("Address2"), WS_CHILD | WS_VSCROLL | WS_BORDER | WS_VISIBLE | LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, 300, 410, 80, 20, hWnd, (HMENU)IDL_Addr2, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		for (int i = 0; i <= 16; i++)
		{
			//sprintf(str, "%x", i); //��10.8תΪ�ַ���
			//itoa(i, str,10);
			swprintf_s(str, 4, L"%d", i); //��x=1234�����buffer
			wprintf(str);
			SendMessage(Login_Button, LB_ADDSTRING, i, (LPARAM)(str));
		}
		SendMessage(Login_Button, LB_ADDSTRING, 17, (LPARAM)(TEXT("All")));
		SendMessage(Login_Button, LB_SETCURSEL, (WPARAM)1, 0);
		//---------------------------------------�����б�Address3
		Login_Button = CreateWindow(TEXT("Listbox"), TEXT("Address3"), WS_CHILD | WS_VSCROLL | WS_BORDER | WS_VISIBLE | LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, 500, 410, 80, 20, hWnd, (HMENU)IDL_Addr3, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		for (int i = 0; i <= 16; i++)
		{
			//sprintf(str, "%x", i); //��10.8תΪ�ַ���
			//itoa(i, str,10);
			swprintf_s(str, 4, L"%d", i); //��x=1234�����buffer
			wprintf(str);
			SendMessage(Login_Button, LB_ADDSTRING, i, (LPARAM)(str));
		}
		SendMessage(Login_Button, LB_ADDSTRING, 17, (LPARAM)(TEXT("All")));
		SendMessage(Login_Button, LB_SETCURSEL, (WPARAM)1, 0);

		//---------------------------------------��ʾ��
		Login_Button = CreateWindow(TEXT("edit"), TEXT("��������"), WS_CHILD | WS_VISIBLE | WS_BORDER /*�߿�*/ | ES_AUTOHSCROLL /*ˮƽ����*/| ES_AUTOVSCROLL /*��ֱ����*/ | ES_MULTILINE/*����*/ | WS_VSCROLL/*��ֱ������*/ | WS_HSCROLL/*��ֱ������*/| ES_READONLY/*ֻ��*/| ES_WANTRETURN/*֧�ֻس�����*/, 0, 0, 695, 300, hWnd, (HMENU)IDE_Receive, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		Login_Button = CreateWindow(TEXT("edit"), TEXT("��������"), WS_CHILD | WS_VISIBLE | WS_BORDER /*�߿�*/ | ES_AUTOHSCROLL /*ˮƽ����*/ | ES_AUTOVSCROLL /*��ֱ����*/ | ES_MULTILINE/*����*/ | WS_VSCROLL/*��ֱ������*/ | WS_HSCROLL/*��ֱ������*/ /*| ES_READONLYֻ��*/ | ES_WANTRETURN/*֧�ֻس�����*/, 0, 305, 695, 100, hWnd, (HMENU)IDE_Send, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		Login_Button = CreateWindow(TEXT("edit"), TEXT("���ռ���"), WS_CHILD | WS_VISIBLE | WS_BORDER /*�߿�*/ | ES_READONLY/*ֻ��*/ , 700, 300, 80, 20, hWnd, (HMENU)IDS_ReceiveCount, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
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
// ������CRC16�������Ϣ�������
INT_PTR CALLBACK AboutCRC16(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDE_CRC16,TEXT("\
ת�������8λ��ǰ����8λ�ں�			\
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
// ������Э�顱�����Ϣ�������
INT_PTR CALLBACK AboutProl(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDE_Prol, TEXT("\
֡��ʽ			\
\r\n0x7E	:��ʼ��	\
\r\nmsg	:��Ϣ��\
\r\nCRC16	:2�ֽ�У����\
\r\n0x7F	:������\
\r\n	\
\r\nmsg��ʽ����Ϣ��ʽ)		\
\r\nlen	:��Ϣ�峤��(�������ֽ�)	\
\r\ncmd	:�����\
\r\naddress1	:��ַ1\
\r\naddress2	:��ַ2\
\r\naddress3	:��ַ3\
\r\ndata0	:����0\
\r\n~	:����\
\r\ndatan	:����n\
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
				//LPCWSTR COMx;
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
					SetDlgItemText(hDlg, IDE_Receive, TEXT(""));	//�޸Ŀؼ�����
					SetDlgItemText(hDlg, IDE_Send, TEXT(""));	//�޸Ŀؼ�����
					portflag = 1;
				}				
			}
			else
			{				
				if (0 == CloseHandle(hCom))
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
// ��������ݡ������Ϣ�������
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
// ���������������Ϣ�������
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
			if (0 == memcmp(TEXT("�򿪹���"), text, 6))	//��ȡ�ؼ��ַ�
			{
				TxdBuffer[2] = 0x05;
				TxdBuffer[6] = 0x01;
				TxdBuffer[7] = 0x00;
				TxdBuffer[8] = 0x00;
				SetDlgItemText(hDlg, IDB_LayPower, TEXT("�رչ���"));
			
			}
			else
			{
				TxdBuffer[2] = 0x05;
				TxdBuffer[6] = 0x00;
				TxdBuffer[7] = 0x00;
				TxdBuffer[8] = 0x00;
				SetDlgItemText(hDlg, IDB_LayPower, TEXT("�򿪹���"));
			}
			goto sendData;
	
			case IDB_RedCtl:
				GetDlgItemText(hDlg, IDB_RedCtl, text, 6);
				if (0 == memcmp(TEXT("�򿪺��"), text, 6))	//��ȡ�ؼ��ַ�
				{					
					TxdBuffer[6] = 0x01;
					TxdBuffer[7] = 0x00;
					TxdBuffer[8] = 0x00;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("�ر���ʾ"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("�򿪻Ƶ�"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("������"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("��RGB"));
				}
				else
				{
					TxdBuffer[6] = 0x00;
					TxdBuffer[7] = 0x00;
					TxdBuffer[8] = 0x00;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("�򿪺��"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("�򿪻Ƶ�"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("������"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("��RGB"));
				}
								
				goto sendData;
			case IDB_GreCtl:
				GetDlgItemText(hDlg, IDB_GreCtl, text, 6);
				if (0 == memcmp(TEXT("�򿪻Ƶ�"), text, 6))	//��ȡ�ؼ��ַ�
				{
					TxdBuffer[6] = 0x00;
					TxdBuffer[7] = 0x01;
					TxdBuffer[8] = 0x00;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("�򿪺��"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("�ر���ʾ"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("������"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("��RGB"));
				}
				else
				{
					TxdBuffer[6] = 0x00;
					TxdBuffer[7] = 0x00;
					TxdBuffer[8] = 0x00;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("�򿪺��"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("�򿪻Ƶ�"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("������"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("��RGB"));
				}
				
				goto sendData;
			case IDB_BluCtl:
				GetDlgItemText(hDlg, IDB_BluCtl, text, 6);
				if (0 == memcmp(TEXT("������"), text, 6))	//��ȡ�ؼ��ַ�
				{					
					TxdBuffer[6] = 0x00;
					TxdBuffer[7] = 0x00;
					TxdBuffer[8] = 0x01;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("�򿪺��"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("�򿪻Ƶ�"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("�ر���ʾ"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("��RGB"));
				}
				else
				{
					TxdBuffer[6] = 0x00;
					TxdBuffer[7] = 0x00;
					TxdBuffer[8] = 0x00;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("�򿪺��"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("�򿪻Ƶ�"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("������"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("��RGB"));
				}
				goto sendData;
			case IDB_RGBCtl:
				GetDlgItemText(hDlg, IDB_RGBCtl, text, 6);
				if (0==memcmp(TEXT("��RGB"), text, 6))	//��ȡ�ؼ��ַ�
				{					
					TxdBuffer[6] = 0x01;
					TxdBuffer[7] = 0x01;
					TxdBuffer[8] = 0x01;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("�򿪺��"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("�򿪻Ƶ�"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("������"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("�ر���ʾ"));
				}
				else
				{
					TxdBuffer[6] = 0x00;
					TxdBuffer[7] = 0x00;
					TxdBuffer[8] = 0x00;
					SetDlgItemText(hDlg, IDB_RedCtl, TEXT("�򿪺��"));
					SetDlgItemText(hDlg, IDB_GreCtl, TEXT("�򿪻Ƶ�"));
					SetDlgItemText(hDlg, IDB_BluCtl, TEXT("������"));
					SetDlgItemText(hDlg, IDB_RGBCtl, TEXT("��RGB"));
				}
			
			sendData:

				char address1 = GetListBoxdData(hDlg, IDL_Addr1);
				char address2 = GetListBoxdData(hDlg, IDL_Addr2);
				char address3 = GetListBoxdData(hDlg, IDL_Addr3);

				TxdBuffer[3] = address1;
				TxdBuffer[4] = address2;
				TxdBuffer[5] = address3;
				TxdBuffer[11] = 0x7F;

				//-----------------------����CRC
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
//----------------------��ȡlistboxֵ
unsigned char GetListBoxdData(HWND hWnd, int nIDDlgItem)
{
	//SendMessage(HWND hWnd,)
	HWND hDlg;
	hDlg = GetDlgItem(hWnd, nIDDlgItem);
	wchar_t text[128] = { 0xFF };
	unsigned char data = 0;
	unsigned char Item = (unsigned char)SendMessage(hDlg, LB_GETCURSEL, 0, 0);//���ַ
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
//----------------------���listbox����
unsigned char CheckListBoxdData(HWND hWnd)
{
	if (0 == GetListBoxdData(hWnd, IDL_Addr1))	//��ַ��1Ϊ0
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
		if (0 == (GetListBoxdData(hWnd, IDL_Addr2) && GetListBoxdData(hWnd, IDL_Addr3)))	//ADDR2����ADDR3��һ��Ϊ0
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
//----------------------��ȡlistboxֵ
unsigned char SetEnableWindow(HWND hWnd, int nIDDlgItem)
{
	//SendMessage(HWND hWnd,)
	HWND hDlg;
	hDlg = GetDlgItem(hWnd, nIDDlgItem);
	if (NULL == hDlg)
		return 0;
	EnableWindow(hDlg, TRUE);		//��ʱ���ÿؼ�
	return	1;
}
//----------------------��ȡlistboxֵ
unsigned char SetDisableWindow(HWND hWnd, int nIDDlgItem)
{
	//SendMessage(HWND hWnd,)
	HWND hDlg;
	hDlg = GetDlgItem(hWnd, nIDDlgItem);
	if (NULL == hDlg)
		return 0;
	EnableWindow(hDlg, FALSE);		//��ʱ���ÿؼ�
	return	1;
}



/*******************************************************************************
*������			:	InvertUint8
*��������		:	��������˵��
*����				:
*����ֵ			:	��
*�޸�ʱ��		:	��
*�޸�˵��		:	��
*ע��				:	wegam@sina.com
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
*������			:	InvertUint16
*��������		:	��������˵��
*����				:
*����ֵ			:	��
*�޸�ʱ��		:	��
*�޸�˵��		:	��
*ע��				:	wegam@sina.com
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
*������			:	CRC16_MODBUS
*��������		:	����ʽx16+x15+x5+1��0x8005������ʼֵ0xFFFF����λ��ǰ����λ�ں󣬽����0x0000���
*����				:
*����ֵ			:	��
*�޸�ʱ��		:	��
*�޸�˵��		:	��
*ע��				:	wegam@sina.com
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
