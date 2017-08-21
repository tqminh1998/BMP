#pragma once
#include <stdint.h>
#include <stdio.h>
#include <fstream>

#pragma pack(1)

#define SIZE_OFHEADER 14
#define SIZE_OFDIB 40


#define _BRIGHTEN_ADJUST 60
#define _DARKEN_ADJUST -20

#define _BORDER_LENGTH 10
#define _BORDER_RED 200
#define _BORDER_BLUE 120
#define _BORDER_GREEN 255

using namespace std;
struct bmpSignature
{
	unsigned char data[2];
};

struct BmpHeader
{
	bmpSignature signature;
	uint32_t fileSize;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t dataOffset;
};

struct BmpDib
{
	uint32_t dibSize;
	int32_t	 imageWidth;
	int32_t  imageHeight;
	uint16_t colorPlaneCount;
	uint16_t pixelSize;
	uint32_t compressMethod;
	uint32_t bitmapByteCount;
	int32_t  horizontalResolution;
	int32_t  verticalResolution;
	uint32_t colorCount;
	uint32_t importantColorCount;
};

struct Color
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
};

struct PixelArray
{
	Color	 **pixels; 
	uint32_t rowCount;
	uint32_t columnCount;
};

struct Bmpfile
{
	BmpHeader header;
	BmpDib dib;
	PixelArray pixelarray;
};

bool isBmpFile(ifstream &f);

int CountPaddingBytes(const Bmpfile &X);

// read bmp file
void readBmp(ifstream &f, Bmpfile &X);

void readPixelArr(ifstream &f, Bmpfile &X);

void readBmpHeader(ifstream &f, Bmpfile &X);

void readDib(ifstream &f, Bmpfile &X);

// main functions
int truncate(int value);

void Brighten(Bmpfile &X);

void Darken(Bmpfile &X);

void Hotter(Bmpfile &X);

void Cooler(Bmpfile &X);

void GrayScale(Bmpfile &X);

void FlipHorizontal(Bmpfile &X);

void FlipVertical(Bmpfile &X);

void CreateBorder(Bmpfile &X);

void CutBmp(Bmpfile &X, int x_left_top, int y_left_top, int x_right_bot, int y_right_bot);

void Blur3x3(int i, int j, Bmpfile &X);

void BlurBmp(Bmpfile &X);

void drawBmp(BmpDib dib, PixelArray data);

// write bmp file
void writeBmp(char *filename, Bmpfile &X);

void releaseBmpPixelArray(Color **pixelarray, int rowCount);