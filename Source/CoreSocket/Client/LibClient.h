#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <fcntl.h>
#include <io.h>
#include <string>
#include <afxsock.h>
#include <fstream>
#include <iostream>
#include <time.h>
#include <conio.h>
#include <Windows.h>
#include <Tchar.h>
using namespace std;

struct TaiKhoan
{
	string ID;
	string passwd;
};

string receiveStr(CSocket &client);
void sendStr(CSocket &client, string strSend_);
void strcpy_string(string &str_, const char* s_);
int convertStringToInt(const string &str_);

TaiKhoan input();
string createCapcha(int amount_);
string loginAccount(TaiKhoan &user_, CSocket& client_);
string createAccount(TaiKhoan &user_, CSocket& client_);
int showDayAndTinhList(CSocket &client_);

// Đồ họa ^^
void setFontSize(int FontSize);
void resizeConsole(int width, int height);
void textcolor(int x);
void writeTitle();