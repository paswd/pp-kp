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
#include "mpi.h"
#include "types.hpp"
#include "matrix.hpp"

using namespace std;

class BasicIterationCluster {
private:
	size_t MachinesCnt;
	size_t MachineId;
	size_t ActiveMachinesCnt;
	size_t LayersOnMachine;

	size_t Height;
	size_t Width;

	TNum *ValBuffer;
	int *ValBufferCounts;
	int *ValBufferDispls;

	TNum *LayerBuffer;

	size_t getLayersOnMachine(size_t machineId) {
		//cout << ActiveMachinesCnt << endl;
		//cout << "ActiveMach: " << ActiveMachinesCnt << endl;
		if (machineId >= ActiveMachinesCnt) {
			return 0.;
		}
		size_t res = Height / ActiveMachinesCnt;
		if (machineId < Height % ActiveMachinesCnt) {
			res++;
		}
		return res;
	}

	void setMainClusterParams(size_t machinesCnt, size_t machineId) {
		MachinesCnt = machinesCnt;
		MachineId = machineId;
	}

	vector <TNum> Func(TMatrix *matrix, vector <TNum> b) {
		//cout << "ActiveMach: " << ActiveMachinesCnt << endl;
		//size_t size = b.size();
		vector <TNum> x(Height, 1);
		size_t n = 0;
		vector <TNum> e(LayersOnMachine);
		vector <TNum> d(LayersOnMachine);
		bool is_set_first = true;

		size_t iter = 0;
		//size_t begin = MachineId * LayersOnMachine;
		size_t begin = 0;
		cout << MachineId << "::" << getLayersOnMachine(MachineId) << endl;
		for (size_t i = 0; i < MachineId; i++) {
			begin += getLayersOnMachine(i);
		}
		if (MachineId == 1) {
			cout << "begin = " << begin << endl;
		}
		//cout << "Curr = " << matrix->GetValue(1, 1) << endl;
		//matrix->PrintBin();
		while (true) {
			iter++;
			if (is_set_first) {
				n = 1;
			}
			is_set_first = true;
			vector <TNum> xn(LayersOnMachine);
			for (size_t i = 0; i < LayersOnMachine; i++) {
				e[i] = 0.;
				for (size_t j = 0; j < Width; j++) {
					e[i] += matrix->GetValue(i, j) * x[j];
					/*if (MachineId == 0 && i < 2) {
						cout << "* i, j = " << i << " " << j << endl;
						cout << "* e[i] = " << e[i] << endl;
						cout << "* M[i][j] = " << matrix->GetValue(i, j) << endl;
						cout << "* x[j] = " << x[j] << endl;
					}*/

				}
				e[i] -= b[i];
				d[i] = e[i] / matrix->GetValue(i, i + begin);
				xn[i] = x[i + begin] - d[i];
				if (MachineId == 1) {
					cout << "~~~~~" << endl;
					matrix->Print();
					cout << "~~~~~" << endl;
					cout << "i = " << i << endl;
					cout << "i + begin = " << i + begin << endl;
					cout << "M[i][i + begin] = " << matrix->GetValue(i, i + begin) << endl;
					cout << "e[i] = " << e[i] << endl;
					cout << "d[i] = " << d[i] << endl;
					cout << "xn[i] = " << xn[i] << endl;
					cout << "x[i + begin] = " << x[i + begin] << endl;
				}

			}
			n++;
			if (n <= LayersOnMachine) {
				is_set_first = false;
				continue;
			}/*
			Это должно быть заменено
			for (size_t i = 0; i < LayersOnMachine; i++) {
				x[i] = xn[i];
			}
			*/
			//TNum * = new TNum[d.size()];
			for (size_t i = 0; i < LayersOnMachine; i++) {
				LayerBuffer[i] = xn[i];
			}
			/*if (MachineId == 1) {
				for (size_t i = 0; i < LayersOnMachine; i++) {
					cout << xn[i] << " ";
				}
				cout << endl;
			}*/
			MPI_Allgatherv(LayerBuffer, LayersOnMachine, MPI_DOUBLE, ValBuffer, ValBufferCounts, ValBufferDispls,
				MPI_DOUBLE, MPI_COMM_WORLD);
			for (size_t i = 0; i < Height; i++) {
				x[i] = ValBuffer[i];
			}
			if (MachineId == 0) {
				for (size_t i = 0; i < Height; i++) {
					cout << x[i] << " ";
				}
				cout << endl;
			}
			
			//cout << "dc" << endl;
			/*if (MachineId == 0) {
				cout << "TEST: ";
			}*/
			for (size_t i = 0; i < LayersOnMachine; i++) {
				LayerBuffer[i] = d[i];
				/*if (MachineId == 0) {
					cout << d[i] << " ";
				}*/
			}
			/*if (MachineId == 0) {
				cout << endl;
			}*/
			/*cout << endl;
			cout << "cnts" << endl;
			//Allgather
			for (size_t i = 0; i < MachinesCnt; i++) {
				cout << ValBufferCounts[i] << " ";
			}
			cout << endl;
			cout << "dspls" << endl;
			for (size_t i = 0; i < MachinesCnt; i++) {
				cout << ValBufferDispls[i] << " ";
			}
			cout << endl;*/
			MPI_Allgatherv(LayerBuffer, LayersOnMachine, MPI_DOUBLE, ValBuffer, ValBufferCounts, ValBufferDispls,
				MPI_DOUBLE, MPI_COMM_WORLD);
			for (size_t i = 0; i < Height; i++) {
				//cout << ValBuffer[i] << " ";
			}
			//cout << endl;
			//delete [] dc;

			TNum max_d = 0;
			bool max_empty = true;

			for (size_t i = 0; i < Height; i++) {
				TNum dn = abs(ValBuffer[i]);
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
	void arrInit() {
		ValBuffer = new TNum[Height];
		ValBufferCounts = new int[MachinesCnt];
		ValBufferDispls = new int[MachinesCnt];

		LayerBuffer = new TNum[LayersOnMachine];
		size_t currentDispl = 0;

		for (size_t i = 0; i < MachinesCnt; i++) {
			//cout << "ActiveMach: " << ActiveMachinesCnt << endl;
			size_t currentLayersCnt = getLayersOnMachine(i);
			//cout << "CurrLayersCnt" << endl;
			//cout << currentLayersCnt << endl;
			ValBufferCounts[i] = currentLayersCnt;
			ValBufferDispls[i] = currentDispl;
			currentDispl += currentLayersCnt;
		}
	}
	void arrClear() {
		//delete [] ValBuffer;
		//delete [] ValBufferCounts;
		//delete [] ValBufferDispls;
	}

public:
	BasicIterationCluster(size_t machinesCnt, size_t machineId) {
		//initDefaultVars();
		//getDataFromFile(filename);
		setMainClusterParams(machinesCnt, machineId);
		//arrInit();
	}
	~BasicIterationCluster(void) {
		arrClear();
	}

	void Solve(string filename) {
		ifstream fin(filename.c_str());
		if (!fin.is_open()) {
		       cout << "Ошибка чтения файла" << endl;
		       return;
		}
		//size_t size = 0;
		vector <TNum> b;
		//size_t height, width;
		fin >> Height >> Width;
		ActiveMachinesCnt = min(Height, MachinesCnt);
		arrInit();
		
		LayersOnMachine = getLayersOnMachine(MachineId);
		size_t begin = 0;
		for (size_t i = 0; i < MachineId; i++) {
			begin += getLayersOnMachine(i);
		}
		//cout << "TEST1" << endl;
		//cout << begin << ":" << LayersOnMachine << endl;
		//cout << Width << ":" << Height << endl;
		TMatrix matrix(fin, begin, Height, LayersOnMachine, Width);
		//cout << "TEST2" << endl;z

		matrix.Print();
		//cout << "TEST3" << endl;

		b.resize(0);
		for (size_t i = 0; i < Height; i++) {
			TNum tmp;
			if (!(fin >> tmp)) {
				throw 1;
			}
			if (i < begin || i >= begin + LayersOnMachine) {
				continue;
			}
			b.push_back(tmp);
		}
		for (size_t i = 0; i < LayersOnMachine; i++) {
			cout << b[i] << " ";
		}
		cout << endl;
		fin.close();
		
		vector <TNum> x = Func(&matrix, b);
		cout << "Решение:" << endl;
		for (size_t i = 0; i < Height; i++) {
			cout << "X" << i + 1 << " = " << x[i] << endl;
		}
	}

	
};

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	int machineId, machinesCnt;
	MPI_Comm_size(MPI_COMM_WORLD, &machinesCnt);
	MPI_Comm_rank(MPI_COMM_WORLD, &machineId);

	string filename = "in.txt";
	BasicIterationCluster cluster(machinesCnt, machineId);
	cluster.Solve(filename);

	cout << "TEST" << endl;
	MPI_Finalize();

	return 0;
}
