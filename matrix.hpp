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

#ifndef _MATRIX_HPP_
#define _MATRIX_HPP_

#include "types.hpp"
#include <iostream>
#include <vector>

class TMatrix {
private:
	bool InitStatus;
	size_t Height;
	size_t Width;

	TNum **Values;

public:
	TMatrix(void);
	TMatrix(TMatrix *sample);
	TMatrix(size_t h, size_t w);
	TMatrix(std::vector <TNum> vect);
	~TMatrix(void);
	void Init(TMatrix *sample, size_t h, size_t w);
	void Clear(void);
	void Get(void);
	void Print(void);
	TNum GetValue(size_t i, size_t j);
	void SetValue(TNum value, size_t i, size_t j);
	size_t GetWidth(void);
	size_t GetHeight(void);
	void Resize(size_t h, size_t w);
	bool IsNull(void);
	bool ReadFromFile(std::ifstream &fin, bool show_imported_matrix);
	void Transpose(void);
	void SetUnit(size_t side);
	bool IsDiag(void);
	bool IsDiag(TNum accuracy);
	void MultiplyNum(TNum num);
	void Summ(TMatrix *matrix);
	void Minus(void);
};

TMatrix SubMatrix(TMatrix matrix, size_t i, size_t j, size_t h, size_t w);
void MatrixComposition(TMatrix *m1, TMatrix *m2, TMatrix *res);

#endif
