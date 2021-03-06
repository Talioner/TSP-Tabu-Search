#include "stdafx.h"
#include <iostream>
#include <ctime>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;

//Stoepr
class Stopwatch {
private:
	clock_t tstart;
	clock_t tstop;
public:
	double time;
	Stopwatch::Stopwatch() {

	}
	void Stopwatch::start() {
		tstart = clock();
		time = 0;
	}
	void Stopwatch::stop() {
		tstop = clock();
		time = ((double)(tstop - tstart)) / CLOCKS_PER_SEC;
	}
	double Stopwatch::read() {
		return time;
	}
};

struct Swap {
	int newTourCost = 0, i = 0, j = 0, value = 0;
};

//Oblicza koszt zadanej trasy
int cost(int* a, vector<vector<int>> m) {
	int cost = 0;
	for (int i = 0; i < m.size(); i++) cost += m[a[i]][a[i + 1]];
	return cost;
}

void showTour(int* a, int aSize) {
	for (int i = 0; i < aSize; i++) cout << a[i] << " ";
	cout << endl;
}

void copyArray(int* a, int* b, int aSize) {
	for (int i = 0; i < aSize; i++) {
		b[i] = a[i];
	}
}

int randomOrder(vector<vector<int>> m, int* a) {
	for (int i = 0; i < m.size(); i++) {
		swap(a[rand() % (m.size() - 2) + 1], a[rand() % (m.size() - 2) + 1]);
	}
	return 0;
}

int Neighbour(vector<vector<int>> m, int* a) {
	int cost = 0;
	int min = 999999;
	int* tab = new int[m.size()];
	int x = 0;
	int x2 = 0;
	int* deleted = new int[m.size()];
	for (int i = 0; i < m.size(); i++) {
		deleted[i] = -10;
	}
	deleted[0] = 1;

	for (int i = 0; i < m.size() - 1; i++) {
		for (int j = 0; j < m.size(); j++) {
			if (m[x][j] < min && m[x][j] != -1 && deleted[j] == -10) {
				min = m[x][j];
				x2 = j;
			}
		}
		x = x2;
		a[i] = x;
		deleted[x] = 1;
		cost += min;
		min = 999999;
	}
	cost += m[x][0];
	return  cost;
}

//Oblicza najbardziej oplacalna zamiane wierzcholkow
Swap findBestSwap(int* a, vector<vector<int>> m, int currentTourCost, int bestEverCost, int* tabuArray, int tabuSize) {
	bool isTabu = 0;
	int* b = new int[m.size() + 1];
	copyArray(a, b, m.size() + 1);
	int newTourCost = 0;
	int bestSwapValue = 999999;
	Swap bestSwap;
	for (int i = 1; i < m.size(); i++) {
		for (int j = i + 1; j < m.size(); j++) {
			swap(b[i], b[j]);										//zamienia wierzcholki
			//showTour(b, m.size() + 1);
			newTourCost = cost(b, m);								//liczy nowy koszt
			if (newTourCost - currentTourCost < bestSwapValue) {	//zapisuje zamiane jesli jest najlepsza
				//cout << "Sprawdzamy nowa najlepsza zamiane: " << b[j] << " na " << b[i] << endl;
				for (int k = 0; k < tabuSize; k++) {
					if (tabuArray[k] == b[i]) {
						isTabu = 1;
						//cout << "Jest tabu. Sprawdzam czy spelnia aspiracje..." << endl;
						if (newTourCost < bestEverCost) {
							//cout << "Spelnia: " << newTourCost << " < " << bestEverCost << endl;
							bestEverCost = newTourCost;
							bestSwapValue = newTourCost - currentTourCost;
							bestSwap.newTourCost = newTourCost;
							bestSwap.i = i;
							bestSwap.j = j;
							bestSwap.value = newTourCost - currentTourCost;
							break;
						}
						//cout << "Nie spelnia." << endl;
						break;
					}
				}
				if (isTabu == 0) {
					bestSwapValue = newTourCost - currentTourCost;
					//cout << "Nie jest tabu to nowy najlepszy: " << bestSwapValue << endl;
					bestSwap.newTourCost = newTourCost;
					bestSwap.i = i;
					bestSwap.j = j;
					bestSwap.value = bestSwapValue;
					//cout << "NAJLEPSZY:" << endl;
					//showTour(b, m.size() + 1);
				}
			}
			copyArray(a, b, m.size() + 1);
			isTabu = 0;
			//cout << "TO JEST STARTOWY:" << endl;
			//showTour(b, m.size() + 1);
		}
	}
	return bestSwap;
	delete[] b;
}
// zmiana rozmiaru tabu
/*int changeTabuSize(vector<vector<int>> m, int* tabu, int tabuSize, double count) {
	int* b = new int[tabuSize];
	int oldSize = tabuSize;
	copyArray(tabu, b, tabuSize);
	for (int i = 0; i < tabuSize; i++) {
		tabu[i] = -1;
	}
	if (count < 0) {
		tabuSize--;
		copyArray(b, tabu, tabuSize);
	}
	if(count > 0) {
		tabuSize++;
		copyArray(b, tabu, oldSize);
	}
	if (count == 0) {
		tabuSize = m.size() * 0.5;
	}
	cout << "Count: " << count << endl;
	return tabuSize;
}*/

int tabuSearch(vector<vector<int>> m) {

	int* bestTourEver = new int[m.size() + 1];
	int* officialBestTourEver = new int[m.size() + 1];
	int bestTourEverCost = 999999;
	int tabuSize = m.size() * 0.4;
	int* tabuArray = new int[tabuSize];
	int reset = 100;
	//int count = 0;

	//Generuje startowa trase
	int* startingTour = new int[m.size() + 1];
	cout << Neighbour(m, startingTour) << endl;					// nizej zeby startowy byl algorytmem najblizszego sasiada
	for (int i = 0; i < m.size(); i++) startingTour[i] = i;
	startingTour[m.size()] = 0;
	int bestTourCost = cost(startingTour, m);
	bestTourEverCost = bestTourCost;
	showTour(startingTour, m.size() + 1);
	cout << bestTourCost << endl;

	//Wykonuje najlepsza zamiane wierzcholkow
	for (int i = 0; i < 25; i++) {
		Swap bestSwap;
		bestSwap = findBestSwap(startingTour, m, bestTourCost, bestTourEverCost, tabuArray, tabuSize);		//Znajduje najlepsza zamiane
		swap(startingTour[bestSwap.i], startingTour[bestSwap.j]);											//Wykonuje najlepsza zamiane
		tabuArray[i % tabuSize] = startingTour[bestSwap.j];													//Aktualizuje liste tabu
		//cout << "Do tabu dodano: " << tabuArray[i % tabuSize] << endl;
		bestTourCost = bestSwap.newTourCost;
		//showTour(startingTour, m.size() + 1);
		//cout << bestTourCost << endl;
		if (bestTourCost < bestTourEverCost) {
			bestTourEverCost = bestTourCost;
			copyArray(startingTour, bestTourEver, m.size() + 1);
			//cout << bestTourEverCost << endl;
			reset = 100;
			//count--;
		}
		if(bestSwap.value > 0) {
			reset--;
			//count++;
		}

		/*if (reset == 0) {																// RESET
			randomOrder(m, startingTour);
			//cout << "RESET: " << cost(startingTour, m) << endl;
			reset = 100;
			//count = 0;
			//tabuSize = changeTabuSize(m, tabuArray, tabuSize, count);
			for (int z = 0; z < tabuSize; z++) {
				tabuArray[z] = -1;
			}
		}*/

		//cout << "I - >>>>>>>>>>>>>>>>>>>>>>>>> " << i << endl;
		/*if (count == 2) {
			tabuSize = changeTabuSize(m, tabuArray, tabuSize, count);
			count = 0;
		}
		if (count == -2) {
			tabuSize = changeTabuSize(m, tabuArray, tabuSize, count);
			count = 0;
		}
		cout << "Tabu size: " << tabuSize << endl;
		*/
	}

	cout << bestTourEverCost << endl;
	showTour(bestTourEver, m.size() + 1);


	return 0;
}


int main()
{

	vector<vector<int>> start;
	Stopwatch clock;
	char salesmanSelection = '111';
	while (salesmanSelection != '0') {
		cout << "1 - wygeneruj losowe dane" << endl;
		cout << "2 - wyswietl dane" << endl;
		cout << "3 - tabu search" << endl;
		cout << "4 - wczytaj dane z tsp_data.txt" << endl;
		cout << "Aby zakonczyc - 0" << endl;
		cout << "Wybierz: ";
		cin >> salesmanSelection;
		cout << endl;

		switch (salesmanSelection)
		{
		case '1':
		{

			/*while(!start.matrixV.empty()) start.matrixV.pop_back();
			while (!start.fullRoute.empty()) start.fullRoute.pop_back();
			start.lb = 0;*/
			int dimension;
			cout << "Liczba miast: ";
			cin >> dimension;
			cout << endl;

			srand(time(NULL));

			for (int i = 0; i < dimension; i++) {
				start.push_back(vector<int>());
				for (int j = 0; j < dimension; j++) {
					if (i == j) start[i].push_back(-1);
					else start[i].push_back((rand() % (99 - 10) + 10));
				}
			}
			break;

		}
		break;
		case '2':
		{
			for (int i = 0; i < start.size(); i++) {
				for (int j = 0; j < start[i].size(); j++) {
					cout << start[i][j] << "   ";
				}
				cout << endl;
			}
			break;
		}
		break;
		case '3':
		{
			clock.start();
			tabuSearch(start);
			clock.stop();
			cout << "Czas wykonywania: " << clock.read() << endl;
			break;
		}
		break;
		case '4':
		{
			int dimension;
			int buf = 0;
			fstream file;

			/*file.open("ft53.atsp");
			file >> dimension;
			for (int i = 0; i < dimension; i++) {
			start.push_back(vector<int>());
			for (int j = 0; j < dimension; j++) {
			file >> buf;
			start[i].push_back(buf);
			}
			}*/

			file.open("gr17.tsp");
			file >> dimension;
			for (int i = 0; i < dimension; i++) {
				start.push_back(vector<int>());
				for (int j = 0; j < dimension; j++) {
					start[i].push_back(0);
				}
			}

			for (int i = 0; i < dimension; i++) {
				for (int j = 0; j <= i; j++) {
					file >> buf;
					if (j == i) start[i][j] = -1;
					else {
						start[i][j] = buf;
						start[j][i] = start[i][j];
					}
				}
			}

			break;
		}
		}
	}


	getchar();
	return 0;
}

