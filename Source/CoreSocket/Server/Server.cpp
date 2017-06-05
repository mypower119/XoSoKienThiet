#include "libServer.h"

DWORD WINAPI startReceive(LPVOID lpParam);
void CreateConnect(CSocket &Server, CSocket* &Client, int nMaxClient, HANDLE* &Thr, DWORD* &thrid);

int main()
{
	SetConsoleTitle(TEXT("Xỗ Số Kiến Thiết - Server"));
	resizeConsole(700, 600);

	AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
	CSocket server;
	unsigned int port = 1234;
	CString nIP = "127.0.0.1";
	
	AfxSocketInit(NULL);
	server.Create(port,0,nIP);	

	string strMaxClient;
	do{
		cout << "\nNhap so luong client server co the tiep nhan: ";
		fflush(stdin);
		getline(cin, strMaxClient);
		if (convertStringToInt(strMaxClient) <= 0)
			cout << "Nhap so luong khong hop le, vui long nhap lai !!! " << endl;
	} while (convertStringToInt(strMaxClient) <= 0);
	
	int nMaxClient = convertStringToInt(strMaxClient);
	CSocket* client = new CSocket[nMaxClient];
	HANDLE* thr = new HANDLE[nMaxClient];
	DWORD* thrid = new DWORD[nMaxClient];
	
	server.Listen();
	cout << "Server dang lang nghe ket noi tu client !!!\n" << endl;
	CreateConnect(server, client, nMaxClient, thr, thrid);

	delete[] client;
	delete[] thr;
	delete[] thrid;
	return 0;
}

DWORD WINAPI startReceive(LPVOID lpParam)
{
	CSocket client;
	client.Attach(*(CSocket*)(lpParam));

	// các xử lý;
	int sttClient;
	client.Receive((char*)&sttClient, sizeof(int));

	cout << "Dang cho client dang nhap tai khoan XSKT ...\n" << endl;
	string nChoice;
	nChoice = receiveStr(client, sttClient);
	TaiKhoan user;
	if (nChoice == "1")
	{
		if (loginAccount(user, client, sttClient) == "exit")
			goto CloseClient;
	}
	else if (nChoice == "2")
	{
		if (createAccount(user, client, sttClient) == "exit")
			goto CloseClient;
	}
	else
		goto CloseClient;
	cout << "Client dang nhap thanh cong !!!" << endl;

	while (1)
	{
	ChoiceDay:
		Data listDays;
		init(listDays);
		if (queryDaysList(listDays))
			sendList(client, listDays);

		int index;
		string temp;
		if ((temp = receiveStr(client, sttClient)) == "exit")
			goto CloseClient;
		index = convertStringToInt(temp) - 1;

		Data listTinh;
		init(listTinh);
		sendStr(client, listDays.data[index]);
		if (queryTinhList(listDays, index, listTinh))
			sendList(client, listTinh);

		// Chức năng truy vấn từ client
		string query;
		bool check = true;

		int choise;
		while (1){
			do{
				if ((query = receiveStr(client, sttClient)) == "exit")
					goto CloseClient;

				if (query == "out")
				{
					goto ChoiceDay;
				}
				else if (checkQuery(query) == 0)
				{
					check = false;
					sendStr(client, "false");
				}
				else
				{
					check = true;
					sendStr(client, "true");
					choise = checkQuery(query);
					client.Send((char*)&choise, sizeof(int));
				}
			} while (!check);

			switch (choise)
			{
			case 1:
				queryHResult(client, listTinh);
				break;
			case 2:
			{
				queryTinhResult(client, query, listDays.data[index]);
				break;
			}
			case 3:
			{
				queryTinhVeResult(client, query, listDays.data[index]);
				break;
			}
			default:
				break;
			}
		}
	}
CloseClient:
	cout << "Mat ket noi voi Client(" << sttClient <<") !!!" << endl;
	client.Close();
	return 0;
}

void CreateConnect(CSocket &Server, CSocket* &Client, int nMaxClient, HANDLE* &Thr, DWORD* &thrid)
{
	int i = 1;
	while (i <= nMaxClient)
	{
		if (Server.Accept(Client[i - 1]))
		{
			textcolor(9);
			cout << "Client thu " << i  << " ket noi thanh cong" << endl;
			textcolor(7);
			Client[i - 1].Send((char*)&i, sizeof(int));
			Thr[i - 1] = CreateThread(NULL, 0, startReceive, &Client[i - 1], 0, &thrid[i - 1]);
			WaitForMultipleObjects(1, Thr, true, 10);
			i++;
		}
	}
	cin.get();
	Server.Close();
}