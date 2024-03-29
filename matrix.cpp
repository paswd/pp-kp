/*
 ==================================================
| __________                 __      __________    |
| \______   \_____    ______/  \    /  \______ \   |
|  |     ___/\__  \  /  ___/\   \/\/   /|    |  \  |
|  |    |     / __ \_\___ \  \        / |    `   \ |
|  |____|    (____  /____  >  \__/\  / /_______  / |
|                \/     \/        \/          \/   |
 ==================================================
(c) Developed by Pavel Sushko (PasWD)
http://paswd.ru/
me@paswd.ru

Original repository:
https://github.com/paswd/pp-kp

All rights reserved
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

#include "matrix.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>

using namespace std;

#define EMPTY_MATRIX TMatrix(0, 0)

TMatrix::TMatrix(std::ifstream &fin, size_t begin, size_t origSize, size_t height, size_t width) {
	Height = height;
	Width = width;
	ReadFromFile(fin, begin, origSize, false);
}

TMatrix::~TMatrix(void) {
	this->Clear();
}

void TMatrix::Clear(void) {

}
void TMatrix::Print(void) {
	for (size_t i = 0; i < Height; i++) {
		for (size_t j = 0; j < Width; j++) {
			cout << GetValue(i, j) << " ";
		}
		cout << endl;
	}
}
void TMatrix::PrintDirect(void) {
	for (size_t i = 0; i < Data.size(); i++) {
		cout << Data[i] << " ";
	}
	cout << endl;
	for (size_t i = 0; i < Cols.size(); i++) {
		cout << Cols[i] << " ";
	}
	cout << endl;
	for (size_t i = 0; i < NewRows.size(); i++) {
		cout << NewRows[i] << " ";
	}

	cout << endl;
}

TNum TMatrix::GetValue(size_t i, size_t j) {
	if (i >= this->Height || j >= this->Width) {
		return 0.;
	}
	TNum res = 0.;
	size_t begin = NewRows[i];
	size_t end = NewRows[i + 1];

	for (size_t k = begin; k < end; k++) {
		if (Cols[k] == j) {
			res = Data[k];
			break;
		}
	}

	return res;
}

size_t TMatrix::GetWidth(void) {
	return this->Width;
}
size_t TMatrix::GetHeight(void) {
	return this->Height;
}

bool TMatrix::IsNull(void) {
	if (this->Height == 0 || this->Width == 0) {
		return true;
	}
	return false;
}

bool TMatrix::ReadFromFile(ifstream &fin, size_t begin, size_t origSize, bool show_imported_matrix) {
	Data.resize(0);
	Cols.resize(0);
	NewRows.resize(0);

	try {
		size_t repeats = 1;
		for (size_t i = 0; i < origSize; i++) {
			bool newRow = true;
			for (size_t j = 0; j < Width; j++) {
				TNum tmp;
				if (!(fin >> tmp)) {
					throw 2;
				}

				if (i < begin || i >= begin + Height) {
					newRow = false;
					continue;
				}
				if (tmp != 0) {
					size_t currPos = Data.size();
					Data.push_back(tmp);
					Cols.push_back(j);
					if (newRow) {
						newRow = false;
						for (size_t k = 0; k < repeats; k++) {
							NewRows.push_back(currPos);
						}
						repeats = 1;
					}
				}
			}
			if (newRow) {
				repeats++;
			}
		}
		NewRows.push_back(Data.size());

	} catch (int a) {
		cout << "Ошибка чтения из файла" << endl;
		return false;
	}
	if (show_imported_matrix) {
		cout << ":" << endl;
		Print();
	}
	return true;
}
