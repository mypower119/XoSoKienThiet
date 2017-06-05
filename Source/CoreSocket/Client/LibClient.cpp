#include "LibClient.h"

void writeTitle()
{
	fstream input;
	input.open("XoSoKienThiet.txt", ios::in);

	if (input)
	{
		char data;
		while (!input.eof())
		{
			input.get(data);
			if (data == 'U' || data == 'V')
				textcolor(14);
			wcout << data;
			textcolor(7);
		}
		input.close();
	}
}

void setFontSize(int FontSize)
{
	CONSOLE_FONT_INFOEX info = { 0 };
	info.cbSize = sizeof(info);
	info.dwFontSize.Y = FontSize; // leave X as zero
	info.FontWeight = FW_NORMAL;
	wcscpy(info.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);
	_setmode(_fileno(stdout), _O_U16TEXT);
}

void resizeConsole(int width, int height)
{
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r);
	MoveWindow(console, r.left, r.top, width, height, TRUE);
}
// Hàm tô màu.
void textcolor(int x)
{
	HANDLE mau;
	mau = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(mau, x);
}

string receiveStr(CSocket &client)
{
	string result;
	char* temp;
	int size;
	if (client.Receive((char*)&size, sizeof(int)) == -1)
		return "exit";
	temp = new char[size + 1];
	if (client.Receive((char*)temp, size) == -1)
		return "exit";
	temp[size] = NULL;
	strcpy_string(result, temp);
	return result;
}

void sendStr(CSocket &client, string strSend_)
{
	int lenght = strSend_.length();
	client.Send((char*)&lenght, sizeof(int));
	client.Send(strSend_.c_str() , lenght);
}

void strcpy_string(string &str_, const char* s_)
{
	for (int i = 0; i < strlen(s_); i++)
	{
		str_.push_back(s_[i]);
	}
}

/*
**
**
**
*/
TaiKhoan input(){
	TaiKhoan temp;

	cout.flush();
	cout << "\n\tNhap ID: ";
	getline(cin, temp.ID);

	cout << "\tNhap mat khau: ";
	int pass;
	int count = 0;
	char p[20];
	do{
		fflush(stdin);
		pass = getch();
		if (pass != 13 && pass != 8)
			cout << "*";
		if (pass == 8 && count>0)
		{
			cout << "\b" << " " << "\b";
			count--;
		}
		else{
			if (pass == 8 && count == 0)
				count = 0;
			else if (pass != 13)
			{
				p[count] = pass;
				count++;
			}
		}
	} while (pass != 13);
	p[count] = '\0';
	cout << endl;
	strcpy_string(temp.passwd, p);
	return temp;
}

string createCapcha(int amount_)
{
	srand(time(0));
	string result;
	for (int i = 0; i < amount_; i++)
	{
		char p = (char)(rand() % (90 - 65 + 1) + 65);
		result.push_back(p);
	}
	return result;
}

string loginAccount(TaiKhoan &user_, CSocket& client_)
{
	int countSecurity = 1;
	do{
		cout << "Nhap thong tin dang nhap("<< countSecurity<< "): " << endl;
		do{
			user_ = input();
			if (user_.ID.length() == 0 || user_.passwd.length() == 0){
				textcolor(12);
				cout << "Ten tai khoan hoac mat khau khong duoc de trong !!! " << endl;
				textcolor(15);
			}
		} while (user_.ID.length() == 0 || user_.passwd.length() == 0);
		sendStr(client_, user_.ID);
		sendStr(client_, user_.passwd);
		if (countSecurity >= 3)
		{
			string capcha;
			string inputCapcha;
			do{
				capcha = createCapcha(5);
				cout << "Ban nhap sai qua so lan quy dinh !!!\nNhap 'exit' de thoat hoac nhap capcha '" + capcha + "': ";
				getline(cin, inputCapcha);
				if (inputCapcha == "exit")
				{
					sendStr(client_, "?");
					return "exit";
				}
			} while (inputCapcha != capcha);
		}
		countSecurity++;
	} while (receiveStr(client_) == "false");
	return "continue";
}

string createAccount(TaiKhoan &user_, CSocket& client_)
{
	string check;
	do{
		cout << "Nhap thong tin dang ky:" << endl;
		do{
			user_ = input();
			if (user_.ID.length() == 0 || user_.passwd.length() == 0){
				textcolor(12);
				cout << "Ten tai khoan hoac mat khau khong duoc de trong !!! " << endl;
				textcolor(15);
			}
		} while (user_.ID.length() == 0 || user_.passwd.length() == 0);
		sendStr(client_, user_.ID);
		sendStr(client_, user_.passwd);
		check = receiveStr(client_);
		if (check == "false")
		{
			cout << "Thong tin tai khoan da ton tai hoac khong phai dang Email !!!" << endl;
			cout << "Ban co muon tiep tuc dang ky lai (y/n) ?" << endl;
			char temp = getch();
			if (temp == 'n')
			{
				sendStr(client_, "?");
				return "exit";
			}
		}
	} while (check == "false");
	return "continue";
}

int showDayAndTinhList(CSocket &client_)
{
	string isSuccess;
	if ((isSuccess = receiveStr(client_)) == "exit")
		return -1;
	int n = 0;
	if (isSuccess == "true")
	{
		client_.Receive((char*)&n, sizeof(int));
		for (int i = 0; i < n; i++)
			cout << "\t" << i + 1 << ". " << receiveStr(client_) << endl;
	}
	else{
		cout << "Faile !!!" << endl;
	}
	return n;
}


int convertStringToInt(const string &str_)
{
	int n = 0;
	int length = str_.length();
	for (int i = 0; i < length; i++)
	{
		if (!(str_[i] >= '0' && str_[i] <= '9'))
			return INT_MIN;
		n = n * 10 + (int)str_[i] - 48;
	}
	return n;
}