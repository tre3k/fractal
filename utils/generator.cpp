#include <iostream>
#include "hilbert.hpp"

std::array<unsigned int, 2>
IndexToPosition2D8BV2(unsigned long int idx)
{
    std::array<unsigned int, 2> tmp;

    tmp[0] = (unsigned int)((idx >> 32 & 0xffffff));
    tmp[1] = (unsigned int)(idx & 0xffffff);

//    std::cout << tmp[0] << " : " << tmp[1] << std::endl;

    return hilbert::v2::IndexToPosition(tmp);
}

struct sPos {
	int x, y;
} typedef Pos;

void paintOrthoLine(int **data, Pos from, Pos to) {
	bool isXVariable = false;
	bool isYVariable = false;

	if(from.x != to.x) isXVariable = true;
	if(from.y != to.y) isYVariable = true;

	int max, min;

	if(isXVariable) {
		if(from.x > to.x) {
			max = from.x;
			min = to.x;
		} else {
			max = to.x;
			min = from.x;
		}
		for(int i = min; i <= max; i++) data[from.y][i] = 1;

	}

	if(isYVariable) {
		if(from.y > to.y) {
			max = from.y;
			min = to.y;
		} else {
			max = to.y;
			min = from.y;
		}
		for(int i = min; i <= max; i++) data[i][from.x] = 1;
	}
}

int main(int argc, char *argv[]){


	const int size = 1024;
	const int hilbert_count = 65536;
	const int offset = 256;

	int **data = new int * [size];
	for(int i = 0; i < size; i++){
		data[i] = new int [size];
		for(int j = 0; j < size; j++) data[i][j] = 0;
	}

	Pos from, to;
	for(int i = 0; i < hilbert_count; i++) {
		auto ret = IndexToPosition2D8BV2(i);

		to.x = ret[0] * 2 + offset;
		to.y = ret[1] * 2 + offset;

		if(i > 0) paintOrthoLine(data, from, to);

		from = to;
//		std::cout << i << " : " << to.x << ", " << to.y << std::endl;
	}

	// OUT
	std::cout << size << std::endl << size << std::endl;
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			std::cout << data[i][j] << std::endl;
		}
	}

	return 0;
}
