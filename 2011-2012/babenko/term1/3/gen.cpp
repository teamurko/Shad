#include <iostream>

int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);
    if (argc != 4) {
        std::cerr << "Usage: <binary> <num triangles> <max len> <seed>" << std::endl;
        return 1;
    }
    int numTriangles = atoi(argv[1]);
    std::cout << numTriangles << std::endl;
    srand(atoi(argv[3]));
    int p = atoi(argv[2]);
    for(size_t i = 0; i < numTriangles; ++i) {
        for(size_t j = 0; j < 3; ++j) {
            std::cout << rand() % p + 1;
            if (j < 2) std::cout << " ";
        }
        std::cout << std::endl;
    }
}
