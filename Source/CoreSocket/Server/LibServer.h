#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <fcntl.h>
#include <io.h>
#include <string>
#include <afxsock.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include "sqlite3.h"
using namespace std;

#define MAX_LENGHT 1000
#define DATABASE "DBXoSoKienThiet.db"

struct TaiKhoan
{
	string ID;
	string passwd;
};

struct Data
{
	string data[MAX_LENGHT];
	int sl;
};

void init(Data &dt);

string receiveStr(CSocket &client, const int& sttClient_);
void sendStr(CSocket &client, string strSend_);
int convertStringToInt(const string &str_);
char* convertStringToChar(string str_);
void strcpy_string(string &str_, const char* s_);

bool isCharacter(const char Character);
bool isNumber(const char Character);
bool isValidEmailAddress(const char * EmailAddress);

bool checkExistID(TaiKhoan tk);
bool checkLogin(TaiKhoan tk);
string receiveStrAccount(CSocket &client, const int& sttClient_);
string createAccount(TaiKhoan &tk_, CSocket &client_, const int& sttClient_);
string loginAccount(TaiKhoan &tk_, CSocket &client_, const int& sttClient_);

bool queryDaysList(Data &dt_);
bool queryTinhList(Data dtSource_, int index_, Data &dtDst_);
void sendList(CSocket &client_, Data dt_);

void cutTinhVeSo(string source_, string &tinh_, string &soVe_);
bool checkQueryTinhVeSo(string source_);
int checkQuery(string strSource_);

int compareVeSo(string str_, string strVeSoDo_);
string trungGiai(Data dt_, string veSo_);
int* findUpperTinh(string str_);

void queryHResult(CSocket &client_, const Data &listTinh_);
void queryTinhResult(CSocket &client_, string tinh_, string day_);
void queryTinhVeResult(CSocket &client, string tinhVeSo_, string day_);
string space(int n);
int calculator(int n_);
string resultXoSo(Data veSo_);

sqlite3* openDatabase(char* nameDB);
static int callback(void *NotUsed, int argc, char **argv, char **azColName);
bool insertAccount(char* nameDB, char* insertQuery);
int selectQuery(char* nameDB, char* selectQuery, Data &dt, int nColumn);
char* createQuery(char* selectColumn_, char* tableFrom_, string* whereRows_, char** dkRows_, int muontRows_);

void resizeConsole(int width, int height);
void textcolor(int x);