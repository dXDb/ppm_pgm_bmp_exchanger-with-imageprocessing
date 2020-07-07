#include "head_er.h"

unsigned char *decode_RLE(unsigned char *img, myBITMAPINFOHEADER b_Header) {
	// decoded image
	unsigned char *d_image = new unsigned char[b_Header.biWidth * b_Header.biHeight];
	int real_px = 0, now_line = 1, left_line = 0, left_width = b_Header.biWidth, after_x = 0, after_y = 0, table_cnt, table_char;

	for (int i = 0; i < b_Header.biSizeImage; i += 2) {
		switch ((int) *(img + i)) {
			// End of LIne, End of Bitmap, Delta, absolute mode
		case 0:
			switch ((int) *(img + (i + 1)))
			{
			case 0:
				now_line++;
				for (int j = 0; j < left_width; j++) *(d_image + real_px + j) = (unsigned char)0;
				real_px += left_width;
				left_width = b_Header.biWidth;
				break;

			case 1:
				left_line = b_Header.biHeight - now_line - 1;
				for (int j = 0; j < left_line * b_Header.biWidth + left_width; j++) *(d_image + real_px + j) = (unsigned char)0;
				i = b_Header.biSizeImage;
				break;

			case 2:
				i += 2;
				after_x = (int) *(img + i);
				after_y = (int) *(img + i + 1);

				if (after_x > left_width) {
					after_x -= left_width;
					left_width = b_Header.biWidth - after_x;
					after_y++;
				}
				else left_width -= after_x;

				for (int j = 0; j < after_y * b_Header.biWidth + after_x; j++) *(d_image + real_px + j) = (unsigned char)0;

				now_line += after_y;
				real_px += after_x + b_Header.biWidth * after_y;
				break;

			default:
				table_cnt = (int) *(img + i + 1);
				i += 2;

				for (int j = 0; j < table_cnt; j++) *(d_image + j + real_px) = *(img + i++);

				if (table_cnt % 2 == 1) i++;

				// 줄확인
				if (table_cnt > b_Header.biWidth) {
					for (int j = 0; j < int(table_cnt / b_Header.biWidth); j++) now_line++;
					table_cnt %= b_Header.biWidth;
				}

				// leftwidth 확인
				if (table_cnt > left_width) {
					left_width = b_Header.biWidth - (table_cnt - left_width);
					now_line++;
				}
				else left_width -= table_cnt;
				real_px += table_cnt;
				i -= 2;
				break;
			}
			break;

		// Encoded mode
		default:
			table_cnt = (int) *(img + i);
			table_char = (int) *(img + i + 1);

			if (table_cnt >= b_Header.biWidth) {
				for (int j = 0; j < int(table_cnt / b_Header.biWidth); j++) now_line++;
				table_cnt %= b_Header.biWidth;
			}

			for (int j = 0; j < table_cnt; j++) *(d_image + j + real_px) = (unsigned char)table_char;

			if (table_cnt > left_width) {
				left_width = b_Header.biWidth - (table_cnt - left_width);
				now_line++;
			}
			else left_width -= table_cnt;

			real_px += table_cnt;
			break;
		}
	}

	return d_image;
}

// 24비트 아닌 것
unsigned char *bmp_o(myBITMAPFILEHEADER f_Header, myBITMAPINFOHEADER b_Header, int whole_width, ifstream *file, myRGBQUAD *c_table, int type) {
	// 이미지 픽셀
	unsigned char *img = new unsigned char[b_Header.biSizeImage];
	file->read((char*)img, b_Header.biSizeImage);

	// RLE
	if (b_Header.biCompression != 0) img = decode_RLE(img, b_Header);

	// 반전
	for (int i = 0; i < 256; i++) {
		c_table[i].rgbBlue = 255 - c_table[i].rgbBlue;
		c_table[i].rgbGreen = 255 - c_table[i].rgbGreen;
		c_table[i].rgbRed = 255 - c_table[i].rgbRed;
	}

	return img;
}

// 24비트
unsigned char *bmp_24(myBITMAPFILEHEADER f_Header, myBITMAPINFOHEADER b_Header, int whole_width, ifstream *file, int type) {
	// 이미지 픽셀
	unsigned char *img = new unsigned char[b_Header.biSizeImage];
	file->read((char*)img, b_Header.biSizeImage);

	// 반전
	for (int i = 0; i < b_Header.biHeight; i++)	for (int j = 0; j < (b_Header.biWidth) * 3; j++) img[i * whole_width + j] = 255 - img[i * whole_width + j];

	return img;
}

// 파일 저장
void file_save(myBITMAPFILEHEADER f_Header, myBITMAPINFOHEADER b_Header, myRGBQUAD *c_table, unsigned char *img, int startX, int startY, int finishX, int finishY, int *intRGB) {
	// 사각형
	int whole_width = (((b_Header.biBitCount * b_Header.biWidth) + 31) / 32 * 4);
	if (b_Header.biBitCount == 24) img = makeBox(img, b_Header.biWidth, b_Header.biHeight, whole_width, 2, startX, startY, finishX, finishY, intRGB);
	else img = makeBox(img, b_Header.biWidth, b_Header.biHeight, whole_width, 3, startX, startY, finishX, finishY, intRGB);

	// 파일 저장
	ofstream output("output.bmp", ios::binary);
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

void bmp_rgb(string file_name, int type, int processType, int intX, int intY, int intValue, int startX, int startY, int finishX, int finishY, int *intRGB) {
	// 파일 선언
	ifstream file(file_name, ios::binary);

	// 파일 헤더의 멤버
	myBITMAPFILEHEADER f_Header;
	file.read((char*)&f_Header.bfType, 2);
	file.read((char*)&f_Header.bfSize, 4);
	file.read((char*)&f_Header.bfReserved1, 2);
	file.read((char*)&f_Header.bfReserved2, 2);
	file.read((char*)&f_Header.bfOffBits, 4);

	// 비트맵 정보 헤더의 멤버
	myBITMAPINFOHEADER b_Header;
	file.read((char*)&b_Header.biSize, 4);
	file.read((char*)&b_Header.biWidth, 4);
	file.read((char*)&b_Header.biHeight, 4);
	file.read((char*)&b_Header.biPlanes, 2);
	file.read((char*)&b_Header.biBitCount, 2);
	file.read((char*)&b_Header.biCompression, 4);
	file.read((char*)&b_Header.biSizeImage, 4);
	file.read((char*)&b_Header.biXPelsPerMeter, 4);
	file.read((char*)&b_Header.biYPelsPerMeter, 4);
	file.read((char*)&b_Header.biClrUsed, 4);
	file.read((char*)&b_Header.biClrImportant, 4);

	// 칼라 테이블
	myRGBQUAD *c_table = new myRGBQUAD[256];
	if (b_Header.biBitCount != 24) file.read((char*) c_table, f_Header.bfOffBits - 54);

	// 4의 배수 가로길이 만들기
	int whole_width = (((b_Header.biBitCount * b_Header.biWidth) + 31) / 32 * 4);

	/*   --------------------------------------------- */

	unsigned char *img;

	// bmp 사각형 및 반전
	if (b_Header.biBitCount == 24) img = bmp_24(f_Header, b_Header, whole_width, &file, 2);
	else img = bmp_o(f_Header, b_Header, whole_width, &file, c_table, 3);

	// RLE 헤더 설정
	if (b_Header.biCompression != 0) {
		b_Header.biCompression = 0;
		b_Header.biSizeImage = whole_width * b_Header.biHeight;
		f_Header.bfSize = whole_width * b_Header.biHeight + f_Header.bfOffBits;
	}

	// 영상처리 2개
	if (type == 3) {
		choose(f_Header, b_Header, c_table, img, intX, intY, intValue, processType, startX, startY, finishX, finishY, intRGB);
		file.close();
		return;
	}

	file_save(f_Header, b_Header, c_table, img, startX, startY, finishX, finishY, intRGB);
	file.close();
	return;
}