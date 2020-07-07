#include "head_er.h"

void choose(myBITMAPFILEHEADER f_Header, myBITMAPINFOHEADER b_Header, myRGBQUAD *c_table, unsigned char *img, int x, int y, int val, int menu, int startX, int startY, int finishX, int finishY, int *intRGB) {
	int whole_width = (((b_Header.biBitCount * b_Header.biWidth) + 31) / 32 * 4);
	unsigned char* new_img = new unsigned char[whole_width * b_Header.biHeight + 2];

	switch (menu) {
	// 2배 확배
	case 1:
		// 8비트
		if (b_Header.biBitCount != 24) {
			for (int i = y, line = 0; line < b_Header.biHeight; i++, line += 2) {
				for (int j = x, cnt = 0; cnt < b_Header.biWidth; j++, cnt += 2) {
					*(new_img + (b_Header.biHeight - line - 1) * whole_width + cnt) = *(img + (b_Header.biHeight - i - 1) * whole_width + j);
					*(new_img + (b_Header.biHeight - line - 1) * whole_width + cnt + 1) = *(img + (b_Header.biHeight - i - 1) * whole_width + j);
					*(new_img + (b_Header.biHeight - line - 2) * whole_width + cnt) = *(img + (b_Header.biHeight - i - 1) * whole_width + j);
					*(new_img + (b_Header.biHeight - line - 2) * whole_width + cnt + 1) = *(img + (b_Header.biHeight - i - 1) * whole_width + j);
				}
			}
		}

		// 24비트
		else {
			for (int i = b_Header.biHeight - y - 1, line = b_Header.biHeight - 1; line >= 0; i--, line -= 2) {
				for (int j = x, cnt = 0; cnt < b_Header.biWidth; j++, cnt += 2) {
					for (int h = 0; h < 3; h++) {
						*(new_img + line * whole_width + cnt * 3 + h) = *(img + i * whole_width + j * 3 + h);
						*(new_img + line * whole_width + (cnt + 1) * 3 + h) = *(img + i * whole_width + j * 3 + h);
						*(new_img + (line - 1) * whole_width + cnt * 3 + h) = *(img + i * whole_width + j * 3 + h);
						*(new_img + (line - 1) * whole_width + (cnt + 1) * 3 + h) = *(img + i * whole_width + j * 3 + h);
					}
				}
			}
		}

		// 파일 저장
		file_save(f_Header, b_Header, c_table, new_img, startX, startY, finishX, finishY, intRGB);
		return;
	
	// Threshold
	case 2:
		// 8비트
		if (b_Header.biBitCount != 24) for (int i = 0; i < pow(2, b_Header.biBitCount); i++) {
			// 255
			if ((c_table[i].rgbBlue + c_table[i].rgbGreen + c_table[i].rgbRed) / 3 >= val) {
				c_table[i].rgbBlue = 255;
				c_table[i].rgbGreen = 255;
				c_table[i].rgbRed = 255;
			}

			// 0
			else {
				c_table[i].rgbBlue = 0;
				c_table[i].rgbGreen = 0;
				c_table[i].rgbRed = 0;
			}
		}

		// 24비트
		else {
			for (int i = 0; i < b_Header.biHeight; i++) for (int j = 0; j < b_Header.biWidth * 3; j += 3) if ((*(img + i * whole_width + j) + *(img + i * whole_width + j + 1) + *(img + i * whole_width + j + 2)) / 3 >= val) {
				*(img + i * whole_width + j) = 255;
				*(img + i * whole_width + j + 1) = 255;
				*(img + i * whole_width + j + 2) = 255;
			}

			else {
				*(img + i * whole_width + j) = 0;
				*(img + i * whole_width + j + 1) = 0;
				*(img + i * whole_width + j + 2) = 0;
			}
		}

		// 파일 저장
		file_save(f_Header, b_Header, c_table, img, startX, startY, finishX, finishY, intRGB);
		return;
	}
	return;
}

// 박스생성
unsigned char* makeBox(unsigned char *img, int width, int height, int whole_width, int type, int startX, int startY, int finishX, int finishY, int *intRGB) {
	if (startX >= width || startY >= height) return img;
	if (finishX > width) finishX = width;
	if (finishY > height) finishY = height;

	int sw = finishX - startX;
	int sh = finishY - startY;

	// ppm
	if (type == 1) for (int i = 0; i < sh; i++) for (int j = 0; j < sw; j++) for (int channel = 0; channel < 3; channel++) *(img + (startY + i) * width * 3 + (startX + j) * 3 + channel) = 255 - intRGB[channel];

	// bmp - 24bit
	else if (type == 2) for (int i = 0; i < sh; i++) for (int j = 0; j < sw; j++) for (int channel = 0; channel < 3; channel++) *(img + (height - (startY + i + 1)) * whole_width + (startX + j) * 3 + channel) = 255 - intRGB[2 - channel];

	// bmp - 8bit RGB
	else if (type == 3) for (int i = 0; i < sh; i++) for (int j = 0; j < sw; j++) *(img + (height - startY - i - 1) * whole_width + startX + j) = intRGB[0];

	return img;
}