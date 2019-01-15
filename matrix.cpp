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

TMatrix::TMatrix(std::ifstream &fin, size_t height, size_t width) {
	Height = height;
	Width = width;
	ReadFromFile(fin, true);
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

/*void TMatrix::SetValue(TNum value, size_t i, size_t j) {
	if (i >= this->Height || j >= this->Width) {
		return;
	}
	TNum curr = GetValue(i, j);
	if (curr == 0. && value == 0.) {
		break;
	}
}*/
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

bool TMatrix::ReadFromFile(ifstream &fin, bool show_imported_matrix) {
	cout << "Чтение из файла..." << endl;
	/*ifstream fin(filename.c_str());
	if (!fin.is_open()) {
        cout << "Ошибка чтения файла" << endl;
        return false;
	}*/
	Data.resize(0);
	Cols.resize(0);
	NewRows.resize(0);

	try {
		size_t repeats = 1;
		for (size_t i = 0; i < Height; i++) {
			bool newRow = true;
			for (size_t j = 0; j < Width; j++) {
				TNum tmp;
				if (!(fin >> tmp)) {
					throw 2;
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
		//throw 0;

	} catch (int a) {
		//if (a != 0) {
		//delete tmp_matrix;
		cout << "Ошибка чтения из файла" << endl;
		return false;
		//}
	}
	//fin.close();
	//this->Init(tmp_matrix, tmp_matrix->GetHeight(), tmp_matrix->GetWidth());
	//delete tmp_matrix;
	cout << "Матрица успешно импортирована";
	if (show_imported_matrix) {
		cout << ":" << endl;
		for (size_t i = 0; i < Height; i++) {
			for (size_t j = 0; j < Width; j++) {
				cout << this->GetValue(i, j) << " ";
			}
			cout << endl;
		}
	} else {
		cout << endl;
	}
	return true;
}


/*void TMatrix::Minus(void) {
	for (size_t i = 0; i < this->Height; i++) {
		for (size_t j = 0; j < this->Width; j++) {
			this->SetValue(-this->GetValue(i, j), i, j);
		}
	}
}*/
