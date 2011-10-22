#include <iostream>

void gen(int n)
{
    std::cout << n << std::endl;
    for (size_t i = 0; i < n ; ++i) {
        if (i > 0) {
            std::cout << " ";
        }
        std::cout << rand() % 10;
    }
    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "Usage: <binary> <integer_arg1> <integer_arg2>" << std::endl;
    }
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    gen(n);
    gen(m);
}
