#include "LibServer.h"

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

/*
** hàm nhận thông điệp kiểu trả về là thông điệp char*
** hàm nhận thông điệp từ client hoặc từ server
** Tham số truyền vào là 1 client
*/
string receiveStr(CSocket &client, const int& sttClient_)
{
	string result;
	char* temp;
	int size;
	if(client.Receive((char*)&size, sizeof(int)) == -1)
	{
		client.Close();
		return "exit";
	}
	temp = new char[size + 1];
	if (client.Receive((char*)temp, size) == -1)
	{
		client.Close();
		return "exit";
	}
	temp[size] = NULL;
	strcpy_string(result, temp);
	if (result == "exit")
		client.Close();
	textcolor(15);
	cout << "-> Client("<<sttClient_<<") truy van: " << result << endl;
	textcolor(7);
	return result;
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


void sendStr(CSocket &client, string strSend_)
{
	int lenght = strSend_.length();
	client.Send((char*)&lenght, sizeof(int));
	client.Send(strSend_.c_str(), lenght);
}

/*
** Chuyển string sang char
** Tham số truyền vào string
** Kiểu trả về char*
*/
char* convertStringToChar(string str_)
{
	char* temp;
	int lenght = str_.length();
	temp = new char[lenght + 1];
	for (int i = 0; i < lenght; i++)
		temp[i] = str_[i];
	temp[lenght] = NULL;
	return temp;
}

/*
** Copy DL kiểu char* sang string <=> hàm strcpy
*/
void strcpy_string(string &str_, const char* s_)
{
	if (s_ != NULL){
		int length = strlen(s_);
		for (int i = 0; i < length; i++)
		{
			str_.push_back(s_[i]);
		}
	}
	
}

bool isCharacter(const char Character)
{
	return ((Character >= 'a' && Character <= 'z') || (Character >= 'A' && Character <= 'Z'));
}
bool isNumber(const char Character)
{
	return (Character >= '0' && Character <= '9');
}

/*
** Kiểm tra định dạng ID theo chuẩn email
*/
bool isValidEmailAddress(const char * EmailAddress)
{
	if (!EmailAddress)
		return 0;
	if (!isCharacter(EmailAddress[0]))
		return 0;
	int AtOffset = -1;
	int DotOffset = -1;
	unsigned int Length = strlen(EmailAddress);
	for (unsigned int i = 0; i < Length; i++)
	{
		if (EmailAddress[i] == '@')
			AtOffset = (int)i;
		else if (EmailAddress[i] == '.')
			DotOffset = (int)i;
	}
	if (AtOffset == -1 || DotOffset == -1)
		return 0;
	if (AtOffset > DotOffset)
		return 0;
	return !(DotOffset >= ((int)Length - 1));
}


/*
** Truy vấn từ Database thông tin account có tồn tại chưa
**
*/

bool checkExistID(TaiKhoan tk)
{
	Data dt;
	char* query;

	string whereRow[1];
	whereRow[0] = "[ID]";

	char** dkRow = new char*[1];
	dkRow[0] = new char[100];
	strcpy(dkRow[0], tk.ID.c_str());

	query = createQuery("[ID]", "[ACCOUNT]", whereRow, dkRow, 1);

	int check = selectQuery(DATABASE, query, dt, 1);
	if (check == 1){
		for (int i = 0; i < dt.sl; i++)
		{
			if (dt.data[i] == tk.ID)
				return true;
		}
	}
	return false;
}


string receiveStrAccount(CSocket &client, const int& sttClient_)
{
	string result;
	char* temp;
	int size;
	if (client.Receive((char*)&size, sizeof(int)) == -1)
	{
		client.Close();
		return "?";
	}
	temp = new char[size + 1];
	if (client.Receive((char*)temp, size) == -1)
	{
		client.Close();
		return "?";
	}
	temp[size] = NULL;
	strcpy_string(result, temp);
	if (result == "?")
	{
		cout << "-> Client(" << sttClient_<<") truy van : exit"<< endl;
		client.Close();
	}
	else
		cout << "-> Client(" << sttClient_<<") truy van : " << result << endl;
	return result;
}

string createAccount(TaiKhoan &tk_, CSocket &client_,const int& sttClient_)
{
	bool check = true;
	do{
		tk_.ID = receiveStrAccount(client_, sttClient_);
		tk_.passwd = receiveStrAccount(client_, sttClient_);
		if (tk_.ID == "?" || tk_.passwd == "?")
			return "exit";
		if (!checkExistID(tk_) && isValidEmailAddress(tk_.ID.c_str()))
		{
			check = true;
			sendStr(client_, "true");
		}
		else{
			check = false;
			sendStr(client_, "false");
		}
	} while (check == false);


	char query[100];
	strcpy(query, "insert into [ACCOUNT] values ('");
	strcat(query, convertStringToChar(tk_.ID));
	strcat(query, "', '");
	strcat(query, convertStringToChar(tk_.passwd));
	strcat(query, "');");
	insertAccount(DATABASE, query);
	return "continue";
}

string loginAccount(TaiKhoan &tk_, CSocket &client_, const int& sttClient_)
{
	bool check = true;
	do
	{
		tk_.ID = receiveStrAccount(client_, sttClient_);
		tk_.passwd = receiveStrAccount(client_, sttClient_);
		if (tk_.ID == "?" || tk_.passwd == "?")
			return "exit";

		if (!checkLogin(tk_))
		{
			check = false;
			sendStr(client_, "false");
		}
		else{
			check = true;
			sendStr(client_, "true");
		}
			
	} while (!check);
	return "continue";
}

bool checkLogin(TaiKhoan tk)
{
	Data dt;
	char query[100];

	string whereRow[2];
	whereRow[0] = "[ID]";
	whereRow[1] = "[PASSWD]";

	char** dkRow = new char*[2];
	dkRow[0] = new char[100];
	dkRow[1] = new char[100];
	strcpy(dkRow[0], tk.ID.c_str());
	strcpy(dkRow[1], tk.passwd.c_str());

	strcpy(query, createQuery(" [ID], [PASSWD]", "[ACCOUNT]", whereRow, dkRow, 2));
	int check = selectQuery(DATABASE, query, dt, 2);

	if (check == 1){
		if (dt.data[0] == tk.ID && dt.data[1] == tk.passwd)
			return true;
	}
	return false;
}

/*
** Hàm truy vấn DS ngày xổ số
** Gán giá trị cột ngày vào mảng string data;
** Gửi từng thông điệp từng ngày với chỉ số index
** Truy vấn thành công true -> thất bại false
*/
bool queryDaysList(Data &dt_)
{
	char query[] = "select * from [NGAYXOSO]";
	int isSuccess = selectQuery(DATABASE, query, dt_, 1);
	if (isSuccess == 1)
		return true;
	return false;
}

/*
** Truy vấn Ds tĩnh 
** tham số gôm data ngày, index
*/
bool queryTinhList(Data dtSource_, int index_, Data &dtDst_)
{
	string* Row = new string;
	Row[0] = "[NGAY]";
	char** dkRow = new char*[1];
	dkRow[0] = new char[100];
	strcpy(dkRow[0], convertStringToChar(dtSource_.data[index_]));
	char query[100];
	strcpy(query, createQuery("[TINH]", "[TINHMOTHUONG]", Row, dkRow, 1));
	if (selectQuery(DATABASE, query, dtDst_, 1) == 1)
		return true;
	return false;
}

void sendList(CSocket &client_, Data dt_)
{
	if (dt_.sl != 0){
		sendStr(client_, "true");

		client_.Send((char*)&dt_.sl, sizeof(int));
		for (int i = 0; i < dt_.sl; i++)
		{
			sendStr(client_, dt_.data[i]);
		}
	}
	else{
		sendStr(client_, "false");
	}
}


void cutTinhVeSo(string source_, string &tinh_, string &soVe_)
{
	char* temp = strtok(convertStringToChar(source_), " ");
	strcpy_string(tinh_, temp);
	temp = strtok(NULL, "\0");
	strcpy_string(soVe_, temp);

}


/*
** Hàm kiểm tra truy vấn từ client
** trả về 0: không đúng
** 1: là truy vấn h
** 2: <tinh>
** 3: <tinh>_<số vé>
*/
bool checkQueryTinhVeSo(string source_)
{
	int n = source_.length();
	if (source_[0] == ' ' || source_[n - 1] == ' ')
		return 0;

	int count = 0;
	for (int i = 1; i < n - 1; i++)
	{
		if (source_[i] == ' ')
			count++;
	}
	if (count == 1)
	{
		string tinh, veSo;
		cutTinhVeSo(source_, tinh, veSo);
		if (findUpperTinh(tinh)[0] == 0)
			return 0;
		for (int i = 0; i < tinh.length(); i++)
			if (!isCharacter(tinh[i]))
				return 0;
		for (int i = 0; i < veSo.length(); i++)
			if (!isNumber(veSo[i]))
				return 0;
		if (veSo.length() != 6)
			return 0;
		return 1;
	}
	return 0;
}

int checkQuery(string strSource_)
{
	if (strSource_[0] == '\0')
		return 0;
	if (strSource_ == "H" || strSource_ == "h")
		return 1;
	else if (strSource_.find(" ") == -1 && findUpperTinh(strSource_)[0] != 0) // không tìm thấy
		return 2;
	else if (checkQueryTinhVeSo(strSource_))
	{
		return 3;
	}
	return 0;

}

int compareVeSo(string str_, string strVeSoDo_)
{
	int n = str_.length();
	int m = strVeSoDo_.length();
	int i, j;
	for (i = n - 1, j = m - 1; i >= 0 && j >= 0; j--, i--)
	{
		if (str_[i] != strVeSoDo_[j])
		{
			break;
		}
	}
	if (i == -1)
		return 0;
	return -1;
}

string trungGiai(Data dt_, string veSo_)
{
	for (int i = 2; i < dt_.sl; i++)
	{
		if (compareVeSo(dt_.data[i], veSo_) == 0)
		{
			switch (i)
			{
			case 2:
				return "0";
			case 3:
				return "1";
			case 4:
				return "2";
			case 5: case 6:
				return "3";
			case 7:case 8: case 9 : case 10 : case 11 : case 12: case 13:
				return "4";
			case 14:
				return "5";
			case 15: case 16: case 17:
				return "6";
			case 18:
				return "7";
			case 19:
				return "8";
			}
		}
	}
	return "9";// chuc ban may man lan sau
}

int* findUpperTinh(string str_)
{
	int* result = new int[10];

	for (int i = 0; i < 10; i++)
		result[i] = 0;

	for (int i = 1, j = 0; i < str_.length(); i++)
	{
		if (str_[i] >= 'A' && str_[i] <= 'Z')
		{
			result[j] = i;
			j++;
		}
	}
	return result;
}

/*
** Trả kết quả về từ Các truy vấn từ client
** H : heip -> hưởng dẫn truy vấn 2 cái còn lại
** <tinh> : truy vấn kết quả xổ số 1 tĩnh
** <tinh> <số vé> : dò xổ số
*/

void queryHResult(CSocket &client_, const Data &listTinh_)
{
	sendStr(client_, "\nCu phap truy van:\n\tTruy van tinh: <Tinh>\n\tEx: LongAn\n\tTruy van do so: <TinhThanh><space><SoVe(6 so)>\n\tEx:DongNai 123456\n\tThoat chuong trinh truy van 'exit'\n\tTruy van 'out' de chon lai ngay xo so\nDanh sach tinh mo thuong:\n");
	sendList(client_, listTinh_);
}

string space(int n)
{
	string result;
	for (int i = 0; i < n; i++)
		result.push_back(' ');
	return result;
}

int calculator(int n_)
{
	return 30 - (n_ / 2);
}

string resultXoSo(Data veSo_)
{
	string result;
	string temp;

	result += "Giai Dac Biet:\t";
	result += space(calculator(veSo_.data[2].length())) + veSo_.data[2] + "\n";

	result += "Giai Nhat:\t";
	result += space(calculator(veSo_.data[3].length())) + veSo_.data[3] + "\n";

	result += "Giai Nhi:\t";
	result += space(calculator(veSo_.data[4].length())) + veSo_.data[4] + "\n";

	result += "Giai Ba:\t";
	temp = veSo_.data[5] + " - " + veSo_.data[6];
	result += space(calculator(temp.length())) + temp + "\n";

	temp.clear();
	result += "Giai tu:\t";
	for (int i = 7; i <= 13; i++)
	{
		if (i != 13)
			temp = temp + veSo_.data[i] + " - ";
		else
			temp = temp + veSo_.data[i];
	}
	result += space(calculator(temp.length())) + temp + "\n";

	result += "Giai nam:\t";
	result += space(calculator(veSo_.data[14].length())) + veSo_.data[14] + "\n";

	result += "Giai sau:\t";
	temp.clear();
	for (int i = 15; i <= 17; i++)
	{
		if (i != 17)
			temp = temp + veSo_.data[i] + " - ";
		else
			temp = temp + veSo_.data[i];
	}
	result += space(calculator(temp.length())) + temp + "\n";

	result += "Giai bay:\t";
	result += space(calculator(veSo_.data[18].length())) + veSo_.data[18] + "\n";

	result += "Giai tam:\t";
	result += space(calculator(veSo_.data[19].length())) + veSo_.data[19] + "\n";
	return result;
}

void queryTinhResult(CSocket &client_, string tinh_, string day_)
{
	Data veSo;
	init(veSo);
	int* a = findUpperTinh(tinh_);
	for (int i = 0, j = 0; i < 10; i++)
	{
		if (a[i] == 0)
			break;
		else{
			tinh_.insert(a[i] + j, " ");
			j++;
		}
	}
	string temp = "select* from[VESO] where[TINH] = '" + tinh_ + "' and [NGAY] = '" + day_ + "'";

	if (selectQuery(DATABASE, convertStringToChar(temp), veSo, 20) == 1){
		string msg = "\t\tKet qua xo so tinh " + tinh_ + " ngay " + day_ + " : \n\n";
		msg = msg + resultXoSo(veSo);			
		sendStr(client_, msg);
	}
	else{
		sendStr(client_, "Tinh quy khach vua chon hien nay chua mo thuong !!!\n");
	}
}

void queryTinhVeResult(CSocket &client, string tinhVeSo_, string day_)
{
	Data veSoTrung;
	string tinh, veSo;
	cutTinhVeSo(tinhVeSo_, tinh, veSo);

	int* a = findUpperTinh(tinh);
	for (int i = 0, j = 0; i < 10; i++)
	{
		if (a[i] == 0)
			break;
		else{
			tinh.insert(a[i] + j, " ");
			j++;
		}
	}
	
	string temp = "select* from [VESO] where [TINH] = '" + tinh + "' and [NGAY] = '" + day_ + "'";

	if (selectQuery(DATABASE, convertStringToChar(temp), veSoTrung, 20) == 1)
	{
		string giai = trungGiai(veSoTrung, veSo);
		if (giai == "9")
			sendStr(client, "Chuc ban may man lan sau !!!\n");
		else{
			init(veSoTrung);
			string temp2 = "select* from [THUONG] where [ID] =" + giai;
			selectQuery(DATABASE, convertStringToChar(temp2), veSoTrung, 3);
			string msg = "Chuc mung ban, ban da trung: " + veSoTrung.data[1] + "\nTien thuong la: " + veSoTrung.data[2] + "\n";
			sendStr(client, msg);
		}
	}
	else
		sendStr(client, "Tinh quy khach vua chon chua mo thuong !!!\n");
}


void init(Data &dt)
{
	for (int i = 0; i < MAX_LENGHT; i++)
		dt.data[i] = "";
	dt.sl = 0;
}

/*
** Truy vấn
**
*/

sqlite3* openDatabase(char* nameDB)
{
	sqlite3* db;
	int o = sqlite3_open(nameDB, &db);
	if (o != SQLITE_OK) {
		fprintf(stderr, "Error occurred on opening database.\n");
		return NULL;
	}
	return db;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	return 0;
}

bool insertAccount(char* nameDB, char* insertQuery)
{
	bool flag = false;
	sqlite3 *db;
	char* zErrMsg = 0;
	/* Open database */
	db = openDatabase(nameDB);
	if (db)
	{
		int rc = sqlite3_exec(db, insertQuery, callback, 0, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		else{
			fprintf(stdout, "Records created successfully\n");
			flag = true;
		}
		sqlite3_close(db);
		return flag;
	}
	return flag;
}

/*
** Truy vấn với kết quả trả về là số nguyên tham số truyền vào data bị thay đổi nếu:
** có kết quả truy vấn = 1
** Lỗi = 0
** Truy vấn ko có DL = -1
*/
int selectQuery(char* nameDB, char* selectQuery, Data &dt, int nColumn)
{
	sqlite3_stmt * stmt;
	int row = 0;
	sqlite3* db = openDatabase(nameDB);
	if (db)
	{
		sqlite3_stmt *selectStmt;
		sqlite3_prepare(db, selectQuery, strlen(selectQuery) + 1, &selectStmt, NULL);

		int i = 0;
		while (1)
		{
			int s;
			s = sqlite3_step(selectStmt);
			if (s == SQLITE_ROW)
			{
				const unsigned char * text;

				for (int j = 0; j < nColumn; j++)
				{
					text = sqlite3_column_text(selectStmt, j);
					strcpy_string(dt.data[i], (char*)text);
					i++;
				}
				row++;
			}
			else if (s == SQLITE_DONE)
			{
				break;
			}
			else
			{
				fprintf(stderr, "Failed.\n");
				return 0;
			}
		}
		dt.sl = i;
		sqlite3_finalize(selectStmt);
		if (row == 0)
			return -1;
		else if (row != 0)
			return 1;
	}
	return 0;
}

char* createQuery(char* selectColumn_, char* tableFrom_, string* whereRows_, char** dkRows_, int muontDkRows_)
{
	char* dst = new char[100];
	strcpy(dst, "select");
	strcat(dst, selectColumn_);
	strcat(dst, " from ");
	strcat(dst, tableFrom_);
	strcat(dst, " where ");
	for (int i = 0; i < muontDkRows_; i++)
	{
		strcat(dst, whereRows_[i].c_str());
		strcat(dst, " = '");
		strcat(dst, dkRows_[i]);
		if (i != muontDkRows_ - 1)
			strcat(dst, "' and ");
		else
			strcat(dst, "'");
	}
	return dst;
}
