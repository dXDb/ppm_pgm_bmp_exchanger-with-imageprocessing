#include "head_er.h"

// PXM 파일정보 가공
pxm temp(ifstream *file) {
	int line_cnt = 0, width, height, Findex = -1;
	string header_p, pixel, maximum_p;
	while (line_cnt != 4) {
		switch (line_cnt) {
		case 0:
			getline(*file, header_p);
			line_cnt++;
			break;

		case 1:
			getline(*file, pixel);
			while (pixel == "") getline(*file, pixel);
			line_cnt++;

			// pixel의 값을 파싱해 공백을 기준으로 width와 height로 분리
			Findex = pixel.find(" ", 0);
			if (Findex != -1) {
				width = stoi(pixel.substr(0, Findex));
				height = stoi(pixel.substr(Findex + 1, pixel.length()));
				line_cnt++;
			}
			break;

			// 한 line에 없을 경우 width값으로 대입하고 다른 라인에서 찾기
		case 2:
			width = stoi(pixel);
			getline(*file, pixel);
			height = stoi(pixel);
			line_cnt++;
			break;

			// maximum값 찾기
		case 3:
			getline(*file, maximum_p);
			while (maximum_p == "") getline(*file, maximum_p);
			line_cnt++;
			break;

		default:
			break;
		}
	}

	pxm rep = {0};
	unsigned char *img;
	if (header_p == "P5") {
		img = new unsigned char[width * height];
		file->read((char*)img, width * height);
	}

	else {
		img = new unsigned char[width * height * 3];
		file->read((char*)img, width * height * 3);
	}

	rep.width = width;
	rep.height = height;
	rep.img = img;

	return rep;
}

// bmp pixel 받아오기
unsigned char *get_bmp_img(myBITMAPFILEHEADER f_Header, myBITMAPINFOHEADER b_Header, myRGBQUAD *c_table, ifstream *file) {
	int whole_width = (((b_Header.biBitCount * b_Header.biWidth) + 31) / 32 * 4);

	unsigned char *img;

	if (b_Header.biBitCount == 24) img = bmp_24(f_Header, b_Header, whole_width, file, 4);
	else img = bmp_o(f_Header, b_Header, whole_width, file, c_table, 5);
	return img;
}

// 영상보여주기 위한 temp 생성
void makeTemp(myBITMAPFILEHEADER f_Header, myBITMAPINFOHEADER b_Header, myRGBQUAD *c_table, unsigned char *img) {
	// 파일 저장
	ofstream output("temp.bmp", ios::binary);
	output.write((char*)&f_Header.bfType, 2);
	output.write((char*)&f_Header.bfSize, 4);
	output.write((char*)&f_Header.bfReserved1, 2);
	output.write((char*)&f_Header.bfReserved2, 2);
	output.write((char*)&f_Header.bfOffBits, 4);
	output.write((char*)&b_Header.biSize, 4);
	output.write((char*)&b_Header.biWidth, 4);
	output.write((char*)&b_Header.biHeight, 4);
	output.write((char*)&b_Header.biPlanes, 2);
	output.write((char*)&b_Header.biBitCount, 2);
	output.write((char*)&b_Header.biCompression, 4);
	output.write((char*)&b_Header.biSizeImage, 4);
	output.write((char*)&b_Header.biXPelsPerMeter, 4);
	output.write((char*)&b_Header.biYPelsPerMeter, 4);
	output.write((char*)&b_Header.biClrUsed, 4);
	output.write((char*)&b_Header.biClrImportant, 4);
	if (b_Header.biBitCount != 24) output.write((char*)c_table, f_Header.bfOffBits - 54);
	output.write((char*)img, b_Header.biSizeImage);
	output.close();
	return;
}

void exchange_bmp(ifstream *file, string filetype, int startX, int startY, int finishX, int finishY, int *intRGB) {
	myBITMAPFILEHEADER f_Header;
	file->read((char*)&f_Header.bfType, 2);
	file->read((char*)&f_Header.bfSize, 4);
	file->read((char*)&f_Header.bfReserved1, 2);
	file->read((char*)&f_Header.bfReserved2, 2);
	file->read((char*)&f_Header.bfOffBits, 4);

	myBITMAPINFOHEADER b_Header;
	file->read((char*)&b_Header.biSize, 4);
	file->read((char*)&b_Header.biWidth, 4);
	file->read((char*)&b_Header.biHeight, 4);
	file->read((char*)&b_Header.biPlanes, 2);
	file->read((char*)&b_Header.biBitCount, 2);
	file->read((char*)&b_Header.biCompression, 4);
	file->read((char*)&b_Header.biSizeImage, 4);
	file->read((char*)&b_Header.biXPelsPerMeter, 4);
	file->read((char*)&b_Header.biYPelsPerMeter, 4);
	file->read((char*)&b_Header.biClrUsed, 4);
	file->read((char*)&b_Header.biClrImportant, 4);

	myRGBQUAD c_table[256];
	if (b_Header.biBitCount != 24) file->read((char*)c_table, f_Header.bfOffBits - 54);

	unsigned char *img;
	unsigned char *pxm = new unsigned char[b_Header.biWidth * b_Header.biHeight * 3];
	img = get_bmp_img(f_Header, b_Header, c_table, file);

	int temp = 0;

	// 패딩처리
	int whole_width = (((b_Header.biBitCount * b_Header.biWidth) + 31) / 32 * 4);

	if (b_Header.biBitCount == 24) img = makeBox(img, b_Header.biWidth, b_Header.biHeight, whole_width, 2, startX, startY, finishX, finishY, intRGB);
	else img = makeBox(img, b_Header.biWidth, b_Header.biHeight, whole_width, 3, startX, startY, finishX, finishY, intRGB);

	// bmp -> ppm
	if (filetype == "ppm") {
		if (b_Header.biBitCount == 8) {
			// 파일 헤더 정보 설정
			ofstream fout("output.ppm", ios::binary);
			fout << "P6" << (char)10;
			fout << b_Header.biWidth << ' ' << b_Header.biHeight << (char)10 << 255 << (char)10;

			// img 상하반전
			for (int i = 0; i < b_Header.biHeight; i++) for (int j = 0; j < b_Header.biWidth; j++)  *(pxm + i * b_Header.biWidth + j) = *(img + (b_Header.biHeight - i - 1) * whole_width + j);
			for (int i = 0; i < b_Header.biHeight * b_Header.biWidth; i++) *(img + i) = *(pxm + i);

			// 픽셀 추가
			for (int i = 0, j = 0; i < b_Header.biWidth * b_Header.biHeight; i++, j += 3) {
				temp = *(img + i);
				*(pxm + j) = c_table[temp].rgbRed;
				*(pxm + j + 1) = c_table[temp].rgbGreen;
				*(pxm + j + 2) = c_table[temp].rgbBlue;
			}

			// 파일 저장
			fout.write((char*)pxm, b_Header.biWidth * b_Header.biHeight * 3);
		}

		else if (b_Header.biBitCount == 24) {
			// 파일 헤더 정보 설정
			ofstream fout("output.ppm", ios::binary);
			fout << "P6" << (char)10;
			fout << b_Header.biWidth << ' ' << b_Header.biHeight << (char)10 << 255 << (char)10;

			// img 상하반전 및 픽셀 추가
			for (int i = 0; i < b_Header.biHeight; i++)	for (int j = 0; j < b_Header.biWidth; j++) for (int h = 0; h < 3; h++) *(pxm + i * b_Header.biWidth * 3 + j * 3 + h) = *(img + (b_Header.biHeight - i - 1) * whole_width + j * 3 + 2 - h);

			// 파일 저장
			fout.write((char*)pxm, b_Header.biWidth * b_Header.biHeight * 3);
		}
	}

	// bmp -> pgm
	else if (filetype == "pgm") {
		if (b_Header.biBitCount == 8) {
			// 파일 헤더 정보 설정
			ofstream fout("output.pgm", ios::binary);
			fout << "P5" << (char)10;
			fout << b_Header.biWidth << ' ' << b_Header.biHeight << (char)10 << 255 << (char)10;

			// img 상하반전
			for (int i = 0; i < b_Header.biHeight; i++) for (int j = 0; j < b_Header.biWidth; j++)  *(pxm + i * b_Header.biWidth + j) = *(img + (b_Header.biHeight - i - 1) * whole_width + j);
			for (int i = 0; i < b_Header.biHeight * b_Header.biWidth; i++) *(img + i) = *(pxm + i);

			// 픽셀 추가
			for (int i = 0, j = 0; i < b_Header.biWidth * b_Header.biHeight; i++) {
				temp = 0;
				temp += c_table[*(img + i)].rgbRed;
				temp += c_table[*(img + i)].rgbGreen;
				temp += c_table[*(img + i)].rgbBlue;
				*(pxm + i) = temp / 3;
			}

			// 파일 저장
			fout.write((char*)pxm, b_Header.biWidth * b_Header.biHeight);
		}

		// 파일 헤더 정보 설정
		else if (b_Header.biBitCount == 24) {
			ofstream fout("output.pgm", ios::binary);
			fout << "P5" << (char)10;
			fout << b_Header.biWidth << ' ' << b_Header.biHeight << (char)10 << 255 << (char)10;

			// img 상하반전
			for (int i = 0; i < b_Header.biHeight; i++) for (int j = 0; j < b_Header.biWidth; j++) *(pxm + i * b_Header.biWidth + j) = (*(img + (b_Header.biHeight - i - 1) * whole_width + j * 3) + *(img + (b_Header.biHeight - i - 1) * whole_width + j * 3 + 1) + *(img + (b_Header.biHeight - i - 1) * whole_width + j * 3 + 2)) / 3;

			// 파일 저장
			fout.write((char*)pxm, b_Header.biWidth * b_Header.biHeight);
		}
	}
	return;
}

void exchange_ppm(ifstream *file, string filetype, int startX, int startY, int finishX, int finishY, int *intRGB) {
	pxm rep = { 0 };
	int px = 0, cnt = 0;
	unsigned char *ex_img;

	// BMP 변환용 헤더
	myBITMAPFILEHEADER f_Header = { 0 };
	myBITMAPINFOHEADER b_Header = { 0 };

	// 픽셀값
	rep = temp(file);
	ex_img = new unsigned char[rep.width * rep.height * 3];

	// 반전
	if (filetype != "pppm") for (int i = 0; i < rep.height; i++) for (int j = 0; j < rep.width; j++) for (int h = 0; h < 3; h++) *(rep.img + i * rep.width * 3 + j * 3 + h) = 255 - *(rep.img + i * rep.width * 3 + j * 3 + h);

	// ppm -> bmp
	if (filetype == "bmp" || filetype == "pppm") {
		// 헤더설정
		f_Header.bfType = 0x4D42;
		f_Header.bfSize = rep.width * rep.height * 3 + 56;
		f_Header.bfOffBits = 54;
		b_Header.biBitCount = 24;
		b_Header.biSize = 40;
		b_Header.biWidth = rep.width;
		b_Header.biHeight = rep.height;
		b_Header.biPlanes = 1;

		// 패딩처리
		int whole_width = (((b_Header.biBitCount * b_Header.biWidth) + 31) / 32 * 4);

		b_Header.biSizeImage = whole_width * rep.height;

		// 상하반전 색보정
		ex_img = new unsigned char[whole_width * rep.height];
		for (int i = 0; i < rep.height; i++) for (int j = 0; j < rep.width; j++) for (int h = 0; h < 3; h++) *(ex_img + i * whole_width + j * 3 + h) = *(rep.img + (rep.height - i - 1) * rep.width * 3 + j * 3 + 2 - h);

		// 파일 저장
		myRGBQUAD *c_table = { 0 };
		if (filetype == "pppm") makeTemp(f_Header, b_Header, c_table, ex_img);
		else file_save(f_Header, b_Header, c_table, ex_img, startX, startY, finishX, finishY, intRGB);
	}

	// ppm -> pgm
	else if (filetype == "pgm") {
		// makeBox
		rep.img = makeBox(rep.img, rep.width, rep.height, -1, 1, startX, startY, finishX, finishY, intRGB);

		ex_img = new unsigned char[rep.width * rep.height];
		// 색 보정
		for (int i = 0; i < rep.height; i++) {
			for (int j = 0; j < rep.width; j++) {
				for (int h = 0; h < 3; h++) px += *(rep.img + i * rep.width * 3 + j * 3 + h);
				*(rep.img + cnt) = px / 3;
				cnt++;
				px = 0;
			}
		}

		// 파일 저장
		ofstream fout("output.pgm", ios::binary);
		fout << "P5" << (char)10;
		fout << rep.width << ' ' << rep.height << (char)10 << 255 << (char)10;
		fout.write((char*)rep.img, rep.width * rep.height);
	}
	return;
}

void exchange_pgm(ifstream *file, string filetype, int startX, int startY, int finishX, int finishY, int *intRGB) {
	pxm rep = { 0 };
	int px = 0, cnt = 0;
	unsigned char *ex_img;

	// BMP 변환용 헤더
	myBITMAPFILEHEADER f_Header = { 0 };
	myBITMAPINFOHEADER b_Header = { 0 };

	// 픽셀값
	rep = temp(file);

	// 반전
	if (filetype != "ppgm") for (int i = 0; i < rep.height; i++) for (int j = 0; j < rep.width; j++) *(rep.img + i * rep.width + j) = 255 - *(rep.img + i * rep.width + j);

	// pgm -> bmp
	if (filetype == "bmp" || filetype == "ppgm") {
		// 헤더설정
		f_Header.bfType = 0x4D42;
		f_Header.bfSize = rep.width * rep.height * 3 + 56;
		f_Header.bfOffBits = 54;
		b_Header.biBitCount = 24;
		b_Header.biSize = 40;
		b_Header.biWidth = rep.width;
		b_Header.biHeight = rep.height;
		b_Header.biPlanes = 1;

		// 패딩처리
		int whole_width = (((b_Header.biBitCount * b_Header.biWidth) + 31) / 32 * 4);

		b_Header.biSizeImage = whole_width * rep.height + 2;

		// 상하반전 색보정
		ex_img = new unsigned char[whole_width * rep.height];
		for (int i = 0; i < rep.height; i++) for (int j = 0; j < rep.width; j++) for (int h = 0; h < 3; h++) *(ex_img + i * whole_width + j * 3 + h) = *(rep.img + (rep.height - i - 1) * rep.width + j);

		// 파일 저장
		myRGBQUAD *c_table = { 0 };
		if (filetype == "ppgm") makeTemp(f_Header, b_Header, c_table, ex_img);
		else file_save(f_Header, b_Header, c_table, ex_img, startX, startY, finishX, finishY, intRGB);
	}

	// pgm -> ppm
	else if (filetype == "ppm") {
		ex_img = new unsigned char[rep.width * rep.height * 3];

		//색보정
		for (int i = 0; i < rep.height; i++) {
			for (int j = 0; j < rep.width; j++) {
				for (int h = 0; h < 3; h++) px += *(ex_img + i * rep.width * 3 + j * 3 + h) = *(rep.img + cnt);
				cnt++;
			}
		}

		// makeBox
	    ex_img = makeBox(ex_img, rep.width, rep.height, -1, 1, startX, startY, finishX, finishY, intRGB);

		// 파일 저장
		ofstream fout("output.ppm", ios::binary);
		fout << "P6" << (char)10;
		fout << rep.width << ' ' << rep.height << (char)10 << 255 << (char)10;
		fout.write((char*)ex_img, rep.width * rep.height * 3);
	}
	return;
}

void exchange_file(string file_name, string filetype, int startX, int startY, int finishX, int finishY, int *intRGB) {
	// 파일 선언
	ifstream checkHeader(file_name, ios::binary);
	ifstream file(file_name, ios::binary);

	myWORD fileHeader;
	checkHeader.read((char*)&fileHeader, sizeof(myWORD));
	switch (fileHeader) {
	// bmp
	case 0x4D42:
		exchange_bmp(&file, filetype, startX, startY, finishX, finishY, intRGB);
		file.close();
		checkHeader.close();
		break;

	// ppm
	case 0x3650:
		exchange_ppm(&file, filetype, startX, startY, finishX, finishY, intRGB);
		file.close();
		checkHeader.close();
		break;

	// pgm
	case 0x3550:
		exchange_pgm(&file, filetype, startX, startY, finishX, finishY, intRGB);
		file.close();
		checkHeader.close();
		break;

	default:
		break;
	}
	return;
}