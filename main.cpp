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
		vector <TNum> x(Height, 1);
		size_t n = 0;
		vector <TNum> e(LayersOnMachine);
		vector <TNum> d(LayersOnMachine);
		bool is_set_first = true;

		size_t iter = 0;
		size_t begin = 0;
		
		for (size_t i = 0; i < MachineId; i++) {
			begin += getLayersOnMachine(i);
		}

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

				}
				e[i] -= b[i];
				d[i] = e[i] / matrix->GetValue(i, i + begin);
				xn[i] = x[i + begin] - d[i];

			}
			n++;
			if (n <= LayersOnMachine) {
				is_set_first = false;
				continue;
			}

			for (size_t i = 0; i < LayersOnMachine; i++) {
				LayerBuffer[i] = xn[i];
			}

			MPI_Allgatherv(LayerBuffer, LayersOnMachine, MPI_DOUBLE, ValBuffer, ValBufferCounts, ValBufferDispls,
				MPI_DOUBLE, MPI_COMM_WORLD);
			for (size_t i = 0; i < Height; i++) {
				x[i] = ValBuffer[i];
			}

			for (size_t i = 0; i < LayersOnMachine; i++) {
				LayerBuffer[i] = d[i];
			}
			
			MPI_Allgatherv(LayerBuffer, LayersOnMachine, MPI_DOUBLE, ValBuffer, ValBufferCounts, ValBufferDispls,
				MPI_DOUBLE, MPI_COMM_WORLD);

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

		if (MachineId == 0) {
			cout << "=============" << endl << endl;
			cout << "Число итераций:" << endl;
		}
		MPI_Barrier(MPI_COMM_WORLD);
		for (size_t mach = 0; mach < MachinesCnt; mach++) {
			if (mach == MachineId) {
				cout << "Core #" << MachineId << ": " << iter << endl;
			}
			MPI_Barrier(MPI_COMM_WORLD);
		}
		return x;
	}
	void arrInit() {
		ValBuffer = new TNum[Height];
		ValBufferCounts = new int[MachinesCnt];
		ValBufferDispls = new int[MachinesCnt];

		LayerBuffer = new TNum[LayersOnMachine];
		size_t currentDispl = 0;

		for (size_t i = 0; i < MachinesCnt; i++) {
			size_t currentLayersCnt = getLayersOnMachine(i);
			ValBufferCounts[i] = currentLayersCnt;
			ValBufferDispls[i] = currentDispl;
			currentDispl += currentLayersCnt;
		}
	}
	void arrClear() {
		delete [] ValBuffer;
		//delete [] ValBufferCounts;
		//delete [] ValBufferDispls;

		delete [] LayerBuffer;
	}

public:
	BasicIterationCluster(size_t machinesCnt, size_t machineId) {
		setMainClusterParams(machinesCnt, machineId);
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
		vector <TNum> b;
		fin >> Height >> Width;
		ActiveMachinesCnt = min(Height, MachinesCnt);
		arrInit();
		
		LayersOnMachine = getLayersOnMachine(MachineId);
		size_t begin = 0;
		for (size_t i = 0; i < MachineId; i++) {
			begin += getLayersOnMachine(i);
		}

		TMatrix matrix(fin, begin, Height, LayersOnMachine, Width);

		if (MachineId == 0) {
			cout << "Полученные данные:" << endl;
		}
		MPI_Barrier(MPI_COMM_WORLD);
		for (size_t mach = 0; mach < MachinesCnt; mach++) {
			if (mach == MachineId) {
				if (mach == 0) {
					cout << "Матрица:" << endl;
				}
				matrix.Print();
				if (mach == MachinesCnt - 1) {
					cout << "_____________" << endl << endl;
				}
			}
			MPI_Barrier(MPI_COMM_WORLD);
		}

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
		for (size_t mach = 0; mach < MachinesCnt; mach++) {
			if (mach == MachineId) {
				if (mach == 0) {
					cout << "Свободный член:" << endl;
				}
				for (size_t i = 0; i < LayersOnMachine; i++) {
					cout << b[i] << endl;
				}
			}
			MPI_Barrier(MPI_COMM_WORLD);
		}
		fin.close();
		
		vector <TNum> x = Func(&matrix, b);
		if (MachineId == 0) {
			cout << "_____________" << endl << endl;
			cout << "Полученный ответ:" << endl;
			for (size_t i = 0; i < Height; i++) {
				cout << "X[" << i + 1 << "] = " << x[i] << endl;
			}
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

	MPI_Finalize();

	return 0;
}
