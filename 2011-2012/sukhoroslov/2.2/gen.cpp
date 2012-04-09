#include <iostream>
#include <cstdio>

int main(int argc, char** argv)
{
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int nit = atoi(argv[3]);
    double part = atof(argv[4]);
    printf("%d %d %d\n", n, m, nit);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            char val = '0';
            if (rand() % 1000 < 1000 * part) {
                val = '1';
            }
            printf("%c", val);
        }
        printf("\n");
    }
    return 0;
}
