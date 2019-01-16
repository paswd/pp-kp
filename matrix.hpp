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

	std::vector <TNum> Data;
	std::vector <size_t> Cols;
	std::vector <size_t> NewRows;

	bool ReadFromFile(std::ifstream &fin, size_t begin, size_t origSize, bool show_imported_matrix);

public:
	TMatrix(std::ifstream &fin, size_t begin, size_t origSize, size_t height, size_t width);
	~TMatrix(void);
	void Clear(void);
	void Print(void);
	void PrintDirect(void);
	TNum GetValue(size_t i, size_t j);
	size_t GetWidth(void);
	size_t GetHeight(void);
	bool IsNull(void);
};

#endif
