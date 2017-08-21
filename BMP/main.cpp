#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <stdlib.h>
#include "bitmap.h"
#include <fstream>

using namespace std;

char filename_in[100], filename_out[100];
Bmpfile X;

void InputFileOut() {
	cout << "Enter the name of the output file:";
	cin.ignore();
	cin.getline(filename_out, 100);
}

int main()
{
	while (true) {
		system("cls");
		
		cout << "Welcome to the image processing" << endl;
		cout << "Enter the bmp file: ";

		cin.getline(filename_in, 100);
		ifstream fin(filename_in, ios::binary);
		
		if (fin.fail()) {
			cout << "Can't not open this file" << endl;
			
		}

		else
		{
			bool check_console = false;
			system("cls");
			readBmp(fin, X);
			
			cout << "Menu:" << endl;
			cout << "1.Brighten" << endl;
			cout << "2.Darken" << endl;
			cout << "3.Hotter" << endl;
			cout << "4.Cooler" << endl;
			cout << "5.Grayscale" << endl;
			cout << "6.Flip Horizontal" << endl;
			cout << "7.Flip Vertical" << endl;
			cout << "8.Create border" << endl;
			cout << "9.Draw the picture to console" << endl;
			cout << "10.Cut bitmap picture" << endl;
			cout << "11.Blur bitmap picture" << endl;

			int key;
			do {
				cout << "Your choice:";
				cin >> key;
			} while (key < 1 || key > 11);

			ofstream fout(filename_out, ios::binary);
			switch (key)
			{
			case 1:
				InputFileOut();
				Brighten(X);
				writeBmp(filename_out, X);
				break;
			case 2:
				InputFileOut();
				Darken(X);
				writeBmp(filename_out, X);
				break;
			case 3:
				InputFileOut();
				Hotter(X);
				writeBmp(filename_out, X);
				break;
			case 4:
				InputFileOut();
				Cooler(X);
				writeBmp(filename_out, X);
				break;
			case 5: 
				InputFileOut();
				GrayScale(X);
				writeBmp(filename_out, X);
				break;
			case 6:
				InputFileOut();
				FlipHorizontal(X);
				writeBmp(filename_out, X);
				break;
			case 7:
				InputFileOut();
				FlipVertical(X);
				writeBmp(filename_out, X);
				break;
			case 8:
				InputFileOut();
				CreateBorder(X);
				writeBmp(filename_out, X);
				break;
			case 9:
				drawBmp(X.dib, X.pixelarray);
				check_console = true;
				break;
			case 10:
				InputFileOut();
				cout << "The size of picture is: " << X.dib.imageHeight << "*" << X.dib.imageWidth << " (pixels)" << endl;
				cout << "The picture beside" << endl;
				drawBmp(X.dib, X.pixelarray);
				int x_left_top, y_left_top, x_right_bot, y_right_bot;
				while (true) {
					cout << "Enter coordinate in the left top (x,y): ";
					cin >>  x_left_top >> y_left_top;
					cout << "Enter coordinate in the right bottom (x,y): ";
					cin >> x_right_bot >> y_right_bot;
					if (x_right_bot > x_left_top && y_right_bot > y_left_top &&
						x_right_bot >= 0 && x_right_bot < X.dib.imageHeight &&
						x_left_top >= 0 && x_left_top < X.dib.imageHeight &&
						y_right_bot >= 0 && y_right_bot < X.dib.imageWidth &&
						y_left_top >= 0 && y_left_top < X.dib.imageWidth)
						break;
					else
						cout << "Invalid coordinate, input again ^^!!" << endl;
				}
				CutBmp(X, x_left_top, y_left_top, x_right_bot, y_right_bot);
				writeBmp(filename_out, X);
				break;
			case 11:
				InputFileOut();
				BlurBmp(X);
				writeBmp(filename_out, X);
				break;
			}

			if (!check_console)
			{

				char key1;
				do {
					cout << "Do you want to draw the picture after processing to console [y/n]: ";
					cin >> key1;
				} while (key1 != 'y' && key1 != 'n');

				if (key1 == 'y') {
					system("cls");
					drawBmp(X.dib, X.pixelarray);
				}
			}
		}
		char key2;
		do {
			cout << "Do you want to continue[y/n]: ";
			cin >> key2;
		} while (key2 != 'y' && key2 != 'n');

		if (key2 == 'n')
			break;
		else {
			cin.ignore();
			releaseBmpPixelArray(X.pixelarray.pixels, X.dib.imageHeight);
			continue;
		}
		

	}
	releaseBmpPixelArray(X.pixelarray.pixels, X.dib.imageHeight);
	return 0;
}
