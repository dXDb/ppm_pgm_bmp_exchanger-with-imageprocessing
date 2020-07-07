#include "head_er.h"

//프로시저 생성
LRESULT CALLBACK WndProc(HWND hWnd, UINT mesg, WPARAM wParam, LPARAM lParam) {
	// 객체 생성
	static HWND staticImg, staticState, btnBmp, btnPpm, btnPgm, btnExpand, edtX, edtY, btnThreshold, edtValue, edtLink, btnImageChange, edtR, edtG, edtB;
	static HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	// BITMAP 표시
	static HDC hdc, hMemorydc;
	static PAINTSTRUCT paintStruct;
	static HBITMAP hImg, hOldImg;
	static BITMAP img;
	static int x, y;

	// 사각형 표시
	static RECT rectRectangle;
	static PAINTSTRUCT paintStructRectangle;
	static HBRUSH brush, oldBrush;
	static char getcharacter;
	static char R[10], G[10], B[10], X[10], Y[10], Value[10];
	static int intRGB[3] = { 0 }, windowX, windowY, windowWidth, windowHeight, changed = 0, intX = 0, intY = 0, intValue = 0, startX = -1, startY = -1, finishX = -1, finishY = -1;
	static long style;
	static TCHAR tfileType[3] = { 0 };
	static int* intImg;
	static ifstream file;
	static myBITMAPFILEHEADER f_Header;
	static myBITMAPINFOHEADER b_Header;
	static myRGBQUAD* c_table;
	// 경로 정보
	static OPENFILENAME ofn;
	static TCHAR szFile[260] = { 0 };

	switch (mesg) {
	case WM_CREATE:
		// 라벨생성 부분
		staticState	   = CreateWindow("STATIC", "상태 : 메세지", WS_CHILD | WS_VISIBLE, 20, 515, 680,  40, hWnd, NULL, hInstance, NULL);

		// 버튼생성 부분
		btnBmp         = CreateWindow("BUTTON", "BMP 저장",    BS_FLAT | WS_CHILD | WS_VISIBLE | WS_DISABLED, 830, 10,  150, 45, hWnd, (HMENU)(WM_USER + 1), hInstance, NULL);
		btnPpm         = CreateWindow("BUTTON", "PPM 저장",    BS_FLAT | WS_CHILD | WS_VISIBLE | WS_DISABLED, 830, 65,  150, 45, hWnd, (HMENU)(WM_USER + 2), hInstance, NULL);
		btnPgm         = CreateWindow("BUTTON", "PGM 저장",    BS_FLAT | WS_CHILD | WS_VISIBLE | WS_DISABLED, 830, 120, 150, 45, hWnd, (HMENU)(WM_USER + 3), hInstance, NULL);
		btnExpand      = CreateWindow("BUTTON", "Expand",      BS_FLAT | WS_CHILD | WS_VISIBLE | WS_DISABLED, 830, 235, 150, 45, hWnd, (HMENU)(WM_USER + 4), hInstance, NULL);
		btnThreshold   = CreateWindow("BUTTON", "Threshold",   BS_FLAT | WS_CHILD | WS_VISIBLE | WS_DISABLED, 830, 350, 150, 45, hWnd, (HMENU)(WM_USER + 5), hInstance, NULL);
		btnImageChange = CreateWindow("BUTTON", "영상 열기",   BS_FLAT | WS_CHILD | WS_VISIBLE              , 830, 460, 150, 45, hWnd, (HMENU)(WM_USER + 6), hInstance, NULL);

		// 에딧생성 부분
		edtX     = CreateWindow("EDIT", "0",        WS_BORDER | WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_DISABLED, 830, 290,  65, 45, hWnd, (HMENU)(WM_USER + 7),  hInstance, NULL);
		edtY     = CreateWindow("EDIT", "0",        WS_BORDER | WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_DISABLED, 915, 290,  65, 45, hWnd, (HMENU)(WM_USER + 8),  hInstance, NULL);
		edtLink  = CreateWindow("EDIT", "파일주소", WS_BORDER | WS_CHILD | WS_VISIBLE             | WS_DISABLED,  20, 460, 800, 45, hWnd, (HMENU)(WM_USER + 9),  hInstance, NULL);
		edtValue = CreateWindow("EDIT", "0",        WS_BORDER | WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_DISABLED, 830, 405, 150, 45, hWnd, (HMENU)(WM_USER + 10), hInstance, NULL);
		edtR     = CreateWindow("EDIT", "0",        WS_BORDER | WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_DISABLED, 690, 515,  70, 40, hWnd, (HMENU)(WM_USER + 11), hInstance, NULL);
		edtG     = CreateWindow("EDIT", "0",        WS_BORDER | WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_DISABLED, 770, 515,  70, 40, hWnd, (HMENU)(WM_USER + 12), hInstance, NULL);
		edtB     = CreateWindow("EDIT", "0",        WS_BORDER | WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_DISABLED, 850, 515,  70, 40, hWnd, (HMENU)(WM_USER + 13), hInstance, NULL);
		break;

	case WM_PAINT:
		// RGB 표시
		hdc = BeginPaint(hWnd, &paintStruct);
		hMemorydc = CreateCompatibleDC(hdc);

		hImg = (HBITMAP)LoadImage(NULL, "temp.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		hOldImg = (HBITMAP)SelectObject(hMemorydc, hImg);

		//영상정보 수집
		GetObject(hImg, sizeof(BITMAP), &img);
		if (img.bmWidth < 410) x = 410;
		else x = img.bmWidth;
		if (img.bmHeight < 410) y = 410;
		else y = img.bmHeight;;

		// (20, 10)부터 영상 생성
		BitBlt(hdc, 20, 10, img.bmWidth, img.bmHeight, hMemorydc, 0, 0, SRCCOPY);

		SelectObject(hMemorydc, hOldImg);
		DeleteObject(hImg);
		DeleteObject(hMemorydc);
		
		// 사각형 색상 표시
		brush    = CreateSolidBrush(RGB(intRGB[0], intRGB[1], intRGB[2]));
		if (b_Header.biBitCount == 8) brush = CreateSolidBrush(RGB(c_table[intRGB[0]].rgbRed, c_table[intRGB[0]].rgbGreen, c_table[intRGB[0]].rgbBlue));
		oldBrush = (HBRUSH)SelectObject(hdc, brush);
		if (changed == 0) Rectangle(hdc, 940, 515, 980, 555);
		else Rectangle(hdc, windowWidth - 20 - 40, 10 + y + 10 + 45 + 10, windowWidth - 20, 10 + y + 10 + 45 + 10 + 40);
		Rectangle(hdc, startX, startY, finishX, finishY);
		SelectObject(hdc, oldBrush);
		EndPaint(hWnd, &paintStruct);
		break;

	// 사각형 시작
	case WM_LBUTTONDOWN:
		if (LOWORD(lParam) >= 20) startX = LOWORD(lParam);
		else startX = 20;

		if (HIWORD(lParam) >= 10) startY = HIWORD(lParam);
		else startY = 10;
		break;

	// 사각형 종료
	case WM_LBUTTONUP:
		if (LOWORD(lParam) <= img.bmWidth + 20) finishX = LOWORD(lParam);
		else finishX = img.bmWidth + 20;

		if (HIWORD(lParam) <= img.bmHeight + 10) {
			finishY = HIWORD(lParam);
			SetWindowText(staticState, "완료 : 사각형 생성 완료");
		} else {
			finishY = img.bmHeight + 10;
			SetWindowText(staticState, "완료 : 사각형 생성 완료");
		}

		if (startX > finishX) swap(startX, finishX);
		if (startY > finishY) swap(startY, finishY);

		if (startX > img.bmWidth + 20 || startY > img.bmHeight + 10 || finishX > img.bmWidth + 20 || finishY > img.bmHeight + 10) {
			startX = -1;
			startY = -1;
			finishX = -1;
			finishY = -1;
			SetWindowText(staticState, "에러 : 사각형 생성 불가 지역");
		}

		if (startX < 20 || startY < 10 || finishX < 20 || finishY < 10) {
			startX = -1;
			startY = -1;
			finishX = -1;
			finishY = -1;
			SetWindowText(staticState, "에러 : 사각형 생성 불가 지역");
		}

		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		break;

	case WM_COMMAND:
		// bmp 저장
		if (LOWORD(wParam) == WM_USER + 1) {
			if (strcmp(tfileType, "bmp") == 0) {
				bmp_rgb(szFile, 2, 0, intX, intY, intValue, startX - 20, startY - 10, finishX - 20, finishY - 10, intRGB);
				SetWindowText(staticState, "완료 : bmp 파일을 output.bmp으로 변환");
			}

			if (strcmp(tfileType, "ppm") == 0) {
				exchange_file(szFile, "bmp", startX - 20, startY - 10, finishX - 20, finishY - 10, intRGB);
				SetWindowText(staticState, "완료 : ppm 파일을 output.bmp으로 변환");
			}

			if (strcmp(tfileType, "pgm") == 0) {
				exchange_file(szFile, "bmp", startX - 20, startY - 10, finishX - 20, finishY - 10, intRGB);
				SetWindowText(staticState, "완료 : pgm 파일을 output.bmp으로 변환");
			}
		}

		// ppm 저장
		if (LOWORD(wParam) == WM_USER + 2) {
			if (strcmp(tfileType, "bmp") == 0) {
				exchange_file(szFile, "ppm", startX - 20, startY - 10, finishX - 20, finishY - 10, intRGB);
				SetWindowText(staticState, "완료 : bmp 파일을 output.ppm으로 변환");
			}

			if (strcmp(tfileType, "pgm") == 0) {
				exchange_file(szFile, "ppm", startX - 20, startY - 10, finishX - 20, finishY - 10, intRGB);
				SetWindowText(staticState, "완료 : pgm 파일을 output.ppm으로 변환");
			}
		}

		// pgm 저장
		if (LOWORD(wParam) == WM_USER + 3) {
			if (strcmp(tfileType, "bmp") == 0) {
				exchange_file(szFile, "pgm", startX - 20, startY - 10, finishX - 20, finishY - 10, intRGB);
				SetWindowText(staticState, "완료 : bmp 파일을 output.pgm으로 변환");
			}

			if (strcmp(tfileType, "ppm") == 0) {
				exchange_file(szFile, "pgm", startX - 20, startY - 10, finishX - 20, finishY - 10, intRGB);
				SetWindowText(staticState, "완료 : ppm 파일을 output.pgm으로 변환");
			}
		}

		// Expand
		if (LOWORD(wParam) == WM_USER + 4) {
			bmp_rgb(szFile, 3, 1, intX, intY, intValue, (startX - 20) * 2, (startY - 10) * 2, (finishX - 20) * 2, (finishY - 10) * 2, intRGB);
			SetWindowText(staticState, "완료 : Expand 후 output.bmp 저장");
		}

		// Threshold
		if (LOWORD(wParam) == WM_USER + 5) {
			bmp_rgb(szFile, 3, 2, intX, intY, intValue, startX - 20, startY - 10, finishX - 20, finishY - 10, intRGB);
			SetWindowText(staticState, "완료 : Threshold 후 output.bmp 저장");
		}

		// 이미지 로드
		if (LOWORD(wParam) == WM_USER + 6) {
			// 초기화
			f_Header = { 0 };
			b_Header = { 0 };
			startX = -1;
			startY = -1;
			finishX = -1;
			finishY = -1;

			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = _T("bmp\0*.bmp\0ppm\0*.ppm\0pgm\0*.pgm\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&ofn) != TRUE) break;

			// 파일 확장자명 얻기
			tfileType[0] = ofn.lpstrFile[strlen(ofn.lpstrFile) - 3];
			tfileType[1] = ofn.lpstrFile[strlen(ofn.lpstrFile) - 2];
			tfileType[2] = ofn.lpstrFile[strlen(ofn.lpstrFile) - 1];
			for (int i = 0; i < 3; i++) tfileType[i] = tolower(tfileType[i]);

			if (strcmp(tfileType, "bmp") == 0) {
				file = ifstream(ofn.lpstrFile, ios::binary);
				file.read((char*)&f_Header.bfType, 2);
				file.read((char*)&f_Header.bfSize, 4);
				file.read((char*)&f_Header.bfReserved1, 2);
				file.read((char*)&f_Header.bfReserved2, 2);
				file.read((char*)&f_Header.bfOffBits, 4);

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
				c_table = new myRGBQUAD[256];
				if (b_Header.biBitCount != 24) file.read((char*)c_table, f_Header.bfOffBits - 54);
			}

			// 버튼 활성화 비활성화 설정
			if (strcmp(tfileType, "bmp") != 0) {
				EnableWindow(btnBmp, TRUE);
				EnableWindow(btnExpand, FALSE);
				EnableWindow(btnThreshold, FALSE);
				EnableWindow(edtX, FALSE);
				EnableWindow(edtY, FALSE);
				EnableWindow(edtValue, FALSE);
			} else {
				EnableWindow(btnBmp, TRUE);
				EnableWindow(btnExpand, TRUE);
				EnableWindow(btnThreshold, TRUE);
				EnableWindow(edtX, TRUE);
				EnableWindow(edtY, TRUE);
				EnableWindow(edtValue, TRUE);
			}

			if (strcmp(tfileType, "ppm") != 0) EnableWindow(btnPpm, TRUE);
			else {
				EnableWindow(btnPpm, FALSE);
				EnableWindow(btnExpand, FALSE);
				EnableWindow(btnThreshold, FALSE);
				EnableWindow(edtX, FALSE);
				EnableWindow(edtY, FALSE);
				EnableWindow(edtValue, FALSE);
			}

			if (strcmp(tfileType, "pgm") != 0) EnableWindow(btnPgm, TRUE);
			else {
				EnableWindow(btnPgm, FALSE);
				EnableWindow(btnExpand, FALSE);
				EnableWindow(btnThreshold, FALSE);
				EnableWindow(edtX, FALSE);
				EnableWindow(edtY, FALSE);
				EnableWindow(edtValue, FALSE);
			}

			EnableWindow(edtR  , TRUE);
			EnableWindow(edtG  , TRUE);
			EnableWindow(edtB  , TRUE);

			if (b_Header.biBitCount == 8) {
				EnableWindow(edtG, FALSE);
				EnableWindow(edtB, FALSE);
				SetWindowText(edtG, "");
				SetWindowText(edtB, "");
			}

			// ppm pgm 이미지 설정
			remove("temp.bmp");
			if (strcmp(tfileType, "bmp") == 0) CopyFile(ofn.lpstrFile, "temp.bmp", FALSE);
			if (strcmp(tfileType, "ppm") == 0) exchange_file(szFile, "pppm", startX - 20, startY - 10, finishX - 20, finishY - 10, intRGB);
			if (strcmp(tfileType, "pgm") == 0) exchange_file(szFile, "ppgm", startX - 20, startY - 10, finishX - 20, finishY - 10, intRGB);

			// 영상 값 업데이트
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);

			// 화면 조정
			windowX      = 400;
			windowY      = 200;
			windowWidth  = 20 + x + 10 + 150 + 20;
			windowHeight = 31 + 10 + y + 10 + 45 + 10 + 45 + 10;

			// 메인
			MoveWindow(hWnd, windowX, windowY, windowWidth, windowHeight, TRUE);

			// 버튼
			MoveWindow(btnBmp,         20 + x + 10, 10,                                       150, 45, TRUE);
			MoveWindow(btnPpm,         20 + x + 10, 10 + 45 + 10,                             150, 45, TRUE);
			MoveWindow(btnPgm,         20 + x + 10, 10 + 45 + 10 + 45 + 10,                   150, 45, TRUE);
			MoveWindow(btnExpand,      20 + x + 10, 10 + y - 45 -10 - 45 - 10 - 45 - 10 - 45, 150, 45, TRUE);
			MoveWindow(btnThreshold,   20 + x + 10, 10 + y - 45 -10 - 45,                     150, 45, TRUE);
			MoveWindow(btnImageChange, 20 + x + 10, 10 + y + 10,                              150, 45, TRUE);

			// 에딧
			MoveWindow(edtX,     20 + x + 10,                                    10 + y - 45 -10 - 45 - 10 - 45, 65 , 45, TRUE);
			MoveWindow(edtY,     windowWidth - 20 - 65,                          10 + y - 45 -10 - 45 - 10 - 45, 65 , 45, TRUE);
			MoveWindow(edtLink,  20,                                             10 + y + 10                   , x,   45, TRUE);
			MoveWindow(edtValue, 20 + x + 10,                                    10 + y - 45                   , 150, 45, TRUE);
			MoveWindow(edtR,     windowWidth - 20 - 40 - 10 - 40 - 10 - 40 - 10 - 40, 10 + y + 10 + 45 + 10,     40,  40, TRUE);
			MoveWindow(edtG,     windowWidth - 20 - 40 - 10 - 40 - 10 - 40,           10 + y + 10 + 45 + 10,     40,  40, TRUE);
			MoveWindow(edtB,     windowWidth - 20 - 40 - 10 - 40,                     10 + y + 10 + 45 + 10,     40,  40, TRUE);

			// 스태틱
			MoveWindow(staticState, 20, 10 + y + 10 + 45 + 10, x - 120, 40, TRUE);

			changed = 1;

			SetWindowText(edtLink, ofn.lpstrFile);
			SetWindowText(staticState, "완료 : 영상을 불러왔습니다.");

			GetClientRect(hWnd, &rectRectangle);
			InvalidateRect(hWnd, &rectRectangle, TRUE);
		}

		// 영상처리기능 값 조절
		// X값 범위
		if (LOWORD(wParam) == WM_USER + 7) {
			if (HIWORD(wParam) == EN_CHANGE) {
				GetWindowText(edtX, X, 10);
				intX = atoi(X);
				if (intX > x / 2) {
					_itoa_s(x / 2, X, 10);
					SetWindowText(edtX, X);
					intX = x / 2;
					SetWindowText(staticState, "에러 : 최대 값은 이미지의 절반 입니다");
				}

				if (intX < 0) {
					SetWindowText(edtX, "0");
					intX = 0;
					SetWindowText(staticState, "에러 : 최소 값은 0입니다");
				}
			}
		}

		// Y값 범위
		if (LOWORD(wParam) == WM_USER + 8) {
			if (HIWORD(wParam) == EN_CHANGE) {
				GetWindowText(edtY, Y, 10);
				intY = atoi(Y);
				if (intY > y / 2) {
					_itoa_s(y / 2, Y, 10);
					SetWindowText(edtY, Y);
					intY = y / 2;
					SetWindowText(staticState, "에러 : 최대 값은 이미지의 절반 입니다");
				}

				if (intY < 0) {
					SetWindowText(edtY, "0");
					intY = 0;
					SetWindowText(staticState, "에러 : 최소 값은 0입니다");
				}
			}
		}

		// Value값 범위
		if (LOWORD(wParam) == WM_USER + 10) {
			if (HIWORD(wParam) == EN_CHANGE) {
				GetWindowText(edtValue, Value, 10);
				intValue = atoi(Value);
				if (intValue > 255) {
					SetWindowText(edtValue, "255");
					intValue = 255;
					SetWindowText(staticState, "에러 : 최대 값은 255입니다");
				}

				if (intValue < 0) {
					SetWindowText(edtValue, "0");
					intValue = 0;
					SetWindowText(staticState, "에러 : 최소 값은 0입니다");
				}
			}
		}

		// 사각형 색상 지정
		// R 컬러 범위
		if (LOWORD(wParam) == WM_USER + 11) {
			if (HIWORD(wParam) == EN_CHANGE) {
				GetWindowText(edtR, R, 10);
				intRGB[0] = atoi(R);
				if (intRGB[0] > 255) {
					SetWindowText(edtR, "255");
					intRGB[0] = 255;
					SetWindowText(staticState, "에러 : 최대 크기는 255입니다");
				}
					
				if(intRGB[0] < 0) {
					SetWindowText(edtR, "0");
					intRGB[0] = 0;
					SetWindowText(staticState, "에러 : 최소 크기는 0입니다");
				}


				GetClientRect(hWnd, &rectRectangle);
				InvalidateRect(hWnd, &rectRectangle, TRUE);
				break;
			}
		}

		// G 컬러 범위
		if (LOWORD(wParam) == WM_USER + 12) {
			if (HIWORD(wParam) == EN_CHANGE) {
				GetWindowText(edtG, G, 10);
				intRGB[1] = atoi(G);
				if (intRGB[1] > 255) {
					SetWindowText(edtG, "255");
					intRGB[1] = 255;
					SetWindowText(staticState, "에러 : 최대 크기는 255입니다");
				}
					
				if (intRGB[1] < 0) {
					SetWindowText(edtG, "0");
					intRGB[1] = 0;
					SetWindowText(staticState, "에러 : 최소 크기는 0입니다");
				}
				GetClientRect(hWnd, &rectRectangle);
				InvalidateRect(hWnd, &rectRectangle, TRUE);
				break;
			}
		}

		// B 컬러 범위
		if (LOWORD(wParam) == WM_USER + 13) {
			if (HIWORD(wParam) == EN_CHANGE) {
				GetWindowText(edtB, B, 10);
				intRGB[2] = atoi(B);
				if (intRGB[2] > 255) {
					SetWindowText(edtB, "255");
					intRGB[2] = 255;
					SetWindowText(staticState, "에러 : 최대 크기는 255입니다");
				}
					
				if (intRGB[2] < 0) {
					SetWindowText(edtB, "0");
					intRGB[2] = 0;
					SetWindowText(staticState, "에러 : 최소 크기는 0입니다");
				}

				GetClientRect(hWnd, &rectRectangle);
				InvalidateRect(hWnd, &rectRectangle, TRUE);
				break;
			}
		}
		break;

	// 종료
	case WM_DESTROY:
		remove("temp.bmp");
		PostQuitMessage(0);
		return FALSE;
	}

	return DefWindowProc(hWnd, mesg, wParam, lParam);
}

// 메인윈도우 생성
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprevInstance, LPSTR lpszArg, int nCmdShow) {
	HWND hWnd;
	MSG msg;
	WNDCLASS WndClass;

	WndClass.style = NULL;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = "hwnd";
	if (!RegisterClass(&WndClass)) return NULL;

	hWnd = CreateWindow("hwnd", "컴퓨터공학과 2학년 류원정 20160434", WS_CAPTION | WS_POPUPWINDOW, 400, 200, 1000, 601, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}