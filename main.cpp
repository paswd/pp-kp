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

Parallel programming in Moscow Aviation Institute
Course project

Variant:
* Basic iteration matrix solve method
* CSR matrix
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <complex>
#include "types.hpp"
#include "matrix.hpp"

using namespace std;

vector <TNum> BasicIterationFunc(TMatrix *matrix, vector <TNum> b);

void BasicIterationSolve(string filename) {
	TMatrix matrix;
	ifstream fin(filename.c_str());
	if (!fin.is_open()) {
        cout << "Ошибка чтения файла" << endl;
        return;
	}
	size_t size = 0;
	vector <TNum> b;
	try {
		bool readres = matrix.ReadFromFile(fin, true);

		if (!readres) {
			throw 1;
		}

		size = matrix.GetHeight();
		b.resize(size);

		for (size_t i = 0; i < size; i++) {
			if (!(fin >> b[i])) {
				throw 1;
			}
		}

	} catch (int a) {
		fin.close();
		cout << "Ошибка чтения из файла" << endl;
		return;
	}
	cout << "Результирующий вектор успешно импортирован:" << endl;
	for (size_t i = 0; i < size; i++) {
		cout << b[i] << " ";
	}
	cout << endl;

	fin.close();
	
	vector <TNum> x = BasicIterationFunc(&matrix, b);
	cout << "Решение:" << endl;
	for (size_t i = 0; i < size; i++) {
		cout << "X" << i + 1 << " = " << x[i] << endl;
	}
}

vector <TNum> BasicIterationFunc(TMatrix *matrix, vector <TNum> b) {
	size_t size = b.size();
	vector <TNum> x(size, 1);
	size_t n;
	vector <TNum> e(size);
	vector <TNum> d(size);
	bool is_set_first = true;

	size_t iter = 0;
	while (true) {
		iter++;
		if (is_set_first) {
			n = 1;
		}
		is_set_first = true;
		vector <TNum> xn(size);
		for (size_t i = 0; i < size; i++) {
			e[i] = 0.;
			for (size_t j = 0; j < size; j++) {
				e[i] += matrix->GetValue(i, j) * x[j];
			}
			e[i] -= b[i];
			d[i] = e[i] / matrix->GetValue(i, i);
			xn[i] = x[i] - d[i];

		}
		n++;
		if (n <= size) {
			is_set_first = false;
			continue;
		}
		for (size_t i = 0; i < size; i++) {
			x[i] = xn[i];
		}
		TNum max_d = 0;
		bool max_empty = true;
		for (size_t i = 0; i < size; i++) {
			TNum dn = abs(d[i]);
			if (dn > max_d || max_empty) {
				max_d = dn;
				max_empty = false;
			}
		}
		if (max_d < EPS) {
			break;
		}
	}
	cout << "Число итераций: " << iter << endl;
	return x;
}

int main(int argc, char** argv) {
	//MPI_Init(&argc, &argv);
	//int machineId, machinesCnt;
	//MPI_Comm_size(MPI_COMM_WORLD, &machinesCnt);
	//MPI_Comm_rank(MPI_COMM_WORLD, &machineId);

	string filename = "in.txt";
	BasicIterationSolve(filename);

	//MPI_Finalize();

	return 0;
}
