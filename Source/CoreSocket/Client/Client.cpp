#include "LibClient.h"

int main()
{
	SetConsoleTitle(TEXT("Xỗ Số Kiến Thiết - Client"));
	resizeConsole(800, 700);
	
	AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
	AfxSocketInit(NULL);

	CSocket client;
	char sAdd[1000];
	unsigned int port = 1234;
	AfxSocketInit(NULL);
	client.Create();
	
	cout << "\nNhap dia chi cua Server (IP Server Xo So : 127.0.0.1): ";
	gets(sAdd);

	if (client.Connect(CA2W(sAdd), port))
	{
		int sttClient;
		client.Receive((char*)&sttClient, sizeof(int));
		client.Send((char*)&sttClient, sizeof(int));

		writeTitle();
		textcolor(12);
		cout << "\n____________________________";
		textcolor(15);
		cout << " Client da ket noi thanh cong ";
		textcolor(12);
		cout << "_______________________";
		textcolor(15);

		cout << "\n\t1. Dang Nhap" << endl;
		cout << "\t2. Dang ky tai khoan moi" << endl;
		string strChoice;
		do{
			cout << "Chon chuc nang: ";
			textcolor(15);
			getline(cin, strChoice);
			textcolor(15);
			if (strChoice != "1" && strChoice != "2" && strChoice != "exit")
			{
				textcolor(12);
				cout << "Ban chon chuc nang khong dung vui long nhap lai hoac truy van 'exit' de thoat'!!!" << endl;
				textcolor(15);
			}
		} while (strChoice != "1" && strChoice != "2" && strChoice != "exit");
		sendStr(client, strChoice);
		if (strChoice == "exit")
			goto Exit;

		TaiKhoan user;
		if (strChoice == "1")
		{
			if (loginAccount(user, client) == "exit")
				goto Exit;
		}
		else{
			if (createAccount(user, client) == "exit")
				goto Exit;
		}
		textcolor(14);
		cout << "Dang nhap thanh cong, Chao mung ban den voi XSKT !!!" << endl;
		textcolor(15);
		
		while (1)
		{
		ChoiceDay:
			cout << "\nDanh sach ngay co ket qua xo so: " << endl;
			textcolor(14);
			int nDays = showDayAndTinhList(client);
			textcolor(15);
			if (nDays == -1)
				goto Faile;

			string strIndexDays;
			int nIndexDays;
			do{
				cout << "Nhap STT ngay muon chon: ";
				cin >> strIndexDays;
				nIndexDays = convertStringToInt(strIndexDays);
				if ((nIndexDays < 1 || nIndexDays > nDays) && strIndexDays != "exit"){
					textcolor(12);
					cout << "Sai chuc nang !!!\nQuy khach vui long nhap lai hoac truy van 'exit' de thoat" << endl;
					textcolor(15);
				}
			} while ((nIndexDays < 1 || nIndexDays > nDays) && strIndexDays != "exit");

			if (strIndexDays == "exit")
			{
				sendStr(client, strIndexDays);
				goto Exit;
			}
			// thứ tự ngày
			sendStr(client, strIndexDays);

			// xuất DS tĩnh
			cout << "\nDanh sach tinh mo thuong ngay " << receiveStr(client) << " :" << endl;
			textcolor(14);
			showDayAndTinhList(client);
			textcolor(15);

			// Chức năng truy vấn: 
			cout << "Nhap truy van 'h' (help) de duoc huong dan cu phap !!!" << endl;
			string query;
			while (1){
				do{
					cout << "-> Nhap truy van: ";
					fflush(stdin);
					getline(cin, query);
					if (query == "exit")
					{
						sendStr(client, query);
						goto Exit;
					}
					else if (query == "out")
					{
						sendStr(client, query);
						goto ChoiceDay;
					}
					else if (query.length() != 0){
						sendStr(client, query);
						query = receiveStr(client);
					}
					if (query == "false" || query.length() == 0){
						textcolor(12);
						cout << "Truy van khong hop le !!! truy van 'h' (help) de duoc ho tro." << endl;
						textcolor(15);
					}
				} while (query == "false" || query.length() == 0);

				int choise;
				if (client.Receive((char*)&choise, sizeof(int)) == -1)
				{
					goto Faile;
				}

				switch (choise)
				{
				case 1:
					textcolor(14);
					cout << receiveStr(client);
					textcolor(9);
					showDayAndTinhList(client);
					textcolor(15);
					cout << "\nNhan Enter de tiep tuc truy van !!! " << endl;
					cin.get();
					break;
				case 2:
					textcolor(14);
					cout << receiveStr(client);
					textcolor(15);
					cout << "\nNhan Enter de tiep tuc truy van !!! " << endl;
					cin.get();
					break;
				case 3:
					textcolor(14);
					cout << receiveStr(client);
					textcolor(15);
					cout << "\nNhan Enter de tiep tuc truy van !!! " << endl;
					cin.get();
					break;
				default:
					goto Faile;
					break;
				}
			}
		}
	Faile:
		textcolor(12);
		cout << "\nMat ket noi voi Server !!!" << endl;
		goto Exit;
	}
	textcolor(12);
	cout << "Ket noi den Server that bai !!!" << endl;
Exit:
	client.Close();
	cout << "Bam phim bat ky de thoat hoan toan !!!" << endl;
	cin.get();
	return 0;
}