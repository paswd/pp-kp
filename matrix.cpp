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

TMatrix::TMatrix(void) {
	this->InitStatus = false;
	this->Height = 0;
	this->Width = 0;
}
TMatrix::TMatrix(TMatrix *sample) {
	this->InitStatus = false;
	this->Init(sample, sample->GetHeight(), sample->GetWidth());
}
TMatrix::TMatrix(size_t h, size_t w) {
	this->InitStatus = false;
	this->Init(NULL, h, w);
}
TMatrix::TMatrix(vector <TNum> vect) {
	this->InitStatus = false;
	this->Init(NULL, vect.size(), 1);
	for (size_t i = 0; i < vect.size(); i++) {
		this->SetValue(vect[i], i, 1);
	}
}

TMatrix::~TMatrix(void) {
	this->Clear();
}

void TMatrix::Init(TMatrix *sample, size_t h, size_t w) {
	if (this->InitStatus) {
		this->Clear();
	}
	this->Height = h;
	this->Width = w;
	this->Values = new TNum*[this->Height];
	for (size_t i = 0; i < this->Height; i++) {
		this->Values[i] = new TNum[this->Width];
		for (size_t j = 0; j < this->Width; j++) {
			if (sample == NULL) {
				this->Values[i][j] = 0.;
			} else {
				this->Values[i][j] = sample->GetValue(i, j);
			}
		}
	}
	this->InitStatus = true;
}
void TMatrix::Clear(void) {
	if (!this->InitStatus) {
		return;
	}
	for (size_t i = 0; i < this->Height; i++) {
		delete [] this->Values[i];
	}
	delete [] this->Values;
	this->Height = 0;
	this->Width = 0;
	this->InitStatus = false;
}

void TMatrix::Get(void) {
	for (size_t i = 0; i < this->Height; i++) {
		for (size_t j = 0; j < this->Width; j++) {
			cin >> this->Values[i][j];
		}
	}
}
void TMatrix::Print(void) {
	for (size_t i = 0; i < this->Height; i++) {
		for (size_t j = 0; j < this->Width; j++) {
			cout << this->Values[i][j] << " ";
		}
		cout << endl;
	}
}

TNum TMatrix::GetValue(size_t i, size_t j) {
	if (i >= this->Height || j >= this->Width) {
		return 0;
	}
	return this->Values[i][j];
}

void TMatrix::SetValue(TNum value, size_t i, size_t j) {
	if (i >= this->Height || j >= this->Width) {
		return;
	}
	this->Values[i][j] = value;
}
size_t TMatrix::GetWidth(void) {
	return this->Width;
}
size_t TMatrix::GetHeight(void) {
	return this->Height;
}

void TMatrix::Resize(size_t h, size_t w) {
	/*TMatrix *tmp = new TMatrix(h, w);
	for (size_t i = 0; i < min(this->Height, h); i++) {
		for (size_t j = 0; j < min(this->Width, w); j++) {
			tmp->SetValue(this->Values[i][j], i, j);
		}
	}*/
	TNum **new_values = new TNum*[h];
	for (size_t i = 0; i < h; i++) {
		new_values[i] = new TNum[w];
		for (size_t j = 0; j < w; j++) {
			if (i < this->Height && j < this->Width) {
				new_values[i][j] = this->Values[i][j];
			} else {
				this->Values[i][j] = 0.;
			}
		}
	}
	this->Clear();
	this->Values = new_values;
	this->Height = h;
	this->Width = w;
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
	TMatrix *tmp_matrix = new TMatrix();

	try {
		size_t h, w;
		if (!(fin >> h)) {
			throw 1;
		}
		if (!(fin >> w)) {
			throw 1;
		}
		tmp_matrix->Init(NULL, h, w);
		for (size_t i = 0; i < h; i++) {
			for (size_t j = 0; j < w; j++) {
				TNum tmp;
				if (!(fin >> tmp)) {
					throw 2;
				}
				tmp_matrix->SetValue(tmp, i, j);
			}
		}
		//throw 0;

	} catch (int a) {
		//if (a != 0) {
		delete tmp_matrix;
		cout << "Ошибка чтения из файла" << endl;
		return false;
		//}
	}
	//fin.close();
	this->Init(tmp_matrix, tmp_matrix->GetHeight(), tmp_matrix->GetWidth());
	delete tmp_matrix;
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
void TMatrix::Transpose(void) {
	/*TNum **new_values = new TNum*[this->Width];

	for (size_t i = 0; i < this->Width; i++) {
		new_values[i] = new TNum[this->Height];
		for (size_t j = 0; j < this->Height; j++) {
			new_values[i][j] = this->Values[j][i];
		}
	}

	size_t h = this->Width;
	size_t w = this->Height;
	this->Clear();
	this->Width = w;
	this->Height = h;
	this->Values = new_values;
	this->InitStatus = true;*/
	TMatrix *new_matrix = new TMatrix(this->Width, this->Height);
	for (size_t i = 0; i < this->Width; i++) {
		for (size_t j = 0; j < this->Height; j++) {
			new_matrix->SetValue(this->GetValue(j, i), i, j);
		}
	}
	this->Clear();
	this->Init(new_matrix, new_matrix->GetHeight(), new_matrix->GetWidth());
	delete new_matrix;
}
void TMatrix::SetUnit(size_t side) {
	this->Clear();
	this->Init(NULL, side, side);
	for (size_t i = 0; i < side; i++) {
		this->SetValue(1., i, i);
	}
}

bool TMatrix::IsDiag(void) {
	return this->IsDiag(EPS);
}
bool TMatrix::IsDiag(TNum accuracy) {
	bool res = true;
	for (size_t i = 0; i < this->Height; i++) {
		for (size_t j = 0; j < this->Width; j++) {
			if (i == j) {
				continue;
			}
			//cout << "[" << i << ":" << j << "] " << this->Values[i][j] << endl;
			if (!(abs(this->Values[i][j]) < accuracy)) {
				res = false;
				break;
			}
		}
		if (!res) {
			break;
		}
	}
	return res;
}

void TMatrix::MultiplyNum(TNum num) {
	for (size_t i = 0; i < this->Height; i++) {
		for (size_t j = 0; j < this->Width; j++) {
			this->Values[i][j] *= num;
		}
	}
}
void TMatrix::Summ(TMatrix *matrix) {
	if (this->Height != matrix->GetHeight() || this->Width != matrix->GetWidth()) {
		return;
	}
	for (size_t i = 0; i < this->Height; i++) {
		for (size_t j = 0; j < this->Width; j++) {
			this->Values[i][j] += matrix->GetValue(i, j);
		}
	}
}
void TMatrix::Minus(void) {
	for (size_t i = 0; i < this->Height; i++) {
		for (size_t j = 0; j < this->Width; j++) {
			this->Values[i][j] = -this->Values[i][j];
		}
	}
}

TMatrix SubMatrix(TMatrix matrix, size_t i, size_t j, size_t h, size_t w) {
	if (i + h > matrix.GetHeight() || j + w > matrix.GetWidth()) {
		return EMPTY_MATRIX;
	}
	TMatrix new_matrix(h, w);
	for (size_t x = 0; x < h; x++) {
		for (size_t y = 0; y < w; y++) {
			new_matrix.SetValue(matrix.GetValue(x - i, y - j), x, y);
		}
	}
	return new_matrix;
}

void MatrixComposition(TMatrix *m1, TMatrix *m2, TMatrix *res) {
	if (m1->GetWidth() != m2->GetHeight()) {
		res->Clear();
		return;
	}
	size_t h = m1->GetHeight();
	size_t w = m2->GetWidth();
	//TMatrix res(h, w);
	res->Init(NULL, h, w);
	for (size_t i = 0; i < h; i++) {
		for (size_t j = 0; j < w; j++) {
			TNum res_num = 0.;
			for (size_t r = 0; r < m1->GetWidth(); r++) {
				res_num += m1->GetValue(i, r) * m2->GetValue(r, j);
			}
			res->SetValue(res_num, i, j);
		}
	}
}

