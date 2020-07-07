#pragma once

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

#define BUFSIZE 4096

typedef long				myLONG;
typedef unsigned long       myDWORD;
typedef unsigned char       myBYTE;
typedef unsigned short      myWORD;


struct myBITMAPFILEHEADER {
	myWORD    bfType;
	myDWORD   bfSize;
	myWORD    bfReserved1;
	myWORD    bfReserved2;
	myDWORD   bfOffBits;
};

struct myBITMAPINFOHEADER {
	myDWORD      biSize;
	myLONG       biWidth;
	myLONG       biHeight;
	myWORD       biPlanes;
	myWORD       biBitCount;
	myDWORD      biCompression;
	myDWORD      biSizeImage;
	myLONG       biXPelsPerMeter;
	myLONG       biYPelsPerMeter;
	myDWORD      biClrUsed;
	myDWORD      biClrImportant;
};

struct myRGBQUAD {
	myBYTE    rgbBlue;
	myBYTE    rgbGreen;
	myBYTE    rgbRed;
	myBYTE    rgbReserved;
};

// pxm 정보
struct pxm {
	int width;
	int height;
	unsigned char *img;
};

using namespace std;

void exchange_file(string file_name, string filetype, int startX, int startY, int finishX, int finishY, int *intRGB);
void bmp_rgb(string file_name, int type, int processType, int intX, int intY, int intValue, int startX, int startY, int finishX, int finishY, int *intRGB);
void choose(myBITMAPFILEHEADER f_Header, myBITMAPINFOHEADER b_Header, myRGBQUAD *c_table, unsigned char *img, int x, int y, int val, int menu, int startX, int startY, int finishX, int finishY, int *intRGB);
unsigned char *bmp_o(myBITMAPFILEHEADER f_Header, myBITMAPINFOHEADER b_Header, int whole_width, ifstream *file, myRGBQUAD *c_table, int type);
unsigned char *bmp_24(myBITMAPFILEHEADER f_Header, myBITMAPINFOHEADER b_Header, int whole_width, ifstream *file, int type);
unsigned char *decode_RLE(unsigned char *img, myBITMAPINFOHEADER b_Header);
unsigned char* makeBox(unsigned char *img, int width, int height, int whole_width, int type, int startX, int startY, int finishX, int finishY, int *intRGB);
void file_save(myBITMAPFILEHEADER f_Header, myBITMAPINFOHEADER b_Header, myRGBQUAD *c_table, unsigned char *img, int startX, int startY, int finishX, int finishY, int *intRGB);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprevInstance, LPSTR lpszArg, int nCmdShow);

// 1. 메세지 처리
// 2. 이미지 저장