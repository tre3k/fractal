#include <iostream>

const unsigned int space_size = 1024;
const unsigned int square_size = 512;

int main(int argc, char **argv) {
    std::cout << space_size << std::endl << space_size << std::endl;

    unsigned int x0, x1, y0, y1;
    x0 = (space_size-square_size)/2;
    x1 = square_size + x0;
    y0 = x0;
    y1 = x1;

    short int value {0};

    for(unsigned int i = 0; i < space_size; i++) {
        for(unsigned int j = 0; j < space_size; j++) {
            value = 0;
            if(i > x0 && i < x1 && j > y0 && j < y1) value = 1;
            std::cout << value << std::endl;
        }
    }




    return 0;
}
