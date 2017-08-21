#include <iostream>
#include <fstream>
#include <Windows.h>
#include "bitmap.h"
#include <algorithm>
using namespace std;



bool isBmpFile(ifstream &f)
{
	if (f.fail()) {
		return false;
	}
	bmpSignature sign;
	f.read((char*)&sign, sizeof(bmpSignature));
	return (sign.data[0] == 'B' && sign.data[1] == 'M');
}

int CountPaddingBytes(const Bmpfile &X)
{
	char paddingCount = (4 - ((X.pixelarray.columnCount * (X.dib.pixelSize / 8)) % 4)) % 4;

	return paddingCount;
}

void readBmpHeader(ifstream &f, Bmpfile &X)
{
	if (f.fail())
		return;
	f.seekg(0, f.beg);
	f.read((char*)&X.header, sizeof(BmpHeader));

}

void readDib(ifstream &f, Bmpfile &X)
{
	if (f.fail())
		return;

	f.seekg(sizeof(BmpHeader), f.beg);
	f.read((char*)&X.dib, sizeof(BmpDib));
}

void readPixelArr(ifstream &f, Bmpfile &X)
{
	if (f.fail())
		return;
	X.pixelarray.columnCount = X.dib.imageWidth;
	X.pixelarray.rowCount = X.dib.imageHeight;

	char paddingCount = CountPaddingBytes(X);

	f.seekg(X.header.dataOffset, f.beg);
	X.pixelarray.pixels = new Color*[X.pixelarray.rowCount];

	for (int i = 0; i < X.pixelarray.rowCount; i++)
	{
		X.pixelarray.pixels[X.pixelarray.rowCount - i - 1] = new Color[X.dib.imageWidth];
		for (int j = 0; j < X.dib.imageWidth; j++)
			f.read((char*)&X.pixelarray.pixels[X.pixelarray.rowCount - i - 1][j], sizeof(Color));
		f.seekg(paddingCount, f.cur);
	}
}

void readBmp(ifstream &f, Bmpfile &X)
{
	readBmpHeader(f, X);
	readDib(f, X);
	readPixelArr(f, X);
}

int truncate(int value)
{
	if (value < 0) return 0;
	if (value > 255) return 255;

	return value;
}

void Brighten(Bmpfile &X)
{
	for (int i = 0; i < X.pixelarray.rowCount; i++)
		for (int j = 0; j < X.pixelarray.columnCount; j++) {
			X.pixelarray.pixels[i][j].blue = truncate(X.pixelarray.pixels[i][j].blue + _BRIGHTEN_ADJUST);
			X.pixelarray.pixels[i][j].red = truncate(X.pixelarray.pixels[i][j].red + _BRIGHTEN_ADJUST);
			X.pixelarray.pixels[i][j].green = truncate(X.pixelarray.pixels[i][j].green + _BRIGHTEN_ADJUST);
		}
}

void Darken(Bmpfile &X)
{
	for (int i = 0; i < X.pixelarray.rowCount; i++)
		for (int j = 0; j < X.pixelarray.columnCount; j++) {
			X.pixelarray.pixels[i][j].blue = truncate((X.pixelarray.pixels[i][j].blue + _DARKEN_ADJUST) );
			X.pixelarray.pixels[i][j].red = truncate((X.pixelarray.pixels[i][j].red + _DARKEN_ADJUST));
			X.pixelarray.pixels[i][j].green = truncate((X.pixelarray.pixels[i][j].green + _DARKEN_ADJUST));
		}
}

void Hotter(Bmpfile &X)
{
	for (int i = 0; i < X.pixelarray.rowCount; i++)
		for (int j = 0; j < X.pixelarray.columnCount; j++) {
			X.pixelarray.pixels[i][j].red = truncate((X.pixelarray.pixels[i][j].red + 10));
		}
}

void Cooler(Bmpfile &X)
{
	for (int i = 0; i < X.pixelarray.rowCount; i++)
		for (int j = 0; j < X.pixelarray.columnCount; j++) {
			X.pixelarray.pixels[i][j].blue = truncate((X.pixelarray.pixels[i][j].blue + 10));
		}
}

void GrayScale(Bmpfile &X)
{
	for (int i = 0; i < X.pixelarray.rowCount; i++)
		for (int j = 0; j < X.pixelarray.columnCount; j++) {
			int _gray_adjust = (X.pixelarray.pixels[i][j].blue+ X.pixelarray.pixels[i][j].red +
				X.pixelarray.pixels[i][j].green) / 3;

			X.pixelarray.pixels[i][j].blue = _gray_adjust;
			X.pixelarray.pixels[i][j].red = _gray_adjust;
			X.pixelarray.pixels[i][j].green = _gray_adjust;
		}
}

void FlipHorizontal(Bmpfile &X)
{
	for (int i = 0; i < X.pixelarray.rowCount; i++)
		for (int j = 0; j < X.pixelarray.columnCount / 2; j++) {
			swap(X.pixelarray.pixels[i][j], X.pixelarray.pixels[i][X.pixelarray.columnCount - j - 1]);
		}
}

void FlipVertical(Bmpfile &X)
{
	for (int i = 0; i < X.pixelarray.columnCount; i++)
		for (int j = 0; j < X.pixelarray.rowCount / 2; j++) {
			swap(X.pixelarray.pixels[j][i], X.pixelarray.pixels[X.pixelarray.rowCount - j - 1][i]);
		}
}

void CreateBorder(Bmpfile &X)
{
	
	int32_t old_H = X.dib.imageHeight;
	int32_t old_W = X.dib.imageWidth;

	X.pixelarray.rowCount = X.dib.imageHeight += 2 * _BORDER_LENGTH; // 2*10
	X.pixelarray.columnCount = X.dib.imageWidth += 2 * _BORDER_LENGTH;

	int padding = CountPaddingBytes(X);

	X.dib.bitmapByteCount = X.dib.imageHeight*(X.dib.imageWidth *(X.dib.pixelSize / 8) + padding);

	X.header.fileSize = X.header.dataOffset + X.dib.bitmapByteCount;
	//------------------------------------------------------------------

	Color **tmp_pixels;
	tmp_pixels = new Color*[old_H];

	for (int i = 0; i < old_H; i++) {
		tmp_pixels[i] = new Color[old_W];

		for (int j = 0; j < old_W; j++) {
			tmp_pixels[i][j] = X.pixelarray.pixels[i][j];
		}
	}

	//----------------------------------------------------------------

	int32_t New_H = X.dib.imageHeight;
	int32_t New_W = X.dib.imageWidth;

	releaseBmpPixelArray(X.pixelarray.pixels, old_H);

	X.pixelarray.pixels = new Color*[X.pixelarray.rowCount];

	for (int i = 0; i < X.pixelarray.rowCount; i++)
	{
		X.pixelarray.pixels[i] = new Color[X.dib.imageWidth];
		for (int j = 0; j < X.dib.imageWidth; j++) {
			X.pixelarray.pixels[i][j].red = _BORDER_RED;  
			X.pixelarray.pixels[i][j].green = _BORDER_GREEN;
			X.pixelarray.pixels[i][j].blue = _BORDER_BLUE;
		}
	}

	for (int i = 0; i < old_H; i++) {
		for (int j = 0; j < old_W; j++) {
			X.pixelarray.pixels[i + _BORDER_LENGTH][j + _BORDER_LENGTH] = tmp_pixels[i][j];
		}
	}
	
	releaseBmpPixelArray(tmp_pixels, old_H);
	
}

void CutBmp(Bmpfile &X, int x_left_top, int y_left_top, int x_right_bot, int y_right_bot)
{
	int New_H = x_right_bot - x_left_top;
	int New_W = y_right_bot - y_left_top;

	int32_t old_H = X.dib.imageHeight;
	int32_t old_W = X.dib.imageWidth;

	Color **tmp_pixels;
	tmp_pixels = new Color*[old_H];

	for (int i = 0; i < old_H; i++) {
		tmp_pixels[i] = new Color[old_W];

		for (int j = 0; j < old_W; j++) {
			tmp_pixels[i][j] = X.pixelarray.pixels[i][j];
		}
	}

	releaseBmpPixelArray(X.pixelarray.pixels, old_H);

	X.pixelarray.rowCount = X.dib.imageHeight = New_H;
	X.pixelarray.columnCount = X.dib.imageWidth = New_W;

	int padding = CountPaddingBytes(X);

	X.dib.bitmapByteCount = X.dib.imageHeight*(X.dib.imageWidth *(X.dib.pixelSize / 8) + padding);

	X.header.fileSize = X.header.dataOffset + X.dib.bitmapByteCount;


	X.pixelarray.pixels = new Color*[X.pixelarray.rowCount];

	int i_tmp = x_left_top;
	int j_tmp = y_left_top;
	for (int i = 0; i < X.pixelarray.rowCount; i++)
	{	
		X.pixelarray.pixels[i] = new Color[X.dib.imageWidth];
		j_tmp = y_left_top;
		for (int j = 0; j < X.dib.imageWidth; j++) {
			X.pixelarray.pixels[i][j] = tmp_pixels[i_tmp][j_tmp];
			j_tmp++;
		}
		i_tmp++;
	}

	
	releaseBmpPixelArray(tmp_pixels, old_H);
}

void Blur3x3(int i, int j, Bmpfile &X, Color **tmp_pixels)
{
	int avgR = 0;
	int avgG = 0;
	int avgB = 0;
	int cnt = 0;
	for (int w = i - 1; w <= i + 1; w++) {
		if (w < 0 || w >= X.dib.imageHeight)
			continue;
		else 
			for (int z = j - 1; z <= j + 1; z++) {
			if (z < 0 || z >= X.dib.imageWidth)
				continue;
			else {
				avgR = avgR + (tmp_pixels[w][z].red);
				avgG = avgG + (tmp_pixels[w][z].green);
				avgB = avgB + (tmp_pixels[w][z].blue);
				cnt++;	
			}
		}
	}

	X.pixelarray.pixels[i][j].red = (avgR / cnt);
	X.pixelarray.pixels[i][j].green = (avgG / cnt);
	X.pixelarray.pixels[i][j].blue = (avgB /cnt);

}

void BlurBmp(Bmpfile &X)
{
	Color **tmp_pixels;
	tmp_pixels = new Color*[X.dib.imageHeight];

	for (int i = 0; i < X.dib.imageHeight; i++) {
		tmp_pixels[i] = new Color[X.dib.imageWidth];

		for (int j = 0; j < X.dib.imageWidth; j++) {
			tmp_pixels[i][j] = X.pixelarray.pixels[i][j];
		}
	}

	for (int i = 0; i < X.pixelarray.rowCount; i++)
		for (int j = 0; j < X.pixelarray.columnCount; j++) {
			Blur3x3(i, j, X, tmp_pixels);
		}

	releaseBmpPixelArray(tmp_pixels, X.dib.imageHeight);
}

void drawBmp(BmpDib dib, PixelArray data)
{
	HWND console = GetConsoleWindow();
	HDC hdc = GetDC(console);

	for (int i = 0; i < dib.imageHeight; i++)
		for (int j = 0; j < dib.imageWidth; j++)
		{
			Color pixel = data.pixels[i][j];
			SetPixel(hdc, j + 400, i, RGB(pixel.red, pixel.green, pixel.blue));
		}

	ReleaseDC(console, hdc);
}

void writeBmp(char *filename, Bmpfile &X)
{
	ofstream f(filename, ios::binary);

	f.write((char*)&X.header, sizeof(BmpHeader));
	f.write((char*)&X.dib, sizeof(BmpDib));

	char paddingCount = (4 - (X.dib.imageWidth * (X.dib.pixelSize / 8) % 4)) % 4;

	for (int i = 0; i < X.pixelarray.rowCount; i++)
	{
		for (int j = 0; j < X.pixelarray.columnCount; j++)
			f.write((char*)&X.pixelarray.pixels[X.pixelarray.rowCount - i - 1][j], sizeof(Color));
		f.seekp(paddingCount, f.cur);
	}

	f.close();
}

void releaseBmpPixelArray(Color **pixelarray, int rowCount)
{
	for (int i = 0; i < rowCount; i++)
		delete[]pixelarray[i];

	delete[]pixelarray;
}

